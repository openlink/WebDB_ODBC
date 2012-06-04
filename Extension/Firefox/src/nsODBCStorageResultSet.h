/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __nsODBCStorageResultSet_h__
#define __nsODBCStorageResultSet_h__

#include "nsIODBCStorageResultSet.h"
#include "nsCOMArray.h"
class nsIODBCStorageRow;

class nsODBCStorageResultSet : public nsIODBCStorageResultSet
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGERESULTSET

  nsODBCStorageResultSet();
  ~nsODBCStorageResultSet();

  /**
   * Adds a tuple to this result set.
   */
  nsresult add(nsIODBCStorageRow *aTuple);

  /**
   * @returns the number of rows this result set holds.
   */
  PRInt32 rows() const { return mData.Count(); }

private:
  /**
   * Stores the current index of the active result set.
   */
  PRInt32 mCurrentIndex;
  /**
   * Stores the tuples.
   */
  nsCOMArray<nsIODBCStorageRow> mData;
};

#endif // __nsODBCStorageResultSet_h__
