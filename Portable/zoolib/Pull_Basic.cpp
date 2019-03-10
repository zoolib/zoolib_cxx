/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/Pull_Basic.h"

#include "zoolib/Chan_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/Util_Chan.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {

using namespace PullPush;
using std::string;

// =================================================================================================
#pragma mark - Helpers

namespace { // anonymous

bool spRead_Until(const ChanR_UTF& iChanR, UTF32 iTerminator, string& oString)
	{
	oString.clear();
	return sCopy_Until<UTF32>(iChanR, iTerminator, ChanW_UTF_string8(&oString));
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Pull_Basic_Options

Pull_Basic_Options::Pull_Basic_Options(UTF32 iNameFromValue, UTF32 iEntryFromEntry)
:	fSeparator_NameFromValue(iNameFromValue)
,	fSeparator_EntryFromEntry(iEntryFromEntry)
	{}

// =================================================================================================
#pragma mark -

bool sPull_Basic_Push_PPT(const ChanR_UTF& iChanR, const Pull_Basic_Options& iOptions,
	const ChanW_PPT& iChanW)
	{
	for (bool isFirst = true; /*no test*/; isFirst = false)
		{
		string theName;
		if (not spRead_Until(iChanR, iOptions.fSeparator_NameFromValue, theName))
			{
			if (isFirst)
				return false;
			sPush(kEnd, iChanW);
			return true;
			}
		else if (isFirst)
			{
			sPush(kStartMap, iChanW);
			}

		string theValue;
		spRead_Until(iChanR, iOptions.fSeparator_EntryFromEntry, theValue);

		sPush(theName, theValue, iChanW);
		}
	}

} // namespace ZooLib
