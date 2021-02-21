// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pull_SeparatedValues.h"

#include "zoolib/NameUniquifier.h" // For sName

#include "zoolib/Unicode.h" // For operator+=

#include "zoolib/ZMACRO_foreach.h"

#include <vector>

namespace ZooLib {

using namespace PullPush;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Static parsing functions

static bool spReadValues(const ChanR_UTF& iChanR, UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	vector<string8>& oValues)
	{
	bool gotAny = false;
	oValues.clear();
	string8 curValue;
	for (;;)
		{
		if (NotQ<UTF32> theCPQ = sQRead(iChanR))
			{
			oValues.push_back(curValue);
			break;
			}
		else
			{
			gotAny = true;

			if (*theCPQ == iSeparator_Line)
				{
				oValues.push_back(curValue);
				break;
				}

			if (*theCPQ == iSeparator_Value)
				{
				oValues.push_back(curValue);
				curValue.clear();
				}
			else
				{
				curValue += *theCPQ;
				}
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

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_PPT& iChanW)
	{
	vector<string8> theNames;
	if (not spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line,
		theNames))
		{
		return false;
		}

	sPush_Start_Seq(iChanW);
	for (;;)
		{
		vector<string8> theValues;
		if (not spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line,
		 	theValues))
			{
			sPush_End(iChanW);
			return true;
			}

		sPush_Start_Map(iChanW);
		for (size_t xx = 0; xx < theNames.size() && xx < theValues.size(); ++xx)
			sPush(theNames[xx], theValues[xx], iChanW);
		sPush_End(iChanW);
		}
	}

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	UTF32 iSeparator_Value, UTF32 iSeparator_Line,
	const ChanW_PPT& iChanW)
	{
	return sPull_SeparatedValues_Push_PPT(iChanR,
		Pull_SeparatedValues_Options(iSeparator_Value, iSeparator_Line),
		iChanW);
	}

} // namespace ZooLib
