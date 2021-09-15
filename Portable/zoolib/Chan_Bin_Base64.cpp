// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Chan_Bin_Base64.h"

namespace ZooLib {
namespace Base64 {

// =================================================================================================
#pragma mark - Internal (anonymous)

namespace { // anonymous

void spEncode(
	const Encode& iEncode, const byte* iSource, size_t iSourceCount, byte* oDest)
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

constexpr Encode spEncodeStd =
	{
	{
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 97, 98, 99,100,101,102,
	103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
	119,120,121,122, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 43, 47
	},
	61
	};

// Marker value for non-coding characters.
constexpr uint8 __ =  255;

constexpr Decode spDecodeStd =
	{{
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,

	 __, __, __, __, __, __, __, __, __, __, __, 62, __, __, __, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, __, __, __, __, __, __,
	 __,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, __, __, __, __, __,
	 __, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, __, __, __, __, __,

	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	 __, __, __, __, __, __, __, __, __, __, __, __, __, __, __, __,
	}};

} // anonymous namespace

// =================================================================================================
#pragma mark -

Decode sDecode(uint8 i62, uint8 i63)
	{ return sDecode(i62, i63, '='); }

Decode sDecode(uint8 i62, uint8 i63, uint8 iPadding)
	{
	Decode result = spDecodeStd;
	result.fTable[62] = i62;
	result.fTable[63] = i63;
	result.fTable[iPadding] = 254;
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
	{ return sDecode('+', '/', '='); }

Encode sEncode_Normal()
	{ return sEncode('+', '/', '='); }

} // namespace Base64

// =================================================================================================
#pragma mark - ChanR_Bin_Base64Decode

ChanR_Bin_Base64Decode::ChanR_Bin_Base64Decode(const ChanR_Bin& iChanR)
:	fDecode(Base64::sDecode_Normal())
,	fChanR(iChanR)
,	fIndex(0)
	{}

ChanR_Bin_Base64Decode::ChanR_Bin_Base64Decode(
	const Base64::Decode& iDecode, const ChanR_Bin& iChanR)
:	fDecode(iDecode)
,	fChanR(iChanR)
,	fIndex(0)
	{}

ChanR_Bin_Base64Decode::~ChanR_Bin_Base64Decode()
	{}

size_t ChanR_Bin_Base64Decode::Read(byte* oDest, size_t iCount)
	{
	byte* const localEnd = oDest + iCount;
	while (oDest < localEnd)
		{
		if (NotQ<byte> curByteQ = sQRead(fChanR))
			{
			break;
			}
		else switch (const uint8 dd = fDecode.fTable[*curByteQ])
			{
			case 255: break; // Non-coding character, ignore
			case 254: fIndex = 0; break; // Padding, reset state
			default:
				{
				switch (fIndex & 3)
					{
					case 0: fPending = dd << 2; break;
					case 1: *oDest++ = fPending | (dd >> 4); fPending = dd << 4; break;
					case 2: *oDest++ = fPending | (dd >> 2); fPending = dd << 6; break;
					case 3: *oDest++ = fPending | dd; break;
					}
				++fIndex;
				}
			}
		}
	return iCount - (localEnd - oDest);
	}

// =================================================================================================
#pragma mark - ChanW_Bin_Base64Encode

ChanW_Bin_Base64Encode::ChanW_Bin_Base64Encode(const ChanW_Bin& iChanW)
:	fEncode(Base64::sEncode_Normal())
,	fChanW(iChanW)
,	fSourceCount(0)
	{}

ChanW_Bin_Base64Encode::ChanW_Bin_Base64Encode(
	const Base64::Encode& iEncode, const ChanW_Bin& iChanW)
:	fEncode(iEncode)
,	fChanW(iChanW)
,	fSourceCount(0)
	{}

ChanW_Bin_Base64Encode::~ChanW_Bin_Base64Encode()
	{
	if (fSourceCount > 0)
		{
		byte sinkBuf[4];
		Base64::spEncode(fEncode, fSourceBuf, fSourceCount, sinkBuf);
		sWriteMemFully(fChanW, sinkBuf, 4);
		fSourceCount = 0;
		}
	}

size_t ChanW_Bin_Base64Encode::Write(const byte* iSource, size_t iCount)
	{
	const byte* localSource = iSource;
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		while (countRemaining && fSourceCount != 3)
			{
			fSourceBuf[fSourceCount] = *localSource++;
			++fSourceCount;
			--countRemaining;
			}

		if (fSourceCount == 3)
			{
			byte sinkBuf[4];
			Base64::spEncode(fEncode, fSourceBuf, 3, sinkBuf);
			sWriteMemFully(fChanW, sinkBuf, 4);
			fSourceCount = 0;
			}
		}
	return localSource - iSource;
	}

void ChanW_Bin_Base64Encode::Flush()
	{
	if (fSourceCount > 0)
		{
		byte sinkBuf[4];
		Base64::spEncode(fEncode, fSourceBuf, fSourceCount, sinkBuf);
		sWriteMemFully(fChanW, sinkBuf, 4);
		fSourceCount = 0;
		}
	sFlush(fChanW);
	}

} // namespace ZooLib
