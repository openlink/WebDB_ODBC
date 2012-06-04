/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif

#include <sql.h>

#include "nsStringAPI.h"

#include "nsODBCStorageRow.h"

////////////////////////////////////////////////////////////////////////////////
//// nsODBCStorageRow

/**
 * Note:  This object is only ever accessed on one thread at a time.  It it not
 *        threadsafe, but it does need threadsafe AddRef and Release.
 */
NS_IMPL_THREADSAFE_ISUPPORTS2(
  nsODBCStorageRow,
  nsIODBCStorageRow,
  nsIODBCStorageValueArray
)

nsresult
nsODBCStorageRow::Initialize(nsIODBCStorageStatement *aStatement)
{
  mStatement = aStatement;

  // Get the number of results
  mStatement->GetColumnCount(&mNumCols);

  // Start copying over values
  for (PRUint16 i = 0; i < mNumCols; i++) {
    // Store the value
    nsIVariant *variant = nsnull;

    nsresult rc = mStatement->GetVariant(i, &variant);
    if (rc != NS_OK)
      return rc;

    // Insert into our storage array
    NS_ENSURE_TRUE(mData.InsertObjectAt(variant, i), NS_ERROR_OUT_OF_MEMORY);
  }

  return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
//// nsIODBCStorageRow

NS_IMETHODIMP
nsODBCStorageRow::GetResultByIndex(PRUint16 aIndex, nsIVariant **_result)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  NS_ADDREF(*_result = mData.ObjectAt(aIndex));
  return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageRow::GetResultByName(const nsAString &aName, nsIVariant **_result)
{
  PRUint16 index;
  NS_ENSURE_SUCCESS(mStatement->GetColumnIndex(aName, &index), NS_ERROR_NOT_AVAILABLE);
  return GetResultByIndex(index, _result);
}

////////////////////////////////////////////////////////////////////////////////
//// nsIODBCStorageValueArray

NS_IMETHODIMP
nsODBCStorageRow::GetNumEntries(PRUint16 *_entries)
{
  *_entries = mNumCols;
  return NS_OK;
}

NS_IMETHODIMP 
nsODBCStorageRow::IsNullable(PRUint16 aIndex, PRBool *_retval NS_OUTPARAM)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mStatement->IsNullable(aIndex, _retval);
}

NS_IMETHODIMP
nsODBCStorageRow::GetTypeOfIndex(PRUint16 aIndex, PRInt32 *_type)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mStatement->GetTypeOfIndex(aIndex, _type);
}

NS_IMETHODIMP
nsODBCStorageRow::GetBool(PRUint16 aIndex, PRBool *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsBool(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetVariant(PRUint16 aIndex, nsIVariant **_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  NS_IF_ADDREF(*_value = mData.ObjectAt(aIndex)); 
  return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageRow::Data(const nsAString &aName, nsIVariant **_value)
{
  PRUint16 aIndex;
  NS_ENSURE_SUCCESS(mStatement->GetColumnIndex(aName, &aIndex), NS_ERROR_NOT_AVAILABLE);
  NS_IF_ADDREF(*_value = mData.ObjectAt(aIndex)); 
  return NS_OK;
}


NS_IMETHODIMP
nsODBCStorageRow::GetInt16(PRUint16 aIndex, PRInt16 *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsInt16(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetInt32(PRUint16 aIndex, PRInt32 *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsInt32(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetInt64(PRUint16 aIndex, PRInt64 *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsInt64(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetFloat(PRUint16 aIndex, float *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsFloat(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetDouble(PRUint16 aIndex, double *_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsDouble(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetUTF8String(PRUint16 aIndex, nsACString &_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsAUTF8String(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetString(PRUint16 aIndex, nsAString &_value)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  return mData.ObjectAt(aIndex)->GetAsAString(_value);
}


NS_IMETHODIMP
nsODBCStorageRow::GetBlob(PRUint16 aIndex, PRUint32 *_size, PRUint8 **_blob)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  PRUint16 type;
  nsIID interfaceIID;
  return mData.ObjectAt(aIndex)->GetAsArray(&type, &interfaceIID, _size,
                                            reinterpret_cast<void **>(_blob));
}


NS_IMETHODIMP
nsODBCStorageRow::GetIsNull(PRUint16 aIndex, PRBool *_isNull)
{
  if (aIndex >= mNumCols)
    return NS_ERROR_ILLEGAL_VALUE;

  PRUint16 type;
  (void)mData.ObjectAt(aIndex)->GetDataType(&type);
  *_isNull = type == nsIDataType::VTYPE_EMPTY;
  return NS_OK;
}


