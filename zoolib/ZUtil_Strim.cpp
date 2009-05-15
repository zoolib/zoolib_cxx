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

#include "zoolib/ZUtil_Strim.h"

#include "zoolib/ZCompat_cmath.h" // For pow, NAN and INFINITY
#include "zoolib/ZDebug.h"
#include "zoolib/ZStrim_Escaped.h"
#include "zoolib/ZStrimR_Boundary.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Strim

void ZUtil_Strim::sCopy_WS(const ZStrimU& iStrimU, const ZStrimW& iDest)
	{
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimU.ReadCP(theCP))
			break;
		if (!ZUnicode::sIsWhitespace(theCP))
			{
			iStrimU.Unread();
			break;
			}
		iDest.WriteCP(theCP);
		}
	}

void ZUtil_Strim::sSkip_WS(const ZStrimU& iStrimU)
	{
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimU.ReadCP(theCP))
			break;
		if (!ZUnicode::sIsWhitespace(theCP))
			{
			iStrimU.Unread();
			break;
			}
		}
	}

// ----------

void ZUtil_Strim::sCopy_WSAndCPlusPlusComments(const ZStrimU& iStrimU, const ZStrimW& iDest)
	{
	// There is a weakness in this code -- it will cause any standalone '/' to be absorbed.
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimU.ReadCP(theCP))
			break;

		if (ZUnicode::sIsWhitespace(theCP))
			{
			iDest.WriteCP(theCP);
			}
		else
			{
			if (theCP != '/')
				{
				iStrimU.Unread();
				break;
				}

			if (!iStrimU.ReadCP(theCP))
				break;

			if (theCP == '/')
				{
				iDest.Write("//");
				sCopy_Line(iStrimU, iDest);
				}
			else if (theCP == '*')
				{
				iDest.Write("/*");
				if (!sCopy_Until(iStrimU, "*/", iDest))
					throw ParseException("Unexpected end of data whilst parsing a /**/ comment");
				iDest.Write("*/");
				}
			else
				{
				break;
				}
			}
		}
	}

void ZUtil_Strim::sSkip_WSAndCPlusPlusComments(const ZStrimU& iStrimU)
	{ sCopy_WSAndCPlusPlusComments(iStrimU, ZStrimW_Null()); }

// ----------

void ZUtil_Strim::sCopy_Line(const ZStrimR& iStrimR, const ZStrimW& iDest)
	{
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimR.ReadCP(theCP))
			break;
		if (ZUnicode::sIsEOL(theCP))
			break;
		iDest.WriteCP(theCP);
		}	
	}

void ZUtil_Strim::sSkip_Line(const ZStrimR& iStrimR)
	{ sCopy_Line(iStrimR, ZStrimW_Null()); }

string8 ZUtil_Strim::sRead_Line(const ZStrimR& iStrimR)
	{
	string8 result;
	sCopy_Line(iStrimR, ZStrimW_String(result));
	return result;
	}

// ----------

bool ZUtil_Strim::sCopy_Until(const ZStrimR& iStrimR, UTF32 iTerminator, const ZStrimW& iDest)
	{
	for (;;)
		{
		UTF32 theCP;
		if (!iStrimR.ReadCP(theCP))
			return false;
		if (theCP == iTerminator)
			return true;
		iDest.WriteCP(theCP);
		}		
	}

bool ZUtil_Strim::sSkip_Until(const ZStrimR& iStrimR, UTF32 iTerminator)
	{ return sCopy_Until(iStrimR, iTerminator, ZStrimW_Null()); }

string8 ZUtil_Strim::sRead_Until(const ZStrimR& iStrimR, UTF32 iTerminator)
	{
	string8 result;
	sCopy_Until(iStrimR, iTerminator, ZStrimW_String(result));
	return result;
	}

// ----------

bool ZUtil_Strim::sCopy_Until(const ZStrimR& iStrimR,
	const string8& iTerminator, const ZStrimW& iDest)
	{
	ZStrimR_Boundary theStrimBoundary(iTerminator, iStrimR);
	theStrimBoundary.CopyAllTo(iDest);
	return theStrimBoundary.HitBoundary();
	}

bool ZUtil_Strim::sSkip_Until(const ZStrimR& iStrimR, const string8& iTerminator)
	{ return sCopy_Until(iStrimR, iTerminator, ZStrimW_Null()); }

string8 ZUtil_Strim::sRead_Until(const ZStrimR& iStrimR, const string8& iTerminator)
	{
	string8 result;
	sCopy_Until(iStrimR, iTerminator, ZStrimW_String(result));
	return result;
	}

// ----------

bool ZUtil_Strim::sTryRead_CP(const ZStrimU& iStrimU, UTF32 iCP)
	{
	// Ensure that we only try to read a valid CP, one that
	// can actually be returned by ReadCP.
	ZAssertStop(2, ZUnicode::sIsValid(iCP));

	UTF32 theCP;
	if (!iStrimU.ReadCP(theCP))
		return false;

	if (theCP == iCP)
		return true;
		
	iStrimU.Unread();
	return false;
	}

bool ZUtil_Strim::sTryRead_Digit(const ZStrimU& iStrimU, int& oDigit)
	{
	UTF32 curCP;
	if (!iStrimU.ReadCP(curCP))
		return false;

	if (curCP >= '0' && curCP <= '9')
		{
		oDigit = curCP - '0';
		return true;
		}

	iStrimU.Unread();
	return false;
	}

bool ZUtil_Strim::sTryRead_HexDigit(const ZStrimU& iStrimU, int& oDigit)
	{
	UTF32 curCP;
	if (!iStrimU.ReadCP(curCP))
		return false;

	if (curCP >= '0' && curCP <= '9')
		{
		oDigit = curCP - '0';
		return true;
		}

	if (curCP >= 'a' && curCP <= 'f')
		{
		oDigit = curCP - 'a' + 10;
		return true;
		}

	if (curCP >= 'A' && curCP <= 'F')
		{
		oDigit = curCP - 'A' + 10;
		return true;
		}
		
	iStrimU.Unread();
	return false;
	}

bool ZUtil_Strim::sTryRead_DecimalInteger(const ZStrimU& iStrimU, int64& oInteger)
	{
	oInteger = 0;
	bool isValid = false;
	for (;;)
		{
		int curDigit;
		if (!sTryRead_Digit(iStrimU, curDigit))
			return isValid;
		isValid = true;
		oInteger *= 10;
		oInteger += curDigit;
		}
	}

bool ZUtil_Strim::sTryRead_HexInteger(const ZStrimU& iStrimU, int64& oInteger)
	{
	oInteger = 0;
	bool isValid = false;
	for (;;)
		{
		int curDigit;
		if (!sTryRead_HexDigit(iStrimU, curDigit))
			return isValid;
		isValid = true;
		oInteger *= 16;
		oInteger += curDigit;
		}
	}

bool ZUtil_Strim::sTryRead_GenericInteger(const ZStrimU& iStrimU, int64& oInteger)
	{
	bool hadPrefix = true;
	bool isNegative = false;

	if (sTryRead_CP(iStrimU, '-'))
		isNegative = true;
	else if (!sTryRead_CP(iStrimU, '+'))
		hadPrefix = false;

	if (sTryRead_CP(iStrimU, '0'))
		{
		UTF32 theCP;
		if (!iStrimU.ReadCP(theCP))
			{
			oInteger = 0;
			return true;
			}

		if (theCP == 'X' || theCP == 'x')
			{
			if (sTryRead_HexInteger(iStrimU, oInteger))
				{
				if (isNegative)
					oInteger = -oInteger;
				return true;
				}
			throw ParseException("Expected a valid hex integer after '0x' prefix");
			}
		
		iStrimU.Unread();
		if (!ZUnicode::sIsDigit(theCP))
			{
			oInteger = 0;
			return true;
			}

		bool readDecimal = sTryRead_DecimalInteger(iStrimU, oInteger);
		// We know that the first CP to be read is a digit, so sTryRead_DecimalInteger can't fail.
		ZAssertStop(2, readDecimal);
		if (isNegative)
			oInteger = -oInteger;
		return true;
		}

	if (sTryRead_DecimalInteger(iStrimU, oInteger))
		{
		if (isNegative)
			oInteger = -oInteger;
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

bool ZUtil_Strim::sTryRead_Double(const ZStrimU& iStrimU, double& oDouble)
	{
	UTF32 curCP;
	if (!iStrimU.ReadCP(curCP))
		return false;

	if (curCP == 'n' || curCP == 'N')
		{
		if (sTryRead_CP(iStrimU, 'a') || sTryRead_CP(iStrimU, 'A'))
			{
			if (sTryRead_CP(iStrimU, 'n') || sTryRead_CP(iStrimU, 'N'))
				{
				// It's a nan
				oDouble = NAN;
				return true;
				}
			}
		throw ParseException("Illegal character when trying to read a double");
		}
	else if (curCP == 'i' || curCP == 'I')
		{
		if (sTryRead_CP(iStrimU, 'n') || sTryRead_CP(iStrimU, 'F'))
			{
			if (sTryRead_CP(iStrimU, 'f') || sTryRead_CP(iStrimU, 'F'))
				{
				// It's an inf
				oDouble = INFINITY;
				return true;
				}
			}
		throw ParseException("Illegal character when trying to read a double");
		}
	else if (!ZUnicode::sIsDigit(curCP) && curCP != '.')
		{
		iStrimU.Unread();
		return false;
		}

	oDouble = 0.0;
	while (ZUnicode::sIsDigit(curCP))
		{
		oDouble *= 10;
		oDouble += (curCP - '0');
		if (!iStrimU.ReadCP(curCP))
			return true;
		}

	if (curCP != '.')
		{
		iStrimU.Unread();
		return true;
		}

	if (!iStrimU.ReadCP(curCP))
		return true;

	if (ZUnicode::sIsDigit(curCP))
		{
		double fracPart = 0.0;
		double divisor = 1;
		while (ZUnicode::sIsDigit(curCP))
			{
			divisor *= 10;
			fracPart *= 10;
			fracPart += (curCP - '0');
			if (!iStrimU.ReadCP(curCP))
				{
				oDouble += fracPart / divisor;
				return true;
				}
			}
		oDouble += fracPart / divisor;
		}

	if (curCP != 'e' && curCP != 'E')
		{
		iStrimU.Unread();
		return true;
		}

	bool isNegativeExponent = false;

	if (sTryRead_CP(iStrimU, '-'))
		isNegativeExponent = true;
	else if (sTryRead_CP(iStrimU, '+'))
		isNegativeExponent = false;

	int64 theExponent;
	if (sTryRead_DecimalInteger(iStrimU, theExponent))
		{
		if (isNegativeExponent)
			oDouble = oDouble * pow(10.0, int(-theExponent));
		else
			oDouble = oDouble * pow(10.0, int(theExponent));
		}

	return true;
	}

bool ZUtil_Strim::sTryRead_SignedDecimalInteger(const ZStrimU& iStrimU, int64& oInteger)
	{
	if (sTryRead_CP(iStrimU, '-'))
		{
		if (!sTryRead_DecimalInteger(iStrimU, oInteger))
			throw ParseException("Expected a valid integer after '-' prefix");
		oInteger = -oInteger;
		return true;
		}
	else if (sTryRead_CP(iStrimU, '+'))
		{
		if (!sTryRead_DecimalInteger(iStrimU, oInteger))
			throw ParseException("Expected a valid integer after '+' prefix");
		return true;
		}
	return sTryRead_DecimalInteger(iStrimU, oInteger);
	}

bool ZUtil_Strim::sTryRead_SignedDouble(const ZStrimU& iStrimU, double& oDouble)
	{
	if (sTryRead_CP(iStrimU, '-'))
		{
		if (!sTryRead_Double(iStrimU, oDouble))
			throw ParseException("Expected a valid double after '-' prefix");
		oDouble = -oDouble;
		return true;
		}
	else if (sTryRead_CP(iStrimU, '+'))
		{
		if (!sTryRead_Double(iStrimU, oDouble))
			throw ParseException("Expected a valid double after '+' prefix");
		return true;
		}
	return sTryRead_Double(iStrimU, oDouble);
	}

void ZUtil_Strim::sCopy_EscapedString(
	const ZStrimU& iStrimU, UTF32 iTerminator, const ZStrimW& iDest)
	{
	ZStrimR_Escaped(iStrimU, iTerminator).CopyAllTo(iDest);
	}

void ZUtil_Strim::sRead_EscapedString(const ZStrimU& iStrimU, UTF32 iTerminator, string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	sCopy_EscapedString(iStrimU, iTerminator, ZStrimW_String(oString));
	}

bool ZUtil_Strim::sTryCopy_EscapedString(const ZStrimU& iStrimU,
	UTF32 iDelimiter, const ZStrimW& iDest)
	{
	if (!sTryRead_CP(iStrimU, iDelimiter))
		return false;

	sCopy_EscapedString(iStrimU, iDelimiter, iDest);

	if (!sTryRead_CP(iStrimU, iDelimiter))
		throw ParseException("Missing string delimiter");

	return true;
	}

bool ZUtil_Strim::sTryRead_EscapedString(const ZStrimU& iStrimU, UTF32 iDelimiter, string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	return sTryCopy_EscapedString(iStrimU, iDelimiter, ZStrimW_String(oString));
	}

bool ZUtil_Strim::sTryCopy_Identifier(const ZStrimU& iStrimU, const ZStrimW& iDest)
	{
	UTF32 curCP;
	if (!iStrimU.ReadCP(curCP))
		return false;

	if (!ZUnicode::sIsAlpha(curCP) && curCP != '_')
		{
		iStrimU.Unread();
		return false;
		}

	iDest.WriteCP(curCP);

	for (;;)
		{
		UTF32 curCP;
		if (!iStrimU.ReadCP(curCP))
			break;

		if (!ZUnicode::sIsAlphaDigit(curCP) && curCP != '_')
			{
			iStrimU.Unread();
			break;
			}
		iDest.WriteCP(curCP);
		}

	return true;
	}

bool ZUtil_Strim::sTryRead_Identifier(const ZStrimU& iStrimU, string8& oString)
	{
	// Resize, rather than clear, so we don't discard any space reserved by our caller.
	oString.resize(0);
	return sTryCopy_Identifier(iStrimU, ZStrimW_String(oString));
	}

NAMESPACE_ZOOLIB_END
