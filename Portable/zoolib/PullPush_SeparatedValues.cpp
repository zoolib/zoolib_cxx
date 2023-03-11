// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush_SeparatedValues.h"

#include "zoolib/ParseException.h"
#include "zoolib/Unicode.h" // For operator+=
#include "zoolib/ChanW_UTF.h"

#include "zoolib/ZMACRO_foreach.h"

#include <vector>

namespace ZooLib {

//using namespace Util_Chan_UTF_Operators;
using namespace PullPush;

using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Static parsing functions

static
vector<string8> spReadValues(const ChanR_UTF& iChanR, UTF32 iSeparator_Value, UTF32 iTerminator_Line)
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
		else if (*theCPQ == iTerminator_Line)
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
	UTF32 iSeparator_Value, UTF32 iTerminator_Line)
:	fSeparator_Value(iSeparator_Value)
,	fTerminator_Line(iTerminator_Line)
	{}

// =================================================================================================
#pragma mark -

void sPull_SeparatedValues_Push_PPT_Alternate(const Pull_SeparatedValues_Options& iOptions,
	const ChanR_UTF& iChanR,
	const ChanW_PPT& iChanW)
	{ sPull_SeparatedValues_Push_PPT(iChanR, iOptions, iChanW); }

bool sPull_SeparatedValues_Push_PPT(const ChanR_UTF& iChanR,
	const Pull_SeparatedValues_Options& iOptions,
	const ChanW_PPT& iChanW)
	{
	const vector<string8> theNames =
		spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fTerminator_Line);

	if (theNames.empty())
		return false;

	sPush_Start_Seq(iChanW);
	for (;;)
		{
		const vector<string8> theValues =
			spReadValues(iChanR, iOptions.fSeparator_Value, iOptions.fTerminator_Line);

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
	UTF32 iSeparator_Value, UTF32 iTerminator_Line,
	const ChanW_PPT& iChanW)
	{
	return sPull_SeparatedValues_Push_PPT(iChanR,
		Pull_SeparatedValues_Options(iSeparator_Value, iTerminator_Line),
		iChanW);
	}

// =================================================================================================
#pragma mark - Push_SeparatedValues_Options

Push_SeparatedValues_Options::Push_SeparatedValues_Options(
	UTF32 iSeparator_Value, UTF32 iTerminator_Line)
:	fSeparator_Value(iSeparator_Value)
,	fTerminator_Line(iTerminator_Line)
	{}

// =================================================================================================
#pragma mark -

bool sPull_PPT_Push_SeparatedValues(const ChanR_PPT& iChanR,
	const Push_SeparatedValues_Options& iOptions,
	const ChanW_UTF& iChanW)
	{
	ZQ<PPT> theStartQ = sQRead(iChanR);
	if (not theStartQ)
		return false;

	if (not sIsStart_Seq(*theStartQ))
		sThrow_ParseException("Expected Start_Seq");

	for (;;)
		{
		ZQ<PPT> theQ = sQRead(iChanR);
		if (not theQ)
			sThrow_ParseException("Expected PPT");

		if (not sIsStart_Seq(*theQ))
			{
			if (sIsEnd(*theQ))
				break;
			sThrow_ParseException("Expected End of document, or start of line");
			}

		for (bool isFirst = true; /*no test*/; isFirst = false)
			{
			ZQ<PPT> theQ = sQEReadPPTOrEnd(iChanR);
			if (not theQ)
				{
				sEWrite(iChanW, iOptions.fTerminator_Line);
				break;
				}

			if (not isFirst)
				sEWrite(iChanW, iOptions.fSeparator_Value);

			if (ZQ<string8> theStringQ = theQ->QGet<string8>())
				sEWrite(iChanW, *theStringQ);
			else
				{
				sEWrite(iChanW, "???");
				if (sIsStart(*theQ))
					sSkip_Node(iChanR, 1);
				}
			}
		}

	return true;
	}

} // namespace ZooLib
