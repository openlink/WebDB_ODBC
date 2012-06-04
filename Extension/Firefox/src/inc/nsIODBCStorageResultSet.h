/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageResultSet.idl
 */

#ifndef __gen_nsIODBCStorageResultSet_h__
#define __gen_nsIODBCStorageResultSet_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIODBCStorageRow; /* forward declaration */


/* starting interface:    nsIODBCStorageResultSet */
#define NS_IODBCSTORAGERESULTSET_IID_STR "8c0682f4-3071-48a9-af07-a573de0137f7"

#define NS_IODBCSTORAGERESULTSET_IID \
  {0x8c0682f4, 0x3071, 0x48a9, \
    { 0xaf, 0x07, 0xa5, 0x73, 0xde, 0x01, 0x37, 0xf7 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageResultSet : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGERESULTSET_IID)

  /**
   * Obtains the next row from the result set from the statement that was
   * executed.
   *
   * @returns the next row from the result set.  This will be null when there
   *          are no more results.
   */
  /* nsIODBCStorageRow getNextRow (); */
  NS_SCRIPTABLE NS_IMETHOD GetNextRow(nsIODBCStorageRow **_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageResultSet, NS_IODBCSTORAGERESULTSET_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGERESULTSET \
  NS_SCRIPTABLE NS_IMETHOD GetNextRow(nsIODBCStorageRow **_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGERESULTSET(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetNextRow(nsIODBCStorageRow **_retval NS_OUTPARAM) { return _to GetNextRow(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGERESULTSET(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetNextRow(nsIODBCStorageRow **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetNextRow(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageResultSet : public nsIODBCStorageResultSet
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGERESULTSET

  nsODBCStorageResultSet();

private:
  ~nsODBCStorageResultSet();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageResultSet, nsIODBCStorageResultSet)

nsODBCStorageResultSet::nsODBCStorageResultSet()
{
  /* member initializers and constructor code */
}

nsODBCStorageResultSet::~nsODBCStorageResultSet()
{
  /* destructor code */
}

/* nsIODBCStorageRow getNextRow (); */
NS_IMETHODIMP nsODBCStorageResultSet::GetNextRow(nsIODBCStorageRow **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageResultSet_h__ */
