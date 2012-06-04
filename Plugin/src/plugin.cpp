/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 *
 * ***** END LICENSE BLOCK ***** */

//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif


#ifdef XP_UNIX
#include <string.h>
#endif

#include <stdio.h>


#include <sql.h>
#include <sqlext.h>
#include <wchar.h>

#include "plugin.h"
#include "npfunctions.h"

#include "utils.h"
#include "mthreads.h"
#include "Database.h"


#include "script.h"

#if 0
void log(const char *s)
{
  FILE *fp;
#ifdef XP_WIN
  fp = fopen("c:\\p.log","a");
#else
  fp = fopen("/Users/sergei/_pl.log","a");
#endif
  if (fp) 
    {
      fputs(s,fp);
      fclose(fp);
    }
}
#endif

//===========================================================================
static NPObject *
AllocateDatabaseObject(NPP npp, NPClass *aClass)
{
  return new DatabaseObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(DatabaseObject,  AllocateDatabaseObject);

//========================================================================



CPlugin::CPlugin(NPP pNPInstance, int _isSafari) :
  m_pNPInstance(pNPInstance),
  m_pNPStream(NULL),
  m_bInitialized(false),
#if defined(XP_WIN) && !defined(CHROME)
  m_win_message(NULL),
#endif
  m_isSafari(_isSafari),
  m_pScriptableObject(NULL)
{
  NPObject *_WindowObj = NULL;
  NPVariant _ret;
  NPVariant _vDatabase;

#if defined(XP_WIN) && !defined(CHROME)
  m_win_message = new WinMessageWindow();;
#endif

  VOID_TO_NPVARIANT(_ret);
  VOID_TO_NPVARIANT(_vDatabase);

  NPN_GetValue(m_pNPInstance, NPNVWindowNPObject, &_WindowObj);

  m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(DatabaseObject));
  if (m_pScriptableObject)
    NPN_RetainObject(m_pScriptableObject);
  OBJECT_TO_NPVARIANT(m_pScriptableObject, _vDatabase);

  NPIdentifier n_ODBCUtils = NPN_GetStringIdentifier("$ODBCUtils");

  NPN_SetProperty(m_pNPInstance, _WindowObj, n_ODBCUtils, &_vDatabase);
  NPN_ReleaseVariantValue(&_vDatabase);

  NPString str;
  str.utf8characters = (NPUTF8*)code;
  str.utf8length = (uint32_t)strlen(str.utf8characters);
  /*bool rc =*/ NPN_Evaluate(m_pNPInstance, _WindowObj, &str, &_ret);

  NPIdentifier n_Worker = NPN_GetStringIdentifier("WorkerUtils");
  NPN_SetProperty(m_pNPInstance, _WindowObj, n_Worker, &_ret);
//--  NPN_ReleaseVariantValue(&_ret);
  if (_WindowObj)
    NPN_ReleaseObject(_WindowObj);
}


CPlugin::~CPlugin()
{
  if (m_pScriptableObject)
    NPN_ReleaseObject(m_pScriptableObject);
#if defined(XP_WIN) && !defined(CHROME)
  if (m_win_message)
    delete(m_win_message);
#endif

}


NPBool CPlugin::init(NPWindow* pNPWindow)
{
  m_Window = pNPWindow;
  m_bInitialized = true;
  return true;
}

void CPlugin::shut()
{
  m_bInitialized = false;
}

NPBool CPlugin::isInitialized()
{
  return m_bInitialized;
}

NPError CPlugin::SetWindow(NPWindow* aWindow)
{
  m_Window = aWindow;
  return NPERR_NO_ERROR;
}

int16_t CPlugin::handleEvent(void* aEvent)
{
#ifdef XP_WIN0
  NPEvent * event = (NPEvent *)aEvent;
	
  switch (event->event) {
    case WM_PAINT: 
    {
      if(!m_Window)
        break;

      // get the dirty rectangle to update or repaint the whole window
      RECT * drc = (RECT *)event->lParam;
      if(drc)
        FillRect((HDC)event->wParam, drc, (HBRUSH)(COLOR_ACTIVECAPTION+1));
      else {
        RECT rc;
        rc.bottom = m_Window->y + m_Window->height;
        rc.left   = m_Window->x;
        rc.right  = m_Window->x + m_Window->width;
        rc.top    = m_Window->y;
        FillRect((HDC)event->wParam, &rc, (HBRUSH)(COLOR_ACTIVECAPTION+1));
      }
      break;
    }
    case WM_KEYDOWN:
    {
      Beep(1000,100);
      break;
    }
    default:
      return 0;
  }
#endif
  return 1;
}



NPObject *
CPlugin::GetScriptableObject()
{
  if (!m_pScriptableObject)
    m_pScriptableObject = NPN_CreateObject(m_pNPInstance, GET_NPOBJECT_CLASS(DatabaseObject));

  if (m_pScriptableObject)
    NPN_RetainObject(m_pScriptableObject);

  return m_pScriptableObject;
}

