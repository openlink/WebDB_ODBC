/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- 
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "npapi.h"
#include "npruntime.h"

class ConnectionObject;

class DatabaseObject : public ScriptablePluginObjectBase
{
public:
  DatabaseObject(NPP npp);
  virtual ~DatabaseObject();

  virtual bool HasMethod(NPIdentifier name);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);

private:
   NPIdentifier mc_CreateConnection_id;
   NPIdentifier mc_GetDataSource_id;

   SQLHANDLE m_henv;

   NPError GetDataSource(bool getFirst, NPVariant *retval);
};


#endif // __DATABASE_H__
