/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM .\IDL_MOZ\nsIODBCStorageError.idl
 */

#ifndef __gen_nsIODBCStorageError_h__
#define __gen_nsIODBCStorageError_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
#ifdef ERROR
#undef ERROR
#endif

/* starting interface:    nsIODBCStorageError */
#define NS_IODBCSTORAGEERROR_IID_STR "b076e53e-2cb7-40f9-9b9e-b2d3eee5e050"

#define NS_IODBCSTORAGEERROR_IID \
  {0xb076e53e, 0x2cb7, 0x40f9, \
    { 0x9b, 0x9e, 0xb2, 0xd3, 0xee, 0xe5, 0xe0, 0x50 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIODBCStorageError : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_IODBCSTORAGEERROR_IID)

  /**
   * General SQL error or missing database.
   */
  enum { ERROR = 1 };

  /**
   * Indicates what type of error occurred.
   */
  /* readonly attribute long code; */
  NS_SCRIPTABLE NS_IMETHOD GetCode(PRInt32 *aCode) = 0;

  /**
   * An error string the gives more details, if available.
   */
  /* readonly attribute AString message; */
  NS_SCRIPTABLE NS_IMETHOD GetMessage(nsAString & aMessage) = 0;

  /* readonly attribute AString state; */
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIODBCStorageError, NS_IODBCSTORAGEERROR_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIODBCSTORAGEERROR \
  NS_SCRIPTABLE NS_IMETHOD GetCode(PRInt32 *aCode); \
  NS_SCRIPTABLE NS_IMETHOD GetMessage(nsAString & aMessage); \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIODBCSTORAGEERROR(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetCode(PRInt32 *aCode) { return _to GetCode(aCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetMessage(nsAString & aMessage) { return _to GetMessage(aMessage); } \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) { return _to GetState(aState); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIODBCSTORAGEERROR(_to) \
  NS_SCRIPTABLE NS_IMETHOD GetCode(PRInt32 *aCode) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCode(aCode); } \
  NS_SCRIPTABLE NS_IMETHOD GetMessage(nsAString & aMessage) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMessage(aMessage); } \
  NS_SCRIPTABLE NS_IMETHOD GetState(nsAString & aState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetState(aState); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsODBCStorageError : public nsIODBCStorageError
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEERROR

  nsODBCStorageError();

private:
  ~nsODBCStorageError();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsODBCStorageError, nsIODBCStorageError)

nsODBCStorageError::nsODBCStorageError()
{
  /* member initializers and constructor code */
}

nsODBCStorageError::~nsODBCStorageError()
{
  /* destructor code */
}

/* readonly attribute long code; */
NS_IMETHODIMP nsODBCStorageError::GetCode(PRInt32 *aCode)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString message; */
NS_IMETHODIMP nsODBCStorageError::GetMessage(nsAString & aMessage)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute AString state; */
NS_IMETHODIMP nsODBCStorageError::GetState(nsAString & aState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIODBCStorageError_h__ */
