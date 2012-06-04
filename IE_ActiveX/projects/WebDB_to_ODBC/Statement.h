#include <windows.h>
#include <windowsx.h>

#include <string>
#include <sstream>
#include <vector>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include <sql.h>
#include <sqlext.h>


#ifndef H_STATEMENT
#define H_STATEMENT

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

    nsODBCParam(const std::wstring& val)  { mType = nsODBCParam::VTYPE_STRING; mVal.mStringValue = new std::wstring(val); };

    union {
        int            mInt32Value;
        double         mDoubleValue;
        bool           mBoolValue;
        std::wstring  *mStringValue;
    }  mVal;
    int mType;
    int mInd;
    bool  mIsLong;

    ~nsODBCParam() {
      if (mType == nsODBCParam::VTYPE_STRING) {
        if (mVal.mStringValue)
          delete(mVal.mStringValue);
        mVal.mStringValue = NULL;
      } 
    };
};

typedef std::vector<nsODBCParam * > VecODBCParams;


class Statement : public FB::JSAPIAuto
{
public:
    Statement(const FB::BrowserHostPtr& host, SQLHANDLE hdbc, const FB::JSAPIWeakPtr conn);
    virtual ~Statement();

    long GetErrorCode();
    std::wstring GetErrorMessage();
    std::wstring GetErrorState();

    int ClearParams();
    int AddParam(const FB::variant& value);
    int Close();
    int Execute(const std::wstring& query);
    int GetColumnCount();
    int GetRowCount();
    bool Fetch();
    bool MoreResults();
   
    std::wstring GetColumnName(int index);
    FB::variant  GetVariant(int index);
    int GetColumnType(int index);
    bool IsColumnNullable(int index);

    void GetTables(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2, const FB::variant& arg3);
    void GetColumns(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2, const FB::variant& arg3);
    void GetTypeInfo(int dataType);
    void GetPrimaryKeys(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2);
    void GetForeignKeys(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2, const FB::variant& arg3, const FB::variant& arg4, const FB::variant& arg5);
    void GetProcedures(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2);
    void GetProcedureColumns(const FB::variant& arg0, const FB::variant& arg1, const FB::variant& arg2, const FB::variant& arg3);

private:
    FB::BrowserHostPtr m_host;

    SQLHANDLE m_hdbc;
    SQLHANDLE m_hstmt;

    FB::JSAPIPtr m_conn;


    std::wstring m_errMessage;
    std::wstring m_errState;
    long    m_errCode;
    int     m_columnCount;
    int     m_rowCount;

    VecODBCParams  m_params;

    void StoreError(SWORD fHandleType, SQLHANDLE handle);
    int CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
    int CheckStmtRC(SQLRETURN rc);
    int CheckDbcRC(SQLRETURN rc);


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

#endif // H_STATEMENT
