/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- 
 *
 * ***** END LICENSE BLOCK ***** */

#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif


#ifdef XP_UNIX
#include <string.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <wchar.h>

#include "plugin.h"
#include "npfunctions.h"

#include "utils.h"
#include "mthreads.h"
#include "Connection.h"
#include "Statement.h"

//extern void log(const char *s);
//static char tmp[1024];

static NPObject *
AllocateStatementObject(NPP npp, NPClass *aClass)
{
  return new StatementObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(StatementObject,  AllocateStatementObject);

//========================================================================
//========================================================================


ConnectionObject::ConnectionObject(NPP npp)
    : ScriptablePluginObjectBase(npp)
{
    mc_run_id = NPN_GetStringIdentifier("run");
    mc_Open_id = NPN_GetStringIdentifier("Open");
    mc_Close_id = NPN_GetStringIdentifier("Close");
    mc_ExecAsync_id = NPN_GetStringIdentifier("ExecAsync");
    mc_BeginTransaction_id = NPN_GetStringIdentifier("BeginTransaction");
    mc_CommitTransaction_id = NPN_GetStringIdentifier("CommitTransaction");
    mc_RollbackTransaction_id = NPN_GetStringIdentifier("RollbackTransaction");
    mc_CreateStatement_id = NPN_GetStringIdentifier("CreateStatement");
    mc_Init_id = NPN_GetStringIdentifier("Init");

    mc_errorCode_id = NPN_GetStringIdentifier("errorCode");
    mc_errorMessage_id = NPN_GetStringIdentifier("errorMessage");
    mc_errorState_id = NPN_GetStringIdentifier("errorState");
    mc_schemaVersion_id = NPN_GetStringIdentifier("schemaVersion");
    mc_readOnlyMode_id = NPN_GetStringIdentifier("readOnlyMode");
    mc_lastInsertRowID_id = NPN_GetStringIdentifier("lastInsertRowID");

    m_errCode = 0;
    m_errMessage[0] = 0;
    m_errState[0] = 0;

    m_henv = NULL;
    m_hdbc = NULL;
    mIdentCase = 0;
    mAccessMode = 0;
    mTransactionInProgress = false;
}


ConnectionObject::~ConnectionObject()
{
  Close();
}


bool
ConnectionObject::HasMethod(NPIdentifier name)
{
//sprintf(tmp, "con HasMethod [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_Open_id
   || name == mc_Close_id
   || name == mc_ExecAsync_id
   || name == mc_BeginTransaction_id
   || name == mc_CommitTransaction_id
   || name == mc_RollbackTransaction_id 
   || name == mc_CreateStatement_id )
    return true;
  else
    return false;
}

bool
ConnectionObject::HasProperty(NPIdentifier name)
{
//sprintf(tmp, "con HasProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_errorCode_id
    || name == mc_errorMessage_id
    || name == mc_errorState_id
    || name == mc_schemaVersion_id
    || name == mc_readOnlyMode_id 
    || name == mc_lastInsertRowID_id)
    return true;
  else
    return false;
}

bool
ConnectionObject::GetProperty(NPIdentifier name, NPVariant *result)
{
//sprintf(tmp, "con GetProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
//char *tmp = NPN_UTF8FromIdentifier(name);
  NPError rc;
  VOID_TO_NPVARIANT(*result);

  if (name == mc_schemaVersion_id)
    {
      SQLINTEGER ver;
      rc = GetSchemaVersion(&ver);
      if (rc != NPERR_NO_ERROR)
        return true;
      INT32_TO_NPVARIANT(ver, *result);
      return true;
    }
  else if (name == mc_errorMessage_id)
    {
      STRING_TO_NPVARIANT(GetErrorString(), *result);
      return true;
    }
  else if (name == mc_errorCode_id)
    {
      INT32_TO_NPVARIANT(GetErrorCode(), *result);
      return true;
    }
  else if (name == mc_errorState_id)
    {
      STRING_TO_NPVARIANT(GetErrorState(), *result);
      return true;
    }
  else if (name == mc_readOnlyMode_id)
    {
      bool read;
      rc = GetReadOnlyMode(&read);
      if (rc != NPERR_NO_ERROR)
        return true;
      BOOLEAN_TO_NPVARIANT(read, *result);
      return true;
	}
  else if (name == mc_lastInsertRowID_id)
    {
      long id = -1;
      rc = GetLastInsertRowID(&id);
      if (rc != NPERR_NO_ERROR)
        return true;
      INT32_TO_NPVARIANT(id, *result);
      return true;
    }

  return false;

}

bool
ConnectionObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
//sprintf(tmp, "con SetProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_schemaVersion_id)
    {
      if (!NPVARIANT_IS_INT32(*value)) {
        NPN_SetException(this, "Wrong argument type");
        return true;
      }

      SetSchemaVersion(NPVARIANT_TO_INT32(*value));
      return true;
    }
  else if (name == mc_readOnlyMode_id)
    {
      if (!NPVARIANT_IS_BOOLEAN(*value)) {
        NPN_SetException(this, "Wrong argument type");
        return true;
      }
      SetReadOnlyMode(NPVARIANT_TO_BOOLEAN(*value));
      return true;
    }
  return false;
}


bool
ConnectionObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
//sprintf(tmp, "con Invoke [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  VOID_TO_NPVARIANT(*result);

  if (name == mc_Open_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!NPVARIANT_IS_STRING(args[0])) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }
    Open((NPString*)&NPVARIANT_TO_STRING(args[0]));
    return true;

  } else if (name == mc_Close_id) {
    Close();
    return true;

  } else if (name == mc_ExecAsync_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!NPVARIANT_IS_OBJECT(args[0])) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }
    ExecAsync(NPVARIANT_TO_OBJECT(args[0]));
    return true;

  } else if (name == mc_BeginTransaction_id) {
    BeginTransaction();
    return true;

  } else if (name == mc_CommitTransaction_id) {
    CommitTransaction();
    return true;

  } else if (name == mc_RollbackTransaction_id) {
    RollbackTransaction();
    return true;
  } else if (name == mc_CreateStatement_id) {
    if (argCount > 0) {
      NPN_SetException(this, "Wrong parameters count");
      return true;
    }

    NPObject *pStmtObj = NPN_CreateObject(mNpp, GET_NPOBJECT_CLASS(StatementObject));

    NPVariant ret, par[2];
#if 0
    INT32_TO_NPVARIANT((long)m_hdbc, par[0]);
#else
    OBJECT_TO_NPVARIANT((NPObject*)m_hdbc, par[0]);
#endif
    OBJECT_TO_NPVARIANT(this, par[1]);
    NPN_Invoke(mNpp, pStmtObj, mc_Init_id, par, 2, &ret);
    NPN_ReleaseVariantValue(&ret);
    OBJECT_TO_NPVARIANT(pStmtObj, *result);
    return true;
  }  

  return false;
}


//===========================================================================
//===========================================================================


void 
ConnectionObject::StoreError(SWORD fHandleType, SQLHANDLE handle)
{	
    SQLWCHAR szErrState[SQL_SQLSTATE_SIZE+1];    // SQL Error State string
    SQLINTEGER dwErrCode;				 // Native Error code
    SQLWCHAR szErrText[SQL_MAX_MESSAGE_LENGTH+1];// SQL Error Text string
    SWORD    wErrMsgLen;			 // Error message length
	
    SQLWCHAR szBuffer[1024];	     //SQL_SQLSTATE_SIZE+SQL_MAX_MESSAGE_LENGTH+1];
    szBuffer[0] = 0;		
	
    SQLRETURN  nErrResult;  // Return Code from SQLGetDiagRec
    SWORD sMsgNum = 1;	    // no of records

    m_errMessage[0] = 0;
    m_errState[0] = 0;
    m_errCode = 0;

    nErrResult = SQLGetDiagRecW(fHandleType, handle, sMsgNum,
		szErrState, &dwErrCode, szErrText,
		SQL_MAX_MESSAGE_LENGTH-1, &wErrMsgLen);
    if (nErrResult != SQL_NO_DATA) 
    {	

	if(nErrResult == SQL_ERROR || nErrResult == SQL_INVALID_HANDLE){
	  wcscpy(m_errMessage, L"\nError in executing SQLGetDiagRecW function");			

	} else {
#ifdef XP_WIN
          _snwprintf((wchar_t *)m_errMessage, sizeof(m_errMessage)/sizeof(wchar_t), L"\nSQL Error State  : %s\nNative Error Code: %lX\nODBC Error       : %s",(LPWSTR) szErrState, dwErrCode, (LPWSTR)szErrText);				
#else
          swprintf((wchar_t *)m_errMessage, sizeof(m_errMessage)/sizeof(wchar_t), L"\nSQL Error State  : %ls\nNative Error Code: %lX\nODBC Error       : %ls",(LPWSTR) szErrState, dwErrCode, (LPWSTR)szErrText);				
#endif
          m_errCode = (dwErrCode?dwErrCode:1);
          wcscpy(m_errState, szErrState);
        }
      NPN_SetException(this, "$DB$");
    }	
}


NPError
ConnectionObject::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
{
  m_errCode = 0;
  m_errState[0] = 0;
  m_errMessage[0] = 0;

  if (!SQL_SUCCEEDED(rc)) {

    StoreError(fHandleType, handle);  
    return NPERR_GENERIC_ERROR;

  } else {

    return NPERR_NO_ERROR;
  }
}


NPError
ConnectionObject::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}

NPError
ConnectionObject::CheckStmtRC(SQLHANDLE hstmt, SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_STMT, hstmt, rc);
}

NPError
ConnectionObject::CheckEnvRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_ENV, m_henv, rc);
}




void asyncProc(void *ptr) 
{
  NPVariant result;
  NPVariant arg;
  AsyncParam *param = (AsyncParam *)ptr;

  VOID_TO_NPVARIANT(result);
  VOID_TO_NPVARIANT(arg);

  NPN_Invoke(param->npp, param->pObj, param->mc_run_id, &arg, 0, &result);

  AsyncParamFree(param);
}


NPError
ConnectionObject::ExecAsync(NPObject *pObj)
{
  AsyncParam *param = (AsyncParam *)NPN_MemAlloc(sizeof(AsyncParam));
  if (!param)
	  return NPERR_GENERIC_ERROR;
  param->npp = mNpp;
  param->pObj = NPN_RetainObject(pObj);
  param->mc_run_id = mc_run_id;
  
  NPN_PluginThreadAsyncCall(mNpp, asyncProc, (void*)param);
  return NPERR_NO_ERROR;
}


NPError
ConnectionObject::Open(NPString *connString)
{
//    NS_ENSURE_TRUE(mAsyncExecutionMutex, NS_ERROR_OUT_OF_MEMORY);
//    NS_ENSURE_TRUE(mTransactionMutex, NS_ERROR_OUT_OF_MEMORY);

    if (m_hdbc) {
      NPN_SetException(this, "Connection is already opened");
      return NPERR_GENERIC_ERROR;
    }
	
    NPError rc;

    // Allocate the environment handle
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE, &m_henv));
    if (rc != NPERR_NO_ERROR) {
      return rc;
    }
		
    // Set the enviroment attributes
    rc = CheckEnvRC(SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
    if (rc != NPERR_NO_ERROR) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = NULL; 		
      return rc;
    }	

    // Allocate a new database Connection Handle.
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc));
    if (rc != NPERR_NO_ERROR) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = NULL; 		
      return rc;
    }

    // set the connection attributes
    rc = CheckEnvRC(SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (void *)5, 0));
    if (rc != NPERR_NO_ERROR)
      goto failed;

    SQLWCHAR connOut[4096];    
    short buflen;

    rc = CheckDbcRC(SQLDriverConnectW (m_hdbc, 0, nsWString(connString).Data(), 
       SQL_NTS, connOut, NUMCHAR(connOut), &buflen, SQL_DRIVER_COMPLETE));

    // Free all the allocated handles if the SQLConnect call is unsuccessful
    if (rc != NPERR_NO_ERROR)
      goto failed;

    bool v;
    SQLSMALLINT retLen;
    rc = CheckDbcRC(SQLGetInfoW (m_hdbc, SQL_IDENTIFIER_CASE, &mIdentCase,
			(SQLSMALLINT) sizeof (mIdentCase), &retLen));
    if (rc != NPERR_NO_ERROR)
      goto failed;

    rc = CheckDbcRC(SQLGetConnectOption (m_hdbc, SQL_ACCESS_MODE, &mAccessMode));
    if (rc != NPERR_NO_ERROR)
      goto failed;

    if (mAccessMode == SQL_MODE_READ_ONLY)
      return NPERR_NO_ERROR; 

    if (mIdentCase == SQL_IC_LOWER) {
      TableExists("nsiodbc_version", &v);
      if (!v) {
        ExecuteSimpleSQL("drop table nsiodbc_version", true);
        rc = ExecuteSimpleSQL("create table nsiodbc_version(ver integer)", false);
        if (rc == NPERR_NO_ERROR)
          ExecuteSimpleSQL("insert into nsiodbc_version values(0)", false);
      }
    } else {
      TableExists("NSIODBC_VERSION", &v);
      if (!v) {
        ExecuteSimpleSQL("drop table NSIODBC_VERSION", true);
        rc = ExecuteSimpleSQL("create table NSIODBC_VERSION(VER integer)", false);
        if (rc == NPERR_NO_ERROR)
          ExecuteSimpleSQL("insert into NSIODBC_VERSION values(0)", false);
      }
    }
    
    return NPERR_NO_ERROR;

failed:
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
    m_hdbc = NULL; 				
    m_henv = NULL; 		
    return rc;
}


//===============================================
NPError
ConnectionObject::Close()
{
    NPError rc;

    if (!m_hdbc)
        return NPERR_NO_ERROR; //NS_ERROR_NOT_INITIALIZED;


    rc = CheckEnvRC(SQLDisconnect(m_hdbc));
    if (rc != NPERR_NO_ERROR)
      return rc;

    // free the database handle
    if (m_hdbc != NULL) 
      SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);

	// free the environment handle
    if (m_henv != NULL) 
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

    m_hdbc = NULL; 				
    m_henv = NULL; 

    return NPERR_NO_ERROR;
}


NPError
ConnectionObject::GetLastInsertRowID(long *aLastInsertRowID)
{
    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    *aLastInsertRowID = 0;

    NPError rc = NPERR_NO_ERROR;
    SQLHANDLE hstmt = NULL;
    SQLCHAR buf[256];
    SQLSMALLINT len;

    int ret = SQLGetInfo(m_hdbc, SQL_DBMS_NAME, buf, sizeof(buf), &len);
    if (ret == SQL_SUCCESS && strncmp((char*)buf, "OpenLink Virtuoso",17)==0) {

      rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
      if (rc != NPERR_NO_ERROR)
        return rc;

      rc = CheckStmtRC(hstmt, SQLExecDirect(hstmt, (SQLCHAR *)"select {fn convert(identity_value(), SQL_INTEGER)}", SQL_NTS));
      if (rc != NPERR_NO_ERROR)
        goto done;

      if (SQL_SUCCEEDED(SQLFetch(hstmt))) {
        SQLLEN cbInd;
        SQLINTEGER val;
        rc = CheckStmtRC(hstmt, SQLGetData(hstmt, 1, SQL_C_LONG, &val, 0, &cbInd));
        if (rc != NPERR_NO_ERROR)
          goto done;
        if (cbInd != SQL_NULL_DATA)
          *aLastInsertRowID = val;
      }
    }

done:
    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;

}


long
ConnectionObject::GetErrorCode()
{
    return m_errCode;
}

const char *
ConnectionObject::GetErrorString()
{
    return strdup_WtoU8(m_errMessage);
}

/* readonly attribute AString lastErrorState */
const char *
ConnectionObject::GetErrorState()
{
    return strdup_WtoU8(m_errState);
}

NPError
ConnectionObject::GetSchemaVersion(SQLINTEGER *version)
{
    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    *version = 0;
    SQLHANDLE hstmt = NULL;
    NPError rc;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NPERR_NO_ERROR)
      return rc;

    rc = CheckStmtRC(hstmt, SQLExecDirect(hstmt, 
    	(SQLCHAR *)(mIdentCase==SQL_IC_LOWER?"select ver from nsiodbc_version":"select VER from NSIODBC_VERSION"),
    	 SQL_NTS));
    if (rc != NPERR_NO_ERROR)
      goto done;

    if (SQL_SUCCEEDED(SQLFetch(hstmt))) {
      SQLLEN cbInd;
      int val;
      rc = CheckStmtRC(hstmt, SQLGetData(hstmt, 1, SQL_C_LONG, &val, 0, &cbInd));
      if (rc != NPERR_NO_ERROR)
        goto done;
      if (cbInd != SQL_NULL_DATA)
        *version = val;
    }

done:
    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}

NPError
ConnectionObject::SetSchemaVersion(SQLINTEGER aVersion)
{
    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    SQLHANDLE hstmt = NULL;
    NPError rc;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NPERR_NO_ERROR)
      return rc;

    char buf[256];

    sprintf(buf, (mIdentCase==SQL_IC_LOWER?"update nsiodbc_version set ver= %d":"update NSIODBC_VERSION set VER= %d"), (int)aVersion);
    rc = CheckStmtRC(hstmt, SQLExecDirect(hstmt, (SQLCHAR *)buf, SQL_NTS));

    if (hstmt != NULL)
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}

/* attribute boolean readOnlyMode; */
NPError 
ConnectionObject::GetReadOnlyMode(bool *aReadOnlyMode)
{
    NPError rc;
    SQLUINTEGER mode;
    SQLINTEGER  ind;

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLGetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, &mode, 0, &ind));
    if (rc != NPERR_NO_ERROR)
      return rc;

    *aReadOnlyMode = (mode == SQL_MODE_READ_ONLY? true: false);

    return rc;
}

NPError 
ConnectionObject::SetReadOnlyMode(bool aReadOnlyMode)
{
    NPError rc;
	SQLPOINTER flag = (SQLPOINTER)(aReadOnlyMode ? SQL_MODE_READ_ONLY:SQL_MODE_READ_WRITE);

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLSetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, flag, 0));
    return rc;
}


NPError
ConnectionObject::ExecuteSimpleSQL(const char *aSQLStatement, bool ignoreErr)
{
    NPError rc;
    SQLHANDLE hstmt; 

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NPERR_NO_ERROR)
      return rc;

    if (ignoreErr) {
      SQLRETURN ret = SQLExecDirectW(hstmt, nsWString(aSQLStatement).Data(), SQL_NTS);
      rc = SQL_SUCCEEDED(ret)?NPERR_NO_ERROR:NPERR_GENERIC_ERROR;
    } else {
      rc = CheckStmtRC(hstmt, SQLExecDirectW(hstmt, nsWString(aSQLStatement).Data(), SQL_NTS));
    }
    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}

/* boolean tableExists (in AString aTableName); */
NPError
ConnectionObject::TableExists(const char *aTableName, bool *_retval)
{
    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    NPError rc;
    SQLHANDLE hstmt; 

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NPERR_NO_ERROR)
      return rc;

    rc = CheckStmtRC(hstmt, SQLTablesW(hstmt, NULL, 0, NULL, 0,
      nsWString(aTableName).Data(), SQL_NTS, NULL, 0));
    if (rc != NPERR_NO_ERROR) {
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
      return NPERR_GENERIC_ERROR;
    }

    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);

    *_retval = exists;
    return NPERR_NO_ERROR;
}


/**
 ** Transactions
 **/

NPError
ConnectionObject::GetTransactionInProgress(bool *_retval)
{
    AutoLock autoLock(mTransactionMutex);
    *_retval = mTransactionInProgress;
    return NPERR_NO_ERROR;
}


NPError
ConnectionObject::BeginTransaction()
{
    AutoLock autoLock(mTransactionMutex);
    if (mTransactionInProgress) {
      NPN_SetException(this, "Transaction is in progress already");
      return NPERR_GENERIC_ERROR;
    }

    NPError rc;

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0));
    if (rc == NPERR_NO_ERROR) 
      mTransactionInProgress = true;

    return rc;
}


NPError
ConnectionObject::CommitTransaction()
{
    AutoLock autoLock(mTransactionMutex);
//    if (!mTransactionInProgress)
//      return NPERR_GENERIC_ERROR;

    NPError rc;

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT));
    if (rc == NPERR_NO_ERROR)
        mTransactionInProgress = false;
    return rc;
}

NPError
ConnectionObject::RollbackTransaction()
{
    AutoLock autoLock(mTransactionMutex);
//    if (!mTransactionInProgress)
//        return NPERR_GENERIC_ERROR; //NS_ERROR_FAILURE;

    NPError rc;

    if (!m_hdbc) {
      NPN_SetException(this, "Connection isn't initialized");
      return NPERR_GENERIC_ERROR;
    }

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK));
    if (rc == NPERR_NO_ERROR)
        mTransactionInProgress = false;
    return rc;
}


