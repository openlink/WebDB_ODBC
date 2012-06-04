/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageStatementCallback.idl
 */

#ifndef __gen_nsIODBCStorageStatementCallback_h__
#define __gen_nsIODBCStorageStatementCallback_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIODBCStorageResultSet; /* forward declaration */

class nsIODBCStorageError; /* forward declaration */


/* starting interface:    nsIODBCStorageStatementCallback */
#define NS_IODBCSTORAGESTATEMENTCALLBACK_IID_STR "7e4b5ee5-3a6e-4818-9125-438010b73341"

#define NS_IODBCSTORAGESTATEMENTCALLBACK_IID \
  {0x7e4b5ee5, 0x3a6e, 0x4818, \
    { 0x91, 0x25, 0x43, 0x80, 0x10, 0xb7, 0x33, 0x41 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageStatementCallback : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGESTATEMENTCALLBACK_IID)

  /**
   * Called when some result is obtained from the database.  This function can
   * be called more than once with a different storageIResultSet each time for
   * any given asynchronous statement.
   *
   * @param aResultSet
   *        The result set containing the data from the database.
   */
  /* void handleResult (in nsIODBCStorageResultSet aResultSet); */
  NS_SCRIPTABLE NS_IMETHOD HandleResult(nsIODBCStorageResultSet *aResultSet) = 0;

  /**
   * Called when some error occurs while executing the statement.  This function
   * may be called more than once with a different storageIError each time for
   * any given asynchronous statement.
   *
   * @param aError
   *        An object containing information about the error.
   */
  /* void handleError (in nsIODBCStorageError aError); */
  NS_SCRIPTABLE NS_IMETHOD HandleError(nsIODBCStorageError *aError) = 0;

  /**
   * Called when the statement has finished executing.  This function will only
   * be called once for any given asynchronous statement.
   *
   * @param aReason
   *        Indicates if the statement is no longer executing because it either
   *        finished (REASON_FINISHED), was canceled (REASON_CANCELED), or
   *        a fatal error occurred (REASON_ERROR).
   */
  enum { REASON_FINISHED = 0U };

  enum { REASON_CANCELED = 1U };

  enum { REASON_ERROR = 2U };

  /* void handleCompletion (in unsigned short aReason); */
  NS_SCRIPTABLE NS_IMETHOD HandleCompletion(PRUint16 aReason) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageStatementCallback, NS_IODBCSTORAGESTATEMENTCALLBACK_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGESTATEMENTCALLBACK \
  NS_SCRIPTABLE NS_IMETHOD HandleResult(nsIODBCStorageResultSet *aResultSet); \
  NS_SCRIPTABLE NS_IMETHOD HandleError(nsIODBCStorageError *aError); \
  NS_SCRIPTABLE NS_IMETHOD HandleCompletion(PRUint16 aReason); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGESTATEMENTCALLBACK(_to) \
  NS_SCRIPTABLE NS_IMETHOD HandleResult(nsIODBCStorageResultSet *aResultSet) { return _to HandleResult(aResultSet); } \
  NS_SCRIPTABLE NS_IMETHOD HandleError(nsIODBCStorageError *aError) { return _to HandleError(aError); } \
  NS_SCRIPTABLE NS_IMETHOD HandleCompletion(PRUint16 aReason) { return _to HandleCompletion(aReason); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGESTATEMENTCALLBACK(_to) \
  NS_SCRIPTABLE NS_IMETHOD HandleResult(nsIODBCStorageResultSet *aResultSet) { return !_to ? NS_ERROR_NULL_POINTER : _to->HandleResult(aResultSet); } \
  NS_SCRIPTABLE NS_IMETHOD HandleError(nsIODBCStorageError *aError) { return !_to ? NS_ERROR_NULL_POINTER : _to->HandleError(aError); } \
  NS_SCRIPTABLE NS_IMETHOD HandleCompletion(PRUint16 aReason) { return !_to ? NS_ERROR_NULL_POINTER : _to->HandleCompletion(aReason); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageStatementCallback : public nsIODBCStorageStatementCallback
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGESTATEMENTCALLBACK

  nsODBCStorageStatementCallback();

private:
  ~nsODBCStorageStatementCallback();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageStatementCallback, nsIODBCStorageStatementCallback)

nsODBCStorageStatementCallback::nsODBCStorageStatementCallback()
{
  /* member initializers and constructor code */
}

nsODBCStorageStatementCallback::~nsODBCStorageStatementCallback()
{
  /* destructor code */
}

/* void handleResult (in nsIODBCStorageResultSet aResultSet); */
NS_IMETHODIMP nsODBCStorageStatementCallback::HandleResult(nsIODBCStorageResultSet *aResultSet)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void handleError (in nsIODBCStorageError aError); */
NS_IMETHODIMP nsODBCStorageStatementCallback::HandleError(nsIODBCStorageError *aError)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void handleCompletion (in unsigned short aReason); */
NS_IMETHODIMP nsODBCStorageStatementCallback::HandleCompletion(PRUint16 aReason)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageStatementCallback_h__ */
