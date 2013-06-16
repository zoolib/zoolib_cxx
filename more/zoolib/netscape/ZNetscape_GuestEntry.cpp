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

#include "zoolib/ZMemory.h"
#include "zoolib/netscape/ZNetscape_Guest.h"
#include "zoolib/netscape/ZNetscape_GuestEntry.h"
#include "zoolib/ZUtil_MacOSX.h"

using namespace ZooLib;
using ZNetscape::NPNetscapeFuncs_Z;

using std::min;
using std::vector;

// =================================================================================================

#if defined(XP_WIN)
	#define EXPORT_DECL(ret) extern "C" __declspec(dllexport) ret __stdcall
	#define EXPORT_DEF(ret) ret __stdcall
#else
	#define EXPORT_DECL(ret) extern "C" __attribute__((visibility("default"))) ret
	#define EXPORT_DEF(ret) ret
#endif

#if !ZCONFIG(Processor, PPC) || !__MACH__
	// We're not building for CFM, so these entry points are exported.
	EXPORT_DECL(NPError) NP_GetEntryPoints(NPPluginFuncs*);
	EXPORT_DECL(NPError) NP_Initialize(NPNetscapeFuncs_Z*);
	EXPORT_DECL(NPError) NP_Shutdown();
#endif

EXPORT_DEF(NPError) NP_GetEntryPoints(NPPluginFuncs* oPluginFuncs)
	{ return ZNetscape::GuestMeister::sGet()->GetEntryPoints(oPluginFuncs); }

EXPORT_DEF(NPError) NP_Initialize(NPNetscapeFuncs_Z* iBrowserFuncs)
	{ return ZNetscape::GuestMeister::sGet()->Initialize(iBrowserFuncs); }

EXPORT_DEF(NPError) NP_Shutdown()
	{ return ZNetscape::GuestMeister::sGet()->Shutdown(); }

EXPORT_DECL(int) main(NPNetscapeFuncs_Z*, NPPluginFuncs*, NPP_ShutdownProcPtr*);

EXPORT_DEF(int) main(
	NPNetscapeFuncs_Z* iNPNF, NPPluginFuncs* oPluginFuncs, NPP_ShutdownProcPtr* oShutdownFunc)
	{
	// This function is called by CFM browsers, and also by Mozilla-based code.
	// On Intel the function pointers are just regular function pointers, on PPC they
	// will be tvectors.

	NPError result;

	#if __MACH__ && ZCONFIG(Processor, PPC)

		static vector<char> spGlue_NPNF;
		static vector<char> spGlue_PluginFuncs;
		static vector<char> spGlue_Shutdown;
		// We're MachO on PPC, but main has been called. We have to assume that
		// the caller is expecting CFM function pointers.

		// Take a local copy of the passed-in table, no bigger than what was
		// passed, and no bigger than what we're expecting.
		NPNetscapeFuncs_Z localNPNF;
		ZMemZero_T(localNPNF);
		ZMemCopy(&localNPNF, iNPNF, min(size_t(iNPNF->size), sizeof(localNPNF)));

		// Rewrite them as CFM-callable thunks.
		ZUtil_MacOSX::sCreateThunks_MachOCalledByCFM(
			&localNPNF.geturl,
			(localNPNF.size - offsetof(NPNetscapeFuncs, geturl)) / sizeof(void*),
			spGlue_NPNF);

		// And pass the munged local structure to NP_Initialize.
		result = NP_Initialize(&localNPNF);

		// Get our plugin's function pointers
		NP_GetEntryPoints(oPluginFuncs);

		// And munge them into MachO-callable thunks.
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(
			&oPluginFuncs->newp,
			(oPluginFuncs->size - offsetof(NPPluginFuncs, newp)) / sizeof(void*),
			spGlue_PluginFuncs);

		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown;
		ZUtil_MacOSX::sCreateThunks_CFMCalledByMachO(&oShutdownFunc, 1, spGlue_Shutdown);

	#else

		// We're something else, so raw function pointers are OK.
		result = NP_Initialize(iNPNF);
		NP_GetEntryPoints(oPluginFuncs);
		*oShutdownFunc = (NPP_ShutdownProcPtr)NP_Shutdown;

	#endif

	return result;
	}
