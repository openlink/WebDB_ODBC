
#ifndef _NSODBCSTORAGECONNECTION_H_
#define _NSODBCSTORAGECONNECTION_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif


#include "nsCOMPtr.h"

#include "nsStringAPI.h"

#include "nsISupportsUtils.h"
#include "nsInterfaceHashtable.h"
#include "nsIODBCStorageConnection.h"

#include "nsIMutableArray.h"
#include "nsODBCUtils.h"


//class nsIFile;
class nsIEventTarget;
class nsIThread;
class nsIODBCStorageService;
class nsODBCStorageStatement;

class nsODBCStorageConnection : public nsIODBCStorageConnection
{
public:

    nsODBCStorageConnection(nsIODBCStorageService* aService);

    NS_IMETHOD Initialize(const nsAString & connString);
    NS_IMETHOD Initialize(const nsAString & DSN, const nsAString & UID, const nsAString & PWD);

    // interfaces
    NS_DECL_ISUPPORTS
    NS_DECL_NSIODBCSTORAGECONNECTION

    // fetch the native handle
    SQLHANDLE GetNativeConnection() { return m_hdbc; }

    /**
     * Lazily creates and returns a background execution thread.  In the future,
     * the thread may be re-claimed if left idle, so you should call this
     * method just before you dispatch and not save the reference.
     * 
     * @returns an event target suitable for asynchronous statement execution.
     */
    already_AddRefed<nsIEventTarget> getAsyncExecutionTarget();

private:
    ~nsODBCStorageConnection();

protected:

    NS_IMETHOD CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
    NS_IMETHOD CheckEnvRC(SQLRETURN rc);
    NS_IMETHOD CheckDbcRC(SQLRETURN rc);


    SQLHANDLE m_henv;
    SQLHANDLE m_hdbc;					

    nsString mDSN;
    nsString mUID;
    nsString mPWD;
    nsString mconnString;
    nsString mconnOutString;

    nsString  m_errorMessage;
    nsString  m_errorState;
    long      m_errorCode;
    int       mIdentCase;
    int       mAccessMode;

    /**
     * Protects access to mAsyncExecutionThread.
     */
    SpinLock mAsyncExecutionMutex;

    /**
     * Lazily created thread for asynchronous statement execution.  Consumers
     * should use getAsyncExecutionTarget rather than directly accessing this
     * field.
     */
    nsCOMPtr<nsIThread> mAsyncExecutionThread;
    /**
     * Set to true by Close() prior to actually shutting down the thread.  This
     * lets getAsyncExecutionTarget() know not to hand out any more thread
     * references (or to create the thread in the first place).  This variable
     * should be accessed while holding the mAsyncExecutionMutex.
     */
    PRBool mAsyncExecutionThreadShuttingDown;

    SpinLock mTransactionMutex;
    PRBool mTransactionInProgress;

    // This isn't accessed but is used to make sure that the connections do
    // not outlive the service. The service, for example, owns certain locks
    // in nsStorageAsyncIO file that the connections depend on.
    nsCOMPtr<nsIODBCStorageService> mODBCStorageService;

    friend class nsODBCStorageStatement;

};

#endif /* _NSODBCSTORAGECONNECTION_H_ */
