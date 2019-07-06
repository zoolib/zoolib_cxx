/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/Util_Chan_UTF.h"

#include "zoolib/Compat_cmath.h"
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/Chan_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/ChanR_XX_Boundary.h"
#include "zoolib/Unicode.h" // For Unicode::sIsEOL

#include <vector>

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
#pragma mark -

static bool spTryRead_Digit(const ChanRU_UTF& iChanRU, int& oDigit)
	{
	if (ZQ<int> theQ = sQRead_Digit(iChanRU))
		{
		oDigit = *theQ;
		return true;
		}
	return false;
	}

static bool spTryRead_HexDigit(const ChanRU_UTF& iChanRU, int& oDigit)
	{
	if (ZQ<int> theQ = sQRead_HexDigit(iChanRU))
		{
		oDigit = *theQ;
		return true;
		}
	return false;
	}

static bool spIsWhitespace(UTF32 iCP)
	{
	if (Unicode::sIsWhitespace(iCP))
		return true;

	if (iCP == 0xFEFF)
		{
		// BOM -- treat it as whitespace.
		return true;
		}

	return false;
	}

// -----------------

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator)
	{
	string8 result;
	sCopy_Until(iSource, iTerminator, ChanW_UTF_string8(&result));
	return result;
	}

// -----------------

bool sTryRead_CP(UTF32 iCP, const ChanRU_UTF& iChanRU)
	{
	// Ensure that we only try to read a valid CP, one that
	// can actually be returned by ReadCP.
	ZAssertStop(2, Unicode::sIsValid(iCP));

	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return false; }
	else if (*theCPQ == iCP)
		{ return true; }
	else
		{
		sUnread(iChanRU, *theCPQ);
		return false;
		}
	}

// -----------------

ZQ<int> sQRead_Digit(const ChanRU_UTF& iChanRU)
	{
	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return null; }
	else if (*theCPQ >= '0' && *theCPQ <= '9')
		{ return *theCPQ - '0'; }
	else
		{
		sUnread(iChanRU, *theCPQ);
		return null;
		}
	}

ZQ<int> sQRead_HexDigit(const ChanRU_UTF& iChanRU)
	{
	if (NotQ<UTF32> theCPQ = sQRead(iChanRU))
		{ return null; }
	else if (*theCPQ >= '0' && *theCPQ <= '9')
		{ return *theCPQ - '0'; }
	else if (*theCPQ >= 'a' && *theCPQ <= 'f')
		{ return *theCPQ - 'a' + 10; }
	else if (*theCPQ >= 'A' && *theCPQ <= 'F')
		{ return *theCPQ - 'A' + 10; }
	else
		{
		sUnread(iChanRU, *theCPQ);
		return null;
		}
	}

// -----------------

bool sRead_String(const string8& iPattern, const ChanR_UTF& iChanR)
	{
	for (string8::const_iterator iter = iPattern.begin(), iterEnd = iPattern.end();
		/*no test*/; /*no inc*/)
		{
		UTF32 patternCP;
		if (not Unicode::sReadInc(iter, iterEnd, patternCP))
			{
			// Exhausted the pattern, we've matched everything.
			return true;
			}

		if (NotQ<UTF32> targetCPQ = sQRead(iChanR))
			{
			// Exhausted the target before seeing the pattern.
			return false;
			}
		else if (*targetCPQ != patternCP)
			{
			// Mismatch.
			return false;
			}
		}
	}

// -----------------

bool sTryRead_String(const string8& iPattern, const ChanRU_UTF& iChanRU)
	{
	std::vector<UTF32> stack;

	for (string8::const_iterator iter = iPattern.begin(), iterEnd = iPattern.end();
		/*no test*/; /*no inc*/)
		{
		UTF32 targetCP;
		if (not Unicode::sReadInc(iter, iterEnd, targetCP))
			{
			// Exhausted target, and thus successful.
			return true;
			}

		if (NotQ<UTF32> candidateCPQ = sQRead(iChanRU))
			{
			// Exhausted strim.
			break;
			}
		else
			{
			stack.push_back(*candidateCPQ);
			if (targetCP != *candidateCPQ)
				{
				// Mismatched code points.
				break;
				}
			}
		}

	while (not stack.empty())
		{
		sUnread(iChanRU, stack.back());
		stack.pop_back();
		}
	return false;
	}

// -----------------

bool sTryRead_CaselessString(const string8& iPattern, const ChanRU_UTF& iChanRU)
	{
	std::vector<UTF32> stack;

	for (string8::const_iterator iter = iPattern.begin(), iterEnd = iPattern.end();
		/*no test*/; /*no inc*/)
		{
		UTF32 targetCP;
		if (not Unicode::sReadInc(iter, iterEnd, targetCP))
			{
			// Exhausted target, and thus successful.
			return true;
			}

		if (NotQ<UTF32> candidateCPQ = sQRead(iChanRU))
			{
			// Exhausted strim.
			break;
			}
		else
			{
			stack.push_back(*candidateCPQ);
			if (Unicode::sToLower(targetCP) != Unicode::sToLower(*candidateCPQ))
				{
				// Mismatched code points.
				break;
				}
			}
		}

	while (not stack.empty())
		{
		sUnread(iChanRU, stack.back());
		stack.pop_back();
		}
	return false;
	}

// -----------------

bool sTryRead_Sign(const ChanRU_UTF& iChanRU, bool& oIsNegative)
	{
	if (sTryRead_CP('-', iChanRU))
		{
		oIsNegative = true;
		return true;
		}
	else if (sTryRead_CP('+', iChanRU))
		{
		oIsNegative = false;
		return true;
		}
	return false;
	}

static bool spTryRead_HexInteger(const ChanRU_UTF& iChanRU, int64& oInt64)
	{
	oInt64 = 0;
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not spTryRead_HexDigit(iChanRU, curDigit))
			return gotAny;
		oInt64 *= 16;
		oInt64 += curDigit;
		}
	}

bool sTryRead_HexInteger(const ChanRU_UTF& iChanRU, int64& oInt64)
	{ return spTryRead_HexInteger(iChanRU, oInt64); }

static bool spTryRead_Mantissa(const ChanRU_UTF& iChanRU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	using namespace Util_Chan;

	oInt64 = 0;
	oDouble = 0;
	oIsDouble = false;

	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not spTryRead_Digit(iChanRU, curDigit))
			return gotAny;

		if (not oIsDouble)
			{
			int64 priorInt64 = oInt64;
			oInt64 *= 10;
			oInt64 += curDigit;
			if (oInt64 < priorInt64)
				{
				// We've overflowed.
				oIsDouble = true;
				}
			}
		oDouble *= 10;
		oDouble += curDigit;
		}
	}

static bool spTryRead_DecimalInteger(const ChanRU_UTF& iChanRU,
	int64& oInt64)
	{
	oInt64 = 0;
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not spTryRead_Digit(iChanRU, curDigit))
			return gotAny;
		oInt64 *= 10;
		oInt64 += curDigit;
		}
	}

bool sTryRead_DecimalInteger(const ChanRU_UTF& iChanRU,
	int64& oInt64)
	{ return spTryRead_DecimalInteger(iChanRU, oInt64); }

static bool spTryRead_SignedDecimalInteger(const ChanRU_UTF& iChanRU,
	int64& oInt64)
	{
	bool isNegative = false;
	bool hadSign = sTryRead_Sign(iChanRU, isNegative);
	if (spTryRead_DecimalInteger(iChanRU, oInt64))
		{
		if (isNegative)
			oInt64 = -oInt64;
		return true;
		}

	if (hadSign)
		sThrow_ParseException("Expected a valid integer after sign prefix");

	return false;
	}

bool sTryRead_SignedDecimalInteger(const ChanRU_UTF& iChanRU,
	int64& oInt64)
	{ return spTryRead_SignedDecimalInteger(iChanRU, oInt64); }

static bool spTryRead_DecimalNumber(const ChanRU_UTF& iChanRU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	using namespace Util_Chan;

	if (sTryRead_CaselessString("nan", iChanRU))
		{
		oIsDouble = true;
		oDouble = NAN;
		return true;
		}

	if (sTryRead_CaselessString("inf", iChanRU))
		{
		oIsDouble = true;
		oDouble = INFINITY;
		return true;
		}

	if (not spTryRead_Mantissa(iChanRU, oInt64, oDouble, oIsDouble))
		return false;

	if (sTryRead_CP('.', iChanRU))
		{
		oIsDouble = true;
		double fracPart = 0.0;
		double divisor = 1.0;

		for (;;)
			{
			int curDigit;
			if (not spTryRead_Digit(iChanRU, curDigit))
				break;
			divisor *= 10;
			fracPart *= 10;
			fracPart += curDigit;
			}
		oDouble += fracPart / divisor;
		}

	if (sTryRead_CP('e', iChanRU) || sTryRead_CP('E', iChanRU))
		{
		oIsDouble = true;
		int64 exponent;
		if (not spTryRead_SignedDecimalInteger(iChanRU, exponent))
			sThrow_ParseException("Expected a valid exponent after 'e'");
		oDouble = oDouble * pow(10.0, int(exponent));
		}

	return true;
	}

bool sTryRead_SignedGenericNumber(const ChanRU_UTF& iChanRU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	oIsDouble = false;
	bool isNegative = false;
	bool hadSign = sTryRead_Sign(iChanRU, isNegative);
	if (sTryRead_CP('0', iChanRU))
		{
		UTF32 theCP;
		if (not sQRead(iChanRU, theCP))
			{
			oInt64 = 0;
			return true;
			}

		if (theCP == 'X' || theCP == 'x')
			{
			if (spTryRead_HexInteger(iChanRU, oInt64))
				{
				if (isNegative)
					oInt64 = -oInt64;
				return true;
				}
			sThrow_ParseException("Expected a valid hex integer after '0x' prefix");
			}

		sUnread(iChanRU, theCP);

		if (theCP != '.' and not Unicode::sIsDigit(theCP))
			{
			oInt64 = 0;
			return true;
			}

		sUnread(iChanRU, UTF32('0'));
		}

	if (spTryRead_DecimalNumber(iChanRU, oInt64, oDouble, oIsDouble))
		{
		if (isNegative)
			{
			oInt64 = -oInt64;
			oDouble = -oDouble;
			}
		return true;
		}

	if (hadSign)
		{
		// We've already absorbed a plus or minus sign, hence we have a parse exception.
		if (isNegative)
			sThrow_ParseException("Expected a valid number after '-' prefix");
		else
			sThrow_ParseException("Expected a valid number after '+' prefix");
		}

	return false;
	}

// -----------------

bool sSkip_WS(const ChanRU_UTF& iChanRU)
	{
	bool readAny = false;
	while (ZQ<UTF32> theCPQ = sQRead(iChanRU))
		{
		readAny = true;
		if (not spIsWhitespace(*theCPQ))
			{
			sUnread(iChanRU, *theCPQ);
			break;
			}
		}
	return readAny;
	}

// -----------------

bool sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF& oDest)
	{
	bool readAny = false;
	while (ZQ<UTF32> theCPQ = sQRead(iSource))
		{
		readAny = true;
		if (Unicode::sIsEOL(*theCPQ))
			break;
		sEWrite(oDest, *theCPQ);
		}
	return readAny;
	}

bool sSkip_Line(const ChanR_UTF& iSource)
	{ return sCopy_Line(iSource, ChanW_XX_Discard<UTF32>()); }

ZQ<string8> sQRead_Line(const ChanR_UTF& iSource)
	{
	string8 result;
	if (sCopy_Line(iSource, ChanW_UTF_string8(&result)))
		return result;
	return null;
	}

// -----------------

bool sCopy_WSAndCPlusPlusComments(const ChanRU_UTF& iChanRU,
	const ChanW_UTF& oDest)
	{
//	ZAssert(sUnreadableLimit(iChanRU) >= 2);

	bool readAny = false;
	for (;;)
		{
		if (ZQ<UTF32> firstCPQ = sQRead(iChanRU))
			{
			readAny = true;
			if (spIsWhitespace(*firstCPQ))
				{
				sEWrite(oDest, *firstCPQ);
				continue;
				}
			else if (*firstCPQ == '/')
				{
				if (sTryRead_CP('/', iChanRU))
					{
					sEWrite(oDest, "//");
					sCopy_Line(iChanRU, oDest);
					continue;
					}
				else if (sTryRead_CP('*', iChanRU))
					{
					sEWrite(oDest, "/*");
					if (not sCopy_Until(iChanRU, "*/", oDest))
						sThrow_ParseException("Unexpected end of data while parsing a /**/ comment");
					sEWrite(oDest, "*/");
					continue;
					}
				}
			sUnread(iChanRU, *firstCPQ);
			}
		break;
		}
	return readAny;
	}

bool sSkip_WSAndCPlusPlusComments(const ChanRU_UTF& iChanRU)
	{ return sCopy_WSAndCPlusPlusComments(iChanRU, ChanW_XX_Discard<UTF32>()); }

// -----------------

static bool spCopy_Until(const ChanR_UTF& iChanR, const string32& iTerminator,
	const ChanW_UTF& oDest)
	{
	ChanR_XX_Boundary<UTF32> chanR(iTerminator.begin(), iTerminator.end(), iChanR);
	sCopyAll(chanR, oDest);
	return chanR.HitBoundary();
	}

bool sCopy_Until(const ChanR_UTF& iChanR, const string8& iTerminator,
	const ChanW_UTF& oDest)
	{ return spCopy_Until(iChanR, Unicode::sAsUTF32(iTerminator), oDest); }

bool sSkip_Until(const ChanR_UTF& iChanR, const string8& iTerminator)
	{ return sCopy_Until(iChanR, iTerminator, ChanW_XX_Discard<UTF32>()); }

bool sRead_Until(const ChanR_UTF& iChanR, const string8& iTerminator,
	string8& oString)
	{
	oString.resize(0);
	return sCopy_Until(iChanR, iTerminator, ChanW_UTF_string8(&oString));
	}

// -----------------

void sCopy_EscapedString(UTF32 iTerminator, const ChanRU_UTF& iChanRU,
	const ChanW_UTF& oDest)
	{ sCopyAll(ChanR_UTF_Escaped(iTerminator, iChanRU), oDest); }

void sRead_EscapedString(UTF32 iTerminator, const ChanRU_UTF& iChanRU,
	string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	sCopy_EscapedString(iTerminator, iChanRU, ChanW_UTF_string8(&oString));
	}

// -----------------

bool sTryCopy_EscapedString(UTF32 iDelimiter, const ChanRU_UTF& iChanRU,
	const ChanW_UTF& oDest)
	{
	if (not sTryRead_CP(iDelimiter, iChanRU))
		return false;

	sCopy_EscapedString(iDelimiter, iChanRU, oDest);

	if (not sTryRead_CP(iDelimiter, iChanRU))
		sThrow_ParseException("Missing string delimiter");

	return true;
	}

bool sTryRead_EscapedString(UTF32 iDelimiter, const ChanRU_UTF& iChanRU,
	string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	return sTryCopy_EscapedString(iDelimiter, iChanRU, ChanW_UTF_string8(&oString));
	}

// -----------------

// =================================================================================================
#pragma mark -

void sWriteExact(const ChanW_UTF& iChanW, float iVal)
	{
	// 9 decimal digits are necessary and sufficient for single precision IEEE 754.
	// "What Every Computer Scientist Should Know About Floating Point", Goldberg, 1991.
	// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
	sEWritef(iChanW, "%.9g", iVal);
	}

void sWriteExact(const ChanW_UTF& iChanW, double iVal)
	{
	// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
	sEWritef(iChanW, "%.17g", iVal);
	}

void sWriteExact(const ChanW_UTF& iChanW, long double iVal)
	{
	// This is a guess for now.
	sEWritef(iChanW, "%.34Lg", iVal);
	}

} // namespace Util_Chan
} // namespace ZooLib
