/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/netscape/ZNetscape.h"

#include "zoolib/ZStringf.h"

#include <cstring>

using std::string;
using std::strncpy;

namespace ZooLib {
namespace ZNetscape {

// =================================================================================================
// MARK: - NPClass_Z

#define ASSIGN(a) this->a = a

NPClass_Z::NPClass_Z(
	NPAllocateFunctionPtr allocate,
	NPDeallocateFunctionPtr deallocate,
	NPInvalidateFunctionPtr invalidate,
	NPHasMethodFunctionPtr hasMethod,
	NPInvokeFunctionPtr invoke,
	NPInvokeDefaultFunctionPtr invokeDefault,
	NPHasPropertyFunctionPtr hasProperty,
	NPGetPropertyFunctionPtr getProperty,
	NPSetPropertyFunctionPtr setProperty,
	NPRemovePropertyFunctionPtr removeProperty,
	NPEnumerationFunctionPtr enumerate)
	{
	structVersion = 2;
	ASSIGN(allocate);
	ASSIGN(deallocate);
	ASSIGN(invalidate);
	ASSIGN(hasMethod);
	ASSIGN(invoke);
	ASSIGN(invokeDefault);
	ASSIGN(hasProperty);
	ASSIGN(getProperty);
	ASSIGN(setProperty);
	ASSIGN(removeProperty);
	ASSIGN(enumerate);
	}

#undef ASSIGN

NPClass_Z* sGetClass(NPObject* obj)
	{
	if (obj)
		return static_cast<NPClass_Z*>(obj->_class);
	return nullptr;
	}

// =================================================================================================
// MARK: - sAsString

string sAsString(const NPString& iNPString)
	{ return string(sNPStringCharsConst(iNPString), sNPStringLengthConst(iNPString)); }

#define CASE(a) case a: return #a

string sAsString(NPNVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPNVxDisplay);
		CASE(NPNVxtAppContext);
		CASE(NPNVnetscapeWindow);
		CASE(NPNVjavascriptEnabledBool);
		CASE(NPNVasdEnabledBool);
		CASE(NPNVisOfflineBool);
		CASE(NPNVserviceManager);
		CASE(NPNVDOMElement);
		CASE(NPNVDOMWindow);
		CASE(NPNVToolkit);
		CASE(NPNVSupportsXEmbedBool);
		CASE(NPNVWindowNPObject);
		CASE(NPNVPluginElementNPObject);
		CASE(NPNVSupportsWindowless);
		CASE(NPNVprivateModeBool);
		#if defined(XP_MACOSX)
			#ifndef NP_NO_QUICKDRAW
				CASE(NPNVsupportsQuickDrawBool);
			#endif
			#ifndef NP_NO_CARBON
				CASE(NPNVsupportsCarbonBool);
			#endif
			CASE(NPNVsupportsCoreGraphicsBool);
			CASE(NPNVsupportsCoreAnimationBool);
			CASE(NPNVsupportsOpenGLBool);
			CASE(NPNVsupportsCocoaBool);
		#endif
		default:
			break;
		}
	return sStringf("NPNVariable=%d", iVar);
	}

string sAsString(NPPVariable iVar)
	{
	switch (iVar)
		{
		CASE(NPPVpluginNameString);
		CASE(NPPVpluginDescriptionString);
		CASE(NPPVpluginWindowBool);
		CASE(NPPVpluginTransparentBool);
		CASE(NPPVjavaClass);
		CASE(NPPVpluginWindowSize);
		CASE(NPPVpluginTimerInterval);
		CASE(NPPVpluginScriptableInstance);
		CASE(NPPVpluginScriptableIID);
		CASE(NPPVjavascriptPushCallerBool);
		CASE(NPPVpluginKeepLibraryInMemory);
		CASE(NPPVpluginNeedsXEmbed);
		CASE(NPPVpluginScriptableNPObject);
		#if defined(XP_MACOSX)
			CASE(NPPVpluginDrawingModel);
			CASE(NPPVpluginEventModel);
		#endif
		default:
			break;
		}
	return sStringf("NPPVariable=%d", iVar);
	}

#undef CASE

} // namespace ZNetscape
} // namespace ZooLib
