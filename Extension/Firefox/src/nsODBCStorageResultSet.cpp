/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined (UNIX)
#endif

#include <sql.h>

#include "nsODBCStorageRow.h"
#include "nsODBCStorageResultSet.h"

////////////////////////////////////////////////////////////////////////////////
//// mozStorageResultSet

/**
 * Note:  This object is only ever accessed on one thread at a time.  It it not
 *        threadsafe, but it does need threadsafe AddRef and Release.
 */
NS_IMPL_THREADSAFE_ISUPPORTS1(nsODBCStorageResultSet, nsIODBCStorageResultSet)

nsODBCStorageResultSet::nsODBCStorageResultSet() :
    mCurrentIndex(0)
{
}

nsODBCStorageResultSet::~nsODBCStorageResultSet()
{
  mData.Clear();
}

nsresult
nsODBCStorageResultSet::add(nsIODBCStorageRow *aRow)
{
  return mData.AppendObject(aRow) ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

////////////////////////////////////////////////////////////////////////////////
//// nsIODBCStorageResultSet

NS_IMETHODIMP
nsODBCStorageResultSet::GetNextRow(nsIODBCStorageRow **_row)
{
  NS_ENSURE_ARG_POINTER(_row);

  if (mCurrentIndex >= mData.Count()) {
    // Just return null here
    return NS_OK;
  }

  NS_ADDREF(*_row = mData.ObjectAt(mCurrentIndex++));
  return NS_OK;
}
