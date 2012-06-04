/**********************************************************\

  Auto-generated WebDB_to_ODBCAPI.h

\**********************************************************/

#include <windows.h>
#include <windowsx.h>

#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "WebDB_to_ODBC.h"
#include <sql.h>
#include <sqlext.h>
#include "Connection.h"

#ifndef H_WebDB_to_ODBCAPI
#define H_WebDB_to_ODBCAPI

class WebDB_to_ODBCAPI : public FB::JSAPIAuto
{
public:
    WebDB_to_ODBCAPI(const WebDB_to_ODBCPtr& plugin, const FB::BrowserHostPtr& host);
    virtual ~WebDB_to_ODBCAPI();

    WebDB_to_ODBCPtr getPlugin();

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    FB::variant getDataSource(bool getFirst);
    FB::JSAPIPtr CreateConnection();

private:
    WebDB_to_ODBCWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    std::string m_testString;
    SQLHANDLE m_henv;
};

#endif // H_WebDB_to_ODBCAPI

