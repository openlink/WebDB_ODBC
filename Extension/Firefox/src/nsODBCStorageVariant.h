/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __nsODBCStorageVariant_h__
#define __nsODBCStorageVariant_h__

#include <float.h>

#include "nsIVariant.h"
#include "nsTArray.h"
#include "nsMemory.h"
#include <utility>
#include "prprf.h"
#include "prdtoa.h"

/**
 * This class is used by the storage module whenever an nsIVariant needs to be
 * returned.  We provide traits for the basic sqlite types to make use easier.
 * The following types map to the indicated sqlite type:
 * nsString  -> TEXT (use nsODBCStorageText)
 * PRUint8[] -> BLOB (use nsODBCStorageBlob)
 * nsnull    -> NULL (use nsODBCStorageNull)
 * PRInt16   -> SHORT (use nsODBCStorageInt16)
 * PRInt32   -> INTEGER (use nsODBCStorageInt32)
 * PRInt64   -> BIGINT (use nsODBCStorageInt64)
 * float     -> FLOAT (use nsODBCStorageFloat)
 * double    -> DOUBLE (use nsODBCStorageDouble)

 */

#define NO_CONVERSION return NS_ERROR_CANNOT_CONVERT_DATA;

////////////////////////////////////////////////////////////////////////////////
//// Base Class

class nsODBCStorageVariant_base : public nsIVariant
{
public:
  NS_DECL_ISUPPORTS

  NS_IMETHOD GetDataType(PRUint16 *_type)
  {
    *_type = nsIDataType::VTYPE_EMPTY;
    return NS_OK;
  }

  NS_IMETHOD GetAsInt32(PRInt32 *_integer) { NO_CONVERSION }
  NS_IMETHOD GetAsInt64(PRInt64 *) { NO_CONVERSION }
  NS_IMETHOD GetAsDouble(double *) { NO_CONVERSION }
  NS_IMETHOD GetAsAUTF8String(nsACString &) { NO_CONVERSION }
  NS_IMETHOD GetAsAString(nsAString &) { NO_CONVERSION }
  NS_IMETHOD GetAsArray(PRUint16 *, nsIID *, PRUint32 *, void **) { NO_CONVERSION }
  NS_IMETHOD GetAsInt8(PRUint8 *) { NO_CONVERSION }
  NS_IMETHOD GetAsInt16(PRInt16 *) { NO_CONVERSION }
  NS_IMETHOD GetAsUint8(PRUint8 *) { NO_CONVERSION }
  NS_IMETHOD GetAsUint16(PRUint16 *) { NO_CONVERSION }
  NS_IMETHOD GetAsUint32(PRUint32 *) { NO_CONVERSION }
  NS_IMETHOD GetAsUint64(PRUint64 *) { NO_CONVERSION }
  NS_IMETHOD GetAsFloat(float *) { NO_CONVERSION }
  NS_IMETHOD GetAsBool(PRBool *) { NO_CONVERSION }
  NS_IMETHOD GetAsChar(char *) { NO_CONVERSION }
  NS_IMETHOD GetAsWChar(PRUnichar *) { NO_CONVERSION }
  NS_IMETHOD GetAsID(nsID *) { NO_CONVERSION }
  NS_IMETHOD GetAsDOMString(nsAString &) { NO_CONVERSION }
  NS_IMETHOD GetAsString(char **) { NO_CONVERSION }
  NS_IMETHOD GetAsWString(PRUnichar **) { NO_CONVERSION }
  NS_IMETHOD GetAsISupports(nsISupports **) { NO_CONVERSION }
  NS_IMETHOD GetAsJSVal(jsval *) { NO_CONVERSION }
  NS_IMETHOD GetAsInterface(nsIID **, void **) { NO_CONVERSION }
  NS_IMETHOD GetAsACString(nsACString &) { NO_CONVERSION }
  NS_IMETHOD GetAsStringWithSize(PRUint32 *, char **) { NO_CONVERSION }
  NS_IMETHOD GetAsWStringWithSize(PRUint32 *, PRUnichar **) { NO_CONVERSION }

protected:
  virtual ~nsODBCStorageVariant_base() { }
};

//NS_IMPL_THREADSAFE_ISUPPORTS1(
//  nsODBCStorageVariant_base,
//  nsIVariant
//)

////////////////////////////////////////////////////////////////////////////////
//// Traits

/**
 * Generics
 */

template <typename DataType>
struct variant_traits
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_EMPTY; }
};

template <typename DataType>
struct variant_storage_traits
{
  typedef DataType ConstructorType;
  typedef DataType StorageType;
  static inline StorageType storage_conversion(ConstructorType aData) { return aData; }
};

template <typename DataType>
struct variant_integer_traits
{
  typedef typename variant_storage_traits<DataType>::StorageType StorageType;
  static inline nsresult asBool(StorageType, PRBool *) { NO_CONVERSION }
  static inline nsresult asInt8(StorageType, PRUint8 *) { NO_CONVERSION }
  static inline nsresult asInt16(StorageType, PRInt16 *) { NO_CONVERSION }
  static inline nsresult asInt32(StorageType, PRInt32 *) { NO_CONVERSION }
  static inline nsresult asInt64(StorageType, PRInt64 *) { NO_CONVERSION }
};

template <typename DataType>
struct variant_float_traits
{
  typedef typename variant_storage_traits<DataType>::StorageType StorageType;
  static inline nsresult asFloat(StorageType, float *) { NO_CONVERSION }
  static inline nsresult asDouble(StorageType, double *) { NO_CONVERSION }
};

template <typename DataType>
struct variant_text_traits
{
  typedef typename variant_storage_traits<DataType>::StorageType StorageType;
  static inline nsresult asUTF8String(StorageType, nsACString &) { NO_CONVERSION }
  static inline nsresult asString(StorageType, nsAString &) { NO_CONVERSION }
};

template <typename DataType>
struct variant_blob_traits
{
  typedef typename variant_storage_traits<DataType>::StorageType StorageType;
  static inline nsresult asArray(StorageType, PRUint16 *, PRUint32 *, void **)
  { NO_CONVERSION }
};





/**
 * INT16 types
 */

template < >
struct variant_traits<PRInt16>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_INT16; }
};
template < >
struct variant_integer_traits<PRInt16>
{
  static inline nsresult asBool(PRInt16 aValue, PRBool *_result)
  {
    *_result = (aValue != 0 ? PR_TRUE: PR_FALSE);
    return NS_OK;
  }
  static inline nsresult asInt8(PRInt16 aValue, PRUint8 *_result)
  {
    if (aValue > (PRInt16)PR_UINT8_MAX || aValue < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRUint8)aValue;
    return NS_OK;
  }
  static inline nsresult asInt16(PRInt16 aValue, PRInt16 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asInt32(PRInt16 aValue, PRInt32 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asInt64(PRInt64 aValue, PRInt64 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
};
// xpcvariant just calls get double for integers...
template < >
struct variant_float_traits<PRInt16>
{
  static inline nsresult asFloat(PRInt16 aValue, float *_result)
  {
    *_result = float(aValue);
    return NS_OK;
  }
  static inline nsresult asDouble(PRInt16 aValue, double *_result)
  {
    *_result = double(aValue);
    return NS_OK;
  }
};


/**
 * INT32 types
 */

template < >
struct variant_traits<PRInt32>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_INT32; }
};
template < >
struct variant_integer_traits<PRInt32>
{
  static inline nsresult asBool(PRInt32 aValue, PRBool *_result)
  {
    *_result = (aValue != 0 ? PR_TRUE: PR_FALSE);
    return NS_OK;
  }
  static inline nsresult asInt8(PRInt32 aValue, PRUint8 *_result)
  {
    if (aValue > (PRInt32)PR_UINT8_MAX || aValue < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asInt16(PRInt32 aValue, PRInt16 *_result)
  {
    if (aValue > PR_INT16_MAX || aValue < PR_INT16_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asInt32(PRInt32 aValue, PRInt32 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asInt64(PRInt32 aValue, PRInt64 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
};
// xpcvariant just calls get double for integers...
template < >
struct variant_float_traits<PRInt32>
{
  static inline nsresult asFloat(PRInt32 aValue, float *_result)
  {
    *_result = float(aValue);
    return NS_OK;
  }
  static inline nsresult asDouble(PRInt32 aValue, double *_result)
  {
    *_result = double(aValue);
    return NS_OK;
  }
};


/**
 * INTEGER types
 */

template < >
struct variant_traits<PRInt64>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_INT64; }
};
template < >
struct variant_integer_traits<PRInt64>
{
  static inline nsresult asBool(PRInt64 aValue, PRBool *_result)
  {
    *_result = (aValue != 0 ? PR_TRUE: PR_FALSE);
    return NS_OK;
  }
  static inline nsresult asInt8(PRInt64 aValue, PRUint8 *_result)
  {
    if (aValue > (PRInt64)PR_UINT8_MAX || aValue < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRUint8)aValue;
    return NS_OK;
  }
  static inline nsresult asInt16(PRInt64 aValue, PRInt16 *_result)
  {
    if (aValue > PR_INT16_MAX || aValue < PR_INT16_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt16)aValue;
    return NS_OK;
  }
  static inline nsresult asInt32(PRInt64 aValue, PRInt32 *_result)
  {
    if (aValue > PR_INT32_MAX || aValue < PR_INT32_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt32)aValue;
    return NS_OK;
  }
  static inline nsresult asInt64(PRInt64 aValue, PRInt64 *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
};
// xpcvariant just calls get double for integers...
template < >
struct variant_float_traits<PRInt64>
{
  static inline nsresult asFloat(PRInt64 aValue, float *_result)
  {
    *_result = float(aValue);
    return NS_OK;
  }
  static inline nsresult asDouble(PRInt64 aValue, double *_result)
  {
    *_result = double(aValue);
    return NS_OK;
  }
};

/**
 * FLOAT types
 */

template < >
struct variant_traits<float>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_FLOAT; }
};
template < >
struct variant_integer_traits<float>
{
  static inline nsresult asBool(float aValue, PRBool *_result)
  {
    *_result = (aValue != 0 ? PR_TRUE: PR_FALSE);
    return NS_OK;
  }
  static inline nsresult asInt8(float aValue, PRUint8 *_result)
  {
    if (aValue > (float)PR_UINT8_MAX || aValue < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRUint8)aValue;
    return NS_OK;
  }
  static inline nsresult asInt16(float aValue, PRInt16 *_result)
  {
    if (aValue > PR_INT16_MAX || aValue < PR_INT16_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt16)aValue;
    return NS_OK;
  }
  static inline nsresult asInt32(float aValue, PRInt32 *_result)
  {
    if (aValue > PR_INT32_MAX || aValue < PR_INT32_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt32)aValue;
    return NS_OK;
  }
  static inline nsresult asInt64(float aValue, PRInt64 *_result)
  {
    *_result = (PRInt64)aValue;
    return NS_OK;
  }
};
template < >
struct variant_float_traits<float>
{
  static inline nsresult asFloat(float aValue, float *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
  static inline nsresult asDouble(float aValue, double *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
};

/**
 * FLOAT types
 */

template < >
struct variant_traits<double>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_DOUBLE; }
};
template < >
struct variant_integer_traits<double>
{
  static inline nsresult asBool(double aValue, PRBool *_result)
  {
    *_result = (aValue != 0 ? PR_TRUE: PR_FALSE);
    return NS_OK;
  }
  static inline nsresult asInt8(double aValue, PRUint8 *_result)
  {
    if (aValue > (double)PR_UINT8_MAX || aValue < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRUint8)aValue;
    return NS_OK;
  }
  static inline nsresult asInt16(double aValue, PRInt16 *_result)
  {
    if (aValue > PR_INT16_MAX || aValue < PR_INT16_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt16)aValue;
    return NS_OK;
  }
  static inline nsresult asInt32(double aValue, PRInt32 *_result)
  {
    if (aValue > PR_INT32_MAX || aValue < PR_INT32_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt32)aValue;
    return NS_OK;
  }
  static inline nsresult asInt64(double aValue, PRInt64 *_result)
  {
    *_result = (PRInt64)aValue;
    return NS_OK;
  }
};
template < >
struct variant_float_traits<double>
{
  static inline nsresult asFloat(double aValue, float *_result)
  {
    if (aValue > FLT_MAX || aValue < FLT_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;
    *_result = (float)aValue;
    return NS_OK;
  }
  static inline nsresult asDouble(double aValue, double *_result)
  {
    *_result = aValue;
    return NS_OK;
  }
};




/**
 * TEXT types
 */

template < >
struct variant_traits<nsString>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_ASTRING; }
};
template < >
struct variant_storage_traits<nsString>
{
  typedef const nsAString & ConstructorType;
  typedef nsString StorageType;
  static inline StorageType storage_conversion(ConstructorType aText)
  {
    return StorageType(aText);
  }
};
template < >
struct variant_integer_traits<nsString>
{
  static inline nsresult asBool(const nsAString &aValue, PRBool *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    PRInt32 v = 0;
    nsresult rc;
    if (PR_sscanf(narrow.get(), "%ld", &v) == 1)
      rc = NS_OK;
    else
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (v != 0 ? PR_TRUE: PR_FALSE);
    return rc;
  }
  static inline nsresult asInt8(const nsAString &aValue, PRUint8 *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    PRInt32 v = 0;
    nsresult rc;
    if (PR_sscanf(narrow.get(), "%ld", &v) == 1)
      rc = NS_OK;
    else
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    if (v > (PRInt32)PR_UINT8_MAX || v < 0)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRUint8)v;
    return rc;
  }
  static inline nsresult asInt16(const nsAString &aValue, PRInt16 *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    PRInt32 v = 0;
    nsresult rc;
    if (PR_sscanf(narrow.get(), "%ld", &v) == 1)
      rc = NS_OK;
    else
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    if (v > PR_INT16_MAX || v < PR_INT16_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt16)v;
    return NS_OK;
  }
  static inline nsresult asInt32(const nsAString &aValue, PRInt32 *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    PRInt32 v = 0;
    nsresult rc;
    if (PR_sscanf(narrow.get(), "%ld", &v) == 1)
      rc = NS_OK;
    else
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    if (v > PR_INT32_MAX || v < PR_INT32_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt32)v;
    return NS_OK;
  }
  static inline nsresult asInt64(const nsAString &aValue, PRInt64 *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    PRInt64 v = 0;
    nsresult rc;
    if (PR_sscanf(narrow.get(), "%lld", &v) == 1)
      rc = NS_OK;
    else
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    *_result = (PRInt64)v;
    return NS_OK;
  }
};
template < >
struct variant_float_traits<nsString>
{
  static inline nsresult asFloat(const nsAString &aValue, float *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

    double v = 0.0;
    nsresult rc;
    const char *str = narrow.get();
    char *conv_stopped;
    // Use PR_strtod, not strtod, since we don't want locale involved.
    v = PR_strtod(str, &conv_stopped);
    if (conv_stopped == str+narrow.Length())
      rc = NS_OK;
    else // Not all the string was scanned
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    if (v > FLT_MAX || v < FLT_MIN)
      return NS_ERROR_CANNOT_CONVERT_DATA;
    *_result = (float)v;
    return rc;
  }
  static inline nsresult asDouble(const nsAString &aValue, double *_result)
  {
    NS_ConvertUTF16toUTF8 narrow(aValue);

//    double v = 0.0;
    nsresult rc;
    const char *str = narrow.get();
    char *conv_stopped;
    // Use PR_strtod, not strtod, since we don't want locale involved.
    *_result = PR_strtod(str, &conv_stopped);
    if (conv_stopped == str+narrow.Length())
      rc = NS_OK;
    else // Not all the string was scanned
      rc = NS_ERROR_CANNOT_CONVERT_DATA;

    return rc;
  }
};
template < >
struct variant_text_traits<nsString>
{
  static inline nsresult asUTF8String(const nsString &aValue,
                                      nsACString &_result)
  {
    CopyUTF16toUTF8(aValue, _result);
    return NS_OK;
  }
  static inline nsresult asString(const nsString &aValue,
                                  nsAString &_result)
  {
    _result = aValue;
    return NS_OK;
  }
};

/**
 * BLOB types
 */

template < >
struct variant_traits<PRUint8[]>
{
  static inline PRUint16 type() { return nsIDataType::VTYPE_ARRAY; }
};
template < >
struct variant_storage_traits<PRUint8[]>
{
  typedef std::pair<const PRUint8 *, PRUint32> ConstructorType;
  typedef nsTArray<PRUint8> StorageType;
  static inline StorageType storage_conversion(ConstructorType aBlob)
  {
    nsTArray<PRUint8> data(aBlob.second);
    (void)data.AppendElements(static_cast<const PRUint8 *>(aBlob.first),
                              aBlob.second);
    return data;
  }
};
template < >
struct variant_blob_traits<PRUint8[]>
{
  static inline nsresult asArray(nsTArray<PRUint8> &aData,
                                 PRUint16 *_type, PRUint32 *_size,
                                 void **_result)
  {
    // For empty blobs, we return nsnull.
    if (aData.Length() == 0) {
      *_result = nsnull;
      *_type = nsIDataType::VTYPE_UINT8;
      *_size = 0;
      return NS_OK;
    }

    // Copy the array
    *_result = nsMemory::Clone(aData.Elements(), aData.Length() * sizeof(PRUint8));
    NS_ENSURE_TRUE(*_result, NS_ERROR_OUT_OF_MEMORY);

    // Set type and size
    *_type = nsIDataType::VTYPE_UINT8;
    *_size = aData.Length();
    return NS_OK;
  }
};

////////////////////////////////////////////////////////////////////////////////
//// Template Implementation

template <typename DataType>
class nsODBCStorageVariant : public nsODBCStorageVariant_base
{
public:
  nsODBCStorageVariant(typename variant_storage_traits<DataType>::ConstructorType aData) :
      mData(variant_storage_traits<DataType>::storage_conversion(aData))
  {
  }

  NS_IMETHOD GetDataType(PRUint16 *_type)
  {
    *_type = variant_traits<DataType>::type();
    return NS_OK;
  }
  NS_IMETHOD GetAsBool(PRBool *_bool)
  {
    return variant_integer_traits<DataType>::asBool(mData, _bool);
  }
  NS_IMETHOD GetAsInt8(PRUint8 *_integer)
  {
    return variant_integer_traits<DataType>::asInt8(mData, _integer);
  }
  NS_IMETHOD GetAsInt16(PRInt16 *_integer)
  {
    return variant_integer_traits<DataType>::asInt16(mData, _integer);
  }
  NS_IMETHOD GetAsInt32(PRInt32 *_integer)
  {
    return variant_integer_traits<DataType>::asInt32(mData, _integer);
  }

  NS_IMETHOD GetAsInt64(PRInt64 *_integer)
  {
    return variant_integer_traits<DataType>::asInt64(mData, _integer);
  }

  NS_IMETHOD GetAsFloat(float *_float)
  {
    return variant_float_traits<DataType>::asFloat(mData, _float);
  }

  NS_IMETHOD GetAsDouble(double *_double)
  {
    return variant_float_traits<DataType>::asDouble(mData, _double);
  }

  NS_IMETHOD GetAsAUTF8String(nsACString &_str)
  {
    return variant_text_traits<DataType>::asUTF8String(mData, _str);
  }

  NS_IMETHOD GetAsAString(nsAString &_str)
  {
    return variant_text_traits<DataType>::asString(mData, _str);
  }

  NS_IMETHOD GetAsArray(PRUint16 *_type, nsIID *, PRUint32 *_size, void **_data)
  {
    return variant_blob_traits<DataType>::asArray(mData, _type, _size, _data);
  }

private:
  nsODBCStorageVariant() { }
  typename variant_storage_traits<DataType>::StorageType mData;
};


////////////////////////////////////////////////////////////////////////////////
//// Handy typedefs!  Use these for the right mapping.

typedef nsODBCStorageVariant<PRBool>  nsODBCStorageBool;
typedef nsODBCStorageVariant<PRInt16> nsODBCStorageInt16;
typedef nsODBCStorageVariant<PRInt32> nsODBCStorageInt32;
typedef nsODBCStorageVariant<PRInt64> nsODBCStorageInt64;
typedef nsODBCStorageVariant<float>   nsODBCStorageFloat;
typedef nsODBCStorageVariant<double>  nsODBCStorageDouble;
typedef nsODBCStorageVariant<nsString>  nsODBCStorageText;
typedef nsODBCStorageVariant<PRUint8[]> nsODBCStorageBlob;
typedef nsODBCStorageVariant_base nsODBCStorageNull;

#endif // __nsODBCStorageVariant_h__
