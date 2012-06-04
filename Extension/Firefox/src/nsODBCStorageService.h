
#ifndef _NSODBCSTORAGESERVICE_H_
#define _NSODBCSTORAGESERVICE_H_

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif


#include "nsStringAPI.h"

#include "nsIODBCStorageService.h"

#include "nsODBCUtils.h"


#define IODBCSTORAGESERVICE_CONTRACTID "@openlinksw.com/odbc;1"
#define IODBCSTORAGESERVICE_CLASSNAME "nsIODBCStorageService"
#define IODBCSTORAGESERVICE_CID { 0x23ac82f2,0x3692,0x4b47, { 0x8e, 0xe7, 0xba, 0xe3, 0x9e, 0x0e, 0x27, 0x80 } }

class nsODBCStorageConnection;

class nsODBCStorageService : public nsIODBCStorageService
{
    friend class nsODBCStorageConnection;

public:
    static nsODBCStorageService *GetSingleton();

    NS_DECL_ISUPPORTS
    NS_DECL_NSIODBCSTORAGESERVICE

private:
    virtual ~nsODBCStorageService();
    nsODBCStorageService() { m_errorCode = 0; m_henv = NULL;};

protected:
    static nsODBCStorageService *gODBCStorageService;

    nsString  m_errorMessage;
    nsString  m_errorState;
    PRInt32   m_errorCode;
    SQLHANDLE m_henv;

};

#endif /* _NSODBCSTORAGESERVICE_H_ */
