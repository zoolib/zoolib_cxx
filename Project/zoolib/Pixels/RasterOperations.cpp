// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/RasterOperations.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/Memory.h"
#include "zoolib/ZDebug.h"

#include "zoolib/Pixels/Cartesian_Geom.h"
#include "zoolib/Pixels/PixvalAccessor.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

static const size_t kBufSize = sStackBufferSize;

using std::min;
using std::vector;

// =================================================================================================
#pragma mark -

Pixval sGetPixval(const RasterDesc& iRD, const void* iBaseAddress, int iH, int iV)
	{
	const void* rowAddress = sCalcRowAddress(iRD, iBaseAddress, iV);
	return PixvalAccessor(iRD.fPixvalDesc).GetPixval(rowAddress, iH);
	}

void sSetPixval(const RasterDesc& iRD, void* oBaseAddress, int iH, int iV, Pixval iPixval)
	{
	void* rowAddress = sCalcRowAddress(iRD, oBaseAddress, iV);
	PixvalAccessor(iRD.fPixvalDesc).SetPixval(rowAddress, iH, iPixval);
	}

void sFill(const RasterDesc& iRD, void* oBaseAddress, Pixval iPixval)
	{
	if (iRD.fPixvalDesc.fDepth >= 17 && iRD.fPixvalDesc.fDepth <= 24)
		{
		// For 17 to 24 bit pixVals we may have to write separately each
		// of the three bytes used per pixVal.
		uint8 byte1, byte2, byte3;
		if (iRD.fPixvalDesc.fBigEndian)
			{
			byte1 = (iPixval >> 16) & 0xFF;
			byte2 = (iPixval >> 8) & 0xFF;
			byte3 = iPixval & 0xFF;
			}
		else
			{
			byte1 = iPixval & 0xFF;
			byte2 = (iPixval >> 8) & 0xFF;
			byte3 = (iPixval >> 16) & 0xFF;
			}

		if (byte1 == byte2 && byte2 == byte3)
			{
			// We're lucky -- all the bytes of the pixVal are the same, so we can use sMemSet.
			sMemSet(oBaseAddress, byte1, iRD.fRowBytes * iRD.fRowCount);
			}
		else
			{
			size_t localRowCount = iRD.fRowCount + 1;
			size_t localChunkCount = (iRD.fRowBytes / 3) + 1;
			uint8* localBaseAddress = static_cast<uint8*>(oBaseAddress);
			while (--localRowCount)
				{
				size_t chunkCount = localChunkCount;
				uint8* chunkAddress = localBaseAddress;
				localBaseAddress += iRD.fRowBytes;
				while (--chunkCount)
					{
					*chunkAddress++ = byte1;
					*chunkAddress++ = byte2;
					*chunkAddress++ = byte3;
					}
				}
			}
		}
	else
		{
		// We're going to build a Pixval to fill the raster with, but this will only
		// work correctly if rowBytes is a multiple of four.
		ZAssertStop(1, (iRD.fRowBytes & 0x03) == 0);
		int effectiveDepth;
		switch (iRD.fPixvalDesc.fDepth)
			{
			case 1:
				effectiveDepth = 1;
				break;
			case 2:
				effectiveDepth = 2;
				break;
			case 3: case 4:
				effectiveDepth = 4;
				break;
			case 5: case 6: case 7: case 8:
				effectiveDepth = 8;
				break;
			case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
				effectiveDepth = 16;
				break;
			case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
				effectiveDepth = 32;
				break;
			default:
				ZUnimplemented();
			}

		Pixval storedValue = iPixval;
		if (effectiveDepth < 32)
			{
			// Smear the < 32 bits across all 32 bits (we'll only do
			// more than one iteration if effective depth was 16 or less).
			storedValue &= (1 << effectiveDepth) - 1;
			while (effectiveDepth < 32)
				{
				storedValue = storedValue | (storedValue << effectiveDepth);
				effectiveDepth *= 2;
				}
			}

		if (((storedValue & 0xFFFF) == ((storedValue >> 16) & 0xFFFF))
			&& ((storedValue & 0xFF) == ((storedValue >> 8) & 0xFF)))
			{
			// We're lucky -- all the bytes of the pixVal are the same so we can use sMemSet.
			sMemSet(oBaseAddress,
				storedValue & 0xFF, iRD.fRowBytes * iRD.fRowCount);
			}
		else
			{
			if (iRD.fPixvalDesc.fBigEndian != ZCONFIG(Endian, Big))
				storedValue = sByteSwapped(storedValue);
			size_t chunkCount = ((iRD.fRowBytes * iRD.fRowCount) / 4) + 1;
			Pixval* chunkAddress = static_cast<Pixval*>(oBaseAddress);
			while (--chunkCount)
				*chunkAddress++ = storedValue;
			}
		}
	}

void sFill(const RasterDesc& iRD, void* oBaseAddress, const RectPOD& iBounds, Pixval iPixval)
	{
	const Ord width = W(iBounds);

	vector<Pixval> sourcePixvals(width, iPixval);

	PixvalAccessor destAccessor(iRD.fPixvalDesc);

	const Ord height = H(iBounds);

	for (Ord vCurrent = 0; vCurrent < height; ++vCurrent)
		{
		void* rowAddress = sCalcRowAddress(iRD, oBaseAddress, T(iBounds) + vCurrent);
		destAccessor.SetPixvals(rowAddress, L(iBounds), width, &sourcePixvals[0]);
		}
	}

void sBlitRowPixvals(
	const PixvalDesc& iSourcePvD, const void* iSourceBase, int iSourceH,
	const PixvalDesc& iDestPvD, void* oDestBase, int iDestH,
	int iCount)
	{
	if (iSourcePvD.fDepth == iDestPvD.fDepth
		&& iSourcePvD.fBigEndian == iDestPvD.fBigEndian
		&& (iSourcePvD.fDepth >= 8
			|| ((iSourceH | iDestH | iCount) & 0x07) == 0))
		{
		Ord hOffsetSource = iSourceH * iSourcePvD.fDepth / 8;
		Ord hOffsetDest = iDestH * iDestPvD.fDepth / 8;
		Ord countToCopy = iCount * iSourcePvD.fDepth / 8;
		const uint8* sourceAddress = static_cast<const uint8*>(iSourceBase) + hOffsetSource;
		uint8* destAddress = static_cast<uint8*>(oDestBase) + hOffsetDest;
		sMemCopy(destAddress, sourceAddress, countToCopy);
		}
	else
		{
		PixvalAccessor sourceAccessor(iSourcePvD);
		PixvalAccessor destAccessor(iDestPvD);

		for (Ord hCurrent = 0; hCurrent < iCount; /*no inc*/)
			{
			Pixval buffer[kBufSize];
			const Ord count = min(Ord(iCount - hCurrent), Ord(kBufSize));
			sourceAccessor.GetPixvals(iSourceBase, iSourceH + hCurrent, count, buffer);
			destAccessor.SetPixvals(oDestBase, iDestH + hCurrent, count, buffer);
			hCurrent += count;
			}
		}
	}

void sBlitPixvals(
	const RasterDesc& iSourceRD, const void* iSourceBase, const RectPOD& iSourceBounds,
	const RasterDesc& iDestRD, void* oDestBase, const PointPOD& iDestLocation)
	{
	Ord vCount = H(iSourceBounds);
	Ord hCount = W(iSourceBounds);

	if (iSourceRD.fPixvalDesc == iDestRD.fPixvalDesc
		&& (iSourceRD.fPixvalDesc.fDepth >= 8
			|| ((iSourceBounds.left | iDestLocation.h | hCount) & 0x07) == 0))
		{
		Ord hOffsetSource = iSourceBounds.left * iSourceRD.fPixvalDesc.fDepth / 8;
		Ord hOffsetDest = iDestLocation.h * iSourceRD.fPixvalDesc.fDepth / 8;
		Ord countToCopy = hCount * iSourceRD.fPixvalDesc.fDepth / 8;
		for (Ord vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const uint8* sourceRowAddress = static_cast<const uint8*>(
				sCalcRowAddress(iSourceRD, iSourceBase, iSourceBounds.top + vCurrent))
				+ hOffsetSource;

			uint8* destRowAddress = static_cast<uint8*>(
				sCalcRowAddress(iDestRD, oDestBase, iDestLocation.v + vCurrent))
				+ hOffsetDest;

			sMemCopy(destRowAddress, sourceRowAddress, countToCopy);
			}
		}
	else
		{
		for (Ord vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				sCalcRowAddress(iSourceRD, iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				sCalcRowAddress(iDestRD, oDestBase, iDestLocation.v + vCurrent);

			PixvalAccessor sourceAccessor(iSourceRD.fPixvalDesc);
			PixvalAccessor destAccessor(iDestRD.fPixvalDesc);

			for (Ord hCurrent = 0; hCurrent < hCount; /*no inc*/)
				{
				Pixval buffer[kBufSize];
				const size_t count = min(size_t(hCount - hCurrent), kBufSize);

				sourceAccessor.GetPixvals(sourceRowAddress,
					hCurrent + iSourceBounds.left, count, buffer);

				destAccessor.SetPixvals(destRowAddress, hCurrent + iDestLocation.h, count, buffer);
				hCurrent += count;
				}
			}
		}
	}

} // namespace Pixels
} // namespace ZooLib

