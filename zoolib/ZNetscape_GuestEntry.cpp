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

#include "zoolib/ZNetscape_GuestEntry.h"
#include "zoolib/ZNetscape_Guest.h"
#include "zoolib/ZUtil_MacOSX.h"

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Netscape)

using ZooLib::ZNetscape::NPNetscapeFuncs_Z;

extern "C" {

#if __MACH__
#	pragma export on
#endif

NPError NP_Initialize(NPNetscapeFuncs_Z*);
NPError NP_Initialize(NPNetscapeFuncs_Z* iBrowserFuncs)
	{ return ZOOLIB_PREFIX::ZNetscape::GuestMeister::sGet()->Initialize(iBrowserFuncs); }

NPError NP_GetEntryPoints(NPPluginFuncs*);
NPError NP_GetEntryPoints(NPPluginFuncs* oPluginFuncs)
	{ return ZOOLIB_PREFIX::ZNetscape::GuestMeister::sGet()->GetEntryPoints(oPluginFuncs); }

NPError NP_Shutdown();
NPError NP_Shutdown()
	{ return ZOOLIB_PREFIX::ZNetscape::GuestMeister::sGet()->Shutdown(); }

#if __MACH__
#	pragma export off
#endif

// For compatibility with CFM and Mozilla-type browsers
#pragma export on

int main(NPNetscapeFuncs_Z*, NPPluginFuncs*, NPP_ShutdownProcPtr*);
int main(NPNetscapeFuncs_Z* iNPNF, NPPluginFuncs* oPluginFuncs, NPP_ShutdownProcPtr* oShutdownFunc)
	{
	// This function is called by CFM browsers, and also by Mozilla-based code.
	// On Intel the function pointers are just regular function pointers, on PPC they
	// will be tvectors.

	NPError result;

	#if __MACH__ && ZCONFIG(Processor, PPC)

		// We're MachO on PPC, but main has been called. We have to assume that
		// the caller is expecting CFM function pointers.

		// Take a local copy of the passed-in table, no bigger than what was
		// passed, and no bigger than what we're expecting.
		NPNetscapeFuncs_Z localNPNF;
		ZBlockZero(&localNPNF, sizeof(localNPNF));
		ZBlockCopy(iNPNF, &localNPNF, min(size_t(iNPNF->size), sizeof(localNPNF)));

		// Rewrite them as CFM-callable thunks.
		ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
			&localNPNF.geturl,
			(localNPNF.size - offsetof(NPNetscapeFuncs_Z, geturl)) / sizeof(void*),
			fGlue_NPNF);

		// And pass the munged local structure to NP_Initialize.
		result = NP_Initialize(&localNPNF);

		// Get our plugin's function pointers
		NP_GetEntryPoints(oPluginFuncs);

		// And munge them into MachO-callable thunks.
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
			&oPluginFuncs->newp,
			(oPluginFuncs->size - offsetof(NPPluginFuncs, newp)) / sizeof(void*),
			fGlue_PluginFuncs);

		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown; 	
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(&oShutdownFunc, 1, fGlue_Shutdown);
		
	#else

		// We're something else, so raw function pointers are OK.
		result = NP_Initialize(iNPNF);
		NP_GetEntryPoints(oPluginFuncs);
		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown; 	

	#endif

	return result;
	}

#pragma export off

} // extern "C"

#endif // ZCONFIG_SPI_Enabled(Netscape)
