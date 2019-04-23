/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Cartesian_Geom.h"
#include "zoolib/Pixels/PixvalAccessor.h"

#include "zoolib/ByteSwap.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

using std::min;
using std::vector;

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - sFill

void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval)
	{
	if (iRasterDesc.fPixvalDesc.fDepth >= 17 && iRasterDesc.fPixvalDesc.fDepth <= 24)
		{
		// For 17 to 24 bit pixVals we may have to write separately each
		// of the three bytes used per pixVal.
		uint8 byte1, byte2, byte3;
		if (iRasterDesc.fPixvalDesc.fBigEndian)
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
			sMemSet(iBaseAddress, byte1, iRasterDesc.fRowBytes * iRasterDesc.fRowCount);
			}
		else
			{
			size_t localRowCount = iRasterDesc.fRowCount + 1;
			size_t localChunkCount = (iRasterDesc.fRowBytes / 3) + 1;
			uint8* localBaseAddress = static_cast<uint8*>(iBaseAddress);
			while (--localRowCount)
				{
				size_t chunkCount = localChunkCount;
				uint8* chunkAddress = localBaseAddress;
				localBaseAddress += iRasterDesc.fRowBytes;
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
		// We're going to build a uint32 to fill the raster with, but this will only
		// work correctly if rowBytes is a multiple of four.
		ZAssertStop(1, (iRasterDesc.fRowBytes & 0x03) == 0);
		int32 effectiveDepth;
		switch (iRasterDesc.fPixvalDesc.fDepth)
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

		uint32 storedValue = iPixval;
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
			sMemSet(iBaseAddress,
				storedValue & 0xFF, iRasterDesc.fRowBytes * iRasterDesc.fRowCount);
			}
		else
			{
			if (iRasterDesc.fPixvalDesc.fBigEndian != ZCONFIG(Endian, Big))
				storedValue = sByteSwapped(storedValue);
			size_t chunkCount = ((iRasterDesc.fRowBytes * iRasterDesc.fRowCount) / 4) + 1;
			uint32* chunkAddress = static_cast<uint32*>(iBaseAddress);
			while (--chunkCount)
				*chunkAddress++ = storedValue;
			}
		}
	}

void sFill(
	void* iBaseAddress, const RasterDesc& iRasterDesc, const RectPOD& iBounds, uint32 iPixval)
	{
	int32 hSize = W(iBounds);

	vector<uint32> sourcePixvals(hSize, iPixval);

	PixvalAccessor destAccessor(iRasterDesc.fPixvalDesc);

	int32 vSize = H(iBounds);

	for (int32 vCurrent = 0; vCurrent < vSize; ++vCurrent)
		{
		void* rowAddress = sCalcRowAddress(iRasterDesc, iBaseAddress, iBounds.top + vCurrent);
		destAccessor.SetPixvals(rowAddress, iBounds.left, hSize, &sourcePixvals[0]);
		}
	}

// =================================================================================================
#pragma mark - sMunge

template <class S, class D>
static void sMungeRow_T(
	void* iRowAddress, const PixvalDesc& iPixvalDesc,
	const S& iMapPixvalToRGB, const D& iMapRGBToPixval,
	Ord inStartH, int32 inCount, Ord inCoordV,
	MungeProc iMungeProc, void* iRefcon)
	{
	PixvalAccessor sourceAccessor(iPixvalDesc);
	int32 hCurrent = 0;
	while (hCurrent < inCount)
		{
		uint32 sourceValue = sourceAccessor.GetPixval(iRowAddress, inStartH + hCurrent);
		RGBA theRGBA = iMapPixvalToRGB.AsRGBA(sourceValue);
		if (iMungeProc(hCurrent, inCoordV, theRGBA, iRefcon))
			{
			uint32 destValue = iMapRGBToPixval.AsPixval(theRGBA);
			sourceAccessor.SetPixval(iRowAddress, inStartH + hCurrent, destValue);
			}
		++hCurrent;
		}
	}

template <class S, class D>
static void sMunge_T(
	void* iBaseAddress, const RasterDesc& iRasterDesc,
	const S& iMapPixvalToRGB, const D& iMapRGBToPixval,
	const RectPOD& iBounds, MungeProc iMungeProc, void* iRefcon)
	{
	int32 hSize = W(iBounds);
	int32 vSize = H(iBounds);

	for (int32 vCurrent = 0; vCurrent < vSize; ++vCurrent)
		{
		void* rowAddress = sCalcRowAddress(iRasterDesc, iBaseAddress, iBounds.top + vCurrent);
		sMungeRow_T(rowAddress, iRasterDesc.fPixvalDesc,
				iMapPixvalToRGB, iMapRGBToPixval,
				iBounds.left, hSize, vCurrent,
				iMungeProc, iRefcon);
		}
	}

void sMunge(
	void* iBaseAddress, const RasterDesc& iRasterDesc, const PixelDesc& iPixelDescReal,
	const RectPOD& iBounds, MungeProc iMungeProc, void* iRefcon)
	{
	ZRef<PixelDescRep> thePixelDescRep = iPixelDescReal;
	const PixelDescRep& iPixelDesc = *thePixelDescRep;


	if (PixelDescRep_Color* thePixelDescRep_Color =
		thePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Color, *thePixelDescRep_Color, iBounds, iMungeProc, iRefcon);
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
		thePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		thePixelDescRep_Indexed->BuildReverseLookupIfNeeded();
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Indexed, *thePixelDescRep_Indexed, iBounds, iMungeProc, iRefcon);
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray =
		thePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Gray, *thePixelDescRep_Gray, iBounds, iMungeProc, iRefcon);
		}
	else
		{
		sMunge_T(iBaseAddress, iRasterDesc, iPixelDesc, iPixelDesc, iBounds, iMungeProc, iRefcon);
		}
	}

// =================================================================================================
#pragma mark - sBlit

template <class S, class D>
static void sBlitRow_T(
	const void* iSourceRowAddress, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGBA,
	int32 iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int32 hCurrent = 0;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int32 tempCount = count + 1;
		uint32* currentPixval = buffer;
		while (--tempCount)
			{
			RGBA theRGBA = iSourcePixvalToRGBA.AsRGBA(*currentPixval);
			*currentPixval++ = iDestRGBToPixval.AsPixval(theRGBA);
			}
		destAccessor.SetPixvals(iDestRowAddress, iDestH + hCurrent, count, buffer);
		hCurrent += count;
		}
	}

template <class S, class D>
static void sBlitRowInvert_T(
	const void* iSourceRowAddress, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGBA,
	int32 iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int32 hCurrent = 0;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int32 tempCount = count + 1;
		uint32* currentPixval = buffer;
		while (--tempCount)
			{
			RGBA theRGBA = iSourcePixvalToRGBA.AsRGBA(*currentPixval);
			sRed(theRGBA) = 1 - sRed(theRGBA);
			sGreen(theRGBA) = 1 - sGreen(theRGBA);
			sBlue(theRGBA) = 1 - sBlue(theRGBA);
			*currentPixval++ = iDestRGBToPixval.AsPixval(theRGBA);
			}
		destAccessor.SetPixvals(iDestRowAddress, iDestH + hCurrent, count, buffer);
		hCurrent += count;
		}
	}

template <class S, class D>
static void sBlitWithMaps_T(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const S& iSourcePixvalToRGBA,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const D& iDestRGBToPixval,
	const RectPOD& iSourceBounds, PointPOD iDestLocation, bool iInvertColors)
	{
	int32 vCount = H(iSourceBounds);
	int32 hCount = W(iSourceBounds);

	if (iInvertColors)
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				sCalcRowAddress(iSourceRasterDesc, iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				sCalcRowAddress(iDestRasterDesc, iDestBase, iDestLocation.v + vCurrent);

			sBlitRowInvert_T(
				sourceRowAddress, iSourceRasterDesc.fPixvalDesc, iSourcePixvalToRGBA,
				iSourceBounds.left,
				destRowAddress, iDestRasterDesc.fPixvalDesc, iDestRGBToPixval,
				iDestLocation.h,
				hCount);
			}
		}
	else
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				sCalcRowAddress(iSourceRasterDesc, iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				sCalcRowAddress(iDestRasterDesc, iDestBase, iDestLocation.v + vCurrent);

			sBlitRow_T(
				sourceRowAddress, iSourceRasterDesc.fPixvalDesc, iSourcePixvalToRGBA,
				iSourceBounds.left,
				destRowAddress, iDestRasterDesc.fPixvalDesc, iDestRGBToPixval,
				iDestLocation.h,
				hCount);
			}
		}
	}

void sBlitPixvals(const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation)
	{
	int32 vCount = H(iSourceBounds);
	int32 hCount = W(iSourceBounds);

	if (iSourceRasterDesc.fPixvalDesc == iDestRasterDesc.fPixvalDesc
		&& (iSourceRasterDesc.fPixvalDesc.fDepth >= 8
			|| ((iSourceBounds.left | iDestLocation.h | hCount) & 0x07) == 0))
		{
		int32 hOffsetSource = iSourceBounds.left * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		int32 hOffsetDest = iDestLocation.h * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		int32 countToCopy = hCount * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const uint8* sourceRowAddress = static_cast<const uint8*>(
				sCalcRowAddress(iSourceRasterDesc, iSourceBase, iSourceBounds.top + vCurrent));
				+ hOffsetSource;

			uint8* destRowAddress = static_cast<uint8*>(
					sCalcRowAddress(iDestRasterDesc, iDestBase, iDestLocation.v + vCurrent))
					+ hOffsetDest;

			sMemCopy(destRowAddress, sourceRowAddress, countToCopy);
			}
		}
	else
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				sCalcRowAddress(iSourceRasterDesc, iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				sCalcRowAddress(iDestRasterDesc, iDestBase, iDestLocation.v + vCurrent);

			PixvalAccessor sourceAccessor(iSourceRasterDesc.fPixvalDesc);
			PixvalAccessor destAccessor(iDestRasterDesc.fPixvalDesc);

			int32 hCurrent = 0;
			while (hCurrent < hCount)
				{
				uint32 buffer[kBufSize];
				const size_t count = min(size_t(hCount - hCurrent), kBufSize);

				sourceAccessor.GetPixvals(sourceRowAddress,
					hCurrent + iSourceBounds.left, count, buffer);

				destAccessor.SetPixvals(destRowAddress, hCurrent + iDestLocation.h, count, buffer);
				hCurrent += count;
				}
			}
		}
	}

void sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDescReal,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDescReal,
	const RectPOD& iSourceBounds, PointPOD iDestLocation)
	{
	bool iInvertColors = false;

	ZRef<PixelDescRep> sourcePixelDescRep = iSourcePixelDescReal;
	ZRef<PixelDescRep> destPixelDescRep = iDestPixelDescReal;

	const PixelDescRep& iSourcePixelDesc = *sourcePixelDescRep;
	const PixelDescRep& iDestPixelDesc = *destPixelDescRep;

	if (PixelDescRep_Color* sourcePixelDescRep_Color =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			if (not iInvertColors && sourcePixelDescRep_Color->Matches(destPixelDescRep_Color))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Indexed* sourcePixelDescRep_Indexed =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			if (not iInvertColors && sourcePixelDescRep_Indexed->Matches(destPixelDescRep_Indexed))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Gray* sourcePixelDescRep_Gray =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			if (not iInvertColors && sourcePixelDescRep_Gray->Matches(destPixelDescRep_Gray))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	}

template <class S, class D>
static void sBlitRowWithMaps_T(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGBA,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 iCount, bool iInvertColors)
	{
	if (iInvertColors)
		{
		sBlitRowInvert_T(iSourceBase, iSourcePixvalDesc, iSourcePixvalToRGBA, iSourceH,
			iDestBase, iDestPixvalDesc, iDestRGBToPixval, iDestH,
			iCount);
		}
	else
		{
		sBlitRow_T(iSourceBase, iSourcePixvalDesc, iSourcePixvalToRGBA, iSourceH,
			iDestBase, iDestPixvalDesc, iDestRGBToPixval, iDestH,
			iCount);
		}
	}

void sBlitRowPixvals(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc,
	int32 iDestH,
	int32 iCount)
	{
	if (iSourcePixvalDesc.fDepth == iDestPixvalDesc.fDepth
		&& iSourcePixvalDesc.fBigEndian == iDestPixvalDesc.fBigEndian
		&& (iSourcePixvalDesc.fDepth >= 8
			|| ((iSourceH | iDestH | iCount) & 0x07) == 0))
		{
		int32 hOffsetSource = iSourceH * iSourcePixvalDesc.fDepth / 8;
		int32 hOffsetDest = iDestH * iDestPixvalDesc.fDepth / 8;
		int32 countToCopy = iCount * iSourcePixvalDesc.fDepth / 8;
		const uint8* sourceAddress = static_cast<const uint8*>(iSourceBase) + hOffsetSource;
		uint8* destAddress = static_cast<uint8*>(iDestBase) + hOffsetDest;
		sMemCopy(destAddress, sourceAddress, countToCopy);
		}
	else
		{
		PixvalAccessor sourceAccessor(iSourcePixvalDesc);
		PixvalAccessor destAccessor(iDestPixvalDesc);

		int32 hCurrent = 0;
		while (hCurrent < iCount)
			{
			uint32 buffer[kBufSize];
			const int32 count = min(int32(iCount - hCurrent), int32(kBufSize));
			sourceAccessor.GetPixvals(iSourceBase, iSourceH + hCurrent, count, buffer);
			destAccessor.SetPixvals(iDestBase, iDestH + hCurrent, count, buffer);
			hCurrent += count;
			}
		}
	}

void sBlitRow(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const PixelDesc& iSourcePixelDescReal,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const PixelDesc& iDestPixelDescReal,
	int32 iDestH,
	int32 iCount)
	{
	bool iInvertColors = false;

	ZRef<PixelDescRep> sourcePixelDescRep = iSourcePixelDescReal;
	ZRef<PixelDescRep> destPixelDescRep = iDestPixelDescReal;

	const PixelDescRep& iSourcePixelDesc = *sourcePixelDescRep;
	const PixelDescRep& iDestPixelDesc = *destPixelDescRep;

	if (PixelDescRep_Color* sourcePixelDescRep_Color =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			if (not iInvertColors && sourcePixelDescRep_Color->Matches(destPixelDescRep_Color))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc, iDestH,
					iCount);
				}
			else
				{
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Color, iDestH,
					iCount, iInvertColors);
				}
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else if (PixelDescRep_Indexed* sourcePixelDescRep_Indexed =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			if (not iInvertColors && sourcePixelDescRep_Indexed->Matches(destPixelDescRep_Indexed))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc,
					iDestH,
					iCount);
				}
			else
				{
				destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
					iDestH,
					iCount, iInvertColors);
				}
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else if (PixelDescRep_Gray* sourcePixelDescRep_Gray =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			if (not iInvertColors && sourcePixelDescRep_Gray->Matches(destPixelDescRep_Gray))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc,
					iDestH,
					iCount);
				}
			else
				{
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
					iDestH,
					iCount, iInvertColors);
				}
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	}

// =================================================================================================
#pragma mark - sBlit

void sBlit(const ZRef<PixmapRep>& iSource, const RectPOD& iSourceBounds,
	const ZRef<PixmapRep>& ioDest, PointPOD iDestLoc);

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc);

} // namespace Pixels
} // namespace ZooLib
