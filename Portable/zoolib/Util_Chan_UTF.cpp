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

#include "zoolib/Compat_cmath.h"
#include "zoolib/Chan_UTF_Escaped.h"
#include "zoolib/Chan_UTF_string.h" // For ChanW_UTF_string8
#include "zoolib/Unicode.h" // For Unicode::sIsEOL
#include "zoolib/Util_Chan_UTF.h"

#include <vector>

namespace ZooLib {
namespace Util_Chan {

// =================================================================================================
// MARK: -

static bool spIsWhitespace(UTF32 iCP)
	{
	if (Unicode::sIsWhitespace(iCP))
		return true;

	if (iCP == 0xFEFF)
		return true;

	return false;
	}

// -----------------

string8 sRead_Until(const ChanR_UTF& iSource, UTF32 iTerminator)
	{
	string8 result;
	sCopy_Until<UTF32>(iSource, ChanW_UTF_string8(&result), iTerminator);
	return result;
	}

// -----------------

bool sTryRead_CP(UTF32 iCP, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	// Ensure that we only try to read a valid CP, one that
	// can actually be returned by ReadCP.
	ZAssertStop(2, Unicode::sIsValid(iCP));

	if (ZQ<UTF32,false> theCPQ = sQRead(iChanR))
		{ return false; }
	else if (*theCPQ == iCP)
		{ return true; }
	else
		{
		sUnread(*theCPQ, iChanU);
		return false;
		}
	}

// -----------------

ZQ<int> sQRead_Digit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	if (ZQ<UTF32,false> theCPQ = sQRead(iChanR))
		{ return null; }
	else if (*theCPQ >= '0' && *theCPQ <= '9')
		{ return *theCPQ - '0'; }
	else
		{
		sUnread(*theCPQ, iChanU);
		return null;
		}
	}

ZQ<int> sQRead_HexDigit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	if (ZQ<UTF32,false> theCPQ = sQRead(iChanR))
		{ return null; }
	else if (*theCPQ >= '0' && *theCPQ <= '9')
		{ return *theCPQ - '0'; }
	else if (*theCPQ >= 'a' && *theCPQ <= 'f')
		{ return *theCPQ - 'a' + 10; }
	else if (*theCPQ >= 'A' && *theCPQ <= 'F')
		{ return *theCPQ - 'A' + 10; }
	else
		{
		sUnread(*theCPQ, iChanU);
		return null;
		}
	}

// -----------------

bool sTryRead_Digit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int& oDigit)
	{
	if (ZQ<int> theQ = sQRead_Digit(iChanR, iChanU))
		{
		oDigit = *theQ;
		return true;
		}
	return false;
	}

bool sTryRead_HexDigit(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int& oDigit)
	{
	if (ZQ<int> theQ = sQRead_HexDigit(iChanR, iChanU))
		{
		oDigit = *theQ;
		return true;
		}
	return false;
	}

static bool sQRead(UTF32& oCP, ChanR_UTF& iChanR)
	{
	if (ZQ<UTF32> theQ = sQRead(iChanR))
		{
		oCP = *theQ;
		return true;
		}
	return false;
	}

bool sTryRead_Sign(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, bool& oIsNegative)
	{
	if (sTryRead_CP( '-', iChanR, iChanU))
		{
		oIsNegative = true;
		return true;
		}
	else if (sTryRead_CP('+', iChanR, iChanU))
		{
		oIsNegative = false;
		return true;
		}
	return false;
	}

bool sTryRead_SignedGenericInteger(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int64& oInt64)
	{
	bool isNegative = false;
	bool hadPrefix = sTryRead_Sign(iChanR, iChanU, isNegative);

	if (sTryRead_CP('0', iChanR, iChanU))
		{
		UTF32 theCP;
		if (not sQRead(theCP, iChanR))
			{
			oInt64 = 0;
			return true;
			}

		if (theCP == 'X' || theCP == 'x')
			{
			if (sTryRead_HexInteger(iChanR, iChanU, oInt64))
				{
				if (isNegative)
					oInt64 = -oInt64;
				return true;
				}
			throw ParseException("Expected a valid hex integer after '0x' prefix");
			}

		sUnread(theCP, iChanU);
		if (not Unicode::sIsDigit(theCP))
			{
			oInt64 = 0;
			return true;
			}

		/*bool readDecimal = */sTryRead_DecimalInteger(iChanR, iChanU, oInt64);
		// We know that the first CP to be read is a digit, so sTryRead_DecimalInteger can't fail.
		if (isNegative)
			oInt64 = -oInt64;
		return true;
		}

	if (sTryRead_DecimalInteger(iChanR, iChanU, oInt64))
		{
		if (isNegative)
			oInt64 = -oInt64;
		return true;
		}

	if (hadPrefix)
		{
		// We've already absorbed a plus or minus sign, hence we have a parse exception.
		if (isNegative)
			throw ParseException("Expected a valid integer after '-' prefix");
		else
			throw ParseException("Expected a valid integer after '+' prefix");
		}

	return false;
	}

bool sTryRead_HexInteger(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int64& oInt64)
	{
	oInt64 = 0;
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not sTryRead_HexDigit(iChanR, iChanU, curDigit))
			return gotAny;
		oInt64 *= 16;
		oInt64 += curDigit;
		}
	}

bool sTryRead_Mantissa(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	using namespace Util_Chan;

	oInt64 = 0;
	oDouble = 0;
	oIsDouble = false;

	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not sTryRead_Digit(iChanR, iChanU, curDigit))
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

bool sTryRead_DecimalInteger(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int64& oInt64)
	{
	oInt64 = 0;
	for (bool gotAny = false; /*no test*/; gotAny = true)
		{
		int curDigit;
		if (not sTryRead_Digit(iChanR, iChanU, curDigit))
			return gotAny;
		oInt64 *= 10;
		oInt64 += curDigit;
		}
	}

bool sTryRead_SignedDecimalInteger(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, int64& oInt64)
	{
	bool isNegative = false;
	bool hadSign = sTryRead_Sign(iChanR, iChanU, isNegative);
	if (sTryRead_DecimalInteger(iChanR, iChanU, oInt64))
		{
		if (isNegative)
			oInt64 = -oInt64;
		return true;
		}

	if (hadSign)
		throw ParseException("Expected a valid integer after sign prefix");

	return false;
	}

bool sTryRead_DecimalNumber(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	using namespace Util_Chan;

	if (sTryRead_CaselessString("nan", iChanR, iChanU))
		{
		oIsDouble = true;
		oDouble = NAN;
		return true;
		}

	if (sTryRead_CaselessString("inf", iChanR, iChanU))
		{
		oIsDouble = true;
		oDouble = INFINITY;
		return true;
		}

	if (not sTryRead_Mantissa(iChanR, iChanU, oInt64, oDouble, oIsDouble))
		return false;

	if (sTryRead_CP('.', iChanR, iChanU))
		{
		oIsDouble = true;
		double fracPart = 0.0;
		double divisor = 1.0;

		for (;;)
			{
			int curDigit;
			if (not sTryRead_Digit(iChanR, iChanU, curDigit))
				break;
			divisor *= 10;
			fracPart *= 10;
			fracPart += curDigit;
			}
		oDouble += fracPart / divisor;
		}

	if (sTryRead_CP('e', iChanR, iChanU) || sTryRead_CP('E', iChanR, iChanU))
		{
		oIsDouble = true;
		int64 exponent;
		if (not sTryRead_SignedDecimalInteger(iChanR, iChanU, exponent))
			throw ParseException("Expected a valid exponent after 'e'");
		oDouble = oDouble * pow(10.0, int(exponent));
		}

	return true;
	}

bool sTryRead_SignedDecimalNumber(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	int64& oInt64, double& oDouble, bool& oIsDouble)
	{
	bool isNegative = false;
	bool hadSign = sTryRead_Sign(iChanR, iChanU, isNegative);
	if (sTryRead_DecimalNumber(iChanR, iChanU, oInt64, oDouble, oIsDouble))
		{
		if (isNegative)
			{
			oInt64 = -oInt64;
			oDouble = -oDouble;
			}
		return true;
		}

	if (hadSign)
		throw ParseException("Expected a valid number after sign prefix");

	return false;
	}

bool sTryRead_Double(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, double& oDouble)
	{
	int64 dummyInt64;
	bool isDouble;
	return sTryRead_DecimalNumber(iChanR, iChanU, dummyInt64, oDouble, isDouble);
	}

bool sTryRead_SignedDouble(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU, double& oDouble)
	{
	int64 dummyInt64;
	bool isDouble;
	return sTryRead_SignedDecimalNumber(iChanR, iChanU, dummyInt64, oDouble, isDouble);
	}

// -----------------

bool sTryRead_CaselessString(const string8& iTarget,
	const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{
	string8::const_iterator targetIter = iTarget.begin();
	string8::const_iterator targetEnd = iTarget.end();

	std::vector<UTF32> stack;
	for (;;)
		{
		UTF32 targetCP;
		if (not Unicode::sReadInc(targetIter, targetEnd, targetCP))
			{
			// Exhausted target, and thus successful.
			return true;
			}

		if (ZQ<UTF32,false> candidateCPQ = sQRead(iChanR))
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
		sUnread(stack.back(), iChanU);
		stack.pop_back();
		}
	return false;
	}


// -----------------

void sCopy_Line(const ChanR_UTF& iSource, const ChanW_UTF& oDest)
	{
	for (;;)
		{
		if (ZQ<UTF32,false> theCPQ = sQRead(iSource))
			break;
		else if (Unicode::sIsEOL(*theCPQ))
			break;
		else
			sWriteMust(*theCPQ, oDest);
		}
	}

void sSkip_Line(const ChanR_UTF& iSource)
	{ sCopy_Line(iSource, ChanW_XX_Discard<UTF32>()); }

string8 sRead_Line(const ChanR_UTF& iSource)
	{
	string8 result;
	sCopy_Line(iSource, ChanW_UTF_string8(&result));
	return result;
	}

// -----------------

void sCopy_WSAndCPlusPlusComments(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest)
	{
	ZAssert(sUnreadableLimit(iChanU) >= 2);

	for (;;)
		{
		if (ZQ<UTF32> firstCPQ = sQRead(iChanR))
			{
			if (spIsWhitespace(*firstCPQ))
				{
				sWriteMust(*firstCPQ, oDest);
				continue;
				}
			else if (*firstCPQ == '/')
				{
				if (sTryRead_CP('/', iChanR, iChanU))
					{
					sWriteMust("//", oDest);
					sCopy_Line(iChanR, oDest);
					continue;
					}
				else if (sTryRead_CP('*', iChanR, iChanU))
					{
					sWriteMust("/*", oDest);
					ZUnimplemented();
	//##				if (not sCopy_Until("*/", iChanR, oDest))
	//##					throw ParseException("Unexpected end of data while parsing a /**/ comment");
					sWriteMust("*/", oDest);
					continue;
					}
				}
			sUnread(*firstCPQ, iChanU);
			break;
			}
		}
	}

void sSkip_WSAndCPlusPlusComments(const ChanR_UTF& iChanR, const ChanU_UTF& iChanU)
	{ sCopy_WSAndCPlusPlusComments(iChanR, iChanU, ChanW_XX_Discard<UTF32>()); }

// -----------------

void sCopy_EscapedString(UTF32 iTerminator, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest)
	{
	sCopyAll(ChanR_UTF_Escaped(iTerminator, iChanR, iChanU), oDest);
	}

void sRead_EscapedString(UTF32 iTerminator, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	sCopy_EscapedString(iTerminator, iChanR, iChanU, ChanW_UTF_string8(&oString));
	}

// -----------------

bool sTryCopy_EscapedString(UTF32 iDelimiter, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	const ChanW_UTF& oDest)
	{
	if (not sTryRead_CP(iDelimiter, iChanR, iChanU))
		return false;

	sCopy_EscapedString(iDelimiter, iChanR, iChanU, oDest);

	if (not sTryRead_CP(iDelimiter, iChanR, iChanU))
		throw ParseException("Missing string delimiter");

	return true;
	}

bool sTryRead_EscapedString(UTF32 iDelimiter, const ChanR_UTF& iChanR, const ChanU_UTF& iChanU,
	string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	return sTryCopy_EscapedString(iDelimiter, iChanR, iChanU, ChanW_UTF_string8(&oString));
	}

// -----------------

// =================================================================================================
// MARK: -

void sWriteExact(float iVal, const ChanW_UTF& iChanW)
	{
	// 9 decimal digits are necessary and sufficient for single precision IEEE 754.
	// "What Every Computer Scientist Should Know About Floating Point", Goldberg, 1991.
	// <http://docs.sun.com/source/806-3568/ncg_goldberg.html>
	sWritefMust(iChanW, "%.9g", iVal);
	}

void sWriteExact(double iVal, const ChanW_UTF& iChanW)
	{
	// 17 decimal digits are necessary and sufficient for double precision IEEE 754.
	sWritefMust(iChanW, "%.17g", iVal);
	}

void sWriteExact(long double iVal, const ChanW_UTF& iChanW)
	{
	// This is a guess for now.
	sWritefMust(iChanW, "%.34Lg", iVal);
	}

} // namespace Util_Chan
} // namespace ZooLib
