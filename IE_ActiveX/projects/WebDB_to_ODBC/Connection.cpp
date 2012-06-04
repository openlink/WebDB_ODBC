//#define UNICODE

#include "JSObject.h"
#include <boost/assign.hpp>
#include "DOM/Document.h"
#include "variant_list.h"
#include "APITypes.h"
#include <vector>
using boost::assign::list_of;

#include "Connection.h"
#include "Statement.h"
#include "Common.h"



Connection::Connection(const FB::BrowserHostPtr& host) : m_host(host)
{

    registerMethod("Open",  make_method(this, &Connection::Open));
    registerMethod("Close",  make_method(this, &Connection::Close));
    registerMethod("BeginTransaction",  make_method(this, &Connection::BeginTransaction));
    registerMethod("CommitTransaction",  make_method(this, &Connection::CommitTransaction));
    registerMethod("RollbackTransaction",  make_method(this, &Connection::RollbackTransaction));
    registerMethod("CreateStatement",  make_method(this, &Connection::CreateStatement));
    registerMethod("ExecAsync",  make_method(this, &Connection::ExecAsync));

    
    registerProperty("errorCode",  make_property(this, 
    					&Connection::GetErrorCode));
    registerProperty("errorMessage", make_property(this, 
    					&Connection::GetErrorMessage));
    registerProperty("errorState", make_property(this, 
    					&Connection::GetErrorState));

    registerProperty("schemaVersion", make_property(this, 
    					&Connection::GetSchemaVersion,
    					&Connection::SetSchemaVersion));
    registerProperty("readOnlyMode",  make_property(this, 
    					&Connection::GetReadOnlyMode,
    					&Connection::SetReadOnlyMode));
    registerProperty("lastInsertRowID",  make_property(this, 
    					&Connection::GetLastInsertRowID));


    m_errCode = 0;

    m_henv = NULL;
    m_hdbc = NULL;
    mIdentCase = 0;
    mAccessMode = 0;
}

Connection::~Connection()
{
    Close();
}



void 
Connection::StoreError(SWORD fHandleType, SQLHANDLE handle)
{	
    SQLWCHAR szErrState[SQL_SQLSTATE_SIZE+1];    // SQL Error State string
    SQLINTEGER dwErrCode;				 // Native Error code
    SQLWCHAR szErrText[SQL_MAX_MESSAGE_LENGTH+1];// SQL Error Text string
    SWORD    wErrMsgLen;			 // Error message length
    SQLWCHAR szTmp[SQL_MAX_MESSAGE_LENGTH+64];
	
    SQLWCHAR szBuffer[1024];	     //SQL_SQLSTATE_SIZE+SQL_MAX_MESSAGE_LENGTH+1];
    szBuffer[0] = 0;		
	
    SQLRETURN  nErrResult;  // Return Code from SQLGetDiagRec
    SWORD sMsgNum = 1;	    // no of records

    m_errMessage.empty();
    m_errState.empty();
    m_errCode = 0;

    nErrResult = SQLGetDiagRecW(fHandleType, handle, sMsgNum,
		szErrState, &dwErrCode, szErrText,
		SQL_MAX_MESSAGE_LENGTH-1, &wErrMsgLen);
    if (nErrResult != SQL_NO_DATA) 
    {	
	if(nErrResult == SQL_ERROR || nErrResult == SQL_INVALID_HANDLE){
	  m_errMessage.assign(L"\nError in executing SQLGetDiagRecW function");			

	} else {
          _snwprintf((wchar_t *)szTmp, sizeof(szTmp)/sizeof(wchar_t), L"\nSQL Error State  : %s\nNative Error Code: %lX\nODBC Error       : %s",(LPWSTR) szErrState, dwErrCode, (LPWSTR)szErrText);
	  m_errMessage.assign(szTmp);
          m_errCode = (dwErrCode?dwErrCode:1);
          m_errState.assign(szErrState);
        }
      throw FB::script_error("$DB$");
    }	
}



int
Connection::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
{
  m_errCode = 0;
  m_errState.empty();
  m_errMessage.empty();

  if (!SQL_SUCCEEDED(rc)) {

    StoreError(fHandleType, handle);  
    return PE_ERROR;

  } else {

    return PE_OK;
  }
}


int
Connection::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}

int
Connection::CheckStmtRC(SQLHANDLE hstmt, SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_STMT, hstmt, rc);
}

int
Connection::CheckEnvRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_ENV, m_henv, rc);
}




int
Connection::ExecAsync(const FB::JSObjectPtr& callback)
{
  FB::VariantList args;

  callback->InvokeAsync("run", args);
  return PE_OK;
}


int
Connection::Open(const std::wstring& connString)
{
    if (m_hdbc) {
      throw FB::script_error("Connection is already opened");
    }
	
    int rc;

    // Allocate the environment handle
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE, &m_henv));
    if (rc != PE_OK) {
      return rc;
    }
		
    // Set the enviroment attributes
    rc = CheckEnvRC(SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
    if (rc != PE_OK) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = NULL; 		
      return rc;
    }	

    // Allocate a new database Connection Handle.
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc));
    if (rc != PE_OK) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = NULL; 		
      return rc;
    }

    // set the connection attributes
    rc = CheckEnvRC(SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (void *)5, 0));
    if (rc != PE_OK)
      goto failed;

    SQLWCHAR connOut[4096];    
    short buflen;

    rc = CheckDbcRC(SQLDriverConnectW (m_hdbc, 0, (SQLWCHAR *)connString.c_str(), 
       SQL_NTS, connOut, NUMCHAR(connOut), &buflen, SQL_DRIVER_COMPLETE));

    // Free all the allocated handles if the SQLConnect call is unsuccessful
    if (rc != PE_OK)
      goto failed;

    bool v;
    SQLSMALLINT retLen;
    rc = CheckDbcRC(SQLGetInfoW (m_hdbc, SQL_IDENTIFIER_CASE, &mIdentCase,
			(SQLSMALLINT) sizeof (mIdentCase), &retLen));
    if (rc != PE_OK)
      goto failed;

    rc = CheckDbcRC(SQLGetConnectOption (m_hdbc, SQL_ACCESS_MODE, &mAccessMode));
    if (rc != PE_OK)
      goto failed;

    if (mAccessMode == SQL_MODE_READ_ONLY)
      return PE_OK; 

    if (mIdentCase == SQL_IC_LOWER) {
      TableExists(L"nsiodbc_version", &v);
      if (!v) {
        ExecuteSimpleSQL(L"drop table nsiodbc_version", true);
        rc = ExecuteSimpleSQL(L"create table nsiodbc_version(ver integer)", false);
        if (rc == PE_OK)
          ExecuteSimpleSQL(L"insert into nsiodbc_version values(0)", false);
      }
    } else {
      TableExists(L"NSIODBC_VERSION", &v);
      if (!v) {
        ExecuteSimpleSQL(L"drop table NSIODBC_VERSION", true);
        rc = ExecuteSimpleSQL(L"create table NSIODBC_VERSION(VER integer)", false);
        if (rc == PE_OK)
          ExecuteSimpleSQL(L"insert into NSIODBC_VERSION values(0)", false);
      }
    }
    
    return PE_OK;

failed:
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
    m_hdbc = NULL; 				
    m_henv = NULL; 		
    return rc;
}


//===============================================
int
Connection::Close()
{
    int rc;

    if (!m_hdbc)
        return PE_OK;


    rc = CheckEnvRC(SQLDisconnect(m_hdbc));
    if (rc != PE_OK)
      return rc;

    // free the database handle
    if (m_hdbc != NULL) 
      SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);

	// free the environment handle
    if (m_henv != NULL) 
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

    m_hdbc = NULL; 				
    m_henv = NULL; 

    return PE_OK;
}


long
Connection::GetLastInsertRowID()
{
    long lastInsertRowID = 0;

    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    int rc = PE_OK;
    SQLHANDLE hstmt = NULL;
    SQLCHAR buf[256];
    SQLSMALLINT len;

    int ret = SQLGetInfo(m_hdbc, SQL_DBMS_NAME, buf, sizeof(buf), &len);
    if (ret == SQL_SUCCESS && strncmp((char*)buf, "OpenLink Virtuoso",17)==0) {

      rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
      if (rc != PE_OK)
        return 0;

      rc = CheckStmtRC(hstmt, SQLExecDirectA(hstmt, (SQLCHAR *)"select {fn convert(identity_value(), SQL_INTEGER)}", SQL_NTS));
      if (rc != PE_OK)
        goto done;

      if (SQL_SUCCEEDED(SQLFetch(hstmt))) {
        SQLLEN cbInd;
        int val;
        rc = CheckStmtRC(hstmt, SQLGetData(hstmt, 1, SQL_C_LONG, &val, 0, &cbInd));
        if (rc != PE_OK)
          goto done;
        if (cbInd != SQL_NULL_DATA)
          lastInsertRowID = val;
      }
    }

done:
    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return lastInsertRowID;

}


long
Connection::GetErrorCode()
{
    return m_errCode;
}

std::wstring
Connection::GetErrorMessage()
{
    return m_errMessage;
}

/* readonly attribute AString lastErrorState */
std::wstring
Connection::GetErrorState()
{
    return m_errState;
}

int
Connection::GetSchemaVersion()
{
    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    int version = 0;
    SQLHANDLE hstmt = NULL;
    int rc;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != PE_OK)
      return 0;

    rc = CheckStmtRC(hstmt, SQLExecDirectA(hstmt, 
    	(SQLCHAR *)(mIdentCase==SQL_IC_LOWER?"select ver from nsiodbc_version":"select VER from NSIODBC_VERSION"),
    	 SQL_NTS));
    if (rc != PE_OK)
      goto done;

    if (SQL_SUCCEEDED(SQLFetch(hstmt))) {
      SQLLEN cbInd;
      rc = CheckStmtRC(hstmt, SQLGetData(hstmt, 1, SQL_C_LONG, &version, 0, &cbInd));
      if (rc != PE_OK)
        goto done;
    }

done:
    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return version;
}


void
Connection::SetSchemaVersion(int aVersion)
{
    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    SQLHANDLE hstmt = NULL;
    int rc;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != PE_OK)
      return;

    char buf[256];

    sprintf(buf, (mIdentCase==SQL_IC_LOWER?"update nsiodbc_version set ver= %d":"update NSIODBC_VERSION set VER= %d"), aVersion);
    rc = CheckStmtRC(hstmt, SQLExecDirectA(hstmt, (SQLCHAR *)buf, SQL_NTS));

    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
}

/* attribute boolean readOnlyMode; */
bool 
Connection::GetReadOnlyMode()
{
    int rc;
    SQLUINTEGER mode;
    SQLINTEGER  ind;

    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    rc = CheckDbcRC(SQLGetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, &mode, 0, &ind));
    if (rc != PE_OK)
      return false;

    return (mode == SQL_MODE_READ_ONLY? true: false);
}


void
Connection::SetReadOnlyMode(bool aReadOnlyMode)
{
    SQLPOINTER flag = (SQLPOINTER)(aReadOnlyMode ? SQL_MODE_READ_ONLY:SQL_MODE_READ_WRITE);

    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    CheckDbcRC(SQLSetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, flag, 0));
}


int
Connection::ExecuteSimpleSQL(const std::wstring& aSQLStatement, bool ignoreErr)
{
    int rc;
    SQLHANDLE hstmt; 

    if (!m_hdbc) {
      throw FB::script_error("Connection isn't initialized");
    }

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != PE_OK)
      return rc;

    try {
      if (ignoreErr) {
        SQLRETURN ret = SQLExecDirect(hstmt, (SQLWCHAR *)aSQLStatement.c_str(), SQL_NTS);
        rc = SQL_SUCCEEDED(ret)?PE_OK:PE_ERROR;
      } else {
        rc = CheckStmtRC(hstmt, SQLExecDirect(hstmt, (SQLWCHAR *)aSQLStatement.c_str(), SQL_NTS));
      }
    } catch(...){}

    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}


/* boolean tableExists (in AString aTableName); */
int
Connection::TableExists(const std::wstring& aTableName, bool *_retval)
{
    if (!m_hdbc)
      throw FB::script_error("Connection isn't initialized");

    int rc;
    SQLHANDLE hstmt; 

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != PE_OK)
      return rc;

    rc = CheckStmtRC(hstmt, SQLTablesW(hstmt, NULL, 0, NULL, 0,
      (SQLWCHAR*)aTableName.c_str(), SQL_NTS, NULL, 0));
    if (rc != PE_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rc;
    }

    bool exists = false;
    SQLRETURN ret = SQLFetch(hstmt);

    if (SQL_SUCCEEDED(ret)) {
      exists = true;

    } else if (ret == SQL_NO_DATA_FOUND) {
      exists = false;

    } else {
      StoreError(SQL_HANDLE_STMT, hstmt);  
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return PE_ERROR;
    }

    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);

    *_retval = exists;
    return PE_OK;
}


/**
 ** Transactions
 **/
int
Connection::BeginTransaction()
{
    int rc;
//??todo add check transaction is progress check

    if (!m_hdbc) {
      throw FB::script_error("Connection isn't initialized");
    }

    rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0));
    return rc;
}


int
Connection::CommitTransaction()
{
    int rc;

    if (!m_hdbc) {
      throw FB::script_error("Connection isn't initialized");
    }

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT));
    return rc;
}

int
Connection::RollbackTransaction()
{
    int rc;

    if (!m_hdbc) {
      throw FB::script_error("Connection isn't initialized");
    }

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK));
    return rc;
}


FB::JSAPIPtr
Connection::CreateStatement()
{
    return boost::make_shared<Statement>(m_host, m_hdbc, m_conn);
}

