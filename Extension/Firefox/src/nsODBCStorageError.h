/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __nsODBCStorageError_h__
#define __nsODBCStorageError_h__

#include "nsIODBCStorageError.h"
#include "nsStringAPI.h"

class nsODBCStorageError : public nsIODBCStorageError
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEERROR

  nsODBCStorageError(int aCode, const nsAString& aState, const nsAString& aMessage);

private:
  int mCode;
  nsString mMessage;
  nsString mState;
};

#endif // __nsODBCStorageError_h__
