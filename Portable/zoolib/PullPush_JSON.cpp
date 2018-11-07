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
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/NameUniquifier.h" // For sName
#include "zoolib/ParseException.h"
#include "zoolib/Unicode.h"
#include "zoolib/Util_Chan_UTF.h"
#include "zoolib/Util_Chan_UTF_Operators.h"

namespace ZooLib {

using namespace PullPush;
using namespace PullPush_JSON;
using Util_Chan::sSkip_WSAndCPlusPlusComments;
using Util_Chan::sTryRead_CP;
using std::min;
using std::string;

// =================================================================================================
#pragma mark -

static bool spPull_JSON_Other_Push(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, const ChanW_Any& iChanW)
	{
	int64 asInt64;
	double asDouble;
	bool isDouble;

	if (Util_Chan::sTryRead_SignedGenericNumber(iChanR, iChanU, asInt64, asDouble, isDouble))
		{
		if (isDouble)
			sPush(asDouble, iChanW);
		else
			sPush(asInt64, iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("null", iChanR, iChanU))
		{
		sPush(Any(null), iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("false", iChanR, iChanU))
		{
		sPush(false, iChanW);
		return true;
		}

	if (Util_Chan::sTryRead_CaselessString("true", iChanR, iChanU))
		{
		sPush(true, iChanW);
		return true;
		}

	return false;
	}

// =================================================================================================
#pragma mark -

static const UTF32 spThreeQuotes[] = { '\"', '\"', '\"' };

static bool spPull_JSON_String_Push_UTF(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, const ChanW_UTF& iChanW)
	{
	ChanR_XX_Boundary<UTF32> theChanR_Boundary(spThreeQuotes, countof(spThreeQuotes), iChanR);
	int quotesSeen = 1;
	for (;;)
		{
		switch (quotesSeen)
			{
			case 0:
				{
				sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

				if (sTryRead_CP('"', iChanR, iChanU))
					quotesSeen = 1;
				else
					return true;
				break;
				}
			case 1:
				{
				if (sTryRead_CP('"', iChanR, iChanU))
					{
					// We have two quotes in a row.
					quotesSeen = 2;
					}
				else
					{
					const std::pair<int64,int64> result =
						sCopyAll(ChanR_UTF_Escaped('"', iChanR, iChanU), iChanW);

					if (sTryRead_CP('"', iChanR, iChanU))
						quotesSeen = 0;
					else if (result.first == 0)
						throw ParseException("Expected \" to close a string");
					}
				break;
				}
			case 2:
				{
				if (sTryRead_CP('"', iChanR, iChanU))
					{
					// We have three quotes in a row.
					quotesSeen = 3;
					if (ZQ<UTF32,false> theCPQ = sQRead(iChanR))
						{
						if (not Unicode::sIsEOL(*theCPQ))
							{
							// And the following character was *not* an EOL, so we'll treat it as
							// part of the string.
							sUnread(iChanU, *theCPQ);
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
					throw ParseException("Expected \"\"\" to close a string");
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

static bool spPull_JSON_String_Push(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, const ChanW_Any& iChanW)
	{
#if 1
	string theString;
	bool result = spPull_JSON_String_Push_UTF(iChanR, iChanU, ChanW_UTF_string<UTF8>(&theString));
	sPush(theString, iChanW);
	return result;
#else
	PullPushPair<UTF32> thePullPushPair = sMakePullPushPair<UTF32>();
	sPush(sGetClear(thePullPushPair.second), iChanW);

	bool result = spPull_JSON_String_Push_UTF(iChanR, iChanU, *thePullPushPair.first);
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
		throw ParseException("Expected '>' to close a base64 data");

	return true;
	}

// =================================================================================================
#pragma mark -

static bool spPull_Hex_Push_Bin(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, const ChanW_Bin& iChanW)
	{
	std::pair<int64,int64> counts = sCopyAll(ChanR_Bin_HexStrim(iChanR, iChanU), iChanW);
	if (counts.first != counts.second)
		return false;
	if (not sTryRead_CP('>', iChanR, iChanU))
		throw ParseException("Expected '>' to close a hex data");
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark sPull

bool sPull_JSON_Push(const ChanRU_UTF& iChanRU, const ReadOptions& iRO, const ChanW_Any& iChanW)
	{ return sPull_JSON_Push(iChanRU, iChanRU, iRO, iChanW); }

bool sPull_JSON_Push(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ReadOptions& iRO,
	const ChanW_Any& iChanW)
	{
	sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

	if (sTryRead_CP('[', iChanR, iChanU))
		{
		sPush(kStartSeq, iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
			if (sTryRead_CP(']', iChanR, iChanU))
				{
				sPush(kEnd, iChanW);
				return true;
				}

			if (not sPull_JSON_Push(iChanR, iChanU, iRO, iChanW))
				throw ParseException("Expected value");

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (sTryRead_CP(',', iChanR, iChanU))
				{}
			else if (iRO.fAllowSemiColons.DGet(false) && sTryRead_CP(';', iChanR, iChanU))
				{}
			else if (iRO.fLooseSeparators.DGet(false))
				{}
			else if (iRO.fAllowSemiColons.DGet(false))
				throw ParseException("Require ',' or ';' to separate array elements");
			else
				throw ParseException("Require ',' to separate array elements");
			}
		}
	else if (sTryRead_CP('{', iChanR, iChanU))
		{
		sPush(kStartMap, iChanW);
		for (;;)
			{
			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);
			if (sTryRead_CP('}', iChanR, iChanU))
				{
				sPush(kEnd, iChanW);
				return true;
				}

			string theName;
			if (not Util_Chan_JSON::sTryRead_PropertyName(iChanR, iChanU,
				theName, iRO.fAllowUnquotedPropertyNames.DGet(false)))
				{ throw ParseException("Expected a member name"); }

			sPush(sName(theName), iChanW);

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (not sTryRead_CP(':', iChanR, iChanU))
				{
				if (not iRO.fAllowEquals.DGet(false))
					throw ParseException("Expected ':' after a member name");

				if (not sTryRead_CP('=', iChanR, iChanU))
					throw ParseException("Expected ':' or '=' after a member name");
				}

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (not sPull_JSON_Push(iChanR, iChanU, iRO, iChanW))
				throw ParseException("Expected value");

			sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

			if (sTryRead_CP(',', iChanR, iChanU))
				{}
			else if (iRO.fAllowSemiColons.DGet(false) && sTryRead_CP(';', iChanR, iChanU))
				{}
			else if (iRO.fLooseSeparators.DGet(false))
				{}
			else if (iRO.fAllowSemiColons.DGet(false))
				throw ParseException("Require ',' or ';' to separate object elements");
			else
				throw ParseException("Require ',' to separate object elements");
			}
		}
	else if (sTryRead_CP('"', iChanR, iChanU))
		{
		// Could use YadStrimmerR_JSON and sPullPush_UTF, but this is a good chance to
		// demo how easy PullPush makes it to do fiddly source parsing.
		return spPull_JSON_String_Push(iChanR, iChanU, iChanW);
		}
	else if (iRO.fAllowBinary.DGet(false) && sTryRead_CP('<', iChanR, iChanU))
		{
		sSkip_WSAndCPlusPlusComments(iChanR, iChanU);

		PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
		sPush(sGetClear(thePullPushPair.second), iChanW);

		bool result;
		if (sTryRead_CP('=', iChanR, iChanU))
			result = spPull_Base64_Push_Bin(iChanR, *thePullPushPair.first);
		else
			result = spPull_Hex_Push_Bin(iChanR, iChanU, *thePullPushPair.first);

		sDisconnectWrite(*thePullPushPair.first);

		return result;
		}
	else
		{
		return spPull_JSON_Other_Push(iChanR, iChanU, iChanW);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark

static bool spPull_Push_JSON(const Any& iAny,
	const ChanR_Any& iChanR,
	size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW);

static bool spPull_Push_JSON_Seq(const ChanR_Any& iChanR,
	size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
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
			if (ZQ<Any,false> theNotQ = sQRead(iChanR))
				{
				return false;
				}
			else if (sPGet<PullPush::End>(*theNotQ))
				{
				break;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				sWriteLFIndent(iIndent, iOptions, iChanW);
				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent, iOptions, false, iChanW))
					return false;
				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				sWriteLFIndent(iIndent, iOptions, iChanW);
				if (iOptions.fNumberSequences.DGet(false))
					iChanW << "/*" << count << "*/";
				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent, iOptions, false, iChanW))
					return false;
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
			if (ZQ<Any,false> theNotQ = sQRead(iChanR))
				{
				return false;
				}
			else if (sPGet<PullPush::End>(*theNotQ))
				{
				break;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";
				if (not spPull_Push_JSON(*theNotQ, iChanR,
					iIndent, iOptions, iMayNeedInitialLF,
					iChanW))
					{
					return false;
					}
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
				if (not spPull_Push_JSON(*theNotQ, iChanR,
					iIndent, iOptions, iMayNeedInitialLF,
					iChanW))
					{
					return false;
					}
				}
			}
		iChanW << "]";
		}
	return true;
	}

static bool spPull_Push_JSON_Map(const ChanR_Any& iChanR,
	size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
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
			ZQ<Any> theNameOrEndQ = sQRead(iChanR);
			if (not theNameOrEndQ)
				return false;

			if (sPGet<PullPush::End>(*theNameOrEndQ))
				break;

			const Name* theNameStar = sPGet<Name>(*theNameOrEndQ);
			if (not theNameStar)
				return false;

			if (ZQ<Any,false> theNotQ = sQRead(iChanR))
				{
				return false;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				sWriteLFIndent(iIndent, iOptions, iChanW);
				Util_Chan_JSON::sWritePropName(*theNameStar, useSingleQuotes, iChanW);
				iChanW << " = ";

				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW))
					return false;

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				sWriteLFIndent(iIndent, iOptions, iChanW);
				Util_Chan_JSON::sWriteString(*theNameStar, useSingleQuotes, iChanW);
				iChanW << ": ";

				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW))
					return false;
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
			ZQ<Any> theNameOrEndQ = sQRead(iChanR);
			if (not theNameOrEndQ)
				return false;

			if (sPGet<PullPush::End>(*theNameOrEndQ))
				break;

			const Name* theNameStar = sPGet<Name>(*theNameOrEndQ);
			if (not theNameStar)
				return false;

			if (ZQ<Any,false> theNotQ = sQRead(iChanR))
				{
				return false;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";

				Util_Chan_JSON::sWritePropName(*theNameStar, useSingleQuotes, iChanW);
				if (sBreakStrings(iOptions))
					iChanW << " = ";
				else
					iChanW << "=";

				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW))
					return false;

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				if (sBreakStrings(iOptions))
					iChanW << " ";
				Util_Chan_JSON::sWriteString(*theNameStar, useSingleQuotes, iChanW);
				iChanW << ":";
				if (sBreakStrings(iOptions))
					iChanW << " ";

				if (not spPull_Push_JSON(*theNotQ, iChanR, iIndent + 1, iOptions, true, iChanW))
					return false;
				}
			wroteAny = true;
			}
		if (wroteAny && sBreakStrings(iOptions))
			iChanW << " ";
		iChanW << "}";
		}
	return true;
	}

bool sPull_Push_JSON(const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
	{ return sPull_Push_JSON(iChanR, 0, WriteOptions(), iChanW); }

bool sPull_Push_JSON(const ChanR_Any& iChanR,
	size_t iInitialIndent, const WriteOptions& iOptions,
	const ChanW_UTF& iChanW)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		spPull_Push_JSON(*theQ, iChanR, iInitialIndent, iOptions, false, iChanW);
		return true;
		}
	return false;
	}

static bool spPull_Push_JSON(const Any& iAny,
	const ChanR_Any& iChanR,
	size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanW_UTF& iChanW)
	{
	if (const string* theString = sPGet<string>(iAny))
		{
		Util_Chan_JSON::sWriteString(*theString, false, iChanW);
		return true;
		}

	if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iAny))
		{
		Util_Chan_JSON::sWriteString(*theChanner, iChanW);
		return true;
		}

	if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iAny))
		{
		Util_Chan_JSON::sPull_Bin_Push_JSON(*theChanner, iIndent, iOptions, iMayNeedInitialLF, iChanW);
		return true;
		}

	if (sPGet<PullPush::StartMap>(iAny))
		return spPull_Push_JSON_Map(iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);

	if (sPGet<PullPush::StartSeq>(iAny))
		return spPull_Push_JSON_Seq(iChanR, iIndent, iOptions, iMayNeedInitialLF, iChanW);

	Util_Chan_JSON::sWriteSimpleValue(iAny, iOptions, iChanW);

	return true;
	}

} // namespace ZooLib
