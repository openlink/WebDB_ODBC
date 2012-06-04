/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

//////////////////////////////////////////////////////////////
//
// Main plugin entry point implementation
//
#include "npapi.h"
#include "npfunctions.h"

#ifndef HIBYTE
#define HIBYTE(x) ((((uint32_t)(x)) & 0xff00) >> 8)
#endif


NPNetscapeFuncs NPNFuncs;

#ifdef XP_MACOSX

#pragma GCC visibility push(default)
extern "C"
{
NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
NPError NP_Shutdown(void);
#if 0
int main(NPNetscapeFuncs* nCallbacks, NPPluginFuncs* pCallbacks, NPP_ShutdownProcPtr* unloadProcPtr);
#endif

}
#pragma GCC visibility pop
#endif /* XP_MACOSX */


#if defined(XP_WIN) || defined(XP_MACOSX)

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
  if(pFuncs == NULL)
    return NPERR_INVALID_FUNCTABLE_ERROR;

  if(pFuncs->size < sizeof(NPPluginFuncs))
    return NPERR_INVALID_FUNCTABLE_ERROR;

  pFuncs->version       = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
  pFuncs->size 		= sizeof(NPPluginFuncs);
  pFuncs->newp          = NPP_New;
  pFuncs->destroy       = NPP_Destroy;
  pFuncs->setwindow     = NPP_SetWindow;
  pFuncs->newstream     = NPP_NewStream;
  pFuncs->destroystream = NPP_DestroyStream;
  pFuncs->asfile        = NPP_StreamAsFile;
  pFuncs->writeready    = NPP_WriteReady;
  pFuncs->write         = NPP_Write;
  pFuncs->print         = NPP_Print;
  pFuncs->event         = NPP_HandleEvent;
  pFuncs->urlnotify     = NPP_URLNotify;
  pFuncs->getvalue      = NPP_GetValue;
  pFuncs->setvalue      = NPP_SetValue;
  pFuncs->javaClass     = NULL;

  return NPERR_NO_ERROR;
}

#endif 

char *NPP_GetMIMEDescription();

char*
NP_GetPluginVersion(void) {
    return (char *)"1.0.7";
}


char*
NP_GetMIMEDescription()
{
  return NPP_GetMIMEDescription();
}

NPError 
NP_GetValue(void* future, NPPVariable variable, void *value)
{
  return NPP_GetValue((NPP_t *)future, variable, value);
}

NPError OSCALL
NP_Initialize(NPNetscapeFuncs* pFuncs
#ifdef XP_UNIX
              , NPPluginFuncs* pluginFuncs
#endif
              )
{
  if(pFuncs == NULL)
    return NPERR_INVALID_FUNCTABLE_ERROR;

  if(HIBYTE(pFuncs->version) > NP_VERSION_MAJOR)
    return NPERR_INCOMPATIBLE_VERSION_ERROR;

  if (pFuncs->size < ((char *)&pFuncs->posturlnotify - (char *)pFuncs))
    return NPERR_INVALID_FUNCTABLE_ERROR;

#ifdef XP_UNIX
  if (pluginFuncs->size < sizeof(NPPluginFuncs))
    return NPERR_INVALID_FUNCTABLE_ERROR;
#endif

  int minor = pFuncs->version & 0xFF;

  NPNFuncs.version                 = pFuncs->version;
  NPNFuncs.size                    = pFuncs->size;
  NPNFuncs.posturl                 = pFuncs->posturl;
  NPNFuncs.geturl                  = pFuncs->geturl;
  NPNFuncs.requestread             = pFuncs->requestread;
  NPNFuncs.newstream               = pFuncs->newstream;
  NPNFuncs.write                   = pFuncs->write;
  NPNFuncs.destroystream           = pFuncs->destroystream;
  NPNFuncs.status                  = pFuncs->status;
  NPNFuncs.uagent                  = pFuncs->uagent;
  NPNFuncs.memalloc                = pFuncs->memalloc;
  NPNFuncs.memfree                 = pFuncs->memfree;
  NPNFuncs.memflush                = pFuncs->memflush;
  NPNFuncs.reloadplugins           = pFuncs->reloadplugins;

#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) >= 20)
  NPNFuncs.pluginthreadasynccall   = pFuncs->pluginthreadasynccall;
#endif

  if( minor >= NPVERS_HAS_LIVECONNECT )
  {
    NPNFuncs.getJavaEnv              = NULL;
    NPNFuncs.getJavaPeer             = NULL;
  }

  NPNFuncs.getvalue                = pFuncs->getvalue;
  NPNFuncs.setvalue                = pFuncs->setvalue;

  if( minor >= NPVERS_HAS_NOTIFICATION )
  {
    NPNFuncs.geturlnotify            = pFuncs->geturlnotify;
    NPNFuncs.posturlnotify           = pFuncs->posturlnotify;
  }

  if (pFuncs->size >= ((char *)&pFuncs->setexception - (char *)pFuncs))
  {
    NPNFuncs.invalidaterect          = pFuncs->invalidaterect;
    NPNFuncs.invalidateregion        = pFuncs->invalidateregion;
    NPNFuncs.forceredraw             = pFuncs->forceredraw;

    /* npruntime support */
    if (minor >= 14)
    {
      NPNFuncs.getstringidentifier     = pFuncs->getstringidentifier;
      NPNFuncs.getstringidentifiers    = pFuncs->getstringidentifiers;
      NPNFuncs.getintidentifier        = pFuncs->getintidentifier;
      NPNFuncs.identifierisstring      = pFuncs->identifierisstring;
      NPNFuncs.utf8fromidentifier      = pFuncs->utf8fromidentifier;
      NPNFuncs.intfromidentifier       = pFuncs->intfromidentifier;
      NPNFuncs.createobject            = pFuncs->createobject;
      NPNFuncs.retainobject            = pFuncs->retainobject;
      NPNFuncs.releaseobject           = pFuncs->releaseobject;
      NPNFuncs.invoke                  = pFuncs->invoke;
      NPNFuncs.invokeDefault           = pFuncs->invokeDefault;
      NPNFuncs.evaluate                = pFuncs->evaluate;
      NPNFuncs.getproperty             = pFuncs->getproperty;
      NPNFuncs.setproperty             = pFuncs->setproperty;
      NPNFuncs.removeproperty          = pFuncs->removeproperty;
      NPNFuncs.hasproperty             = pFuncs->hasproperty;
      NPNFuncs.hasmethod               = pFuncs->hasmethod;
      NPNFuncs.releasevariantvalue     = pFuncs->releasevariantvalue;
      NPNFuncs.setexception            = pFuncs->setexception;
    }
  }
  else
  {
    NPNFuncs.getstringidentifier     = NULL;
    NPNFuncs.getstringidentifiers    = NULL;
    NPNFuncs.getintidentifier        = NULL;
    NPNFuncs.identifierisstring      = NULL;
    NPNFuncs.utf8fromidentifier      = NULL;
    NPNFuncs.intfromidentifier       = NULL;
    NPNFuncs.createobject            = NULL;
    NPNFuncs.retainobject            = NULL;
    NPNFuncs.releaseobject           = NULL;
    NPNFuncs.invoke                  = NULL;
    NPNFuncs.invokeDefault           = NULL;
    NPNFuncs.evaluate                = NULL;
    NPNFuncs.getproperty             = NULL;
    NPNFuncs.setproperty             = NULL;
    NPNFuncs.removeproperty          = NULL;
    NPNFuncs.hasproperty             = NULL;
    NPNFuncs.hasmethod               = NULL;
    NPNFuncs.releasevariantvalue     = NULL;
    NPNFuncs.setexception            = NULL;
  }

/**
    if (pFuncs->size >= ((char *)&pFuncs->poppopupsenabledstate - (char *)pFuncs))
    {
      NPNFuncs.pushpopupsenabledstate = pFuncs->pushpopupsenabledstate;
      NPNFuncs.poppopupsenabledstate  = pFuncs->poppopupsenabledstate;
    }
    else
**/
    {
      NPNFuncs.pushpopupsenabledstate = NULL;
      NPNFuncs.poppopupsenabledstate  = NULL;
    }


#ifdef XP_UNIX
  /*
   * Set up the plugin function table that Netscape will use to
   * call us.  Netscape needs to know about our version and size
   * and have a UniversalProcPointer for every function we
   * implement.
   */
  pluginFuncs->version    = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
  pluginFuncs->size       = sizeof(NPPluginFuncs);
  pluginFuncs->newp       = (NPP_NewProcPtr)(NPP_New);
  pluginFuncs->destroy    = (NPP_DestroyProcPtr)(NPP_Destroy);
  pluginFuncs->setwindow  = (NPP_SetWindowProcPtr)(NPP_SetWindow);
  pluginFuncs->newstream  = (NPP_NewStreamProcPtr)(NPP_NewStream);
  pluginFuncs->destroystream = (NPP_DestroyStreamProcPtr)(NPP_DestroyStream);
  pluginFuncs->asfile     = (NPP_StreamAsFileProcPtr)(NPP_StreamAsFile);
  pluginFuncs->writeready = (NPP_WriteReadyProcPtr)(NPP_WriteReady);
  pluginFuncs->write      = (NPP_WriteProcPtr)(NPP_Write);
  pluginFuncs->print      = (NPP_PrintProcPtr)(NPP_Print);
  pluginFuncs->getvalue   = (NPP_GetValueProcPtr)(NPP_GetValue);
  pluginFuncs->setvalue   = (NPP_SetValueProcPtr)(NPP_SetValue);

  pluginFuncs->event      = NULL;

  if( minor >= NPVERS_HAS_NOTIFICATION )
    pluginFuncs->urlnotify  = (NPP_URLNotifyProcPtr)(NPP_URLNotify);

  pluginFuncs->javaClass  = NULL;

  NPP_Initialize();
#endif

  return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown()
{
#ifdef XP_UNIX
  NPP_Shutdown();
#endif
  return NPERR_NO_ERROR;
}

#if 0
#ifdef XP_MACOSX
int OSCALL
main(NPNetscapeFuncs* nCallbacks, NPPluginFuncs* pCallbacks, NPP_ShutdownProcPtr* unloadProcPtr)
{
  NPError rc = NP_Initialize(nCallbacks);
  if (rc != NPERR_NO_ERROR)
    return rc;

  rc = NP_GetEntryPoints(pCallbacks);
  if (rc != NPERR_NO_ERROR)
    return rc;

  *unloadProcPtr = (NPP_ShutdownProcPtr)(NPP_Shutdown);
  return NPERR_NO_ERROR;
}
#endif
#endif