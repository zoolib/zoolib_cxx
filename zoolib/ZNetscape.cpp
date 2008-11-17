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

#include "zoolib/ZNetscape.h"
#include "zoolib/ZString.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZNetscape

namespace ZNetscape {

#define CASE(a) case a: return #a

std::string sAsString(NPNVariable iVar)
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
		// Need to find a cleaner way to handle this portably.
		case 1000: return "NPNVpluginDrawingModel";
		case 2000: return "NPNVsupportsQuickDrawBool";
		case 2001: return "NPNVsupportsCoreGraphicsBool";
		}
	return ZString::sFormat("NPNVariable=%d", iVar);
	}

std::string sAsString(NPPVariable iVar)
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
		}
	return ZString::sFormat("NPPVariable=%d", iVar);
	}

#undef CASE

} // namespace ZNetscape
