
#include <stdio.h>

#include "nsError.h"
#include "nsIMutableArray.h"
#include "nsAutoPtr.h"
#include "nsIVariant.h"
#include "nsIPrefService.h"
#include "nsIPrefBranch.h"
#include "nsThreadUtils.h"


#include "nsODBCStorageEvents.h"
#include "nsODBCStorageConnection.h"
#include "nsODBCStorageService.h"
#include "nsODBCStorageStatement.h"

#include "prprf.h"

#ifdef PR_LOGGING
PRLogModuleInfo* gStorageLog = nsnull;
#endif


NS_IMPL_THREADSAFE_ISUPPORTS1(nsODBCStorageConnection, nsIODBCStorageConnection)

nsODBCStorageConnection::nsODBCStorageConnection(nsIODBCStorageService* aService) :
    m_henv(nsnull)
,   m_hdbc(nsnull)
,   mAsyncExecutionThreadShuttingDown(PR_FALSE)
,   mTransactionInProgress(PR_FALSE)
,   mODBCStorageService(aService)
{
}

nsODBCStorageConnection::~nsODBCStorageConnection()
{
    (void)Close();
}




NS_IMETHODIMP
nsODBCStorageConnection::CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc)
{
  m_errorCode = 0;
  m_errorState.Truncate();
  m_errorMessage.Truncate();

  if (!SQL_SUCCEEDED(rc)) {

    StoreError(fHandleType, handle, m_errorCode, m_errorMessage, m_errorState);  
    return NS_ERROR_FAILURE;

  } else {

    return NS_OK;
  }
}


NS_IMETHODIMP
nsODBCStorageConnection::CheckDbcRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_DBC, m_hdbc, rc);
}

NS_IMETHODIMP
nsODBCStorageConnection::CheckEnvRC(SQLRETURN rc)
{
  return CheckRC(SQL_HANDLE_ENV, m_henv, rc);
}


NS_IMETHODIMP
nsODBCStorageConnection::Initialize(const nsAString & connString)
{
    NS_ASSERTION (!m_hdbc, "Initialize called on already opened database!");
	
    nsresult rc;
    mconnString.Assign(connString);

    // Allocate the environment handle
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE, &m_henv));
    if (rc != NS_OK) {
      return rc;
    }
		
    // Set the enviroment attributes
    rc = CheckEnvRC(SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER));
    if (rc != NS_OK) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = nsnull; 		
      return rc;
    }	

    // Allocate a new database Connection Handle.
    rc = CheckEnvRC(SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc));
    if (rc != NS_OK) {
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);		
      m_henv = nsnull; 		
      return rc;
    }

    // set the connection attributes
    rc = CheckEnvRC(SQLSetConnectAttr(m_hdbc, SQL_LOGIN_TIMEOUT, (void *)5, 0));
    if (rc != NS_OK)
      goto failed;

    SQLWCHAR connOut[4096];    
    short buflen;

    rc = CheckDbcRC(SQLDriverConnectW (m_hdbc, 0, nsWString(connString).Data(), 
       SQL_NTS, connOut, NUMCHAR(connOut), &buflen, SQL_DRIVER_COMPLETE));

    // Free all the allocated handles if the SQLConnect call is unsuccessful
    if (rc != NS_OK)
      goto failed;

    mconnString.Assign(connString);
    NS_WStringToUTF16(connOut, mconnOutString);

    mIdentCase = 0;

    PRBool v;
    SQLSMALLINT retLen;
    rc = CheckDbcRC(SQLGetInfoW (m_hdbc, SQL_IDENTIFIER_CASE, &mIdentCase,
			(SQLSMALLINT) sizeof (mIdentCase), &retLen));
    if (rc != NS_OK)
      goto failed;

    rc = CheckDbcRC(SQLGetConnectOption (m_hdbc, SQL_ACCESS_MODE, &mAccessMode));
    if (rc != NS_OK)
      goto failed;

    if (mAccessMode == SQL_MODE_READ_ONLY)
      return NS_OK; 
    
    if (mIdentCase == SQL_IC_LOWER) {
      TableExists(NS_LITERAL_STRING("nsiodbc_version"), &v);
      if (!v) {
        ExecuteSimpleSQL(NS_LITERAL_STRING("drop table nsiodbc_version"));
        rc = ExecuteSimpleSQL(NS_LITERAL_STRING("create table nsiodbc_version(ver integer)"));
        if (rc==NS_OK)
          ExecuteSimpleSQL(NS_LITERAL_STRING("insert into nsiodbc_version values(0)"));
      }
    } else {
      TableExists(NS_LITERAL_STRING("NSIODBC_VERSION"), &v);
      if (!v) {
        ExecuteSimpleSQL(NS_LITERAL_STRING("drop table NSIODBC_VERSION"));
        rc = ExecuteSimpleSQL(NS_LITERAL_STRING("create table NSIODBC_VERSION(VER integer)"));
        if (rc==NS_OK)
          ExecuteSimpleSQL(NS_LITERAL_STRING("insert into NSIODBC_VERSION values(0)"));
      }
    }
    
    return NS_OK;

failed:
    SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
    m_hdbc = nsnull; 				
    m_henv = nsnull; 		
    return rc;
}

NS_IMETHODIMP
nsODBCStorageConnection::Initialize(const nsAString & DSN, const nsAString & UID, const nsAString & PWD)
{
    nsString connStr;

    connStr.AssignLiteral("DSN=");
    connStr.Append(DSN);
    connStr.AppendLiteral(";UID=");
    connStr.Append(UID);
    connStr.AppendLiteral(";PWD=");
    connStr.Append(PWD);
    connStr.AppendLiteral(";");

    nsresult rc = Initialize(connStr);
    if (rc == NS_OK) {
      mDSN.Assign(DSN);
      mUID.Assign(UID);
      mPWD.Assign(PWD);
    }
    return rc;
}

/*****************************************************************************
 ** nsIStorageConnection interface
 *****************************************************************************/

/**
 ** Core status/initialization
 **/

NS_IMETHODIMP
nsODBCStorageConnection::Close()
{
    nsresult rc;

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;


    // Flag that we are shutting down the async thread, so that
    // getAsyncExecutionTarget knows not to expose/create the async thread.
    {
        nsODBCAutoLock mutex(mAsyncExecutionMutex);
        mAsyncExecutionThreadShuttingDown = PR_TRUE;
    }

    // Shutdown the async thread if it exists.  (Because we just set the flag,
    // we are the only code that is going to be touching this variable from here
    // on out.)
    if (mAsyncExecutionThread) {
        mAsyncExecutionThread->Shutdown();
        mAsyncExecutionThread = nsnull;
    }


    rc = CheckEnvRC(SQLDisconnect(m_hdbc));
    if (rc != NS_OK) {
      return rc;
    }

    // free the database handle
    if (m_hdbc != nsnull) 
      SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);

	// free the environment handle
    if (m_henv != nsnull) 
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

    m_hdbc = nsnull; 				
    m_henv = nsnull; 

    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageConnection::GetConnectionReady(PRBool *aConnectionReady)
{
    *aConnectionReady = (m_hdbc != nsnull);
    return NS_OK;
}


/* readonly attribute AString DSN; */
NS_IMETHODIMP 
nsODBCStorageConnection::GetDSN(nsAString & aDSN)
{
    aDSN.Assign(mDSN);
    return NS_OK;
}

/* readonly attribute AString PWD; */
NS_IMETHODIMP 
nsODBCStorageConnection::GetPWD(nsAString & aPWD)
{
    aPWD.Assign(mPWD);
    return NS_OK;
}

/* readonly attribute AString UID; */
NS_IMETHODIMP 
nsODBCStorageConnection::GetUID(nsAString & aUID)
{
    aUID.Assign(mUID);
    return NS_OK;
}

/* readonly attribute AString connString; */
NS_IMETHODIMP 
nsODBCStorageConnection::GetConnString(nsAString & aConnString)
{
    aConnString.Assign(mconnString);
    return NS_OK;
}




NS_IMETHODIMP
nsODBCStorageConnection::GetLastInsertRowID(PRInt64 *aLastInsertRowID)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    *aLastInsertRowID = 0L;

    char buf[256];
    SQLSMALLINT len;

    int ret = SQLGetInfo(m_hdbc, SQL_DBMS_NAME, buf, sizeof(buf), &len);
    if (ret == SQL_SUCCESS && vstrncmp(buf, "OpenLink Virtuoso",17)==0) {
      nsCOMPtr<nsIODBCStorageStatement> stmt;
      nsresult rv = CreateStatement(NS_LITERAL_STRING(
          "select {fn convert(identity_value(), SQL_INTEGER)}"), getter_AddRefs(stmt));
      if (NS_SUCCEEDED(rv)) {
        PRBool hasResult;
        rv = stmt->ExecuteStep(&hasResult);
        if (NS_SUCCEEDED(rv) && hasResult)
            stmt->GetInt64(0, aLastInsertRowID);
        else
            rv = NS_ERROR_FAILURE;
      }
    }
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageConnection::GetLastErrorCode(PRInt32 *aLastError)
{
    *aLastError = m_errorCode;
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageConnection::GetLastErrorString(nsAString& aLastErrorString)
{
    aLastErrorString.Assign(m_errorMessage);
    return NS_OK;
}

/* readonly attribute AString lastErrorState */
NS_IMETHODIMP nsODBCStorageConnection::GetLastErrorState(nsAString & aLastErrorState)
{
    aLastErrorState.Assign(m_errorState);
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageConnection::GetSchemaVersion(PRInt32 *version)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    *version = 0;
    nsCOMPtr<nsIODBCStorageStatement> stmt;
    nsAutoString str;

    str.AssignLiteral(mIdentCase==SQL_IC_LOWER?"select ver from nsiodbc_version":"select VER from NSIODBC_VERSION");
    nsresult rv = CreateStatement(str, getter_AddRefs(stmt));
    if (NS_SUCCEEDED(rv)) {
      PRBool hasResult;
      rv = stmt->ExecuteStep(&hasResult);
      if (NS_SUCCEEDED(rv) && hasResult)
          stmt->GetInt32(0, version);
      else
          rv = NS_ERROR_FAILURE;
    }
    return rv;
}

NS_IMETHODIMP
nsODBCStorageConnection::SetSchemaVersion(PRInt32 aVersion)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsAutoString str;

    str.AssignLiteral(mIdentCase==SQL_IC_LOWER?"update nsiodbc_version set ver= ":"update NSIODBC_VERSION set VER= ");
    str.AppendInt(aVersion);

    nsresult rv = ExecuteSimpleSQL(str);
    return rv;
}


/* attribute boolean readOnlyMode; */
NS_IMETHODIMP 
nsODBCStorageConnection::GetReadOnlyMode(PRBool *aReadOnlyMode)
{
    nsresult rc;
    SQLUINTEGER mode;
    SQLINTEGER  ind;

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLGetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, &mode, 0, &ind));
    if (rc != NS_OK)
      return rc;

    *aReadOnlyMode = (mode == SQL_MODE_READ_ONLY? PR_TRUE: PR_FALSE);

    return rc;
}

NS_IMETHODIMP 
nsODBCStorageConnection::SetReadOnlyMode(PRBool aReadOnlyMode)
{
    nsresult rc;
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLSetConnectAttr(m_hdbc, SQL_ATTR_ACCESS_MODE, 
    		(SQLPOINTER)(aReadOnlyMode ? SQL_MODE_READ_ONLY:SQL_MODE_READ_WRITE), 0));
    return rc;
}




/**
 ** Statements & Queries
 **/

NS_IMETHODIMP
nsODBCStorageConnection::CreateStatement(const nsAString& aSQLStatement,
                                      nsIODBCStorageStatement **_retval)
{
    NS_ENSURE_ARG_POINTER(_retval);
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    nsresult rv = statement->Initialize (this, aSQLStatement);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        return rv;
    }

    rv = statement->Prepare ();
    if (NS_FAILED(rv)) {
        m_errorCode = statement->m_errorCode;
        m_errorState = statement->m_errorState;
        m_errorMessage = statement->m_errorMessage;  
        NS_RELEASE(statement);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageConnection::ExecuteSimpleSQL(const nsAString& aSQLStatement)
{
    nsresult rc;
    SQLHANDLE hstmt; 
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NS_OK) {
      return rc;
    }

    rc = CheckRC(SQL_HANDLE_STMT, hstmt, SQLExecDirectW(hstmt, nsWString(aSQLStatement).Data(), SQL_NTS));
    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}


/* long executeSQL (in AString aSQLStatement); */
NS_IMETHODIMP 
nsODBCStorageConnection::ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval)
{
    nsresult rc;
    SQLHANDLE hstmt; 
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NS_OK) {
      return rc;
    }

    rc = CheckRC(SQL_HANDLE_STMT, hstmt, SQLExecDirectW(hstmt, nsWString(aSQLStatement).Data(), SQL_NTS));
    if (rc != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rc;
    }

    SQLLEN count;
    rc = CheckRC(SQL_HANDLE_STMT, hstmt, SQLRowCount(hstmt, &count));
    *_retval = count;
    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    return rc;
}


NS_IMETHODIMP
nsODBCStorageConnection::ExecuteAsync(nsIODBCStorageStatement ** aStatements,
                                   PRUint32 aNumStatements,
                                   nsIODBCStorageStatementCallback *aCallback,
                                   nsIODBCStoragePendingStatement **_stmt)
{
    nsCOMArray<nsIODBCStorageStatement> stmts(aNumStatements);
    
    nsresult rc = NS_OK;
    for (PRUint32 i = 0; i < aNumStatements && rc == NS_OK; i++) {

        nsIODBCStorageStatement *new_stmt = aStatements[i]->DupForAsync();

        SQLHANDLE hdbc;
        new_stmt->GetConnectionHandle(&hdbc);

        NS_ASSERTION(hdbc == m_hdbc, "Statement must be from this database connection!");

        if (!new_stmt) {
            rc = NS_ERROR_OUT_OF_MEMORY;
            break;
        }

        if (!stmts.AppendObject(new_stmt)) {
            rc = NS_ERROR_OUT_OF_MEMORY;
            break;
        }
    }

    // Dispatch to the background
    if (rc == NS_OK)
        rc = NS_executeAsync(stmts, this, aCallback, _stmt);

    // Always reset all the statements
    for (PRUint32 i = 0; i < aNumStatements; i++)
        (void)aStatements[i]->Reset();

    return rc;
}


/* boolean tableExists (in AString aTableName); */
NS_IMETHODIMP
nsODBCStorageConnection::TableExists(const nsAString& aTableName, PRBool *_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rc;
    SQLHANDLE hstmt; 
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NS_OK) {
      return rc;
    }

    rc = CheckRC(SQL_HANDLE_STMT, hstmt, SQLTablesW(hstmt, NULL, 0, NULL, 0,
      nsWString(aTableName).Data(), SQL_NTS, NULL, 0));
    if (rc != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rc;
    }

    PRBool exists = PR_FALSE;
    SQLRETURN ret = SQLFetch(hstmt);

    if (SQL_SUCCEEDED(ret)) {
      exists = PR_TRUE;

    } else if (ret == SQL_NO_DATA_FOUND) {
      exists = PR_FALSE;

    } else {
      StoreError(SQL_HANDLE_STMT, hstmt, m_errorCode, m_errorMessage, m_errorState);  
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return NS_ERROR_FAILURE;
    }

    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);

    *_retval = exists;
    return NS_OK;
}



/* nsIODBCStorageStatement getTables (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aTableType); */
NS_IMETHODIMP 
nsODBCStorageConnection::GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);
    nsWString ttype(aTableType);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLTablesW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0),
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      tbl.Data(), (tbl.Data()?SQL_NTS:0), 
      ttype.Data(), (ttype.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}


/* nsIODBCStorageStatement getColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
NS_IMETHODIMP nsODBCStorageConnection::GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);
    nsWString col(aColumnName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLColumnsW(hstmt, 
    	cat.Data(), (cat.Data()?SQL_NTS:0),
      	sch.Data(), (sch.Data()?SQL_NTS:0), 
      	tbl.Data(), (tbl.Data()?SQL_NTS:0), 
      	col.Data(), (col.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getTypeInfo (in short aDataType); */
NS_IMETHODIMP nsODBCStorageConnection::GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLGetTypeInfo(hstmt, aDataType));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getPrimaryKeys (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLPrimaryKeysW(hstmt, 
    	cat.Data(), (cat.Data()?SQL_NTS:0),
      	sch.Data(), (sch.Data()?SQL_NTS:0), 
      	tbl.Data(), (tbl.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getForeignKeys (in nsIVariant aPkCatalogName, in nsIVariant aPkSchemaName, in nsIVariant aPkTableName, in nsIVariant aFkCatalogName, in nsIVariant aFkSchemaName, in nsIVariant aFkTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString pcat(aPkCatalogName);
    nsWString psch(aPkSchemaName);
    nsWString ptbl(aPkTableName);

    nsWString fcat(aFkCatalogName);
    nsWString fsch(aFkSchemaName);
    nsWString ftbl(aFkTableName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLForeignKeysW(hstmt, 
      pcat.Data(), (pcat.Data()?SQL_NTS:0), 
      psch.Data(), (psch.Data()?SQL_NTS:0), 
      ptbl.Data(), (ptbl.Data()?SQL_NTS:0), 
      fcat.Data(), (fcat.Data()?SQL_NTS:0), 
      fsch.Data(), (fsch.Data()?SQL_NTS:0), 
      ftbl.Data(), (ftbl.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getSpecialColumns (in unsigned short aIdentifierType, in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aScope, in unsigned short aNullable); */
NS_IMETHODIMP nsODBCStorageConnection::GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLSpecialColumnsW(hstmt, aIdentifierType,
      cat.Data(), (cat.Data()?SQL_NTS:0), 
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      tbl.Data(), (tbl.Data()?SQL_NTS:0), 
      aScope, aNullable));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getStatistics (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aUnique); */
NS_IMETHODIMP nsODBCStorageConnection::GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLStatisticsW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0), 
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      tbl.Data(), (tbl.Data()?SQL_NTS:0), 
      aUnique, 0));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getProcedures (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName); */
NS_IMETHODIMP nsODBCStorageConnection::GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString proc(aProcName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLProceduresW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0), 
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      proc.Data(), (proc.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getProcedureColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName, in nsIVariant aColName); */
NS_IMETHODIMP nsODBCStorageConnection::GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString proc(aProcName);
    nsWString col(aColName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLProcedureColumnsW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0), 
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      proc.Data(), (proc.Data()?SQL_NTS:0), 
      col.Data(), (col.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getTablePrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLTablePrivilegesW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0), 
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      tbl.Data(), (tbl.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}

/* nsIODBCStorageStatement getColumnsPrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
NS_IMETHODIMP nsODBCStorageConnection::GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rv;
    SQLHANDLE hstmt; 

    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rv = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rv != NS_OK) {
      return rv;
    }

    nsWString cat(aCatalogName);
    nsWString sch(aSchemaName);
    nsWString tbl(aTableName);
    nsWString col(aColumnName);

    rv = CheckRC(SQL_HANDLE_STMT, hstmt, SQLColumnPrivilegesW(hstmt, 
      cat.Data(), (cat.Data()?SQL_NTS:0),
      sch.Data(), (sch.Data()?SQL_NTS:0), 
      tbl.Data(), (tbl.Data()?SQL_NTS:0), 
      col.Data(), (col.Data()?SQL_NTS:0)));
    if (rv != NS_OK) {
      SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
      return rv;
    }

    nsODBCStorageStatement *statement = new nsODBCStorageStatement();
    if (!statement)
      return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(statement);

    rv = statement->Initialize (this, hstmt);
    if (NS_FAILED(rv)) {
        NS_RELEASE(statement);
        SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
        return rv;
    }

    *_retval = (nsIODBCStorageStatement *)statement;
    return NS_OK;
}




/**
 ** Transactions
 **/

NS_IMETHODIMP
nsODBCStorageConnection::GetTransactionInProgress(PRBool *_retval)
{
    nsODBCAutoLock mutex(mTransactionMutex);
    *_retval = mTransactionInProgress;
    return NS_OK;
}


NS_IMETHODIMP
nsODBCStorageConnection::BeginTransaction()
{
    nsODBCAutoLock mutex(mTransactionMutex);
    if (mTransactionInProgress)
        return NS_ERROR_FAILURE;

    nsresult rc;
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0));
    if (rc == NS_OK) 
      mTransactionInProgress = PR_TRUE;

    return rc;
}

NS_IMETHODIMP
nsODBCStorageConnection::BeginTransactionAs(PRInt32 aTransactionType)
{
    nsODBCAutoLock mutex(mTransactionMutex);
    if (mTransactionInProgress)
        return NS_ERROR_FAILURE;

    nsresult rc;
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    SQLINTEGER level;
    switch(aTransactionType) {
        case TRANSACTION_READ_UNCOMMITTED:
          level = SQL_TXN_READ_UNCOMMITTED;
          break;
        case TRANSACTION_READ_COMMITTED:
          level = SQL_TXN_READ_COMMITTED;
          break;
        case TRANSACTION_REPEATABLE_READ:
          level = SQL_TXN_REPEATABLE_READ;
          break;
        case TRANSACTION_SERIALIZABLE:
          level = SQL_TXN_SERIALIZABLE;
          break;
        default:
          return NS_ERROR_ILLEGAL_VALUE;
    }

    rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, 0));
    if (rc != NS_OK)
        return rc;

    mTransactionInProgress = PR_TRUE;

    rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_TXN_ISOLATION, (SQLPOINTER)level, 0));
    if (!NS_SUCCEEDED(rc)) {
      rc = CheckDbcRC(SQLSetConnectAttrW(m_hdbc, SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0));
    }
    return rc;
}

NS_IMETHODIMP
nsODBCStorageConnection::CommitTransaction()
{
    nsODBCAutoLock mutex(mTransactionMutex);
    if (!mTransactionInProgress)
        return NS_ERROR_FAILURE;

    nsresult rc;
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT));
    if (NS_SUCCEEDED(rc))
        mTransactionInProgress = PR_FALSE;
    return rc;
}

NS_IMETHODIMP
nsODBCStorageConnection::RollbackTransaction()
{
    nsODBCAutoLock mutex(mTransactionMutex);
    if (!mTransactionInProgress)
        return NS_ERROR_FAILURE;

    nsresult rc;
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    rc = CheckDbcRC(SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK));
    if (NS_SUCCEEDED(rc))
        mTransactionInProgress = PR_FALSE;
    return rc;
}


/**
 ** Table creation
 **/
NS_IMETHODIMP
nsODBCStorageConnection::CreateTable(/*const nsID& aID,*/
                                  const char *aTableName,
                                  const char *aTableSchema)
{
    if (!m_hdbc)
        return NS_ERROR_NOT_INITIALIZED;

    nsresult rc;
    char *buf;
    SQLHANDLE hstmt; 

    rc = CheckDbcRC(SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt));
    if (rc != NS_OK) {
      return rc;
    }

    buf = PR_smprintf("CREATE TABLE %s (%s)", aTableName, aTableSchema);
    if (!buf)
        return NS_ERROR_OUT_OF_MEMORY;

    rc = CheckRC(SQL_HANDLE_STMT, hstmt, SQLExecDirectA(hstmt, (SQLCHAR*)buf, SQL_NTS));
    SQLFreeHandle (SQL_HANDLE_STMT, hstmt);
    PR_smprintf_free(buf);

    return rc;
}



/**
 ** Other bits
 **/

already_AddRefed<nsIEventTarget>
nsODBCStorageConnection::getAsyncExecutionTarget()
{
    nsODBCAutoLock mutex(mAsyncExecutionMutex);
    
    // If we are shutting down the asynchronous thread, don't hand out any more
    // references to the thread. 
    if (mAsyncExecutionThreadShuttingDown)
        return nsnull;

    if (!mAsyncExecutionThread) {
        nsresult rv = NS_NewThread(getter_AddRefs(mAsyncExecutionThread));
        if (NS_FAILED(rv)) {
            NS_WARNING("Failed to create async thread.");
            return nsnull;
        }
    }

    nsIEventTarget *eventTarget;
    NS_ADDREF(eventTarget = mAsyncExecutionThread);
    return eventTarget;
}

