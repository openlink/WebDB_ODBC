/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

////////////////////////////////////////////////////////////
//
// Implementation of plugin entry points (NPP_*)
// most are just empty stubs for this particular plugin 
//
#include "plugin.h"
#include <string.h>

#if defined(XP_MACOSX) && !defined(CHROME)
#include "mthreads.h"
#include "FB_OneShotManager.h"
#endif

#ifdef XP_MACOSX

#pragma GCC visibility push(default)
extern "C"
{
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[], char* argv[], NPSavedData* saved);
NPError NPP_Destroy(NPP instance, NPSavedData** save);
NPError NPP_SetWindow(NPP instance, NPWindow* window);
NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason);
int32_t NPP_WriteReady(NPP instance, NPStream* stream);
int32_t NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer);
void    NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void    NPP_Print(NPP instance, NPPrint* platformPrint);
int16_t NPP_HandleEvent(NPP instance, void* event);
void    NPP_URLNotify(NPP instance, const char* URL, NPReason reason, void* notifyData);
NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);
}
#pragma GCC visibility pop
#endif /* XP_MACOSX */




char*
NPP_GetMIMEDescription(void)
{
#ifdef CHROME
  return (char*)"application/iodbc-chrome:.:NPIODBC_Chrome;application/iodbc:.:NPIODBC_Chrome";
#else
  return (char*)"application/iodbc:.:NPIODBC;application/iodbc-safari:.:NPIODBC;application/iodbc-opera:.:NPIODBC";
#endif
}



NPError NPP_Initialize(void)
{
  return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
}


// here the plugin creates an instance of our CPlugin object which 
// will be associated with this newly created plugin instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16_t mode,
                int16_t argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{   
  int isSafari = 0;

  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  char *agent = (char *)NPN_UserAgent(instance);
  if (strstr(agent, "Safari"))
    isSafari = 1;

  NPN_SetValue(instance, NPPVpluginWindowBool, (void*)false);
#ifdef XP_MACOSX
  NPN_SetValue(instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
#endif

  CPlugin * pPlugin = new CPlugin(instance, isSafari);
  if(pPlugin == NULL)
    return NPERR_OUT_OF_MEMORY_ERROR;

  instance->pdata = (void *)pPlugin;

  return NPERR_NO_ERROR;
}

// here is the place to clean up and destroy the CPlugin object
NPError NPP_Destroy (NPP instance, NPSavedData** save)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

#if defined(XP_MACOSX) && !defined(CHROME)
  OneShotManager::getInstance().clear(instance);
#endif

  CPlugin * pPlugin = (CPlugin *)instance->pdata;
  if(pPlugin != NULL) {
    pPlugin->shut();
    delete pPlugin;
    instance->pdata = NULL;
  }
  return rv;
}

// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow (NPP instance, NPWindow* pNPWindow)
{    
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  if(pNPWindow == NULL)
    return NPERR_GENERIC_ERROR;
    
  CPlugin * pPlugin = (CPlugin *)instance->pdata;

  if(pPlugin == NULL) 
    return NPERR_GENERIC_ERROR;

  // window just created
  if(!pPlugin->isInitialized() && (pNPWindow->window != NULL)) { 
    if(!pPlugin->init(pNPWindow)) {
      delete pPlugin;
      pPlugin = NULL;
      return NPERR_MODULE_LOAD_FAILED_ERROR;
    }
  }

  // window goes away
  if((pNPWindow->window == NULL) && pPlugin->isInitialized())
    return NPERR_NO_ERROR;

  // window resized
  if(pPlugin->isInitialized() && (pNPWindow->window != NULL))
    return NPERR_NO_ERROR;

  // this should not happen, nothing to do
  if((pNPWindow->window == NULL) && !pPlugin->isInitialized())
    return NPERR_NO_ERROR;

  return rv;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{

  switch (variable) {
  case NPPVpluginNameString:
#ifdef CHROME
    *((char **)value) = (char*)"NPIODBC_Chrome";
#else
    *((char **)value) = (char*)"NPIODBC";
#endif
    break;
  case NPPVpluginDescriptionString:
#ifdef CHROME
    *((char **)value) = (char*)"OpenLink HTML5 WebDb-to-ODBC (Bridge) Plugin ver:1.0.3 for Chrome";
#else
    *((char **)value) = (char*)"OpenLink HTML5 WebDb-to-ODBC (Bridge) Plugin ver:1.0.3";
#endif
    break;
  case NPPVpluginScriptableNPObject:
    {
      if(instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

      CPlugin * plugin = (CPlugin *)instance->pdata;
      if(plugin == NULL)
        return NPERR_GENERIC_ERROR;
    
      *(NPObject **)value = plugin->GetScriptableObject();
      break;
    }
  case NPPVpluginNeedsXEmbed:
    *(bool*)value = false;
    break;
  default:
    return NPERR_GENERIC_ERROR;
  }

  return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance,
                      NPMIMEType type,
                      NPStream* stream, 
                      NPBool seekable,
                      uint16_t* stype)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int32_t NPP_WriteReady (NPP instance, NPStream *stream)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32_t rv = 0x0fffffff;
  return rv;
}

int32_t NPP_Write (NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{   
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32_t rv = len;
  return rv;
}

NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname)
{
  if(instance == NULL)
    return;
}

void NPP_Print (NPP instance, NPPrint* printInfo)
{
  if(instance == NULL)
    return;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
  if(instance == NULL)
    return;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int16_t	NPP_HandleEvent(NPP instance, void* event)
{
  if(instance == NULL)
    return 0;

  int16_t rv = 0;
  CPlugin * pPlugin = (CPlugin *)instance->pdata;
  if (pPlugin)
    rv = pPlugin->handleEvent(event);

  return rv;
}

NPObject *NPP_GetScriptableInstance(NPP instance)
{
  if(!instance)
    return 0;

  NPObject *npobj = 0;
  CPlugin * pPlugin = (CPlugin *)instance->pdata;
  if (!pPlugin)
    npobj = pPlugin->GetScriptableObject();

  return npobj;
}
