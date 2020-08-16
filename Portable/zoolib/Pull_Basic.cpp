// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

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
			sPush_End(iChanW);
			return true;
			}
		else if (isFirst)
			{
			sPush_Start_Map(iChanW);
			}

		string theValue;
		spRead_Until(iChanR, iOptions.fSeparator_EntryFromEntry, theValue);

		sPush(theName, theValue, iChanW);
		}
	}

} // namespace ZooLib
