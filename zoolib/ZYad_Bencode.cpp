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
#include "zoolib/ZYad_Bencode.h"
#include "ZUnicode.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static void sThrowParseException(const string& iMessage)
	{
	throw ZYadParseException_Bencode(iMessage);
	}

static bool sTryRead_Byte(const ZStreamU& s, uint8 iByte)
	{
	if (iByte == s.ReadUInt8())
		return true;
	s.Unread();
	return false;
	}

static bool sTryRead_Digit(const ZStreamU& s, int& oDigit)
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
		if (!sTryRead_Digit(s, curDigit))
			return isValid;
		isValid = true;
		oInteger *= 10;
		oInteger += curDigit;
		}
	}

static bool sTryRead_SignedInteger(const ZStreamU& s, int64& oInteger)
	{
	const bool isNegative = sTryRead_Byte(s, '-');

	if (!sTryRead_DecimalInteger(s, oInteger))
		return false;

	if (isNegative)
		oInteger = -oInteger;

	return true;
	}

static int64 sRead_PositiveInteger(const ZStreamU& s)
	{
	int64 result;
	if (!sTryRead_DecimalInteger(s, result))
		sThrowParseException("Expected unsigned length");
	return result;
	}

static string sReadString(const ZStreamU& s)
	{
	const int32 stringLength = sRead_PositiveInteger(s);
	if (!sTryRead_Byte(s, ':'))
		sThrowParseException("Expected ':' terminator for string length");

	string theString(stringLength, 0);
	s.Read(const_cast<char*>(theString.data()), stringLength);
	return theString;
	}

static ZRef<ZYadR_Bencode> sReadStringish(const ZStreamU& s)
	{
	const int32 theLength = sRead_PositiveInteger(s);
	if (!sTryRead_Byte(s, ':'))
		sThrowParseException("Expected ':' terminator for string/binary length");

	if (!theLength)
		return new ZYadPrimR_Bencode(ZTValue());

	const string theString = s.ReadString(theLength);
	string::const_iterator current = theString.begin();
	string::const_iterator end = theString.end();
	size_t countSkipped = 0;
	UTF32 dummyCP;
	while (ZUnicode::sReadInc(current, end, dummyCP, countSkipped) && !countSkipped)
		{}

	if (countSkipped == 0)
		return new ZYadPrimR_Bencode(theString);
	else
		return new ZYadRawR_Bencode(ZMemoryBlock(theString.data(), theLength));
	}

static ZRef<ZYadR_Bencode> sMakeYadR_Bencode(const ZStreamU& s)
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
			if (!sTryRead_SignedInteger(s, theInteger))
				sThrowParseException("Expected signed decimal integer");
			if (!sTryRead_Byte(s, 'e'))
				sThrowParseException("Expected 'e' terminator for integer");
			return new ZYadPrimR_Bencode(theInteger);
			}
		default:
			{
			s.Unread();
			// It must be a 'string'.

			// It could be valid UTF-8, or could be
			// arbitrary bytes, so we call sReadStringish.
			return sReadStringish(s);
			}
		}
	return ZRef<ZYadR_Bencode>();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadParseException_Bencode

ZYadParseException_Bencode::ZYadParseException_Bencode(const string& iWhat)
:	ZYadParseException(iWhat)
	{}

ZYadParseException_Bencode::ZYadParseException_Bencode(const char* iWhat)
:	ZYadParseException(iWhat)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadPrimR_Bencode

ZYadPrimR_Bencode::ZYadPrimR_Bencode(const ZTValue& iTV)
:	ZYadR_TValue(iTV)
	{}

void ZYadPrimR_Bencode::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadRawR_Bencode

ZYadRawR_Bencode::ZYadRawR_Bencode(const ZMemoryBlock& iMB)
:	ZYadRawRPos_MemoryBlock(iMB)
	{}

void ZYadRawR_Bencode::Finish()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadListR_Bencode

ZYadListR_Bencode::ZYadListR_Bencode(const ZStreamU& iStreamU)
:	fStreamU(iStreamU),
	fPosition(0),
	fFinished(false)
	{}

void ZYadListR_Bencode::Finish()
	{
	this->SkipAll();
	}

bool ZYadListR_Bencode::HasChild()
	{
	this->pMoveIfNecessary();
	return fValue_Current;
	}

ZRef<ZYadR> ZYadListR_Bencode::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		++fPosition;
		}

	return fValue_Prior;
	}

size_t ZYadListR_Bencode::GetPosition()
	{ return fPosition; }

void ZYadListR_Bencode::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fFinished)
		return;

	if (sTryRead_Byte(fStreamU, 'e'))
		fFinished = true;
	else
		fValue_Current = sMakeYadR_Bencode(fStreamU);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYadMapR_Bencode

ZYadMapR_Bencode::ZYadMapR_Bencode(const ZStreamU& iStreamU)
:	fStreamU(iStreamU),
	fFinished(false)
	{}
	
void ZYadMapR_Bencode::Finish()
	{
	this->SkipAll();
	}

bool ZYadMapR_Bencode::HasChild()
	{
	this->pMoveIfNecessary();

	return fValue_Current;
	}

ZRef<ZYadR> ZYadMapR_Bencode::NextChild()
	{
	this->pMoveIfNecessary();

	if (fValue_Current)
		{
		fValue_Prior = fValue_Current;
		fValue_Current.Clear();
		fName.clear();
		}

	return fValue_Prior;
	}

string ZYadMapR_Bencode::Name()
	{
	this->pMoveIfNecessary();

	return fName;
	}

void ZYadMapR_Bencode::pMoveIfNecessary()
	{
	if (fValue_Current)
		return;

	if (fValue_Prior)
		{
		fValue_Prior->Finish();
		fValue_Prior.Clear();
		}

	if (fFinished)
		return;

	if (sTryRead_Byte(fStreamU, 'e'))
		{
		fFinished = true;
		}
	else
		{
		fName = sReadString(fStreamU);
		fValue_Current = sMakeYadR_Bencode(fStreamU);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZYad_Bencode

ZRef<ZYadR> ZYad_Bencode::sMakeYadR(const ZStreamU& iStreamU)
	{ return sMakeYadR_Bencode(iStreamU); }

NAMESPACE_ZOOLIB_END
