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
#pragma mark * GuestFactory_MacPlugin

GuestFactory_MacPlugin::GuestFactory_MacPlugin(const std::string& iPath)
:	fPlugInRef(nil)
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
				if (ZLOG(s, eDebug, "GuestFactory_MacPlugin"))
					{
					s.Writef("ctor, fPlugInRef refcount = %d",
						::CFGetRetainCount(fPlugInRef));
					}

				CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);
				NPNetscapeFuncs theNPNetscapeFuncs;
				this->GetNPNetscapeFuncs(theNPNetscapeFuncs);
				
				NP_InitializeFuncPtr theInitialize = (NP_InitializeFuncPtr)
					::CFBundleGetFunctionPointerForName(
					theBundleRef, CFSTR("NP_Initialize"));

				theInitialize(&theNPNetscapeFuncs);
				}
			}
		}

	if (!fPlugInRef)
		throw std::runtime_error("Couldn't load plugin");
	}

GuestFactory_MacPlugin::~GuestFactory_MacPlugin()
	{
	CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	NPP_ShutdownProcPtr theShutdown =
		(NPP_ShutdownProcPtr)
		::CFBundleGetFunctionPointerForName(
		theBundleRef, CFSTR("NP_Shutdown"));

	theShutdown();

	if (ZLOG(s, eDebug, "GuestFactory_MacPlugin"))
		{
		s.Writef("dtor, fPlugInRef refcount = %d",
			::CFGetRetainCount(fPlugInRef));
		}

	::CFRelease(fPlugInRef);
	}

void GuestFactory_MacPlugin::GetEntryPoints(NPPluginFuncs& oNPPluginFuncs)
	{
	CFBundleRef theBundleRef = ::CFPlugInGetBundle(fPlugInRef);

	NP_GetEntryPointsFuncPtr theGetEntryPoints = (NP_GetEntryPointsFuncPtr)
		::CFBundleGetFunctionPointerForName(
		theBundleRef, CFSTR("NP_GetEntryPoints"));

	theGetEntryPoints(&oNPPluginFuncs);
	}

} // namespace ZNetscape

#endif // ZCONFIG_SPI_Enabled(MacOSX)
