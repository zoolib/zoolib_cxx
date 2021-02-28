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

static
vector<string8> spReadValues(const ChanR_UTF& iChanR,UTF32 iSeparator_Value, UTF32 iSeparator_Line)
	{
	vector<string8> result;
	string8 curValue;
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		if (NotQ<UTF32> theCPQ = sQRead(iChanR))
			{
			if (gotAny)
				result.push_back(curValue);
			break;
			}
		else if (*theCPQ == iSeparator_Line)
			{
			result.push_back(curValue);
			break;
			}
		else if (*theCPQ == iSeparator_Value)
			{
			result.push_back(curValue);
			curValue.clear();
			}
		else
			{
			curValue += *theCPQ;
			}
		}
	return result;
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
	const vector<string8> theNames =
		spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line);

	if (theNames.empty())
		return false;

	sPush_Start_Seq(iChanW);
	for (;;)
		{
		const vector<string8> theValues =
			spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fSeparator_Line);

		if (theValues.empty())
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
