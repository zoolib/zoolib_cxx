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

#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_DividedValues.h"

namespace ZooLib {
namespace ZYad_DividedValues {

using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static parsing functions

static bool spReadValues(vector<string8>& oValues,
	UTF32 iDivider_Value, UTF32 iDivider_Line, const ZStrimR& iStrimR)
	{
	bool gotAny;
	oValues.clear();
	string8 curValue;
	for (;;)
		{
		UTF32 theCP;
		if (not iStrimR.ReadCP(theCP))
			{
			oValues.push_back(curValue);
			break;
			}

		gotAny = true;

		if (theCP == iDivider_Line)
			{
			oValues.push_back(curValue);
			break;
			}

		if (theCP == iDivider_Value)
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
#pragma mark -
#pragma mark * ParseException

ParseException::ParseException(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ParseException::ParseException(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * YadSeqR

class YadSeqR : public ZYadSeqR
	{
public:
	YadSeqR(const vector<string8>& iNames,
		UTF32 iDivider_Value, UTF32 iDivider_Line, ZRef<ZStrimmerR> iStrimmerR);

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc();

private:
	vector<string8> fNames;
	UTF32 fDivider_Value;
	UTF32 fDivider_Line;
	ZRef<ZStrimmerR> fStrimmerR;
	};

YadSeqR::YadSeqR(const vector<string8>& iNames,
	UTF32 iDivider_Value, UTF32 iDivider_Line, ZRef<ZStrimmerR> iStrimmerR)
:	fNames(iNames)
,	fDivider_Value(iDivider_Value)
,	fDivider_Line(iDivider_Line)
,	fStrimmerR(iStrimmerR)
	{}

ZRef<ZYadR> YadSeqR::ReadInc()
	{
	vector<string8> theValues;
	if (!spReadValues(theValues, fDivider_Value, fDivider_Line, fStrimmerR->GetStrimR()))
		return null;

	ZMap_Any theMap;
	for (size_t x = 0; x < fNames.size() && x < theValues.size(); ++x)
		theMap.Set(fNames[x], theValues[x]);
	return sYadR(theMap);
	}

// =================================================================================================
#pragma mark -
#pragma mark * sYadR

ZRef<ZYadR> sYadR(UTF32 iDivider_Value, UTF32 iDividerLine, ZRef<ZStrimmerR> iStrimmerR)
	{
	// Read the first line and build list of property names.
	vector<string8> values;
	if (!spReadValues(values, iDivider_Value, iDividerLine, iStrimmerR->GetStrimR()))
		return null;
	return new YadSeqR(values, iDivider_Value, iDividerLine, iStrimmerR);
	}

} // namespace ZYad_DividedValues
} // namespace ZooLib
