/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef _NSODBCPARAM_H_
#define _NSODBCPARAM_H_

#include "nsAutoPtr.h"
#include "nsStringAPI.h"

#include "nsIVariant.h"
#include "nsODBCUtils.h"



class nsODBCParam : public nsISupports
{
public:
    nsODBCParam() { mType = nsIDataType::VTYPE_EMPTY;};
    nsODBCParam(PRInt16 val) { mType = nsIDataType::VTYPE_INT16; mVal.mInt16Value = val;};
    nsODBCParam(PRInt32 val) { mType = nsIDataType::VTYPE_INT32; mVal.mInt32Value = val;};
    nsODBCParam(PRInt64 val) { mType = nsIDataType::VTYPE_INT64; mVal.mInt64Value = val;};
    nsODBCParam(float val)   { mType = nsIDataType::VTYPE_FLOAT; mVal.mFloatValue = val;};
    nsODBCParam(double val)  { mType = nsIDataType::VTYPE_DOUBLE; mVal.mDoubleValue = val;};

    nsODBCParam(const nsAString &val)  { mType = nsIDataType::VTYPE_ASTRING; mVal.mStringValue = new nsWString(val); m_str = val;};

    nsODBCParam(const PRUint8* val, PRUint32 count)  { mType = nsIDataType::VTYPE_ARRAY; mVal.mBinValue = new nsBinary(val, count);};

    // interfaces
    NS_DECL_ISUPPORTS

    union {
        SQLSMALLINT    mInt16Value;
        SQLINTEGER     mInt32Value;
        SQLBIGINT      mInt64Value;
        SQLREAL        mFloatValue;
        SQLDOUBLE      mDoubleValue;
        PRBool         mBoolValue;
        nsWString *    mStringValue;
        nsBinary *     mBinValue;     
    }  mVal;
    PRInt32 mType;
    SQLLEN  mInd;
    PRBool  mIsLong;

private:
    ~nsODBCParam() {
      if (mType == nsIDataType::VTYPE_ASTRING) {
        if (mVal.mStringValue)
          delete mVal.mStringValue;
        mVal.mStringValue = NULL;
      } else if (mType == nsIDataType::VTYPE_ARRAY) {
        if (mVal.mBinValue)
          delete mVal.mBinValue;
        mVal.mBinValue = NULL;
      }
    };
    nsString m_str;
};




#endif /* _NSODBCPARAM_H_ */
