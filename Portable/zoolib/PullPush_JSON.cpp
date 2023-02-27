// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush_JSON.h"

#include "zoolib/ChanR_Bin_HexStrim.h"
#include "zoolib/ChanR_XX_Boundary.h"
#include "zoolib/ChanR_XX_Terminated.h"
#include "zoolib/Chan_XX_Buffered.h"
#include "zoolib/Chan_UTF_string.h"
#include "zoolib/Chan_Bin_ASCIIStrim.h"
#include "zoolib/Chan_Bin_Base64.h"
#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {

using namespace PullPush;
using namespace Util_STL;
using Util_Chan::sSkip_WSAndCPlusPlusComments;
using Util_Chan::sTryRead_CP;
using std::min;
using std::string;
using std::vector;

using Util_Chan_JSON::PushTextOptions_JSON;

namespace { // anonymous

enum class EParent
	{
	Other,
	Map,
	Seq
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -

static bool spPull_JSON_Other_Push(
	const ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW, bool iLooseNumbers)
	{
	int64 asInt64;
	double asDouble;
	bool isDouble;

	if (Util_Chan::sTryRead_SignedGenericNumber(iChanRU, asInt64, asDouble, isDouble))
		{
		if (isDouble)
			{
			sPush(asDouble, iChanW);
			}
		else
			{
			if (iLooseNumbers && asInt64 == -1)
				{
				// Read and discard an L suffix (that python puts on things sometimes)
				sTryRead_CP(iChanRU, 'l') || sTryRead_CP(iChanRU, 'L');
				}
			sPush(asInt64, iChanW);
			}
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString(iChanRU, "null"))
		{
		sPush(PPT(null), iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString(iChanRU, "false"))
		{
		sPush(false, iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString(iChanRU, "true"))
		{
		sPush(true, iChanW);
		return true;
		}

	return false;
	}

// =================================================================================================
#pragma mark -

static const UTF32 spThreeQuotes[] = { '\"', '\"', '\"' };

static bool spPull_JSON_String_Push_UTF(const ChanRU_UTF& iChanRU,
	UTF32 iTerminator, const ChanW_UTF& iChanW)
	{
	ChanR_XX_Boundary<UTF32> theChanR_Boundary(spThreeQuotes, countof(spThreeQuotes), iChanRU);
	int quotesSeen = 1;
	for (;;)
		{
		switch (quotesSeen)
			{
			case 0:
				{
				sSkip_WSAndCPlusPlusComments(iChanRU);

				if (sTryRead_CP(iChanRU, iTerminator))
					quotesSeen = 1;
				else
					return true;
				break;
				}
			case 1:
				{
				if (sTryRead_CP(iChanRU, iTerminator))
					{
					// We have two quotes in a row.
					quotesSeen = 2;
					}
				else
					{
					const std::pair<int64,int64> result =
						sCopyAll(ChanR_UTF_Escaped(iTerminator, iChanRU), iChanW);

					if (sTryRead_CP(iChanRU, iTerminator))
						quotesSeen = 0;
					else if (result.first == 0)
						sThrow_ParseException(string("Expected ") + iTerminator + " to close a string");
					}
				break;
				}
			case 2:
				{
				if (sTryRead_CP(iChanRU, iTerminator))
					{
					// We have three quotes in a row.
					quotesSeen = 3;
					if (ZQ<UTF32> theCPQ = sQRead(iChanRU))
						{
						if (not Unicode::sIsEOL(*theCPQ))
							{
							// And the following character was *not* an EOL, so we'll treat it as
							// part of the string.
							sUnread(iChanRU, *theCPQ);
							}
						}
					}
				else
					{
					// We have two quotes in a row, followed by something
					// else, so we had an empty string segment.
					quotesSeen = 0;
					}
				break;
				}
			case 3:
				{
				// We've got three quotes in a row, and any trailing EOL has been stripped.
				const std::pair<int64,int64> result = sCopyAll(theChanR_Boundary, iChanW);
				if (result.first)
					{}
				else if (not theChanR_Boundary.HitBoundary())
					{
					sThrow_ParseException("Expected \"\"\" to close a string");
					}
				else
					{
					theChanR_Boundary.Reset();
					quotesSeen = 0;
					}
				break;
				}
			}
		}
	}

static bool spPull_JSON_String_Push(const ChanRU_UTF& iChanRU,
	UTF32 iTerminator, const ChanW_PPT& iChanW)
	{
#if 0
	string theString;
	bool result = spPull_JSON_String_Push_UTF(iChanRU,
		iTerminator, ChanW_UTF_string<UTF8>(&theString));
	sPush(theString, iChanW);
	return result;
#else
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);
	sFlush(iChanW);

	bool result = spPull_JSON_String_Push_UTF(iChanRU, iTerminator, *thePullPushPair.first);
	sDisconnectWrite(*thePullPushPair.first);
	return result;
#endif
	}

// =================================================================================================
#pragma mark -

static bool spPull_Base64_Push_Bin(const ChanR_UTF& iChanR, const ChanW_Bin& iChanW)
	{
	ChanR_XX_Terminated<UTF32> theChanR_UTF_Terminated('>', iChanR);
	ChanR_Bin_ASCIIStrim theChanR_Bin_ASCIIStrim(theChanR_UTF_Terminated);
	ChanR_Bin_Base64Decode theChanR_Bin_Base64Decode(theChanR_Bin_ASCIIStrim);

	std::pair<int64,int64> counts = sCopyAll(theChanR_Bin_Base64Decode, iChanW);

	if (counts.first != counts.second)
		return false;

	if (not theChanR_UTF_Terminated.HitTerminator())
		sThrow_ParseException("Expected '>' to close a base64 data");

	return true;
	}

// =================================================================================================
#pragma mark -

static bool spPull_Hex_Push_Bin(const ChanRU_UTF& iChanRU, const ChanW_Bin& iChanW)
	{
	std::pair<int64,int64> counts = sCopyAll(ChanR_Bin_HexStrim(iChanRU), iChanW);
	if (counts.first != counts.second)
		return false;
	if (not sTryRead_CP(iChanRU, '>'))
		sThrow_ParseException("Expected '>' to close a hex data");
	return true;
	}

// =================================================================================================
#pragma mark - sPull_JSON_Push_PPT public API

bool sPull_JSON_Push_PPT(const ChanRU_UTF& iChanRU,
	const Util_Chan_JSON::PullTextOptions_JSON& iOptions,
	const ChanW_PPT& iChanW)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);

	if (sTryRead_CP(iChanRU, '['))
		{
		sPush_Start_Seq(iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (sTryRead_CP(iChanRU, ']'))
				{
				sPush_End(iChanW);
				return true;
				}

			if (not sPull_JSON_Push_PPT(iChanRU, iOptions, iChanW))
				sThrow_ParseException("Expected value or ']'");

			if (iOptions.fLooseSeparators | false)
				{
				// We allow zero or more separators
				for (;;)
					{
					sSkip_WSAndCPlusPlusComments(iChanRU);
					if (sTryRead_CP(iChanRU, ','))
						{}
					else if ((iOptions.fAllowSemiColons | false) && sTryRead_CP(iChanRU, ';'))
						{}
					else
						break;
					}
				}
			else
				{
				sSkip_WSAndCPlusPlusComments(iChanRU);
				if (sTryRead_CP(iChanRU, ','))
					{}
				else if (iOptions.fAllowSemiColons | false)
					{
					if (not sTryRead_CP(iChanRU, ';'))
						sThrow_ParseException("Require ',' or ';' to separate array elements");
					}
				else
					{
					sThrow_ParseException("Require ',' to separate array elements");
					}
				}
			}
		}
	else if (sTryRead_CP(iChanRU, '{'))
		{
		sPush_Start_Map(iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (sTryRead_CP(iChanRU, '}'))
				{
				sPush_End(iChanW);
				return true;
				}

			string theName;
			if (not Util_Chan_JSON::sTryRead_PropertyName(iChanRU,
				theName, iOptions.fAllowUnquotedPropertyNames | false))
				{ sThrow_ParseException("Expected a member name or '}'"); }

			sPush(sName(theName), iChanW);

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sTryRead_CP(iChanRU, ':'))
				{
				if (not (iOptions.fAllowEquals | false))
					sThrow_ParseException("Expected ':' after a member name");

				if (not sTryRead_CP(iChanRU, '='))
					sThrow_ParseException("Expected ':' or '=' after a member name");
				}

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sPull_JSON_Push_PPT(iChanRU, iOptions, iChanW))
				sThrow_ParseException("Expected value");

			if (iOptions.fLooseSeparators | false)
				{
				// We allow zero or more separators
				for (;;)
					{
					sSkip_WSAndCPlusPlusComments(iChanRU);
					if (sTryRead_CP(iChanRU, ','))
						{}
					else if ((iOptions.fAllowSemiColons | false) && sTryRead_CP(iChanRU, ';'))
						{}
					else
						break;
					}
				}
			else
				{
				sSkip_WSAndCPlusPlusComments(iChanRU);
				if (sTryRead_CP(iChanRU, ','))
					{}
				else if (iOptions.fAllowSemiColons | false)
					{
					if (not sTryRead_CP(iChanRU, ';'))
						sThrow_ParseException("Require ',' or ';' to separate object elements");
					}
				else
					{
					sThrow_ParseException("Require ',' to separate object elements");
					}
				}
			}
		}
	else if (sTryRead_CP(iChanRU, '"'))
		{
		return spPull_JSON_String_Push(iChanRU, '"', iChanW);
		}
	else if (sTryRead_CP(iChanRU, '\''))
		{
		return spPull_JSON_String_Push(iChanRU, '\'', iChanW);
		}
	else if ((iOptions.fAllowBinary | false) && sTryRead_CP(iChanRU, '<'))
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
		sPush(sGetClear(thePullPushPair.second), iChanW);
		sFlush(iChanW);

		bool result;
		if (sTryRead_CP(iChanRU, '='))
			result = spPull_Base64_Push_Bin(iChanRU, *thePullPushPair.first);
		else
			result = spPull_Hex_Push_Bin(iChanRU, *thePullPushPair.first);

		sDisconnectWrite(*thePullPushPair.first);

		return result;
		}
	else
		{
		return spPull_JSON_Other_Push(iChanRU, iChanW, iOptions.fAllowBinary.Get());
		}
	}

// =================================================================================================
#pragma mark - sPull_PPT_Push_JSON

static void spPull_PPT_Push_JSON(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	size_t iBaseIndent,
	vector<EParent>& ioParents,
	const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW);

static void spPull_PPT_Push_JSON_Seq(const ChanR_PPT& iChanR,
	size_t iBaseIndent,
	vector<EParent>& ioParents,
	const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW)
	{
	ioParents.push_back(EParent::Seq);

	EIndentationStyle theIndentationStyle = sIndentationStyle(iOptions);

	if (iOptions.fIndentOnlySequencesQ.Get())
		{
		foreacha (aa, ioParents)
			{
			if (aa != EParent::Seq)
				{
				theIndentationStyle = EIndentationStyle::None;
				break;
				}
			}
		}

	if (theIndentationStyle == EIndentationStyle::None)
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		iChanW << "[";
		for (bool isFirst = true; /*no test*/; isFirst = false)
			{
			if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else if (iOptions.fUseExtendedNotationQ | false)
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					{
					iChanW << ",";
					if (sBreakStrings(iOptions))
						iChanW << " ";
					}
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
				}
			}
		iChanW << "]";
		}
	else
		{
		const size_t theIndentation = iBaseIndent + ioParents.size() - 1;
		const size_t childIndentation = theIndentation+1;

		for (uint64 count = 0; /*no test*/; ++count)
			{
			if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
				{
				if (count == 0)
					{
					iChanW << "[]";
					}
				else
					{
					if (theIndentationStyle == EIndentationStyle::Whitesmiths)
						sWrite_LFIndent(iChanW, childIndentation, iOptions);
					else
						sWrite_LFIndent(iChanW, theIndentation, iOptions);
					iChanW << "]";
					}
				break;
				}
			else
				{
				if (count == 0)
					{
					if (theIndentationStyle != EIndentationStyle::KR
						&& ioParents.size() >= 2 && ioParents.end()[-2] == EParent::Map)
						{ // Immediate parent is a map, has to be on new line
						if (theIndentationStyle == EIndentationStyle::Whitesmiths)
							sWrite_LFIndent(iChanW, childIndentation, iOptions);
						else
							sWrite_LFIndent(iChanW, theIndentation, iOptions);
						}
					iChanW << "[";
					}

				if (iOptions.fUseExtendedNotationQ | false)
					{
					sWrite_LFIndent(iChanW, childIndentation, iOptions);
					if (iOptions.fNumberSequencesQ | false)
						iChanW << "/*" << count << "*/";
					spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
					iChanW << ";";
					}
				else
					{
					if (count)
						iChanW << ",";
					sWrite_LFIndent(iChanW, childIndentation, iOptions);
					spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
					}
				}
			}
		}

	ioParents.pop_back();
	}

static void spPull_PPT_Push_JSON_Map(const ChanR_PPT& iChanR,
	size_t iBaseIndent,
	vector<EParent>& ioParents,
	const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW)
	{
	const bool useSingleQuotes = iOptions.fPreferSingleQuotesQ | false;

	ioParents.push_back(EParent::Map);

	const EIndentationStyle theIndentationStyle =
		iOptions.fIndentOnlySequencesQ.Get() ? EIndentationStyle::None : sIndentationStyle(iOptions);

	if (theIndentationStyle == EIndentationStyle::None)
		{
		iChanW << "{";
		bool wroteAny = false;
		for (bool isFirst = true; /*no test*/; isFirst = false)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else if (iOptions.fUseExtendedNotationQ | false)
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";

				Util_Chan_JSON::sWrite_PropName(iChanW, *theNameQ, useSingleQuotes);
				if (sBreakStrings(iOptions))
					iChanW << " = ";
				else
					iChanW << "=";

				spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				if (sBreakStrings(iOptions))
					iChanW << " ";
				Util_Chan_JSON::sWrite_String(iChanW, *theNameQ, useSingleQuotes);
				iChanW << ":";
				if (sBreakStrings(iOptions))
					iChanW << " ";

				spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
				}
			wroteAny = true;
			}
		if (wroteAny && sBreakStrings(iOptions))
			iChanW << " ";
		iChanW << "}";
		}
	else
		{
		const size_t theIndentation = iBaseIndent + ioParents.size() - 1;
		const size_t childIndentation = theIndentation+1;

		for (uint64 count = 0; /*no test*/; ++count)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				if (count == 0)
					{
					iChanW << "{}";
					}
				else
					{
					if (theIndentationStyle == EIndentationStyle::Whitesmiths)
						sWrite_LFIndent(iChanW, childIndentation, iOptions);
					else
						sWrite_LFIndent(iChanW, theIndentation, iOptions);
					iChanW << "}";
					}
				break;
				}
			else if (NotQ<PPT> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else
				{
				if (count == 0)
					{
					if (theIndentationStyle != EIndentationStyle::KR
						&& ioParents.size() >= 2 && ioParents.end()[-2] == EParent::Map)
						{ // Immediate parent is a map, has to be on new line
						if (theIndentationStyle == EIndentationStyle::Whitesmiths)
							sWrite_LFIndent(iChanW, childIndentation, iOptions);
						else
							sWrite_LFIndent(iChanW, theIndentation, iOptions);
						}
					iChanW << "{";
					}

				if (iOptions.fUseExtendedNotationQ | false)
					{
					sWrite_LFIndent(iChanW, childIndentation, iOptions);
					Util_Chan_JSON::sWrite_PropName(iChanW, *theNameQ, useSingleQuotes);
					iChanW << " = ";
					spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
					iChanW << ";";
					}
				else
					{
					if (count)
						iChanW << ",";
					sWrite_LFIndent(iChanW, childIndentation, iOptions);
					Util_Chan_JSON::sWrite_String(iChanW, *theNameQ, useSingleQuotes);
					iChanW << ": ";
					spPull_PPT_Push_JSON(*theNotQ, iChanR, iBaseIndent, ioParents, iOptions, iChanW);
					}
				}
			}
		}
	ioParents.pop_back();
	}

static void spPull_PPT_Push_JSON(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	size_t iIndent,
	vector<EParent>& ioParents,
	const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW)
	{
	if (ZP<Callable_JSON_WriteFilter> theFilter = ThreadVal<ZP<Callable_JSON_WriteFilter>>::sGet())
		{
		ThreadVal_PushTextIndent tv_PushTextIndent(iIndent);
		ThreadVal<PushTextOptions_JSON> tv_Options(iOptions);
		if (sCall(theFilter, iPPT, iChanR, iChanW))
			return;
		}

	if (const string* theString = sPGet<string>(iPPT))
		{
		Util_Chan_JSON::sWrite_String(iChanW, *theString, false);
		return;
		}

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(iPPT))
		{
		Util_Chan_JSON::sWrite_String(iChanW, *theChanner);
		return;
		}

	if (const Data_ZZ* theData = sPGet<Data_ZZ>(iPPT))
		{
		if (not iOptions.fBinaryAsBase64Q)
			{
			// We don't have a value for fBinaryAsBase64Q, so we're not emitting binary at all.
			sWrite_SimpleValue(iChanW, Any(), iOptions);
			}
		else
			{
			Util_Chan_JSON::sPull_Bin_Push_JSON(ChanRPos_Bin_Data<Data_ZZ>(*theData),
				iIndent + ioParents.size(), iOptions, iChanW);
			}
		return;
		}

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(iPPT))
		{
		if (not iOptions.fBinaryAsBase64Q)
			{
			// We don't have a value for fBinaryAsBase64Q, so we're not emitting binary at all.
			sWrite_SimpleValue(iChanW, Any(), iOptions);
			sSkipAll(*theChanner);
			}
		else
			{
			Util_Chan_JSON::sPull_Bin_Push_JSON(*theChanner,
				iIndent + ioParents.size(), iOptions, iChanW);
			}
		return;
		}

	if (sIsStart_Map(iPPT))
		{
		spPull_PPT_Push_JSON_Map(iChanR, iIndent, ioParents, iOptions, iChanW);
		return;
		}

	if (sIsStart_Seq(iPPT))
		{
		spPull_PPT_Push_JSON_Seq(iChanR, iIndent, ioParents, iOptions, iChanW);
		return;
		}

	Util_Chan_JSON::sWrite_SimpleValue(iChanW, iPPT.As<Any>(), iOptions);
	}

// =================================================================================================
#pragma mark - sPull_PPT_Push_JSON public API

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR, const ChanW_UTF& iChanW)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		vector<EParent> parents;
		spPull_PPT_Push_JSON(*theQ, iChanR,
			ThreadVal_PushTextIndent::sGet(),
			parents,
			ThreadVal<PushTextOptions_JSON>::sGet(),
			iChanW);
		return true;
		}
	return false;
	}

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iIndent,
	const PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW)
	{
	ThreadVal_PushTextIndent tv_PushTextIndent(iIndent);
	ThreadVal<PushTextOptions_JSON> tv_Options(iOptions);
	return sPull_PPT_Push_JSON(iChanR, iChanW);
	}

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iIndent,
	const PushTextOptions_JSON& iOptions,
	const ZP<Callable_JSON_WriteFilter>& iWriteFilter,
	const ChanW_UTF& iChanW)
	{
	ThreadVal_PushTextIndent tv_PushTextIndent(iIndent);
	ThreadVal<PushTextOptions_JSON> tv_Options(iOptions);
	ThreadVal<ZP<Callable_JSON_WriteFilter>> tv_Filter(iWriteFilter);

	return sPull_PPT_Push_JSON(iChanR, iChanW);
	}

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

static void spPull_JSON_Push_PPT(
	const Util_Chan_JSON::PullTextOptions_JSON& iOptions,
	const ChanRU_UTF& iChanRU,
	const ChanW_PPT& iChanW)
	{ sPull_JSON_Push_PPT(iChanRU, iOptions, iChanW); }

ZP<ChannerR_PPT> sChannerR_PPT_JSON(const ZP<Channer<ChanRU_UTF>>& iChanner,
	const Util_Chan_JSON::PullTextOptions_JSON& iOptions)
	{ return sStartPullPush(sBindL(iOptions, sCallable(spPull_JSON_Push_PPT)), iChanner); }

} // namespace ZooLib
