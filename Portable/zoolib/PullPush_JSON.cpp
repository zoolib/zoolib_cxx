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
#include "zoolib/Util_Chan_UTF_Operators.h"


#if ZCONFIG(Compiler,GCC)
	#include <cxxabi.h>
	#include <stdlib.h> // For free, required by __cxa_demangle
#endif

namespace ZooLib {
namespace PullPush_JSON {

using std::min;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark Helpers

static bool spEmitSeq(size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW);

static bool spEmitMap(size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW);

static bool spEmit(const Any& iIany, size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW);

// =================================================================================================
#pragma mark -
#pragma mark sPush

bool sPullPush(const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
	{ return sPullPush(0, WriteOptions(), iChanR, iChanW); }

bool sPullPush(size_t iInitialIndent, const WriteOptions& iOptions,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
	{
	if (ZQ<Any> theQ = sQRead(iChanR))
		{
		spEmit(*theQ, iInitialIndent, iOptions, false, iChanR, iChanW);
		return true;
		}
	return false;
	}

static bool spEmit(const Any& iAny, size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
	{
	if (ZRef<ChannerR_UTF> theChanner = sGet<ZRef<ChannerR_UTF>>(iAny))
		{
		Yad_JSON::spWriteString(*theChanner, iChanW);
		return true;
		}

	if (ZRef<ChannerR_Bin> theChanner = sGet<ZRef<ChannerR_Bin>>(iAny))
		{
		Yad_JSON::spToStrim_Stream(*theChanner, iIndent, iOptions, iMayNeedInitialLF, iChanW);
		return true;
		}

	if (sPGet<PullPush::StartMap>(iAny))
		{
		return spEmitMap(iIndent, iOptions, iMayNeedInitialLF, iChanR, iChanW);
		}

	if (sPGet<PullPush::StartSeq>(iAny))
		{
		return spEmitSeq(iIndent, iOptions, iMayNeedInitialLF, iChanR, iChanW);
		}

	Yad_JSON::spToStrim_SimpleValue(iAny, iOptions, iChanW);
	return true;
	}

static bool spEmitSeq(size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
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
			spWriteLFIndent(iIndent, iOptions, iChanW);
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
			else if (false && iOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(iIndent, iOptions, iChanW);
				if (not spEmit(*theNotQ, iIndent, iOptions, false, iChanR, iChanW))
					return false;
				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				spWriteLFIndent(iIndent, iOptions, iChanW);
				if (iOptions.fNumberSequences.DGet(false))
					iChanW << "/*" << count << "*/";
				if (not spEmit(*theNotQ, iIndent, iOptions, false, iChanR, iChanW))
					return false;
				}
			++count;
			}
		spWriteLFIndent(iIndent, iOptions, iChanW);
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
			else if (false && iOptions.fUseExtendedNotation.DGet(false))
				{
				if (not isFirst && sBreakStrings(iOptions))
					iChanW << " ";
				if (not spEmit(*theNotQ, iIndent, iOptions, iMayNeedInitialLF, iChanR, iChanW))
					return false;
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
				if (not spEmit(*theNotQ, iIndent, iOptions, iMayNeedInitialLF, iChanR, iChanW))
					return false;
				}
			}
		iChanW << "]";
		}
	return true;
	}

bool spEmitMap(size_t iIndent, const WriteOptions& iOptions, bool iMayNeedInitialLF,
	const ChanR_Any& iChanR, const ChanW_UTF& iChanW)
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
			spWriteLFIndent(iIndent, iOptions, iChanW);
			}

		iChanW << "{";
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			ZQ<Any> theNameQ = sQRead(iChanR);
			if (not theNameQ)
				return false;

			if (sPGet<PullPush::End>(*theNameQ))
				break;

			const Name* theNameStar = sPGet<Name>(*theNameQ);
			if (not theNameStar)
				return false;

			if (ZQ<Any,false> theNotQ = sQRead(iChanR))
				{
				return false;
				}
			else if (iOptions.fUseExtendedNotation.DGet(false))
				{
				spWriteLFIndent(iIndent, iOptions, iChanW);
				Yad_JSON::spWritePropName(*theNameStar, useSingleQuotes, iChanW);
				iChanW << " = ";

				if (not spEmit(*theNotQ, iIndent + 1, iOptions, true, iChanR, iChanW))
					return false;

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				spWriteLFIndent(iIndent, iOptions, iChanW);
				Yad_JSON::spWriteString(*theNameStar, useSingleQuotes, iChanW);
				iChanW << ": ";

				if (not spEmit(*theNotQ, iIndent + 1, iOptions, true, iChanR, iChanW))
					return false;
				}
			}
		spWriteLFIndent(iIndent, iOptions, iChanW);
		iChanW << "}";
		}
	else
		{
		iChanW << "{";
		bool wroteAny = false;
		for (bool isFirst = true; /*no test*/ ; isFirst = false)
			{
			ZQ<Any> theNameQ = sQRead(iChanR);
			if (not theNameQ)
				return false;

			if (sPGet<PullPush::End>(*theNameQ))
				break;

			const Name* theNameStar = sPGet<Name>(*theNameQ);
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

				Yad_JSON::spWritePropName(*theNameStar, useSingleQuotes, iChanW);
				if (sBreakStrings(iOptions))
					iChanW << " = ";
				else
					iChanW << "=";

				if (not spEmit(*theNotQ, iIndent + 1, iOptions, true, iChanR, iChanW))
					return false;

				iChanW << ";";
				}
			else
				{
				if (not isFirst)
					iChanW << ",";
				if (sBreakStrings(iOptions))
					iChanW << " ";
				Yad_JSON::spWriteString(*theNameStar, useSingleQuotes, iChanW);
				iChanW << ":";
				if (sBreakStrings(iOptions))
					iChanW << " ";

				if (not spEmit(*theNotQ, iIndent + 1, iOptions, true, iChanR, iChanW))
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

} // namespace PullPush_JSON
} // namespace ZooLib
