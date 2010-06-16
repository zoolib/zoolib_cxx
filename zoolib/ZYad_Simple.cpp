/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_Simple.h"

namespace ZooLib {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void spThrowParseException(const string& iMessage)
	{ throw ZYadParseException_Simple(iMessage); }

bool spRead_Until(const ZStrimU& iStrimU, UTF32 iTerminator, string& oString)
	{
	oString.clear();
	return ZUtil_Strim::sCopy_Until(iStrimU, iTerminator, ZStrimW_String(oString));
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Simple

ZYadParseException_Simple::ZYadParseException_Simple(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_Simple::ZYadParseException_Simple(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Simple

ZYadMapR_Simple::ZYadMapR_Simple(ZRef<ZStrimmerU> iStrimmerU)
:	fStrimmerU(iStrimmerU)
	{}

void ZYadMapR_Simple::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	using namespace ZUtil_Strim;

	const ZStrimU& theStrimU = fStrimmerU->GetStrimU();

	if (iIsFirst)
		{
		UTF32 aCP;
		if (!theStrimU.ReadCP(aCP))
			{
			// couldn't read anything, we're empty.
			return;
			}
		theStrimU.Unread(aCP);
		}
	else
		{
		// We've already returned something
		if (!sTryRead_CP(theStrimU, ','))
			{
			// and there's no separator, so we're at the end.
			return;
			}
		}

	if (!spRead_Until(theStrimU, '=', oName))
		spThrowParseException("Expected a member name, followed by '='");

	string theValue;
	if (sTryRead_CP(theStrimU, '"'))
		{
		// We've got a quoted value.
		sRead_EscapedString(theStrimU, '"', theValue);
		if (!sTryRead_CP(theStrimU, '"'))
			spThrowParseException("Expected '\"' to close value");
		}
	else
		{
		if (spRead_Until(theStrimU, ',', theValue))
			theStrimU.Unread(',');
		}

	oYadR = sMakeYadR(theValue);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_Simple

ZRef<ZYadR> ZYad_Simple::sMakeYadR(ZRef<ZStrimmerU> iStrimmerU)
	{ return new ZYadMapR_Simple(iStrimmerU); }

} // namespace ZooLib
