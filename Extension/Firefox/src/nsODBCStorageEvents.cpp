/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif

#include "nsThreadUtils.h"
#include "nsAutoPtr.h"
#include "nsStringAPI.h"
#include "nsCOMArray.h"

#include "sql.h"

#include "nsIODBCStorageStatementCallback.h"
#include "nsIODBCStoragePendingStatement.h"
#include "nsIODBCStorageStatement.h"
#include "nsODBCUtils.h"
#include "nsODBCStorageHelper.h"
#include "nsODBCStorageResultSet.h"
#include "nsODBCStorageRow.h"
#include "nsODBCStorageConnection.h"
#include "nsODBCStorageError.h"
#include "nsODBCStorageEvents.h"

/**
 * The following constants help batch rows into result sets.
 * MAX_MILLISECONDS_BETWEEN_RESULTS was chosen because any user-based task that
 * takes less than 200 milliseconds is considered to feel instantaneous to end
 * users.  MAX_ROWS_PER_RESULT was arbitrarily chosen to reduce the number of
 * dispatches to calling thread, while also providing reasonably-sized sets of
 * data for consumers.  Both of these constants are used because we assume that
 * consumers are trying to avoid blocking their execution thread for long
 * periods of time, and dispatching many small events to the calling thread will
 * end up blocking it.
 */
#define MAX_MILLISECONDS_BETWEEN_RESULTS 75
#define MAX_ROWS_PER_RESULT 15

////////////////////////////////////////////////////////////////////////////////
//// Asynchronous Statement Execution

/**
 * Enum used to describe the state of execution.
 */
enum ExecutionState {
    PENDING = -1
  , COMPLETED = nsIODBCStorageStatementCallback::REASON_FINISHED
  , CANCELED = nsIODBCStorageStatementCallback::REASON_CANCELED
  , ERROR = nsIODBCStorageStatementCallback::REASON_ERROR
};

/**
 * Interface used to check if an event should run.
 */
class iEventStatus : public nsISupports
{
public:
  virtual PRBool runEvent() = 0;
};

/**
 * Notifies a callback with a result set.
 */
class CallbackResultNotifier : public nsIRunnable
{
public:
  NS_DECL_ISUPPORTS

  CallbackResultNotifier(nsIODBCStorageStatementCallback *aCallback,
                         nsIODBCStorageResultSet *aResults,
                         iEventStatus *aEventStatus) :
      mCallback(aCallback)
    , mResults(aResults)
    , mEventStatus(aEventStatus)
  {
  }

  NS_IMETHOD Run()
  {
    NS_ASSERTION(mCallback, "Trying to notify about results without a callback!");

    if (mEventStatus->runEvent() && mCallback)
      (void)mCallback->HandleResult(mResults);

    return NS_OK;
  }

private:
  CallbackResultNotifier() { }

  nsIODBCStorageStatementCallback *mCallback;
  nsCOMPtr<nsIODBCStorageResultSet> mResults;
  nsRefPtr<iEventStatus> mEventStatus;
};
NS_IMPL_THREADSAFE_ISUPPORTS1(
  CallbackResultNotifier,
  nsIRunnable
)

/**
 * Notifies the calling thread that an error has occurred.
 */
class ErrorNotifier : public nsIRunnable
{
public:
  NS_DECL_ISUPPORTS

  ErrorNotifier(nsIODBCStorageStatementCallback *aCallback,
                nsIODBCStorageError *aErrorObj,
                iEventStatus *aEventStatus) :
      mCallback(aCallback)
    , mErrorObj(aErrorObj)
    , mEventStatus(aEventStatus)
  {
  }

  NS_IMETHOD Run()
  {
    if (mEventStatus->runEvent() && mCallback)
      (void)mCallback->HandleError(mErrorObj);

    return NS_OK;
  }

private:
  ErrorNotifier() { }

  nsIODBCStorageStatementCallback *mCallback;
  nsCOMPtr<nsIODBCStorageError> mErrorObj;
  nsRefPtr<iEventStatus> mEventStatus;
};
NS_IMPL_THREADSAFE_ISUPPORTS1(
  ErrorNotifier,
  nsIRunnable
)

/**
 * Notifies the calling thread that the statement has finished executing.
 */
class CompletionNotifier : public nsIRunnable
{
public:
  NS_DECL_ISUPPORTS

  /**
   * This takes ownership of the callback.  It is released on the thread this is
   * dispatched to (which should always be the calling thread).
   */
  CompletionNotifier(nsIODBCStorageStatementCallback *aCallback,
                     ExecutionState aReason) :
      mCallback(aCallback)
    , mReason(aReason)
  {
  }

  NS_IMETHOD Run()
  {
    if (mCallback)
      (void)mCallback->HandleCompletion(mReason);
    NS_RELEASE(mCallback);

    return NS_OK;
  }

private:
  CompletionNotifier() { }

  nsIODBCStorageStatementCallback *mCallback;
  ExecutionState mReason;
};
NS_IMPL_THREADSAFE_ISUPPORTS1(
  CompletionNotifier,
  nsIRunnable
)

/**
 * Executes a statement asynchronously in the background.
 */
class AsyncExecute : public nsIRunnable
                   , public nsIODBCStoragePendingStatement
                   , public iEventStatus
{
public:
  NS_DECL_ISUPPORTS

  /**
   * This takes ownership of both the statement and the callback.
   */
  AsyncExecute(nsCOMArray<nsIODBCStorageStatement> &aStatements,
               nsIODBCStorageConnection *aConnection,
               nsIODBCStorageStatementCallback *aCallback) :
      mConnection(aConnection)
    , mTransactionManager(nsnull)
    , mCallback(aCallback)
    , mCallingThread(do_GetCurrentThread())
    , mMaxIntervalWait(PR_MillisecondsToInterval(MAX_MILLISECONDS_BETWEEN_RESULTS))
    , mIntervalStart(PR_IntervalNow())
    , mState(PENDING)
    , mCancelRequested(PR_FALSE)
  {
    (void)mStatements.AppendObjects(aStatements);
    NS_ASSERTION(mStatements.Length(), "We weren't given any statements!");
  }

  nsresult Initialize()
  {
    NS_IF_ADDREF(mCallback);
    return NS_OK;
  }

  NS_IMETHOD Run()
  {
    // do not run if we have been canceled
    {
      nsODBCAutoLock mutex(mLock);
      if (mCancelRequested) {
        mState = CANCELED;
        mutex.unlock();
        return NotifyComplete();
      }
    }

    // If there is more than one statement, run it in a transaction.  We assume
    // that we have been given write statements since getting a batch of read
    // statements doesn't make a whole lot of sense.
    if (mStatements.Count() > 1) {
      // We don't error if this failed because it's not terrible if it does.
      mTransactionManager = new nsODBCStorageTransaction(mConnection, PR_FALSE,
                                                      nsIODBCStorageConnection::TRANSACTION_READ_COMMITTED);
    }

    // Execute each statement, giving the callback results if it returns any.
    for (PRInt32 i = 0; i < mStatements.Count(); i++) {
        PRBool finished = (i == (mStatements.Count() - 1));
      if (!ExecuteAndProcessStatement(mStatements[i], finished))
        break;
    }

    // If we still have results that we haven't notified about, take care of
    // them now.
    if (mResultSet)
      (void)NotifyResults();

    // Notify about completion
    return NotifyComplete();
  }

  NS_IMETHOD Cancel(PRBool *_successful)
  {
#ifdef DEBUG
    PRBool onCallingThread = PR_FALSE;
    (void)mCallingThread->IsOnCurrentThread(&onCallingThread);
    NS_ASSERTION(onCallingThread, "Not canceling from the calling thread!");
#endif

    // If we have already canceled, we have an error, but always indicate that
    // we are trying to cancel.
    NS_ENSURE_FALSE(mCancelRequested, NS_ERROR_UNEXPECTED);

    {
      nsODBCAutoLock mutex(mLock);

      // We need to indicate that we want to try and cancel now.
      mCancelRequested = PR_TRUE;

      // Establish if we can cancel
      *_successful = (mState == PENDING);
    }

    // Note, it is possible for us to return false here, and end up canceling
    // events that have been dispatched to the calling thread.  This is OK,
    // however, because only read statements (such as SELECT) are going to be
    // posting events to the calling thread that actually check if they should
    // run or not.

    return NS_OK;
  }

  /**
   * This is part of iEventStatus.  It indicates if an event should be ran based
   * on if we are trying to cancel or not.
   */
  PRBool runEvent()
  {
#ifdef DEBUG
    PRBool onCallingThread = PR_FALSE;
    (void)mCallingThread->IsOnCurrentThread(&onCallingThread);
    NS_ASSERTION(onCallingThread, "runEvent not running on the calling thread!");
#endif

    // We do not need to acquire mLock here because it can only ever be written
    // to on the calling thread, and the only thread that can call us is the
    // calling thread, so we know that our access is serialized.
    return !mCancelRequested;
  }

private:
  AsyncExecute() : mMaxIntervalWait(0) { }

  ~AsyncExecute()
  {
  }

  /**
   * Executes a given statement until completion, an error occurs, or we are
   * canceled.  If aFinished is true, we know that we are the last statement,
   * and should set mState accordingly.
   *
   * @pre mLock is not held
   *
   * @param aStatement
   *        The statement to execute and then process.
   * @param aFinished
   *        Indicates if this is the last statement or not.  If it is, we have
   *        to set the proper state.
   * @returns true if we should continue to process statements, false otherwise.
   */
  PRBool ExecuteAndProcessStatement(nsIODBCStorageStatement *aStatement, PRBool aFinished)
  {
    // We need to hold a lock for statement execution so we can properly
    // reflect state in case we are canceled.  We unlock in a few areas in
    // order to allow for cancelation to occur.
    nsODBCAutoLock mutex(mLock);

    nsresult rv = NS_OK;

    while (PR_TRUE) {
      PRBool ret;
      rv = aStatement->ExecuteStep(&ret);

      if (rv != NS_OK) {
        // Set error state
        mState = ERROR;

        // Drop our mutex - NotifyError doesn't want it held
        mutex.unlock();

        // Notify
        PRInt32 erCode;
        nsString erMessage;
        nsString erState;
         
        aStatement->GetLastErrorCode(&erCode);
        aStatement->GetLastErrorString(erMessage);
        aStatement->GetLastErrorState(erState);

        (void)NotifyError(erCode, erState, erMessage);

        // And stop processing statements
        return PR_FALSE;
      }
      // Break out if we have no more results
      if (ret == PR_FALSE)
        break;

      // If we do not have a callback, there's no point in executing this
      // statement anymore, but we wish to continue to execute statements.  We
      // also need to update our state if we are finished, so break out of the
      // while loop.
      if (!mCallback)
        break;

      // If we have been canceled, there is no point in going on...
      if (mCancelRequested) {
        mState = CANCELED;
        return PR_FALSE;
      }

      // Build our results and notify if it's time.
      rv = BuildAndNotifyResults(aStatement);
      if (NS_FAILED(rv))
        break;
    }

    // If we have an error that we have not already notified about, set our
    // state accordingly, and notify.
    if (NS_FAILED(rv)) {
      mState = ERROR;

      // Drop our mutex - NotifyError doesn't want it held
      mutex.unlock();

      // Notify, and stop processing statements.
      (void)NotifyError(nsIODBCStorageError::ERROR, NS_LITERAL_STRING("HY000"), NS_LITERAL_STRING(""));
      return PR_FALSE;
    }

    // If we are done, we need to set our state accordingly while we still
    // hold our lock.  We would have already returned if we were canceled or had
    // an error at this point.
    if (aFinished)
      mState = COMPLETED;

    return PR_TRUE;
  }

  /**
   * Builds a result set up with a row from a given statement.  If we meet the
   * right criteria, go ahead and notify about this results too.
   *
   * @pre mLock is held
   *
   * @param aStatement
   *        The statement to get the row data from.
   */
  nsresult BuildAndNotifyResults(nsIODBCStorageStatement *aStatement)
  {
    NS_ASSERTION(mCallback, "Trying to dispatch results without a callback!");

    // At this point, it is safe to not hold the lock and allow for cancelation.
    // We may add an event to the calling thread, but that thread will not end
    // up running when it checks back with us to see if it should run.
    nsODBCAutoUnlock cancelationScope(mLock);

    // Build result object if we need it.
    if (!mResultSet)
      mResultSet = new nsODBCStorageResultSet();
    NS_ENSURE_TRUE(mResultSet, NS_ERROR_OUT_OF_MEMORY);

    nsRefPtr<nsODBCStorageRow> row(new nsODBCStorageRow());
    NS_ENSURE_TRUE(row, NS_ERROR_OUT_OF_MEMORY);

    nsresult rv = row->Initialize(aStatement);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = mResultSet->add(row);
    NS_ENSURE_SUCCESS(rv, rv);

    // If we have hit our maximum number of allowed results, or if we have hit
    // the maximum amount of time we want to wait for results, notify the
    // calling thread about it.
    PRIntervalTime now = PR_IntervalNow();
    PRIntervalTime delta = now - mIntervalStart;
    if (mResultSet->rows() >= MAX_ROWS_PER_RESULT || delta > mMaxIntervalWait) {
      // Notify the caller
      rv = NotifyResults();
      if (NS_FAILED(rv))
        return NS_OK; // we'll try again with the next result

      // Reset our start time
      mIntervalStart = now;
    }

    return NS_OK;
  }

  /**
   * Notifies callback about completion, and does any necessary cleanup.
   *
   * @pre mLock is not held
   */
  nsresult NotifyComplete()
  {
    NS_ASSERTION(mState != PENDING,
                 "Still in a pending state when calling Complete!");

    // Finalize our statements before we try to commit or rollback.  If we are
    // canceling and have statements that think they have pending work, the
    // rollback will fail.
    mStatements.Clear();

    // Handle our transaction, if we have one
    if (mTransactionManager) {
      if (mState == COMPLETED) {
        nsresult rv = mTransactionManager->Commit();
        if (NS_FAILED(rv)) {
          mState = ERROR;
          (void)NotifyError(nsIODBCStorageError::ERROR, NS_LITERAL_STRING("HY000"),
                            NS_LITERAL_STRING("Transaction failed to commit"));
        }
      }
      else {
        (void)mTransactionManager->Rollback();
      }
      delete mTransactionManager;
      mTransactionManager = nsnull;
    }

    // Notify about completion iff we have a callback.
    if (mCallback) {
      nsRefPtr<CompletionNotifier> completionEvent =
        new CompletionNotifier(mCallback, mState);
      NS_ENSURE_TRUE(completionEvent, NS_ERROR_OUT_OF_MEMORY);

      // We no longer own mCallback (the CompletionNotifier takes ownership).
      mCallback = nsnull;

      (void)mCallingThread->Dispatch(completionEvent, NS_DISPATCH_NORMAL);
    }

    return NS_OK;
  }

  /**
   * Notifies callback about an error.
   *
   * @pre mLock is not held
   *
   * @param aErrorCode
   *        The error code defined in nsIODBCStorageError for the error.
   * @param aMessage
   *        The error string, if any.
   */
  nsresult NotifyError(PRInt32 aErrorCode, const nsAString &aState, const nsAString &aMessage)
  {
    if (!mCallback)
      return NS_OK;

    nsCOMPtr<nsIODBCStorageError> errorObj =
      new nsODBCStorageError(aErrorCode, aState, aMessage);
    NS_ENSURE_TRUE(errorObj, NS_ERROR_OUT_OF_MEMORY);

    nsRefPtr<ErrorNotifier> notifier =
      new ErrorNotifier(mCallback, errorObj, this);
    NS_ENSURE_TRUE(notifier, NS_ERROR_OUT_OF_MEMORY);

    return mCallingThread->Dispatch(notifier, NS_DISPATCH_NORMAL);
  }

  /**
   * Notifies the callback about a result set.
   *
   * @pre mLock is not held
   */
  nsresult NotifyResults()
  {
    NS_ASSERTION(mCallback, "NotifyResults called without a callback!");

    nsRefPtr<CallbackResultNotifier> notifier =
      new CallbackResultNotifier(mCallback, mResultSet, this);
    NS_ENSURE_TRUE(notifier, NS_ERROR_OUT_OF_MEMORY);

    nsresult rv = mCallingThread->Dispatch(notifier, NS_DISPATCH_NORMAL);
    if (NS_SUCCEEDED(rv))
      mResultSet = nsnull; // we no longer own it on success
    return rv;
  };

  nsCOMArray<nsIODBCStorageStatement > mStatements;
  nsIODBCStorageConnection *mConnection;
  nsODBCStorageTransaction *mTransactionManager;
  nsIODBCStorageStatementCallback *mCallback;
  nsCOMPtr<nsIThread> mCallingThread;
  nsRefPtr<nsODBCStorageResultSet> mResultSet;

  /**
   * The maximum amount of time we want to wait between results.  Defined by
   * MAX_MILLISECONDS_BETWEEN_RESULTS and set at construction.
   */
  const PRIntervalTime mMaxIntervalWait;

  /**
   * The start time since our last set of results.
   */
  PRIntervalTime mIntervalStart;

  /**
   * Indicates the state the object is currently in.
   */
  ExecutionState mState;

  /**
   * Indicates if we should try to cancel at a cancelation point or not.
   */
  PRBool mCancelRequested;

  /**
   * This is the lock that protects our state from changing.  This includes the
   * following variables:
   *   -mState
   *   -mCancelRequested is only set on the calling thread while the lock is
   *    held.  It is always read from within the lock on the background thread,
   *    but not on the calling thread (see runEvent for why).
   */
  SpinLock mLock;
};
NS_IMPL_THREADSAFE_ISUPPORTS2(
  AsyncExecute,
  nsIRunnable,
  nsIODBCStoragePendingStatement
)

nsresult
NS_executeAsync(nsCOMArray<nsIODBCStorageStatement> &aStatements,
                nsODBCStorageConnection *aConnection,
                nsIODBCStorageStatementCallback *aCallback,
                nsIODBCStoragePendingStatement **_stmt)
{
  // Create our event to run in the background
  nsRefPtr<AsyncExecute> event(new AsyncExecute(aStatements, aConnection, aCallback));
  NS_ENSURE_TRUE(event, NS_ERROR_OUT_OF_MEMORY);

  nsresult rv = event->Initialize();
  NS_ENSURE_SUCCESS(rv, rv);

  // Dispatch it to the background
  nsCOMPtr<nsIEventTarget> target(aConnection->getAsyncExecutionTarget());
  NS_ENSURE_TRUE(target, NS_ERROR_NOT_AVAILABLE);
  rv = target->Dispatch(event, NS_DISPATCH_NORMAL);
  NS_ENSURE_SUCCESS(rv, rv);

  // Return it as the pending statement object
  NS_ADDREF(*_stmt = event);
  return NS_OK;
}
