/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageValueArray.idl
 */

#ifndef __gen_nsIODBCStorageValueArray_h__
#define __gen_nsIODBCStorageValueArray_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIVariant_h__
#include "nsIVariant.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIODBCStorageValueArray */
#define NS_IODBCSTORAGEVALUEARRAY_IID_STR "b60cc3a8-c04b-422b-b703-fce3a0583b57"

#define NS_IODBCSTORAGEVALUEARRAY_IID \
  {0xb60cc3a8, 0xc04b, 0x422b, \
    { 0xb7, 0x03, 0xfc, 0xe3, 0xa0, 0x58, 0x3b, 0x57 }}

/**
 * nsIODBCStorageValueArray wraps an array of SQL values,
 * such as a single database row.
 */
class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageValueArray : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGEVALUEARRAY_IID)

  /**
   * These type values are returned by getTypeOfIndex
   * to indicate what type of value is present at
   * a given column.
   */
  enum { VALUE_TYPE_UNKNOWN = 0 };

  enum { VALUE_TYPE_CHAR = 1 };

  enum { VALUE_TYPE_NUMERIC = 2 };

  enum { VALUE_TYPE_DECIMAL = 3 };

  enum { VALUE_TYPE_INTEGER = 4 };

  enum { VALUE_TYPE_SMALLINT = 5 };

  enum { VALUE_TYPE_FLOAT = 6 };

  enum { VALUE_TYPE_REAL = 7 };

  enum { VALUE_TYPE_DOUBLE = 8 };

  enum { VALUE_TYPE_DATE = 9 };

  enum { VALUE_TYPE_TIME = 10 };

  enum { VALUE_TYPE_TIMESTAMP = 11 };

  enum { VALUE_TYPE_VARCHAR = 12 };

  enum { VALUE_TYPE_LONGVARCHAR = -1 };

  enum { VALUE_TYPE_BINARY = -2 };

  enum { VALUE_TYPE_VARBINARY = -3 };

  enum { VALUE_TYPE_LONGVARBINARY = -4 };

  enum { VALUE_TYPE_BIGINT = -5 };

  enum { VALUE_TYPE_TINYINT = -6 };

  enum { VALUE_TYPE_BIT = -7 };

  enum { VALUE_TYPE_WCHAR = -8 };

  enum { VALUE_TYPE_WVARCHAR = -9 };

  enum { VALUE_TYPE_WLONGVARCHAR = -10 };

  enum { VALUE_TYPE_GUID = -11 };

  /**
   * numEntries
   *
   * number of entries in the array (each corresponding to a column
   * in the database row)
   */
  /* readonly attribute unsigned short numEntries; */
  NS_SCRIPTABLE NS_IMETHOD GetNumEntries(PRUint16 *aNumEntries) = 0;

  /**
   * Returns the type of the value at the given column index;
   * one of VALUE_TYPE_NULL, VALUE_TYPE_INTEGER, VALUE_TYPE_FLOAT,
   * VALUE_TYPE_TEXT, VALUE_TYPE_BLOB.
   */
  /* long getTypeOfIndex (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* boolean isNullable (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) = 0;

  /**
   * Obtain a value for the given entry (column) index.
   * Due to SQLite's type conversion rules, any of these are valid
   * for any column regardless of the column's data type.  However,
   * if the specific type matters, getTypeOfIndex should be used
   * first to identify the column type, and then the appropriate
   * get method should be called.
   *
   * If you ask for a string value for a NULL column, you will get an empty
   * string with IsVoid set to distinguish it from an explicitly set empty
   * string.
   */
  /* boolean getBool (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetBool(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) = 0;

  /* short getInt16 (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetInt16(PRUint16 aIndex, PRInt16 *_retval NS_OUTPARAM) = 0;

  /* long getInt32 (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetInt32(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) = 0;

  /* long long getInt64 (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetInt64(PRUint16 aIndex, PRInt64 *_retval NS_OUTPARAM) = 0;

  /* float getFloat (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetFloat(PRUint16 aIndex, float *_retval NS_OUTPARAM) = 0;

  /* double getDouble (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetDouble(PRUint16 aIndex, double *_retval NS_OUTPARAM) = 0;

  /* AString getString (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetString(PRUint16 aIndex, nsAString & _retval NS_OUTPARAM) = 0;

  /* AUTF8String getUTF8String (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetUTF8String(PRUint16 aIndex, nsACString & _retval NS_OUTPARAM) = 0;

  /* nsIVariant getVariant (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetVariant(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* nsIVariant data (in AString aName); */
  NS_SCRIPTABLE NS_IMETHOD Data(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) = 0;

  /* void getBlob (in unsigned short aIndex, out unsigned long aDataSize, [array, size_is (aDataSize)] out octet aData); */
  NS_SCRIPTABLE NS_IMETHOD GetBlob(PRUint16 aIndex, PRUint32 *aDataSize NS_OUTPARAM, PRUint8 **aData NS_OUTPARAM) = 0;

  /* boolean getIsNull (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetIsNull(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageValueArray, NS_IODBCSTORAGEVALUEARRAY_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGEVALUEARRAY \
  NS_SCRIPTABLE NS_IMETHOD GetNumEntries(PRUint16 *aNumEntries); \
  NS_SCRIPTABLE NS_IMETHOD GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetBool(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetInt16(PRUint16 aIndex, PRInt16 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetInt32(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetInt64(PRUint16 aIndex, PRInt64 *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetFloat(PRUint16 aIndex, float *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetDouble(PRUint16 aIndex, double *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetString(PRUint16 aIndex, nsAString & _retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetUTF8String(PRUint16 aIndex, nsACString & _retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetVariant(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Data(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetBlob(PRUint16 aIndex, PRUint32 *aDataSize NS_OUTPARAM, PRUint8 **aData NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetIsNull(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGEVALUEARRAY(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetNumEntries(PRUint16 *aNumEntries) { return _to GetNumEntries(aNumEntries); } \
  NS_SCRIPTABLE NS_IMETHOD GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) { return _to GetTypeOfIndex(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return _to IsNullable(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetBool(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return _to GetBool(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt16(PRUint16 aIndex, PRInt16 *_retval NS_OUTPARAM) { return _to GetInt16(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt32(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) { return _to GetInt32(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt64(PRUint16 aIndex, PRInt64 *_retval NS_OUTPARAM) { return _to GetInt64(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetFloat(PRUint16 aIndex, float *_retval NS_OUTPARAM) { return _to GetFloat(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetDouble(PRUint16 aIndex, double *_retval NS_OUTPARAM) { return _to GetDouble(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetString(PRUint16 aIndex, nsAString & _retval NS_OUTPARAM) { return _to GetString(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetUTF8String(PRUint16 aIndex, nsACString & _retval NS_OUTPARAM) { return _to GetUTF8String(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetVariant(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) { return _to GetVariant(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Data(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) { return _to Data(aName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetBlob(PRUint16 aIndex, PRUint32 *aDataSize NS_OUTPARAM, PRUint8 **aData NS_OUTPARAM) { return _to GetBlob(aIndex, aDataSize, aData); } \
  NS_SCRIPTABLE NS_IMETHOD GetIsNull(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return _to GetIsNull(aIndex, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGEVALUEARRAY(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetNumEntries(PRUint16 *aNumEntries) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNumEntries(aNumEntries); } \
  NS_SCRIPTABLE NS_IMETHOD GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTypeOfIndex(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->IsNullable(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetBool(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBool(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt16(PRUint16 aIndex, PRInt16 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetInt16(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt32(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetInt32(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetInt64(PRUint16 aIndex, PRInt64 *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetInt64(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetFloat(PRUint16 aIndex, float *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFloat(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetDouble(PRUint16 aIndex, double *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDouble(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetString(PRUint16 aIndex, nsAString & _retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetString(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetUTF8String(PRUint16 aIndex, nsACString & _retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetUTF8String(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetVariant(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVariant(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD Data(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Data(aName, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetBlob(PRUint16 aIndex, PRUint32 *aDataSize NS_OUTPARAM, PRUint8 **aData NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetBlob(aIndex, aDataSize, aData); } \
  NS_SCRIPTABLE NS_IMETHOD GetIsNull(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsNull(aIndex, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageValueArray : public nsIODBCStorageValueArray
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEVALUEARRAY

  nsODBCStorageValueArray();

private:
  ~nsODBCStorageValueArray();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageValueArray, nsIODBCStorageValueArray)

nsODBCStorageValueArray::nsODBCStorageValueArray()
{
  /* member initializers and constructor code */
}

nsODBCStorageValueArray::~nsODBCStorageValueArray()
{
  /* destructor code */
}

/* readonly attribute unsigned short numEntries; */
NS_IMETHODIMP nsODBCStorageValueArray::GetNumEntries(PRUint16 *aNumEntries)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long getTypeOfIndex (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isNullable (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean getBool (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetBool(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* short getInt16 (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetInt16(PRUint16 aIndex, PRInt16 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long getInt32 (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetInt32(PRUint16 aIndex, PRInt32 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* long long getInt64 (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetInt64(PRUint16 aIndex, PRInt64 *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* float getFloat (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetFloat(PRUint16 aIndex, float *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double getDouble (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetDouble(PRUint16 aIndex, double *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AString getString (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetString(PRUint16 aIndex, nsAString & _retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* AUTF8String getUTF8String (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetUTF8String(PRUint16 aIndex, nsACString & _retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant getVariant (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetVariant(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant data (in AString aName); */
NS_IMETHODIMP nsODBCStorageValueArray::Data(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getBlob (in unsigned short aIndex, out unsigned long aDataSize, [array, size_is (aDataSize)] out octet aData); */
NS_IMETHODIMP nsODBCStorageValueArray::GetBlob(PRUint16 aIndex, PRUint32 *aDataSize NS_OUTPARAM, PRUint8 **aData NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean getIsNull (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageValueArray::GetIsNull(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageValueArray_h__ */
