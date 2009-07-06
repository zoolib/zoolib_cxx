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

#include "zoolib/ZNetscape_GuestFactory.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_MacOSX.h"

#if ZCONFIG_SPI_Enabled(Win)
	#include "zoolib/ZWinHeader.h"
#endif

#if ZCONFIG_SPI_Enabled(CoreFoundation)
	#include ZMACINCLUDE2(CoreFoundation,CFBundle.h)
	#if __MACH__
	#	include <mach-o/dyld.h> // For NSModule
	#endif
#endif

#include <stdexcept>
#include <vector>

NAMESPACE_ZOOLIB_BEGIN

using std::runtime_error;
using std::vector;

using ZNetscape::NPNetscapeFuncs_Z;

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static void sThrowMissingEntryPoint()
	{
	throw runtime_error("Missing entry point");
	}

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions, Win

#if ZCONFIG_SPI_Enabled(Win)

template <typename P>
P sLookup_T(HMODULE iHMODULE, const char* iName)
	{ return reinterpret_cast<P>(::GetProcAddress(iHMODULE, iName)); }

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions, Mach

#if ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

static NSModule sLoadNSModule(CFBundleRef iBundleRef)
	{
	if (ZRef<CFURLRef> executableURL = ::CFBundleCopyExecutableURL(iBundleRef))
		{
		char buff[PATH_MAX];

		if (::CFURLGetFileSystemRepresentation(executableURL, true, (UInt8*)buff, PATH_MAX))
			{
			NSObjectFileImage image;
			if (NSObjectFileImageSuccess == ::NSCreateObjectFileImageFromFile(buff, &image))
				{
				return ::NSLinkModule(
					image, buff,
					NSLINKMODULE_OPTION_BINDNOW
					| NSLINKMODULE_OPTION_PRIVATE
					| NSLINKMODULE_OPTION_RETURN_ON_ERROR);
				}
			}
		}
	return nullptr;
	}

static void* sLookup(NSModule iModule, const char* iName)
	{
	if (NSSymbol theSymbol = ::NSLookupSymbolInModule(iModule, iName))
		return ::NSAddressOfSymbol(theSymbol);
	return nullptr;
	}

template <typename P>
P sLookup_T(NSModule iNSModule, const char* iName)
	{ return reinterpret_cast<P>(sLookup(iNSModule, iName)); }

#endif // ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions, CoreFoundation

#if ZCONFIG_SPI_Enabled(CoreFoundation)

template <typename P>
P sLookup_T(CFBundleRef iBundleRef, CFStringRef iName)
	{ return reinterpret_cast<P>(::CFBundleGetFunctionPointerForName(iBundleRef, iName)); }

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory_Win

#if ZCONFIG_SPI_Enabled(Win)

class GuestFactory_Win : public ZNetscape::GuestFactory
	{
public:
	GuestFactory_Win(HMODULE iHMODULE);
	virtual ~GuestFactory_Win();

	virtual const NPPluginFuncs& GetEntryPoints();

private:
	HMODULE fHMODULE;
	NPNetscapeFuncs_Z fNPNF;
	NPPluginFuncs fNPPluginFuncs;
	NPP_ShutdownProcPtr fShutdown;
	};

GuestFactory_Win::GuestFactory_Win(HMODULE iHMODULE)
:	fHMODULE(iHMODULE)
	{
	// Get our own copy of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	ZBlockZero(&fNPPluginFuncs, sizeof(fNPPluginFuncs));
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	fShutdown = sLookup_T<NPP_ShutdownProcPtr>(fHMODULE, "NP_Shutdown");
	
	NP_InitializeFuncPtr theInit
		= sLookup_T<NP_InitializeFuncPtr>
		(fHMODULE, "NP_Initialize");

	NP_GetEntryPointsFuncPtr theEntryPoints
		= sLookup_T<NP_GetEntryPointsFuncPtr>
		(fHMODULE, "NP_GetEntryPoints");

	if (!fShutdown || !theInit || !theEntryPoints)
		sThrowMissingEntryPoint();

	NPError theNPError;
	theNPError = theEntryPoints(&fNPPluginFuncs);

	// Hmm -- why am I doing this here? Leave it for now, but it's
	// obviously flash-specific.
	typedef NPError (*NP_DISABLELOCALSECURITY)();
	if (NP_DISABLELOCALSECURITY theDLS
		= sLookup_T<NP_DISABLELOCALSECURITY>(fHMODULE, "Flash_DisableLocalSecurity"))
		{
		theDLS();
		}

	theNPError = theInit(&fNPNF);
	}

GuestFactory_Win::~GuestFactory_Win()
	{
	fShutdown();

	::FreeLibrary(fHMODULE);
	}

const NPPluginFuncs& GuestFactory_Win::GetEntryPoints()
	{
	return fNPPluginFuncs;
	}

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory_HostMachO

#if ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

// We may well be wanting to load the plugin into an environment where it's already been loaded.
// GuestFactory_HostMachO uses the Bundle mechanism to locate the plugin, but uses NSLinkModule
// to load and instantiate an independent copy of the library. In this way the library's static
// reference to NPNetscapeFuncs_Z is independent of any other instantiation of the library.

class GuestFactory_HostMachO : public ZNetscape::GuestFactory
	{
public:
	GuestFactory_HostMachO(ZRef<CFPlugInRef> iPlugInRef);
	virtual ~GuestFactory_HostMachO();

	virtual const NPPluginFuncs& GetEntryPoints();

private:
	ZRef<CFPlugInRef> fPlugInRef;
	NSModule fNSModule;
	NPNetscapeFuncs_Z fNPNF;
	NPPluginFuncs fNPPluginFuncs;
	NPP_ShutdownProcPtr fShutdown;

	#if ZCONFIG(Processor,PPC)
		vector<char> fGlue_NPNF;
		vector<char> fGlue_PluginFuncs;
		vector<char> fGlue_Shutdown;
	#endif
	};

GuestFactory_HostMachO::GuestFactory_HostMachO(ZRef<CFPlugInRef> iPlugInRef)
:	fPlugInRef(iPlugInRef),
	fNSModule(nullptr)
	{
	// If the plugin contains ObjC code then unloading it will kill the
	// host application. So (for now at least) we do an extra retain, leaving
	// the rest of the plugin management as it should be.
	::CFRetain(fPlugInRef.Get());

	// Get our own copies of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	ZBlockZero(&fNPPluginFuncs, sizeof(fNPPluginFuncs));
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	ZRef<CFBundleRef> theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	// Force the bundle to be physically loaded by asking for an entry point. If we
	// don't do this, and unload the bundle, then any subsequent loader will
	// get nils for any entry point. In Safari this manifests with a
	// "Internal error unloading bundle" log message.
	bool isMachOPlugin = ::CFBundleGetFunctionPointerForName(theBundleRef, CFSTR("NP_Initialize"));

	// This also tells us that the NP_Initialize entry point exists, so it's
	// a macho binary (CFM would only provide 'main'), and we should use NSModule
	// to load our independent instantiation.

	if (isMachOPlugin)
		{
		fNSModule = sLoadNSModule(theBundleRef);

		fShutdown = sLookup_T<NPP_ShutdownProcPtr>(fNSModule, "_NP_Shutdown");

		NP_GetEntryPointsFuncPtr theEntryPoints
			= sLookup_T<NP_GetEntryPointsFuncPtr>
			(fNSModule, "_NP_GetEntryPoints");

		NP_InitializeFuncPtr theInit
			= sLookup_T<NP_InitializeFuncPtr>
			(fNSModule, "_NP_Initialize");

		if (!fShutdown || !theInit || !theEntryPoints)
			sThrowMissingEntryPoint();

		theEntryPoints(&fNPPluginFuncs);
		theInit(&fNPNF);
		}
	else
		{
		// There's no NP_Initialize, assume we're dealing with a plugin
		// that's expecting CFM-type behavior. On PPC that means that function
		// pointers we pass to the plugin must be callable by CFM code, although
		// this is MachO, and so we must pass pointers to CFM thunks in fNPNF.

		#if ZCONFIG(Processor,PPC)
			// Rework fNPNF as CFM-callable thunks
			ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
				&fNPNF.geturl,
				(fNPNF.size - offsetof(NPNetscapeFuncs, geturl)) / sizeof(void*),
				fGlue_NPNF);
		#endif

		// Call main, which itself may be a CFM function, but the bundle function
		// lookup mechanism will have created a MachO-callable thunk for it.
		
		MainFuncPtr theMain
			= sLookup_T<MainFuncPtr>
			(theBundleRef, CFSTR("main"));

		if (!theMain)
			sThrowMissingEntryPoint();

		theMain(&fNPNF, &fNPPluginFuncs, &fShutdown);

		#if ZCONFIG(Processor,PPC)
			// Rework fNPPluginFuncs and fShutdown as MachO-Callable thunks.
			ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
				&fNPPluginFuncs.newp,
				(fNPPluginFuncs.size - offsetof(NPPluginFuncs, newp)) / sizeof(void*),
				fGlue_PluginFuncs);

			ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(&fShutdown, 1, fGlue_Shutdown);
		#endif		
		}
	}

GuestFactory_HostMachO::~GuestFactory_HostMachO()
	{
	fShutdown();

	if (fNSModule)
		{
		// We manually loaded our own NSModule, which we need to unload.
		::NSUnLinkModule(fNSModule, NSUNLINKMODULE_OPTION_NONE);
		}
	}

const NPPluginFuncs& GuestFactory_HostMachO::GetEntryPoints()
	{ return fNPPluginFuncs; }

#endif // ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory_HostCFM

#if ZCONFIG_SPI_Enabled(CoreFoundation) && ZCONFIG(Processor,PPC) && !__MACH__

#if !defined(NPP_MainEntryUPP)
	extern "C" {
	typedef NPError (*NP_InitializeFuncPtr)(NPNetscapeFuncs_Z*);
	typedef NPError (*NP_GetEntryPointsFuncPtr)(NPPluginFuncs*);
	typedef void (*NPP_ShutdownProcPtr)();
//	typedef void (*BP_CreatePluginMIMETypesPreferencesFuncPtr)(void);
	typedef NPError (*MainFuncPtr)(NPNetscapeFuncs_Z*, NPPluginFuncs*, NPP_ShutdownProcPtr*);
	} // extern "C"
#endif

class GuestFactory_HostCFM : public ZNetscape::GuestFactory
	{
public:
	GuestFactory_HostCFM(ZRef<CFPlugInRef> iPlugInRef);
	virtual ~GuestFactory_HostCFM();

	virtual const NPPluginFuncs& GetEntryPoints();

private:
	ZRef<CFPlugInRef> fPlugInRef;
	NPNetscapeFuncs_Z fNPNF;
	NPPluginFuncs fNPPluginFuncs;
	NPP_ShutdownProcPtr fShutdown;

	vector<char> fGlue_NPNF;
	vector<char> fGlue_PluginFuncs;
	vector<char> fGlue_Shutdown;
	};

GuestFactory_HostCFM::GuestFactory_HostCFM(ZRef<CFPlugInRef> iPlugInRef)
:	fPlugInRef(iPlugInRef)
	{
	// If the plugin contains ObjC code then unloading it will kill the
	// host application. So (for now at least) we do an extra retain, leaving
	// the rest of the plugin management as it should be.
	::CFRetain(fPlugInRef.Get());

	// Get local copies of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	ZBlockZero(&fNPPluginFuncs, sizeof(fNPPluginFuncs));
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	// We're PowerPC -- look for main(), and if it's there we can just call it.
	MainFuncPtr theMain = sLookup_T<MainFuncPtr>(theBundleRef, CFSTR("main"));

	if (theMain)
		{
		theMain(&fNPNF, &fNPPluginFuncs, &fShutdown);
		}
	else
		{
		// We're running as CFM, but the plugin will be using MachO.
		// Rework fNPNF as MachO-callable thunks.
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
			&fNPNF.geturl,
			(fNPNF.size - offsetof(NPNetscapeFuncs_Z, geturl)) / sizeof(void*),
			fGlue_NPNF);

		fShutdown = sLookup_T<NPP_ShutdownProcPtr>(theBundleRef, CFSTR("NP_Shutdown"));

		NP_GetEntryPointsFuncPtr theEntryPoints
			= sLookup_T<NP_GetEntryPointsFuncPtr>
			(theBundleRef, CFSTR("NP_GetEntryPoints"));

		NP_InitializeFuncPtr theInit
			= sLookup_T<NP_InitializeFuncPtr>
			(theBundleRef, CFSTR("NP_Initialize"));

		if (!fShutdown || !theInit || !theEntryPoints)
			sThrowMissingEntryPoint();

		theEntryPoints(&fNPPluginFuncs);

		// Rework fNPPluginFuncs and fShutDown as CFM-Callable thunks.
		ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
			&fNPPluginFuncs.newp,
			(fNPPluginFuncs.size - offsetof(NPPluginFuncs, newp)) / sizeof(void*),
			fGlue_PluginFuncs);
		ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(&fShutdown, 1, fGlue_Shutdown);

		theInit(&fNPNF);
		}
	}

GuestFactory_HostCFM::~GuestFactory_HostCFM()
	{ fShutdown(); }

const NPPluginFuncs& GuestFactory_HostCFM::GetEntryPoints()
	{ return fNPPluginFuncs; }

#endif // ZCONFIG_SPI_Enabled(CoreFoundation) && ZCONFIG(Processor,PPC) && !__MACH__

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

ZRef<ZNetscape::GuestFactory> ZNetscape::sMakeGuestFactory(const std::string& iPath)
	{
	#if ZCONFIG_SPI_Enabled(Win)
		string16 thePath = ZUnicode::sAsUTF16(iPath);
		if (HMODULE theHMODULE = ::LoadLibraryW(thePath.c_str()))
			{
			try
				{
				return new GuestFactory_Win(theHMODULE);
				}
			catch (...)
				{
				::FreeLibrary(theHMODULE);				
				}
			}
	#endif

	#if ZCONFIG_SPI_Enabled(CoreFoundation)
		if (ZRef<CFStringRef> thePath = Adopt(::CFStringCreateWithCString(
			nullptr, iPath.c_str(), kCFStringEncodingUTF8)))
			{
			if (ZRef<CFURLRef> theURL = Adopt(::CFURLCreateWithFileSystemPath(
				nullptr, thePath, kCFURLPOSIXPathStyle, true)))
				{
				if (ZRef<CFPlugInRef> thePlugInRef = Adopt(::CFPlugInCreate(nullptr, theURL)))
					{
					try
						{
						#if __MACH__
							return new GuestFactory_HostMachO(thePlugInRef);
						#elif ZCONFIG(Processor,PPC)
							return new GuestFactory_HostCFM(thePlugInRef);
						#endif
						}
					catch (...)
						{}
					}
				}
			}
	#endif

	return ZRef<ZNetscape::GuestFactory>();
	}

NAMESPACE_ZOOLIB_END
