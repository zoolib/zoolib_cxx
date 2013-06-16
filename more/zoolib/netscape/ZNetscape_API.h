/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZNetscape_API_h__
#define __ZNetscape_API_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZMacCFM.h"
#include "zoolib/ZStdInt.h"

// =================================================================================================
// MARK: - Standard integer types.

// Much as it pains me, the standard npapi.h files do introduce global definitions
// for int16, uint16, int32 and uint32, so we have to do the same.

#ifndef _INT16
	#define _INT16
	using ZStdInt::int16;
#endif

#ifndef _UINT16
	#define _UINT16
	using ZStdInt::uint16;
#endif

#ifndef _INT32
	#define _INT32
	using ZStdInt::int32;
#endif

#ifndef _UINT32
	#define _UINT32
	using ZStdInt::uint32;
#endif

// =================================================================================================
// MARK: - NPAPI platform designation

#if ZCONFIG_SPI_Enabled(Win)

	#define XP_WIN 1

#elif ZCONFIG_SPI_Enabled(Carbon64)

	#if __MACH__
		#define XP_MACOSX 1
	#else
		#define XP_MAC 1
	#endif

#elif ZCONFIG_SPI_Enabled(X11)

	#define XP_UNIX 1

#endif

// =================================================================================================
// MARK: - NP_NO_QUICKDRAW and NP_NO_CARBON

#if defined(XP_MACOSX) && __LP64__
	#define NP_NO_QUICKDRAW 1
	#define NP_NO_CARBON 1
#endif

// =================================================================================================
// MARK: - Mac headers

#if defined(XP_MACOSX)
	#include <ApplicationServices/ApplicationServices.h>
	#include <OpenGL/OpenGL.h>
#endif

#if defined(XP_MACOSX) || defined(XP_MAC)
	#include ZMACINCLUDE2(Carbon,Carbon.h)
#endif

// =================================================================================================
// MARK: - Windows headers

#if defined(XP_WIN)
	#include "zoolib/ZCompat_Win.h"
#endif

// =================================================================================================
// MARK: - UNIX headers

#if defined(XP_UNIX)
	#include "zoolib/ZCompat_XLib.h"
#endif

// =================================================================================================
// MARK: - NP_VERSION

#define NP_VERSION_MAJOR 0
#define NP_VERSION_MINOR 24

// =================================================================================================
#pragma mark -
#pragma mark "* Mac-specific, set structure alignment to be 68K"

#if ! __LP64__
	#if defined(XP_MAC) || defined(XP_MACOSX)
		#pragma options align=mac68k
	#endif
#endif

// =================================================================================================
// MARK: - Basic data typedefs

typedef ZStdInt::uint8 NPBool;
typedef ZStdInt::int16 NPError;
typedef ZStdInt::int16 NPReason;
typedef char* NPMIMEType;

// =================================================================================================
// MARK: - NPP

typedef struct _NPP
	{
	void* pdata;
	void* ndata;
	} NPP_t;

typedef NPP_t* NPP;

// =================================================================================================
// MARK: - NPStream

typedef struct _NPStream
	{
	void* pdata;
	void* ndata;
	const char* url;
	ZStdInt::uint32 end;
	uint32 lastmodified;
	void* notifyData;
	const char* headers;
	} NPStream;

// =================================================================================================
// MARK: - NPByteRange

typedef struct _NPByteRange
	{
	int32 offset;
	uint32 length;
	struct _NPByteRange* next;
	} NPByteRange;

// =================================================================================================
// MARK: - NPSavedData

typedef struct _NPSavedData
	{
	int32 len;
	void* buf;
	} NPSavedData;

// =================================================================================================
// MARK: - NPRect

typedef struct _NPRect
	{
	uint16 top;
	uint16 left;
	uint16 bottom;
	uint16 right;
	} NPRect;

// =================================================================================================
// MARK: - Unix-specific

#if defined(XP_UNIX)
enum
	{
	NP_SETWINDOW = 1,
	NP_PRINT
	};

typedef struct
	{
	int32 type;
	} NPAnyCallbackStruct;

typedef struct
	{
	int32 type;
	Display* display;
	Visual* visual;
	Colormap colormap;
	unsigned int depth;
	} NPSetWindowCallbackStruct;

typedef struct
	{
	int32 type;
	FILE* fp;
	} NPPrintCallbackStruct;
#endif // XP_UNIX

// =================================================================================================
// MARK: - NPPVariable and NPNVariable

#define NP_ABI_MASK 0

typedef enum
	{
	NPPVpluginNameString = 1,
	NPPVpluginDescriptionString = 2,
	NPPVpluginWindowBool = 3,
	NPPVpluginTransparentBool = 4,
	NPPVjavaClass = 5,
	NPPVpluginWindowSize = 6,
	NPPVpluginTimerInterval = 7,
	NPPVpluginScriptableInstance = (10 | NP_ABI_MASK),
	NPPVpluginScriptableIID = 11,
	NPPVjavascriptPushCallerBool = 12,
	NPPVpluginKeepLibraryInMemory = 13,
	NPPVpluginNeedsXEmbed = 14,
	NPPVpluginScriptableNPObject = 15,
	NPPVformValue = 16,
	NPPVpluginUrlRequestsDisplayedBool = 17,
	NPPVpluginWantsAllNetworkStreams = 18

	#if defined(XP_MACOSX)
		, NPPVpluginDrawingModel = 1000
		, NPPVpluginEventModel = 1001
		, NPPVpluginTextInputFuncs = 1002
	#endif
	} NPPVariable;

typedef enum
	{
	NPNVxDisplay = 1,
	NPNVxtAppContext = 2,
	NPNVnetscapeWindow = 3,
	NPNVjavascriptEnabledBool = 4,
	NPNVasdEnabledBool = 5,
	NPNVisOfflineBool = 6,

	NPNVserviceManager = (10 | NP_ABI_MASK),
	NPNVDOMElement = (11 | NP_ABI_MASK),
	NPNVDOMWindow = (12 | NP_ABI_MASK),
	NPNVToolkit = (13 | NP_ABI_MASK),
	NPNVSupportsXEmbedBool = 14,

	NPNVWindowNPObject = 15,
	NPNVPluginElementNPObject = 16,
	NPNVSupportsWindowless = 17,
	NPNVprivateModeBool = 18

	#if defined(XP_MACOSX)
		, NPNVpluginDrawingModel = 1000

		#ifndef NP_NO_QUICKDRAW
			, NPNVsupportsQuickDrawBool = 2000
		#endif

		, NPNVsupportsCoreGraphicsBool = 2001
		, NPNVsupportsOpenGLBool = 2002
		, NPNVsupportsCoreAnimationBool = 2003

		#ifndef NP_NO_CARBON
			, NPNVsupportsCarbonBool = 3000
		#endif

		, NPNVsupportsCocoaBool = 3001
		, NPNVbrowserTextInputFuncs = 1002
	#endif
	} NPNVariable;

typedef enum {
  NPNURLVCookie = 501,
  NPNURLVProxy
} NPNURLVariable;

// =================================================================================================
// MARK: - NPWindowType and NPWindow

typedef enum
	{
	NPWindowTypeWindow = 1,
	NPWindowTypeDrawable
	} NPWindowType;

// =================================================================================================
// MARK: - NPDrawingModel

#if defined(XP_MACOSX)
	typedef enum
		{
		#ifndef NP_NO_QUICKDRAW
			NPDrawingModelQuickDraw = 0,
		#endif
		NPDrawingModelCoreGraphics = 1,
		NPDrawingModelOpenGL = 2
		} NPDrawingModel;
#endif

// =================================================================================================
// MARK: - NPEventModel, NPCocoaEventType, NPCocoaEvent

#if defined(XP_MACOSX)

	typedef enum
		{
		#ifndef NP_NO_CARBON
			NPEventModelCarbon = 0,
		#endif
		NPEventModelCocoa = 1,
		} NPEventModel;

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
#endif

// =================================================================================================
// MARK: - NPWindow

typedef struct _NPWindow
	{
	void* window;
	int32 x;
	int32 y;
	uint32 width;
	uint32 height;
	NPRect clipRect;
	#if defined(XP_UNIX)
		void* ws_info;
	#endif
	NPWindowType type;
	} NPWindow;

// =================================================================================================
// MARK: - NPFullPrint, NPEmbedPrint, NPPrint

typedef struct _NPFullPrint
	{
	NPBool pluginPrinted;
	NPBool printOne;
	void* platformPrint;
	} NPFullPrint;

typedef struct _NPEmbedPrint
	{
	NPWindow window;
	void* platformPrint;
	} NPEmbedPrint;

typedef struct _NPPrint
	{
	uint16 mode;
	union
		{
		NPFullPrint fullPrint;
		NPEmbedPrint embedPrint;
		} print;
	} NPPrint;

// =================================================================================================
// MARK: - NPMenu

#if defined(XP_MACOSX)
	typedef NPNSMenu NPMenu;
#else
	typedef void* NPMenu;
#endif

// =================================================================================================
// MARK: - NPCoordinateSpace

typedef enum {
  NPCoordinateSpacePlugin = 1,
  NPCoordinateSpaceWindow,
  NPCoordinateSpaceFlippedWindow,
  NPCoordinateSpaceScreen,
  NPCoordinateSpaceFlippedScreen
} NPCoordinateSpace;

// =================================================================================================
// MARK: - NPEvent

#if defined(XP_MAC) || defined(XP_MACOSX)

	#if !defined(NP_NO_CARBON)
		typedef EventRecord NPEvent;
	#endif

#elif defined(XP_WIN)

	typedef struct _NPEvent
		{
		uint16 event;
		uintptr_t wParam;
		uintptr_t lParam;
		} NPEvent;

#elif defined(XP_UNIX)

	typedef XEvent NPEvent;

#else

	typedef void* NPEvent;

#endif

// =================================================================================================
// MARK: - NPRegion

#if defined(XP_MAC)

	typedef RgnHandle NPRegion;

#elif defined(XP_MACOSX)

	typedef void* NPRegion;

	#if !defined(NP_NO_QUICKDRAW)
		typedef RgnHandle NPQDRegion;
	#endif

	typedef CGPathRef NPCGRegion;

#elif defined(XP_WIN)

	typedef HRGN NPRegion;

#elif defined(XP_UNIX)

	typedef Region NPRegion;

#else

	typedef void *NPRegion;

#endif

// =================================================================================================
// MARK: - Mac-specific NP_Port and events

#if defined(XP_MACOSX)

	typedef struct NP_CGContext
		{
		CGContextRef context;
		#if defined(NP_NO_CARBON)
			NPNSWindow* window;
		#else
			void* window;
		#endif
		} NP_CGContext;


	typedef struct NP_GLContext
		{
		CGLContextObj context;
		#if defined(NP_NO_CARBON)
			NPNSWindow* window;
		#else
			void* window;
		#endif
		} NP_GLContext;

#endif


#if defined(XP_MAC) || defined(XP_MACOSX)

	#ifndef NP_NO_QUICKDRAW
		typedef struct NP_Port
			{
			CGrafPtr port;
			int32 portx;
			int32 porty;
			} NP_Port;
	#endif

	enum NPEventType
		{
		NPEventType_GetFocusEvent = (osEvt + 16),
		NPEventType_LoseFocusEvent,
		NPEventType_AdjustCursorEvent,
		NPEventType_MenuCommandEvent,
		NPEventType_ClippingChangedEvent,
		NPEventType_ScrollingBeginsEvent = 1000,
		NPEventType_ScrollingEndsEvent
		};

#endif

// =================================================================================================
// MARK: - Values passed to NPP_New

#define NP_EMBED 1
#define NP_FULL 2

// =================================================================================================
// MARK: - Values passed to NPP_NewStream

#define NP_NORMAL 1
#define NP_SEEK 2
#define NP_ASFILE 3
#define NP_ASFILEONLY 4

#define NP_MAXREADY	(((unsigned)(~0)<<1)>>1)

// =================================================================================================
// MARK: - NPError values

#define NPERR_BASE 0
#define NPERR_NO_ERROR (NPERR_BASE + 0)
#define NPERR_GENERIC_ERROR (NPERR_BASE + 1)
#define NPERR_INVALID_INSTANCE_ERROR (NPERR_BASE + 2)
#define NPERR_INVALID_FUNCTABLE_ERROR (NPERR_BASE + 3)
#define NPERR_MODULE_LOAD_FAILED_ERROR (NPERR_BASE + 4)
#define NPERR_OUT_OF_MEMORY_ERROR (NPERR_BASE + 5)
#define NPERR_INVALID_PLUGIN_ERROR (NPERR_BASE + 6)
#define NPERR_INVALID_PLUGIN_DIR_ERROR (NPERR_BASE + 7)
#define NPERR_INCOMPATIBLE_VERSION_ERROR (NPERR_BASE + 8)
#define NPERR_INVALID_PARAM (NPERR_BASE + 9)
#define NPERR_INVALID_URL (NPERR_BASE + 10)
#define NPERR_FILE_NOT_FOUND (NPERR_BASE + 11)
#define NPERR_NO_DATA (NPERR_BASE + 12)
#define NPERR_STREAM_NOT_SEEKABLE (NPERR_BASE + 13)

// =================================================================================================
// MARK: - NPReason values

#define NPRES_BASE 0
#define NPRES_DONE (NPRES_BASE + 0)
#define NPRES_NETWORK_ERR (NPRES_BASE + 1)
#define NPRES_USER_BREAK (NPRES_BASE + 2)

// =================================================================================================
// MARK: - NPVers values

#define NPVERS_HAS_STREAMOUTPUT 8
#define NPVERS_HAS_NOTIFICATION 9
#define NPVERS_HAS_LIVECONNECT 9
#define NPVERS_WIN16_HAS_LIVECONNECT 9
#define NPVERS_68K_HAS_LIVECONNECT 11
#define NPVERS_HAS_WINDOWLESS 11
#define NPVERS_HAS_XPCONNECT_SCRIPTING 13
#define NPVERS_HAS_NPRUNTIME_SCRIPTING 14
#define NPVERS_HAS_FORM_VALUES 15
#define NPVERS_HAS_POPUPS_ENABLED_STATE 16
#define NPVERS_HAS_RESPONSE_HEADERS 17
#define NPVERS_HAS_NPOBJECT_ENUM 18
#define NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL 19
#define NPVERS_MACOSX_HAS_EVENT_MODELS 20

// =================================================================================================
// MARK: - NPObject, NPClass, NPVariant etc.

typedef struct NPObject NPObject;
typedef struct NPClass NPClass;

typedef char NPUTF8;
typedef struct _NPString
	{
	const NPUTF8* utf8characters;
	uint32_t utf8length;
	} NPString;

typedef enum
	{
	NPVariantType_Void,
	NPVariantType_Null,
	NPVariantType_Bool,
	NPVariantType_Int32,
	NPVariantType_Double,
	NPVariantType_String,
	NPVariantType_Object
	} NPVariantType;

typedef struct _NPVariant
	{
	NPVariantType type;
	union
		{
		bool boolValue;
		int32_t intValue;
		double doubleValue;
		NPString stringValue;
		NPObject *objectValue;
		} value;
	} NPVariant;

typedef void *NPIdentifier;

typedef NPObject *(*NPAllocateFunctionPtr)(NPP npp, NPClass *aClass);
typedef void (*NPDeallocateFunctionPtr)(NPObject *obj);
typedef void (*NPInvalidateFunctionPtr)(NPObject *obj);
typedef bool (*NPHasMethodFunctionPtr)(NPObject *obj, NPIdentifier name);
typedef bool (*NPInvokeFunctionPtr)(
	NPObject *obj, NPIdentifier name, const NPVariant *args, unsigned argCount, NPVariant *result);
typedef bool (*NPInvokeDefaultFunctionPtr)(
	NPObject *npobj, const NPVariant *args, unsigned argCount, NPVariant *result);
typedef bool (*NPHasPropertyFunctionPtr)(NPObject *obj, NPIdentifier name);
typedef bool (*NPGetPropertyFunctionPtr)(NPObject *obj, NPIdentifier name, NPVariant *result);
typedef bool (*NPSetPropertyFunctionPtr)(NPObject *obj, NPIdentifier name, const NPVariant *value);
typedef bool (*NPRemovePropertyFunctionPtr)(NPObject *npobj, NPIdentifier name);
typedef bool (*NPEnumerationFunctionPtr)(NPObject *npobj, NPIdentifier **value, uint32_t *count);

struct NPClass
	{
	uint32_t structVersion;
	NPAllocateFunctionPtr allocate;
	NPDeallocateFunctionPtr deallocate;
	NPInvalidateFunctionPtr invalidate;
	NPHasMethodFunctionPtr hasMethod;
	NPInvokeFunctionPtr invoke;
	NPInvokeDefaultFunctionPtr invokeDefault;
	NPHasPropertyFunctionPtr hasProperty;
	NPGetPropertyFunctionPtr getProperty;
	NPSetPropertyFunctionPtr setProperty;
	NPRemovePropertyFunctionPtr removeProperty;
	NPEnumerationFunctionPtr enumerate;
	};

#define NP_CLASS_STRUCT_VERSION 2

struct NPObject
	{
	NPClass *_class;
	uint32_t referenceCount;
	};

// =================================================================================================
// MARK: - NPFunctions

typedef void* JRIGlobalRef;
typedef struct __JRIEnv JRIEnv;
typedef void* jref;

ZMacCFM_DefineProc3(NPError, NPN_GetURL, NPP, const char*, const char*);

ZMacCFM_DefineProc6(
	NPError, NPN_PostURL, NPP, const char*, const char*, uint32, const char*, NPBool);

ZMacCFM_DefineProc2(NPError, NPN_RequestRead, NPStream*, NPByteRange*);
ZMacCFM_DefineProc4(NPError, NPN_NewStream, NPP, NPMIMEType, const char*, NPStream**);
ZMacCFM_DefineProc4(int32, NPN_Write, NPP, NPStream*, int32, void*);
ZMacCFM_DefineProc3(NPError, NPN_DestroyStream, NPP, NPStream*, NPReason);
ZMacCFM_DefineProc2_Void(NPN_Status, NPP, const char*);
ZMacCFM_DefineProc1(const char*, NPN_UserAgent, NPP);
ZMacCFM_DefineProc1(void*, NPN_MemAlloc, uint32);
ZMacCFM_DefineProc1_Void(NPN_MemFree, void*);
ZMacCFM_DefineProc1(uint32, NPN_MemFlush, uint32);
ZMacCFM_DefineProc1_Void(NPN_ReloadPlugins, NPBool);
ZMacCFM_DefineProc0(JRIEnv*, NPN_GetJavaEnv);
ZMacCFM_DefineProc1(jref, NPN_GetJavaPeer, NPP);
ZMacCFM_DefineProc4(NPError, NPN_GetURLNotify, NPP, const char*, const char*, void*);

ZMacCFM_DefineProc7(
	NPError, NPN_PostURLNotify, NPP, const char*, const char*, uint32, const char*, NPBool, void*);

ZMacCFM_DefineProc3(NPError, NPN_GetValue, NPP, NPNVariable, void*);
ZMacCFM_DefineProc3(NPError, NPN_SetValue, NPP, NPPVariable, void*);
ZMacCFM_DefineProc2_Void(NPN_InvalidateRect, NPP, NPRect*);
ZMacCFM_DefineProc2_Void(NPN_InvalidateRegion, NPP, NPRegion);
ZMacCFM_DefineProc1_Void(NPN_ForceRedraw, NPP);
ZMacCFM_DefineProc1(NPIdentifier, NPN_GetStringIdentifier, const NPUTF8*);
ZMacCFM_DefineProc3_Void(NPN_GetStringIdentifiers, const NPUTF8**, int32_t, NPIdentifier*);
ZMacCFM_DefineProc1(NPIdentifier, NPN_GetIntIdentifier, int32_t);
ZMacCFM_DefineProc1(bool, NPN_IdentifierIsString, NPIdentifier);
ZMacCFM_DefineProc1(NPUTF8*, NPN_UTF8FromIdentifier, NPIdentifier);
ZMacCFM_DefineProc1(int32_t, NPN_IntFromIdentifier, NPIdentifier);
ZMacCFM_DefineProc2(NPObject*, NPN_CreateObject, NPP, NPClass*);
ZMacCFM_DefineProc1(NPObject*, NPN_RetainObject, NPObject*);
ZMacCFM_DefineProc1_Void(NPN_ReleaseObject, NPObject*);

ZMacCFM_DefineProc6(
	bool, NPN_Invoke, NPP, NPObject*, NPIdentifier, const NPVariant*, unsigned, NPVariant*);

ZMacCFM_DefineProc5(
	bool, NPN_InvokeDefault, NPP, NPObject*, const NPVariant*, unsigned, NPVariant*);

ZMacCFM_DefineProc4(bool, NPN_Evaluate, NPP, NPObject*, NPString*, NPVariant*);
ZMacCFM_DefineProc4(bool, NPN_GetProperty, NPP, NPObject*, NPIdentifier, NPVariant*);
ZMacCFM_DefineProc4(bool, NPN_SetProperty, NPP, NPObject*, NPIdentifier, const NPVariant*);
ZMacCFM_DefineProc3(bool, NPN_RemoveProperty, NPP, NPObject*, NPIdentifier);
ZMacCFM_DefineProc3(bool, NPN_HasProperty, NPP, NPObject*, NPIdentifier);
ZMacCFM_DefineProc3(bool, NPN_HasMethod, NPP, NPObject*, NPIdentifier);
ZMacCFM_DefineProc1_Void(NPN_ReleaseVariantValue, NPVariant*);
ZMacCFM_DefineProc2_Void(NPN_SetException, NPObject*, const NPUTF8*);
ZMacCFM_DefineProc2_Void(NPN_PushPopupsEnabledState, NPP, NPBool);
ZMacCFM_DefineProc1_Void(NPN_PopPopupsEnabledState, NPP);
ZMacCFM_DefineProc4(bool, NPN_Enumerate, NPP, NPObject*, NPIdentifier**, uint32_t*);
ZMacCFM_DefineProc3_Void(NPN_PluginThreadAsyncCall, NPP, void (*)(void*), void*);
ZMacCFM_DefineProc5(bool, NPN_Construct, NPP, NPObject*, const NPVariant*, uint32_t, NPVariant*);

typedef NPError (*NPN_GetValueForURLProcPtr)(
	NPP npp, NPNURLVariable variable, const char* url, char** value, uint32_t* len);

typedef NPError (*NPN_SetValueForURLProcPtr)(
	NPP npp, NPNURLVariable variable, const char* url, const char* value, uint32_t len);

typedef NPError (*NPN_GetAuthenticationInfoProcPtr)(
	NPP npp, const char* protocol, const char* host, int32_t port, const char* scheme,
	const char *realm, char** username, uint32_t* ulen, char** password, uint32_t* plen);


ZMacCFM_DefineProc4(
	uint32, NPN_ScheduleTimer, NPP, uint32, NPBool, void (*)(NPP npp, uint32 timerID));

ZMacCFM_DefineProc2_Void(NPN_UnscheduleTimer, NPP, uint32);
ZMacCFM_DefineProc2(NPError, NPN_PopUpContextMenu, NPP, NPMenu*);

typedef NPBool (*NPN_ConvertPointProcPtr)(
	NPP npp, double sourceX, double sourceY, NPCoordinateSpace sourceSpace,
	double *destX, double *destY, NPCoordinateSpace destSpace);

ZMacCFM_DefineProc7(
	NPError, NPP_New, NPMIMEType, NPP, uint16, int16, char**, char**, NPSavedData*);

ZMacCFM_DefineProc2(NPError, NPP_Destroy, NPP, NPSavedData**);
ZMacCFM_DefineProc2(NPError, NPP_SetWindow, NPP, NPWindow*);
ZMacCFM_DefineProc5(NPError, NPP_NewStream, NPP, NPMIMEType, NPStream*, NPBool, uint16*);
ZMacCFM_DefineProc3(NPError, NPP_DestroyStream, NPP, NPStream*, NPReason);
ZMacCFM_DefineProc3_Void(NPP_StreamAsFile, NPP, NPStream*, const char*);
ZMacCFM_DefineProc2(int32, NPP_WriteReady, NPP, NPStream*);
ZMacCFM_DefineProc5(int32, NPP_Write, NPP, NPStream*, int32_t, int32_t, void*);
ZMacCFM_DefineProc2_Void(NPP_Print, NPP, NPPrint*);
ZMacCFM_DefineProc2(int16, NPP_HandleEvent, NPP, void*);
ZMacCFM_DefineProc4_Void(NPP_URLNotify, NPP, const char*, NPReason, void*);
ZMacCFM_DefineProc3(NPError, NPP_GetValue, NPP, NPPVariable, void*);
ZMacCFM_DefineProc3(NPError, NPP_SetValue, NPP, NPNVariable, void*);
ZMacCFM_DefineProc0(void*, NPP_GetJavaClassProcPtr);

// =================================================================================================
// MARK: - NPNetscapeFuncs

typedef struct _NPNetscapeFuncs
	{
	uint16 size;
	uint16 version;

	NPN_GetURLProcPtr geturl;
	NPN_PostURLProcPtr posturl;
	NPN_RequestReadProcPtr requestread;
	NPN_NewStreamProcPtr newstream;
	NPN_WriteProcPtr write;
	NPN_DestroyStreamProcPtr destroystream;
	NPN_StatusProcPtr status;
	NPN_UserAgentProcPtr uagent;
	NPN_MemAllocProcPtr memalloc;
	NPN_MemFreeProcPtr memfree;
	NPN_MemFlushProcPtr memflush;
	NPN_ReloadPluginsProcPtr reloadplugins;
	NPN_GetJavaEnvProcPtr getJavaEnv;
	NPN_GetJavaPeerProcPtr getJavaPeer;
	NPN_GetURLNotifyProcPtr geturlnotify;
	NPN_PostURLNotifyProcPtr posturlnotify;
	NPN_GetValueProcPtr getvalue;
	NPN_SetValueProcPtr setvalue;
	NPN_InvalidateRectProcPtr invalidaterect;
	NPN_InvalidateRegionProcPtr invalidateregion;
	NPN_ForceRedrawProcPtr forceredraw;

	NPN_GetStringIdentifierProcPtr getstringidentifier;
	NPN_GetStringIdentifiersProcPtr getstringidentifiers;
	NPN_GetIntIdentifierProcPtr getintidentifier;
	NPN_IdentifierIsStringProcPtr identifierisstring;
	NPN_UTF8FromIdentifierProcPtr utf8fromidentifier;
	NPN_IntFromIdentifierProcPtr intfromidentifier;
	NPN_CreateObjectProcPtr createobject;
	NPN_RetainObjectProcPtr retainobject;
	NPN_ReleaseObjectProcPtr releaseobject;
	NPN_InvokeProcPtr invoke;
	NPN_InvokeDefaultProcPtr invokeDefault;
	NPN_EvaluateProcPtr evaluate;
	NPN_GetPropertyProcPtr getproperty;
	NPN_SetPropertyProcPtr setproperty;
	NPN_RemovePropertyProcPtr removeproperty;
	NPN_HasPropertyProcPtr hasproperty;
	NPN_HasMethodProcPtr hasmethod;
	NPN_ReleaseVariantValueProcPtr releasevariantvalue;
	NPN_SetExceptionProcPtr setexception;
	NPN_PushPopupsEnabledStateProcPtr pushpopupsenabledstate;
	NPN_PopPopupsEnabledStateProcPtr poppopupsenabledstate;
	NPN_EnumerateProcPtr enumerate;
	NPN_PluginThreadAsyncCallProcPtr pluginthreadasynccall;
	NPN_ConstructProcPtr construct;
    NPN_GetValueForURLProcPtr getvalueforurl;
    NPN_SetValueForURLProcPtr setvalueforurl;
    NPN_GetAuthenticationInfoProcPtr getauthenticationinfo;
    NPN_ScheduleTimerProcPtr scheduletimer;
    NPN_UnscheduleTimerProcPtr unscheduletimer;
    NPN_PopUpContextMenuProcPtr popupcontextmenu;
    NPN_ConvertPointProcPtr convertpoint;
	} NPNetscapeFuncs;

// =================================================================================================
// MARK: - NPPluginFuncs

typedef struct _NPPluginFuncs
	{
	uint16 size;
	uint16 version;
	NPP_NewProcPtr newp;
	NPP_DestroyProcPtr destroy;
	NPP_SetWindowProcPtr setwindow;
	NPP_NewStreamProcPtr newstream;
	NPP_DestroyStreamProcPtr destroystream;
	NPP_StreamAsFileProcPtr asfile;
	NPP_WriteReadyProcPtr writeready;
	NPP_WriteProcPtr write;
	NPP_PrintProcPtr print;
	NPP_HandleEventProcPtr event;
	NPP_URLNotifyProcPtr urlnotify;
	JRIGlobalRef javaClass;
	NPP_GetValueProcPtr getvalue;
	NPP_SetValueProcPtr setvalue;
	} NPPluginFuncs;

// =================================================================================================
// MARK: - Mac-specific, reset structure alignment

#if ! __LP64__
	#if defined(XP_MAC) || defined(XP_MACOSX)
		#pragma options align=reset
	#endif
#endif

#if defined(XP_WIN)
	#define ZNetscape_API_EXPORTED_CALLBACK(_type, _name) _type (__stdcall * _name)
#else
	#define ZNetscape_API_EXPORTED_CALLBACK(_type, _name) _type (* _name)
#endif


typedef ZNetscape_API_EXPORTED_CALLBACK(NPError, NP_GetEntryPointsFuncPtr)(NPPluginFuncs*);
typedef ZNetscape_API_EXPORTED_CALLBACK(void, NPP_ShutdownProcPtr)(void);

#if defined(XP_MACOSX)
	typedef void (*BP_CreatePluginMIMETypesPreferencesFuncPtr)(void);
	typedef NPError (*MainFuncPtr)(NPNetscapeFuncs*, NPPluginFuncs*, NPP_ShutdownProcPtr*);
#endif


#if defined(XP_UNIX)
	typedef ZNetscape_API_EXPORTED_CALLBACK(NPError, NP_InitializeFuncPtr)(
		NPNetscapeFuncs*, NPPluginFuncs*);
	typedef ZNetscape_API_EXPORTED_CALLBACK(char*, NP_GetMIMEDescriptionFuncPtr)(void);
#else
	typedef ZNetscape_API_EXPORTED_CALLBACK(NPError, NP_InitializeFuncPtr)(NPNetscapeFuncs*);
#endif

#endif // __ZNetscape_API_h__
