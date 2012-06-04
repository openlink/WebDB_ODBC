#include <windows.h>
#include <windowsx.h>

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include <sql.h>
#include <sqlext.h>

#ifndef H_CONNECTION
#define H_CONNECTION

class Connection : public FB::JSAPIAuto
{
public:
    Connection(const FB::BrowserHostPtr& host);
    virtual ~Connection();

    int Open(const std::wstring& connString);
    int Close();
    int BeginTransaction();
    int CommitTransaction();
    int RollbackTransaction();
    FB::JSAPIPtr CreateStatement();
    int ExecAsync(const FB::JSObjectPtr& callback);

    long GetErrorCode();
    std::wstring GetErrorMessage();
    std::wstring GetErrorState();
    int GetSchemaVersion();
    void SetSchemaVersion(int aVersion);
    bool GetReadOnlyMode();
    void SetReadOnlyMode(bool aReadOnlyMode);
    long GetLastInsertRowID();

    void Init(const FB::JSAPIWeakPtr ptr) { m_conn = ptr;};

private:
    FB::JSAPIWeakPtr m_conn;
    FB::BrowserHostPtr m_host;

    SQLHANDLE m_henv;
    SQLHANDLE m_hdbc;

    std::wstring m_errMessage;
    std::wstring m_errState;
    long    m_errCode;

    int     mIdentCase;
    int     mAccessMode;

    void StoreError(SWORD fHandleType, SQLHANDLE handle);
    int CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
    int CheckDbcRC(SQLRETURN rc);
    int CheckStmtRC(SQLHANDLE hstmt, SQLRETURN rc);
    int CheckEnvRC(SQLRETURN rc);

    int ExecuteSimpleSQL(const std::wstring& aSQLStatement, bool ignoreErr);
    int TableExists(const std::wstring& aTableName, bool *_retval);

};

#endif // H_CONNECTION
