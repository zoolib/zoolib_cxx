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

#include "zoolib/netscape/ZNetscape_GuestFactory.h"

#include "zoolib/ZCompat_Win.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZUtil_MacOSX.h"
#include "zoolib/ZUtil_Win.h"
#include "zoolib/ZVal_CF.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)
	#include ZMACINCLUDE2(CoreFoundation,CFBundle.h)
	#include ZMACINCLUDE2(CoreFoundation,CFPlugin.h)
	#if __MACH__
		#include <mach-o/dyld.h> // For NSModule
	#endif
#endif

#if __MACH__
	#include <csignal>
#endif

#include <stdexcept>
#include <vector>

namespace ZooLib {

using std::runtime_error;
using std::vector;

using ZNetscape::NPNetscapeFuncs_Z;

// =================================================================================================
// MARK: - Helper functions

static void spThrowMissingEntryPoint()
	{
	throw runtime_error("Missing entry point");
	}

// =================================================================================================
// MARK: - Helper functions, Win

#if ZCONFIG_SPI_Enabled(Win)

template <typename P>
P spLookup(HMODULE iHMODULE, const char* iName)
	{ return reinterpret_cast<P>(::GetProcAddress(iHMODULE, iName)); }

static ZQ<int> spQGetMajorVersion(const UTF16* iNativePath)
	{
	if (ZQ<uint64> theQ = ZUtil_Win::sQGetVersion_File(iNativePath))
		return (*theQ) >> 48;
	return null;
	}

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
// MARK: - Helper functions, Mach

#if ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

static NSModule spLoadNSModule(CFBundleRef iBundleRef)
	{
	// FIXME
	// On 10.3+ we should use dlopen with RTLD_LOCAL.
	if (ZRef<CFURLRef> executableURL = sAdopt& ::CFBundleCopyExecutableURL(iBundleRef))
		{
		char buff[PATH_MAX];

		if (::CFURLGetFileSystemRepresentation(executableURL, true, (UInt8*)buff, PATH_MAX))
			{
			NSObjectFileImage image;
			if (NSObjectFileImageSuccess == ::NSCreateObjectFileImageFromFile(buff, &image))
				{
				NSModule result = ::NSLinkModule(
					image, buff,
					NSLINKMODULE_OPTION_BINDNOW
					| NSLINKMODULE_OPTION_PRIVATE
					| NSLINKMODULE_OPTION_RETURN_ON_ERROR);
				::NSDestroyObjectFileImage(image);
				return result;
				}
			}
		}
	return nullptr;
	}

template <typename P>
P spLookup(NSModule iNSModule, const char* iName)
	{
	if (NSSymbol theSymbol = ::NSLookupSymbolInModule(iNSModule, iName))
		return reinterpret_cast<P>(::NSAddressOfSymbol(theSymbol));
	return nullptr;
	}

#endif // ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

// =================================================================================================
// MARK: - Helper functions, CoreFoundation

#if ZCONFIG_SPI_Enabled(CoreFoundation)

template <typename P>
P spLookup(CFBundleRef iBundleRef, CFStringRef iName)
	{ return reinterpret_cast<P>(::CFBundleGetFunctionPointerForName(iBundleRef, iName)); }

ZQ<int> spQGetMajorVersion(const ZRef<CFStringRef>& iStringRef)
	{
	if (iStringRef)
		{
		if (SInt32 theVal = ::CFStringGetIntValue(iStringRef))
			return theVal;
		}
	return null;
	}

// Note that the use of CFBundleShortVersionString here is because it's where Flash
// reliably puts a usable major version number.

ZQ<int> spQGetMajorVersion(const ZMap_CF& iMap)
	{ return spQGetMajorVersion(iMap.Get(CFSTR("CFBundleShortVersionString")).GetCFString()); }

ZQ<int> spQGetMajorVersion(const ZRef<CFBundleRef>& iBundleRef)
	{
	if (iBundleRef)
		{
		if (ZRef<CFStringRef> theStringRef =(CFStringRef)::CFBundleGetValueForInfoDictionaryKey(
			iBundleRef, CFSTR("CFBundleShortVersionString")))
			{ return spQGetMajorVersion(theStringRef); }
		}
	return null;
	}

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)

// =================================================================================================
// MARK: - GuestFactory_Win

#if ZCONFIG_SPI_Enabled(Win)

class GuestFactory_Win : public ZNetscape::GuestFactory
	{
public:
	GuestFactory_Win(HMODULE iHMODULE);
	virtual ~GuestFactory_Win();

	virtual const NPPluginFuncs& GetEntryPoints();

	virtual ZQ<int> QGetMajorVersion();

private:
	HMODULE fHMODULE;
	NPNetscapeFuncs_Z fNPNF;
	NPPluginFuncs fNPPluginFuncs;
	NPP_ShutdownProcPtr fShutdown;
	};

typedef ZNetscape_API_EXPORTED_CALLBACK(NPError, Flash_DisableLocalSecurityFuncPtr)(void);

GuestFactory_Win::GuestFactory_Win(HMODULE iHMODULE)
:	fHMODULE(iHMODULE)
	{
	// Get our own copy of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	ZMemZero_T(fNPPluginFuncs);
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	fShutdown = spLookup<NPP_ShutdownProcPtr>(fHMODULE, "NP_Shutdown");

	NP_InitializeFuncPtr theInit =
		spLookup<NP_InitializeFuncPtr>(fHMODULE, "NP_Initialize");

	NP_GetEntryPointsFuncPtr theEntryPoints =
		spLookup<NP_GetEntryPointsFuncPtr>(fHMODULE, "NP_GetEntryPoints");

	if (!fShutdown || !theInit || !theEntryPoints)
		spThrowMissingEntryPoint();

	// Windows Flash 10.1 requires theEntryPoints be called first.
	theEntryPoints(&fNPPluginFuncs);

	if (Flash_DisableLocalSecurityFuncPtr theDLS =
		spLookup<Flash_DisableLocalSecurityFuncPtr>(fHMODULE, "Flash_DisableLocalSecurity"))
		{
		theDLS();
		}

	theInit(&fNPNF);
	}

GuestFactory_Win::~GuestFactory_Win()
	{
	fShutdown();
	::FreeLibrary(fHMODULE);
	}

const NPPluginFuncs& GuestFactory_Win::GetEntryPoints()
	{ return fNPPluginFuncs; }

ZQ<int> GuestFactory_Win::QGetMajorVersion()
	{
	vector<UTF16> buffer(1024);
	if (0 < ::GetModuleFileNameW(::GetModuleHandleW(nullptr), &buffer[0], buffer.size()))
		return spQGetMajorVersion(&buffer[0]);
	return null;
	}

#endif // ZCONFIG_SPI_Enabled(Win)

// =================================================================================================
// MARK: - GuestFactory_HostMachO

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

	virtual ZQ<int> QGetMajorVersion();

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

static void spIgnoreSignal(int iSig)
	{}

GuestFactory_HostMachO::GuestFactory_HostMachO(ZRef<CFPlugInRef> iPlugInRef)
:	fPlugInRef(iPlugInRef),
	fNSModule(nullptr)
	{
	// If the plugin contains ObjC code then unloading it will kill the
	// host application. So (for now at least) we do an extra retain, leaving
	// the rest of the plugin management as it should be.
	//##::CFRetain(fPlugInRef);

	// Get our own copies of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	sMemZero_T(fNPPluginFuncs);
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	ZRef<CFBundleRef> theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	// Force the bundle to be physically loaded by asking for an entry point. If we
	// don't do this, and unload the bundle, then any subsequent loader will
	// get nils for any entry point. In Safari this manifests with a
	// "Internal error unloading bundle" log message.
	// This also tells us that the NP_Initialize entry point exists, so it's
	// a macho binary (CFM would only provide 'main'), and we should use NSModule
	// to load our independent instantiation.

	if (::CFBundleGetFunctionPointerForName(theBundleRef, CFSTR("NP_Initialize")))
		{
		fNSModule = spLoadNSModule(theBundleRef);

		fShutdown = spLookup<NPP_ShutdownProcPtr>(fNSModule, "_NP_Shutdown");

		NP_GetEntryPointsFuncPtr theEntryPoints =
			spLookup<NP_GetEntryPointsFuncPtr>(fNSModule, "_NP_GetEntryPoints");

		NP_InitializeFuncPtr theInit =
			spLookup<NP_InitializeFuncPtr>(fNSModule, "_NP_Initialize");

		if (!fShutdown || !theInit || !theEntryPoints)
			spThrowMissingEntryPoint();

		// Mac Flash 10.1 requires theInit be called first. cf GuestFactory_Win.
		// Also, Flash tends to call Debugger, which is usually innocuous, but under
		// some circumstances causes the app to crash with an unhandled SIGTRAP, so
		// we catch and ignore it.
		{
		sig_t prior = ::signal(SIGTRAP, spIgnoreSignal);
		theInit(&fNPNF);
		::signal(SIGTRAP, prior);
		}

		theEntryPoints(&fNPPluginFuncs);
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

		// Call main, which itself will be a CFM function, but the bundle function
		// lookup mechanism will have created a MachO-callable thunk for it.

		MainFuncPtr theMain =
			spLookup<MainFuncPtr>(theBundleRef, CFSTR("main"));

		if (!theMain)
			spThrowMissingEntryPoint();

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

ZQ<int> GuestFactory_HostMachO::QGetMajorVersion()
	{ return spQGetMajorVersion(fPlugInRef); }

#endif // ZCONFIG_SPI_Enabled(CoreFoundation) && __MACH__

// =================================================================================================
// MARK: - GuestFactory_HostCFM

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
	//##::CFRetain(fPlugInRef);

	// Get local copies of our host's function pointers
	GuestFactory::GetNPNF(fNPNF);

	// And clean out our plugin functions struct
	ZMemZero_T(fNPPluginFuncs);
	fNPPluginFuncs.size = sizeof(NPPluginFuncs);

	CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	// We're PowerPC -- look for main(), and if it's there we can just call it.
	MainFuncPtr theMain = spLookup<MainFuncPtr>(theBundleRef, CFSTR("main"));

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

		fShutdown = spLookup<NPP_ShutdownProcPtr>(theBundleRef, CFSTR("NP_Shutdown"));

		NP_GetEntryPointsFuncPtr theEntryPoints =
			spLookup<NP_GetEntryPointsFuncPtr>(theBundleRef, CFSTR("NP_GetEntryPoints"));

		NP_InitializeFuncPtr theInit =
			spLookup<NP_InitializeFuncPtr>(theBundleRef, CFSTR("NP_Initialize"));

		if (!fShutdown || !theInit || !theEntryPoints)
			spThrowMissingEntryPoint();

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
// MARK: - ZNetscape

ZRef<ZNetscape::GuestFactory> ZNetscape::sMakeGuestFactory(
	ZQ<int> iEarliest, ZQ<int> iLatest, const std::string& iNativePath)
	{
	try
		{
		#if ZCONFIG_SPI_Enabled(Win)
			if (iEarliest || iLatest)
				{
				if (ZQ<int,false> theQ = spQGetMajorVersion(ZUnicode::sAsUTF16(iNativePath).c_str()))
					{ return null; }
				else
					{
					if (iEarliest && *iEarliest > *theQ)
						{ return null; }
					else if (iLatest && *iLatest < *theQ)
						{ return null; }
					}
				}

			if (HMODULE theHMODULE = ::LoadLibraryW(ZUnicode::sAsUTF16(iNativePath).c_str()))
				{
				try { return new GuestFactory_Win(theHMODULE); }
				catch (...) { ::FreeLibrary(theHMODULE); }
				}
		#endif

		#if ZCONFIG_SPI_Enabled(CoreFoundation)
			if (ZRef<CFURLRef> theURL = sAdopt& ::CFURLCreateWithFileSystemPath(
				nullptr, ZUtil_CF::sString(iNativePath), kCFURLPOSIXPathStyle, true))
				{
				if (ZMap_CF theMap = sAdopt& ::CFBundleCopyInfoDictionaryInDirectory(theURL))
					{
					if (ZQ<int,false> theQ = spQGetMajorVersion(theMap))
						{ return null; }
					else if (iEarliest && *iEarliest > *theQ)
						{ return null; }
					else if (iLatest && *iLatest < *theQ)
						{ return null; }
					}
						
				if (ZRef<CFBundleRef> theBundleRef = sAdopt& ::CFBundleCreate(nullptr, theURL))
					{
					#if __MACH__
						return new GuestFactory_HostMachO(theBundleRef);
					#elif ZCONFIG(Processor,PPC)
						return new GuestFactory_HostCFM(theBundleRef);
					#endif
					}
				}
		#endif
		}
	catch (...)
		{}

	return null;
	}

ZRef<ZNetscape::GuestFactory> ZNetscape::sMakeGuestFactory(const std::string& iNativePath)
	{ return sMakeGuestFactory(null, null, iNativePath); }

} // namespace ZooLib
