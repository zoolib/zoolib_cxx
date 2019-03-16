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
#include "zoolib/Data_Any.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

using namespace PullPush;
using Util_Chan::sSkip_WSAndCPlusPlusComments;
using Util_Chan::sTryRead_CP;
using std::min;
using std::string;

// =================================================================================================
#pragma mark -

static bool spPull_JSON_Other_Push(const ChanRU_UTF& iChanRU, const ChanW_PPT& iChanW)
	{
	int64 asInt64;
	double asDouble;
	bool isDouble;

	if (Util_Chan::sTryRead_SignedGenericNumber(iChanRU, asInt64, asDouble, isDouble))
		{
		if (isDouble)
			sPush(asDouble, iChanW);
		else
			sPush(asInt64, iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("null", iChanRU))
		{
		sPush(PPT(null), iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("false", iChanRU))
		{
		sPush(false, iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("true", iChanRU))
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

				if (sTryRead_CP(iTerminator, iChanRU))
					quotesSeen = 1;
				else
					return true;
				break;
				}
			case 1:
				{
				if (sTryRead_CP(iTerminator, iChanRU))
					{
					// We have two quotes in a row.
					quotesSeen = 2;
					}
				else
					{
					const std::pair<int64,int64> result =
						sCopyAll(ChanR_UTF_Escaped(iTerminator, iChanRU), iChanW);

					if (sTryRead_CP(iTerminator, iChanRU))
						quotesSeen = 0;
					else if (result.first == 0)
						sThrow_ParseException(string("Expected ") + iTerminator + " to close a string");
					}
				break;
				}
			case 2:
				{
				if (sTryRead_CP(iTerminator, iChanRU))
					{
					// We have three quotes in a row.
					quotesSeen = 3;
					if (ZQ<UTF32,false> theCPQ = sQRead(iChanRU))
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
#if 1
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
	if (not sTryRead_CP('>', iChanRU))
		sThrow_ParseException("Expected '>' to close a hex data");
	return true;
	}

// =================================================================================================
#pragma mark - sPull

bool sPull_JSON_Push_PPT(const ChanRU_UTF& iChanRU,
	const Util_Chan_JSON::PullTextOptions_JSON& iRO,
	const ChanW_PPT& iChanW)
	{
	sSkip_WSAndCPlusPlusComments(iChanRU);

	if (sTryRead_CP('[', iChanRU))
		{
		sPush_Start_Seq(iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (sTryRead_CP(']', iChanRU))
				{
				sPush_End(iChanW);
				return true;
				}

			if (not sPull_JSON_Push_PPT(iChanRU, iRO, iChanW))
				sThrow_ParseException("Expected value");

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (sTryRead_CP(',', iChanRU))
				{}
			else if (iRO.fAllowSemiColons.DGet(false) && sTryRead_CP(';', iChanRU))
				{}
			else if (iRO.fLooseSeparators.DGet(false))
				{}
			else if (iRO.fAllowSemiColons.DGet(false))
				sThrow_ParseException("Require ',' or ';' to separate array elements");
			else
				sThrow_ParseException("Require ',' to separate array elements");
			}
		}
	else if (sTryRead_CP('{', iChanRU))
		{
		sPush_Start_Map(iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanRU);
			if (sTryRead_CP('}', iChanRU))
				{
				sPush_End(iChanW);
				return true;
				}

			string theName;
			if (not Util_Chan_JSON::sTryRead_PropertyName(iChanRU,
				theName, iRO.fAllowUnquotedPropertyNames.DGet(false)))
				{ sThrow_ParseException("Expected a member name"); }

			sPush(sName(theName), iChanW);

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sTryRead_CP(':', iChanRU))
				{
				if (not iRO.fAllowEquals.DGet(false))
					sThrow_ParseException("Expected ':' after a member name");

				if (not sTryRead_CP('=', iChanRU))
					sThrow_ParseException("Expected ':' or '=' after a member name");
				}

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (not sPull_JSON_Push_PPT(iChanRU, iRO, iChanW))
				sThrow_ParseException("Expected value");

			sSkip_WSAndCPlusPlusComments(iChanRU);

			if (sTryRead_CP(',', iChanRU))
				{}
			else if (iRO.fAllowSemiColons.DGet(false) && sTryRead_CP(';', iChanRU))
				{}
			else if (iRO.fLooseSeparators.DGet(false))
				{}
			else if (iRO.fAllowSemiColons.DGet(false))
				sThrow_ParseException("Require ',' or ';' to separate object elements");
			else
				sThrow_ParseException("Require ',' to separate object elements");
			}
		}
	else if (sTryRead_CP('"', iChanRU))
		{
		return spPull_JSON_String_Push(iChanRU, '"', iChanW);
		}
	else if (sTryRead_CP('\'', iChanRU))
		{
		return spPull_JSON_String_Push(iChanRU, '\'', iChanW);
		}
	else if (iRO.fAllowBinary.DGet(false) && sTryRead_CP('<', iChanRU))
		{
		sSkip_WSAndCPlusPlusComments(iChanRU);

		PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
		sPush(sGetClear(thePullPushPair.second), iChanW);
		sFlush(iChanW);

		bool result;
		if (sTryRead_CP('=', iChanRU))
			result = spPull_Base64_Push_Bin(iChanRU, *thePullPushPair.first);
		else
			result = spPull_Hex_Push_Bin(iChanRU, *thePullPushPair.first);

		sDisconnectWrite(*thePullPushPair.first);

		return result;
		}
	else
		{
		return spPull_JSON_Other_Push(iChanRU, iChanW);
		}
	}

// =================================================================================================
#pragma mark - 

static void spPull_PPT_Push_JSON(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	size_t iIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW);

static void spPull_PPT_Push_JSON_Seq(const ChanR_PPT& iChanR,
	size_t iIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW)
	{
	bool needsIndentation = false;
	if (sDoIndentation(iOptions))
		{
		// We're supposed to be indenting if we're complex, ie if any element is:
		// 1. A non-empty vector.
		// 2. A non-empty tuple.
		// or if iOptions.fBreakStrings is true, any element is a string with embedded
		// line breaks or more than iOptions.fStringLineLength characters.
		//##needsIndentation = not iYadSeqR->IsSimple(iOptions);
		needsIndentation = true;
		}

	if (needsIndentation)
		{
		// We need to indent.
		if (iMayNeedInitialLF)
			{
			// We were invoked by a tuple which has already issued the property
			// name and equals sign, so we need to start a fresh line.
			sWriteLFIndent(iIndent, iOptions, iChanW);
			}

		uint64 count = 0;
		iChanW << "[";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else
				{
				int localIndent = sIsStartSeq(*theNotQ) ? iIndent + 1 : iIndent;
				if (iOptions.fUseExtendedNotation.DGet(false))
					{
					sWriteLFIndent(localIndent, iOptions, iChanW);
					if (iOptions.fNumberSequences.DGet(false))
						iChanW << "/*" << count << "*/";
					spPull_PPT_Push_JSON(*theNotQ, iChanR, localIndent, iOptions, false, iChanW);
					iChanW << ";";
					}
				else
					{
					if (not isFirst)
						iChanW << ",";
					sWriteLFIndent(localIndent, iOptions, iChanW);
					spPull_PPT_Push_JSON(*theNotQ, iChanR, localIndent, iOptions, false, iChanW);
					}
				}
			++count;
			}
		sWriteLFIndent(iIndent, iOptions, iChanW);
		iChanW << "]";
		}
	else
		{
		// We're not indenting, so we can just dump everything out on
		// one line, with just some spaces to keep things legible.
		iChanW << "[";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (NotQ<PPT> theNotQ = sQEReadPPTOrEnd(iChanR))
				{
				break;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);
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
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);
				}
			}
		iChanW << "]";
		}
	}

static void spPull_PPT_Push_JSON_Map(const ChanR_PPT& iChanR,
	size_t iIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW)
	{
	bool needsIndentation = false;
	if (sDoIndentation(iOptions))
		{
		//##needsIndentation = not iYadMapR->IsSimple(iOptions);
		needsIndentation = true;
		}

	const bool useSingleQuotes = iOptions.fPreferSingleQuotes.DGet(false);

	if (needsIndentation)
		{
		if (iMayNeedInitialLF)
			{
			// We're going to be indenting, but need to start
			// a fresh line to have our { and contents line up.
			sWriteLFIndent(iIndent, iOptions, iChanW);
			}

		iChanW << "{";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				sWriteLFIndent(iIndent, iOptions, iChanW);
				Util_Chan_JSON::sWritePropName(*theNameQ, useSingleQuotes, iChanW);
				iChanW << " = ";
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW);
				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				sWriteLFIndent(iIndent, iOptions, iChanW);
				Util_Chan_JSON::sWriteString(*theNameQ, useSingleQuotes, iChanW);
				iChanW << ": ";
				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW);
				}
			}
		sWriteLFIndent(iIndent, iOptions, iChanW);
		iChanW << "}";
		}
	else
		{
		iChanW << "{";
		bool wroteAny = false;
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				break;
				}
			else if (NotQ<PPT> theNotQ = sQRead(iChanR))
				{
				sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";

				Util_Chan_JSON::sWritePropName(*theNameQ, useSingleQuotes, iChanW);
				if (sBreakStrings(iOptions))
					iChanW << " = ";
				else
					iChanW << "=";

				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW);

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				if (sBreakStrings(iOptions))
					iChanW << " ";
				Util_Chan_JSON::sWriteString(*theNameQ, useSingleQuotes, iChanW);
				iChanW << ":";
				if (sBreakStrings(iOptions))
					iChanW << " ";

				spPull_PPT_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW);
				}
			wroteAny = true;
			}
		if (wroteAny && sBreakStrings(iOptions))
			iChanW << " ";
		iChanW << "}";
		}
	}

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR, const ChanW_UTF& iChanW)
	{ return sPull_PPT_Push_JSON(iChanR, 0, Util_Chan_JSON::PushTextOptions_JSON(), iChanW); }

bool sPull_PPT_Push_JSON(const ChanR_PPT& iChanR,
	size_t iInitialIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions,
	const ChanW_UTF& iChanW)
	{
	if (ZQ<PPT> theQ = sQRead(iChanR))
		{
		spPull_PPT_Push_JSON(*theQ, iChanR, iInitialIndent, iOptions, false, iChanW);
		return true;
		}
	return false;
	}

static void spPull_PPT_Push_JSON(const PPT& iPPT,
	const ChanR_PPT& iChanR,
	size_t iIndent, const Util_Chan_JSON::PushTextOptions_JSON& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW)
	{
	if (const string* theString = sPGet<string>(iPPT))
		{
		Util_Chan_JSON::sWriteString(*theString, false, iChanW);
		}

	else if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iPPT))
		{
		Util_Chan_JSON::sWriteString(*theChanner, iChanW);
		}

	else if (const Data_Any* theData = sPGet<Data_Any>(iPPT))
		{
		Util_Chan_JSON::sPull_Bin_Push_JSON(ChanRPos_Bin_Data<Data_Any>(*theData),
			iIndent, iOptions, iMayNeedInitialLF, iChanW);
		}

	else if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iPPT))
		{
		Util_Chan_JSON::sPull_Bin_Push_JSON(*theChanner, iIndent, iOptions, iMayNeedInitialLF, iChanW);
		}

	else if (sIsStartMap(iPPT))
		{
		spPull_PPT_Push_JSON_Map(iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);
		}

	else if (sIsStartSeq(iPPT))
		{
		spPull_PPT_Push_JSON_Seq(iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);
		}

	else
		{
		Util_Chan_JSON::sWriteSimpleValue(iPPT.As<Any>(), iOptions, iChanW);
		}
	}

} // namespace ZooLib
