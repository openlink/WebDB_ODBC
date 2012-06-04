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
#include <math.h>


#include "plugin.h"
#include "npfunctions.h"

#include "utils.h"
#include "mthreads.h"
#include "Statement.h"



//=========================================================================
//=========================================================================

StatementObject::StatementObject(NPP npp)
    : ScriptablePluginObjectBase(npp)
  {
    mc_Init_id = NPN_GetStringIdentifier("Init");
    mc_AddParameter_id = NPN_GetStringIdentifier("AddParameter");
    mc_Execute_id = NPN_GetStringIdentifier("Execute");
    mc_Close_id = NPN_GetStringIdentifier("Close");
    mc_Fetch_id = NPN_GetStringIdentifier("Fetch");
    mc_MoreResults_id = NPN_GetStringIdentifier("MoreResults");
    mc_GetColumnName_id = NPN_GetStringIdentifier("GetColumnName");
    mc_GetVariant_id = NPN_GetStringIdentifier("GetVariant");
    mc_GetColumnType_id = NPN_GetStringIdentifier("GetColumnType");
    mc_IsColumnNullable_id = NPN_GetStringIdentifier("IsColumnNullable");


    mc_GetTables_id = NPN_GetStringIdentifier("getTables");
    mc_GetColumns_id = NPN_GetStringIdentifier("getColumns");
    mc_GetTypeInfo_id = NPN_GetStringIdentifier("getTypeInfo");
    mc_GetPrimaryKeys_id = NPN_GetStringIdentifier("getPrimaryKeys");
    mc_GetForeignKeys_id = NPN_GetStringIdentifier("getForeignKeys");
    mc_GetProcedures_id = NPN_GetStringIdentifier("getProcedures");
    mc_GetProcedureColumns_id = NPN_GetStringIdentifier("getProcedureColumns");


    mc_errorCode_id = NPN_GetStringIdentifier("errorCode");
    mc_errorMessage_id = NPN_GetStringIdentifier("errorMessage");
    mc_errorState_id = NPN_GetStringIdentifier("errorState");
    mc_rowCount_id = NPN_GetStringIdentifier("rowCount");   
    mc_columnCount_id = NPN_GetStringIdentifier("columnCount");

    
    m_errCode = 0;
    m_errMessage[0] = 0;
    m_errState[0] = 0;
    m_columnCount = 0;
    m_rowCount = 0;

    m_hdbc = NULL;
    m_hstmt = NULL;
    m_conn = NULL;
  }


StatementObject::~StatementObject()
{
  ClearParams();

  if (m_hstmt != NULL)
    SQLFreeHandle (SQL_HANDLE_STMT, m_hstmt);

  if (m_conn) {
    NPN_ReleaseObject(m_conn);
    m_conn = NULL;
  }

}


bool
StatementObject::HasMethod(NPIdentifier name)
{
//sprintf(tmp, "stmt HasMethod [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_Init_id
   || name == mc_AddParameter_id
   || name == mc_Execute_id
   || name == mc_Close_id
   || name == mc_Fetch_id
   || name == mc_MoreResults_id
   || name == mc_GetColumnName_id
   || name == mc_GetVariant_id
   || name == mc_GetColumnType_id
   || name == mc_IsColumnNullable_id
   || name == mc_GetTables_id
   || name == mc_GetColumns_id
   || name == mc_GetTypeInfo_id
   || name == mc_GetPrimaryKeys_id
   || name == mc_GetForeignKeys_id
   || name == mc_GetProcedures_id
   || name == mc_GetProcedureColumns_id)
    return true;
  else
    return false;
}

bool
StatementObject::HasProperty(NPIdentifier name)
{
//sprintf(tmp, "stmt HasProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_errorCode_id
    || name == mc_errorMessage_id
    || name == mc_errorState_id
    || name == mc_rowCount_id
    || name == mc_columnCount_id )
    return true;
  else
    return false;
}

bool
StatementObject::GetProperty(NPIdentifier name, NPVariant *result)
{
//sprintf(tmp, "stmt GetProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  VOID_TO_NPVARIANT(*result);

  if (name == mc_rowCount_id)
    {
      INT32_TO_NPVARIANT(m_rowCount, *result);
      return true;
    }
  else if (name == mc_columnCount_id)
    {
      INT32_TO_NPVARIANT(m_columnCount, *result);
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
  return false;

}


bool
StatementObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
//sprintf(tmp, "stmt Invoke [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  NPError rc;
  VOID_TO_NPVARIANT(*result);
  int index = 0;

  if (name == mc_Init_id) {

    if (argCount < 2) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
#if 0
    if (!(NPVARIANT_IS_INT32(args[0]) && NPVARIANT_IS_OBJECT(args[1]))) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    SQLHANDLE hdbc = (SQLHANDLE)(long)NPVARIANT_TO_INT32(args[0]);
#else
    if (!(NPVARIANT_IS_OBJECT(args[0]) && NPVARIANT_IS_OBJECT(args[1]))) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    SQLHANDLE hdbc = (SQLHANDLE)NPVARIANT_TO_OBJECT(args[0]);
#endif

    Init(hdbc, NPVARIANT_TO_OBJECT(args[1]));

    return true;

  } else if (name == mc_AddParameter_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    AddParam(args);
    return true;

  } else if (name == mc_Execute_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!NPVARIANT_IS_STRING(args[0])) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    Execute((NPString*)&NPVARIANT_TO_STRING(args[0]));
    return true;

  } else if (name == mc_Close_id) {
    Close();
    return true;

  } else if (name == mc_Fetch_id) {
    bool ret;
    rc = Fetch(&ret);
    if (rc != NPERR_NO_ERROR)
      return true;
    BOOLEAN_TO_NPVARIANT(ret, *result);
    return true;

  } else if (name == mc_MoreResults_id) {
    bool ret;
    rc = MoreResults(&ret);
    if (rc != NPERR_NO_ERROR)
      return true;
    BOOLEAN_TO_NPVARIANT(ret, *result);
    return true;

  } else if (name == mc_GetColumnName_id) {
    const char *ret;
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    if (NPVARIANT_IS_INT32(args[0]))
      index = NPVARIANT_TO_INT32(args[0]);
    else if (NPVARIANT_IS_DOUBLE(args[0]))
      index = (int)NPVARIANT_TO_DOUBLE(args[0]);
    else {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    rc = GetColumnName(index, &ret);
    if (rc != NPERR_NO_ERROR)
      return true;
    STRING_TO_NPVARIANT(ret, *result);
    return true;

  } else if (name == mc_GetVariant_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    if (NPVARIANT_IS_INT32(args[0]))
      index = NPVARIANT_TO_INT32(args[0]);
    else if (NPVARIANT_IS_DOUBLE(args[0]))
      index = (int)NPVARIANT_TO_DOUBLE(args[0]);
    else {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    GetVariant(index, result);
    return true;

  } else if (name == mc_GetColumnType_id) {
    int ret;
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    if (NPVARIANT_IS_INT32(args[0]))
      index = NPVARIANT_TO_INT32(args[0]);
    else if (NPVARIANT_IS_DOUBLE(args[0]))
      index = (int)NPVARIANT_TO_DOUBLE(args[0]);
    else {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    rc = GetColumnType(index, &ret);
    if (rc != NPERR_NO_ERROR)
      return true;
    INT32_TO_NPVARIANT(ret, *result);
    return true;

  } else if (name == mc_IsColumnNullable_id) {
    bool ret;
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    if (NPVARIANT_IS_INT32(args[0]))
      index = NPVARIANT_TO_INT32(args[0]);
    else if (NPVARIANT_IS_DOUBLE(args[0]))
      index = (int)NPVARIANT_TO_DOUBLE(args[0]);
    else {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    rc = IsColumnNullable(index, &ret);
    if (rc != NPERR_NO_ERROR)
      return true;
    BOOLEAN_TO_NPVARIANT(ret, *result);
    return true;

  } else if (name == mc_GetTables_id) {
    if (argCount < 4) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[3]) || NPVARIANT_IS_STRING(args[3]))) {
      NPN_SetException(this, "Wrong 4 argument type");
      return true;
    }

    GetTables(&args[0], &args[1], &args[2], &args[3]);
    return true;

  } else if (name == mc_GetColumns_id) {
    if (argCount < 4) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[3]) || NPVARIANT_IS_STRING(args[3]))) {
      NPN_SetException(this, "Wrong 4 argument type");
      return true;
    }

    GetColumns(&args[0], &args[1], &args[2], &args[3]);
    return true;

  } else if (name == mc_GetTypeInfo_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }

    if (NPVARIANT_IS_INT32(args[0]))
      index = NPVARIANT_TO_INT32(args[0]);
    else if (NPVARIANT_IS_DOUBLE(args[0]))
      index = (int)NPVARIANT_TO_DOUBLE(args[0]);
    else {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    GetTypeInfo(index);
    return true;

  } else if (name == mc_GetPrimaryKeys_id) {
    if (argCount < 3) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }

    GetPrimaryKeys(&args[0], &args[1], &args[2]);
    return true;

  } else if (name == mc_GetForeignKeys_id) {
    if (argCount < 6) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[3]) || NPVARIANT_IS_STRING(args[3]))) {
      NPN_SetException(this, "Wrong 4 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[4]) || NPVARIANT_IS_STRING(args[4]))) {
      NPN_SetException(this, "Wrong 5 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[5]) || NPVARIANT_IS_STRING(args[5]))) {
      NPN_SetException(this, "Wrong 6 argument type");
      return true;
    }

    GetForeignKeys(&args[0], &args[1], &args[2], &args[3], &args[4], &args[5]);
    return true;

  } else if (name == mc_GetProcedures_id) {
    if (argCount < 3) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }

    GetProcedures(&args[0], &args[1], &args[2]);
    return true;

  } else if (name == mc_GetProcedureColumns_id) {
    if (argCount < 4) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[0]) || NPVARIANT_IS_STRING(args[0]))) {
      NPN_SetException(this, "Wrong 1 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[1]) || NPVARIANT_IS_STRING(args[1]))) {
      NPN_SetException(this, "Wrong 2 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[2]) || NPVARIANT_IS_STRING(args[2]))) {
      NPN_SetException(this, "Wrong 3 argument type");
      return true;
    }
    if (!(NPVARIANT_IS_NULL(args[3]) || NPVARIANT_IS_STRING(args[3]))) {
      NPN_SetException(this, "Wrong 4 argument type");
      return true;
    }

    GetProcedureColumns(&args[0], &args[1], &args[2], &args[3]);
    return true;
  
  }  

  return false;
}

//===========================================================================
//===========================================================================



void 
StatementObject::StoreError(SWORD fHandleType, SQLHANDLE handle)
{	
  SQLWCHAR szErrState[SQL_SQLSTATE_SIZE+1];    // SQL Error State string
  SQLINTEGER  dwErrCode;				 // Native Error code
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
StatementObject::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
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
StatementObject::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}

NPError
StatementObject::CheckStmtRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_STMT, m_hstmt, rc);
}


NPError 
StatementObject::ClearParams()
{
  VecODBCParams::iterator i;
  nsODBCParam* pRec;

  if (!m_params.empty()) {
    for(i = m_params.begin(); i != m_params.end(); i++)
    {
      pRec = *i;
      if (pRec)
        delete(pRec);
    }
    m_params.clear();
  }
  return NPERR_NO_ERROR;
}


NPError 
StatementObject::AddParam(const NPVariant *value)
{
  nsODBCParam* pRec = NULL;

  if (NPVARIANT_IS_VOID(*value) || NPVARIANT_IS_NULL(*value)) {
    pRec = new nsODBCParam();

  } else if (NPVARIANT_IS_BOOLEAN(*value)) {
    pRec = new nsODBCParam(NPVARIANT_TO_BOOLEAN(*value));

  } else if (NPVARIANT_IS_INT32(*value)) {
    pRec = new nsODBCParam(NPVARIANT_TO_INT32(*value));

  } else if (NPVARIANT_IS_DOUBLE(*value)) {
    double data = NPVARIANT_TO_DOUBLE(*value);
    double delta = ceil(data);

    if (data-0.000000001 <delta  && data+0.000000001 > delta)
        pRec = new nsODBCParam((int)data);
    else
        pRec = new nsODBCParam(data);
    //--pRec = new nsODBCParam(NPVARIANT_TO_DOUBLE(*value));

  } else if (NPVARIANT_IS_STRING(*value)) {
    pRec = new nsODBCParam((NPString*)&NPVARIANT_TO_STRING(*value));

  } else {
    NPN_SetException(this, "Unsupported parameter type");
    return NPERR_GENERIC_ERROR;
  }

  if (pRec)
    m_params.push_back(pRec);
  return NPERR_NO_ERROR;
}


NPError
StatementObject::Init(SQLHANDLE dbc, NPObject *conn)
{
  m_hdbc = dbc;

  SQLHANDLE hstmt;
  NPError rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));

  if (rc != NPERR_NO_ERROR)
    return rc;

  m_hstmt = hstmt;
  m_conn = conn;
  NPN_RetainObject(conn);
  return NPERR_NO_ERROR;
}


NPError
StatementObject::Close()
{
  if (!m_hstmt)
    return NPERR_NO_ERROR;

  if (m_hstmt) {
    if (m_columnCount > 0)
      SQLCloseCursor (m_hstmt);
    m_columnCount = 0;
    m_rowCount = 0;
  }

  return NPERR_NO_ERROR;
}


NPError
StatementObject::Execute(NPString *query)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return rc;

    /**********/
  int i = 0;
  VecODBCParams::iterator p;

  if (!m_params.empty()) {
    for(p = m_params.begin(); p < m_params.end(); p++) {
      nsODBCParam *val = *p;

      if (val == NULL) {
        return NPERR_GENERIC_ERROR;
      }

      i++;
      switch(val->mType) {
        case nsODBCParam::VTYPE_BOOL :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &(val->mVal.mBoolValue), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != NPERR_NO_ERROR)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_INT32 :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_SLONG, SQL_INTEGER, sizeof(int), 0, &(val->mVal.mInt32Value), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != NPERR_NO_ERROR)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_DOUBLE :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_DOUBLE, SQL_DOUBLE, sizeof(double), 0, &(val->mVal.mDoubleValue),
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != NPERR_NO_ERROR)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_STRING :
        {
          nsWString *v_str = val->mVal.mStringValue;
          val->mInd = SQL_NTS;
    	  rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
    	     SQL_C_WCHAR, (val->mIsLong?SQL_WLONGVARCHAR:SQL_WVARCHAR), 
    	     v_str->Length(), 0, v_str->Data(), 0, (SQLLEN *)&(val->mInd)));
    	  if (rc != NPERR_NO_ERROR)
    	    return rc;
    	  break;
        }

#if 0
      case nsIDataType::VTYPE_ARRAY :
      {
          nsBinary *v_bin = val->mVal.mBinValue;
          val->mInd = v_bin->Length();
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_BINARY, (val->mIsLong?SQL_LONGVARBINARY:SQL_VARBINARY), 
             v_bin->Length(), 0, v_bin->Data(), 0, (SQLINTEGER *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
      }
#endif

        default:
        {
          val->mInd = SQL_NULL_DATA;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_CHAR, SQL_VARCHAR, 0, 0, NULL, 0, (SQLLEN *)&(val->mInd)));
    	  if (rc != NPERR_NO_ERROR)
    	    return rc;
   	  break;
        }
      }
    }
  }
    /**********/

  SQLRETURN code = SQLExecDirectW (m_hstmt, nsWString(query).Data(), SQL_NTS);
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    ClearParams();
    return NPERR_GENERIC_ERROR;

  } 
  SQLFreeStmt (m_hstmt, SQL_RESET_PARAMS);
  ClearParams();

    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
    
  return NPERR_NO_ERROR;
}


//===============================================
NPError 
StatementObject::Fetch(bool *retval)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  SQLRETURN rc = SQLFetch(m_hstmt);
  if (SQL_SUCCEEDED(rc)) {

    *retval = true;
    return NPERR_NO_ERROR;

  } else if (rc == SQL_NO_DATA) {

    *retval = false;
    return NPERR_NO_ERROR;

  } else {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return NPERR_GENERIC_ERROR;
  }
}


NPError 
StatementObject::MoreResults(bool *retval)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  SQLRETURN rc = SQLMoreResults(m_hstmt);
  if (SQL_SUCCEEDED(rc)) {

    SQLSMALLINT cols;
    if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
      m_columnCount = cols;

    if (m_columnCount > 0)
      *retval = true;
    else
      *retval = false;
    return NPERR_NO_ERROR;

  } else if (rc == SQL_NO_DATA) {
    *retval = false;
    return NPERR_NO_ERROR;

  } else {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return NPERR_GENERIC_ERROR;
  }
}
   

NPError 
StatementObject::GetColumnName(int index, const char **retval)
{
  NPError rc;
  SQLWCHAR colName[512];

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  if (index < 0 || index >= m_columnCount) {
    NPN_SetException(this, "Column index is out of range");
    return NPERR_GENERIC_ERROR;
  }

  index++;
  rc = CheckStmtRC(SQLDescribeColW (m_hstmt, index, colName, 
  	   sizeof(colName)/sizeof(wchar_t), 
           NULL, NULL, NULL, NULL, NULL));
  if (rc == NPERR_NO_ERROR)
     *retval = strdup_WtoU8((const wchar_t *)colName);
  return rc;
}


NPError 
StatementObject::GetVariant(int index, NPVariant *value)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  if (index < 0 || index >= m_columnCount) {
    NPN_SetException(this, "Column index is out of range");
    return NPERR_GENERIC_ERROR;
  }

  SQLLEN cbInd;
  int type;
  NPError rc;

  GetColumnType(index, &type);
  index++;


  NULL_TO_NPVARIANT(*value);

  switch (type) {
    case VALUE_TYPE_BIT:
      {
        bool v_bool;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_BIT, &v_bool, 0, &cbInd));
        if (rc != NPERR_NO_ERROR)
          return rc;
        if (cbInd != SQL_NULL_DATA) 
          BOOLEAN_TO_NPVARIANT(v_bool, *value);
        break;
      }
    case VALUE_TYPE_TINYINT:
    case VALUE_TYPE_SMALLINT:
    case VALUE_TYPE_BIGINT:
    case VALUE_TYPE_INTEGER:
      {
        SQLINTEGER v_int32;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_LONG, &v_int32, 0, &cbInd));
        if (rc != NPERR_NO_ERROR)
          return rc;
        if (cbInd != SQL_NULL_DATA)
          INT32_TO_NPVARIANT(v_int32, *value);
        break;
      }

    case VALUE_TYPE_FLOAT:
    case VALUE_TYPE_REAL:
    case VALUE_TYPE_DOUBLE:
      {
        double v_double;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_DOUBLE, &v_double, 0, &cbInd));
        if (rc != NPERR_NO_ERROR)
          return rc;
        if (cbInd != SQL_NULL_DATA)
          DOUBLE_TO_NPVARIANT(v_double, *value);
        break;
      }

/**
    case VALUE_TYPE_BINARY:
    case VALUE_TYPE_VARBINARY:
    case VALUE_TYPE_LONGVARBINARY:
      {
        const char hexString[] = "0123456789ABCDEF";
        SQLLEN len;
        SQLWCHAR buf[32];
        unsigned char *pin;
        char *pout;

        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_BINARY, buf, 0, &len));
        if (rc != NPERR_NO_ERROR)
          return rc;

        if (len == SQL_NO_TOTAL) 
          {
            const SQLLEN dwUnit = 8192;
            unsigned char *pBuff;
            unsigned char *pData = NULL;
            SQLLEN dwAvialable;
            SQLLEN bufSize = dwUnit;
            char *data = NULL;

            pData = (unsigned char *)NPN_MemAlloc(bufSize);
            if (!pData) {
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }

            len = 0;
            pBuff = pData;
            while(1) {
              SQLRETURN ret;
              pBuff = pData + len;
              ret = SQLGetData(m_hstmt, index, SQL_C_BINARY, pBuff, dwUnit, &dwAvialable);
              if (ret == SQL_NO_DATA_FOUND)
                break;
              if ((rc = CheckStmtRC(ret)) != NPERR_NO_ERROR)
                return rc;
              if (dwAvialable < dwUnit) {
                len += dwAvialable;
                break;
              }
              len += dwUnit;
              bufSize += dwUnit;
              unsigned char* tmp = (unsigned char*)NPN_MemAlloc(bufSize);
              if (!pBuff) {
                NPN_SetException(this, "Memory allocation error");
                return NPERR_GENERIC_ERROR;
              }
              memcpy(pBuff, pData, len);
              NPN_MemFree(pData);
              pData = pBuff;
              pBuff = pData + len;
            }

            data = (char*)NPN_MemAlloc(len*2+1);
            if (!data) {
              NPN_MemFree(pData);
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }
            pout = data;
            pin = pData;
            while(len--) {
              unsigned char binVal = *pin++;
              *pout++ = hexString[(binVal >> 4) & 0xf];
              *pout++ = hexString[binVal & 0xf];
            }
            *pout = 0;
            NPN_MemFree(pData);
            STRING_TO_NPVARIANT(data, *value);

          }
          else if (len != SQL_NULL_DATA) 
          {
            unsigned char *pData;
            SQLLEN bufSize = len + 1;
            char *data = NULL;

            pData = (unsigned char *)NPN_MemAlloc(bufSize);
            if (!pData) {
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }

            rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_BINARY, pData, bufSize, &len));
            if (rc != NPERR_NO_ERROR)
              return rc;

            data = (char*)NPN_MemAlloc(len*2+1);
            if (!data) {
              NPN_MemFree(pData);
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }
            pout = data;
            pin = pData;
            while(len--) {
              unsigned char binVal = *pin++;
              *pout++ = hexString[(binVal >> 4) & 0xf];
              *pout++ = hexString[binVal & 0xf];
            }
            *pout = 0;
            NPN_MemFree(pData);
            STRING_TO_NPVARIANT(data, *value);
          }

        break;
      }
**/

    case VALUE_TYPE_BINARY:
    case VALUE_TYPE_VARBINARY:
    case VALUE_TYPE_LONGVARBINARY:

    case VALUE_TYPE_GUID:

    case VALUE_TYPE_CHAR:
    case VALUE_TYPE_VARCHAR:
    case VALUE_TYPE_LONGVARCHAR:
    case VALUE_TYPE_WCHAR:
    case VALUE_TYPE_WVARCHAR:
    case VALUE_TYPE_WLONGVARCHAR:

    case VALUE_TYPE_UNKNOWN:

    case VALUE_TYPE_NUMERIC:
    case VALUE_TYPE_DECIMAL:

    case VALUE_TYPE_DATE:
    case VALUE_TYPE_TIME:
    case VALUE_TYPE_TIMESTAMP:
    default:
      {
        SQLLEN len;
        SQLLEN dtype;
        SQLWCHAR buf[32];

        rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, SQL_COLUMN_TYPE, NULL, 0, NULL, &dtype));
        if (rc != NPERR_NO_ERROR)
          return rc;

        if (dtype == SQL_LONGVARBINARY || dtype == SQL_VARBINARY || dtype == SQL_BINARY ||
            dtype == SQL_LONGVARCHAR || dtype == SQL_VARCHAR || dtype == SQL_CHAR ||
            dtype == SQL_WLONGVARCHAR || dtype == SQL_WVARCHAR || dtype == SQL_WCHAR)
        {
          rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, buf, 0, &len));
          if (rc != NPERR_NO_ERROR)
            return rc;

          if (len == SQL_NO_TOTAL) 
          {
            const SQLLEN dwUnit = 4096*sizeof(SQLWCHAR)+sizeof(SQLWCHAR);
            unsigned char *pBuff = NULL;
            unsigned char *pData = NULL;
            SQLLEN dwAvialable;
            SQLLEN bufSize = dwUnit;
            char *data = NULL;
            SQLRETURN ret;

            pData = (unsigned char *)NPN_MemAlloc(bufSize);
            if (!pData) {
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }

            len = 0;
            while(1) {
              pBuff = pData + len;
              ret = SQLGetData(m_hstmt, index, SQL_C_WCHAR, pBuff, dwUnit, &dwAvialable);
              if (ret == SQL_NO_DATA_FOUND)
                break;
              if ((rc = CheckStmtRC(ret)) != NPERR_NO_ERROR)
                return rc;
              if (dwAvialable < dwUnit) {
                len += dwAvialable;
                break;
              }
              len += dwUnit - sizeof(SQLWCHAR);
              bufSize += dwUnit - sizeof(SQLWCHAR);
              unsigned char* tmp = (unsigned char*)NPN_MemAlloc(bufSize);
              if (!tmp) {
                NPN_SetException(this, "Memory allocation error");
                return NPERR_GENERIC_ERROR;
              }
              memcpy(tmp, pData, len);
              NPN_MemFree(pData);
              pData = tmp;
            }

            data = strdup_WtoU8((SQLWCHAR *)pData);
            if (!data) {
              NPN_MemFree(pData);
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }
            NPN_MemFree(pData);
            STRING_TO_NPVARIANT(data, *value);

          }
          else if (len != SQL_NULL_DATA) 
          {
            unsigned char *pData;
            SQLLEN bufSize = len + sizeof(SQLWCHAR);
            char *data = NULL;

            pData = (unsigned char *)NPN_MemAlloc(bufSize);
            if (!pData) {
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }

            rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, pData, bufSize, &len));
            if (rc != NPERR_NO_ERROR)
              return rc;

            data = strdup_WtoU8((SQLWCHAR *)pData);
            if (!data) {
              NPN_MemFree(pData);
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }
            NPN_MemFree(pData);
            STRING_TO_NPVARIANT(data, *value);
          }
        } else {
          rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, buf, sizeof(buf), &len));
          if (rc != NPERR_NO_ERROR)
            return rc;
          if (len != SQL_NULL_DATA) {
            const char *data = strdup_WtoU8(buf);
            if (!data) {
              NPN_SetException(this, "Memory allocation error");
              return NPERR_GENERIC_ERROR;
            }
            STRING_TO_NPVARIANT(data, *value);
          }
        }  
        break;
      }
    }

  return NPERR_NO_ERROR;
}


NPError 
StatementObject::GetColumnType(int index, int *retval)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  if (index < 0 || index >= m_columnCount) {
    NPN_SetException(this, "Column index is out of range");
    return NPERR_GENERIC_ERROR;
  }

  index++;

  SQLLEN t;
  NPError rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, 
    	SQL_COLUMN_TYPE, NULL, 0, NULL, &t));
  if (rc != NPERR_NO_ERROR)
      return rc;

  switch(t) {
    case SQL_CHAR:
      *retval = VALUE_TYPE_CHAR;
      break;
    case SQL_NUMERIC:
      *retval = VALUE_TYPE_NUMERIC;
      break;
    case SQL_DECIMAL:
      *retval = VALUE_TYPE_DECIMAL;
      break;
    case SQL_INTEGER:
      *retval = VALUE_TYPE_INTEGER;
      break;
    case SQL_SMALLINT:
      *retval = VALUE_TYPE_SMALLINT;
      break;
    case SQL_FLOAT:
      *retval = VALUE_TYPE_FLOAT;
      break;
    case SQL_REAL:
      *retval = VALUE_TYPE_REAL;
      break;
    case SQL_DOUBLE:
      *retval = VALUE_TYPE_DOUBLE;
      break;
    case SQL_DATE:
    case SQL_TYPE_DATE:
      *retval = VALUE_TYPE_DATE;
      break;
    case SQL_TIME:
    case SQL_TYPE_TIME:
      *retval = VALUE_TYPE_TIME;
      break;
    case SQL_TIMESTAMP:
    case SQL_TYPE_TIMESTAMP:
      *retval = VALUE_TYPE_TIMESTAMP;
      break;
    case SQL_VARCHAR:
      *retval = VALUE_TYPE_VARCHAR;
      break;
    case SQL_LONGVARCHAR:
      *retval = VALUE_TYPE_LONGVARCHAR;
      break;
    case SQL_BINARY:
      *retval = VALUE_TYPE_BINARY;
      break;
    case SQL_VARBINARY:
      *retval = VALUE_TYPE_VARBINARY;
      break;
    case SQL_LONGVARBINARY:
      *retval = VALUE_TYPE_LONGVARBINARY;
      break;
    case SQL_BIGINT:
      *retval = VALUE_TYPE_BIGINT;
      break;
    case SQL_TINYINT:
      *retval = VALUE_TYPE_TINYINT;
      break;
    case SQL_BIT:
      *retval = VALUE_TYPE_BIT;
      break;
    case SQL_WCHAR:
      *retval = VALUE_TYPE_WCHAR;
      break;
    case SQL_WVARCHAR:
      *retval = VALUE_TYPE_WVARCHAR;
      break;
    case SQL_WLONGVARCHAR:
      *retval = VALUE_TYPE_WLONGVARCHAR;
      break;
    case SQL_GUID:
      *retval = VALUE_TYPE_GUID;
      break;
    default:
      *retval = VALUE_TYPE_CHAR;
      break;
    }

  return rc;
}


NPError 
StatementObject::IsColumnNullable(int index, bool *retval)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return NPERR_GENERIC_ERROR;
  }

  if (index < 0 || index >= m_columnCount) {
    NPN_SetException(this, "Column index is out of range");
    return NPERR_GENERIC_ERROR;
  }

  index++;
  SQLLEN v;
  NPError rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, 
    	SQL_COLUMN_NULLABLE, NULL, 0, NULL, &v));
  if (rc != NPERR_NO_ERROR)
    return rc;

  *retval =(v != 0? true: false);
  return rc;

}


long
StatementObject::GetErrorCode()
{
    return m_errCode;
}


const char *
StatementObject::GetErrorString()
{
    return strdup_WtoU8(m_errMessage);
}


const char *
StatementObject::GetErrorState()
{
    return strdup_WtoU8(m_errState);
}


void
StatementObject::GetTables(const NPVariant *arg0, const NPVariant *arg1, 
	const NPVariant *arg2, const NPVariant *arg3)
{
  NPString *cat = NULL;
  NPString *sch = NULL; 
  NPString *tbl = NULL;
  NPString *typ = NULL;;

  if (NPVARIANT_IS_STRING(*arg0))
    cat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    sch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    tbl = (NPString*)&NPVARIANT_TO_STRING(*arg2);
  if (NPVARIANT_IS_STRING(*arg3))
    typ = (NPString*)&NPVARIANT_TO_STRING(*arg3);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLTablesW(m_hstmt, 
  	(cat?(nsWString(cat).Data()):NULL), (cat?SQL_NTS:0),
  	(sch?nsWString(sch).Data():NULL), (sch?SQL_NTS:0),
  	(tbl?nsWString(tbl).Data():NULL), (tbl?SQL_NTS:0),
  	(typ?nsWString(typ).Data():NULL), (typ?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetColumns(const NPVariant *arg0, const NPVariant *arg1, 
	const NPVariant *arg2, const NPVariant *arg3)
{
  NPString *cat = NULL;
  NPString *sch = NULL; 
  NPString *tbl = NULL;
  NPString *col = NULL;;

  if (NPVARIANT_IS_STRING(*arg0))
    cat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    sch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    tbl = (NPString*)&NPVARIANT_TO_STRING(*arg2);
  if (NPVARIANT_IS_STRING(*arg3))
    col = (NPString*)&NPVARIANT_TO_STRING(*arg3);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLColumnsW(m_hstmt, 
  	(cat?nsWString(cat).Data():NULL), (cat?SQL_NTS:0),
  	(sch?nsWString(sch).Data():NULL), (sch?SQL_NTS:0),
  	(tbl?nsWString(tbl).Data():NULL), (tbl?SQL_NTS:0),
  	(col?nsWString(col).Data():NULL), (col?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetTypeInfo(int dataType)
{
  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLGetTypeInfo(m_hstmt, dataType);
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetPrimaryKeys(const NPVariant *arg0, const NPVariant *arg1, 
		const NPVariant *arg2)
{
  NPString *cat = NULL;
  NPString *sch = NULL; 
  NPString *tbl = NULL;

  if (NPVARIANT_IS_STRING(*arg0))
    cat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    sch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    tbl = (NPString*)&NPVARIANT_TO_STRING(*arg2);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLPrimaryKeysW(m_hstmt, 
  	(cat?nsWString(cat).Data():NULL), (cat?SQL_NTS:0),
  	(sch?nsWString(sch).Data():NULL), (sch?SQL_NTS:0),
  	(tbl?nsWString(tbl).Data():NULL), (tbl?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetForeignKeys(const NPVariant *arg0, const NPVariant *arg1, 
	const NPVariant *arg2, const NPVariant *arg3, const NPVariant *arg4, 
	const NPVariant *arg5)
{
  NPString *pcat = NULL;
  NPString *psch = NULL; 
  NPString *ptbl = NULL;
  NPString *fcat = NULL;
  NPString *fsch = NULL; 
  NPString *ftbl = NULL;

  if (NPVARIANT_IS_STRING(*arg0))
    pcat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    psch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    ptbl = (NPString*)&NPVARIANT_TO_STRING(*arg2);

  if (NPVARIANT_IS_STRING(*arg3))
    fcat = (NPString*)&NPVARIANT_TO_STRING(*arg3);
  if (NPVARIANT_IS_STRING(*arg4))
    fsch = (NPString*)&NPVARIANT_TO_STRING(*arg4);
  if (NPVARIANT_IS_STRING(*arg5))
    ftbl = (NPString*)&NPVARIANT_TO_STRING(*arg5);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLForeignKeysW(m_hstmt, 
  	(pcat?nsWString(pcat).Data():NULL), (pcat?SQL_NTS:0),
  	(psch?nsWString(psch).Data():NULL), (psch?SQL_NTS:0),
  	(ptbl?nsWString(ptbl).Data():NULL), (ptbl?SQL_NTS:0),
  	(fcat?nsWString(fcat).Data():NULL), (fcat?SQL_NTS:0),
  	(fsch?nsWString(fsch).Data():NULL), (fsch?SQL_NTS:0),
  	(ftbl?nsWString(ftbl).Data():NULL), (ftbl?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetProcedures(const NPVariant *arg0, const NPVariant *arg1, 
	const NPVariant *arg2)
{
  NPString *cat = NULL;
  NPString *sch = NULL; 
  NPString *prc = NULL;

  if (NPVARIANT_IS_STRING(*arg0))
    cat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    sch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    prc = (NPString*)&NPVARIANT_TO_STRING(*arg2);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLProceduresW(m_hstmt, 
  	(cat?nsWString(cat).Data():NULL), (cat?SQL_NTS:0),
  	(sch?nsWString(sch).Data():NULL), (sch?SQL_NTS:0),
  	(prc?nsWString(prc).Data():NULL), (prc?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}


void
StatementObject::GetProcedureColumns(const NPVariant *arg0, const NPVariant *arg1, 
	const NPVariant *arg2, const NPVariant *arg3)
{
  NPString *cat = NULL;
  NPString *sch = NULL; 
  NPString *prc = NULL;
  NPString *col = NULL;

  if (NPVARIANT_IS_STRING(*arg0))
    cat = (NPString*)&NPVARIANT_TO_STRING(*arg0);
  if (NPVARIANT_IS_STRING(*arg1))
    sch = (NPString*)&NPVARIANT_TO_STRING(*arg1);
  if (NPVARIANT_IS_STRING(*arg2))
    prc = (NPString*)&NPVARIANT_TO_STRING(*arg2);
  if (NPVARIANT_IS_STRING(*arg3))
    col = (NPString*)&NPVARIANT_TO_STRING(*arg3);

  if (!m_hstmt) {
    NPN_SetException(this, "Statement isn't initialized");
    return;
  }

  NPError rc = Close();
  if (rc != NPERR_NO_ERROR)
    return;

  SQLRETURN code = SQLProcedureColumnsW(m_hstmt, 
  	(cat?nsWString(cat).Data():NULL), (cat?SQL_NTS:0),
  	(sch?nsWString(sch).Data():NULL), (sch?SQL_NTS:0),
  	(prc?nsWString(prc).Data():NULL), (prc?SQL_NTS:0),
  	(col?nsWString(col).Data():NULL), (col?SQL_NTS:0));
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return;
  } 
    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
}
