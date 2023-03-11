// Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Chan_UTF_Chan_Bin.h"

#include "zoolib/Unicode.h"

namespace ZooLib {

using namespace Unicode;

// =================================================================================================
#pragma mark - spReadUTF32_UTF16

static size_t spReadUTF32_UTF16(UTF32* oDest, const size_t iCount,
	const ChanR_Bin& iChanR_Bin)
	{
	UTF32* localDest = oDest;

	uint16 theCU;
	bool gotCU = false;
	for (UTF32* const localDestEnd = oDest + iCount; localDest < localDestEnd; /*no inc*/)
		{
		if (not gotCU)
			{
			ZQ<uint16> theQ = sQReadLE<uint16>(iChanR_Bin);
			if (not theQ)
				break;
			theCU = *theQ;
			gotCU = true;
			}

		if (sIsSmallNormal(theCU))
			{
			*oDest++ = theCU;
			}
		else if (sIsSmallNormalOrHighSurrogate(theCU))
			{
			// Must be a high surrogate as it's not a small normal.
			ZQ<uint16> theQ2 = sQReadLE<uint16>(iChanR_Bin);
			if (not theQ2)
				break;
			const uint16 theCU2 = *theQ2;
			if (sIsLowSurrogate(theCU2))
				{
				*localDest++ = sUTF32FromSurrogates(theCU, theCU2);
				gotCU = false;
				}
			else
				{
				// Not a low surrogate, discard theCU and go round again with this is the first CU.
				theCU = theCU2;
				}
			}
		else if (sIsBigNormalOrBeyond(theCU))
			{
			*localDest++ = theCU;
			}
		else
			{
			// Must be an out of order low surrogate.
			}
		}
	return localDest - oDest;
	}

// =================================================================================================
#pragma mark - ChanR_UTF_Chan_Bin_UTF16

ChanR_UTF_Chan_Bin_UTF16::ChanR_UTF_Chan_Bin_UTF16(const ChanR_Bin& iChanR_Bin)
:	fChanR_Bin(iChanR_Bin)
	{}

size_t ChanR_UTF_Chan_Bin_UTF16::Read(UTF32* oDest, size_t iCount)
	{ return spReadUTF32_UTF16(oDest, iCount, fChanR_Bin); }

// =================================================================================================
#pragma mark - spReadUTF32_UTF8

static size_t spReadUTF32_UTF8(UTF32* oDest, const size_t iCount,
	const ChanR_Bin& iChanR_Bin)
	{
	UTF32* localDest = oDest;

	byte curByte = 0;
	bool gotByte = false;
	for (UTF32* const localDestEnd = oDest + iCount; localDest < localDestEnd; /*no inc*/)
		{
		if (not gotByte)
			{
			if (NotQ<byte> theQ = sQRead(iChanR_Bin))
				break;
			else
				curByte = *theQ;
			}

		gotByte = false;

		size_t sequenceLength = sUTF8SequenceLength[curByte];
		if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else if (sequenceLength == 1)
			{
			*localDest++ = curByte;
			}
		else
			{
			UTF32 theCP = curByte & sUTF8StartByteMask[sequenceLength];
			gotByte = false;
			while (--sequenceLength)
				{
				if (NotQ<byte> theQ = sQRead(iChanR_Bin))
					break;
				else
					curByte = *theQ;
				if ((curByte & 0xC0) != 0x80)
					{
					// It's not a legal continuation byte.
					gotByte = true;
					break;
					}
				theCP <<= 6;
				theCP += curByte & 0x3F;
				}

			if (not gotByte)
				*localDest++ = theCP;
			}
		}
	return localDest - oDest;
	}

// =================================================================================================
#pragma mark - ChanR_UTF_Chan_Bin_UTF8

ChanR_UTF_Chan_Bin_UTF8::ChanR_UTF_Chan_Bin_UTF8(const ChanR_Bin& iChanR_Bin)
:	fChanR_Bin(iChanR_Bin)
	{}

size_t ChanR_UTF_Chan_Bin_UTF8::Read(UTF32* oDest, size_t iCount)
	{ return spReadUTF32_UTF8(oDest, iCount, fChanR_Bin); }

// =================================================================================================
#pragma mark - ChanW_UTF_Chan_Bin_UTF8

ChanW_UTF_Chan_Bin_UTF8::ChanW_UTF_Chan_Bin_UTF8(const ChanW_Bin& iChanW_Bin)
:	fChanW_Bin(iChanW_Bin)
	{}

size_t ChanW_UTF_Chan_Bin_UTF8::WriteUTF8(const UTF8* iSource, size_t iCountCU)
	{ return sWriteMem(fChanW_Bin, iSource, iCountCU); }

void ChanW_UTF_Chan_Bin_UTF8::Flush()
	{ sFlush(fChanW_Bin); }

} // namespace ZooLib
