/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStoragePendingStatement.idl
 */

#ifndef __gen_nsIODBCStoragePendingStatement_h__
#define __gen_nsIODBCStoragePendingStatement_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIODBCStoragePendingStatement */
#define NS_IODBCSTORAGEPENDINGSTATEMENT_IID_STR "19e51627-79a8-46f5-9b6a-111905ad1830"

#define NS_IODBCSTORAGEPENDINGSTATEMENT_IID \
  {0x19e51627, 0x79a8, 0x46f5, \
    { 0x9b, 0x6a, 0x11, 0x19, 0x05, 0xad, 0x18, 0x30 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStoragePendingStatement : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGEPENDINGSTATEMENT_IID)

  /**
   * Cancels a pending statement, if possible.  This will only fail if you try
   * cancel more than once.
   *
   * @note For read statements (such as SELECT), you will no longer receive any
   *       notifications about results once cancel is called.
   *
   * @returns true if canceled successfully, false otherwise.
   */
  /* boolean cancel (); */
  NS_SCRIPTABLE NS_IMETHOD Cancel(PRBool *_retval NS_OUTPARAM) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStoragePendingStatement, NS_IODBCSTORAGEPENDINGSTATEMENT_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGEPENDINGSTATEMENT \
  NS_SCRIPTABLE NS_IMETHOD Cancel(PRBool *_retval NS_OUTPARAM); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGEPENDINGSTATEMENT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Cancel(PRBool *_retval NS_OUTPARAM) { return _to Cancel(_retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGEPENDINGSTATEMENT(_to) \
  NS_SCRIPTABLE NS_IMETHOD Cancel(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Cancel(_retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStoragePendingStatement : public nsIODBCStoragePendingStatement
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEPENDINGSTATEMENT

  nsODBCStoragePendingStatement();

private:
  ~nsODBCStoragePendingStatement();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStoragePendingStatement, nsIODBCStoragePendingStatement)

nsODBCStoragePendingStatement::nsODBCStoragePendingStatement()
{
  /* member initializers and constructor code */
}

nsODBCStoragePendingStatement::~nsODBCStoragePendingStatement()
{
  /* destructor code */
}

/* boolean cancel (); */
NS_IMETHODIMP nsODBCStoragePendingStatement::Cancel(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStoragePendingStatement_h__ */
