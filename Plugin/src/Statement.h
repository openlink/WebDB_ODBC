/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- 
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __STATEMENT_H__
#define __STATEMENT_H__

#include "npapi.h"
#include "npruntime.h"

#include <vector>
using namespace std;

class nsODBCParam 
{
public:
   const static int VTYPE_EMPTY  = 0;
   const static int VTYPE_BOOL   = 1;
   const static int VTYPE_INT32  = 2;
   const static int VTYPE_DOUBLE = 3;
   const static int VTYPE_STRING = 4;
   const static int VTYPE_BINARY = 5;
    
    nsODBCParam() { mType = nsODBCParam::VTYPE_EMPTY;};
    nsODBCParam(bool val) { mType = nsODBCParam::VTYPE_BOOL; mVal.mBoolValue = val;};
    nsODBCParam(int val) { mType = nsODBCParam::VTYPE_INT32; mVal.mInt32Value = val;};
    nsODBCParam(double val)  { mType = nsODBCParam::VTYPE_DOUBLE; mVal.mDoubleValue = val;};

    nsODBCParam(NPString *val)  { mType = nsODBCParam::VTYPE_STRING; mVal.mStringValue = new nsWString(val); };

    union {
        int            mInt32Value;
        double         mDoubleValue;
        bool           mBoolValue;
        nsWString *    mStringValue;
    }  mVal;
    int mType;
    SQLLEN mInd;
    bool  mIsLong;

    ~nsODBCParam() {
      if (mType == nsODBCParam::VTYPE_STRING) {
        if (mVal.mStringValue)
          delete(mVal.mStringValue);
        mVal.mStringValue = NULL;
      } 
    };
};

typedef vector<nsODBCParam * > VecODBCParams;


class StatementObject : public ScriptablePluginObjectBase
{
public:
  StatementObject(NPP npp);
  virtual ~StatementObject();

  virtual bool HasMethod(NPIdentifier name);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);

private:
   NPIdentifier mc_Init_id;
   NPIdentifier mc_AddParameter_id;
   NPIdentifier mc_Execute_id;
   NPIdentifier mc_Close_id;
   NPIdentifier mc_Fetch_id;
   NPIdentifier mc_MoreResults_id;
   NPIdentifier mc_GetColumnName_id;
   NPIdentifier mc_GetVariant_id;
   NPIdentifier mc_GetColumnType_id;
   NPIdentifier mc_IsColumnNullable_id;

   NPIdentifier mc_GetTables_id;
   NPIdentifier mc_GetColumns_id;
   NPIdentifier mc_GetTypeInfo_id;
   NPIdentifier mc_GetPrimaryKeys_id;
   NPIdentifier mc_GetForeignKeys_id;
   NPIdentifier mc_GetProcedures_id;
   NPIdentifier mc_GetProcedureColumns_id;


   NPIdentifier mc_errorCode_id;
   NPIdentifier mc_errorMessage_id;
   NPIdentifier mc_errorState_id;
   NPIdentifier mc_rowCount_id;   
   NPIdentifier mc_columnCount_id;

   SQLHANDLE m_hdbc;
   SQLHANDLE m_hstmt;
   NPObject  *m_conn;

   wchar_t m_errMessage[1024];
   wchar_t m_errState[SQL_SQLSTATE_SIZE+1];
   long    m_errCode;
   SQLINTEGER m_columnCount;
   SQLINTEGER m_rowCount;

   VecODBCParams  m_params;

   void StoreError(SWORD fHandleType, SQLHANDLE handle);
   NPError CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
   NPError CheckStmtRC(SQLRETURN rc);
   NPError CheckDbcRC(SQLRETURN rc);

   long GetErrorCode();
   const char *GetErrorString();
   const char *GetErrorState();


   NPError ClearParams();
   NPError AddParam(const NPVariant *value);
   NPError Init(SQLHANDLE dbc, NPObject *conn);
   NPError Close();
   NPError Execute(NPString *query);
   NPError GetColumnCount(SQLINTEGER *retval);
   NPError GetRowCount(SQLINTEGER *retval);
   NPError Fetch(bool *retval);
   NPError MoreResults(bool *retval);
   
   NPError GetColumnName(int index, const char **retval);
   NPError GetVariant(int index, NPVariant *retval);
   NPError GetColumnType(int index, int *retval);
   NPError IsColumnNullable(int index, bool *retval);

   void GetTables(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2, const NPVariant *arg3);
   void GetColumns(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2, const NPVariant *arg3);
   void GetTypeInfo(int dataType);
   void GetPrimaryKeys(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2);
   void GetForeignKeys(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2, const NPVariant *arg3, const NPVariant *arg4, const NPVariant *arg5);
   void GetProcedures(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2);
   void GetProcedureColumns(const NPVariant *arg0, const NPVariant *arg1, const NPVariant *arg2, const NPVariant *arg3);

public:
  const static int VALUE_TYPE_UNKNOWN = 0;
  const static int VALUE_TYPE_CHAR   = 1;
  const static int VALUE_TYPE_NUMERIC = 2;
  const static int VALUE_TYPE_DECIMAL = 3;
  const static int VALUE_TYPE_INTEGER = 4;
  const static int VALUE_TYPE_SMALLINT = 5;
  const static int VALUE_TYPE_FLOAT = 6;
  const static int VALUE_TYPE_REAL = 7;
  const static int VALUE_TYPE_DOUBLE = 8;
  const static int VALUE_TYPE_DATE = 9;
  const static int VALUE_TYPE_TIME = 10;
  const static int VALUE_TYPE_TIMESTAMP = 11;
  const static int VALUE_TYPE_VARCHAR = 12;
  const static int VALUE_TYPE_LONGVARCHAR = -1;
  const static int VALUE_TYPE_BINARY = -2;
  const static int VALUE_TYPE_VARBINARY = -3;
  const static int VALUE_TYPE_LONGVARBINARY = -4;
  const static int VALUE_TYPE_BIGINT = -5;
  const static int VALUE_TYPE_TINYINT = -6;
  const static int VALUE_TYPE_BIT = -7;
  const static int VALUE_TYPE_WCHAR = -8;
  const static int VALUE_TYPE_WVARCHAR = -9;
  const static int VALUE_TYPE_WLONGVARCHAR = -10;
  const static int VALUE_TYPE_GUID = -11;

};


#endif // __STATEMENT_H__
