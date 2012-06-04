/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageConnection.idl
 */

#ifndef __gen_nsIODBCStorageConnection_h__
#define __gen_nsIODBCStorageConnection_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIODBCStorageStatementCallback; /* forward declaration */

class nsIODBCStoragePendingStatement; /* forward declaration */

class nsIODBCStorageStatement; /* forward declaration */

class nsIVariant; /* forward declaration */


/* starting interface:    nsIODBCStorageConnection */
#define NS_IODBCSTORAGECONNECTION_IID_STR "21cf14cf-fd87-47a7-be9b-7dba19c51106"

#define NS_IODBCSTORAGECONNECTION_IID \
  {0x21cf14cf, 0xfd87, 0x47a7, \
    { 0xbe, 0x9b, 0x7d, 0xba, 0x19, 0xc5, 0x11, 0x06 }}

/**
 * nsIODBCStorageConnection represents a database connection attached to
 * a specific file or to the in-memory data storage.  It is the
 * primary interface for interacting with a database, including
 * creating prepared statements, executing SQL, and examining database
 * errors.
 *
 * @threadsafe
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageConnection : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGECONNECTION_IID)

  /**
   * Closes a database connection.  C++ callers should simply set the database
   * variable to NULL.
   */
  /* void close (); */
  NS_SCRIPTABLE NS_IMETHOD Close(void) = 0;

  /**
   * Indicates if the connection is open and ready to use.  This will be false
   * if the connection failed to open, or it has been closed.
   */
  /* readonly attribute boolean connectionReady; */
  NS_SCRIPTABLE NS_IMETHOD GetConnectionReady(PRBool *aConnectionReady) = 0;

  /**
   * The current database nsIFile.  Null if the database
   * connection refers to an in-memory database.
   */
  /* readonly attribute AString DSN; */
  NS_SCRIPTABLE NS_IMETHOD GetDSN(nsAString & aDSN) = 0;

  /* readonly attribute AString PWD; */
  NS_SCRIPTABLE NS_IMETHOD GetPWD(nsAString & aPWD) = 0;

  /* readonly attribute AString UID; */
  NS_SCRIPTABLE NS_IMETHOD GetUID(nsAString & aUID) = 0;

  /* readonly attribute AString connString; */
  NS_SCRIPTABLE NS_IMETHOD GetConnString(nsAString & aConnString) = 0;

  /**
   * lastInsertRowID returns the row ID from the last INSERT
   * operation.
   */
  /* readonly attribute long long lastInsertRowID; */
  NS_SCRIPTABLE NS_IMETHOD GetLastInsertRowID(PRInt64 *aLastInsertRowID) = 0;

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

  /**
   * The schema version of the database.  This should not be used until the 
   * database is ready.  The schema will be reported as zero if it is not set.
   */
  /* attribute long schemaVersion; */
  NS_SCRIPTABLE NS_IMETHOD GetSchemaVersion(PRInt32 *aSchemaVersion) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetSchemaVersion(PRInt32 aSchemaVersion) = 0;

  /* attribute boolean readOnlyMode; */
  NS_SCRIPTABLE NS_IMETHOD GetReadOnlyMode(PRBool *aReadOnlyMode) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetReadOnlyMode(PRBool aReadOnlyMode) = 0;

  /**
   * Create a nsIStorageStatement for the given SQL expression.  The
   * expression may use ? to indicate sequential numbered arguments,
   * ?1, ?2 etc. to indicate specific numbered arguments or :name and 
   * $var to indicate named arguments.
   *
   * @param aSQLStatement  The SQL statement to execute
   *
   * @returns a new nsIStorageStatement
   */
  /* nsIODBCStorageStatement createStatement (in AString aSQLStatement); */
  NS_SCRIPTABLE NS_IMETHOD CreateStatement(const nsAString & aSQLStatement, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /**
   * Execute a SQL expression, expecting no arguments.
   *
   * @param aSQLStatement  The SQL statement to execute
   */
  /* void executeSimpleSQL (in AString aSQLStatement); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteSimpleSQL(const nsAString & aSQLStatement) = 0;

  /* long executeSQL (in AString aSQLStatement); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval NS_OUTPARAM) = 0;

  /**
   * Execute an array of queries created with this connection asynchronously
   * using any currently bound parameters.  The statements are ran wrapped in a
   * transaction.  These statements can be reused immediately, and reset does
   * not need to be called.
   *
   * Note:  If you have any custom defined functions, they must be re-entrant
   *        since they can be called on multiple threads.
   *
   * @param aStatements
   *        The array of statements to execute asynchronously, in the order they
   *        are given in the array.
   * @param aNumStatements
   *        The number of statements in aStatements.
   * @param aCallback [optional]
   *        The callback object that will be notified of progress, errors, and
   *        completion.
   * @returns an object that can be used to cancel the statements execution.
   */
  /* nsIODBCStoragePendingStatement executeAsync ([array, size_is (aNumStatements)] in nsIODBCStorageStatement aStatements, in unsigned long aNumStatements, [optional] in nsIODBCStorageStatementCallback aCallback); */
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatement **aStatements, PRUint32 aNumStatements, nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) = 0;

  /**
   * Check if the given table exists.
   *
   * @param aTableName   The table to check
   * @returns TRUE if table exists, FALSE otherwise.
   */
  /* boolean tableExists (in AString aTableName); */
  NS_SCRIPTABLE NS_IMETHOD TableExists(const nsAString & aTableName, PRBool *_retval NS_OUTPARAM) = 0;

  /**
   * Check if the given index exists.
   *
   * @param aIndexName   The index to check
   * @returns TRUE if the index exists, FALSE otherwise.
   */
/**
   * Returns true if a transaction is active on this connection.
   */
  /* readonly attribute boolean transactionInProgress; */
  NS_SCRIPTABLE NS_IMETHOD GetTransactionInProgress(PRBool *aTransactionInProgress) = 0;

  /**
   * Begin a new transaction.  sqlite default transactions are deferred.
   * If a transaction is active, throws an error.
   */
  /* void beginTransaction (); */
  NS_SCRIPTABLE NS_IMETHOD BeginTransaction(void) = 0;

  /**
   * Begins a new transaction with the given type.
   */
  enum { TRANSACTION_READ_UNCOMMITTED = 1 };

  enum { TRANSACTION_READ_COMMITTED = 2 };

  enum { TRANSACTION_REPEATABLE_READ = 3 };

  enum { TRANSACTION_SERIALIZABLE = 4 };

  /* void beginTransactionAs (in PRInt32 transactionType); */
  NS_SCRIPTABLE NS_IMETHOD BeginTransactionAs(PRInt32 transactionType) = 0;

  /**
   * Commits the current transaction.  If no transaction is active,
   * @throws NS_ERROR_STORAGE_NO_TRANSACTION.
   */
  /* void commitTransaction (); */
  NS_SCRIPTABLE NS_IMETHOD CommitTransaction(void) = 0;

  /**
   * Rolls back the current transaction.  If no transaction is active,
   * @throws NS_ERROR_STORAGE_NO_TRANSACTION.
   */
  /* void rollbackTransaction (); */
  NS_SCRIPTABLE NS_IMETHOD RollbackTransaction(void) = 0;

  /**
   * Create the table with the given name and schema.
   *
   * If the table already exists, NS_ERROR_FAILURE is thrown.
   * (XXX at some point in the future it will check if the schema is
   * the same as what is specified, but that doesn't happen currently.)
   *
   * @param aTableName the table name to be created, consisting of
   * [A-Za-z0-9_], and beginning with a letter.
   *
   * @param aTableSchema the schema of the table; what would normally
   * go between the parens in a CREATE TABLE statement: e.g., "foo
   * INTEGER, bar STRING".
   *
   * @throws NS_ERROR_FAILURE if the table already exists or could not
   * be created for any other reason.
   *
   */
  /* void createTable (in string aTableName, in string aTableSchema); */
  NS_SCRIPTABLE NS_IMETHOD CreateTable(const char *aTableName, const char *aTableSchema) = 0;

  /* nsIODBCStorageStatement getTables (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aTableType); */
  NS_SCRIPTABLE NS_IMETHOD GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
  NS_SCRIPTABLE NS_IMETHOD GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getTypeInfo (in short aDataType); */
  NS_SCRIPTABLE NS_IMETHOD GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getPrimaryKeys (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
  NS_SCRIPTABLE NS_IMETHOD GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getForeignKeys (in nsIVariant aPkCatalogName, in nsIVariant aPkSchemaName, in nsIVariant aPkTableName, in nsIVariant aFkCatalogName, in nsIVariant aFkSchemaName, in nsIVariant aFkTableName); */
  NS_SCRIPTABLE NS_IMETHOD GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getSpecialColumns (in unsigned short aIdentifierType, in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aScope, in unsigned short aNullable); */
  NS_SCRIPTABLE NS_IMETHOD GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getStatistics (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aUnique); */
  NS_SCRIPTABLE NS_IMETHOD GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getProcedures (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName); */
  NS_SCRIPTABLE NS_IMETHOD GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getProcedureColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName, in nsIVariant aColName); */
  NS_SCRIPTABLE NS_IMETHOD GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getTablePrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
  NS_SCRIPTABLE NS_IMETHOD GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

  /* nsIODBCStorageStatement getColumnsPrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
  NS_SCRIPTABLE NS_IMETHOD GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageConnection, NS_IODBCSTORAGECONNECTION_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGECONNECTION \
  NS_SCRIPTABLE NS_IMETHOD Close(void); \
  NS_SCRIPTABLE NS_IMETHOD GetConnectionReady(PRBool *aConnectionReady); \
  NS_SCRIPTABLE NS_IMETHOD GetDSN(nsAString & aDSN); \
  NS_SCRIPTABLE NS_IMETHOD GetPWD(nsAString & aPWD); \
  NS_SCRIPTABLE NS_IMETHOD GetUID(nsAString & aUID); \
  NS_SCRIPTABLE NS_IMETHOD GetConnString(nsAString & aConnString); \
  NS_SCRIPTABLE NS_IMETHOD GetLastInsertRowID(PRInt64 *aLastInsertRowID); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString); \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState); \
  NS_SCRIPTABLE NS_IMETHOD GetSchemaVersion(PRInt32 *aSchemaVersion); \
  NS_SCRIPTABLE NS_IMETHOD SetSchemaVersion(PRInt32 aSchemaVersion); \
  NS_SCRIPTABLE NS_IMETHOD GetReadOnlyMode(PRBool *aReadOnlyMode); \
  NS_SCRIPTABLE NS_IMETHOD SetReadOnlyMode(PRBool aReadOnlyMode); \
  NS_SCRIPTABLE NS_IMETHOD CreateStatement(const nsAString & aSQLStatement, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSimpleSQL(const nsAString & aSQLStatement); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatement **aStatements, PRUint32 aNumStatements, nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD TableExists(const nsAString & aTableName, PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetTransactionInProgress(PRBool *aTransactionInProgress); \
  NS_SCRIPTABLE NS_IMETHOD BeginTransaction(void); \
  NS_SCRIPTABLE NS_IMETHOD BeginTransactionAs(PRInt32 transactionType); \
  NS_SCRIPTABLE NS_IMETHOD CommitTransaction(void); \
  NS_SCRIPTABLE NS_IMETHOD RollbackTransaction(void); \
  NS_SCRIPTABLE NS_IMETHOD CreateTable(const char *aTableName, const char *aTableSchema); \
  NS_SCRIPTABLE NS_IMETHOD GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGECONNECTION(_to) \
  NS_SCRIPTABLE NS_IMETHOD Close(void) { return _to Close(); } \
  NS_SCRIPTABLE NS_IMETHOD GetConnectionReady(PRBool *aConnectionReady) { return _to GetConnectionReady(aConnectionReady); } \
  NS_SCRIPTABLE NS_IMETHOD GetDSN(nsAString & aDSN) { return _to GetDSN(aDSN); } \
  NS_SCRIPTABLE NS_IMETHOD GetPWD(nsAString & aPWD) { return _to GetPWD(aPWD); } \
  NS_SCRIPTABLE NS_IMETHOD GetUID(nsAString & aUID) { return _to GetUID(aUID); } \
  NS_SCRIPTABLE NS_IMETHOD GetConnString(nsAString & aConnString) { return _to GetConnString(aConnString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastInsertRowID(PRInt64 *aLastInsertRowID) { return _to GetLastInsertRowID(aLastInsertRowID); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return _to GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return _to GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return _to GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetSchemaVersion(PRInt32 *aSchemaVersion) { return _to GetSchemaVersion(aSchemaVersion); } \
  NS_SCRIPTABLE NS_IMETHOD SetSchemaVersion(PRInt32 aSchemaVersion) { return _to SetSchemaVersion(aSchemaVersion); } \
  NS_SCRIPTABLE NS_IMETHOD GetReadOnlyMode(PRBool *aReadOnlyMode) { return _to GetReadOnlyMode(aReadOnlyMode); } \
  NS_SCRIPTABLE NS_IMETHOD SetReadOnlyMode(PRBool aReadOnlyMode) { return _to SetReadOnlyMode(aReadOnlyMode); } \
  NS_SCRIPTABLE NS_IMETHOD CreateStatement(const nsAString & aSQLStatement, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to CreateStatement(aSQLStatement, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSimpleSQL(const nsAString & aSQLStatement) { return _to ExecuteSimpleSQL(aSQLStatement); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval NS_OUTPARAM) { return _to ExecuteSQL(aSQLStatement, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatement **aStatements, PRUint32 aNumStatements, nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) { return _to ExecuteAsync(aStatements, aNumStatements, aCallback, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD TableExists(const nsAString & aTableName, PRBool *_retval NS_OUTPARAM) { return _to TableExists(aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTransactionInProgress(PRBool *aTransactionInProgress) { return _to GetTransactionInProgress(aTransactionInProgress); } \
  NS_SCRIPTABLE NS_IMETHOD BeginTransaction(void) { return _to BeginTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD BeginTransactionAs(PRInt32 transactionType) { return _to BeginTransactionAs(transactionType); } \
  NS_SCRIPTABLE NS_IMETHOD CommitTransaction(void) { return _to CommitTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD RollbackTransaction(void) { return _to RollbackTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD CreateTable(const char *aTableName, const char *aTableSchema) { return _to CreateTable(aTableName, aTableSchema); } \
  NS_SCRIPTABLE NS_IMETHOD GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetTables(aCatalogName, aSchemaName, aTableName, aTableType, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetColumns(aCatalogName, aSchemaName, aTableName, aColumnName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetTypeInfo(aDataType, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetPrimaryKeys(aCatalogName, aSchemaName, aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetForeignKeys(aPkCatalogName, aPkSchemaName, aPkTableName, aFkCatalogName, aFkSchemaName, aFkTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetSpecialColumns(aIdentifierType, aCatalogName, aSchemaName, aTableName, aScope, aNullable, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetStatistics(aCatalogName, aSchemaName, aTableName, aUnique, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetProcedures(aCatalogName, aSchemaName, aProcName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetProcedureColumns(aCatalogName, aSchemaName, aProcName, aColName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetTablePrivileges(aCatalogName, aSchemaName, aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return _to GetColumnsPrivileges(aCatalogName, aSchemaName, aTableName, aColumnName, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGECONNECTION(_to) \
  NS_SCRIPTABLE NS_IMETHOD Close(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Close(); } \
  NS_SCRIPTABLE NS_IMETHOD GetConnectionReady(PRBool *aConnectionReady) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetConnectionReady(aConnectionReady); } \
  NS_SCRIPTABLE NS_IMETHOD GetDSN(nsAString & aDSN) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDSN(aDSN); } \
  NS_SCRIPTABLE NS_IMETHOD GetPWD(nsAString & aPWD) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPWD(aPWD); } \
  NS_SCRIPTABLE NS_IMETHOD GetUID(nsAString & aUID) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUID(aUID); } \
  NS_SCRIPTABLE NS_IMETHOD GetConnString(nsAString & aConnString) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetConnString(aConnString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastInsertRowID(PRInt64 *aLastInsertRowID) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastInsertRowID(aLastInsertRowID); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorCode(PRInt32 *aLastErrorCode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorCode(aLastErrorCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorString(nsAString & aLastErrorString) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorString(aLastErrorString); } \
  NS_SCRIPTABLE NS_IMETHOD GetLastErrorState(nsAString & aLastErrorState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetLastErrorState(aLastErrorState); } \
  NS_SCRIPTABLE NS_IMETHOD GetSchemaVersion(PRInt32 *aSchemaVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSchemaVersion(aSchemaVersion); } \
  NS_SCRIPTABLE NS_IMETHOD SetSchemaVersion(PRInt32 aSchemaVersion) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSchemaVersion(aSchemaVersion); } \
  NS_SCRIPTABLE NS_IMETHOD GetReadOnlyMode(PRBool *aReadOnlyMode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetReadOnlyMode(aReadOnlyMode); } \
  NS_SCRIPTABLE NS_IMETHOD SetReadOnlyMode(PRBool aReadOnlyMode) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetReadOnlyMode(aReadOnlyMode); } \
  NS_SCRIPTABLE NS_IMETHOD CreateStatement(const nsAString & aSQLStatement, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateStatement(aSQLStatement, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSimpleSQL(const nsAString & aSQLStatement) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteSimpleSQL(aSQLStatement); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteSQL(aSQLStatement, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD ExecuteAsync(nsIODBCStorageStatement **aStatements, PRUint32 aNumStatements, nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->ExecuteAsync(aStatements, aNumStatements, aCallback, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD TableExists(const nsAString & aTableName, PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->TableExists(aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTransactionInProgress(PRBool *aTransactionInProgress) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTransactionInProgress(aTransactionInProgress); } \
  NS_SCRIPTABLE NS_IMETHOD BeginTransaction(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->BeginTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD BeginTransactionAs(PRInt32 transactionType) { return !_to ? NS_ERROR_NULL_POINTER : _to->BeginTransactionAs(transactionType); } \
  NS_SCRIPTABLE NS_IMETHOD CommitTransaction(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->CommitTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD RollbackTransaction(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->RollbackTransaction(); } \
  NS_SCRIPTABLE NS_IMETHOD CreateTable(const char *aTableName, const char *aTableSchema) { return !_to ? NS_ERROR_NULL_POINTER : _to->CreateTable(aTableName, aTableSchema); } \
  NS_SCRIPTABLE NS_IMETHOD GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTables(aCatalogName, aSchemaName, aTableName, aTableType, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumns(aCatalogName, aSchemaName, aTableName, aColumnName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTypeInfo(aDataType, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPrimaryKeys(aCatalogName, aSchemaName, aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetForeignKeys(aPkCatalogName, aPkSchemaName, aPkTableName, aFkCatalogName, aFkSchemaName, aFkTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSpecialColumns(aIdentifierType, aCatalogName, aSchemaName, aTableName, aScope, aNullable, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetStatistics(aCatalogName, aSchemaName, aTableName, aUnique, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProcedures(aCatalogName, aSchemaName, aProcName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetProcedureColumns(aCatalogName, aSchemaName, aProcName, aColName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTablePrivileges(aCatalogName, aSchemaName, aTableName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetColumnsPrivileges(aCatalogName, aSchemaName, aTableName, aColumnName, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageConnection : public nsIODBCStorageConnection
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGECONNECTION

  nsODBCStorageConnection();

private:
  ~nsODBCStorageConnection();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageConnection, nsIODBCStorageConnection)

nsODBCStorageConnection::nsODBCStorageConnection()
{
  /* member initializers and constructor code */
}

nsODBCStorageConnection::~nsODBCStorageConnection()
{
  /* destructor code */
}

/* void close (); */
NS_IMETHODIMP nsODBCStorageConnection::Close()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean connectionReady; */
NS_IMETHODIMP nsODBCStorageConnection::GetConnectionReady(PRBool *aConnectionReady)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString DSN; */
NS_IMETHODIMP nsODBCStorageConnection::GetDSN(nsAString & aDSN)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString PWD; */
NS_IMETHODIMP nsODBCStorageConnection::GetPWD(nsAString & aPWD)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString UID; */
NS_IMETHODIMP nsODBCStorageConnection::GetUID(nsAString & aUID)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString connString; */
NS_IMETHODIMP nsODBCStorageConnection::GetConnString(nsAString & aConnString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long long lastInsertRowID; */
NS_IMETHODIMP nsODBCStorageConnection::GetLastInsertRowID(PRInt64 *aLastInsertRowID)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long lastErrorCode; */
NS_IMETHODIMP nsODBCStorageConnection::GetLastErrorCode(PRInt32 *aLastErrorCode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorString; */
NS_IMETHODIMP nsODBCStorageConnection::GetLastErrorString(nsAString & aLastErrorString)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString lastErrorState; */
NS_IMETHODIMP nsODBCStorageConnection::GetLastErrorState(nsAString & aLastErrorState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute long schemaVersion; */
NS_IMETHODIMP nsODBCStorageConnection::GetSchemaVersion(PRInt32 *aSchemaVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsODBCStorageConnection::SetSchemaVersion(PRInt32 aSchemaVersion)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean readOnlyMode; */
NS_IMETHODIMP nsODBCStorageConnection::GetReadOnlyMode(PRBool *aReadOnlyMode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsODBCStorageConnection::SetReadOnlyMode(PRBool aReadOnlyMode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement createStatement (in AString aSQLStatement); */
NS_IMETHODIMP nsODBCStorageConnection::CreateStatement(const nsAString & aSQLStatement, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void executeSimpleSQL (in AString aSQLStatement); */
NS_IMETHODIMP nsODBCStorageConnection::ExecuteSimpleSQL(const nsAString & aSQLStatement)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long executeSQL (in AString aSQLStatement); */
NS_IMETHODIMP nsODBCStorageConnection::ExecuteSQL(const nsAString & aSQLStatement, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStoragePendingStatement executeAsync ([array, size_is (aNumStatements)] in nsIODBCStorageStatement aStatements, in unsigned long aNumStatements, [optional] in nsIODBCStorageStatementCallback aCallback); */
NS_IMETHODIMP nsODBCStorageConnection::ExecuteAsync(nsIODBCStorageStatement **aStatements, PRUint32 aNumStatements, nsIODBCStorageStatementCallback *aCallback, nsIODBCStoragePendingStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean tableExists (in AString aTableName); */
NS_IMETHODIMP nsODBCStorageConnection::TableExists(const nsAString & aTableName, PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute boolean transactionInProgress; */
NS_IMETHODIMP nsODBCStorageConnection::GetTransactionInProgress(PRBool *aTransactionInProgress)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void beginTransaction (); */
NS_IMETHODIMP nsODBCStorageConnection::BeginTransaction()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void beginTransactionAs (in PRInt32 transactionType); */
NS_IMETHODIMP nsODBCStorageConnection::BeginTransactionAs(PRInt32 transactionType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void commitTransaction (); */
NS_IMETHODIMP nsODBCStorageConnection::CommitTransaction()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void rollbackTransaction (); */
NS_IMETHODIMP nsODBCStorageConnection::RollbackTransaction()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void createTable (in string aTableName, in string aTableSchema); */
NS_IMETHODIMP nsODBCStorageConnection::CreateTable(const char *aTableName, const char *aTableSchema)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getTables (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aTableType); */
NS_IMETHODIMP nsODBCStorageConnection::GetTables(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aTableType, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
NS_IMETHODIMP nsODBCStorageConnection::GetColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getTypeInfo (in short aDataType); */
NS_IMETHODIMP nsODBCStorageConnection::GetTypeInfo(PRInt16 aDataType, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getPrimaryKeys (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetPrimaryKeys(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getForeignKeys (in nsIVariant aPkCatalogName, in nsIVariant aPkSchemaName, in nsIVariant aPkTableName, in nsIVariant aFkCatalogName, in nsIVariant aFkSchemaName, in nsIVariant aFkTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetForeignKeys(nsIVariant *aPkCatalogName, nsIVariant *aPkSchemaName, nsIVariant *aPkTableName, nsIVariant *aFkCatalogName, nsIVariant *aFkSchemaName, nsIVariant *aFkTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getSpecialColumns (in unsigned short aIdentifierType, in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aScope, in unsigned short aNullable); */
NS_IMETHODIMP nsODBCStorageConnection::GetSpecialColumns(PRUint16 aIdentifierType, nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aScope, PRUint16 aNullable, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getStatistics (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in unsigned short aUnique); */
NS_IMETHODIMP nsODBCStorageConnection::GetStatistics(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, PRUint16 aUnique, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getProcedures (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName); */
NS_IMETHODIMP nsODBCStorageConnection::GetProcedures(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getProcedureColumns (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aProcName, in nsIVariant aColName); */
NS_IMETHODIMP nsODBCStorageConnection::GetProcedureColumns(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aProcName, nsIVariant *aColName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getTablePrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName); */
NS_IMETHODIMP nsODBCStorageConnection::GetTablePrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIODBCStorageStatement getColumnsPrivileges (in nsIVariant aCatalogName, in nsIVariant aSchemaName, in nsIVariant aTableName, in nsIVariant aColumnName); */
NS_IMETHODIMP nsODBCStorageConnection::GetColumnsPrivileges(nsIVariant *aCatalogName, nsIVariant *aSchemaName, nsIVariant *aTableName, nsIVariant *aColumnName, nsIODBCStorageStatement **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageConnection_h__ */
