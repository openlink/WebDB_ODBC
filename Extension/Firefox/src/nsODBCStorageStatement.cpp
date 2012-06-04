/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: sw=4 ts=4 sts=4 expandtab
 * ***** BEGIN LICENSE BLOCK *****
 *
 * ***** END LICENSE BLOCK ***** */

#include <stdio.h>

//??--#include "nsAutoLock.h"
#include "mozilla/Mutex.h"


#include "nsODBCStorageConnection.h"
#include "nsODBCStorageStatement.h"
#include "nsODBCStorageEvents.h"

#include "nsODBCStorageVariant.h"



/**
 ** nsODBCStorageStatement
 **/
NS_IMPL_THREADSAFE_ISUPPORTS2(nsODBCStorageStatement,  nsIODBCStorageStatement
	, nsIODBCStorageValueArray)


nsODBCStorageStatement::nsODBCStorageStatement() : 
    m_hstmt(nsnull), 
    mPrepared(PR_FALSE),
    mColumnNames(),
    mExecuting(PR_FALSE),
    m_col(-1)//,
{
}


nsODBCStorageStatement::~nsODBCStorageStatement()
{
    (void)Finalize();
}



NS_IMETHODIMP
nsODBCStorageStatement::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
{
  m_errorCode = 0;
  m_errorState.Truncate();
  m_errorMessage.Truncate();

  if (!SQL_SUCCEEDED(rc)) {

    if (handle == m_hdbc) {
      StoreError(fHandleType, handle, m_errorCode, m_errorMessage, m_errorState);  
      mDBConnection->m_errorCode = m_errorCode;
      mDBConnection->m_errorState = m_errorState;
      mDBConnection->m_errorMessage = m_errorMessage;  
    }else {
      StoreError(fHandleType, handle, m_errorCode, m_errorMessage, m_errorState);  
    }
    return NS_ERROR_FAILURE;

  } else {

    return NS_OK;
  }
}


NS_IMETHODIMP
nsODBCStorageStatement::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}


NS_IMETHODIMP
nsODBCStorageStatement::CheckStmtRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_STMT, m_hstmt, rc);
}



nsresult
nsODBCStorageStatement::Initialize(nsODBCStorageConnection *aDBConnection,
                                const nsAString & aSQLquery)
{
    NS_ASSERTION(aDBConnection, "No database connection given!");
    NS_ASSERTION(!m_hstmt, "Calling Initialize on an already initialized statement!");

    mDBConnection = aDBConnection;
    m_hdbc = aDBConnection->GetNativeConnection();
    NS_ENSURE_TRUE(m_hdbc != nsnull, NS_ERROR_NULL_POINTER);
    m_SQLquery.Assign(aSQLquery);
    mColumnNames.Clear();
    return NS_OK;
}


nsresult
nsODBCStorageStatement::Prepare()
{
    NS_ASSERTION(mDBConnection, "No database connection given!");
    NS_ASSERTION(!mPrepare, "Calling Prepare on an already prepared statement!");

    nsresult rc;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &m_hstmt));
    if (rc != NS_OK) {
      mDBConnection = nsnull;
      m_hdbc = nsnull;
      return rc;
    }

    rc = CheckStmtRC(SQLPrepareW(m_hstmt, nsWString(m_SQLquery).Data(), SQL_NTS));
    if (rc != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, m_hstmt);
      mDBConnection = nsnull;
      m_hdbc = nsnull;
      m_hstmt = nsnull;
      return rc;
    }

    SQLSMALLINT count;

    if (SQL_SUCCEEDED(SQLNumParams(m_hstmt, &count))) 
      mParamCount = count;
    if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &count))) 
      mResultColumnCount = count;

    mParams.SetCapacity(mParamCount);
    mColumnNames.Clear();

    SQLWCHAR colName[512];

    for (SQLSMALLINT i = 1; i <= mResultColumnCount; i++) {
      rc = CheckStmtRC(SQLDescribeColW (m_hstmt, i,colName, NUMCHAR(colName), 
         NULL, NULL, NULL, NULL, NULL));
      if (rc == NS_OK) {
        nsString str;
        NS_WStringToUTF16(colName, str);
        mColumnNames.AppendElement(str);
      }
    }


    mPrepared = PR_TRUE;
    return NS_OK;
}


nsresult
nsODBCStorageStatement::Initialize(nsODBCStorageConnection *aDBConnection, SQLHANDLE hstmt)
{
    NS_ASSERTION(aDBConnection, "No database connection given!");
    NS_ASSERTION(!m_hstmt, "Calling Initialize on an already initialized statement!");

    mDBConnection = aDBConnection;
    m_hdbc = aDBConnection->GetNativeConnection();
    NS_ENSURE_TRUE(m_hdbc != nsnull, NS_ERROR_NULL_POINTER);

    m_hstmt = hstmt;
 
    SQLSMALLINT count;
    nsresult rc = NS_OK;

    mParamCount = 0;
    if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &count))) 
      mResultColumnCount = count;

    mParams.SetCapacity(mParamCount);
    mColumnNames.Clear();

    SQLWCHAR colName[512];

    for (SQLSMALLINT i = 1; i <= mResultColumnCount; i++) {
      rc = CheckStmtRC(SQLDescribeColW (m_hstmt, i,colName, NUMCHAR(colName), 
         NULL, NULL, NULL, NULL, NULL));
      if (rc == NS_OK) {
        nsString str;
        NS_WStringToUTF16(colName, str);
        mColumnNames.AppendElement(str);
      }
    }

    mPrepared = PR_TRUE;
    m_col = -1;
    m_Data = nsnull;
    mExecuting = PR_TRUE;
    
    return rc;
}



////////////////////////////////////////////////////////////////////////////////
//// nsIODBCStorageStatement

NS_IMETHODIMP
nsODBCStorageStatement::GetLastErrorCode(PRInt32 *aLastError)
{
    *aLastError = m_errorCode;
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageStatement::GetLastErrorString(nsAString& aLastErrorString)
{
    aLastErrorString.Assign(m_errorMessage);
    return NS_OK;
}

/* readonly attribute AString lastErrorState; */
NS_IMETHODIMP 
nsODBCStorageStatement::GetLastErrorState(nsAString & aLastErrorState)
{
    aLastErrorState.Assign(m_errorState);
    return NS_OK;
}


/* nsIODBCStorageStatement clone (); */
NS_IMETHODIMP
nsODBCStorageStatement::Clone(nsIODBCStorageStatement **_retval)
{
    nsODBCStorageStatement *mss = new nsODBCStorageStatement();
    if (!mss)
      return NS_ERROR_OUT_OF_MEMORY;

    nsresult rv = mss->Initialize (mDBConnection, m_SQLquery);
    NS_ENSURE_SUCCESS(rv, rv);

    rv = mss->Prepare();
    NS_ENSURE_SUCCESS(rv, rv);

    for(PRInt32 i=0; i< mParams.Count(); i++) {
      nsODBCParam *param = mParams[i];
      mss->mParams.ReplaceObjectAt(param, i);
    }

    NS_ADDREF(*_retval = mss);
    return NS_OK;
}



/* [noscript, notxpcom] nsIODBCStorageStatement dupForAsync (); */
NS_IMETHODIMP_(nsIODBCStorageStatement *) 
nsODBCStorageStatement::DupForAsync()
{
    nsODBCStorageStatement *mss = new nsODBCStorageStatement();
    if (!mss)
      return nsnull;

    nsresult rv = mss->Initialize(mDBConnection, m_SQLquery);
    if (rv != NS_OK) {
      delete mss;
      return nsnull;
    }

    mss->mParamCount = mParamCount; mParamCount = 0;
    mss->mResultColumnCount = mResultColumnCount; mResultColumnCount = 0;
    mss->mParams.SetCapacity(mParamCount);

    for(PRInt32 i=0; i< mParams.Count(); i++) {
      nsODBCParam *param = mParams[i];
      mss->mParams.ReplaceObjectAt(param, i);
    }
    mParams.Clear();

    mss->mColumnNames = mColumnNames;

    SQLHANDLE stmt = m_hstmt;
    m_hstmt = mss->m_hstmt;;
    mss->m_hstmt = stmt;

    return mss;
}



/* void finalize(); */
NS_IMETHODIMP
nsODBCStorageStatement::Finalize()
{
    SQLHANDLE hstmt = m_hstmt;
    m_hstmt = NULL;

    if (!hstmt)
        return NS_OK;

    if (mDBConnection && mDBConnection->m_hdbc != nsnull) {
      if (mResultColumnCount > 0)
        SQLCloseCursor (hstmt);
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    }
    mParams.Clear();

    return NS_OK;
}


/* readonly attribute unsigned short parameterCount; */
NS_IMETHODIMP
nsODBCStorageStatement::GetParameterCount(PRUint16 *aParameterCount)
{
    NS_ENSURE_ARG_POINTER(aParameterCount);

    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    *aParameterCount = mParamCount;
    return NS_OK;
}


/* readonly attribute unsigned short columnCount; */
NS_IMETHODIMP
nsODBCStorageStatement::GetColumnCount(PRUint16 *aColumnCount)
{
    NS_ENSURE_ARG_POINTER(aColumnCount);

    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    *aColumnCount = mResultColumnCount;
    return NS_OK;
}


/* AString getColumnName(in unsigned short aColumnIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetColumnName(PRUint16 aColumnIndex, nsAString & _retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aColumnIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;

    _retval.Assign(mColumnNames[aColumnIndex]);

    return NS_OK;
}


/* short getColumnIndex(in AString aName); */
NS_IMETHODIMP
nsODBCStorageStatement::GetColumnIndex(const nsAString &aName, PRUint16 *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    nsWString colName(aName);

    for(PRUint32 i = 0; i < mResultColumnCount; i++) {
      nsWString name(mColumnNames[i]);
      if (wcscmp(colName.Data(), name.Data()) == 0) {
        *_retval = (PRUint16)i;
        return NS_OK;
      }
    }

#if 0
    PRInt32 idx = mColumnNames.IndexOf(aName);
    if (idx != -1) {
        *_retval = (PRUint16)idx;
        return NS_OK;
    }
#endif
    return NS_ERROR_INVALID_ARG;
}





/* void reset (); */
NS_IMETHODIMP
nsODBCStorageStatement::Reset()
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (mPrepared && m_hstmt) {
      if (mResultColumnCount > 0)
        SQLCloseCursor (m_hstmt);
      SQLFreeStmt (m_hstmt, SQL_RESET_PARAMS);
    }

    mParams.Clear();

    mExecuting = PR_FALSE;
    m_col = -1;
    m_Data = nsnull;

    return NS_OK;
}


NS_IMETHODIMP
nsODBCStorageStatement::SetParams(nsCOMArray<nsODBCParam> & params)
{
    mParams.Clear();
    return mParams.AppendObjects(params);
}



NS_IMETHODIMP 
nsODBCStorageStatement::BindParameter(PRUint16 aParamIndex, nsIVariant *aValue)
{
    PRUint16 dtype;
    nsresult rv = aValue->GetDataType(&dtype);
    if (rv == NS_OK) {
        switch(dtype) {
            case nsIDataType::VTYPE_BOOL:
            case nsIDataType::VTYPE_INT8:
            case nsIDataType::VTYPE_UINT8:
            case nsIDataType::VTYPE_INT16:
                {
                PRInt16 val;
                rv = aValue->GetAsInt16(&val);
                if (rv != NS_OK)
                   return rv;
                rv = BindInt16Parameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_UINT16:
            case nsIDataType::VTYPE_INT32:
                {
                PRInt32 val;
                rv = aValue->GetAsInt32(&val);
                if (rv != NS_OK)
                   return rv;
                rv = BindInt32Parameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_UINT32:
            case nsIDataType::VTYPE_INT64:
            case nsIDataType::VTYPE_UINT64:
                {
                PRInt64 val;
                rv = aValue->GetAsInt64(&val);
                if (rv != NS_OK)
                   return rv;
                rv = BindInt64Parameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_FLOAT:
                {
                float val;
                rv = aValue->GetAsFloat(&val);
                if (rv != NS_OK)
                   return rv;
                rv = BindFloatParameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_DOUBLE:
                {
                double val;
                rv = aValue->GetAsDouble(&val);
                if (rv != NS_OK)
                   return rv;
                rv = BindDoubleParameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_CHAR_STR:
            case nsIDataType::VTYPE_WCHAR_STR:
            case nsIDataType::VTYPE_CHAR:
            case nsIDataType::VTYPE_WCHAR:
            case nsIDataType::VTYPE_DOMSTRING:
            case nsIDataType::VTYPE_STRING_SIZE_IS:
            case nsIDataType::VTYPE_WSTRING_SIZE_IS:
            case nsIDataType::VTYPE_UTF8STRING:
            case nsIDataType::VTYPE_CSTRING:
            case nsIDataType::VTYPE_ASTRING:
                {
                nsString val;
                rv = aValue->GetAsAString(val);
                if (rv != NS_OK)
                   return rv;
                rv = BindStringParameter(aParamIndex, val);
                break;
                }

            case nsIDataType::VTYPE_EMPTY:
                rv = BindNullParameter(aParamIndex);
                break;
            case nsIDataType::VTYPE_ARRAY:
                {
                PRUint16 type;
                nsIID iid;
                PRUint32 count;
                void  *ptr;
                rv = aValue->GetAsArray(&type, &iid, &count, &ptr);
                if (rv != NS_OK)
                   return rv;
                rv = BindBlobParameter(aParamIndex, (PRUint8 *)ptr, count);
                }
                break;
            default:
//VTYPE_VOID
//VTYPE_ID
//VTYPE_INTERFACE
//VTYPE_INTERFACE_IS
//VTYPE_EMPTY_ARRAY
                return NS_ERROR_NOT_IMPLEMENTED;
        }
    }
    return rv;
}


/* void bindStringParameter (in unsigned short aParamIndex, in AString aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindLongStringParameter (in unsigned short aParamIndex, in AString aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    val->mIsLong = PR_TRUE;
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindFloatParameter (in unsigned short aParamIndex, in float aValue); */
NS_IMETHODIMP 
nsODBCStorageStatement::BindFloatParameter(PRUint16 aParamIndex, float aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}

/* void bindDoubleParameter (in unsigned short aParamIndex, in double aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindDoubleParameter(PRUint16 aParamIndex, double aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindInt16Parameter (in unsigned short aParamIndex, in short aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindInt32Parameter (in unsigned short aParamIndex, in long aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindInt64Parameter (in unsigned short aParamIndex, in long long aValue); */
NS_IMETHODIMP
nsODBCStorageStatement::BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindNullParameter (in unsigned short aParamIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::BindNullParameter(PRUint16 aParamIndex)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam();
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindBlobParameter (in unsigned long aParamIndex, [array, const, size_is (aValueSize)] in octet aValue, in unsigned long aValueSize); */
NS_IMETHODIMP
nsODBCStorageStatement::BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue, aValueSize);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    val->mIsLong = PR_TRUE;
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}


/* void bindBinaryParameter (in unsigned long aParamIndex, [array, const, size_is (aValueSize)] in octet aValue, in unsigned long aValueSize); */
NS_IMETHODIMP
nsODBCStorageStatement::BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCParam *val = new nsODBCParam(aValue, aValueSize);
    NS_ENSURE_TRUE(val, NS_ERROR_OUT_OF_MEMORY);
    mParams.ReplaceObjectAt(val, aParamIndex);
    return NS_OK;
}



/* void executeSql (); */
NS_IMETHODIMP 
nsODBCStorageStatement::ExecuteSql()
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    m_errorCode = 0;
    m_errorState.Truncate();
    m_errorMessage.Truncate();

    m_col = -1;
    m_Data = nsnull;
    nsresult rv;

    for(PRInt32 i=0; i< mParams.Count(); i++) {
      nsODBCParam *val = mParams[i];
      if (val == nsnull)
        return NS_ERROR_FAILURE;

      switch(val->mType) {
        case nsIDataType::VTYPE_BOOL :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_BIT, SQL_BIT, sizeof(PRBool), 0, &(val->mVal.mBoolValue), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }
        case nsIDataType::VTYPE_INT16 :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_SHORT, SQL_SMALLINT, sizeof(SQLSMALLINT), 0, &(val->mVal.mInt16Value), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_INT32 :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_SLONG, SQL_INTEGER, sizeof(SQLINTEGER), 0, &(val->mVal.mInt32Value), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_INT64 :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_SBIGINT, SQL_BIGINT, sizeof(SQLBIGINT), 0, &(val->mVal.mInt64Value), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_FLOAT :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_FLOAT, SQL_REAL, sizeof(SQLREAL), 0, &(val->mVal.mFloatValue), 
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_DOUBLE :
        {
          val->mInd = 0;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_DOUBLE, SQL_DOUBLE, sizeof(SQLDOUBLE), 0, &(val->mVal.mDoubleValue),
             0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_ASTRING :
        {
          nsWString *v_str = val->mVal.mStringValue;
          val->mInd = SQL_NTS;
    	  rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
    	     SQL_C_WCHAR, (val->mIsLong?SQL_WLONGVARCHAR:SQL_WVARCHAR), 
    	     v_str->Length(), 0, v_str->Data(), 0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        case nsIDataType::VTYPE_ARRAY :
        {
          nsBinary *v_bin = val->mVal.mBinValue;
          val->mInd = v_bin->Length();
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_BINARY, (val->mIsLong?SQL_LONGVARBINARY:SQL_VARBINARY), 
             v_bin->Length(), 0, v_bin->Data(), 0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }

        default:
        {
          val->mInd = SQL_NULL_DATA;
          rv = CheckStmtRC(SQLBindParameter(m_hstmt, i+1, SQL_PARAM_INPUT, 
             SQL_C_CHAR, SQL_VARCHAR, 0, 0, NULL, 0, (SQLLEN *)&(val->mInd)));
    	  if (rv != NS_OK)
    	    return rv;
    	  break;
        }
      }
    }


    SQLRETURN rc = SQLExecute(m_hstmt);
    if (!SQL_SUCCEEDED(rc) && rc != SQL_NO_DATA) {
      StoreError(SQL_HANDLE_STMT, m_hstmt, m_errorCode, m_errorMessage, m_errorState);  
      SQLFreeStmt (m_hstmt, SQL_RESET_PARAMS);
      return NS_ERROR_FAILURE;
    } else {

      SQLFreeStmt (m_hstmt, SQL_RESET_PARAMS);
      SQLSMALLINT count;
      if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &count)) && count != mResultColumnCount) {
        mResultColumnCount = count;
        mColumnNames.Clear();

        SQLWCHAR colName[512];

        for (SQLSMALLINT i = 1; i <= mResultColumnCount; i++) {
          rv = CheckStmtRC(SQLDescribeColW (m_hstmt, i,colName, NUMCHAR(colName),
             NULL, NULL, NULL, NULL, NULL));
          if (rv == NS_OK) {
            nsString str;
            NS_WStringToUTF16(colName, str);
            mColumnNames.AppendElement(str);
          }
        }
      }
      if (mResultColumnCount > 0)
        mExecuting = PR_TRUE;
      return NS_OK;
    }
}


/* boolean moreResults (); */
NS_IMETHODIMP 
nsODBCStorageStatement::MoreResults(PRBool *_retval NS_OUTPARAM)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    m_errorCode = 0;
    m_errorState.Truncate();
    m_errorMessage.Truncate();

    m_col = -1;
    m_Data = nsnull;
    SQLRETURN rc = SQLMoreResults(m_hstmt);
    if (SQL_SUCCEEDED(rc)) {

      SQLWCHAR colName[512];
      SQLSMALLINT count;

      if (SQL_SUCCEEDED(SQLNumResultCols(m_hstmt, &count))) 
        mResultColumnCount = count;

      mColumnNames.Clear();

      for (SQLSMALLINT i = 1; i <= mResultColumnCount; i++) {
        rc = CheckStmtRC(SQLDescribeColW (m_hstmt, i,colName, NUMCHAR(colName), 
           NULL, NULL, NULL, NULL, NULL));
        if (rc == NS_OK) {
          nsString str;
          NS_WStringToUTF16(colName, str);
          mColumnNames.AppendElement(str);
        }
      }
      if (mResultColumnCount > 0)
        mExecuting = PR_TRUE;
      *_retval = PR_TRUE;
      return NS_OK;

    } else if (rc == SQL_NO_DATA) {
      mExecuting = PR_FALSE;
      *_retval = PR_FALSE;
      return NS_OK;
    } else {
      StoreError(SQL_HANDLE_STMT, m_hstmt, m_errorCode, m_errorMessage, m_errorState);  
      return NS_ERROR_FAILURE;
    }
}

/* boolean fetch (); */
NS_IMETHODIMP 
nsODBCStorageStatement::Fetch(PRBool *_retval NS_OUTPARAM)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (!mExecuting)
      return NS_ERROR_FAILURE;

    m_errorCode = 0;
    m_errorState.Truncate();
    m_errorMessage.Truncate();

    m_col = -1;
    m_Data = nsnull;

    SQLRETURN rc = SQLFetch(m_hstmt);
    if (SQL_SUCCEEDED(rc)) {

      *_retval = PR_TRUE;
      mExecuting = PR_TRUE;
      return NS_OK;

    } else if (rc == SQL_NO_DATA) {

      *_retval = PR_FALSE;
      mExecuting = PR_FALSE;
      return NS_OK;

    } else {
      StoreError(SQL_HANDLE_STMT, m_hstmt, m_errorCode, m_errorMessage, m_errorState);  
      return NS_ERROR_FAILURE;
    }
}

/* readonly attribute long rowCount; */
NS_IMETHODIMP 
nsODBCStorageStatement::GetRowCount(PRInt32 *aRowCount)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    SQLLEN count;
    nsresult rc = CheckStmtRC(SQLRowCount(m_hstmt, &count));
    *aRowCount = count;
    return rc;
}


/* void execute (); */
NS_IMETHODIMP
nsODBCStorageStatement::Execute()
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    nsresult rv = ExecuteSql();
    NS_ENSURE_SUCCESS(rv, rv);
    return Reset();
}


/* boolean executeStep (); */
NS_IMETHODIMP
nsODBCStorageStatement::ExecuteStep(PRBool *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;

    if (mExecuting) {

      /* fetch */
      rv = Fetch(_retval);
      NS_ENSURE_SUCCESS(rv, rv);
    } else {

      rv = ExecuteSql();
      NS_ENSURE_SUCCESS(rv, rv);

      if (mResultColumnCount > 0) {

        rv = Fetch(_retval);
        NS_ENSURE_SUCCESS(rv, rv);

      } else {
        *_retval = PR_FALSE;
      }
    }

    return NS_OK;
}

/* nsICancelable executeAsync([optional] in storageIStatementCallback aCallback); */
nsresult
nsODBCStorageStatement::ExecuteAsync(nsIODBCStorageStatementCallback *aCallback,
                                  nsIODBCStoragePendingStatement **_stmt)
{
    nsIODBCStorageStatement * stmts[1] = {this};
    return mDBConnection->ExecuteAsync(stmts, 1, aCallback, _stmt);
}


/* readonly attribute long state; */
NS_IMETHODIMP
nsODBCStorageStatement::GetState(PRInt32 *_retval)
{
    if (!mDBConnection) {
        *_retval = NS_ODBC_STORAGE_STATEMENT_INVALID;
    } else if (!m_hstmt && mDBConnection) {
        if (!mPrepared) {
          nsresult rv = Prepare();
          NS_ENSURE_SUCCESS(rv, rv);
        }
        if (!m_hstmt)
          *_retval = NS_ODBC_STORAGE_STATEMENT_INVALID;
        else
          *_retval = NS_ODBC_STORAGE_STATEMENT_READY;
    } else if (mExecuting) {
        *_retval = NS_ODBC_STORAGE_STATEMENT_EXECUTING;
    } else {
        *_retval = NS_ODBC_STORAGE_STATEMENT_READY;
    }

    return NS_OK;
}



/* [noscript, notxpcom] readonly attribute ConnHandle connectionHandle; */
NS_IMETHODIMP 
nsODBCStorageStatement::GetConnectionHandle(SQLHANDLE *aConnectionHandle)
{
    *aConnectionHandle = m_hdbc;
    return NS_OK;
}


/* AString escapeStringForLIKE(in AString aValue, in char aEscapeChar); */
NS_IMETHODIMP
nsODBCStorageStatement::EscapeStringForLIKE(const nsAString & aValue, 
                                         const PRUnichar aEscapeChar, 
                                         nsAString &aEscapedString)
{
    const PRUnichar MATCH_ALL('%');
    const PRUnichar MATCH_ONE('_');

    aEscapedString.Truncate();

    const PRUnichar *cur;
    PRUint32 len = NS_StringGetData(aValue, &cur);

    for (PRUint32 i = 0; i < len; i++, cur++) {
        if (*cur == aEscapeChar || *cur == MATCH_ALL || 
            *cur == MATCH_ONE)
            aEscapedString += aEscapeChar;
        aEscapedString += *cur;
    }
    return NS_OK;
}


/***
 *** nsIODBCStorageValueArray
 ***/

/* readonly attribute unsigned short numEntries; */
NS_IMETHODIMP
nsODBCStorageStatement::GetNumEntries(PRUint16 *aLength)
{
    return GetColumnCount(aLength);
}


/* boolean isNullable (in unsigned short aIndex); */
NS_IMETHODIMP 
nsODBCStorageStatement::IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;

    aIndex++;
    SQLLEN v;
    nsresult rc = CheckStmtRC(SQLColAttributesW (m_hstmt, aIndex, 
    	SQL_COLUMN_NULLABLE, NULL, 0, NULL, &v));
    if (rc != NS_OK)
      return rc;
    *_retval =(v != 0? PR_TRUE: PR_FALSE);
    return NS_OK;
}


/* long getTypeOfIndex (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;

    aIndex++;
    SQLLEN t;
    nsresult rc = CheckStmtRC(SQLColAttributesW (m_hstmt, aIndex, 
    	SQL_COLUMN_TYPE, NULL, 0, NULL, &t));
    if (rc != NS_OK)
      return rc;

    switch(t) {
        case SQL_CHAR:
            *_retval = VALUE_TYPE_CHAR;
            break;
        case SQL_NUMERIC:
            *_retval = VALUE_TYPE_NUMERIC;
            break;
        case SQL_DECIMAL:
            *_retval = VALUE_TYPE_DECIMAL;
            break;
        case SQL_INTEGER:
            *_retval = VALUE_TYPE_INTEGER;
            break;
        case SQL_SMALLINT:
            *_retval = VALUE_TYPE_SMALLINT;
            break;
        case SQL_FLOAT:
            *_retval = VALUE_TYPE_FLOAT;
            break;
        case SQL_REAL:
            *_retval = VALUE_TYPE_REAL;
            break;
        case SQL_DOUBLE:
            *_retval = VALUE_TYPE_DOUBLE;
            break;
        case SQL_DATE:
        case SQL_TYPE_DATE:
            *_retval = VALUE_TYPE_DATE;
            break;
        case SQL_TIME:
        case SQL_TYPE_TIME:
            *_retval = VALUE_TYPE_TIME;
            break;
        case SQL_TIMESTAMP:
        case SQL_TYPE_TIMESTAMP:
            *_retval = VALUE_TYPE_TIMESTAMP;
            break;
        case SQL_VARCHAR:
            *_retval = VALUE_TYPE_VARCHAR;
            break;
        case SQL_LONGVARCHAR:
            *_retval = VALUE_TYPE_LONGVARCHAR;
            break;
        case SQL_BINARY:
            *_retval = VALUE_TYPE_BINARY;
            break;
        case SQL_VARBINARY:
            *_retval = VALUE_TYPE_VARBINARY;
            break;
        case SQL_LONGVARBINARY:
            *_retval = VALUE_TYPE_LONGVARBINARY;
            break;
        case SQL_BIGINT:
            *_retval = VALUE_TYPE_BIGINT;
            break;
        case SQL_TINYINT:
            *_retval = VALUE_TYPE_TINYINT;
            break;
        case SQL_BIT:
            *_retval = VALUE_TYPE_BIT;
            break;
        case SQL_WCHAR:
            *_retval = VALUE_TYPE_WCHAR;
            break;
        case SQL_WVARCHAR:
            *_retval = VALUE_TYPE_WVARCHAR;
            break;
        case SQL_WLONGVARCHAR:
            *_retval = VALUE_TYPE_WLONGVARCHAR;
            break;
        case SQL_GUID:
            *_retval = VALUE_TYPE_GUID;
            break;
        default:
            *_retval = VALUE_TYPE_CHAR;
            break;
    }

    return NS_OK;
}


NS_IMETHODIMP
nsODBCStorageStatement::GetVariantMz(PRUint16 aIndex, nsIVariant **_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    SQLLEN cbInd;
    PRInt32 type;
    nsresult rc;
    nsIVariant *val = nsnull;

    m_col = -1;
    m_Data = nsnull;

    GetTypeOfIndex(aIndex, &type);
    aIndex++;

    switch (type) {
      case VALUE_TYPE_BIT:
      {
        PRBool v_bool;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_BIT, &v_bool, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageBool(v_bool);
        break;
      }
      case VALUE_TYPE_TINYINT:
      case VALUE_TYPE_SMALLINT:
      {
        PRInt16 v_int16;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_SHORT, &v_int16, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt16(v_int16);
        break;
      }
      case VALUE_TYPE_INTEGER:
      {
        PRInt32 v_int32;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_LONG, &v_int32, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt32(v_int32);
        break;
      }
      case VALUE_TYPE_BIGINT:
      {
        PRInt64 v_int64;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_SBIGINT, &v_int64, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt64(v_int64);
        break;
      }

      case VALUE_TYPE_FLOAT:
      case VALUE_TYPE_REAL:
      {
        float v_float;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_FLOAT, &v_float, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageFloat(v_float);
        break;
      }
      case VALUE_TYPE_DOUBLE:
      {
        double v_double;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_DOUBLE, &v_double, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageDouble(v_double);
        break;
      }

      case VALUE_TYPE_BINARY:
      case VALUE_TYPE_VARBINARY:
      case VALUE_TYPE_LONGVARBINARY:
      {
        SQLLEN len, dsize;
        PRUint8 *pData = NULL;
        PRUint8 bbuf[8192];

        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_BINARY, bbuf, 0, &len));
        if (rc != NS_OK)
          return rc;

        if (len != SQL_NULL_DATA) {
          len = 0;
          dsize = 0;
          while(1) {
            SQLRETURN ret;
            ret = SQLGetData(m_hstmt, aIndex, SQL_C_BINARY, bbuf, sizeof(bbuf), &len);
            if (ret == SQL_NO_DATA_FOUND)
              break;
            if ((rc = CheckStmtRC(ret)) != NS_OK)
              return rc;
            if (len > (SQLLEN)sizeof(bbuf))
              len = sizeof(bbuf);

            if (!pData)
              pData = (PRUint8 *)NS_Alloc(len);//?? pData = (PRUint8 *)nsMemory::Alloc(len);
            else
              pData = (PRUint8 *)NS_Realloc(pData, dsize + len); //?? pData = (PRUint8 *)nsMemory::Realloc(pData, dsize + len);

            if (!pData)
              return NS_ERROR_OUT_OF_MEMORY;

            memcpy(pData+dsize, bbuf, len);
            dsize += len;
          }
          val = new nsODBCStorageBlob(std::pair<const PRUint8 *, PRUint32>(pData, dsize));
        }
        break;
      }

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
        SQLWCHAR buf[1024];
        nsString v_str;

        rc = CheckStmtRC(SQLColAttributesW (m_hstmt, aIndex, SQL_COLUMN_TYPE, NULL, 0, NULL, &dtype));
        if (rc != NS_OK)
          return rc;


        if (dtype == SQL_LONGVARCHAR || dtype == SQL_VARCHAR || dtype == SQL_CHAR ||
            dtype == SQL_WLONGVARCHAR || dtype == SQL_WVARCHAR || dtype == SQL_WCHAR ||
            dtype == SQL_LONGVARBINARY || dtype == SQL_VARBINARY || dtype == SQL_BINARY)
        {
          rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, buf, 0, &len));
          if (rc != NS_OK)
            return rc;
          if (len != SQL_NULL_DATA) {
            SQLWCHAR bbuf[8192];

            len = sizeof(bbuf);
            while(1) {
              SQLRETURN ret;
              ret = SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, bbuf, sizeof(bbuf), &len);
              if (ret == SQL_NO_DATA_FOUND)
                break;
              if ((rc = CheckStmtRC(ret)) != NS_OK)
                return rc;
              NS_AppendWStringToUTF16(bbuf, v_str);
            }
            val = new nsODBCStorageText(v_str);
          }
        } else {
          rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, buf, sizeof(buf), &len));
          if (rc != NS_OK)
            return rc;
          if (len != SQL_NULL_DATA) {
            NS_AppendWStringToUTF16(buf, v_str);
            val = new nsODBCStorageText(v_str);
          }
        }  
        break;
      }
    }

    if (val == nsnull) {
      val = new nsODBCStorageNull();
    }
    m_Data = val;
    m_col = aIndex-1;
    NS_IF_ADDREF(*_retval = val);  
    return NS_OK;
}


/* nsIVariant getVar (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetVariant(PRUint16 aIndex, nsIVariant **_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    SQLLEN cbInd;
    PRInt32 type;
    nsresult rc;
    nsIVariant *val = nsnull;

    m_col = -1;
    m_Data = nsnull;

    GetTypeOfIndex(aIndex, &type);
    aIndex++;

    switch (type) {
      case VALUE_TYPE_BIT:
      {
        PRBool v_bool;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_BIT, &v_bool, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageBool(v_bool);
        break;
      }
      case VALUE_TYPE_TINYINT:
      case VALUE_TYPE_SMALLINT:
      {
        PRInt16 v_int16;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_SHORT, &v_int16, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt16(v_int16);
        break;
      }
      case VALUE_TYPE_INTEGER:
      {
        PRInt32 v_int32;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_LONG, &v_int32, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt32(v_int32);
        break;
      }
      case VALUE_TYPE_BIGINT:
      {
        PRInt64 v_int64;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_SBIGINT, &v_int64, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageInt64(v_int64);
        break;
      }

      case VALUE_TYPE_FLOAT:
      case VALUE_TYPE_REAL:
      {
        float v_float;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_FLOAT, &v_float, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageFloat(v_float);
        break;
      }
      case VALUE_TYPE_DOUBLE:
      {
        double v_double;
        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_DOUBLE, &v_double, 0, &cbInd));
        if (rc != NS_OK)
          return rc;
        else if (cbInd != SQL_NULL_DATA)
          val = new nsODBCStorageDouble(v_double);
        break;
      }

/**
      case VALUE_TYPE_BINARY:
      case VALUE_TYPE_VARBINARY:
      case VALUE_TYPE_LONGVARBINARY:
      {
        SQLLEN len, dsize;
        PRUint8 *pData = NULL;
        PRUint8 bbuf[8192];

        rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_BINARY, bbuf, 0, &len));
        if (rc != NS_OK)
          return rc;

        if (len != SQL_NULL_DATA) {
          len = 0;
          dsize = 0;
          while(1) {
            SQLRETURN ret;
            ret = SQLGetData(m_hstmt, aIndex, SQL_C_BINARY, bbuf, sizeof(bbuf), &len);
            if (ret == SQL_NO_DATA_FOUND)
              break;
            if ((rc = CheckStmtRC(ret)) != NS_OK)
              return rc;
            if (len > (SQLLEN)sizeof(bbuf))
              len = sizeof(bbuf);

            if (!pData)
              pData = (PRUint8 *)nsMemory::Alloc(len);
            else
              pData = (PRUint8 *)nsMemory::Realloc(pData, dsize + len);

            if (!pData)
              return NS_ERROR_OUT_OF_MEMORY;

            memcpy(pData+dsize, bbuf, len);
            dsize += len;
          }
          val = new nsODBCStorageBlob(std::pair<const PRUint8 *, PRUint32>(pData, dsize));
        }
        break;
      }
***/
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
        SQLWCHAR buf[1024];
        nsString v_str;

        rc = CheckStmtRC(SQLColAttributesW (m_hstmt, aIndex, SQL_COLUMN_TYPE, NULL, 0, NULL, &dtype));
        if (rc != NS_OK)
          return rc;


        if (dtype == SQL_LONGVARCHAR || dtype == SQL_VARCHAR || dtype == SQL_CHAR ||
            dtype == SQL_WLONGVARCHAR || dtype == SQL_WVARCHAR || dtype == SQL_WCHAR ||
            dtype == SQL_LONGVARBINARY || dtype == SQL_VARBINARY || dtype == SQL_BINARY)
        {
          rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, buf, 0, &len));
          if (rc != NS_OK)
            return rc;
          if (len != SQL_NULL_DATA) {
            SQLWCHAR bbuf[8192];

            len = sizeof(bbuf);
            while(1) {
              SQLRETURN ret;
              ret = SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, bbuf, sizeof(bbuf), &len);
              if (ret == SQL_NO_DATA_FOUND)
                break;
              if ((rc = CheckStmtRC(ret)) != NS_OK)
                return rc;
              NS_AppendWStringToUTF16(bbuf, v_str);
            }
            val = new nsODBCStorageText(v_str);
          }
        } else {
          rc = CheckStmtRC(SQLGetData(m_hstmt, aIndex, SQL_C_WCHAR, buf, sizeof(buf), &len));
          if (rc != NS_OK)
            return rc;
          if (len != SQL_NULL_DATA) {
            NS_AppendWStringToUTF16(buf, v_str);
            val = new nsODBCStorageText(v_str);
          }
        }  
        break;
      }
    }

    if (val == nsnull) {
      val = new nsODBCStorageNull();
    }
    m_Data = val;
    m_col = aIndex-1;
    NS_IF_ADDREF(*_retval = val);  
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageStatement::Data(const nsAString &aName, nsIVariant **_retval)
{
  PRUint16 aIndex;
  NS_ENSURE_SUCCESS(GetColumnIndex(aName, &aIndex), NS_ERROR_NOT_AVAILABLE);
  return GetVariant(aIndex, _retval);
}


/* boolean getBool (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetBool(PRUint16 aIndex, PRBool *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = PR_FALSE;
    else
      m_Data->GetAsBool(_retval);

    return NS_OK;
}



/* short getInt16 (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetInt16(PRUint16 aIndex, PRInt16 *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = 0;
    else
      m_Data->GetAsInt16(_retval);

    return NS_OK;
}



/* long getInt32 (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetInt32(PRUint16 aIndex, PRInt32 *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = 0;
    else
      m_Data->GetAsInt32(_retval);

    return NS_OK;
}



/* long long getInt64 (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetInt64(PRUint16 aIndex, PRInt64 *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = 0;
    else
      m_Data->GetAsInt64(_retval);

    return NS_OK;
}



/* double getDouble (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetDouble(PRUint16 aIndex, double *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = 0;
    else
      m_Data->GetAsDouble(_retval);

    return NS_OK;
}



/* float getFloat (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetFloat(PRUint16 aIndex, float *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      *_retval = 0;
    else
      m_Data->GetAsFloat(_retval);

    return NS_OK;
}



/* AString getString (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetString(PRUint16 aIndex, nsAString & _retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      _retval.SetIsVoid(PR_TRUE);
    else
      m_Data->GetAsAString(_retval);

    return NS_OK;
}



/* AUTF8String getUTF8String (in unsigned short aIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetUTF8String(PRUint16 aIndex, nsACString & _retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY)
      _retval.SetIsVoid(PR_TRUE);
    else
      m_Data->GetAsAUTF8String(_retval);

    return NS_OK;
}



/* void getBlob (in unsigned short aIndex, out unsigned long aDataSize, [array, size_is (aDataSize)] out octet aData); */
NS_IMETHODIMP
nsODBCStorageStatement::GetBlob(PRUint16 aIndex, PRUint32 *aDataSize, PRUint8 **aData)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    if (aIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;
    
    if (!mExecuting)
        return NS_ERROR_UNEXPECTED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariantMz(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    nsIID interfaceIID;

    m_Data->GetDataType(&type);
    if (type == nsIDataType::VTYPE_EMPTY) {
      *aDataSize = 0;
      *aData = nsnull;
    } else {
      m_Data->GetAsArray(&type, &interfaceIID, aDataSize, reinterpret_cast<void **>(aData));
    }

    
    return NS_OK;
}



/* boolean getIsNull (); */
NS_IMETHODIMP 
nsODBCStorageStatement::GetIsNull(PRUint16 aIndex, PRBool *_retval)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;

    nsresult rc;

    if (m_Data == nsnull || m_col != aIndex) {
      nsIVariant *val;
      rc = GetVariant(aIndex, &val);
      if (rc != NS_OK)
        return rc;
      NS_IF_RELEASE(val);
    }

    PRUint16 type;
    m_Data->GetDataType(&type);
    *_retval = type == nsIDataType::VTYPE_EMPTY;

    return NS_OK;
}



/* AString getColumnDecltype(in unsigned short aParamIndex); */
NS_IMETHODIMP
nsODBCStorageStatement::GetColumnDecltype(PRUint16 aParamIndex,
                                       nsAString& aDeclType)
{
    if (!mDBConnection)
        return NS_ERROR_NOT_INITIALIZED;

    if (!mPrepared && !m_hstmt) {
      nsresult rv = Prepare();
      NS_ENSURE_SUCCESS(rv, rv);
    }

    if (!m_hstmt)
      return NS_ERROR_NOT_INITIALIZED;
    
    if (aParamIndex >= mResultColumnCount)
        return NS_ERROR_ILLEGAL_VALUE;

    SQLWCHAR desc[512];
    SQLSMALLINT desclen;

    nsresult rc = CheckStmtRC(SQLColAttributesW (m_hstmt, aParamIndex+1, 
    	SQL_COLUMN_TYPE_NAME, (SQLPOINTER)desc, sizeof(desc), &desclen, NULL));
    if (rc != NS_OK)
      return rc;

    NS_WStringToUTF16(desc, aDeclType);

    return NS_OK;
}
