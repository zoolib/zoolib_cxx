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

static const uint8 spBase64EncodeTable[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void spBase64Encode(const uint8* iSource, size_t iSourceCount, uint8* oDest)
	{
	switch (iSourceCount)
		{
		case 3:
			{
			oDest[0] = spBase64EncodeTable[iSource[0] >> 2];
			oDest[1] = spBase64EncodeTable[((iSource[0] & 0x03) << 4) | (iSource[1] >> 4)];
			oDest[2] = spBase64EncodeTable[((iSource[1] & 0x0F) << 2) | (iSource[2] >> 6)];
			oDest[3] = spBase64EncodeTable[iSource[2] & 0x3F];
			break;
			}
		case 2:
			{
			oDest[0] = spBase64EncodeTable[iSource[0] >> 2];
			oDest[1] = spBase64EncodeTable[((iSource[0] & 0x03) << 4) | (iSource[1] >> 4)];
			oDest[2] = spBase64EncodeTable[((iSource[1] & 0x0F) << 2)];
			oDest[3] = '=';
			break;
			}
		case 1:
			{
			oDest[0] = spBase64EncodeTable[iSource[0] >> 2];
			oDest[1] = spBase64EncodeTable[((iSource[0] & 0x03) << 4)];
			oDest[2] = '=';
			oDest[3] = '=';
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

static const uint8 spBase64DecodeTable[] =
	{
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
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Base64Encode

ZStreamR_Base64Encode::ZStreamR_Base64Encode(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	fSinkCount = 4;
	}

ZStreamR_Base64Encode::~ZStreamR_Base64Encode()
	{}

void ZStreamR_Base64Encode::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		while (countRemaining && fSinkCount != 4)
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
			uint8 sourceBuf[3];
			while (sourceCount != 3)
				{
				size_t countRead;
				fStreamSource.Read(sourceBuf + sourceCount, 3 - sourceCount, &countRead);
				if (countRead == 0)
					break;
				sourceCount += countRead;
				}
			if (sourceCount == 0)
				break;
			spBase64Encode(sourceBuf, sourceCount, fSinkBuf);
			fSinkCount = 0;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Base64Decode

ZStreamR_Base64Decode::ZStreamR_Base64Decode(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	fSinkCount = 3;
	}

ZStreamR_Base64Decode::~ZStreamR_Base64Decode()
	{}

void ZStreamR_Base64Decode::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
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

				uint8 c = spBase64DecodeTable[currChar];
				if (c != 0xFF)
					{
					source = (source << 6) | c;
					++sourceCount;
					}
				}

			if (sourceCount == 0)
				break;

			if (sourceCount != 4)
				{
				ZDebugLogf(1, ("ZStreamR_Base64Decode::Imp_Read, base64 stream was truncated"));
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
#pragma mark -
#pragma mark * ZStreamW_Base64Encode

ZStreamW_Base64Encode::ZStreamW_Base64Encode(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	fSourceCount = 0;
	}

ZStreamW_Base64Encode::~ZStreamW_Base64Encode()
	{
	if (fSourceCount > 0)
		{
		uint8 sinkBuf[4];
		spBase64Encode(fSourceBuf, fSourceCount, sinkBuf);
		size_t countWritten;
		fStreamSink.Write(sinkBuf, 4, &countWritten);
		fSourceCount = 0;
		}
	}

void ZStreamW_Base64Encode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
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
			spBase64Encode(fSourceBuf, 3, sinkBuf);
			fSourceCount = 0;
			size_t countWritten;
			fStreamSink.Write(sinkBuf, 4, &countWritten);
			}
		}
	}

void ZStreamW_Base64Encode::Imp_Flush()
	{
	if (fSourceCount > 0)
		{
		uint8 sinkBuf[4];
		spBase64Encode(fSourceBuf, fSourceCount, sinkBuf);
		fSourceCount = 0;
		fStreamSink.Write(sinkBuf, 4);
		}
	fStreamSink.Flush();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Base64Decode

ZStreamW_Base64Decode::ZStreamW_Base64Decode(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	fSource = 0;
	fSourceCount = 0;
	}

ZStreamW_Base64Decode::~ZStreamW_Base64Decode()
	{}

void ZStreamW_Base64Decode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		while (fSourceCount != 4 && countRemaining)
			{
			uint8 c = spBase64DecodeTable[*localSource++];

			--countRemaining;
			if (oCountWritten)
				++*oCountWritten;

			if (c != 0xFF)
				{
				fSource = (fSource << 6) | c;
				++fSourceCount;
				}
			}

		if (fSourceCount == 4)
			{
			uint8 sinkBuf[3];
			sinkBuf[0] = uint8(fSource >> 16);
			sinkBuf[1] = uint8(fSource >> 8);
			sinkBuf[2] = uint8(fSource);
			fStreamSink.Write(sinkBuf, 3, nullptr);
			fSource = 0;
			fSourceCount = 0;
			}
		}
	}

void ZStreamW_Base64Decode::Imp_Flush()
	{
	fStreamSink.Flush();
	}

} // namespace ZooLib
