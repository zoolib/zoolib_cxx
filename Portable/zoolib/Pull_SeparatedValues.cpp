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

#include "zoolib/Pull_SeparatedValues.h"

#include "zoolib/NameUniquifier.h" // For sName

#include "zoolib/ZMACRO_foreach.h"

#include <vector>

namespace ZooLib {

using namespace PullPush;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Static parsing functions

static bool spReadValues(const ChanR_UTF& iChanR, UTF32 iSeparator_Value, UTF32 iSeparator_Line, vector<string8>& oValues)
	{
	bool gotAny = false;
	oValues.clear();
	string8 curValue;
	for (;;)
		{
		UTF32 theCP;
		if (not sQRead(iChanR, theCP))
			{
			oValues.push_back(curValue);
			break;
			}

		gotAny = true;

		if (theCP == iSeparator_Line)
			{
			oValues.push_back(curValue);
			break;
			}

		if (theCP == iSeparator_Value)
			{
			oValues.push_back(curValue);
			curValue.clear();
			}
		else
			{
			curValue += theCP;
			}
		}
	return gotAny;
	}

// =================================================================================================
#pragma mark - Pull_SeparatedValues_Options

Pull_SeparatedValues_Options::Pull_SeparatedValues_Options(
	UTF32 iSeparator_Value, UTF32 iSeparator_Line)
:	fSeparator_Value(iSeparator_Value)
,	fSeparator_Line(iSeparator_Line)
	{}

// =================================================================================================
#pragma mark -

bool sPull_SeparatedValues_Push(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_Any& iChanW)
	{
	vector<string8> theNames;
	if (not spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line,
		theNames))
		{
		return false;
		}

	sPush(kStartMap, iChanW);
	for (;;)
		{
		vector<string8> theValues;
		if (not spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line,
		 	theValues))
			{
			sPush(kEnd, iChanW);
			return true;
			}

		for (size_t xx = 0; xx < theNames.size() && xx < theValues.size(); ++xx)
			sPush(theNames[xx], theValues[xx], iChanW);
		}
	}

bool sPull_SeparatedValues_Push(const ChanR_UTF& iChanR,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	const ChanW_Any& iChanW)
	{
	return sPull_SeparatedValues_Push(iChanR,
		Pull_SeparatedValues_Options(iSeparator_Value, iSeparator_Line),
		iChanW);
	}

} // namespace ZooLib
