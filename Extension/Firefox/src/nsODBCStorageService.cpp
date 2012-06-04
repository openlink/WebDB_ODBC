
#include "nsODBCStorageService.h"
#include "nsODBCStorageConnection.h"
#include "nsCRT.h"
#include "plstr.h"
#include "prinit.h"
//??--#include "nsAutoLock.h"
#include "mozilla/Mutex.h"
#include "nsAutoPtr.h"
#include "nsEmbedCID.h"
#include "nsIObserverService.h"

#include "nsODBCUtils.h"

#include "nsODBCStorageVariant.h"



NS_IMPL_THREADSAFE_ISUPPORTS1(nsODBCStorageService, nsIODBCStorageService)

nsODBCStorageService *nsODBCStorageService::gODBCStorageService = nsnull;

nsODBCStorageService *
nsODBCStorageService::GetSingleton()
{
    if (gODBCStorageService) {
        NS_ADDREF(gODBCStorageService);
        return gODBCStorageService;
    }

    gODBCStorageService = new nsODBCStorageService();
    if (gODBCStorageService) {
        NS_ADDREF(gODBCStorageService);
    }

    return gODBCStorageService;
}


nsODBCStorageService::~nsODBCStorageService()
{
    if (m_henv != NULL) {
        SQLFreeHandle (SQL_HANDLE_ENV, m_henv);
        m_henv = NULL;
    }
    gODBCStorageService = nsnull;
}


////////////////////////////////////////////////////////////////////////////////
//// nsIODBCStorageService


/* nsIODBCStorageConnection openDatabase (in AString DSN, in AString UID, in AString PWD); */
NS_IMETHODIMP 
nsODBCStorageService::OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM)
{
    nsresult rv;

    nsODBCStorageConnection *msc = new nsODBCStorageConnection(this);
    if (!msc)
        return NS_ERROR_OUT_OF_MEMORY;

    rv = msc->Initialize (DSN, UID, PWD);

    msc->GetLastErrorCode(&m_errorCode);
    msc->GetLastErrorString(m_errorMessage);
    msc->GetLastErrorState(m_errorState);

    NS_ENSURE_SUCCESS(rv, rv);

    NS_ADDREF(*_retval = msc);
    return NS_OK;
}

/* nsIODBCStorageConnection openDriverDatabase (in AString connString); */
NS_IMETHODIMP
nsODBCStorageService::OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM)
{
    nsresult rv;

    nsODBCStorageConnection *msc = new nsODBCStorageConnection(this);
    if (!msc)
        return NS_ERROR_OUT_OF_MEMORY;

    rv = msc->Initialize (connString);

    msc->GetLastErrorCode(&m_errorCode);
    msc->GetLastErrorString(m_errorMessage);
    msc->GetLastErrorState(m_errorState);

    NS_ENSURE_SUCCESS(rv, rv);

    NS_ADDREF(*_retval = msc);
    return NS_OK;
}

/* readonly attribute long lastErrorCode; */
NS_IMETHODIMP 
nsODBCStorageService::GetLastErrorCode(PRInt32 *aLastErrorCode)
{
    *aLastErrorCode = m_errorCode;
    return NS_OK;
}

/* readonly attribute AString lastErrorString; */
NS_IMETHODIMP 
nsODBCStorageService::GetLastErrorString(nsAString & aLastErrorString)
{
    aLastErrorString.Assign(m_errorMessage);
    return NS_OK;
}

/* readonly attribute AString lastErrorState; */
NS_IMETHODIMP 
nsODBCStorageService::GetLastErrorState(nsAString & aLastErrorState)
{
    aLastErrorState.Assign(m_errorState);
    return NS_OK;
}

/* AString getDataSource (in boolean getFirst); */
NS_IMETHODIMP 
nsODBCStorageService::GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM)
{
  SQLWCHAR dsnname[SQL_MAX_DSN_LENGTH+1];
  SQLWCHAR dsndesc[SQL_MAX_DSN_LENGTH+1];
  SQLRETURN ret;
  SQLSMALLINT len;
  nsIVariant *val = nsnull;

  if (m_henv == NULL) {
    ret = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
      goto error;
    SQLSetEnvAttr (m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);
  }

  ret = SQLDataSourcesW (m_henv, (getFirst ? SQL_FETCH_FIRST:SQL_FETCH_NEXT), 
      dsnname, sizeof (dsnname)/sizeof(wchar_t), 
      &len, dsndesc, sizeof (dsndesc)/sizeof(wchar_t), NULL);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
    goto error;

  if (ret != SQL_NO_DATA) {
    nsString v_str;
    NS_WStringToUTF16(dsnname, v_str);
    val = new nsODBCStorageText(v_str);

  } else {
    val = new nsODBCStorageNull();
  }
  NS_IF_ADDREF(*_retval = val);  
  return NS_OK;

error:
  return NS_ERROR_FAILURE;
}
