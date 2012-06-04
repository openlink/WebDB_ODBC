/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef _NSODBCSTORAGEHELPER_H_
#define _NSODBCSTORAGEHELPER_H_

#include "nsCOMArray.h"
#include "nsIODBCStorageConnection.h"
#include "nsIODBCStorageStatement.h"


/**
 * This class wraps a transaction inside a given C++ scope, guaranteeing that
 * the transaction will be completed even if you have an exception or
 * return early.
 *
 * aCommitOnComplete controls whether the transaction is committed or rolled
 * back when it goes out of scope. A common use is to create an instance with
 * commitOnComplete = FALSE (rollback), then call Commit on this object manually
 * when your function completes successfully.
 *
 * Note that nested transactions are not supported by sqlite, so if a transaction
 * is already in progress, this object does nothing. Note that in this case,
 * you may not get the transaction type you ask for, and you won't be able
 * to rollback.
 */
class nsODBCStorageTransaction
{
public:
  nsODBCStorageTransaction(nsIODBCStorageConnection* aConnection,
                        PRBool aCommitOnComplete,
                        PRInt32 aType = nsIODBCStorageConnection::TRANSACTION_READ_COMMITTED)
    : mConnection(aConnection),
      mHasTransaction(PR_FALSE),
      mCommitOnComplete(aCommitOnComplete),
      mCompleted(PR_FALSE)
  {
    // We won't try to get a transaction if one is already in progress.
    if (mConnection)
      mHasTransaction = NS_SUCCEEDED(mConnection->BeginTransactionAs(aType));
  }
  ~nsODBCStorageTransaction()
  {
    if (mConnection && mHasTransaction && ! mCompleted) {
      if (mCommitOnComplete)
        mConnection->CommitTransaction();
      else
        mConnection->RollbackTransaction();
    }
  }

  /**
   * Commits the transaction if one is in progress. If one is not in progress,
   * this is a NOP since the actual owner of the transaction outside of our
   * scope is in charge of finally comitting or rolling back the transaction.
   */
  nsresult Commit()
  {
    if (!mConnection || mCompleted)
      return NS_OK; // no connection, or already done
    mCompleted = PR_TRUE;
    if (! mHasTransaction)
      return NS_OK; // transaction not ours, ignore
    return mConnection->CommitTransaction();
  }

  /**
   * Rolls back the transaction in progress. You should only call this function
   * if this object has a real transaction (HasTransaction() = true) because
   * otherwise, there is no transaction to roll back.
   */
  nsresult Rollback()
  {
    if (!mConnection || mCompleted)
      return NS_OK; // no connection, or already done
    mCompleted = PR_TRUE;
    if (! mHasTransaction)
      return NS_ERROR_FAILURE;

    return mConnection->RollbackTransaction();
  }

  /**
   * Returns whether this object wraps a real transaction. False means that
   * this object doesn't do anything because there was already a transaction in
   * progress when it was created.
   */
  PRBool HasTransaction()
  {
    return mHasTransaction;
  }

  /**
   * This sets the default action (commit or rollback) when this object goes
   * out of scope.
   */
  void SetDefaultAction(PRBool aCommitOnComplete)
  {
    mCommitOnComplete = aCommitOnComplete;
  }

protected:
  nsCOMPtr<nsIODBCStorageConnection> mConnection;
  PRBool mHasTransaction;
  PRBool mCommitOnComplete;
  PRBool mCompleted;
};


#endif /* _NSODBCSTORAGEHELPER_H_ */
