/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef _NSODBCSTORAGESTATEMENT_H_
#define _NSODBCSTORAGESTATEMENT_H_

#include <sql.h>
#include <sqlext.h>

#include "xpcom-config.h"
#include "nsAutoPtr.h"
#include "nsStringAPI.h"
#include "nsTArray.h"
//??#include "nsVoidArray.h"
#include "nsCOMArray.h"

#include "nsIVariant.h"
#include "nsIODBCStorageStatement.h"
#include "nsODBCUtils.h"



class nsODBCStorageConnection;

class nsODBCStorageStatement : public nsIODBCStorageStatement
{
    friend class nsODBCStorageConnection;

public:
    nsODBCStorageStatement();

    // interfaces
    NS_DECL_ISUPPORTS
    NS_DECL_NSIODBCSTORAGESTATEMENT
    NS_DECL_NSIODBCSTORAGEVALUEARRAY

    /**
     * Initializes the object on aDBConnection by preparing the SQL statement
     * given by aSQLStatement.
     *
     * @param aDBConnection
     *        The nsODBCStorageConnection object this statement is associated with.
     * @param aSQLquery
     *        The SQL query to prepare that this object will represent.
     */
    nsresult Initialize(nsODBCStorageConnection *aDBConnection,
                        const nsAString &aSQLquery);
    nsresult Initialize(nsODBCStorageConnection *aDBConnection, SQLHANDLE hstmt);

    nsresult Prepare();

private:
    virtual ~nsODBCStorageStatement();

protected:
    NS_IMETHOD CheckRC(SWORD fHandleType, SQLHANDLE handle, SQLRETURN rc);
    NS_IMETHOD CheckDbcRC(SQLRETURN rc);
    NS_IMETHOD CheckStmtRC(SQLRETURN rc);

    NS_IMETHODIMP GetVariantMz(PRUint16 aIndex, nsIVariant **_retval);

    nsRefPtr<nsODBCStorageConnection> mDBConnection;
    SQLHANDLE m_hstmt;					
    SQLHANDLE m_hdbc;					

    PRBool mPrepared;
    nsString  m_errorMessage;
    nsString  m_errorState;
    long      m_errorCode;

    PRUint16 mParamCount;
    PRUint16 mResultColumnCount;
    nsTArray<nsString> mColumnNames;
    PRBool mExecuting;
    nsString m_SQLquery;
    PRBool mWasNull;
  
    nsCOMPtr<nsIVariant> m_Data;
    PRInt32              m_col;
    
    nsCOMArray<nsODBCParam> mParams;

};

#endif /* _NSODBCSTORAGESTATEMENT_H_ */
