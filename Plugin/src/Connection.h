/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- 
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "npapi.h"
#include "npruntime.h"


class ConnectionObject : public ScriptablePluginObjectBase
{
public:
  ConnectionObject(NPP npp);
  virtual ~ConnectionObject();

  virtual bool HasMethod(NPIdentifier name);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);

private:
   NPIdentifier mc_run_id;
   NPIdentifier mc_Open_id;
   NPIdentifier mc_Close_id;
   NPIdentifier mc_ExecAsync_id;
   NPIdentifier mc_BeginTransaction_id;
   NPIdentifier mc_CommitTransaction_id;
   NPIdentifier mc_RollbackTransaction_id;
   NPIdentifier mc_CreateStatement_id;
   NPIdentifier mc_Init_id;

   NPIdentifier mc_errorCode_id;
   NPIdentifier mc_errorMessage_id;
   NPIdentifier mc_errorState_id;
   NPIdentifier mc_schemaVersion_id;
   NPIdentifier mc_readOnlyMode_id;
   NPIdentifier mc_lastInsertRowID_id;

   SQLHANDLE m_henv;
   SQLHANDLE m_hdbc;

   wchar_t m_errMessage[1024];
   wchar_t m_errState[SQL_SQLSTATE_SIZE+1];
   long    m_errCode;

   SQLINTEGER  mIdentCase;
   SQLINTEGER  mAccessMode;

   SpinLock mTransactionMutex;
   bool mTransactionInProgress;


   void StoreError(SWORD fHandleType, SQLHANDLE handle);

   NPError CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
   NPError CheckDbcRC(SQLRETURN rc);
   NPError CheckStmtRC(SQLHANDLE hstmt, SQLRETURN rc);
   NPError CheckEnvRC(SQLRETURN rc);
   
   NPError ExecAsync(NPObject *pObj);
   NPError Open(NPString *connString);
   NPError Close();
   NPError GetLastInsertRowID(long *aLastInsertRowID);
   long GetErrorCode();
   const char *GetErrorString();
   const char *GetErrorState();
   NPError GetSchemaVersion(SQLINTEGER *version);
   NPError SetSchemaVersion(SQLINTEGER aVersion);
   NPError GetReadOnlyMode(bool *aReadOnlyMode);
   NPError SetReadOnlyMode(bool aReadOnlyMode);
   NPError ExecuteSimpleSQL(const char *aSQLStatement, bool ignoreErr);
   NPError TableExists(const char *aTableName, bool *_retval);
   NPError GetTransactionInProgress(bool *_retval);
   NPError BeginTransaction();
   NPError CommitTransaction();
   NPError RollbackTransaction();
};


#endif // __CONNECTION_H__
