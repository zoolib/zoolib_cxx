/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_Base64.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace Base64 {

// =================================================================================================
// MARK: - Internal (anonymous)

namespace { // anonymous

static void spEncode
	(const Encode& iEncode, const uint8* iSource, size_t iSourceCount, uint8* oDest)
	{
	switch (iSourceCount)
		{
		case 3:
			{
			oDest[0] = iEncode.fTable[iSource[0] >> 2];
			oDest[1] = iEncode.fTable[((iSource[0] & 0x03) << 4) | (iSource[1] >> 4)];
			oDest[2] = iEncode.fTable[((iSource[1] & 0x0F) << 2) | (iSource[2] >> 6)];
			oDest[3] = iEncode.fTable[iSource[2] & 0x3F];
			break;
			}
		case 2:
			{
			oDest[0] = iEncode.fTable[iSource[0] >> 2];
			oDest[1] = iEncode.fTable[((iSource[0] & 0x03) << 4) | (iSource[1] >> 4)];
			oDest[2] = iEncode.fTable[((iSource[1] & 0x0F) << 2)];
			oDest[3] = iEncode.fPadding;
			break;
			}
		case 1:
			{
			oDest[0] = iEncode.fTable[iSource[0] >> 2];
			oDest[1] = iEncode.fTable[((iSource[0] & 0x03) << 4)];
			oDest[2] = iEncode.fPadding;
			oDest[3] = iEncode.fPadding;
			break;
			}
		default:
			{
			// spBase64Encode must be called with iSourceCount == 1, 2 or 3
			ZDebugStop(0);
			break;
			}
		}
	}

static const uint8 spBase64EncodeTable[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const Encode spEncodeStd =
	{
	{
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,
	103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
	119,120,121,122, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 43, 47
	},
	61
	};

static const Decode spDecodeStd =
	{{
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255, 62,255,255,255, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,255,255,255,  0,255,255,
	255,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,255,255,255,255,255,
	255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
	}};

} // anonymous namespace

// =================================================================================================
// MARK: - Internal (anonymous)


Decode sDecode(uint8 i62, uint8 i63)
	{
	Decode result = spDecodeStd;
	result.fTable[62] = i62;
	result.fTable[63] = i63;
	return result;
	}

Encode sEncode(uint8 i62, uint8 i63, uint8 iPadding)
	{
	Encode result = spEncodeStd;
	result.fTable[62] = i62;
	result.fTable[63] = i63;
	result.fPadding = iPadding;
	return result;
	}

Decode sDecode_Normal()
	{ return sDecode('+', '/'); }

Encode sEncode_Normal()
	{ return sEncode('+', '/', '='); }

// =================================================================================================
// MARK: - StreamR_Decode

StreamR_Decode::StreamR_Decode(const ZStreamR& iStreamSource)
:	fDecode(sDecode_Normal())
,	fStreamSource(iStreamSource)
,	fSinkCount(3)
	{}

StreamR_Decode::StreamR_Decode(const Decode& iDecode, const ZStreamR& iStreamSource)
:	fDecode(iDecode)
,	fStreamSource(iStreamSource)
,	fSinkCount(3)
	{}

StreamR_Decode::~StreamR_Decode()
	{}

void StreamR_Decode::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		// Transcribe our sink buffer
		while (countRemaining && fSinkCount != 3)
			{
			*localDest++ = fSinkBuf[fSinkCount];
			--countRemaining;
			++fSinkCount;
			if (oCountRead)
				++*oCountRead;
			}

		if (countRemaining)
			{
			size_t sourceCount = 0;
			uint32 source = 0;
			while (sourceCount < 4)
				{
				uint8 currChar;
				if (!fStreamSource.ReadByte(currChar))
					break;

				const uint8 c = fDecode.fTable[currChar];
				if (c == 0xFF)
					{
					// Ignore
					}
				else if (c == 0xFE)
					{
					// Exit.
					break;
					}
				else
					{
					source = (source << 6) | c;
					++sourceCount;
					}
				}

			if (sourceCount == 0)
				break;

			if (sourceCount != 4)
				{
				ZDebugLogf(1, ("StreamR_Decode::Imp_Read, base64 stream was truncated"));
				break;
				}

			fSinkBuf[0] = uint8(source >> 16);
			fSinkBuf[1] = uint8(source >> 8);
			fSinkBuf[2] = uint8(source);
			fSinkCount = 0;
			}
		}
	}

// =================================================================================================
// MARK: - StreamW_Encode

StreamW_Encode::StreamW_Encode(const ZStreamW& iStreamSink)
:	fEncode(sEncode_Normal())
,	fStreamSink(iStreamSink)
,	fSourceCount(0)
	{}

StreamW_Encode::StreamW_Encode(const Encode& iEncode, const ZStreamW& iStreamSink)
:	fEncode(iEncode)
,	fStreamSink(iStreamSink)
,	fSourceCount(0)
	{}

StreamW_Encode::~StreamW_Encode()
	{
	if (fSourceCount > 0)
		{
		uint8 sinkBuf[4];
		spEncode(fEncode, fSourceBuf, fSourceCount, sinkBuf);
		size_t countWritten;
		fStreamSink.Write(sinkBuf, 4, &countWritten);
		fSourceCount = 0;
		}
	}

void StreamW_Encode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		while (countRemaining && fSourceCount != 3)
			{
			fSourceBuf[fSourceCount] = *localSource++;
			++fSourceCount;
			--countRemaining;
			if (oCountWritten)
				++*oCountWritten;
			}

		if (fSourceCount == 3)
			{
			uint8 sinkBuf[4];
			spEncode(fEncode, fSourceBuf, 3, sinkBuf);
			fSourceCount = 0;
			size_t countWritten;
			fStreamSink.Write(sinkBuf, 4, &countWritten);
			}
		}
	}

void StreamW_Encode::Imp_Flush()
	{
	if (fSourceCount > 0)
		{
		uint8 sinkBuf[4];
		spEncode(fEncode, fSourceBuf, fSourceCount, sinkBuf);
		fSourceCount = 0;
		fStreamSink.Write(sinkBuf, 4);
		}
	fStreamSink.Flush();
	}

} // namespace Base64
} // namespace ZooLib
