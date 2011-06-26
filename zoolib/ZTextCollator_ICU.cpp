/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZTextCollator_ICU.h"

#if ZCONFIG_API_Enabled(TextCollator_ICU)

#include "zoolib/ZDebug.h"
#include "zoolib/ZFunctionChain.h"

#include <stdexcept>

#include "unicode/ucol.h"
#include "unicode/usearch.h"

#ifndef kDebug_ICU
#	define kDebug_ICU 1
#endif

using std::string;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace { // anonymous

class Make_Collator
:	public ZFunctionChain_T<ZRef<ZTextCollatorRep>, const ZTextCollatorRep::Param_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (iParam.fLocaleName.empty())
			oResult = new ZTextCollatorRep_ICU(nullptr, iParam.fStrength);
		else
			oResult = new ZTextCollatorRep_ICU(iParam.fLocaleName.c_str(), iParam.fStrength);
		return true;
		}
	} sMaker0;

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTextCollatorRep_ICU

ZAssertCompile(UCOL_EQUAL == 0);
ZAssertCompile(UCOL_GREATER > 0);
ZAssertCompile(UCOL_LESS < 0);

ZTextCollatorRep_ICU::ZTextCollatorRep_ICU(const char* iLocaleName, int iStrength)
	{
	UErrorCode status = U_ZERO_ERROR;
	fCollator = ::ucol_open(iLocaleName, &status);

	if (U_FAILURE(status))
		{
		if (iLocaleName)
			{
			throw std::runtime_error(
				string("Couldn't open ICU collator for locale: ") + iLocaleName);
			}
		else
			{
			throw std::runtime_error("Couldn't open ICU collator for default locale");
			}
		}

	UCollationStrength realStrength;
	switch (iStrength)
		{
		case 1: realStrength = UCOL_PRIMARY; break;
		case 2: realStrength = UCOL_SECONDARY; break;
		case 3: realStrength = UCOL_TERTIARY; break;
		case 4: realStrength = UCOL_QUATERNARY; break;
		default: realStrength = UCOL_IDENTICAL; break;
		}

	::ucol_setStrength(fCollator, realStrength);
	}

ZTextCollatorRep_ICU::ZTextCollatorRep_ICU(UCollator* iCollator)
:	fCollator(iCollator)
	{
	ZAssertStop(kDebug_ICU, fCollator);
	}

ZTextCollatorRep_ICU::~ZTextCollatorRep_ICU()
	{
	ZAssertStop(kDebug_ICU, fCollator);
	::ucol_close(fCollator);
	}

ZRef<ZTextCollatorRep> ZTextCollatorRep_ICU::Clone()
	{
	UErrorCode status = U_ZERO_ERROR;
	UCollator* clonedColl = ::ucol_safeClone(fCollator, nullptr, 0, &status);
	if (U_FAILURE(status))
		throw std::runtime_error("Couldn't clone collator");
	return new ZTextCollatorRep_ICU(clonedColl);
	}

bool ZTextCollatorRep_ICU::Equals(const string16& iLeft, const string16& iRight)
	{
	if (iLeft.empty())
		return iRight.empty();

	if (iRight.empty())
		return false;

	return UCOL_EQUAL == ::ucol_strcoll(fCollator, iLeft.data(), iLeft.size(),
		iRight.data(), iRight.size());
	}

bool ZTextCollatorRep_ICU::Equals(const UTF16* iLeft, size_t iLeftLength,
	const UTF16* iRight, size_t iRightLength)
	{
	if (not iLeftLength)
		return !iRightLength;

	if (not iRightLength)
		return false;

	return UCOL_EQUAL == ::ucol_strcoll(fCollator, iLeft, iLeftLength, iRight, iRightLength);
	}

int ZTextCollatorRep_ICU::Compare(const string16& iLeft, const string16& iRight)
	{
	if (iLeft.empty())
		{
		if (iRight.empty())
			return 0;
		return -1;
		}

	if (iRight.empty())
		return 1;

	return ::ucol_strcoll(fCollator, iLeft.data(), iLeft.size(), iRight.data(), iRight.size());
	}

int ZTextCollatorRep_ICU::Compare(const UTF16* iLeft, size_t iLeftLength,
	const UTF16* iRight, size_t iRightLength)
	{
	if (not iLeftLength)
		{
		if (not iRightLength)
			return 0;
		return -1;
		}

	if (not iRightLength)
		return 1;

	return ::ucol_strcoll(fCollator, iLeft, iLeftLength, iRight, iRightLength);
	}

static bool spContains(UCollator* iCollator, const UTF16* iPattern, size_t iPatternLength,
	const UTF16* iTarget, size_t iTargetLength)
	{
	UErrorCode status = U_ZERO_ERROR;
	UStringSearch* theSearch = ::usearch_openFromCollator(iPattern, iPatternLength,
		iTarget, iTargetLength, iCollator, nullptr, &status);

	if (U_FAILURE(status))
		return false;

	int pos = ::usearch_next(theSearch, &status);

	::usearch_close(theSearch);

	if (U_FAILURE(status))
		return false;

	return pos != USEARCH_DONE;
	}

bool ZTextCollatorRep_ICU::Contains(const string16& iPattern, const string16& iTarget)
	{
	if (iPattern.empty())
		return true;

	if (iTarget.empty())
		return false;

	return spContains(fCollator, iPattern.data(), iPattern.size(), iTarget.data(), iTarget.size());
	}

bool ZTextCollatorRep_ICU::Contains(const UTF16* iPattern, size_t iPatternLength,
	const UTF16* iTarget, size_t iTargetLength)
	{
	if (not iPatternLength)
		return !true;

	if (not iTargetLength)
		return false;

	return spContains(fCollator, iPattern, iPatternLength, iTarget, iTargetLength);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCollator_ICU)
