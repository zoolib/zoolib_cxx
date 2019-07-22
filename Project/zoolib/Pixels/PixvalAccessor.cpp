// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/PixvalAccessor.h"

#include "zoolib/Memory.h"
#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {
namespace Pixels {

static const uint8 s1BitMaskBE[] = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };
static const uint8 s1BitNotMaskBE[] = { 0x7FU, 0xBFU, 0xDFU, 0xEFU, 0xF7U, 0xFBU, 0xFDU, 0xFEU };
static const uint8 s1BitShiftBE[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

static const uint8 s1BitMaskLE[] = { 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x20U, 0x40U, 0x80U};
static const uint8 s1BitNotMaskLE[] = { 0xFEU, 0xFDU, 0xFBU, 0xF7U, 0xEFU, 0xDFU, 0xBFU, 0x7FU };
static const uint8 s1BitShiftLE[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

static const uint8 s2BitMaskBE[] = { 0xC0U, 0x30U, 0x0CU, 0x03U };
static const uint8 s2BitNotMaskBE[] = { 0x3FU, 0xCFU, 0xF3U, 0xFCU };
static const uint8 s2BitShiftBE[] = { 6, 4, 2, 0 };

static const uint8 s2BitMaskLE[] = { 0x03U, 0x0CU, 0x30U, 0xC0U };
static const uint8 s2BitNotMaskLE[] = { 0xFCU, 0xF3U, 0xCFU, 0x3FU };
static const uint8 s2BitShiftLE[] = { 0, 2, 4, 6 };

static const uint8 s4BitMaskBE[] = { 0xF0U, 0x0FU };
static const uint8 s4BitNotMaskBE[] = { 0x0FU, 0xF0U };
static const uint8 s4BitShiftBE[] = { 4, 0 };

static const uint8 s4BitMaskLE[] = { 0x0FU, 0xF0U };
static const uint8 s4BitNotMaskLE[] = { 0xF0U, 0x0FU };
static const uint8 s4BitShiftLE[] = { 0, 4 };

// =================================================================================================
#pragma mark - PixvalAccessor

PixvalAccessor::PixvalAccessor(const PixvalDesc& iPixvalDesc)
:	PixvalAccessor(iPixvalDesc.fDepth, iPixvalDesc.fBigEndian)
	{}

PixvalAccessor::PixvalAccessor(int iDepth, bool iBigEndian)
:	fDepth(iDepth)
	{
	switch (fDepth)
		{
		case 1:
			{
			fDivisorShift = 3;
			fModMask = 0x07;
			if (iBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s1BitMaskBE;
				fBitNotMask = s1BitNotMaskBE;
				fBitShift = s1BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s1BitMaskLE;
				fBitNotMask = s1BitNotMaskLE;
				fBitShift = s1BitShiftLE;
				}
			break;
			}
		case 2:
			{
			fDivisorShift = 2;
			fModMask = 0x03;
			if (iBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s2BitMaskBE;
				fBitNotMask = s2BitNotMaskBE;
				fBitShift = s2BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s2BitMaskLE;
				fBitNotMask = s2BitNotMaskLE;
				fBitShift = s2BitShiftLE;
				}
			break;
			}
		case 3: case 4:
			{
			fDivisorShift = 1;
			fModMask = 0x01;
			if (iBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s4BitMaskBE;
				fBitNotMask = s4BitNotMaskBE;
				fBitShift = s4BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s4BitMaskLE;
				fBitNotMask = s4BitNotMaskLE;
				fBitShift = s4BitShiftLE;
				}
			break;
			}
		case 5: case 6: case 7: case 8:
			{
			fWhichCase = eCase1Byte;
			break;
			}
		case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
			{
			if (iBigEndian == ZCONFIG(Endian, Big))
				fWhichCase = eCase2Bytes;
			else
				fWhichCase = eCase2BytesWithSwap;
			break;
			}
		case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24:
			{
			if ZCONFIG(Endian, Big)
				{
				if (iBigEndian)
					fWhichCase = eCase3BytesBEHostBE;
				else
					fWhichCase = eCase3BytesLEHostBE;
				}
			else
				{
				if (iBigEndian)
					fWhichCase = eCase3BytesBEHostLE;
				else
					fWhichCase = eCase3BytesLEHostLE;
				}
			break;
			}
		case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
			{
			if (iBigEndian == ZCONFIG(Endian, Big))
				fWhichCase = eCase4Bytes;
			else
				fWhichCase = eCase4BytesWithSwap;
			break;
			}
		default:
			{
			ZDebugStopf(0, "%d depth out of range", iDepth);
			}
		}
	}

Pixval PixvalAccessor::GetPixval(const void* iRowAddress, int iH) const
	{
	uint32 value;
	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			value = static_cast<const uint8*>(iRowAddress)[iH];
			break;
			}
		case eCase1ByteWithShiftBE:
		case eCase1ByteWithShiftLE:
			{
			value = static_cast<const uint8*>(iRowAddress)[iH >> fDivisorShift];
			value &= fBitMask[iH & fModMask];
			value >>= fBitShift[iH & fModMask];
			break;
			}
		case eCase2Bytes:
			{
			value = static_cast<const uint16*>(iRowAddress)[iH];
			break;
			}
		case eCase2BytesWithSwap:
			{
			value = ZByteSwap_Read16(static_cast<const uint16*>(iRowAddress) + iH);
			break;
			}
		case eCase3BytesBEHostBE:
		case eCase3BytesBEHostLE:
			{
			const uint8* temp = static_cast<const uint8*>(iRowAddress) + (iH * 3);
			value = temp[0] << 16;
			value |= temp[1] << 8;
			value |= temp[2];
			break;
			}
		case eCase3BytesLEHostBE:
		case eCase3BytesLEHostLE:
			{
			const uint8* temp = static_cast<const uint8*>(iRowAddress) + (iH * 3);
			value = temp[0];
			value |= temp[1] << 8;
			value |= temp[2] << 16;
			break;
			}
		case eCase4Bytes:
			{
			value = static_cast<const uint32*>(iRowAddress)[iH];
			break;
			}
		case eCase4BytesWithSwap:
			{
			value = ZByteSwap_Read32(static_cast<const uint32*>(iRowAddress) + iH);
			break;
			}
		default:
			ZUnimplemented();
		}
	return value;
	}

void PixvalAccessor::SetPixval(void* iRowAddress, int iH, Pixval iPixval) const
	{
	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			static_cast<uint8*>(iRowAddress)[iH] = iPixval;
			break;
			}
		case eCase1ByteWithShiftBE:
		case eCase1ByteWithShiftLE:
			{
			uint8* destAddress =
				static_cast<uint8*>(iRowAddress) + (iH >> fDivisorShift);
			*destAddress = (*destAddress & fBitNotMask[iH & fModMask])
				| (iPixval << fBitShift[iH & fModMask]);
			break;
			}
		case eCase2Bytes:
			{
			static_cast<uint16*>(iRowAddress)[iH] = iPixval;
			break;
			}
		case eCase2BytesWithSwap:
			{
			ZByteSwap_Write16(static_cast<uint16*>(iRowAddress) + iH, iPixval);
			break;
			}
		case eCase3BytesBEHostBE:
		case eCase3BytesBEHostLE:
			{
			uint8* temp = static_cast<uint8*>(iRowAddress) + (iH * 3);
			temp[0] = iPixval >> 16;
			temp[1] = iPixval >> 8;
			temp[2] = iPixval;
			break;
			}
		case eCase3BytesLEHostBE:
		case eCase3BytesLEHostLE:
			{
			uint8* temp = static_cast<uint8*>(iRowAddress) + (iH * 3);
			temp[0] = iPixval;
			temp[1] = iPixval >> 8;
			temp[2] = iPixval >> 16;
			break;
			}
		case eCase4Bytes:
			{
			static_cast<uint32*>(iRowAddress)[iH] = iPixval;
			break;
			}
		case eCase4BytesWithSwap:
			{
			ZByteSwap_Write32(static_cast<uint32*>(iRowAddress) + iH, iPixval);
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

#if 0
// GetPixvals and SetPixvals effectively do the equivalent of the following, but do so
// substantially more efficiently.
void PixvalAccessor::GetPixvals(const void* iRowAddress,
	int32 iStartH, size_t iCount, uint32* oPixvals) const
	{
	while (iCount--)
		*oPixvals++ = this->GetPixval(iRowAddress, iStartH++);
	}

void PixvalAccessor::SetPixvals(void* iRowAddress,
	int32 iStartH, size_t iCount, const Pixval* iPixvals) const
	{
	while (iCount--)
		this->SetPixval(iRowAddress, iStartH++, *iPixvals++);
	}

#endif

void PixvalAccessor::GetPixvals(const void* iRowAddress, int iStartH, size_t iCount,
	Pixval* oPixvals) const
	{
	// There are multiple cases handled by this function and by SetPixvals. Many of the cases
	// want to use variables with the same names and types. Normally I'm a strong advogate of
	// declaring variables at their point of first use, but that makes it hard to know which
	// variable to examine when looking at this code in a source level debugger, so I'm
	// declaring the common names here.

	size_t localCount;
	const uint8* localSource8;
	const uint16* localSource16;
	const uint32* localSource32;
	uint32 value;

	uint32 a0b0c0a1;
	uint32 b1c1a2b2;
	uint32 c2a3b3c3;
	uint32 c1a0b0c0;
	uint32 b2c2a1b1;
	uint32 a3b3c3a2;

	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			localCount = iCount + 1;
			localSource8 = static_cast<const uint8*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = *localSource8++;
			break;
			}
		case eCase1ByteWithShiftBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH >> fDivisorShift);
			const int32 leftShift = fDepth;
			const int32 rightShift = 8 - fDepth;
			const size_t masterLoadCount = 8 / fDepth;
			uint8 currentSourceValue = *localSource8++;
			currentSourceValue = currentSourceValue << (leftShift * (iStartH & fModMask));
			size_t currentLoadCount = masterLoadCount - (iStartH & fModMask);
			localCount = iCount;
			while (localCount)
				{
				--localCount;
				--currentLoadCount;
				uint8 newSourceValue = currentSourceValue << leftShift;
				uint32 value = currentSourceValue >> rightShift;
				if (currentLoadCount)
					{
					currentSourceValue = newSourceValue;
					}
				else
					{
					currentSourceValue = *localSource8++;
					currentLoadCount = masterLoadCount;
					}
				*oPixvals++ = value;
				}
			break;
			}
		case eCase1ByteWithShiftLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH >> fDivisorShift);
			const int32 rightShift = fDepth;
			const uint32 mask = (1 << fDepth) - 1;
			const size_t masterLoadCount = 8 / fDepth;
			uint8 currentSourceValue = *localSource8++;
			currentSourceValue = currentSourceValue >> (rightShift * (iStartH & fModMask));
			size_t currentLoadCount = masterLoadCount - (iStartH & fModMask);
			localCount = iCount;
			while (localCount)
				{
				--localCount;
				--currentLoadCount;
				uint8 newSourceValue = currentSourceValue >> rightShift;
				uint32 value = currentSourceValue & mask;
				if (currentLoadCount)
					{
					currentSourceValue = newSourceValue;
					}
				else
					{
					currentSourceValue = *localSource8++;
					currentLoadCount = masterLoadCount;
					}
				*oPixvals++ = value;
				}
			break;
			}
		case eCase2Bytes:
			{
			localCount = iCount + 1;
			localSource16 = static_cast<const uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = *localSource16++;
			break;
			}
		case eCase2BytesWithSwap:
			{
			localCount = iCount + 1;
			localSource16 = static_cast<const uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = uint16(ZByteSwap_Read16(localSource16++));
			break;
			}
		case eCase3BytesBEHostBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0a1 = localSource32[0];
					b1c1a2b2 = localSource32[1];
					c2a3b3c3 = localSource32[2];
					localSource32 += 3;

					oPixvals[0] = a0b0c0a1 >> 8;
					oPixvals[1] = ((a0b0c0a1 & 0x000000FF) << 16) | (b1c1a2b2 >> 16);
					oPixvals[2] = ((b1c1a2b2 & 0x0000FFFF) << 8) | (c2a3b3c3 >> 24);
					oPixvals[3] = c2a3b3c3 & 0x00FFFFFF;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase3BytesBEHostLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0a1 = ZByteSwap_Read32(localSource32++);
					b1c1a2b2 = ZByteSwap_Read32(localSource32++);
					c2a3b3c3 = ZByteSwap_Read32(localSource32++);

					oPixvals[0] = a0b0c0a1 >> 8;
					oPixvals[1] = ((a0b0c0a1 & 0x000000FF) << 16) | (b1c1a2b2 >> 16);
					oPixvals[2] = ((b1c1a2b2 & 0x0000FFFF) << 8) | (c2a3b3c3 >> 24);
					oPixvals[3] = c2a3b3c3 & 0x00FFFFFF;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase3BytesLEHostBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;

					// AG 2002-04-27. We run out of registers when compiling this
					// with CW Mach, hence the use of spByteSwap_Read32.
					#if __MWERKS__ && __MACH__
					c1a0b0c0 = spByteSwap_Read32(localSource32++);
					b2c2a1b1 = spByteSwap_Read32(localSource32++);
					a3b3c3a2 = spByteSwap_Read32(localSource32++);
					#else
					c1a0b0c0 = ZByteSwap_Read32(localSource32++);
					b2c2a1b1 = ZByteSwap_Read32(localSource32++);
					a3b3c3a2 = ZByteSwap_Read32(localSource32++);
					#endif

					oPixvals[0] = c1a0b0c0 & 0x00FFFFFF;
					oPixvals[1] = (c1a0b0c0 >> 24) | ((b2c2a1b1 & 0x0000FFFF) << 8);
					oPixvals[2] = (b2c2a1b1 >> 16) | ((a3b3c3a2 & 0x000000FF) << 16);
					oPixvals[3] = a3b3c3a2 >> 8;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				--localCount;
				}
			break;
			}
		case eCase3BytesLEHostLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					c1a0b0c0 = localSource32[0];
					b2c2a1b1 = localSource32[1];
					a3b3c3a2 = localSource32[2];
					localSource32 += 3;

					oPixvals[0] = c1a0b0c0 & 0x00FFFFFF;
					oPixvals[1] = (c1a0b0c0 >> 24) | ((b2c2a1b1 & 0x0000FFFF) << 8);
					oPixvals[2] = (b2c2a1b1 >> 16) | ((a3b3c3a2 & 0x000000FF) << 16);
					oPixvals[3] = a3b3c3a2 >> 8;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase4Bytes:
			{
			sMemCopy(oPixvals,
				static_cast<const uint32*>(iRowAddress) + iStartH, iCount * sizeof(uint32));
			break;
			}
		case eCase4BytesWithSwap:
			{
			localCount = iCount + 1;
			localSource32 = static_cast<const uint32*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = uint32(ZByteSwap_Read32(localSource32++));
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

void PixvalAccessor::SetPixvals(void* iRowAddress, int iStartH, size_t iCount,
	const Pixval* iPixvals) const
	{
	size_t localCount;
	uint8* localDest8;
	uint16* localDest16;
	uint32* localDest32;
	uint32 a0b0c0;
	uint32 a1b1c1;
	uint32 a2b2c2;
	uint32 a3b3c3;
	uint32 value;

	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			localCount = iCount + 1;
			localDest8 = static_cast<uint8*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest8++ = *iPixvals++;
			break;
			}
		case eCase1ByteWithShiftBE:
			{
			uint8 destVal8;
			uint8 destMask8;
			uint8 cumuMask8;
			localCount = iCount;
			switch (fDepth)
				{
				case 1:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 3);
					if (iStartH & 0x07)
						{
						destVal8 = 0;
						destMask8 = 0x80 >> (iStartH & 0x07);
						cumuMask8 = 0;
						while ((iStartH & 0x07) && localCount)
							{
							++iStartH;
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 8)
						{
						localCount -= 8;
						destVal8 = 0;
						if (*iPixvals++ & 0x01) destVal8 |= 0x80;
						if (*iPixvals++ & 0x01) destVal8 |= 0x40;
						if (*iPixvals++ & 0x01) destVal8 |= 0x20;
						if (*iPixvals++ & 0x01) destVal8 |= 0x10;
						if (*iPixvals++ & 0x01) destVal8 |= 0x08;
						if (*iPixvals++ & 0x01) destVal8 |= 0x04;
						if (*iPixvals++ & 0x01) destVal8 |= 0x02;
						if (*iPixvals++ & 0x01) destVal8 |= 0x01;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x80;
						cumuMask8 = 0;
						while (localCount)
							{
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 2:
					{
					int32 sourceShift;
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 2);
					if (iStartH & 0x03)
						{
						destVal8 = 0;
						destMask8 = 0xC0 >> (2 * (iStartH & 0x03));
						cumuMask8 = 0;
						sourceShift = 6 - (2 * (iStartH & 0x03));
						while ((iStartH & 0x03) && localCount)
							{
							++iStartH;
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift -= 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;						
						}

					while (localCount >= 4)
						{
						localCount -= 4;
						destVal8 = (*iPixvals++ & 0x03) << 6;
						destVal8 |= (*iPixvals++ & 0x03) << 4;
						destVal8 |= (*iPixvals++ & 0x03) << 2;
						destVal8 |= (*iPixvals++ & 0x03);
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0xC0;
						cumuMask8 = 0;
						sourceShift = 6;
						while (localCount)
							{
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift -= 2;
							}
						*localDest8 = ((*localDest8 & (0xFF ^ cumuMask8))) | destVal8;
						}
					break;
					}
				case 3: case 4:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 1);
					if (iStartH & 0x01)
						{
						--localCount;
						*localDest8 = (*localDest8 & 0xF0) | (*iPixvals++ & 0x0F);
						++localDest8;
						}

					while (localCount >= 2)
						{
						localCount -= 2;
						uint8 temp = ((*iPixvals++ & 0x0F) << 4);
						*localDest8++ = temp | (*iPixvals++ & 0x0F);
						}

					if (localCount)
						*localDest8 = (*localDest8 & 0x0F) | ((*iPixvals++ & 0x0F) << 4);
					break;
					}
				}
			break;
			}
		case eCase1ByteWithShiftLE:
			{
			uint8 destVal8;
			uint8 destMask8;
			uint8 cumuMask8;
			localCount = iCount;
			switch (fDepth)
				{
				case 1:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 3);
					if (iStartH & 0x07)
						{
						destVal8 = 0;
						destMask8 = 0x01 << (iStartH & 0x07);
						cumuMask8 = 0;
						while ((iStartH & 0x07) && localCount)
							{
							++iStartH;
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 8)
						{
						localCount -= 8;
						destVal8 = 0;
						if (*iPixvals++ & 0x01) destVal8 |= 0x01;
						if (*iPixvals++ & 0x01) destVal8 |= 0x02;
						if (*iPixvals++ & 0x01) destVal8 |= 0x04;
						if (*iPixvals++ & 0x01) destVal8 |= 0x08;
						if (*iPixvals++ & 0x01) destVal8 |= 0x10;
						if (*iPixvals++ & 0x01) destVal8 |= 0x20;
						if (*iPixvals++ & 0x01) destVal8 |= 0x40;
						if (*iPixvals++ & 0x01) destVal8 |= 0x80;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x01;
						cumuMask8 = 0;
						while (localCount)
							{
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 2:
					{
					int32 sourceShift;
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 2);
					if (iStartH & 0x03)
						{
						destVal8 = 0;
						destMask8 = 0x03 << (2 * (iStartH & 0x03));
						cumuMask8 = 0;
						sourceShift = 0;
						while ((iStartH & 0x03) && localCount)
							{
							++iStartH;
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift += 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 4)
						{
						localCount -= 4;
						destVal8 = (*iPixvals++ & 0x03);
						destVal8 |= (*iPixvals++ & 0x03) << 2;
						destVal8 |= (*iPixvals++ & 0x03) << 4;
						destVal8 |= (*iPixvals++ & 0x03) << 6;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x03;
						cumuMask8 = 0;
						sourceShift = 0;
						while (localCount)
							{
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift += 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 3: case 4:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 1);
					if (iStartH & 0x01)
						{
						--localCount;
						*localDest8 = (*localDest8 & 0x0F) | ((*iPixvals++ & 0x0F) << 4);
						++localDest8;
						}

					while (localCount >= 2)
						{
						localCount -= 2;
						uint8 temp = (*iPixvals++ & 0x0F);
						*localDest8++ = temp | ((*iPixvals++ & 0x0F) << 4);
						}

					if (localCount)
						*localDest8 = (*localDest8 & 0xF0) | (*iPixvals++ & 0x0F);
					break;
					}
				}
			break;
			}
		case eCase2Bytes:
			{
			localCount = iCount + 1;
			localDest16 = static_cast<uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest16++ = *iPixvals++;
			break;
			}
		case eCase2BytesWithSwap:
			{
			localCount = iCount + 1;
			localDest16 = static_cast<uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest16++ = uint16(ZByteSwap_Read16(iPixvals++));
			break;
			}
		case eCase3BytesBEHostBE:
			{
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0];
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					localDest32[0] = (a0b0c0 << 8) | (a1b1c1 >> 16);
					localDest32[1] = (a1b1c1 << 16) | (a2b2c2 >> 8);
					localDest32[2] = (a2b2c2 << 24) | a3b3c3;
					localDest32 += 3;
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesBEHostLE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0];
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					ZByteSwap_Write32(localDest32++, (a0b0c0 << 8) | (a1b1c1 >> 16));
					ZByteSwap_Write32(localDest32++, (a1b1c1 << 16) | (a2b2c2 >> 8));
					ZByteSwap_Write32(localDest32++, (a2b2c2 << 24) | a3b3c3);
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesLEHostBE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0] & 0x00FFFFFF;
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					#if __MWERKS__ && __MACH__
					spByteSwap_Write32(localDest32++, a0b0c0 | (a1b1c1 << 24));
					spByteSwap_Write32(localDest32++, (a1b1c1 >> 8) | (a2b2c2 << 16));
					spByteSwap_Write32(localDest32++, (a2b2c2 >> 16) | (a3b3c3 << 8));
					#else
					ZByteSwap_Write32(localDest32++, a0b0c0 | (a1b1c1 << 24));
					ZByteSwap_Write32(localDest32++, (a1b1c1 >> 8) | (a2b2c2 << 16));
					ZByteSwap_Write32(localDest32++, (a2b2c2 >> 16) | (a3b3c3 << 8));
					#endif
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesLEHostLE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				uint32* localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0] & 0x00FFFFFF;
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					localDest32[0] = a0b0c0 | (a1b1c1 << 24);
					localDest32[1] = (a1b1c1 >> 8) | (a2b2c2 << 16);
					localDest32[2] = (a2b2c2 >> 16) | (a3b3c3 << 8);
					localDest32 += 3;
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}
			break;
			}
		case eCase4Bytes:
			{
			sMemCopy(static_cast<uint32*>(iRowAddress) + iStartH,
				iPixvals, iCount * sizeof(uint32));
			break;
			}
		case eCase4BytesWithSwap:
			{
			localCount = iCount + 1;
			localDest32 = static_cast<uint32*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest32++ = uint32(ZByteSwap_Read32(iPixvals++));
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

} // namespace Pixels
} // namespace ZooLib
