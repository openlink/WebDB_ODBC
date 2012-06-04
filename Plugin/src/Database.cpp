/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- 
 *
 * ***** END LICENSE BLOCK ***** */

#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif


#ifdef XP_UNIX
#include <string.h>
#endif

#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include <wchar.h>


#include "plugin.h"
#include "npfunctions.h"
//#include "prtypes.h"

#include "utils.h"
#include "mthreads.h"
#include "Database.h"
#include "Connection.h"




static NPObject *
AllocateConnectionObject(NPP npp, NPClass *aClass)
{
  return new ConnectionObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ConnectionObject,  AllocateConnectionObject);

//========================================================================
//========================================================================


DatabaseObject::DatabaseObject(NPP npp)
    : ScriptablePluginObjectBase(npp)
{
    mc_CreateConnection_id = NPN_GetStringIdentifier("CreateConnection");
    mc_GetDataSource_id = NPN_GetStringIdentifier("getDataSource");
    m_henv = NULL;

    SQLRETURN ret;
    ret = SQLAllocHandle (SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
      SQLSetEnvAttr (m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER);
    }
}


DatabaseObject::~DatabaseObject()
{
    if (m_henv != NULL) 
      SQLFreeHandle(SQL_HANDLE_ENV, m_henv);

    m_henv = NULL; 
}


bool
DatabaseObject::HasMethod(NPIdentifier name)
{
//sprintf(tmp, " HasMethod  [%s]\n",  NPN_UTF8FromIdentifier(name)); log(tmp);
  if (name == mc_CreateConnection_id)
    return true;
  else if (name == mc_GetDataSource_id)
    return true;
  else
    return false;
}

bool
DatabaseObject::HasProperty(NPIdentifier name)
{
//sprintf(tmp, " HasProperty [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  return false;
}

bool
DatabaseObject::GetProperty(NPIdentifier name, NPVariant *result)
{
//sprintf(tmp, " GetProperty  [%s]\n",  NPN_UTF8FromIdentifier(name)); log(tmp);
  return false;
}

bool
DatabaseObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
//sprintf(tmp, " SetProperty  [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  return false;
}

bool
DatabaseObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
//sprintf(tmp, " Invoke  [%s]\n", NPN_UTF8FromIdentifier(name)); log(tmp);
  VOID_TO_NPVARIANT(*result);

  if (name == mc_CreateConnection_id) {
    if (argCount > 0) {
      NPN_SetException(this, "Wrong parameters count");
      return true;
    }

    NPObject *pObj = NPN_CreateObject(mNpp, GET_NPOBJECT_CLASS(ConnectionObject));
    OBJECT_TO_NPVARIANT(pObj, *result);
    return true;
  } else if (name == mc_GetDataSource_id) {
    if (argCount < 1) {
      NPN_SetException(this, "Too few parameters count");
      return true;
    }
    if (!NPVARIANT_IS_BOOLEAN(args[0])) {
      NPN_SetException(this, "Wrong argument type");
      return true;
    }

    GetDataSource(NPVARIANT_TO_BOOLEAN(args[0]), result);
    return true;

  }
  return false;
}


NPError
DatabaseObject::GetDataSource(bool getFirst, NPVariant *retval)
{
  SQLWCHAR dsnname[SQL_MAX_DSN_LENGTH+1];
  SQLWCHAR dsndesc[SQL_MAX_DSN_LENGTH+1];
  SQLRETURN ret;
  SQLSMALLINT len;

  NULL_TO_NPVARIANT(*retval);

  ret = SQLDataSourcesW (m_henv, (getFirst ? SQL_FETCH_FIRST:SQL_FETCH_NEXT), 
      dsnname, sizeof (dsnname)/sizeof(wchar_t), 
      &len, dsndesc, sizeof (dsndesc)/sizeof(wchar_t), NULL);
  if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
    goto error;

  if (ret != SQL_NO_DATA) {
    const char *data = strdup_WtoU8(dsnname);
    if (!data) {
      NPN_SetException(this, "Memory allocation error");
      return NPERR_GENERIC_ERROR;
    }
    STRING_TO_NPVARIANT(data, *retval);
  }

  return NPERR_NO_ERROR;

error:
  NPN_SetException(this, "Can't get DataSource names");
  return NPERR_GENERIC_ERROR;

}
