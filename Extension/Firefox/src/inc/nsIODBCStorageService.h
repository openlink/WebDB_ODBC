/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageService.idl
 */

#ifndef __gen_nsIODBCStorageService_h__
#define __gen_nsIODBCStorageService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIODBCStorageConnection; /* forward declaration */

class nsIVariant; /* forward declaration */


/* starting interface:    nsIODBCStorageService */
#define NS_IODBCSTORAGESERVICE_IID_STR "be9effb0-9b42-4258-b38e-2f3ce2aecac7"

#define NS_IODBCSTORAGESERVICE_IID \
  {0xbe9effb0, 0x9b42, 0x4258, \
    { 0xb3, 0x8e, 0x2f, 0x3c, 0xe2, 0xae, 0xca, 0xc7 }}

/**
 * The nsIODBCStorageService interface is intended to be implemented by
 * a service that can create storage connections (nsIStorageConnection)
 * to either a well-known profile database or to a specific database file.
 *
 * This is the only way to open a database connection.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageService : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGESERVICE_IID)

  /**
   * Get a connection to a named special database storage.
   *
   * @param aStorageKey a string key identifying the type of storage
   * requested.  Valid values include: "profile", "memory".
   *
   * @see openDatabase for restrictions on how database connections may be
   * used. For the profile database, you should only access it from the main
   * thread since other callers may also have connections.
   *
   * @returns a new nsIStorageConnection for the requested
   * storage database.
   *
   * @throws NS_ERROR_INVALID_ARG if aStorageKey is invalid.
   */
  /* nsIODBCStorageConnection openDatabase (in AString DSN, in AString UID, in AString PWD); */
  NS_SCRIPTABLE NS_IMETHOD OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageConnection openDriverDatabase (in AString connString); */
  NS_SCRIPTABLE NS_IMETHOD OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM) = 0;

  /**
   * The last ODBC error code.
   */
  /* readonly attribute long lastErrorCode; */
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) = 0;

  /**
   * The last ODBC error as a string 
   */
  /* readonly attribute AString lastErrorString; */
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) = 0;

  /**
   * The last ODBC error as a string 
   */
  /* readonly attribute AString lastErrorState; */
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) = 0;

  /* nsIVariant getDataSource (in boolean getFirst); */
  NS_SCRIPTABLE NS_IMETHOD GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageService, NS_IODBCSTORAGESERVICE_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGESERVICE \
  NS_SCRIPTABLE NS_IMETHOD OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState); \
  NS_SCRIPTABLE NS_IMETHOD GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGESERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM) { return _to OpenDatabase(DSN, UID, PWD, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM) { return _to OpenDriverDatabase(connString, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return _to GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return _to GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return _to GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM) { return _to GetDataSource(getFirst, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGESERVICE(_to) \
  NS_SCRIPTABLE NS_IMETHOD OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenDatabase(DSN, UID, PWD, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->OpenDriverDatabase(connString, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDataSource(getFirst, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageService : public nsIODBCStorageService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGESERVICE

  nsODBCStorageService();

private:
  ~nsODBCStorageService();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageService, nsIODBCStorageService)

nsODBCStorageService::nsODBCStorageService()
{
  /* member initializers and constructor code */
}

nsODBCStorageService::~nsODBCStorageService()
{
  /* destructor code */
}

/* nsIODBCStorageConnection openDatabase (in AString DSN, in AString UID, in AString PWD); */
NS_IMETHODIMP nsODBCStorageService::OpenDatabase(const nsAString & DSN, const nsAString & UID, const nsAString & PWD, nsIODBCStorageConnection **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageConnection openDriverDatabase (in AString connString); */
NS_IMETHODIMP nsODBCStorageService::OpenDriverDatabase(const nsAString & connString, nsIODBCStorageConnection **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long lastErrorCode; */
NS_IMETHODIMP nsODBCStorageService::GetLastErrorCode(PRInt32 *aLastErrorCode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorString; */
NS_IMETHODIMP nsODBCStorageService::GetLastErrorString(nsAString & aLastErrorString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorState; */
NS_IMETHODIMP nsODBCStorageService::GetLastErrorState(nsAString & aLastErrorState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant getDataSource (in boolean getFirst); */
NS_IMETHODIMP nsODBCStorageService::GetDataSource(PRBool getFirst, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageService_h__ */
