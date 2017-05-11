/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZTextCollator_ASCII.h"

#include "zoolib/ZDebug.h"

ZMACRO_MSVCStaticLib_cpp(TextCollator_ASCII)

using std::min;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Factory functions

namespace { // anonymous

class Make_Collator
:	public FunctionChain<ZRef<ZTextCollatorRep>, const ZTextCollatorRep::Param_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (iParam.fLocaleName.empty())
			{
			oResult = new ZTextCollatorRep_ASCII(iParam.fStrength);
			return true;
			}
		return false;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Helper functions

static int spCompare_CaseSensitive(const UTF8* iLeft, size_t iLeftLength,
	const UTF8* iRight, size_t iRightLength)
	{
	for (size_t count = min(iLeftLength, iRightLength); count; --count)
		{
		if (int compare = int(*iLeft++) - int(*iRight++))
			return compare;
		}
	if (iLeftLength < iRightLength)
		return -1;
	if (iLeftLength > iRightLength)
		return 1;
	return 0;
	}

static int spCompare_CaseInsensitive(const UTF8* iLeft, size_t iLeftLength,
	const UTF8* iRight, size_t iRightLength)
	{
	for (size_t count = min(iLeftLength, iRightLength); count; --count)
		{
		if (int compare = int(tolower(*iLeft++)) - int(tolower(*iRight++)))
			return compare;
		}
	if (iLeftLength < iRightLength)
		return -1;
	if (iLeftLength > iRightLength)
		return 1;
	return 0;
	}

static int spCompare(int iStrength, const UTF8* iLeft, size_t iLeftLength,
	const UTF8* iRight, size_t iRightLength)
	{
	if (iStrength == 1)
		return spCompare_CaseInsensitive(iLeft, iLeftLength, iRight, iRightLength);
	else
		return spCompare_CaseSensitive(iLeft, iLeftLength, iRight, iRightLength);
	}

static bool spContains(int iStrength, const UTF8* iPattern, size_t iPatternLength,
	const UTF8* iTarget, size_t iTargetLength)
	{
	// Dumb implementation for now.
	for (size_t patIndex = 0, patEnd = iTargetLength - iPatternLength;
		patIndex <= patEnd; ++patIndex)
		{
		if (0 == spCompare(iStrength, iPattern, iPatternLength, iTarget + patIndex, iPatternLength))
			return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark ZTextCollatorRep_ASCII

ZTextCollatorRep_ASCII::ZTextCollatorRep_ASCII(int iStrength)
	{ fStrength = iStrength; }

ZTextCollatorRep_ASCII::~ZTextCollatorRep_ASCII()
	{}

ZRef<ZTextCollatorRep> ZTextCollatorRep_ASCII::Clone()
	{ return new ZTextCollatorRep_ASCII(fStrength); }

ZRef<ZTextCollatorRep> ZTextCollatorRep_ASCII::SafeCopy()
	{ return this; }

bool ZTextCollatorRep_ASCII::Equals(const string8& iLeft, const string8& iRight)
	{
	if (iLeft.empty())
		return iRight.empty();

	if (iRight.empty())
		return false;

	return 0 == spCompare(fStrength, iLeft.data(), iLeft.size(), iRight.data(), iRight.size());
	}

bool ZTextCollatorRep_ASCII::Equals(const UTF8* iLeft, size_t iLeftLength, const
	UTF8* iRight, size_t iRightLength)
	{
	if (not iLeftLength)
		return !iRightLength;

	if (not iRightLength)
		return false;

	return 0 == spCompare(fStrength, iLeft, iLeftLength, iRight, iRightLength);
	}

int ZTextCollatorRep_ASCII::Compare(const string8& iLeft, const string8& iRight)
	{
	if (iLeft.empty())
		{
		if (iRight.empty())
			return 0;
		return -1;
		}

	if (iRight.empty())
		return 1;

	return spCompare(fStrength, iLeft.data(), iLeft.size(), iRight.data(), iRight.size());
	}

int ZTextCollatorRep_ASCII::Compare(const UTF8* iLeft, size_t iLeftLength,
	const UTF8* iRight, size_t iRightLength)
	{
	if (not iLeftLength)
		{
		if (not iRightLength)
			return 0;
		return -1;
		}

	if (not iRightLength)
		return 1;

	return spCompare(fStrength, iLeft, iLeftLength, iRight, iRightLength);
	}

bool ZTextCollatorRep_ASCII::Contains(const string8& iPattern, const string8& iTarget)
	{
	if (iPattern.empty())
		return true;

	if (iPattern.size() > iTarget.size())
		return false;

	return spContains(fStrength, iPattern.data(), iPattern.size(), iTarget.data(), iTarget.size());
	}

bool ZTextCollatorRep_ASCII::Contains(const UTF8* iPattern, size_t iPatternLength,
	const UTF8* iTarget, size_t iTargetLength)
	{
	if (not iPatternLength)
		return true;

	if (iPatternLength > iTargetLength)
		return false;

	return spContains(fStrength, iPattern, iPatternLength, iTarget, iTargetLength);
	}

} // namespace ZooLib
