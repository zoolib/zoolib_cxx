/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "ZUtil_Stream_BitTorrent.h"
#include "ZMemoryBlock.h"
#include "ZTuple.h"
#include "ZStream.h"
#include "ZStrim_Stream.h"

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

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

static int64 sReadInteger(const ZStreamU& s)
	{
	const bool isNegative = sTryRead_Byte(s, '-');

	int64 result;
	if (!sTryRead_DecimalInteger(s, result))
		throw runtime_error("Problem reading integer");

	if (isNegative)
		return -result;
	else
		return result;
	}

static string sReadString(const ZStreamU& s)
	{
	const int32 stringLength = sReadInteger(s);
	const uint8 terminator = s.ReadUInt8();
	if (terminator != ':')
		throw runtime_error("Expected ':' terminator for string length");

	string theString(stringLength, 0);
	s.Read(const_cast<char*>(theString.data()), stringLength);
	return theString;
	}

static ZTValue sReadStringish(const ZStreamU& s)
	{
	const int32 stringLength = sReadInteger(s);
	const uint8 terminator = s.ReadUInt8();
	if (terminator != ':')
		throw runtime_error("Expected ':' terminator for string/binary length");

	// Suck all the data into a memory block.
	ZMemoryBlock theMB;
	ZStreamRWPos_MemoryBlock(theMB).CopyFrom(s, stringLength);

	// Transcribe the data as UTF8 into theString.
	string theString;
	ZStrimW_String(theString).CopyAllFrom(ZStrimR_StreamUTF8(ZStreamRPos_MemoryBlock(theMB)));

	if (theString.size() == stringLength)
		{
		// The number of bytes in theString matches the number of bytes
		// in theMB, so every byte was valid UTF-8 and we can return it as a string.
		return theString;
		}
	else
		{
		// Some code units were skipped, so return the raw data.
		return theMB;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_Stream_BitTorrent

bool ZUtil_Stream_BitTorrent::sFromStream(const ZStreamU& iStreamU, ZTValue& oTValue)
	{
	try
		{
		oTValue = sTValueFromStream(iStreamU);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZTValue ZUtil_Stream_BitTorrent::sTValueFromStream(const ZStreamU& s)
	{
	ZTValue theTV;
	const uint8 type = s.ReadUInt8();
	switch (type)
		{
		case 'd':
			{
			// Dictionary
			ZTuple& theTuple = theTV.SetMutableTuple();
			for (;;)
				{
				if (sTryRead_Byte(s, 'e'))
					break;
				ZTName name(sReadString(s));
				theTuple.SetValue(name, sTValueFromStream(s));
				}
			break;
			}
		case 'l':
			{
			// List
			vector<ZTValue>& theVector = theTV.SetMutableVector();
			for (;;)
				{
				if (sTryRead_Byte(s, 'e'))
					break;
				theVector.push_back(sTValueFromStream(s));
				}
			break;
			}
		case 'i':
			{
			// Integer
			theTV = sReadInteger(s);
			uint8 terminator = s.ReadUInt8();
			if (terminator != 'e')
				throw runtime_error("Expected 'e' terminator for integer");
			break;
			}
		default:
			{
			s.Unread();
			// It must be a 'string'. It could be valid UTF-8, or could be
			// arbitrary bytes, so we call sReadStringish.
			theTV = sReadStringish(s);
			break;
			}
		}
	return theTV;
	}
