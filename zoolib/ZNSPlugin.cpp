#include "ZDebug.h"
#include "ZNSPlugin.h"

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
#	include <CodeFragments.h>
#	include <Gestalt.h>
#	include <Processes.h>
#	include <Resources.h>
#	include <Sound.h>
#endif

#if ZCONFIG(OS, POSIX)
#endif

#define kDebug_NSPlugin 0

ZNSPluginMeister* ZNSPlugin::sMeister;

// =================================================================================================
#define USE_UPP (TARGET_RT_MAC_CFM && !TARGET_API_MAC_CARBON)

#if USE_UPP
#	define CreateUPP(proc, func)(proc##_UPP)NewRoutineDescriptor((ProcPtr)func, proc##_ProcInfo, GetCurrentArchitecture())
#else
#	define CreateUPP(proc, func) func
#endif

// =================================================================================================
// 0 Arguments -----

#define Priv_DefineProcPtr0(ret, proc) \
	typedef ret(*proc##_ProcPtr)()

#if USE_UPP
#	define Call0(ret, proc, upp) \
			(ret) CallUniversalProc(upp, proc##_ProcInfo)
#	define Priv_DefineUPP0(ret, proc) \
			typedef UniversalProcPtr proc##_UPP; \
			enum \
				{ \
				proc##_ProcInfo = kThinkCStackBased \
				| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(0)) \
				| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
				};

#	define Priv_DefineUPP0_Void(proc) \
			typedef UniversalProcPtr proc##_UPP; \
			enum \
				{ \
				proc##_ProcInfo = kThinkCStackBased \
				| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(0)) \
				};
#else
#	define Call0(ret, proc, upp) \
			upp()
#	define Priv_DefineUPP0(ret, proc) \
			typedef proc##_ProcPtr proc##_UPP

#	define Priv_DefineUPP0_Void(proc) \
			typedef proc##_ProcPtr proc##_UPP
#endif
#define DefineProc0(ret, upp) \
	Priv_DefineProcPtr0(ret, upp); \
	Priv_DefineUPP0(ret, upp)
#define DefineProc0_Void(upp) \
	Priv_DefineProcPtr0(void, upp); \
	Priv_DefineUPP0_Void(upp)

// =================================================================================================
// 1 Argument

#define Priv_DefineProcPtr1(ret, proc, arg1) \
	typedef ret(*proc##_ProcPtr)(arg1)

#if USE_UPP
	#define Call1(ret, proc, upp, arg1) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1)
	#define Priv_DefineUPP1(ret, proc, arg1) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
	#define Priv_DefineUPP1_Void(proc, arg1) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			};
#else
	#define Call1(ret, proc, upp, arg1) \
		upp(arg1)
	#define Priv_DefineUPP1(ret, proc, arg1) \
		typedef proc##_ProcPtr proc##_UPP
	#define Priv_DefineUPP1_Void(proc, arg1) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc1(ret, upp, arg1) \
	Priv_DefineProcPtr1(ret, upp, arg1); \
	Priv_DefineUPP1(ret, upp, arg1)

#define DefineProc1_Void(upp, arg1) \
	Priv_DefineProcPtr1(void, upp, arg1); \
	Priv_DefineUPP1_Void(upp, arg1)

// =================================================================================================
// 2 Arguments

#define Priv_DefineProcPtr2(ret, proc, arg1, arg2) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2)

#if USE_UPP
	#define Call2(ret, proc, upp, arg1, arg2) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2)
	#define Priv_DefineUPP2(ret, proc, arg1, arg2) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
	#define Priv_DefineUPP2_Void(proc, arg1, arg2) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			};
#else
	#define Call2(ret, proc, upp, arg1, arg2) \
		upp(arg1, arg2)
	#define Priv_DefineUPP2(ret, proc, arg1, arg2) \
		typedef proc##_ProcPtr proc##_UPP
	#define Priv_DefineUPP2_Void(proc, arg1, arg2) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc2(ret, upp, arg1, arg2) \
	Priv_DefineProcPtr2(ret, upp, arg1, arg2); \
	Priv_DefineUPP2(ret, upp, arg1, arg2)
#define DefineProc2_Void(upp, arg1, arg2) \
	Priv_DefineProcPtr2(void, upp, arg1, arg2); \
	Priv_DefineUPP2_Void(upp, arg1, arg2)

// =================================================================================================
// 3 Arguments

#define Priv_DefineProcPtr3(ret, proc, arg1, arg2, arg3) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2, arg3)

#if USE_UPP
	#define Call3(ret, proc, upp, arg1, arg2, arg3) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3)
	#define Priv_DefineUPP3(ret, proc, arg1, arg2, arg3) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
	#define Priv_DefineUPP3_Void(proc, arg1, arg2, arg3) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			};
#else
	#define Call3(ret, proc, upp, arg1, arg2, arg3) \
		upp(arg1, arg2, arg3)
	#define Priv_DefineUPP3(ret, proc, arg1, arg2, arg3) \
		typedef proc##_ProcPtr proc##_UPP
	#define Priv_DefineUPP3_Void(proc, arg1, arg2, arg3) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc3(ret, upp, arg1, arg2, arg3) \
	Priv_DefineProcPtr3(ret, upp, arg1, arg2, arg3); \
	Priv_DefineUPP3(ret, upp, arg1, arg2, arg3)
#define DefineProc3_Void(upp, arg1, arg2, arg3) \
	Priv_DefineProcPtr3(void, upp, arg1, arg2, arg3); \
	Priv_DefineUPP3_Void(upp, arg1, arg2, arg3)

// =================================================================================================
// 4 Arguments

#define Priv_DefineProcPtr4(ret, proc, arg1, arg2, arg3, arg4) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2, arg3, arg4)

#if USE_UPP
	#define Call4(ret, proc, upp, arg1, arg2, arg3, arg4) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4)
	#define Priv_DefineUPP4(ret, proc, arg1, arg2, arg3, arg4) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
	#define Priv_DefineUPP4_Void(proc, arg1, arg2, arg3, arg4) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			};
#else
	#define Call4(ret, proc, upp, arg1, arg2, arg3, arg4) \
		upp(arg1, arg2, arg3, arg4)
	#define Priv_DefineUPP4(ret, proc, arg1, arg2, arg3, arg4) \
		typedef proc##_ProcPtr proc##_UPP
	#define Priv_DefineUPP4_Void(proc, arg1, arg2, arg3, arg4) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc4(ret, upp, arg1, arg2, arg3, arg4) \
	Priv_DefineProcPtr4(ret, upp, arg1, arg2, arg3, arg4); \
	Priv_DefineUPP4(ret, upp, arg1, arg2, arg3, arg4)
#define DefineProc4_Void(upp, arg1, arg2, arg3, arg4) \
	Priv_DefineProcPtr4(void, upp, arg1, arg2, arg3, arg4); \
	Priv_DefineUPP4_Void(upp, arg1, arg2, arg3, arg4)

// =================================================================================================
// 5 Arguments

#define Priv_DefineProcPtr5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2, arg3, arg4, arg5)

#if USE_UPP
	#define Call5(ret, proc, upp, arg1, arg2, arg3, arg4, arg5) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5)
	#define Priv_DefineUPP5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
#else
	#define Call5(ret, proc, upp, arg1, arg2, arg3, arg4, arg5) \
		upp(arg1, arg2, arg3, arg4, arg5)
	#define Priv_DefineUPP5(ret, proc, arg1, arg2, arg3, arg4, arg5) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc5(ret, upp, arg1, arg2, arg3, arg4, arg5) \
	Priv_DefineProcPtr5(ret, upp, arg1, arg2, arg3, arg4, arg5); \
	Priv_DefineUPP5(ret, upp, arg1, arg2, arg3, arg4, arg5)

// =================================================================================================
// 6 Arguments

#define Priv_DefineProcPtr6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2, arg3, arg4, arg5, arg6)

#if USE_UPP
	#define Call6(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5, arg6)
	#define Priv_DefineUPP6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
#else
	#define Call6(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
		upp(arg1, arg2, arg3, arg4, arg5, arg6)
	#define Priv_DefineUPP6(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6) \
	Priv_DefineProcPtr6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6); \
	Priv_DefineUPP6(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6)

// =================================================================================================
// 7 Arguments

#define Priv_DefineProcPtr7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	typedef ret(*proc##_ProcPtr)(arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#if USE_UPP
	#define Call7(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		(ret) CallUniversalProc(upp, proc##_ProcInfo, arg1, arg2, arg3, arg4, arg5, arg6, arg7)
	#define Priv_DefineUPP7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef UniversalProcPtr proc##_UPP; \
		enum \
			{ \
			proc##_ProcInfo = kThinkCStackBased \
			| STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(arg1))) \
			| STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(arg2))) \
			| STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(arg3))) \
			| STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(arg4))) \
			| STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(arg5))) \
			| STACK_ROUTINE_PARAMETER(6, SIZE_CODE(sizeof(arg6))) \
			| STACK_ROUTINE_PARAMETER(7, SIZE_CODE(sizeof(arg7))) \
			| RESULT_SIZE(SIZE_CODE(sizeof(ret))) \
			};
#else
	#define Call7(ret, proc, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		upp(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
	#define Priv_DefineUPP7(ret, proc, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
		typedef proc##_ProcPtr proc##_UPP
#endif		
#define DefineProc7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
	Priv_DefineProcPtr7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
	Priv_DefineUPP7(ret, upp, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

// =================================================================================================
// Plugin entry points

DefineProc0_Void(Dispatch_Shutdown);
DefineProc7(ZNSPlugin::EError, Dispatch_New, const char*, ZNSPlugin::NPP, ZNSPlugin::EMode, int16, const char**, const char**, ZNSPlugin::NPSavedData*);
DefineProc2(ZNSPlugin::EError, Dispatch_Destroy, ZNSPlugin::NPP, ZNSPlugin::NPSavedData*&);
DefineProc2(ZNSPlugin::EError, Dispatch_SetWindow, ZNSPlugin::NPP, ZNSPlugin::NPWindow*);
DefineProc5(ZNSPlugin::EError, Dispatch_NewStream, ZNSPlugin::NPP, const char*, ZNSPlugin::NPStream*, ZNSPlugin::NPBool, ZNSPlugin::EStreamType&);
DefineProc3(ZNSPlugin::EError, Dispatch_DestroyStream, ZNSPlugin::NPP, ZNSPlugin::NPStream*, ZNSPlugin::EReason);
DefineProc2(int32, Dispatch_WriteReady, ZNSPlugin::NPP, ZNSPlugin::NPStream*);
DefineProc5(int32, Dispatch_Write, ZNSPlugin::NPP, ZNSPlugin::NPStream*, int32, int32, const void*);
DefineProc3_Void(Dispatch_StreamAsFile, ZNSPlugin::NPP, ZNSPlugin::NPStream*, const char*);
DefineProc2_Void(Dispatch_Print, ZNSPlugin::NPP, ZNSPlugin::NPPrint&);
DefineProc2(int16, Dispatch_HandleEvent, ZNSPlugin::NPP, void*);
DefineProc4_Void(Dispatch_URLNotify, ZNSPlugin::NPP, const char*, ZNSPlugin::EReason, void*);
DefineProc3(ZNSPlugin::EError, Dispatch_GetValue, ZNSPlugin::NPP, ZNSPlugin::NPPVariable, void*);
DefineProc3(ZNSPlugin::EError, Dispatch_SetValue, ZNSPlugin::NPP, ZNSPlugin::NPNVariable, void*);

// Netscape entry points

DefineProc3(ZNSPlugin::EError, Navigator_GetValue, ZNSPlugin::NPP, ZNSPlugin::NPNVariable, void*);
DefineProc3(ZNSPlugin::EError, Navigator_SetValue, ZNSPlugin::NPP, ZNSPlugin::NPPVariable, void*);
DefineProc4(ZNSPlugin::EError, Navigator_GetURLNotify, ZNSPlugin::NPP, const char*, const char*, void*);
DefineProc7(ZNSPlugin::EError, Navigator_PostURLNotify, ZNSPlugin::NPP, const char*, const char*, uint32, const char*, ZNSPlugin::NPBool, void*);
DefineProc3(ZNSPlugin::EError, Navigator_GetURL, ZNSPlugin::NPP, const char*, const char*);
DefineProc6(ZNSPlugin::EError, Navigator_PostURL, ZNSPlugin::NPP, const char*, const char*, uint32, const char*, ZNSPlugin::NPBool);
DefineProc2(ZNSPlugin::EError, Navigator_RequestRead, ZNSPlugin::NPStream*, ZNSPlugin::NPByteRange*);
DefineProc4(ZNSPlugin::EError, Navigator_NewStream, ZNSPlugin::NPP, const char*, const char*, ZNSPlugin::NPStream**);
DefineProc4(int32, Navigator_Write, ZNSPlugin::NPP, ZNSPlugin::NPStream*, int32, const void*);
DefineProc3(ZNSPlugin::EError, Navigator_DestroyStream, ZNSPlugin::NPP, ZNSPlugin::NPStream*, ZNSPlugin::EReason);
DefineProc2_Void(Navigator_Status, ZNSPlugin::NPP, const char*);
DefineProc1(const char*, Navigator_UserAgent, ZNSPlugin::NPP);
DefineProc1(void*, Navigator_MemAlloc, uint32);
DefineProc1_Void(Navigator_MemFree, void*);
DefineProc1(uint32, Navigator_MemFlush, uint32);
DefineProc1_Void(Navigator_ReloadPlugins, ZNSPlugin::NPBool);
DefineProc0(ZNSPlugin::JRIEnv*, Navigator_GetJavaEnv);
DefineProc1(ZNSPlugin::jref, Navigator_GetJavaPeer, ZNSPlugin::NPP);
DefineProc2_Void(Navigator_InvalidateRect, ZNSPlugin::NPP, const ZNSPlugin::NPRect&);
DefineProc2_Void(Navigator_InvalidateRegion, ZNSPlugin::NPP, ZNSPlugin::NPRegion);
DefineProc1_Void(Navigator_ForceRedraw, ZNSPlugin::NPP);

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
#	pragma align=mac68k
#endif

struct NPPluginFuncs
	{
	uint16 fSize;
	uint16 fVersion;
	Dispatch_New_UPP fNew;
	Dispatch_Destroy_UPP fDestroy;
	Dispatch_SetWindow_UPP fSetWindow;
	Dispatch_NewStream_UPP fNewStream;
	Dispatch_DestroyStream_UPP fDestroyStream;
	Dispatch_StreamAsFile_UPP fStreamAsFile;
	Dispatch_WriteReady_UPP fWriteReady;
	Dispatch_Write_UPP fWrite;
	Dispatch_Print_UPP fPrint;
	Dispatch_HandleEvent_UPP fHandleEvent;
	Dispatch_URLNotify_UPP fURLNotify;
	ZNSPlugin::JRIGlobalRef fJavaClass;
	Dispatch_GetValue_UPP fGetValue;
	Dispatch_SetValue_UPP fSetValue;
	};

struct NPNetscapeFuncs
	{
	uint16 fSize;
	uint16 fVersion;
	Navigator_GetURL_UPP fGetURL;
	Navigator_PostURL_UPP fPostURL;
	Navigator_RequestRead_UPP fRequestRead;
	Navigator_NewStream_UPP fNewStream;
	Navigator_Write_UPP fWrite;
	Navigator_DestroyStream_UPP fDestroyStream;
	Navigator_Status_UPP fStatus;
	Navigator_UserAgent_UPP fUserAgent;
	Navigator_MemAlloc_UPP fMemAlloc;
	Navigator_MemFree_UPP fMemFree;
	Navigator_MemFlush_UPP fMemFlush;
	Navigator_ReloadPlugins_UPP fReloadPlugins;
	Navigator_GetJavaEnv_UPP fGetJavaEnv;
	Navigator_GetJavaPeer_UPP fGetJavaPeer;
	Navigator_GetURLNotify_UPP fGetURLNotify;
	Navigator_PostURLNotify_UPP fPostURLNotify;
	Navigator_GetValue_UPP fGetValue;
	Navigator_SetValue_UPP fSetValue;
	Navigator_InvalidateRect_UPP fInvalidateRect;
	Navigator_InvalidateRegion_UPP fInvalidateRegion;
	Navigator_ForceRedraw_UPP fForceRedraw;
	};

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
#	pragma align=reset
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Calls to Navigator

static NPNetscapeFuncs* sNPNetscapeFuncsPtr;

void ZNSPlugin::sNavigator_Version(int* oPluginMajor, int* oPluginMinor, int* oNavigatorMajor, int* oNavigatorMinor)
	{
	if (oPluginMajor)
		*oPluginMajor = ZNSPlugin::kVersionMajor;
	if (oPluginMinor)
		*oPluginMinor = ZNSPlugin::kVersionMinor;
	if (oNavigatorMajor)
		*oNavigatorMajor = sNPNetscapeFuncsPtr->fVersion >> 8;
	if (oNavigatorMinor)
		*oNavigatorMinor = sNPNetscapeFuncsPtr->fVersion & 0xFF;
	}

ZNSPlugin::EError ZNSPlugin::sNavigator_GetURL(NPP iNPP, const char* iURL, const char* iTarget)
	{ return Call3(EError, Navigator_GetURL, sNPNetscapeFuncsPtr->fGetURL, iNPP, iURL, iTarget); }

ZNSPlugin::EError ZNSPlugin::sNavigator_PostURL(NPP iNPP, const char* iURL, const char* iTarget, uint32 iLen, const char* iBuf, NPBool iIsFileName)
	{ return Call6(EError, Navigator_PostURL, sNPNetscapeFuncsPtr->fPostURL, iNPP, iURL, iTarget, iLen, iBuf, iIsFileName); }

ZNSPlugin::EError ZNSPlugin::sNavigator_RequestRead(NPP iNPP, NPStream* iStream, NPByteRange* iRangeList)
	{ return Call2(EError, Navigator_RequestRead, sNPNetscapeFuncsPtr->fRequestRead, iStream, iRangeList); }

ZNSPlugin::EError ZNSPlugin::sNavigator_NewStream(NPP iNPP, const char* iMIMEType, const char* iTarget, NPStream** iStream)
	{
	if (ZNSPlugin::eFeature_HasStreamOutput <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call4(EError, Navigator_NewStream, sNPNetscapeFuncsPtr->fNewStream, iNPP, iMIMEType, iTarget, iStream);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

int32 ZNSPlugin::sNavigator_Write(NPP iNPP, NPStream* iStream, int32 iLen, const void* iSource)
	{
	if (ZNSPlugin::eFeature_HasStreamOutput <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call4(int32, Navigator_Write, sNPNetscapeFuncsPtr->fWrite, iNPP, iStream, iLen, iSource);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

ZNSPlugin::EError ZNSPlugin::sNavigator_DestroyStream(NPP iNPP, NPStream* iStream, EReason iReason)
	{
	if (ZNSPlugin::eFeature_HasStreamOutput <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call3(EError, Navigator_DestroyStream, sNPNetscapeFuncsPtr->fDestroyStream, iNPP, iStream, iReason);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

void ZNSPlugin::sNavigator_Status(NPP iNPP, const char* iMessage)
	{ Call2(void, Navigator_Status, sNPNetscapeFuncsPtr->fStatus, iNPP, iMessage); }

const char* ZNSPlugin::sNavigator_UserAgent(NPP iNPP)
	{ return Call1(const char*, Navigator_UserAgent, sNPNetscapeFuncsPtr->fUserAgent, iNPP); }

void* ZNSPlugin::sNavigator_MemAlloc(uint32 iSize)
	{ return Call1(void*, Navigator_MemAlloc, sNPNetscapeFuncsPtr->fMemAlloc, iSize); }

void ZNSPlugin::sNavigator_MemFree(void* iPtr)
	{ Call1(void, Navigator_MemFree, sNPNetscapeFuncsPtr->fMemFree, iPtr); }

uint32 ZNSPlugin::sNavigator_MemFlush(uint32 iSize)
	{ return Call1(uint32, Navigator_MemFlush, sNPNetscapeFuncsPtr->fMemFlush, iSize); }

void ZNSPlugin::sNavigator_ReloadPlugins(NPBool iReloadPages)
	{ Call1(void, Navigator_ReloadPlugins, sNPNetscapeFuncsPtr->fReloadPlugins, iReloadPages); }

ZNSPlugin::JRIEnv* ZNSPlugin::sNavigator_GetJavaEnv()
	{ return Call0(JRIEnv*, Navigator_GetJavaEnv, sNPNetscapeFuncsPtr->fGetJavaEnv); }

ZNSPlugin::jref ZNSPlugin::sNavigator_GetJavaPeer(NPP iNPP)
	{ return Call1(jref, Navigator_GetJavaPeer, sNPNetscapeFuncsPtr->fGetJavaPeer, iNPP); }

ZNSPlugin::EError ZNSPlugin::sNavigator_GetURLNotify(NPP iNPP, const char* iURL, const char* iTarget, void* iRefCon)
	{
	if (eFeature_HasNotification <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call4(EError, Navigator_GetURLNotify, sNPNetscapeFuncsPtr->fGetURLNotify, iNPP, iURL, iTarget, iRefCon);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

ZNSPlugin::EError ZNSPlugin::sNavigator_PostURLNotify(NPP iNPP, const char* iURL, const char* iTarget, uint32 iLen, const char* iBuf, NPBool iFile, void* iRefCon)
	{
	if (eFeature_HasNotification <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call7(EError, Navigator_PostURLNotify, sNPNetscapeFuncsPtr->fPostURLNotify, iNPP, iURL, iTarget, iLen, iBuf, iFile, iRefCon);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

ZNSPlugin::EError ZNSPlugin::sNavigator_GetValue(NPP iNPP, NPNVariable iVariable, void* oValue)
	{
	// ?? Version check okay?
	if (ZNSPlugin::eFeature_HasWindowless <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call3(EError, Navigator_GetValue, sNPNetscapeFuncsPtr->fGetValue, iNPP, iVariable, oValue);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

ZNSPlugin::EError ZNSPlugin::sNavigator_SetValue(NPP iNPP, NPPVariable iVariable, void* iValue)
	{
	if (ZNSPlugin::eFeature_HasWindowless <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		return Call3(EError, Navigator_SetValue, sNPNetscapeFuncsPtr->fSetValue, iNPP, iVariable, iValue);
	return ZNSPlugin::eError_IncompatibleVersion;
	}

void ZNSPlugin::sNavigator_InvalidateRect(NPP iNPP, const NPRect& iRect)
	{
	if (ZNSPlugin::eFeature_HasWindowless <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		Call2(void, Navigator_InvalidateRect, sNPNetscapeFuncsPtr->fInvalidateRect, iNPP, iRect);
	}

void ZNSPlugin::sNavigator_InvalidateRegion(NPP iNPP, NPRegion iRegion)
	{
	if (ZNSPlugin::eFeature_HasWindowless <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		Call2(void, Navigator_InvalidateRegion, sNPNetscapeFuncsPtr->fInvalidateRegion, iNPP, iRegion);
	}

void ZNSPlugin::sNavigator_ForceRedraw(NPP iNPP)
	{
	if (ZNSPlugin::eFeature_HasWindowless <= (sNPNetscapeFuncsPtr->fVersion & 0xFF))
		Call1(void, Navigator_ForceRedraw, sNPNetscapeFuncsPtr->fForceRedraw, iNPP);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Dispatch wrappers

namespace ZANONYMOUS {

void sDispatch_Shutdown();
ZNSPlugin::EError sDispatch_New(const char* iMIMEType, ZNSPlugin::NPP iNPP, ZNSPlugin::EMode iMode, int16 iArgc, const char* iArgn[], const char* iArgv[], ZNSPlugin::NPSavedData* iSavedData);
ZNSPlugin::EError sDispatch_Destroy(ZNSPlugin::NPP iNPP, ZNSPlugin::NPSavedData*& oSavedData);
ZNSPlugin::EError sDispatch_SetWindow(ZNSPlugin::NPP iNPP, ZNSPlugin::NPWindow* iNPWindow);
ZNSPlugin::EError sDispatch_NewStream(ZNSPlugin::NPP iNPP, const char* iMIMEType, ZNSPlugin::NPStream* iStream, ZNSPlugin::NPBool iSeekable, uint16& oType);
ZNSPlugin::EError sDispatch_DestroyStream(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, ZNSPlugin::EReason iReason);
int32 sDispatch_WriteReady(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream);
int32 sDispatch_Write(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer);
void sDispatch_StreamAsFile(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, const char* iFilePath);
void sDispatch_Print(ZNSPlugin::NPP iNPP, ZNSPlugin::NPPrint& iPlatformPrint);
int16 sDispatch_HandleEvent(ZNSPlugin::NPP iNPP, void* iEvent);
void sDispatch_URLNotify(ZNSPlugin::NPP iNPP, const char* iURL, ZNSPlugin::EReason iReason, void* iRefCon);
//JRIGlobalRef sDispatch_GetJavaClass();

void sDispatch_Shutdown()
	{
	try
		{
		if (ZNSPlugin::sMeister)
			ZNSPlugin::sMeister->Shutdown();
		}
	catch (...)
		{}
	}

ZNSPlugin::EError sDispatch_New(const char* iMIMEType, ZNSPlugin::NPP iNPP, ZNSPlugin::EMode iMode, int16 iArgc, const char* iArgn[], const char* iArgv[], ZNSPlugin::NPSavedData* iSavedData)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->New(iMIMEType, iNPP, iMode, iArgc, iArgn, iArgv, iSavedData);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

ZNSPlugin::EError sDispatch_Destroy(ZNSPlugin::NPP iNPP, ZNSPlugin::NPSavedData*& oSavedData)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->Destroy(iNPP, oSavedData);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

ZNSPlugin::EError sDispatch_SetWindow(ZNSPlugin::NPP iNPP, ZNSPlugin::NPWindow* iNPWindow)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->SetWindow(iNPP, iNPWindow);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

ZNSPlugin::EError sDispatch_NewStream(ZNSPlugin::NPP iNPP, const char* iMIMEType, ZNSPlugin::NPStream* iStream, ZNSPlugin::NPBool iSeekable, uint16& oType)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->NewStream(iNPP, iMIMEType, iStream, iSeekable, oType);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

ZNSPlugin::EError sDispatch_DestroyStream(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, ZNSPlugin::EReason iReason)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->DestroyStream(iNPP, iStream, iReason);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

int32 sDispatch_WriteReady(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->WriteReady(iNPP, iStream);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

int32 sDispatch_Write(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->Write(iNPP, iStream, iOffset, iLen, iBuffer);
		}
	catch (...)
		{}
	return ZNSPlugin::eError_Generic;
	}

void sDispatch_StreamAsFile(ZNSPlugin::NPP iNPP, ZNSPlugin::NPStream* iStream, const char* iFilePath)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			ZNSPlugin::sMeister->StreamAsFile(iNPP, iStream, iFilePath);
		}
	catch (...)
		{}
	}

void sDispatch_Print(ZNSPlugin::NPP iNPP, ZNSPlugin::NPPrint& iPlatformPrint)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			ZNSPlugin::sMeister->Print(iNPP, iPlatformPrint);
		}
	catch (...)
		{}
	}

int16 sDispatch_HandleEvent(ZNSPlugin::NPP iNPP, void* iEvent)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			return ZNSPlugin::sMeister->HandleEvent(iNPP, iEvent);
		}
	catch (...)
		{}
	return false;
	}

void sDispatch_URLNotify(ZNSPlugin::NPP iNPP, const char* iURL, ZNSPlugin::EReason iReason, void* iRefCon)
	{
	try
		{
		if (ZNSPlugin::sMeister)
			ZNSPlugin::sMeister->URLNotify(iNPP, iURL, iReason, iRefCon);
		}
	catch (...)
		{}
	}

#if 0
JRIGlobalRef sDispatch_GetJavaClass()
	{
//	if (jref clazz = Plugin_GetJavaClass())
//		{
//		JRIEnv* env = Navigator_GetJavaEnv();
//		return JRI_NewGlobalRef(env, clazz);
//		}
	return nil;
	}
#endif
} // anonymous namespace

// =================================================================================================
// =================================================================================================
#pragma mark -
#pragma mark * Win32

#if ZCONFIG(OS, Win32)

extern "C" {

ZNSPlugin::EError WINAPI NP_GetEntryPoints(NPPluginFuncs* oNPPluginFuncs);
ZNSPlugin::EError WINAPI NP_Initialize(NPNetscapeFuncs* iNPNetscapeFuncs);
ZNSPlugin::EError WINAPI NP_Shutdown();

} // extern "C" 

HINSTANCE sHINSTANCE;

extern "C" BOOL WINAPI DllMain(HINSTANCE iHINSTANCE, DWORD fdwReason, LPVOID lpvReserved);
extern "C" BOOL WINAPI DllMain(HINSTANCE iHINSTANCE, DWORD fdwReason, LPVOID lpvReserved)
	{
	switch (fdwReason)
		{
		case DLL_PROCESS_ATTACH:
		  sHINSTANCE = iHINSTANCE;
		  break;
		case DLL_THREAD_ATTACH:
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_DETACH:
			break;
		}
	return TRUE;
	}

static NPPluginFuncs* sNPPluginFuncs_Stashed;

ZNSPlugin::EError WINAPI NP_GetEntryPoints(NPPluginFuncs* oNPPluginFuncs)
	{
	if (!oNPPluginFuncs)
		return ZNSPlugin::eError_InvalidFunctionTable;

	oNPPluginFuncs->fVersion =			(ZNSPlugin::kVersionMajor << 8) | ZNSPlugin::kVersionMinor;

	oNPPluginFuncs->fNew =				CreateUPP(Dispatch_New, sDispatch_New);
	oNPPluginFuncs->fDestroy =			CreateUPP(Dispatch_Destroy, sDispatch_Destroy);
	oNPPluginFuncs->fSetWindow =		CreateUPP(Dispatch_SetWindow, sDispatch_SetWindow);
	oNPPluginFuncs->fNewStream =		CreateUPP(Dispatch_NewStream, sDispatch_NewStream);
	oNPPluginFuncs->fDestroyStream =	CreateUPP(Dispatch_DestroyStream, sDispatch_DestroyStream);
	oNPPluginFuncs->fStreamAsFile =		CreateUPP(Dispatch_StreamAsFile, sDispatch_StreamAsFile);
	oNPPluginFuncs->fWriteReady =		CreateUPP(Dispatch_WriteReady, sDispatch_WriteReady);
	oNPPluginFuncs->fWrite =			CreateUPP(Dispatch_Write, sDispatch_Write);
	oNPPluginFuncs->fPrint =			CreateUPP(Dispatch_Print, sDispatch_Print);
	oNPPluginFuncs->fHandleEvent =		0;
	sNPPluginFuncs_Stashed = oNPPluginFuncs;

	return ZNSPlugin::eError_None;
	}

ZNSPlugin::EError WINAPI NP_Initialize(NPNetscapeFuncs* iNPNetscapeFuncs)
	{
	if (!iNPNetscapeFuncs)
		return ZNSPlugin::eError_InvalidFunctionTable;

	sNPNetscapeFuncsPtr = iNPNetscapeFuncs;

	int navigatorMinorVersion = iNPNetscapeFuncs->fVersion & 0xFF;
	int navigatorMajorVersion = iNPNetscapeFuncs->fVersion >> 8;

	if(navigatorMajorVersion > ZNSPlugin::kVersionMajor)
		return ZNSPlugin::eError_IncompatibleVersion;

	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasNotification)
		sNPPluginFuncs_Stashed->fURLNotify = CreateUPP(Plugin_URLNotify, sDispatch_URLNotify);

//	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasLiveConnect)
//		sNPPluginFuncs_Stashed->fJavaClass = sDispatch_GetJavaClass();

	try
		{
		ZNSPluginMeister::sCreate();
		}
	catch (...)
		{
		return ZNSPlugin::eError_Generic;
		}
	
	return ZNSPlugin::eError_None;
	}

ZNSPlugin::EError WINAPI NP_Shutdown()
	{
	sDispatch_Shutdown();
	return ZNSPlugin::eError_None;
	}

#endif // ZCONFIG(OS, Win32)

// =================================================================================================
#pragma mark -
#pragma mark * MacOS7

#if ZCONFIG(OS, MacOS7)
	ZNSPlugin::EError main(NPNetscapeFuncs* iNPNetscapeFuncs, NPPluginFuncs* oNPPluginFuncs, Dispatch_Shutdown_UPP& oDispatch_Shutdown_UPP);
	DefineProc3(ZNSPlugin::EError, main, NPNetscapeFuncs*, NPPluginFuncs*, Dispatch_Shutdown_UPP&);

	#pragma export on
	RoutineDescriptor mainRD = BUILD_ROUTINE_DESCRIPTOR(main_ProcInfo, main);
	#pragma export off

	static void sSetUpQD();
#endif // ZCONFIG(OS, MacOS7)

#if ZCONFIG(OS, Carbon)
	#pragma export on
	ZNSPlugin::EError main(NPNetscapeFuncs* iNPNetscapeFuncs, NPPluginFuncs* oNPPluginFuncs, Dispatch_Shutdown_UPP& oDispatch_Shutdown_UPP);
	#pragma export off
#endif // ZCONFIG(OS, Carbon)

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

static NPNetscapeFuncs sNPNetscapeFuncs;

ZNSPlugin::EError main(NPNetscapeFuncs* iNPNetscapeFuncs, NPPluginFuncs* oNPPluginFuncs, Dispatch_Shutdown_UPP& oDispatch_Shutdown_UPP)
	{
	ZNSPlugin::EError err = ZNSPlugin::eError_None;
	
	// Ensure that everything Netscape passed us is valid!
	if (!iNPNetscapeFuncs || !oNPPluginFuncs || !&oDispatch_Shutdown_UPP)
		return ZNSPlugin::eError_InvalidFunctionTable;

	int navigatorMinorVersion = iNPNetscapeFuncs->fVersion & 0xFF;
	int navigatorMajorVersion = iNPNetscapeFuncs->fVersion >> 8;

	// Check the "major" version passed in Netscape's function table.
	// We won't load if the major version is newer than what we expect.
	if (navigatorMajorVersion > ZNSPlugin::kVersionMajor)
		return ZNSPlugin::eError_IncompatibleVersion;

	// Copy Netscape's function table field by field, as it may be missing
	// the last few fields if the plugin is running on an older browser.	
	sNPNetscapeFuncs.fVersion =			iNPNetscapeFuncs->fVersion;
	sNPNetscapeFuncs.fSize = 			iNPNetscapeFuncs->fSize;
	sNPNetscapeFuncs.fPostURL =			iNPNetscapeFuncs->fPostURL;
	sNPNetscapeFuncs.fGetURL =			iNPNetscapeFuncs->fGetURL;
	sNPNetscapeFuncs.fRequestRead = 	iNPNetscapeFuncs->fRequestRead;
	sNPNetscapeFuncs.fNewStream =		iNPNetscapeFuncs->fNewStream;
	sNPNetscapeFuncs.fWrite =			iNPNetscapeFuncs->fWrite;
	sNPNetscapeFuncs.fDestroyStream = 	iNPNetscapeFuncs->fDestroyStream;
	sNPNetscapeFuncs.fStatus = 			iNPNetscapeFuncs->fStatus;
	sNPNetscapeFuncs.fUserAgent = 		iNPNetscapeFuncs->fUserAgent;
	sNPNetscapeFuncs.fMemAlloc = 		iNPNetscapeFuncs->fMemAlloc;
	sNPNetscapeFuncs.fMemFree = 		iNPNetscapeFuncs->fMemFree;
	sNPNetscapeFuncs.fMemFlush = 		iNPNetscapeFuncs->fMemFlush;
	sNPNetscapeFuncs.fReloadPlugins = 	iNPNetscapeFuncs->fReloadPlugins;

	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasLiveConnect)
		{
		sNPNetscapeFuncs.fGetJavaEnv = iNPNetscapeFuncs->fGetJavaEnv;
		sNPNetscapeFuncs.fGetJavaPeer = iNPNetscapeFuncs->fGetJavaPeer;
		}

	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasNotification)
		{	
		sNPNetscapeFuncs.fGetURLNotify = iNPNetscapeFuncs->fGetURLNotify;
		sNPNetscapeFuncs.fPostURLNotify = iNPNetscapeFuncs->fPostURLNotify;
		}

	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasWindowless)
		{
		sNPNetscapeFuncs.fInvalidateRect = iNPNetscapeFuncs->fInvalidateRect;
		sNPNetscapeFuncs.fInvalidateRegion = iNPNetscapeFuncs->fInvalidateRegion;
		}
	
	sNPNetscapeFuncsPtr = &sNPNetscapeFuncs;

	// Set up the plugin function table that Netscape will use to
	// call us. Netscape needs to know about our version and size
	// and have a UniversalProcPointer for every function we implement.

	oNPPluginFuncs->fVersion =			(ZNSPlugin::kVersionMajor << 8) + ZNSPlugin::kVersionMinor;

	oNPPluginFuncs->fNew =				CreateUPP(Dispatch_New, sDispatch_New);
	oNPPluginFuncs->fDestroy =			CreateUPP(Dispatch_Destroy, sDispatch_Destroy);
	oNPPluginFuncs->fSetWindow =		CreateUPP(Dispatch_SetWindow, sDispatch_SetWindow);
	oNPPluginFuncs->fNewStream =		CreateUPP(Dispatch_NewStream, sDispatch_NewStream);
	oNPPluginFuncs->fDestroyStream =	CreateUPP(Dispatch_DestroyStream, sDispatch_DestroyStream);
	oNPPluginFuncs->fStreamAsFile =		CreateUPP(Dispatch_StreamAsFile, sDispatch_StreamAsFile);
	oNPPluginFuncs->fWriteReady =		CreateUPP(Dispatch_WriteReady, sDispatch_WriteReady);
	oNPPluginFuncs->fWrite =			CreateUPP(Dispatch_Write, sDispatch_Write);
	oNPPluginFuncs->fPrint =			CreateUPP(Dispatch_Print, sDispatch_Print);
	oNPPluginFuncs->fHandleEvent =		CreateUPP(Dispatch_HandleEvent, sDispatch_HandleEvent);

	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasNotification)
		oNPPluginFuncs->fURLNotify = 	CreateUPP(Dispatch_URLNotify, sDispatch_URLNotify);

//	if (navigatorMinorVersion >= ZNSPlugin::eFeature_HasLiveConnect)
//		oNPPluginFuncs->fJavaClass = 	sDispatch_GetJavaClass();

	oDispatch_Shutdown_UPP = CreateUPP(Dispatch_Shutdown, sDispatch_Shutdown);

	#if ZCONFIG(OS, MacOS7)
		sSetUpQD();
	#endif

	try
		{
		ZNSPluginMeister::sCreate();
		}
	catch (...)
		{
		return ZNSPlugin::eError_Generic;
		}

	return ZNSPlugin::eError_None;
	}
#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

#if ZCONFIG(OS, MacOS7)
static QDGlobals* sQDPtr;	// Pointer to Netscape's QuickDraw globals
static short sResFile;		// Refnum of the plugin's resource file
static void sSetUpQD()
	{
	OSErr result = noErr;

	// Memorize the plugin's resource file refnum for later use.
	sResFile = CurResFile();

	Str63 appName;
	FSSpec appFSSpec;

	long response;
	if (noErr == ::Gestalt(gestaltCFMAttr, &response) && (response & (1 << gestaltCFMPresent)))
		{
		// GetProcessInformation takes a process serial number and 
		// will give us back the name and FSSpec of the application.
		// See the Process Manager in IM.
		ProcessInfoRec infoRec;
		infoRec.processInfoLength = sizeof(ProcessInfoRec);
		infoRec.processName = appName;
		infoRec.processAppSpec = &appFSSpec;

		ProcessSerialNumber thePSN;
		thePSN.highLongOfPSN = 0;
		thePSN.lowLongOfPSN = kCurrentProcess;

		result = ::GetProcessInformation(&thePSN, &infoRec);
		if (result != noErr)
			{}//PLUGINDEBUGSTR("\pFailed in GetProcessInformation");
		}
	else
		{
		// If no CFM installed, assume it must be a 68K app.
		result = -1;
		}

	CFragConnectionID connID;
	if (result == noErr)
		{
		// Now that we know the app name and FSSpec, we can call GetDiskFragment
		// to get a connID to use in a subsequent call to FindSymbol (it will also
		// return the address of "main" in app, which we ignore). If GetDiskFragment 
		// returns an error, we assume the app must be 68K.
		Ptr mainAddr; 	
		Str255 errName;
		result = ::GetDiskFragment(&appFSSpec, 0L, 0L, appName, kReferenceCFrag, &connID, &mainAddr, errName);
		}

	if (result == noErr) 
		{
		// The app is a PPC code fragment, so call FindSymbol to get the exported
		// "qd" symbol so we can access its QuickDraw globals.
		CFragSymbolClass symClass;
		result = ::FindSymbol(connID, "\pqd", (Ptr*)&sQDPtr, &symClass);
		if (result != noErr)
			{}//PLUGINDEBUGSTR("\pFailed in FindSymbol qd");
		}
	else
		{
		// The app is 68K, so use its A5 to compute the address of its QuickDraw globals.
		sQDPtr = (QDGlobals*)(*((long*)SetCurrentA5()) - (sizeof(QDGlobals) - sizeof(GrafPtr)));
		}
	}
#endif // ZCONFIG(OS, MacOS7)

// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginInstance

ZNSPluginInstance::ZNSPluginInstance(NPP iNPP)
:	fNPP(iNPP)
	{}

ZNSPluginInstance::~ZNSPluginInstance()
	{}

ZNSPlugin::EError ZNSPluginInstance::Destroy(NPSavedData*& oSavedData)
	{
	delete this;
	return eError_None;
	}

ZNSPlugin::EError ZNSPluginInstance::NewStream(const char* iMIMEType, NPStream* iStream, NPBool iSeekable, EStreamType& oType)
	{
	return eError_None;
	}

ZNSPlugin::EError ZNSPluginInstance::DestroyStream(NPStream* iStream, EReason iReason)
	{
	return eError_None;
	}

void ZNSPluginInstance::StreamAsFile(NPStream* iStream, const char* iFilePath)
	{
	}

int32 ZNSPluginInstance::WriteReady(NPStream* iStream)
	{
	return 0x7FFFFFFF;
	}

int32 ZNSPluginInstance::Write(NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer)
	{
	return iLen;
	}

void ZNSPluginInstance::Print(NPPrint& iPlatformPrint)
	{
	}

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
int16 ZNSPluginInstance::HandleEvent(const EventRecord& iEvent)
	{
	return false;
	}
#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)

void ZNSPluginInstance::URLNotify(const char* iURL, EReason iReason, void* iRefCon)
	{
	}

ZNSPlugin::EError ZNSPluginInstance::GetValue(void* iInstance, NPPVariable variable, void* oValue)
	{
	return eError_Generic;
	}

ZNSPlugin::EError ZNSPluginInstance::SetValue(void* iInstance, NPNVariable variable, void* iValue)
	{
	return eError_Generic;
	}


// =================================================================================================
#pragma mark -
#pragma mark * ZNSPluginMeister

ZNSPluginMeister::ZNSPluginMeister()
	{
	ZAssertStop(kDebug_NSPlugin, !sMeister);
	sMeister = this;
	}

ZNSPluginMeister::~ZNSPluginMeister()
	{
	ZAssertStop(kDebug_NSPlugin, sMeister == this);
	sMeister = nil;
	}

void ZNSPluginMeister::Shutdown()
	{
	delete this;
	}

ZNSPlugin::EError ZNSPluginMeister::New(const char* iMIMEType, NPP iNPP, EMode iMode, int16 iArgc, const char* iArgn[], const char* iArgv[], NPSavedData* iSavedData)
	{
	ZNSPluginInstance* theInstance;
	this->New(iNPP, iMIMEType, iMode, iArgc, iArgn, iArgv, iSavedData, theInstance);
	if (theInstance)
		{
		iNPP->fData_Plugin = theInstance;
		return eError_None;
		}
	return eError_Generic;
	}

ZNSPlugin::EError ZNSPluginMeister::Destroy(NPP iNPP, NPSavedData*& oSavedData)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->Destroy(oSavedData);
	return eError_Generic;
	}

ZNSPlugin::EError ZNSPluginMeister::SetWindow(NPP iNPP, NPWindow* iNPWindow)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->SetWindow(iNPWindow);
	return eError_Generic;
	}

ZNSPlugin::EError ZNSPluginMeister::NewStream(NPP iNPP, const char* iMIMEType, NPStream* iStream, NPBool iSeekable, EStreamType& oType)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->NewStream(iMIMEType, iStream, iSeekable, oType);
	return eError_Generic;
	}

ZNSPlugin::EError ZNSPluginMeister::DestroyStream(NPP iNPP, NPStream* iStream, EReason iReason)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->DestroyStream(iStream, iReason);
	return eError_Generic;
	}

void ZNSPluginMeister::StreamAsFile(NPP iNPP, NPStream* iStream, const char* iFilePath)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		theInstance->StreamAsFile(iStream, iFilePath);
	}

int32 ZNSPluginMeister::WriteReady(NPP iNPP, NPStream* iStream)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->WriteReady(iStream);
	return 0x7FFFFFFF;
	}

int32 ZNSPluginMeister::Write(NPP iNPP, NPStream* iStream, int32 iOffset, int32 iLen, const void* iBuffer)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->Write(iStream, iOffset, iLen, iBuffer);
	return iLen;
	}

void ZNSPluginMeister::Print(NPP iNPP, NPPrint& iPlatformPrint)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->Print(iPlatformPrint);
	}

int16 ZNSPluginMeister::HandleEvent(NPP iNPP, void* iEvent)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		{
		#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
			return theInstance->HandleEvent(*static_cast<const EventRecord*>(iEvent));
		#endif // ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
		}
	return 0;
	}

void ZNSPluginMeister::URLNotify(NPP iNPP, const char* iURL, EReason iReason, void* iRefCon)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->URLNotify(iURL, iReason, iRefCon);
	}

ZNSPlugin::EError ZNSPluginMeister::GetValue(NPP iNPP, void* iInstance, NPPVariable variable, void* oValue)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->GetValue(iInstance, variable, oValue);
	return eError_Generic;	
	}

ZNSPlugin::EError ZNSPluginMeister::SetValue(NPP iNPP, void* iInstance, NPNVariable variable, void* iValue)
	{
	if (ZNSPluginInstance* theInstance = this->NPPToInstance(iNPP))
		return theInstance->SetValue(iInstance, variable, iValue);
	return eError_Generic;	
	}

ZNSPluginInstance* ZNSPluginMeister::NPPToInstance(NPP iNPP)
	{
	if (iNPP)
		return static_cast<ZNSPluginInstance*>(iNPP->fData_Plugin);
	return nil;
	}
