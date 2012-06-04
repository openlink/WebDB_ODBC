/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageRow.idl
 */

#ifndef __gen_nsIODBCStorageRow_h__
#define __gen_nsIODBCStorageRow_h__


#ifndef __gen_nsIODBCStorageValueArray_h__
#include "nsIODBCStorageValueArray.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIVariant; /* forward declaration */


/* starting interface:    nsIODBCStorageRow */
#define NS_IODBCSTORAGEROW_IID_STR "ed68f791-e3c4-44ec-b419-2cd0e9efc985"

#define NS_IODBCSTORAGEROW_IID \
  {0xed68f791, 0xe3c4, 0x44ec, \
    { 0xb4, 0x19, 0x2c, 0xd0, 0xe9, 0xef, 0xc9, 0x85 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageRow : public nsIODBCStorageValueArray {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGEROW_IID)

  /**
   * Obtains the result of a given column specified by aIndex.
   *
   * @param aIndex
   *        Zero-based index of the result to get from the tuple.
   * @returns the result of the specified column.
   */
  /* nsIVariant getResultByIndex (in unsigned short aIndex); */
  NS_SCRIPTABLE NS_IMETHOD GetResultByIndex(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) = 0;

  /**
   * Obtains the result of a given column specified by aIndex.
   *
   * @param aName
   *        Name of the result to get from the tuple.
   * @returns the result of the specified column.
   */
  /* nsIVariant getResultByName (in AString aName); */
  NS_SCRIPTABLE NS_IMETHOD GetResultByName(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageRow, NS_IODBCSTORAGEROW_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGEROW \
  NS_SCRIPTABLE NS_IMETHOD GetResultByIndex(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetResultByName(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGEROW(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetResultByIndex(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) { return _to GetResultByIndex(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetResultByName(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) { return _to GetResultByName(aName, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGEROW(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetResultByIndex(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetResultByIndex(aIndex, _retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetResultByName(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetResultByName(aName, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageRow : public nsIODBCStorageRow
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEROW

  nsODBCStorageRow();

private:
  ~nsODBCStorageRow();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageRow, nsIODBCStorageRow)

nsODBCStorageRow::nsODBCStorageRow()
{
  /* member initializers and constructor code */
}

nsODBCStorageRow::~nsODBCStorageRow()
{
  /* destructor code */
}

/* nsIVariant getResultByIndex (in unsigned short aIndex); */
NS_IMETHODIMP nsODBCStorageRow::GetResultByIndex(PRUint16 aIndex, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* nsIVariant getResultByName (in AString aName); */
NS_IMETHODIMP nsODBCStorageRow::GetResultByName(const nsAString & aName, nsIVariant **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageRow_h__ */
