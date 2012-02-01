/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZCompat_npapi_h__
#define __ZCompat_npapi_h__
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

// Pull in ZStdInt.h so we see its int32 definition
#include "zoolib/ZStdInt.h"

#if defined(ZProjectHeader_npapi)

	#include ZProjectHeader_npapi

#elif defined(__APPLE__) && !ZMACRO_IOS && ! __LP64__

	#include <WebKit/npfunctions.h>
	#if !defined(XP_MACOSX)
		typedef NPError (*NP_InitializeFuncPtr)(NPNetscapeFuncs*);
		typedef NPError (*NP_GetEntryPointsFuncPtr)(NPPluginFuncs*);
	#endif

	#ifndef ZCONFIG_NPStringUpperCaseFieldNames
		#define ZCONFIG_NPStringUpperCaseFieldNames 1
	#endif

	#define NPEventType_GetFocusEvent getFocusEvent
	#define NPEventType_LoseFocusEvent loseFocusEvent
	#define NPEventType_AdjustCursorEvent adjustCursorEvent

#else

	#include "zoolib/netscape/ZNetscape_API.h"

#endif

#if defined(_NPFUNCTIONS_H_)
	#define ZCONFIG_NPAPI_WebKit_10_4 1
#elif defined(NPFUNCTIONS_H)
	#if defined(EXPORTED_CALLBACK)
		#define ZCONFIG_NPAPI_WebKit_Recent 1
	#else
		#define ZCONFIG_NPAPI_WebKit_10_5 1
	#endif
#elif defined(_NPUPP_H_)
	#if defined(_NPUPP_USE_UPP_)
		#define ZCONFIG_NPAPI_Mozilla_Old 1
	#else
		#define ZCONFIG_NPAPI_Mozilla_New 1
	#endif
#endif

// =================================================================================================
// Fixup the NPString field name change

#ifndef ZCONFIG_NPStringUpperCaseFieldNames
	#define ZCONFIG_NPStringUpperCaseFieldNames 0
#endif

#if ZCONFIG_NPStringUpperCaseFieldNames

	inline const NPUTF8* const& sNPStringCharsConst(const NPString& iNPString)
		{ return iNPString.UTF8Characters; }

	inline NPUTF8*& sNPStringChars(NPString& iNPString)
		{ return const_cast<NPUTF8*&>(iNPString.UTF8Characters); }

	inline uint32_t sNPStringLengthConst(const NPString& iNPString)
		{ return iNPString.UTF8Length; }

	inline uint32_t& sNPStringLength(NPString& iNPString)
		{ return iNPString.UTF8Length; }

#else

	inline const NPUTF8* const& sNPStringCharsConst(const NPString& iNPString)
		{ return iNPString.utf8characters; }

	inline NPUTF8*& sNPStringChars(NPString& iNPString)
		{ return const_cast<NPUTF8*&>(iNPString.utf8characters); }

	inline uint32_t sNPStringLengthConst(const NPString& iNPString)
		{ return iNPString.utf8length; }

	inline uint32_t& sNPStringLength(NPString& iNPString)
		{ return iNPString.utf8length; }

#endif

// =================================================================================================
// MARK: - npapi and npruntime header and macro fixups

#ifndef NPVERS_HAS_XPCONNECT_SCRIPTING
	#define NPVERS_HAS_XPCONNECT_SCRIPTING 13
#endif

#ifndef NPVERS_HAS_NPRUNTIME_SCRIPTING
	#define NPVERS_HAS_NPRUNTIME_SCRIPTING 14
#endif

#ifndef NPVERS_HAS_FORM_VALUES
	#define NPVERS_HAS_FORM_VALUES 15
#endif

#ifndef NPVERS_HAS_POPUPS_ENABLED_STATE
	#define NPVERS_HAS_POPUPS_ENABLED_STATE 16
#endif

#ifndef NPVERS_HAS_RESPONSE_HEADERS
	#define NPVERS_HAS_RESPONSE_HEADERS 17
#endif

#ifndef NPVERS_HAS_NPOBJECT_ENUM
	#define NPVERS_HAS_NPOBJECT_ENUM 18
#endif

#ifndef NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL
	#define NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL 19
#endif

#ifndef NPVERS_HAS_ALL_NETWORK_STREAMS
	#define NPVERS_HAS_ALL_NETWORK_STREAMS 20
#endif

#ifndef NPVERS_HAS_URL_AND_AUTH_INFO
	#define NPVERS_HAS_URL_AND_AUTH_INFO 21
#endif

#ifndef NPVERS_HAS_PRIVATE_MODE
	#define NPVERS_HAS_PRIVATE_MODE 22
#endif

#ifndef NPVERS_MACOSX_HAS_EVENT_MODELS
	#define NPVERS_MACOSX_HAS_EVENT_MODELS 23
#endif

#ifndef NPVERS_HAS_CANCEL_SRC_STREAM
	#define NPVERS_HAS_CANCEL_SRC_STREAM 24
#endif

#if NP_VERSION_MINOR < NPVERS_HAS_POPUPS_ENABLED_STATE
	typedef void (*NPN_PushPopupsEnabledStateProcPtr)(NPP instance, NPBool enabled);

	typedef void (*NPN_PopPopupsEnabledStateProcPtr)(NPP instance);
#endif

#if NP_VERSION_MINOR < NPVERS_HAS_NPOBJECT_ENUM
	typedef bool (*NPN_EnumerateProcPtr)
		(NPP npp, NPObject *npobj, NPIdentifier **identifier, uint32_t *count);
#endif

#if NP_VERSION_MINOR < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL
	typedef void (*NPN_PluginThreadAsyncCallProcPtr)
		(NPP npp, void (*func)(void *), void *userData);

	typedef bool (*NPN_ConstructProcPtr)
		(NPP npp, NPObject* obj, const NPVariant *args, uint32_t argCount, NPVariant *result);
#endif

#if NP_VERSION_MINOR < NPVERS_HAS_ALL_NETWORK_STREAMS
	typedef enum {
	  NPNURLVCookie = 501,
	  NPNURLVProxy
	} NPNURLVariable;

	typedef NPError (*NPN_GetValueForURLProcPtr)
		(NPP npp, NPNURLVariable variable, const char* url, char** value, uint32_t* len);

	typedef NPError (*NPN_SetValueForURLProcPtr)
		(NPP npp, NPNURLVariable variable, const char* url, const char* value, uint32_t len);
#endif

#if NP_VERSION_MINOR < NPVERS_HAS_URL_AND_AUTH_INFO
	typedef NPError (*NPN_GetAuthenticationInfoProcPtr)
		(NPP npp, const char* protocol, const char* host, int32_t port, const char* scheme,
		const char *realm, char** username, uint32_t* ulen, char** password, uint32_t* plen);
#endif

#if NP_VERSION_MINOR < NPVERS_MACOSX_HAS_EVENT_MODELS
	typedef uint32 (*NPN_ScheduleTimerProcPtr)
		(NPP npp, uint32 interval, NPBool repeat, void (*timerFunc)(NPP npp, uint32 timerID));

	typedef void (*NPN_UnscheduleTimerProcPtr)
		(NPP npp, uint32 timerID);

	typedef void * NPMenu;
	typedef NPError (*NPN_PopUpContextMenuProcPtr)(NPP instance, NPMenu* menu);
#endif

#if !defined(XP_MACOSX)
	typedef void (*BP_CreatePluginMIMETypesPreferencesFuncPtr)(void);
	typedef NPError (*MainFuncPtr)(NPNetscapeFuncs*, NPPluginFuncs*, NPP_ShutdownProcPtr*);
#endif

#if NP_CLASS_STRUCT_VERSION < 2
	typedef bool (*NPEnumerationFunctionPtr)
		(NPObject *npobj, NPIdentifier **value, uint32_t *count);
#endif

// =================================================================================================
// NPN and NPP variable fixups

// Actually this mozilla check is somewhat superfluous -- no XP_MACOSX in old headers.

#if defined(XP_MACOSX)

#if defined(ZCONFIG_NPAPI_WebKit_10_4) || defined(ZCONFIG_NPAPI_Mozilla_Old)

enum
	{
	NPPVpluginDrawingModel = 1000,
	NPNVpluginDrawingModel = 1000,
	NPPVpluginEventModel = 1001,
	NPNVsupportsCoreGraphicsBool = 2001,
	NPNVsupportsOpenGLBool = 2002,

	#ifndef NP_NO_QUICKDRAW
		NPNVsupportsQuickDrawBool = 2000,
	#endif
	NPNVSupportsWindowless = 17,
	NPNVprivateModeBool = 18,

	NPNVsupportsCarbonBool = 3000,
	NPNVsupportsCoreAnimationBool = 2003,
	NPNVsupportsCocoaBool = 3001
	};

typedef enum
	{
	#ifndef NP_NO_CARBON
		NPEventModelCarbon = 0,
	#endif
	NPEventModelCocoa = 1,
	} NPEventModel;

typedef enum
	{
	#ifndef NP_NO_QUICKDRAW
		NPDrawingModelQuickDraw = 0,
	#endif

	NPDrawingModelCoreGraphics = 1,
	NPDrawingModelOpenGL = 2
	} NPDrawingModel;

typedef struct NP_CGContext
	{
	CGContextRef context;
	WindowRef window;
	} NP_CGContext;

#endif // ZCONFIG_NPAPI_WebKit_10_4

#if defined(ZCONFIG_NPAPI_WebKit_10_5)

enum
	{
	NPPVpluginDrawingModel = 1000,
	NPPVpluginEventModel = 1001,
	NPNVSupportsWindowless = 17,
	NPNVprivateModeBool = 18,
	NPNVsupportsCarbonBool = 3000,
	NPNVsupportsCoreAnimationBool = 2003,
	NPNVsupportsCocoaBool = 3001,
	#ifndef NP_NO_CARBON
		NPEventModelCarbon = 0,
	#endif
	NPEventModelCocoa = 1
	};

	typedef enum
		{
		NPCocoaEventDrawRect = 1,
		NPCocoaEventMouseDown,
		NPCocoaEventMouseUp,
		NPCocoaEventMouseMoved,
		NPCocoaEventMouseEntered,
		NPCocoaEventMouseExited,
		NPCocoaEventMouseDragged,
		NPCocoaEventKeyDown,
		NPCocoaEventKeyUp,
		NPCocoaEventFlagsChanged,
		NPCocoaEventFocusChanged,
		NPCocoaEventWindowFocusChanged,
		NPCocoaEventScrollWheel,
		NPCocoaEventTextInput
		} NPCocoaEventType;

	typedef struct _NPNSString NPNSString;
	typedef struct _NPNSWindow NPNSWindow;
	typedef struct _NPNSMenu NPNSMenu;

	typedef struct _NPCocoaEvent
		{
		NPCocoaEventType type;
		uint32 version;

		union
			{
			struct
				{
				uint32 modifierFlags;
				double pluginX;
				double pluginY;
				int32 buttonNumber;
				int32 clickCount;
				double deltaX;
				double deltaY;
				double deltaZ;
				} mouse;
			struct
				{
				uint32 modifierFlags;
				NPNSString *characters;
				NPNSString *charactersIgnoringModifiers;
				NPBool isARepeat;
				uint16 keyCode;
				} key;
			struct
				{
				CGContextRef context;
				double x;
				double y;
				double width;
				double height;
				} draw;
			struct
				{
				NPBool hasFocus;
				} focus;
			struct
				{
				NPNSString *text;
				} text;
			} data;
		} NPCocoaEvent;

#endif // defined(ZCONFIG_NPAPI_WebKit_10_5)

#endif // defined(XP_MACOSX)

// =================================================================================================

#endif // __ZCompat_npapi_h__
