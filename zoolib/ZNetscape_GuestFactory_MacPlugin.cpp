/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green
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

#include "zoolib/ZNetscape_GuestFactory_MacPlugin.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static void* sLookup(NSModule iModule, const char* iName)
	{
	if (NSSymbol theSymbol = ::NSLookupSymbolInModule(iModule, iName))
		return ::NSAddressOfSymbol(theSymbol);
	return nil;
	}

static NSModule sLoadNSModule(CFBundleRef iBundleRef)
	{
	NSModule module = nil;

	if (CFURLRef executableURL = ::CFBundleCopyExecutableURL(iBundleRef))
		{
		char buff[PATH_MAX];

		if (::CFURLGetFileSystemRepresentation(executableURL, true, (UInt8*)buff, PATH_MAX))
			{
			NSObjectFileImage image;
			if (NSObjectFileImageSuccess == ::NSCreateObjectFileImageFromFile(buff, &image))
				{
				module = ::NSLinkModule(
					image, buff,
					NSLINKMODULE_OPTION_BINDNOW
					| NSLINKMODULE_OPTION_PRIVATE
					| NSLINKMODULE_OPTION_RETURN_ON_ERROR);
				}
			}
		::CFRelease(executableURL);
		}

	return module;
	}

template <typename P>
P sInvoke_T(NSModule iNSModule, const char* iName)
	{
	return reinterpret_cast<P>(sLookup(iNSModule, iName));
	}

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory_MacPlugin

GuestFactory_MacPlugin::GuestFactory_MacPlugin(const std::string& iPath)
:	fPlugInRef(nil),
	fNSModule(nil)
	{
	CFStringRef thePath = ::CFStringCreateWithCString(nil, iPath.c_str(), kCFStringEncodingUTF8);
	if (thePath)
		{
		CFURLRef theURL = ::CFURLCreateWithFileSystemPath(
			nil, thePath, kCFURLPOSIXPathStyle, true);
		::CFRelease(thePath);
		if (theURL)
			{
			fPlugInRef = ::CFPlugInCreate(nil, theURL);
			if (fPlugInRef)
				{
				CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);
				// Force the bundle to be physically loaded by asking for an entry point. If we
				// don't do this, and unload the bundle, then any subsequent loader will
				// get nils for any entry point. In Safari this manifests with a
				// "Internal error unloading bundle" log message.
				::CFBundleGetFunctionPointerForName(theBundleRef, CFSTR("NP_Initialize"));

				fNSModule = sLoadNSModule(theBundleRef);

				NPNetscapeFuncs theNPNetscapeFuncs;
				this->GetNPNetscapeFuncs(theNPNetscapeFuncs);

				sInvoke_T<NP_InitializeFuncPtr>
					(fNSModule, "_NP_Initialize")
					(&theNPNetscapeFuncs);
				}
			}
		}

	if (!fPlugInRef)
		throw std::runtime_error("Couldn't load plugin");
	}

GuestFactory_MacPlugin::~GuestFactory_MacPlugin()
	{
	sInvoke_T<NPP_ShutdownProcPtr>
		(fNSModule, "_NP_Shutdown")();

	::NSUnLinkModule(fNSModule, NSUNLINKMODULE_OPTION_NONE);
	::CFRelease(fPlugInRef);
	}

void GuestFactory_MacPlugin::GetEntryPoints(NPPluginFuncs& oNPPluginFuncs)
	{
	sInvoke_T<NP_GetEntryPointsFuncPtr>
		(fNSModule, "_NP_GetEntryPoints")
		(&oNPPluginFuncs);
	}

} // namespace ZNetscape

#endif // ZCONFIG_SPI_Enabled(MacOSX)
