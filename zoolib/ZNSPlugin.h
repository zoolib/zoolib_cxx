#ifndef __ZNSPlugin__
#define __ZNSPlugin__ 1
#include "zconfig.h"

#include "ZTypes.h"

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
#	include <Events.h>
#	include <QuickDraw.h>
#endif

#if ZCONFIG(OS, Win32)
#	include "ZWinHeader.h"
#endif

#if ZCONFIG(OS, POSIX)
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#endif

class ZNSPluginMeister;
class ZNSPlugin
	{
public:
	static ZNSPluginMeister* sMeister;
	
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	#	pragma options align=mac68k
	#endif
	
	static const unsigned int kVersionMajor = 0;
	static const unsigned int kVersionMinor = 11;
	
	typedef uint8 NPBool;
	
	// ==================================================
	typedef int16 EError;
	enum
		{
		eError_None = 0,
		eError_Generic = 1,
		eError_INVALID_INSTANCE_ERROR = 2,
		eError_InvalidFunctionTable = 3,
		eError_MODULE_LOAD_FAILED_ERROR = 4,
		eError_OUT_OF_MEMORY_ERROR = 5,
		eError_INVALID_PLUGIN_ERROR = 6,
		eError_INVALID_PLUGIN_DIR_ERROR = 7,
		eError_IncompatibleVersion = 8,
		eError_INVALID_PARAM = 9,
		eError_INVALID_URL = 10,
		eError_FILE_NOT_FOUND = 11,
		eError_NO_DATA = 12,
		eError_STREAM_NOT_SEEKABLE = 13
		};
	
	
	typedef int16 EReason;
	enum
		{
		eReason_DONE = 0,
		eReason_NETWORK_ERR = 1,
		eReason_USER_BREAK = 2
		};
	
	
	// Version feature information
	typedef int EFeature;
	enum
		{
		eFeature_HasStreamOutput = 8,
		eFeature_HasNotification = 9,
		eFeature_HasLiveConnect = 9,
		eFeature_HasLiveConnectWin16 = 9,
		eFeature_HasLiveConnect68K = 9,
		eFeature_HasWindowless = 11
		};
	
	// ==================================================
	
	typedef class _jobject* jobject;
	typedef jobject jref;
	typedef const struct JRIEnvInterface* JRIEnv;
	typedef void* JRIGlobalRef;
	
	
	struct NPRect
		{
		uint16 top;
		uint16 left;
		uint16 bottom;
		uint16 right;
		};
	
	
	struct NPP_t
		{
		void* fData_Plugin; // plug-in private data
		void* fData_Navigator; // netscape private data
		};
	
	typedef NPP_t *NPP;
	
	struct NPSavedData
		{
		int32 fLen;
		void* fBuf;
		};
	
	
	// The type of a NPWindow - it specifies the type of the data structure
	// returned in the window field.
	typedef int EWindowType;
	enum
		{
		eWindowType_Window = 1,
		eWindowType_Drawable
		};
	
	
	struct NPWindow
		{
		void* fPlatformWindow; // Platform specific window handle
		int32 fLocationH; // Position of top left corner relative
		int32 fLocationV; //to a netscape page.
		uint32 fWidth; // Maximum window size
		uint32 fHeight;
		NPRect fClipRect; // Clipping rectangle in port coordinates
						// Used by MAC only.
		#if ZCONFIG(OS, POSIX)
			void* ws_info; // Platform-dependent additonal data
		#endif
		EWindowType fType; // Is this a window or a drawable?
		};
	
	
	struct NPStream
		{
		void* fPluginData;
		void* fPrivate; // netscape private data
		const char* fURL;
		uint32 fEnd;
		uint32 fLastModified;
		void* notifydata;
		};
	
	
	struct NPFullPrint
		{
		NPBool fPluginPrinted; // Set TRUE if plugin handled fullscreen printing
		NPBool fPrintOne; // TRUE if plugin should print one copy to default printer
		void* fPlatformPrint; // Platform-specific printing info
		};
	
	
	struct NPEmbedPrint
		{
		NPWindow fWindow;
		void* fPlatformPrint; // Platform-specific printing info
		};
	
	// Values for mode passed to Plugin_New:
	typedef uint16 EMode;
	enum
		{
		eMode_Embed = 1,
		eMode_Full = 2
		};
	
	struct NPPrint
		{
		EMode fMode;
		union
			{
			NPFullPrint fFullPrint; // if mode is full
			NPEmbedPrint fEmbedPrint; // if mode is embed
			};
		};
	
	
	// List of variable names for which Plugin_GetValue shall be implemented
	typedef int NPPVariable;
	enum
		{
		NPPVpluginNameString = 1,
		NPPVpluginDescriptionString,
		NPPVpluginWindowBool,
		NPPVpluginTransparentBool,
		NPPVjavaClass,                /* Not implemented in Mozilla 1.0 */
		NPPVpluginWindowSize,
		NPPVpluginTimerInterval,
	
		NPPVpluginScriptableInstance = 10,
		NPPVpluginScriptableIID = 11,
	
		/* 12 and over are available on Mozilla builds starting with 0.9.9 */
		NPPVjavascriptPushCallerBool = 12,
		NPPVpluginKeepLibraryInMemory = 13   /* available in Mozilla 1.0 */
		};
	
	
	// List of variable names for which Navigator_GetValue is implemented by Mozilla
	typedef int NPNVariable;
	enum
		{
		NPNVxDisplay = 1,
		NPNVxtAppContext,
		NPNVnetscapeWindow,
		NPNVjavascriptEnabledBool,
		NPNVasdEnabledBool,
		NPNVisOfflineBool,
	
		/* 10 and over are available on Mozilla builds starting with 0.9.4 */
		NPNVserviceManager = 10,
		NPNVDOMElement     = 11,   /* available in Mozilla 1.2 */
		NPNVDOMDocument    = 12,
		NPNVDOMWindow      = 13
		};
	
	
	struct NPByteRange
		{
		int32 offset; // negative offset means from the end
		uint32 length;
		NPByteRange* next;
		};
	
	
	
	// Values for stream type passed to Plugin_NewStream:
	typedef uint16 EStreamType;
	enum
		{
		eStreamType_Normal = 1,
		eStreamType_Seek = 2,
		eStreamType_AsFile = 3,
		eStreamType_AsFileOnly = 4
		};
	
	// ==================================================
	
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	
	typedef RgnHandle NPRegion;
	typedef EventRecord NPEvent;
	
	struct NPPort
		{
		CGrafPtr fPort; // Grafport
		int32 fPortH; // position inside the topmost window
		int32 fPortV;
		};
	
	// Non-standard event types that can be passed to HandleEvent
	enum EEventType
		{
		eEventType_GetFocusEvent = (osEvt + 16),
		eEventType_LoseFocusEvent,
		eEventType_AdjustCursorEvent,
		eEventType_MenuCommandEvent,
		eEventType_ClippingChangedEvent,
		eEventType_ScrollingBeginsEvent = 1000,
		eEventType_ScrollingEndsEvent
		};
	
	#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	
	// ==================================================
	
	#if ZCONFIG(OS, Win32)
	
	typedef HRGN NPRegion;
	struct NPEvent
		{
		uint16 fEvent;
		uint32 fWPARAM;
		uint32 fLPARAM;
		};
	
	#endif // ZCONFIG(OS, Win32)
	
	// ==================================================
	
	#if ZCONFIG(OS, POSIX)
	
	typedef Region NPRegion;
	typedef XEvent NPEvent;
	
	typedef int32 ECallbackType;
	enum
		{
		eCallbackType_SetWindow = 1,
		eCallbackType_Print = 2
		};
	
	struct NPAnyCallbackStruct
		{
		ECallbackType fType;
		};
	
	struct NPSetWindowCallbackStruct
		{
		ECallbackType fType;
		Display* fDisplay;
		Visual* fVisual;
		Colormap fColormap;
		unsigned int fDepth;
		};
	
	struct NPPrintCallbackStruct
		{
		ECallbackType fType;
		FILE* fFILE;
		};
	#endif // ZCONFIG(OS, POSIX)
	
	// ==================================================
	
	#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	#	pragma options align=reset
	#endif
	
	// ==================================================
	// Navigator_* functions are provided by the navigator and called by the plugin.
	
	static void sNavigator_Version(int* oPluginMajor, int* oPluginMinor, int* oNavigatorMajor, int* oNavigatorMinor);
	static EError sNavigator_GetURL(NPP iNPP, const char* iURL, const char* iTarget);
	static EError sNavigator_PostURL(NPP iNPP, const char* iURL, const char* iTarget, uint32 iLen, const char* iBuf, NPBool iIsFileName);
	static EError sNavigator_RequestRead(NPP iNPP, NPStream* iStream, NPByteRange* iRangeList);
	static EError sNavigator_NewStream(NPP iNPP, const char* iMIMEType, const char* iTarget, NPStream** iStream);
	static int32 sNavigator_Write(NPP iNPP, NPStream* iStream, int32 iLen, const void* iSource);
	static EError sNavigator_DestroyStream(NPP iNPP, NPStream* iStream, EReason iReason);
	static void sNavigator_Status(NPP iNPP, const char* iMessage);
	static const char* sNavigator_UserAgent(NPP iNPP);
	static void* sNavigator_MemAlloc(uint32 iSize);
	static void sNavigator_MemFree( void* iPtr);
	static uint32 sNavigator_MemFlush(uint32 iSize);
	static void sNavigator_ReloadPlugins(NPBool iReloadPages);
	static JRIEnv* sNavigator_GetJavaEnv();
	static jref sNavigator_GetJavaPeer(NPP iNPP);
	static EError sNavigator_GetURLNotify(NPP iNPP, const char* iURL, const char* iTarget, void* iRefCon);
	static EError sNavigator_PostURLNotify(NPP iNPP, const char* iURL, const char* iTarget, uint32 iLen, const char* iBuf, NPBool iFile, void* iRefCon);
	static EError sNavigator_GetValue(NPP iNPP, NPNVariable iVariable, void* oValue);
	static EError sNavigator_SetValue(NPP iNPP, NPPVariable iVariable, void* iValue);
	static void sNavigator_InvalidateRect(NPP iNPP, const NPRect& iRect);
	static void sNavigator_InvalidateRegion(NPP iNPP, NPRegion iRegion);
	static void sNavigator_ForceRedraw(NPP iNPP);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginInstance

class ZNSPluginInstance : public ZNSPlugin
	{
public:
// Plugin_* functions are provided by the plugin and called by navigator.
	ZNSPluginInstance(NPP iNPP);
	virtual ~ZNSPluginInstance();

	virtual EError Destroy(NPSavedData*& oSavedData);
	virtual EError SetWindow(NPWindow* iNPWindow) = 0;
	virtual EError NewStream(const char* iMIMEType, NPStream* iStream, NPBool iSeekable, EStreamType& oType);
	virtual EError DestroyStream(NPStream* iStream, EReason iReason);
	virtual void StreamAsFile(NPStream* iStream, const char* iFilePath);
	virtual int32 WriteReady(NPStream* iStream);
	virtual int32 Write(NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer);
	virtual void Print(NPPrint& iPlatformPrint);
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	virtual int16 HandleEvent(const EventRecord& iEvent);
#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	virtual void URLNotify(const char* iURL, EReason iReason, void* iRefCon);
	virtual EError GetValue(void* iInstance, NPPVariable variable, void* oValue);
	virtual EError SetValue(void* iInstance, NPNVariable variable, void* iValue);

protected:
	NPP fNPP;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginMeister

class ZNSPluginMeister : public ZNSPlugin
	{
public:
	/*! This must be implemented in your code and should instantiate a concrete subclass of ZNSPluginMeister.*/
	static void sCreate();

	ZNSPluginMeister();
	virtual ~ZNSPluginMeister();

	virtual void Shutdown();
	virtual void New(NPP iNPP, const char* iMIMEType, EMode iMode, int16 iArgc, const char* iArgn[], const char* iArgv[], NPSavedData* iSavedData, ZNSPluginInstance*& oInstance) = 0;
	virtual EError New(const char* iMIMEType, NPP iNPP, EMode iMode, int16 iArgc, const char* iArgn[], const char* iArgv[], NPSavedData* iSavedData);
	virtual EError Destroy(NPP iNPP, NPSavedData*& oSavedData);
	virtual EError SetWindow(NPP iNPP, NPWindow* iNPWindow);
	virtual EError NewStream(NPP iNPP, const char* iMIMEType, NPStream* iStream, NPBool iSeekable, EStreamType& oType);
	virtual EError DestroyStream(NPP iNPP, NPStream* iStream, EReason iReason);
	virtual void StreamAsFile(NPP iNPP, NPStream* iStream, const char* iFilePath);
	virtual int32 WriteReady(NPP iNPP, NPStream* iStream);
	virtual int32 Write(NPP iNPP, NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer);
	virtual void Print(NPP iNPP, NPPrint& iPlatformPrint);
	virtual int16 HandleEvent(NPP iNPP, void* iEvent);
	virtual void URLNotify(NPP iNPP, const char* iURL, EReason iReason, void* iRefCon);
	virtual EError GetValue(NPP iNPP, void* iInstance, NPPVariable variable, void* oValue);
	virtual EError SetValue(NPP iNPP, void* iInstance, NPNVariable variable, void* iValue);

private:
	ZNSPluginInstance* NPPToInstance(NPP iNPP);
	};

#endif // __ZNSPlugin__
