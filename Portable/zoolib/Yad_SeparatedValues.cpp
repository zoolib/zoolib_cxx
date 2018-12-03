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

#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_SeparatedValues.h"
#include "zoolib/Yad_Std.h"

namespace ZooLib {
namespace Yad_SeparatedValues {

using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Static parsing functions

static bool spReadValues(vector<string8>& oValues,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line, const ChanR_UTF& iStrimR)
	{
	bool gotAny = false;
	oValues.clear();
	string8 curValue;
	for (;;)
		{
		UTF32 theCP;
		if (not sQRead(iStrimR ,theCP))
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
#pragma mark - YadSeqR

class ChanR_RefYad : public ChanR_RefYad_Std
	{
public:
	ChanR_RefYad(const vector<string8>& iNames,
		UTF32 iSeparator_Value, UTF32 iSeparator_Line, ZRef<ChannerR_UTF> iChannerR_UTF);

// From ChanR_RefYad_Std
	virtual void Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR);

private:
	vector<string8> fNames;
	UTF32 fSeparator_Value;
	UTF32 fSeparator_Line;
	ZRef<ChannerR_UTF> fChannerR_UTF;
	};

ChanR_RefYad::ChanR_RefYad(const vector<string8>& iNames,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line, ZRef<ChannerR_UTF> iChannerR_UTF)
:	fNames(iNames)
,	fSeparator_Value(iSeparator_Value)
,	fSeparator_Line(iSeparator_Line)
,	fChannerR_UTF(iChannerR_UTF)
	{}

void ChanR_RefYad::Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
	{
	vector<string8> theValues;
	if (not spReadValues(theValues, fSeparator_Value, fSeparator_Line, *fChannerR_UTF))
		return;

	Map_Any theMap;
	for (size_t x = 0; x < fNames.size() && x < theValues.size(); ++x)
		theMap.Set(fNames[x], theValues[x]);
	oYadR = sYadR(theMap);
	}

// =================================================================================================
#pragma mark - sYadR

ZRef<YadR> sYadR(UTF32 iSeparator_Value, UTF32 iSeparatorLine, ZRef<ChannerR_UTF> iChannerR_UTF)
	{
	// Read the first line and build list of property names.
	vector<string8> values;
	if (!spReadValues(values, iSeparator_Value, iSeparatorLine, *iChannerR_UTF))
		return null;
	return sChanner_T<ChanR_RefYad>(values, iSeparator_Value, iSeparatorLine, iChannerR_UTF);
	}

} // namespace Yad_SeparatedValues
} // namespace ZooLib
