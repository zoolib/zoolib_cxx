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

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"

namespace ZNetscape {

// =================================================================================================
#pragma mark -
#pragma mark * GuestFactory_MacPlugin

GuestFactory_MacPlugin::GuestFactory_MacPlugin(const std::string& iPath)
:	fPlugInRef(nil),
	fBundleRef(nil)
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
				fBundleRef = ::CFPlugInGetBundle(fPlugInRef);
				if (fBundleRef)
					{
					NPNetscapeFuncs theNPNetscapeFuncs;
					this->GetNPNetscapeFuncs(theNPNetscapeFuncs);
					
					NP_InitializeFuncPtr theInitialize = (NP_InitializeFuncPtr)
						::CFBundleGetFunctionPointerForName(
						fBundleRef, CFSTR("NP_Initialize"));

					theInitialize(&theNPNetscapeFuncs);
					}
				}
			}
		}
	if (!fPlugInRef)
		throw std::runtime_error("Couldn't load plugin");
	}

GuestFactory_MacPlugin::~GuestFactory_MacPlugin()
	{
	NPP_ShutdownProcPtr theShutdown =
		(NPP_ShutdownProcPtr)
		::CFBundleGetFunctionPointerForName(
		fBundleRef, CFSTR("NP_Shutdown"));
	theShutdown();

	// fBundleRef came from CFPlugInGetBundle, and thus is valid so long
	// as we keep our reference to fPluginRef.

	if (fPlugInRef)
		{
		if (ZLOG(s, eDebug, "GuestFactory_MacPlugin"))
			{
			s.Writef("~GuestFactory_MacPlugin, fPlugInRef refcount = %d",
				::CFGetRetainCount(fPlugInRef));
			}
		::CFRelease(fPlugInRef);
		}
	}

void GuestFactory_MacPlugin::GetEntryPoints(NPPluginFuncs& oNPPluginFuncs)
	{
	NP_GetEntryPointsFuncPtr theGetEntryPoints = (NP_GetEntryPointsFuncPtr)
		::CFBundleGetFunctionPointerForName(
		fBundleRef, CFSTR("NP_GetEntryPoints"));
	theGetEntryPoints(&oNPPluginFuncs);
	}

} // namespace ZNetscape
