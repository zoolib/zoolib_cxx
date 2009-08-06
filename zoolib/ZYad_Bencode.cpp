/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZCompat_cmath.h"
#include "zoolib/ZYad_Any.h"
#include "zoolib/ZYad_Bencode.h"
#include "zoolib/ZUnicode.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void spThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_Bencode(iMessage);
	}

static bool spTryRead_Byte(const ZStreamU& s, uint8 iByte)
	{
	if (iByte == s.ReadUInt8())
		return true;
	s.Unread();
	return false;
	}

static bool spTryRead_Digit(const ZStreamU& s, int& oDigit)
	{
	uint8 theByte;
	if (!s.ReadByte(theByte))
		return false;

	if (theByte >= '0' && theByte <= '9')
		{
		oDigit = theByte - '0';
		return true;
		}

	s.Unread();
	return false;
	}

static bool sTryRead_DecimalInteger(const ZStreamU& s, int64& oInteger)
	{
	oInteger = 0;
	bool isValid = false;
	for (;;)
		{
		int curDigit;
		if (!spTryRead_Digit(s, curDigit))
			return isValid;
		isValid = true;
		oInteger *= 10;
		oInteger += curDigit;
		}
	}

static bool spTryRead_SignedInteger(const ZStreamU& s, int64& oInteger)
	{
	const bool isNegative = spTryRead_Byte(s, '-');

	if (!sTryRead_DecimalInteger(s, oInteger))
		return false;

	if (isNegative)
		oInteger = -oInteger;

	return true;
	}

static int64 spRead_PositiveInteger(const ZStreamU& s)
	{
	int64 result;
	if (!sTryRead_DecimalInteger(s, result))
		spThrowParseException("Expected unsigned length");
	return result;
	}

static string spReadString(const ZStreamU& s)
	{
	const int32 stringLength = spRead_PositiveInteger(s);
	if (!spTryRead_Byte(s, ':'))
		spThrowParseException("Expected ':' terminator for string length");

	return s.ReadString(stringLength);
	}

static ZRef<ZYadR> spReadStringish(const ZStreamU& s)
	{
	const int32 theLength = spRead_PositiveInteger(s);
	if (!spTryRead_Byte(s, ':'))
		spThrowParseException("Expected ':' terminator for string/binary length");

	if (!theLength)
		return new ZYadPrimR_Std(ZAny());

	const string theString = s.ReadString(theLength);
	string::const_iterator current = theString.begin();
	string::const_iterator end = theString.end();
	size_t countSkipped = 0;
	UTF32 dummyCP;
	while (ZUnicode::sReadInc(current, end, dummyCP, countSkipped) && !countSkipped)
		{}

	if (countSkipped == 0)
		{
		// We skipped no code units, so theString is valid UTF8.
		return sMakeYadR(theString);
		}
	else
		{
		return sMakeYadR(ZData_Any(theString.data(), theLength));
		}
	}

static ZRef<ZYadR> spMakeYadR_Bencode(const ZStreamU& s)
	{
	const uint8 type = s.ReadUInt8();
	switch (type)
		{
		case 'd':
			{
			return new ZYadMapR_Bencode(s);
			}
		case 'l':
			{
			return new ZYadListR_Bencode(s);
			}
		case 'i':
			{
			int64 theInteger;
			if (!spTryRead_SignedInteger(s, theInteger))
				spThrowParseException("Expected signed decimal integer");
			if (!spTryRead_Byte(s, 'e'))
				spThrowParseException("Expected 'e' terminator for integer");
			return new ZYadPrimR_Std(ZAny(theInteger));
			}
		default:
			{
			s.Unread();
			// It must be a 'string'.

			// It could be valid UTF-8, or could be
			// arbitrary bytes, so we call sReadStringish.
			return spReadStringish(s);
			}
		}
	return ZRef<ZYadR>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Bencode

ZYadParseException_Bencode::ZYadParseException_Bencode(const string& iWhat)
:	ZYadParseException_Std(iWhat)
	{}

ZYadParseException_Bencode::ZYadParseException_Bencode(const char* iWhat)
:	ZYadParseException_Std(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_Bencode

ZYadListR_Bencode::ZYadListR_Bencode(const ZStreamU& iStreamU)
:	fStreamU(iStreamU)
	{}

void ZYadListR_Bencode::Imp_ReadInc(bool iIsFirst, ZRef<ZYadR>& oYadR)
	{
	if (!spTryRead_Byte(fStreamU, 'e'))
		oYadR = spMakeYadR_Bencode(fStreamU);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Bencode

ZYadMapR_Bencode::ZYadMapR_Bencode(const ZStreamU& iStreamU)
:	fStreamU(iStreamU)
	{}

void ZYadMapR_Bencode::Imp_ReadInc(bool iIsFirst, std::string& oName, ZRef<ZYadR>& oYadR)
	{
	if (!spTryRead_Byte(fStreamU, 'e'))
		{
		oName = spReadString(fStreamU);
		oYadR = spMakeYadR_Bencode(fStreamU);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_Bencode

ZRef<ZYadR> ZYad_Bencode::sMakeYadR(const ZStreamU& iStreamU)
	{ return spMakeYadR_Bencode(iStreamU); }

NAMESPACE_ZOOLIB_END
