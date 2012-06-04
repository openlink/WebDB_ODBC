/**********************************************************\

  Auto-generated WebDB_to_ODBCAPI.cpp

\**********************************************************/

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"

#include "script.h"
#include "WebDB_to_ODBCAPI.h"

///////////////////////////////////////////////////////////////////////////////
/// @fn WebDB_to_ODBCAPI::WebDB_to_ODBCAPI(const WebDB_to_ODBCPtr& plugin, const FB::BrowserHostPtr host)
///
/// @brief  Constructor for your JSAPI object.  You should register your methods, properties, and events
///         that should be accessible to Javascript from here.
///
/// @see FB::JSAPIAuto::registerMethod
/// @see FB::JSAPIAuto::registerProperty
/// @see FB::JSAPIAuto::registerEvent
///////////////////////////////////////////////////////////////////////////////
WebDB_to_ODBCAPI::WebDB_to_ODBCAPI(const WebDB_to_ODBCPtr& plugin, const FB::BrowserHostPtr& host) : m_plugin(plugin), m_host(host)
{
    // Read-only property
    registerProperty("version",
                     make_property(this,
                        &WebDB_to_ODBCAPI::get_version));
    
    ////////////////// MY ////////////////////////////////
    registerMethod("CreateConnection",  make_method(this, &WebDB_to_ODBCAPI::CreateConnection));
    registerMethod("getDataSource",     make_method(this, &WebDB_to_ODBCAPI::getDataSource));

    m_henv = NULL;

    SQLRETURN ret;
    ret = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
      SQLSetEnvAttr (m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);
    }

    host->evaluateJavaScript((char *)code);
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebDB_to_ODBCAPI::~WebDB_to_ODBCAPI()
///
/// @brief  Destructor.  Remember that this object will not be released until
///         the browser is done with it; this will almost definitely be after
///         the plugin is released.
///////////////////////////////////////////////////////////////////////////////
WebDB_to_ODBCAPI::~WebDB_to_ODBCAPI()
{
    if (m_henv != NULL) 
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

    m_henv = NULL; 
}

///////////////////////////////////////////////////////////////////////////////
/// @fn WebDB_to_ODBCPtr WebDB_to_ODBCAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
WebDB_to_ODBCPtr WebDB_to_ODBCAPI::getPlugin()
{
    WebDB_to_ODBCPtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}



// Read-only property version
std::string WebDB_to_ODBCAPI::get_version()
{
    return "1.0.0";
}


// Methods
FB::JSAPIPtr WebDB_to_ODBCAPI::CreateConnection()
{
  boost::shared_ptr<Connection> conn = boost::make_shared<Connection>(m_host);
  conn->Init(conn);
  return conn;
}


FB::variant WebDB_to_ODBCAPI::getDataSource(bool getFirst)
{
  SQLWCHAR dsnname[SQL_MAX_DSN_LENGTH+1];
  SQLWCHAR dsndesc[SQL_MAX_DSN_LENGTH+1];
  SQLRETURN ret;
  SQLSMALLINT len;

  ret = SQLDataSourcesW (m_henv, (getFirst ? SQL_FETCH_FIRST:SQL_FETCH_NEXT), 
      dsnname, sizeof (dsnname)/sizeof(wchar_t), 
      &len, dsndesc, sizeof (dsndesc)/sizeof(wchar_t), NULL);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
    throw FB::script_error("Can't get DataSource names");

  if (ret != SQL_NO_DATA)
    return std::wstring(dsnname);

  return FB::FBNull();
}

