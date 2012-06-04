/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsODBCStorageError.h"

////////////////////////////////////////////////////////////////////////////////
//// nsODBCStorageError

/**
 * Note:  This object is only ever accessed on one thread at a time.  It it not
 *        threadsafe, but it does need threadsafe AddRef and Release.
 */
NS_IMPL_THREADSAFE_ISUPPORTS1(nsODBCStorageError, nsIODBCStorageError)

nsODBCStorageError::nsODBCStorageError(int aCode, const nsAString& aState, const nsAString& aMessage) :
    mCode(aCode)
  , mMessage(aMessage)
  , mState(aState)
{
}

////////////////////////////////////////////////////////////////////////////////
//// mozIStorageError

NS_IMETHODIMP
nsODBCStorageError::GetCode(PRInt32 *aCode)
{
  *aCode = mCode;
  return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageError::GetMessage(nsAString & aMessage)
{
  aMessage = mMessage;
  return NS_OK;
}

NS_IMETHODIMP
nsODBCStorageError::GetState(nsAString & aState)
{
  aState = mState;
  return NS_OK;
}

