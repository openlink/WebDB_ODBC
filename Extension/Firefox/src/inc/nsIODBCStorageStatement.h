/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageStatement.idl
 */

#ifndef __gen_nsIODBCStorageStatement_h__
#define __gen_nsIODBCStorageStatement_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIODBCStorageValueArray_h__
#include "nsIODBCStorageValueArray.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIODBCStorageConnection; /* forward declaration */

class nsISimpleEnumerator; /* forward declaration */

class nsIODBCStorageStatementCallback; /* forward declaration */

class nsIODBCStoragePendingStatement; /* forward declaration */

class nsIVariant; /* forward declaration */

#include "nsODBCParam.h"

/* starting interface:    nsIODBCStorageStatement */
#define NS_IODBCSTORAGESTATEMENT_IID_STR "f3da6881-30a7-4ef0-a554-3a178b14017a"

#define NS_IODBCSTORAGESTATEMENT_IID \
  {0xf3da6881, 0x30a7, 0x4ef0, \
    { 0xa5, 0x54, 0x3a, 0x17, 0x8b, 0x14, 0x01, 0x7a }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageStatement : public nsIODBCStorageValueArray {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGESTATEMENT_IID)

  /**
   * Finalizes a statement so you can successfully close a database connection.
   * This method does not need to be used from native callers since you can just
   * set the statement to null, but is extremely useful to JS callers.
   */
  /* void finalize (); */
  NS_SCRIPTABLE NS_IMETHOD Finalize(void) = 0;

  /**
   * Create a clone of this statement, by initializing a new statement
   * with the same connection and same SQL statement as this one.  It
   * does not preserve statement state; that is, if a statement is
   * being executed when it is cloned, the new statement will not be
   * executing.
   */
  /* nsIODBCStorageStatement clone (); */
  NS_SCRIPTABLE NS_IMETHOD Clone(nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* [noscript, notxpcom] nsIODBCStorageStatement dupForAsync (); */
  NS_IMETHOD_(nsIODBCStorageStatement *) DupForAsync(void) = 0;

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

  /* readonly attribute unsigned short parameterCount; */
  NS_SCRIPTABLE NS_IMETHOD GetParameterCount(PRUint16 *aParameterCount) = 0;

  /**
   * Number of columns returned
   */
  /* readonly attribute unsigned short columnCount; */
  NS_SCRIPTABLE NS_IMETHOD GetColumnCount(PRUint16 *aColumnCount) = 0;

  /**
   * Name of nth column
   */
  /* AString getColumnName (in unsigned short aColumnIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetColumnName(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) = 0;

  /**
   * Obtains the index of the column with the specified name.
   *
   * @param aName The name of the column.
   * @return The index of the column with the specified name.
   */
  /* unsigned short getColumnIndex (in AString aName); */
  NS_SCRIPTABLE NS_IMETHOD GetColumnIndex(const nsAString & aName, PRUint16 *_retval NS_OUTPARAM) = 0;

  /**
   * Obtains the declared column type of a prepared statement.
   *
   * @param aColumnIndex The zero-based index of the column who's declared type
   *                    we are interested in.
   * @returns the declared index type.
   */
  /* AString getColumnDecltype (in unsigned short aColumnIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetColumnDecltype(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) = 0;

  /**
   * Reset parameters/statement execution
   */
  /* void reset (); */
  NS_SCRIPTABLE NS_IMETHOD Reset(void) = 0;

  /* [noscript] void setParams (in ODBCParam params); */
  NS_IMETHOD SetParams(nsCOMArray<nsODBCParam> & params) = 0;

  /**
   * Bind the given value to the parameter at aParamIndex. Index 0
   * denotes the first numbered argument or ?1.
   */
  /* void bindParameter (in unsigned short aParamIndex, in nsIVariant aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindParameter(PRUint16 aParamIndex, nsIVariant *aValue) = 0;

  /* void bindStringParameter (in unsigned short aParamIndex, in AString aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue) = 0;

  /* void bindLongStringParameter (in unsigned short aParamIndex, in AString aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue) = 0;

  /* void bindFloatParameter (in unsigned short aParamIndex, in float aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindFloatParameter(PRUint16 aParamIndex, float aValue) = 0;

  /* void bindDoubleParameter (in unsigned short aParamIndex, in double aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindDoubleParameter(PRUint16 aParamIndex, double aValue) = 0;

  /* void bindInt16Parameter (in unsigned short aParamIndex, in short aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue) = 0;

  /* void bindInt32Parameter (in unsigned short aParamIndex, in long aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue) = 0;

  /* void bindInt64Parameter (in unsigned short aParamIndex, in long long aValue); */
  NS_SCRIPTABLE NS_IMETHOD BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue) = 0;

  /* void bindNullParameter (in unsigned short aParamIndex); */
  NS_SCRIPTABLE NS_IMETHOD BindNullParameter(PRUint16 aParamIndex) = 0;

  /* void bindBlobParameter (in unsigned short aParamIndex, [array, size_is (aValueSize), const] in octet aValue, in unsigned long aValueSize); */
  NS_SCRIPTABLE NS_IMETHOD BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) = 0;

  /* void bindBinaryParameter (in unsigned short aParamIndex, [array, size_is (aValueSize), const] in octet aValue, in unsigned long aValueSize); */
  NS_SCRIPTABLE NS_IMETHOD BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) = 0;

  /* void executeSql (); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteSql(void) = 0;

  /* boolean moreResults (); */
  NS_SCRIPTABLE NS_IMETHOD MoreResults(PRBool *_retval NS_OUTPARAM) = 0;

  /* boolean fetch (); */
  NS_SCRIPTABLE NS_IMETHOD Fetch(PRBool *_retval NS_OUTPARAM) = 0;

  /* readonly attribute long rowCount; */
  NS_SCRIPTABLE NS_IMETHOD GetRowCount(PRInt32 *aRowCount) = 0;

  /**
   * Execute the query, ignoring any results.  This is accomplished by
   * calling step() once, and then calling reset().
   *
   * Error and last insert info, etc. are available from
   * the nsODBCStorageConnection.
   */
  /* void execute (); */
  NS_SCRIPTABLE NS_IMETHOD Execute(void) = 0;

  /**
   * Execute a query, using any currently-bound parameters.  Reset
   * must be called on the statement after the last call of
   * executeStep.
   *
   * @returns a boolean indicating whether there are more rows or not;
   * row data may be accessed using nsODBCIStorageValueArray methods on
   * the statement.
   *
   */
  /* boolean executeStep (); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteStep(PRBool *_retval NS_OUTPARAM) = 0;

  /**
   * Execute a query asynchronously using any currently bound parameters.  This
   * statement can be reused immediately, and reset does not need to be called.
   *
   * Note:  If you have any custom defined functions, they must be re-entrant
   *        since they can be called on multiple threads.
   *
   * @param aCallback [optional]
   *        The callback object that will be notified of progress, errors, and
   *        completion.
   * @returns an object that can be used to cancel the statements execution.
   */
  /* nsIODBCStoragePendingStatement executeAsync ([optional] in nsIODBCStorageStatementCallback aCallback); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) = 0;

  /**
   * The current state.  Row getters are only valid while
   * the statement is in the "executing" state.
   */
  enum { NS_ODBC_STORAGE_STATEMENT_INVALID = 0 };

  enum { NS_ODBC_STORAGE_STATEMENT_READY = 1 };

  enum { NS_ODBC_STORAGE_STATEMENT_EXECUTING = 2 };

  /* readonly attribute long state; */
  NS_SCRIPTABLE NS_IMETHOD GetState(PRInt32 *aState) = 0;

  /* [noscript, notxpcom] readonly attribute ConnHandle connectionHandle; */
  NS_IMETHOD GetConnectionHandle(SQLHANDLE *aConnectionHandle) = 0;

  /**
   * Escape a string for SQL LIKE search.
   *
   * @param     aValue the string to escape for SQL LIKE 
   * @param     aEscapeChar the escape character
   * @returns   an AString of an escaped version of aValue
   *            (%, _ and the escape char are escaped with the escape char)
   *            For example, we will convert "foo/bar_baz%20cheese" 
   *            into "foo//bar/_baz/%20cheese" (if the escape char is '/').
   * @note      consumers will have to use same escape char
   *            when doing statements such as:   ...LIKE '?1' ESCAPE '/'...
   */
  /* AString escapeStringForLIKE (in AString aValue, in wchar aEscapeChar); */
  NS_SCRIPTABLE NS_IMETHOD EscapeStringForLIKE(const nsAString & aValue, PRUnichar aEscapeChar, nsAString & _retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageStatement, NS_IODBCSTORAGESTATEMENT_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGESTATEMENT \
  NS_SCRIPTABLE NS_IMETHOD Finalize(void); \
  NS_SCRIPTABLE NS_IMETHOD Clone(nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_IMETHOD_(nsIODBCStorageStatement *) DupForAsync(void); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState); \
  NS_SCRIPTABLE NS_IMETHOD GetParameterCount(PRUint16 *aParameterCount); \
  NS_SCRIPTABLE NS_IMETHOD GetColumnCount(PRUint16 *aColumnCount); \
  NS_SCRIPTABLE NS_IMETHOD GetColumnName(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetColumnIndex(const nsAString & aName, PRUint16 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetColumnDecltype(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Reset(void); \
  NS_IMETHOD SetParams(nsCOMArray<nsODBCParam> & params); \
  NS_SCRIPTABLE NS_IMETHOD BindParameter(PRUint16 aParamIndex, nsIVariant *aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindFloatParameter(PRUint16 aParamIndex, float aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindDoubleParameter(PRUint16 aParamIndex, double aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue); \
  NS_SCRIPTABLE NS_IMETHOD BindNullParameter(PRUint16 aParamIndex); \
  NS_SCRIPTABLE NS_IMETHOD BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize); \
  NS_SCRIPTABLE NS_IMETHOD BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSql(void); \
  NS_SCRIPTABLE NS_IMETHOD MoreResults(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Fetch(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetRowCount(PRInt32 *aRowCount); \
  NS_SCRIPTABLE NS_IMETHOD Execute(void); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteStep(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetState(PRInt32 *aState); \
  NS_IMETHOD GetConnectionHandle(SQLHANDLE *aConnectionHandle); \
  NS_SCRIPTABLE NS_IMETHOD EscapeStringForLIKE(const nsAString & aValue, PRUnichar aEscapeChar, nsAString & _retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGESTATEMENT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Finalize(void) { return _to Finalize(); } \
  NS_SCRIPTABLE NS_IMETHOD Clone(nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to Clone(_retval); } \
  NS_IMETHOD_(nsIODBCStorageStatement *) DupForAsync(void) { return _to DupForAsync(); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return _to GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return _to GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return _to GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetParameterCount(PRUint16 *aParameterCount) { return _to GetParameterCount(aParameterCount); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnCount(PRUint16 *aColumnCount) { return _to GetColumnCount(aColumnCount); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnName(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) { return _to GetColumnName(aColumnIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnIndex(const nsAString & aName, PRUint16 *_retval NS_OUTPARAM) { return _to GetColumnIndex(aName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnDecltype(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) { return _to GetColumnDecltype(aColumnIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Reset(void) { return _to Reset(); } \
  NS_IMETHOD SetParams(nsCOMArray<nsODBCParam> & params) { return _to SetParams(params); } \
  NS_SCRIPTABLE NS_IMETHOD BindParameter(PRUint16 aParamIndex, nsIVariant *aValue) { return _to BindParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue) { return _to BindStringParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue) { return _to BindLongStringParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindFloatParameter(PRUint16 aParamIndex, float aValue) { return _to BindFloatParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindDoubleParameter(PRUint16 aParamIndex, double aValue) { return _to BindDoubleParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue) { return _to BindInt16Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue) { return _to BindInt32Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue) { return _to BindInt64Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindNullParameter(PRUint16 aParamIndex) { return _to BindNullParameter(aParamIndex); } \
  NS_SCRIPTABLE NS_IMETHOD BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) { return _to BindBlobParameter(aParamIndex, aValue, aValueSize); } \
  NS_SCRIPTABLE NS_IMETHOD BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) { return _to BindBinaryParameter(aParamIndex, aValue, aValueSize); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSql(void) { return _to ExecuteSql(); } \
  NS_SCRIPTABLE NS_IMETHOD MoreResults(PRBool *_retval NS_OUTPARAM) { return _to MoreResults(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Fetch(PRBool *_retval NS_OUTPARAM) { return _to Fetch(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetRowCount(PRInt32 *aRowCount) { return _to GetRowCount(aRowCount); } \
  NS_SCRIPTABLE NS_IMETHOD Execute(void) { return _to Execute(); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteStep(PRBool *_retval NS_OUTPARAM) { return _to ExecuteStep(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) { return _to ExecuteAsync(aCallback, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetState(PRInt32 *aState) { return _to GetState(aState); } \
  NS_IMETHOD GetConnectionHandle(SQLHANDLE *aConnectionHandle) { return _to GetConnectionHandle(aConnectionHandle); } \
  NS_SCRIPTABLE NS_IMETHOD EscapeStringForLIKE(const nsAString & aValue, PRUnichar aEscapeChar, nsAString & _retval NS_OUTPARAM) { return _to EscapeStringForLIKE(aValue, aEscapeChar, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGESTATEMENT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Finalize(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Finalize(); } \
  NS_SCRIPTABLE NS_IMETHOD Clone(nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Clone(_retval); } \
  NS_IMETHOD_(nsIODBCStorageStatement *) DupForAsync(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->DupForAsync(); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetParameterCount(PRUint16 *aParameterCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetParameterCount(aParameterCount); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnCount(PRUint16 *aColumnCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumnCount(aColumnCount); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnName(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumnName(aColumnIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnIndex(const nsAString & aName, PRUint16 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumnIndex(aName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnDecltype(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumnDecltype(aColumnIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Reset(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Reset(); } \
  NS_IMETHOD SetParams(nsCOMArray<nsODBCParam> & params) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetParams(params); } \
  NS_SCRIPTABLE NS_IMETHOD BindParameter(PRUint16 aParamIndex, nsIVariant *aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindStringParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindLongStringParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindFloatParameter(PRUint16 aParamIndex, float aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindFloatParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindDoubleParameter(PRUint16 aParamIndex, double aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindDoubleParameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindInt16Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindInt32Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindInt64Parameter(aParamIndex, aValue); } \
  NS_SCRIPTABLE NS_IMETHOD BindNullParameter(PRUint16 aParamIndex) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindNullParameter(aParamIndex); } \
  NS_SCRIPTABLE NS_IMETHOD BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindBlobParameter(aParamIndex, aValue, aValueSize); } \
  NS_SCRIPTABLE NS_IMETHOD BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize) { return !_to ? NS_ERROR_NULL_POINTER : _to->BindBinaryParameter(aParamIndex, aValue, aValueSize); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSql(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteSql(); } \
  NS_SCRIPTABLE NS_IMETHOD MoreResults(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->MoreResults(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Fetch(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Fetch(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetRowCount(PRInt32 *aRowCount) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetRowCount(aRowCount); } \
  NS_SCRIPTABLE NS_IMETHOD Execute(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Execute(); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteStep(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteStep(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteAsync(aCallback, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetState(PRInt32 *aState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetState(aState); } \
  NS_IMETHOD GetConnectionHandle(SQLHANDLE *aConnectionHandle) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetConnectionHandle(aConnectionHandle); } \
  NS_SCRIPTABLE NS_IMETHOD EscapeStringForLIKE(const nsAString & aValue, PRUnichar aEscapeChar, nsAString & _retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->EscapeStringForLIKE(aValue, aEscapeChar, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageStatement : public nsIODBCStorageStatement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGESTATEMENT

  nsODBCStorageStatement();

private:
  ~nsODBCStorageStatement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageStatement, nsIODBCStorageStatement)

nsODBCStorageStatement::nsODBCStorageStatement()
{
  /* member initializers and constructor code */
}

nsODBCStorageStatement::~nsODBCStorageStatement()
{
  /* destructor code */
}

/* void finalize (); */
NS_IMETHODIMP nsODBCStorageStatement::Finalize()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement clone (); */
NS_IMETHODIMP nsODBCStorageStatement::Clone(nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript, notxpcom] nsIODBCStorageStatement dupForAsync (); */
NS_IMETHODIMP_(nsIODBCStorageStatement *) nsODBCStorageStatement::DupForAsync()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long lastErrorCode; */
NS_IMETHODIMP nsODBCStorageStatement::GetLastErrorCode(PRInt32 *aLastErrorCode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorString; */
NS_IMETHODIMP nsODBCStorageStatement::GetLastErrorString(nsAString & aLastErrorString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorState; */
NS_IMETHODIMP nsODBCStorageStatement::GetLastErrorState(nsAString & aLastErrorState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned short parameterCount; */
NS_IMETHODIMP nsODBCStorageStatement::GetParameterCount(PRUint16 *aParameterCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute unsigned short columnCount; */
NS_IMETHODIMP nsODBCStorageStatement::GetColumnCount(PRUint16 *aColumnCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getColumnName (in unsigned short aColumnIndex); */
NS_IMETHODIMP nsODBCStorageStatement::GetColumnName(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* unsigned short getColumnIndex (in AString aName); */
NS_IMETHODIMP nsODBCStorageStatement::GetColumnIndex(const nsAString & aName, PRUint16 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getColumnDecltype (in unsigned short aColumnIndex); */
NS_IMETHODIMP nsODBCStorageStatement::GetColumnDecltype(PRUint16 aColumnIndex, nsAString & _retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void reset (); */
NS_IMETHODIMP nsODBCStorageStatement::Reset()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void setParams (in ODBCParam params); */
NS_IMETHODIMP nsODBCStorageStatement::SetParams(nsCOMArray<nsODBCParam> & params)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindParameter (in unsigned short aParamIndex, in nsIVariant aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindParameter(PRUint16 aParamIndex, nsIVariant *aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindStringParameter (in unsigned short aParamIndex, in AString aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindStringParameter(PRUint16 aParamIndex, const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindLongStringParameter (in unsigned short aParamIndex, in AString aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindLongStringParameter(PRUint16 aParamIndex, const nsAString & aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindFloatParameter (in unsigned short aParamIndex, in float aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindFloatParameter(PRUint16 aParamIndex, float aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindDoubleParameter (in unsigned short aParamIndex, in double aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindDoubleParameter(PRUint16 aParamIndex, double aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindInt16Parameter (in unsigned short aParamIndex, in short aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindInt16Parameter(PRUint16 aParamIndex, PRInt16 aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindInt32Parameter (in unsigned short aParamIndex, in long aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindInt32Parameter(PRUint16 aParamIndex, PRInt32 aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindInt64Parameter (in unsigned short aParamIndex, in long long aValue); */
NS_IMETHODIMP nsODBCStorageStatement::BindInt64Parameter(PRUint16 aParamIndex, PRInt64 aValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindNullParameter (in unsigned short aParamIndex); */
NS_IMETHODIMP nsODBCStorageStatement::BindNullParameter(PRUint16 aParamIndex)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindBlobParameter (in unsigned short aParamIndex, [array, size_is (aValueSize), const] in octet aValue, in unsigned long aValueSize); */
NS_IMETHODIMP nsODBCStorageStatement::BindBlobParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void bindBinaryParameter (in unsigned short aParamIndex, [array, size_is (aValueSize), const] in octet aValue, in unsigned long aValueSize); */
NS_IMETHODIMP nsODBCStorageStatement::BindBinaryParameter(PRUint16 aParamIndex, const PRUint8 *aValue, PRUint32 aValueSize)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void executeSql (); */
NS_IMETHODIMP nsODBCStorageStatement::ExecuteSql()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean moreResults (); */
NS_IMETHODIMP nsODBCStorageStatement::MoreResults(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean fetch (); */
NS_IMETHODIMP nsODBCStorageStatement::Fetch(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long rowCount; */
NS_IMETHODIMP nsODBCStorageStatement::GetRowCount(PRInt32 *aRowCount)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void execute (); */
NS_IMETHODIMP nsODBCStorageStatement::Execute()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean executeStep (); */
NS_IMETHODIMP nsODBCStorageStatement::ExecuteStep(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStoragePendingStatement executeAsync ([optional] in nsIODBCStorageStatementCallback aCallback); */
NS_IMETHODIMP nsODBCStorageStatement::ExecuteAsync(nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long state; */
NS_IMETHODIMP nsODBCStorageStatement::GetState(PRInt32 *aState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript, notxpcom] readonly attribute ConnHandle connectionHandle; */
NS_IMETHODIMP nsODBCStorageStatement::GetConnectionHandle(SQLHANDLE *aConnectionHandle)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString escapeStringForLIKE (in AString aValue, in wchar aEscapeChar); */
NS_IMETHODIMP nsODBCStorageStatement::EscapeStringForLIKE(const nsAString & aValue, PRUnichar aEscapeChar, nsAString & _retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageStatement_h__ */
