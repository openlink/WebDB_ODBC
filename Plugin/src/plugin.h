/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include "npapi.h"
#include "npruntime.h"

#if defined(XP_WIN) && !defined(CHROME)
#include "win/WinMessageWindow.h"
#endif



// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
public:
  ScriptablePluginObjectBase(NPP npp)
    : mNpp(npp)
  {
  }

  virtual ~ScriptablePluginObjectBase()
  {
  }

  // Virtual NPObject hooks called through this base class. Override
  // as you see fit.
  virtual void Invalidate() {};
  virtual bool HasMethod(NPIdentifier name) 
                 { return false; };
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result) 
                 { return false; };
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result)
                 { return false; }
  virtual bool HasProperty(NPIdentifier name)
                 { return false; }
  virtual bool GetProperty(NPIdentifier name, NPVariant *result)
                 { return false; }
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value)
                 { return false; }
  virtual bool RemoveProperty(NPIdentifier name)
                 { return false; };
  virtual bool Enumerate(NPIdentifier **identifier, uint32_t *count)
                 { return false; };
  virtual bool Construct(const NPVariant *args, uint32_t argCount,
                         NPVariant *result)
                 { return false; };

public:
  static void _Deallocate(NPObject *npobj)
   {
     // Call the virtual destructor.
     delete (ScriptablePluginObjectBase *)npobj;
   };

  static void _Invalidate(NPObject *npobj)
   {
     ((ScriptablePluginObjectBase *)npobj)->Invalidate();
   };

  static bool _HasMethod(NPObject *npobj, NPIdentifier name)
   {
     return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
   };

  static bool _Invoke(NPObject *npobj, NPIdentifier name,
                      const NPVariant *args, uint32_t argCount,
                      NPVariant *result)
   {
     return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                       result);
   };

  static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
                             uint32_t argCount, NPVariant *result)
   {
     return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                              result);
   };

  static bool _HasProperty(NPObject * npobj, NPIdentifier name)
   {
     return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
   };

  static bool _GetProperty(NPObject *npobj, NPIdentifier name,
                           NPVariant *result)
   {
     return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
   };

  static bool _SetProperty(NPObject *npobj, NPIdentifier name,
                           const NPVariant *value)
   {
     return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
   };

  static bool _RemoveProperty(NPObject *npobj, NPIdentifier name)
   {
     return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
   };

  static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier,
                         uint32_t *count)
   {
     return ((ScriptablePluginObjectBase *)npobj)->Enumerate(identifier, count);
   };

  static bool _Construct(NPObject *npobj, const NPVariant *args,
                         uint32_t argCount, NPVariant *result)
   {
     return ((ScriptablePluginObjectBase *)npobj)->Construct(args, argCount,
                                                          result);
   };

protected:
  NPP mNpp;
};

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
static NPClass s##_class##_NPClass = {                                        \
  NP_CLASS_STRUCT_VERSION_CTOR,                                               \
  ctor,                                                                       \
  ScriptablePluginObjectBase::_Deallocate,                                    \
  ScriptablePluginObjectBase::_Invalidate,                                    \
  ScriptablePluginObjectBase::_HasMethod,                                     \
  ScriptablePluginObjectBase::_Invoke,                                        \
  ScriptablePluginObjectBase::_InvokeDefault,                                 \
  ScriptablePluginObjectBase::_HasProperty,                                   \
  ScriptablePluginObjectBase::_GetProperty,                                   \
  ScriptablePluginObjectBase::_SetProperty,                                   \
  ScriptablePluginObjectBase::_RemoveProperty,                                \
  ScriptablePluginObjectBase::_Enumerate,                                     \
  ScriptablePluginObjectBase::_Construct                                      \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass






class CPlugin
{
private:
  NPP m_pNPInstance;

  NPWindow * m_Window;
  
  NPStream * m_pNPStream;
  NPBool m_bInitialized;

  NPObject *m_pScriptableObject;

public:
  char m_String[128];
#if defined(XP_WIN) && !defined(CHROME)
  WinMessageWindow *m_win_message;
#endif
  int m_isSafari;

public:
  CPlugin(NPP pNPInstance, int _isSafari);
  ~CPlugin();

  NPBool init(NPWindow* pNPWindow);
  void shut();
  NPBool isInitialized();
  
  NPError SetWindow(NPWindow* aWindow);
  int16_t handleEvent(void* aEvent);

  NPObject *GetScriptableObject();
};



#endif // __PLUGIN_H__
