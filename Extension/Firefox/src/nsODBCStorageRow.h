/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __nsODBCStorageRow_h__
#define __nsODBCStorageRow_h__

#include "nsIODBCStorageRow.h"
#include "nsCOMArray.h"
#include "nsCOMPtr.h"
#include "nsIODBCStorageStatement.h"

class nsIVariant;


class nsODBCStorageRow : public nsIODBCStorageRow
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIODBCSTORAGEROW
  NS_DECL_NSIODBCSTORAGEVALUEARRAY

  /**
   * Initializes the object with the given statement.  Copies the values from
   * the statement.
   *
   * @param aStatement
   *        The sqlite statement to pull results from.
   */
  nsresult Initialize(nsIODBCStorageStatement *aStatement);

private:
   virtual ~nsODBCStorageRow()  { };

  /**
   * The number of columns in this tuple.
   */
  PRUint16 mNumCols;
  PRBool mWasNull;

  /**
   * Stores the data in the tuple.
   */
  nsCOMArray<nsIVariant> mData;

  nsCOMPtr<nsIODBCStorageStatement> mStatement;
};

#endif // __nsODBCStorageRow_h__
