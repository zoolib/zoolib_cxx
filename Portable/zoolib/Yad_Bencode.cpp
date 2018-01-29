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

#include "zoolib/Unicode.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanU_Bin.h"
#include "zoolib/Compat_cmath.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Yad_Any.h"
#include "zoolib/Yad_Bencode.h"

namespace ZooLib {
namespace Yad_Bencode {

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark Helpers

static void spThrowParseException(const string& iMessage)
	{ throw YadParseException(iMessage); }

static bool spTryRead_Byte(const ChanRU_Bin& iChanRU, uint8 iByte)
	{ return sTryRead<uint8>(iByte, iChanRU, iChanRU); }

static bool spTryRead_Digit(const ChanRU_Bin& s, int& oDigit)
	{
	uint8 theByte;
	if (1 != sRead(s, &theByte, 1))
		return false;

	if (theByte >= '0' && theByte <= '9')
		{
		oDigit = theByte - '0';
		return true;
		}

	sUnread(s, theByte);
	return false;
	}

static bool spTryRead_DecimalInteger(const ChanRU_Bin& s, int64& oInteger)
	{
	oInteger = 0;
	bool isValid = false;
	for (;;)
		{
		int curDigit;
		if (not spTryRead_Digit(s, curDigit))
			return isValid;
		isValid = true;
		oInteger *= 10;
		oInteger += curDigit;
		}
	}

static bool spTryRead_SignedInteger(const ChanRU_Bin& s, int64& oInteger)
	{
	const bool isNegative = spTryRead_Byte(s, '-');

	if (not spTryRead_DecimalInteger(s, oInteger))
		return false;

	if (isNegative)
		oInteger = -oInteger;

	return true;
	}

static int64 spRead_PositiveInteger(const ChanRU_Bin& s)
	{
	int64 result;
	if (not spTryRead_DecimalInteger(s, result))
		spThrowParseException("Expected unsigned length");
	return result;
	}

static string spReadString(const ChanRU_Bin& s)
	{
	const int32 stringLength = spRead_PositiveInteger(s);
	if (not spTryRead_Byte(s, ':'))
		spThrowParseException("Expected ':' terminator for string length");

	return sReadString(s, stringLength);
	}

static ZRef<YadR> spReadStringish(const ChanRU_Bin& s)
	{
	const int32 theLength = spRead_PositiveInteger(s);
	if (not spTryRead_Byte(s, ':'))
		spThrowParseException("Expected ':' terminator for string/binary length");

	if (not theLength)
		return sYadAtomR_Any(Any());

	const string8 theString = sReadString(s, theLength);
	string::const_iterator current = theString.begin();
	string::const_iterator end = theString.end();
	size_t countSkipped = 0;
	UTF32 dummyCP;
	while (Unicode::sReadInc(current, end, dummyCP, countSkipped) && !countSkipped)
		{}

	if (countSkipped == 0)
		{
		// We skipped no code units, so theString is valid UTF8.
		return ZooLib::sYadR(theString);
		}
	else
		{
		return sYadR(Data_Any(theString.data(), theLength));
		}
	}

static ZRef<YadR> spMakeYadR_Bencode(ZRef<ChannerRU_Bin> iChanner)
	{
	const ChanRU_Bin& theChan = *iChanner;

	const uint8 type = sEReadBE<uint8>(theChan);
	switch (type)
		{
		case 'd':
			{
			return sChanner_T<ChanR_NameRefYad>(iChanner);
			}
		case 'l':
			{
			return sChanner_T<ChanR_RefYad>(iChanner);
			}
		case 'i':
			{
			int64 theInteger;
			if (not spTryRead_SignedInteger(theChan, theInteger))
				spThrowParseException("Expected signed decimal integer");
			if (not spTryRead_Byte(theChan, 'e'))
				spThrowParseException("Expected 'e' terminator for integer");
			return sYadAtomR_Any(Any(theInteger));
			}
		default:
			{
			sUnread(theChan, type);
			// It must be a 'string'.

			// It could be valid UTF-8, or could be
			// arbitrary bytes, so we call sReadStringish.
			return spReadStringish(theChan);
			}
		}

	return null;
	}

// =================================================================================================
#pragma mark -
#pragma mark YadParseException

YadParseException::YadParseException(const string& iWhat)
:	ZooLib::YadParseException(iWhat)
	{}

YadParseException::YadParseException(const char* iWhat)
:	ZooLib::YadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_RefYad

ChanR_RefYad::ChanR_RefYad(ZRef<ChannerRU_Bin> iChanner)
:	fChanner(iChanner)
	{}

void ChanR_RefYad::Imp_ReadInc(bool iIsFirst, ZRef<YadR>& oYadR)
	{
	if (not spTryRead_Byte(*fChanner, 'e'))
		oYadR = spMakeYadR_Bencode(fChanner);
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_NameRefYad

ChanR_NameRefYad::ChanR_NameRefYad(ZRef<ChannerRU_Bin> iChanner)
:	fChanner(iChanner)
	{}

void ChanR_NameRefYad::Imp_ReadInc(bool iIsFirst, Name& oName, ZRef<YadR>& oYadR)
	{
	const ChanRU_Bin& theChan = *fChanner;
	if (not spTryRead_Byte(theChan, 'e'))
		{
		oName = spReadString(theChan);
		oYadR = spMakeYadR_Bencode(fChanner);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark ZYad_Bencode

ZRef<YadR> sYadR(ZRef<ChannerRU_Bin> iChanner)
	{ return spMakeYadR_Bencode(iChanner); }

} // namespace Yad_Bencode
} // namespace ZooLib
