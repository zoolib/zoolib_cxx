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

#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_Basic.h"

namespace ZooLib {
namespace Yad_Basic {

using std::string;

// =================================================================================================
#pragma mark - Helpers

namespace { // anonymous

bool spRead_Until(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, UTF32 iTerminator, string& oString)
	{
	oString.clear();
	return sCopy_Until<UTF32>(iChanR, iTerminator, ChanW_UTF_string8(&oString));
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - MapR

class ChanR_NameRefYad
:	public ChanR_NameRefYad_Std
	{
public:
	ChanR_NameRefYad(const Options& iOptions,
		ZRef<ChannerR_UTF> iChannerR_UTF, ZRef<ChannerU_UTF> iChannerU_UTF)
	:	fOptions(iOptions)
	,	fChannerR_UTF(iChannerR_UTF)
	,	fChannerU_UTF(iChannerU_UTF)
		{}

// From ChanR_RefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
		{
		const ChanR_UTF& theChanR = *fChannerR_UTF;
		const ChanU_UTF& theChanU = *fChannerU_UTF;

		if (iIsFirst)
			{
			UTF32 aCP;
			if (not sQRead(theChanR, aCP))
				{
				// couldn't read anything, we're empty.
				return;
				}
			sUnread(theChanU, aCP);
			}
		else
			{
			// We've already returned something
			if (not Util_Chan::sTryRead_CP(fOptions.fSeparator_EntryFromEntry, theChanR, theChanU))
				{
				// and there's no separator, so we're at the end.
				return;
				}
			}

		string theName;
		if (not spRead_Until(theChanR, theChanU, fOptions.fSeparator_NameFromValue, theName))
			{
			sThrow_ParseException(string8("Expected a member name, followed by '")
				+ fOptions.fSeparator_NameFromValue + "'");
			}

		oName = theName;

		string theValue;
		if (spRead_Until(theChanR, theChanU, fOptions.fSeparator_EntryFromEntry, theValue))
			sUnread(theChanU, fOptions.fSeparator_EntryFromEntry);

		oYadR = ZooLib::sYadR(theValue);
		}

private:
	const Options fOptions;
	ZRef<ChannerR_UTF> fChannerR_UTF;
	ZRef<ChannerU_UTF> fChannerU_UTF;
	};

// =================================================================================================
#pragma mark - ZYad_Basic::Options

Options::Options(UTF32 iNameFromValue, UTF32 iEntryFromEntry)
:	fSeparator_NameFromValue(iNameFromValue)
,	fSeparator_EntryFromEntry(iEntryFromEntry)
	{}

// =================================================================================================
#pragma mark - sYadR

ZRef<Channer<ZooLib::ChanR_NameRefYad>> sYadR(const Options& iOptions,
	ZRef<ChannerR_UTF> iChannerR_UTF, ZRef<ChannerU_UTF> iChannerU_UTF)
	{ return sChanner_T<ChanR_NameRefYad>(iOptions, iChannerR_UTF, iChannerU_UTF); }

} // namespace ZYad_Basic
} // namespace ZooLib
