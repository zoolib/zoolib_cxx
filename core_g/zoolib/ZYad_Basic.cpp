/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ChanW_UTF_string.h"
#include "zoolib/Util_Chan.h"

#include "zoolib/ZUtil_Strim.h"
#include "zoolib/ZYad_Basic.h"

namespace ZooLib {
namespace ZYad_Basic {

using std::string;

// =================================================================================================
// MARK: - Helpers

namespace { // anonymous

void spThrowParseException(const string& iMessage)
	{ throw ParseException(iMessage); }

bool spRead_Until(const ZStrimU& iStrimU, UTF32 iTerminator, string& oString)
	{
	oString.clear();
	return sCopy_Until<UTF32>(iStrimU, ChanW_UTF_string8(&oString), iTerminator);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ParseException

ParseException::ParseException(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
// MARK: - MapR

class MapR
:	public ZYadMapR_Std
	{
public:
	MapR(const Options& iOptions, ZRef<ZStrimmerU> iStrimmerU)
	:	fOptions(iOptions)
	,	fStrimmerU(iStrimmerU)
		{}

// From ZYadMapR_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZName& oName, ZRef<ZYadR>& oYadR)
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
			if (!sTryRead_CP(theStrimU, fOptions.fSeparator_EntryFromEntry))
				{
				// and there's no separator, so we're at the end.
				return;
				}
			}

		string theName;
		if (!spRead_Until(theStrimU, fOptions.fSeparator_NameFromValue, theName))
			spThrowParseException("Expected a member name, followed by #Need UTF32->string8 converter or string8+UTF32 appender#");// + fOptions.fSeparator_NameFromValue);
		oName = theName;

		string theValue;
		if (spRead_Until(theStrimU, fOptions.fSeparator_EntryFromEntry, theValue))
			theStrimU.Unread(fOptions.fSeparator_EntryFromEntry);

		oYadR = ZooLib::sYadR(theValue);
		}

private:
	const Options fOptions;
	ZRef<ZStrimmerU> fStrimmerU;
	};

// =================================================================================================
// MARK: - ZYad_Basic::Options

Options::Options(UTF32 iNameFromValue, UTF32 iEntryFromEntry)
:	fSeparator_NameFromValue(iNameFromValue)
,	fSeparator_EntryFromEntry(iEntryFromEntry)
	{}

// =================================================================================================
// MARK: - sYadR

ZRef<ZYadMapR> sYadR(const Options& iOptions, ZRef<ZStrimmerU> iStrimmerU)
	{ return new MapR(iOptions, iStrimmerU); }

} // namespace ZYad_Basic
} // namespace ZooLib
