
#include "JSObject.h"
#include <boost/assign.hpp>
#include "DOM/Document.h"
#include "variant_list.h"

#include "Statement.h"
#include "Common.h"


Statement::Statement(const FB::BrowserHostPtr& host, SQLHANDLE hdbc, const FB::JSAPIWeakPtr conn) : m_host(host)
{

    registerMethod("AddParameter",  make_method(this, &Statement::AddParam ));
    registerMethod("Execute",  make_method(this, &Statement::Execute ));
    registerMethod("Close",  make_method(this, &Statement::Close ));
    registerMethod("Fetch",  make_method(this, &Statement::Fetch ));
    registerMethod("MoreResults",  make_method(this, &Statement::MoreResults ));
    registerMethod("GetColumnName",  make_method(this, &Statement::GetColumnName ));
    registerMethod("GetVariant",  make_method(this, &Statement::GetVariant ));
    registerMethod("GetColumnType",  make_method(this, &Statement::GetColumnType ));
    registerMethod("IsColumnNullable",  make_method(this, &Statement::IsColumnNullable ));
    registerMethod("getTables",  make_method(this, &Statement::GetTables ));
    registerMethod("getColumns",  make_method(this, &Statement::GetColumns ));
    registerMethod("getTypeInfo",  make_method(this, &Statement::GetTypeInfo ));
    registerMethod("getPrimaryKeys",  make_method(this, &Statement::GetPrimaryKeys ));
    registerMethod("getForeignKeys",  make_method(this, &Statement::GetForeignKeys ));
    registerMethod("getProcedures",  make_method(this, &Statement::GetProcedures ));
    registerMethod("getProcedureColumns",  make_method(this, &Statement::GetProcedureColumns ));


    registerProperty("errorCode",  make_property(this, 
    					&Statement::GetErrorCode));
    registerProperty("errorMessage", make_property(this, 
    					&Statement::GetErrorMessage));
    registerProperty("errorState", make_property(this, 
    					&Statement::GetErrorState));
    registerProperty("rowCount", make_property(this, 
    					&Statement::GetRowCount));
    registerProperty("columnCount", make_property(this, 
    					&Statement::GetColumnCount));

    m_conn = conn.lock();
    m_errCode = 0;
    m_columnCount = 0;
    m_rowCount = 0;

    m_hdbc = hdbc;
    m_hstmt = NULL;

    if (CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt))!=PE_OK)
      throw FB::script_error("Can not create new Statement");

}

Statement::~Statement()
{
  ClearParams();

  if (m_hstmt != NULL)
    SQLFreeHandle (SQL_HANDLE_STMT, m_hstmt);
}


void 
Statement::StoreError(SWORD fHandleType, SQLHANDLE handle)
{	
  SQLWCHAR szErrState[SQL_SQLSTATE_SIZE+1];    // SQL Error State string
  SQLINTEGER  dwErrCode;				 // Native Error code
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
Statement::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
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
Statement::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}

int
Statement::CheckStmtRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_STMT, m_hstmt, rc);
}


int
Statement::ClearParams()
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

  return PE_OK;
}


int 
Statement::AddParam(const FB::variant& value)
{
  nsODBCParam* pRec = NULL;

  if (value.is_null() || value.empty()) {
    pRec = new nsODBCParam();
  }
  else if (value.is_of_type<bool>()) {
    pRec = new nsODBCParam(value.convert_cast<bool>());
  }
  else if (value.is_of_type<int>()) {
    pRec = new nsODBCParam(value.convert_cast<int>());
  }
  else if (value.is_of_type<long>()) {
    pRec = new nsODBCParam(value.convert_cast<int>());
  }
  else if (value.is_of_type<double>()) {
    pRec = new nsODBCParam(value.convert_cast<int>());
    double data = value.convert_cast<double>();
    double delta = ceil(data);

    if (data-0.000000001 <delta  && data+0.000000001 > delta)
        pRec = new nsODBCParam((int)data);
    else
        pRec = new nsODBCParam(data);
  }
  else {
    pRec = new nsODBCParam(value.convert_cast<std::wstring>());
  }
/***
  else if (value.is_of_type<std::string>() || value.is_of_type<std::wstring>()) {
    pRec = new nsODBCParam(value.convert_cast<std::wstring>());
  }
  else {
    throw FB::script_error("Unsupported parameter type");
  }
***/

  if (pRec)
    m_params.push_back(pRec);

  return PE_OK;
}



int
Statement::Close()
{
  if (!m_hstmt)
    return PE_OK;

  if (m_hstmt) {
    if (m_columnCount > 0)
      SQLCloseCursor (m_hstmt);
    m_columnCount = 0;
    m_rowCount = 0;
  }

  return PE_OK;
}


int
Statement::Execute(const std::wstring& query)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return rc;

    /**********/
  int i = 0;
  VecODBCParams::iterator p;

  if (!m_params.empty()) {
    for(p = m_params.begin(); p < m_params.end(); p++) {
      nsODBCParam *val = *p;

      if (val == NULL) {
        return PE_ERROR;
      }

      i++;
      switch(val->mType) {
        case nsODBCParam::VTYPE_BOOL :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_BIT, SQL_BIT, sizeof(bool), 0, &(val->mVal.mBoolValue), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != PE_OK)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_INT32 :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_SLONG, SQL_INTEGER, sizeof(int), 0, &(val->mVal.mInt32Value), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != PE_OK)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_DOUBLE :
        {
          val->mInd = 0;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_DOUBLE, SQL_DOUBLE, sizeof(double), 0, &(val->mVal.mDoubleValue),
             0, (SQLLEN *)&(val->mInd)));
    	  if (rc != PE_OK)
    	    return rc;
    	  break;
        }

        case nsODBCParam::VTYPE_STRING :
        {
          SQLWCHAR *pData = (SQLWCHAR*)val->mVal.mStringValue->c_str();
          SQLULEN Len = wcslen(pData);
          val->mInd = SQL_NTS;
    	  rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
    	     SQL_C_WCHAR, (val->mIsLong?SQL_WLONGVARCHAR:SQL_WVARCHAR), 
    	     Len, 0, pData, 0, (SQLLEN *)&(val->mInd)));
    	  if (rc != PE_OK)
    	    return rc;
    	  break;
        }

        default:
        {
          val->mInd = SQL_NULL_DATA;
          rc = CheckStmtRC(SQLBindParameter(m_hstmt, i, SQL_PARAM_INPUT, 
             SQL_C_CHAR, SQL_VARCHAR, 0, 0, NULL, 0, (SQLLEN *)&(val->mInd)));
    	  if (rc != PE_OK)
    	    return rc;
   	  break;
        }
      }
    }
  }
    /**********/

  SQLRETURN code = SQLExecDirectW (m_hstmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
  if (!SQL_SUCCEEDED(code) && code != SQL_NO_DATA) {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    ClearParams();
    return PE_ERROR;

  } 
  SQLFreeStmt (m_hstmt, SQL_RESET_PARAMS);
  ClearParams();

    
  SQLSMALLINT cols;
  if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
    m_columnCount = cols;

  SQLLEN rcnt;
  if (SQL_SUCCEEDED(SQLRowCount(m_hstmt, &rcnt)))
    m_rowCount = rcnt;
    
  return PE_OK;
}


//===============================================
bool 
Statement::Fetch()
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  SQLRETURN rc = SQLFetch(m_hstmt);
  if (SQL_SUCCEEDED(rc)) {

    return true;

  } else if (rc == SQL_NO_DATA) {

    return false;

  } else {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return false;
  }
}


bool 
Statement::MoreResults()
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  SQLRETURN rc = SQLMoreResults(m_hstmt);
  if (SQL_SUCCEEDED(rc)) {

    SQLSMALLINT cols;
    if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &cols)))
      m_columnCount = cols;

    if (m_columnCount > 0)
      return true;
    else
      return false;

  } else if (rc == SQL_NO_DATA) {
    return false;

  } else {
    StoreError(SQL_HANDLE_STMT, m_hstmt);  
    return false;
  }
}
   

std::wstring 
Statement::GetColumnName(int index)
{
  int rc;
  SQLWCHAR colName[512] = {L""};

  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  if (index < 0 || index >= m_columnCount)
    throw FB::script_error("Column index is out of range");

  index++;
  rc = CheckStmtRC(SQLDescribeColW (m_hstmt, index, colName, 
  	   sizeof(colName)/sizeof(wchar_t), 
           NULL, NULL, NULL, NULL, NULL));
  return colName;
}


FB::variant 
Statement::GetVariant(int index)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  if (index < 0 || index >= m_columnCount)
    throw FB::script_error("Column index is out of range");

  SQLLEN cbInd;
  int type;
  int rc;

  type = GetColumnType(index);
  index++;

  switch (type) {
    case VALUE_TYPE_BIT:
      {
        bool v_bool;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_BIT, &v_bool, 0, &cbInd));
        if (rc != PE_OK)
          return rc;
        if (cbInd != SQL_NULL_DATA) 
          return v_bool;
        break;
      }
    case VALUE_TYPE_TINYINT:
    case VALUE_TYPE_SMALLINT:
    case VALUE_TYPE_BIGINT:
    case VALUE_TYPE_INTEGER:
      {
        SQLINTEGER v_int32;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_LONG, &v_int32, 0, &cbInd));
        if (rc != PE_OK)
          return rc;
        if (cbInd != SQL_NULL_DATA)
          return v_int32;
        break;
      }

    case VALUE_TYPE_FLOAT:
    case VALUE_TYPE_REAL:
    case VALUE_TYPE_DOUBLE:
      {
        double v_double;
        rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_DOUBLE, &v_double, 0, &cbInd));
        if (rc != PE_OK)
          return rc;
        if (cbInd != SQL_NULL_DATA)
          return v_double;
        break;
      }


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
		std::wstring v_str;

        rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, SQL_COLUMN_TYPE, NULL, 0, NULL, &dtype));
        if (rc != PE_OK)
          return rc;

        if (dtype == SQL_LONGVARBINARY || dtype == SQL_VARBINARY || dtype == SQL_BINARY ||
            dtype == SQL_LONGVARCHAR || dtype == SQL_VARCHAR || dtype == SQL_CHAR ||
            dtype == SQL_WLONGVARCHAR || dtype == SQL_WVARCHAR || dtype == SQL_WCHAR)
        {
          rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, buf, 0, &len));
          if (rc != PE_OK)
            return rc;

          if (len == SQL_NO_TOTAL) 
          {
            const SQLLEN dwUnit = 4096*sizeof(SQLWCHAR)+sizeof(SQLWCHAR);
            unsigned char pBuff[dwUnit];
            SQLLEN dwAvialable;
            SQLRETURN ret;

            while(1) {
              ret = SQLGetData(m_hstmt, index, SQL_C_WCHAR, &pBuff, dwUnit, &dwAvialable);
              if (ret == SQL_NO_DATA_FOUND)
                break;
              if ((rc = CheckStmtRC(ret)) != PE_OK)
                return rc;
			  
              v_str.append((wchar_t *)pBuff);

              if (dwAvialable < dwUnit)
                break;
            }
            return v_str;

          }
          else if (len != SQL_NULL_DATA) 
          {
            unsigned char *pData;
            SQLLEN bufSize = len + sizeof(SQLWCHAR);
            char *data = NULL;

            pData = (unsigned char *)malloc(bufSize);
            if (!pData)
              throw FB::script_error("Memory allocation error");

            rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, pData, bufSize, &len));
            if (rc != PE_OK)
              return rc;

            v_str.append((const wchar_t*)pData);
            free(pData);
            return v_str;
          }
        } else {
          rc = CheckStmtRC(SQLGetData(m_hstmt, index, SQL_C_WCHAR, buf, sizeof(buf), &len));
          if (rc != PE_OK)
            return rc;
          if (len != SQL_NULL_DATA) {
            v_str.append(buf);
            return v_str;
          }
        }  
        break;
      }
    }
    return FB::FBNull();
}


int 
Statement::GetColumnType(int index)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  if (index < 0 || index >= m_columnCount)
    throw FB::script_error("Column index is out of range");

  index++;

  SQLLEN t;
  int rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, 
    	SQL_COLUMN_TYPE, NULL, 0, NULL, &t));
  if (rc != PE_OK)
      return VALUE_TYPE_UNKNOWN;

  switch(t) {
    case SQL_CHAR:
      return VALUE_TYPE_CHAR;
    case SQL_NUMERIC:
      return VALUE_TYPE_NUMERIC;
    case SQL_DECIMAL:
      return VALUE_TYPE_DECIMAL;
    case SQL_INTEGER:
      return VALUE_TYPE_INTEGER;
    case SQL_SMALLINT:
      return VALUE_TYPE_SMALLINT;
    case SQL_FLOAT:
      return VALUE_TYPE_FLOAT;
    case SQL_REAL:
      return VALUE_TYPE_REAL;
    case SQL_DOUBLE:
      return VALUE_TYPE_DOUBLE;
    case SQL_DATE:
    case SQL_TYPE_DATE:
      return VALUE_TYPE_DATE;
    case SQL_TIME:
    case SQL_TYPE_TIME:
      return VALUE_TYPE_TIME;
    case SQL_TIMESTAMP:
    case SQL_TYPE_TIMESTAMP:
      return VALUE_TYPE_TIMESTAMP;
    case SQL_VARCHAR:
      return VALUE_TYPE_VARCHAR;
    case SQL_LONGVARCHAR:
      return VALUE_TYPE_LONGVARCHAR;
    case SQL_BINARY:
      return VALUE_TYPE_BINARY;
    case SQL_VARBINARY:
      return VALUE_TYPE_VARBINARY;
    case SQL_LONGVARBINARY:
      return VALUE_TYPE_LONGVARBINARY;
    case SQL_BIGINT:
      return VALUE_TYPE_BIGINT;
    case SQL_TINYINT:
      return VALUE_TYPE_TINYINT;
    case SQL_BIT:
      return VALUE_TYPE_BIT;
    case SQL_WCHAR:
      return VALUE_TYPE_WCHAR;
    case SQL_WVARCHAR:
      return VALUE_TYPE_WVARCHAR;
    case SQL_WLONGVARCHAR:
      return VALUE_TYPE_WLONGVARCHAR;
    case SQL_GUID:
      return VALUE_TYPE_GUID;
      break;
    default:
      return VALUE_TYPE_CHAR;
    }
  return VALUE_TYPE_UNKNOWN;
}


bool 
Statement::IsColumnNullable(int index)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  if (index < 0 || index >= m_columnCount)
    throw FB::script_error("Column index is out of range");

  index++;
  SQLLEN v;
  int rc = CheckStmtRC(SQLColAttributesW (m_hstmt, index, 
    	SQL_COLUMN_NULLABLE, NULL, 0, NULL, &v));
  if (rc != PE_OK)
    return false;

  return (v != 0? true: false);
}


long
Statement::GetErrorCode()
{
    return m_errCode;
}


std::wstring
Statement::GetErrorMessage()
{
    return m_errMessage;
}


std::wstring
Statement::GetErrorState()
{
    return m_errState;
}


int 
Statement::GetColumnCount()
{
    return m_columnCount;
}


int 
Statement::GetRowCount()
{
    return m_rowCount;
}


void
Statement::GetTables(const FB::variant& arg0, const FB::variant& arg1, 
	const FB::variant& arg2, const FB::variant& arg3)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLTablesW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS),
  	(arg3.is_null()?NULL:(SQLWCHAR*)arg3.convert_cast<std::wstring>().c_str()), 
  	  (arg3.is_null()?0:SQL_NTS));

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
Statement::GetColumns(const FB::variant& arg0, const FB::variant& arg1, 
	const FB::variant& arg2, const FB::variant& arg3)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLColumnsW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS),
  	(arg3.is_null()?NULL:(SQLWCHAR*)arg3.convert_cast<std::wstring>().c_str()), 
  	  (arg3.is_null()?0:SQL_NTS));
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
Statement::GetTypeInfo(int dataType)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
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
Statement::GetPrimaryKeys(const FB::variant& arg0, const FB::variant& arg1, 
		const FB::variant& arg2)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLPrimaryKeysW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS));
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
Statement::GetForeignKeys(const FB::variant& arg0, const FB::variant& arg1, 
	const FB::variant& arg2, const FB::variant& arg3, 
	const FB::variant& arg4, const FB::variant& arg5)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLForeignKeysW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS),
  	(arg3.is_null()?NULL:(SQLWCHAR*)arg3.convert_cast<std::wstring>().c_str()), 
  	  (arg3.is_null()?0:SQL_NTS),
  	(arg4.is_null()?NULL:(SQLWCHAR*)arg4.convert_cast<std::wstring>().c_str()), 
  	  (arg4.is_null()?0:SQL_NTS),
  	(arg5.is_null()?NULL:(SQLWCHAR*)arg5.convert_cast<std::wstring>().c_str()), 
  	  (arg5.is_null()?0:SQL_NTS));
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
Statement::GetProcedures(const FB::variant& arg0, const FB::variant& arg1, 
	const FB::variant& arg2)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLProceduresW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS));
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
Statement::GetProcedureColumns(const FB::variant& arg0, const FB::variant& arg1, 
	const FB::variant& arg2, const FB::variant& arg3)
{
  if (!m_hstmt)
    throw FB::script_error("Statement isn't initialized");

  int rc = Close();
  if (rc != PE_OK)
    return;

  SQLRETURN code = SQLProcedureColumnsW(m_hstmt, 
  	(arg0.is_null()?NULL:(SQLWCHAR*)arg0.convert_cast<std::wstring>().c_str()), 
  	  (arg0.is_null()?0:SQL_NTS),
  	(arg1.is_null()?NULL:(SQLWCHAR*)arg1.convert_cast<std::wstring>().c_str()), 
  	  (arg1.is_null()?0:SQL_NTS),
  	(arg2.is_null()?NULL:(SQLWCHAR*)arg2.convert_cast<std::wstring>().c_str()), 
  	  (arg2.is_null()?0:SQL_NTS),
  	(arg3.is_null()?NULL:(SQLWCHAR*)arg3.convert_cast<std::wstring>().c_str()), 
  	  (arg3.is_null()?0:SQL_NTS));
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

