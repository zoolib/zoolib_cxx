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
#include "zoolib/Pixels/BlitPriv.h"
#include "zoolib/Pixels/Cartesian_Geom.h"
#include "zoolib/Pixels/PixvalAccessor.h"
#include "zoolib/Pixels/RasterOperations.h"

#include "zoolib/ByteSwap.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

using std::min;
using std::vector;

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - sMunge

template <class S, class D>
static void sMungeRow_T(
	void* iRowAddress, const PixvalDesc& iPixvalDesc,
	const S& iMapPixvalToRGB, const D& iMapRGBToPixval,
	Ord inStartH, int inCount, Ord inCoordV,
	MungeProc iMungeProc, void* iRefcon)
	{
	PixvalAccessor sourceAccessor(iPixvalDesc);
	int hCurrent = 0;
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
	int hSize = W(iBounds);
	int vSize = H(iBounds);

	for (int vCurrent = 0; vCurrent < vSize; ++vCurrent)
		{
		void* rowAddress = sCalcRowAddress(iRasterDesc, iBaseAddress, iBounds.top + vCurrent);
		sMungeRow_T(rowAddress, iRasterDesc.fPixvalDesc,
				iMapPixvalToRGB, iMapRGBToPixval,
				iBounds.left, hSize, vCurrent,
				iMungeProc, iRefcon);
		}
	}

void sMunge(
	void* iBaseAddress, const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc,
	const RectPOD& iBounds, MungeProc iMungeProc, void* iRefcon)
	{
	const ZRef<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();
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
	int iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int iDestH,
	int inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int hCurrent = 0;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int tempCount = count + 1;
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
	int iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int iDestH,
	int inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int hCurrent = 0;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int tempCount = count + 1;
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
	const RasterDesc& iSourceRasterDesc, const void* iSourceBase, const S& iSourcePixvalToRGBA,
	const RasterDesc& iDestRasterDesc, void* iDestBase, const D& iDestRGBToPixval,
	const RectPOD& iSourceBounds, PointPOD iDestLocation, bool iInvertColors)
	{
	int vCount = H(iSourceBounds);
	int hCount = W(iSourceBounds);

	if (iInvertColors)
		{
		for (int vCurrent = 0; vCurrent < vCount; ++vCurrent)
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
		for (int vCurrent = 0; vCurrent < vCount; ++vCurrent)
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

void sBlit(
	const RasterDesc& iSourceRasterDesc, const void* iSourceBase, const PixelDesc& iSourcePixelDesc,
	const RasterDesc& iDestRasterDesc, void* iDestBase, const PixelDesc& iDestPixelDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation)
	{
	bool iInvertColors = false;

	ZRef<PixelDescRep> sourcePixelDescRep = iSourcePixelDesc.GetRep();
	ZRef<PixelDescRep> destPixelDescRep = iDestPixelDesc.GetRep();

	if (PixelDescRep_Color* sourcePixelDescRep_Color =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			if (not iInvertColors && sourcePixelDescRep_Color->Matches(destPixelDescRep_Color))
				{
				sBlitPixvals(iSourceRasterDesc, iSourceBase, iSourceBounds,
					iDestRasterDesc, iDestBase, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Color,
					iDestRasterDesc, iDestBase, *destPixelDescRep_Color,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Color,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Color,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Color,
				iDestRasterDesc, iDestBase, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Indexed* sourcePixelDescRep_Indexed =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Indexed,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			if (not iInvertColors && sourcePixelDescRep_Indexed->Matches(destPixelDescRep_Indexed))
				{
				sBlitPixvals(iSourceRasterDesc, iSourceBase, iSourceBounds,
					iDestRasterDesc, iDestBase, iDestLocation);
				}
			else
				{
				destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
				sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Indexed,
					iDestRasterDesc, iDestBase, *destPixelDescRep_Indexed,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Indexed,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Indexed,
				iDestRasterDesc, iDestBase, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Gray* sourcePixelDescRep_Gray =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Gray,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Gray,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			if (not iInvertColors && sourcePixelDescRep_Gray->Matches(destPixelDescRep_Gray))
				{
				sBlitPixvals(iSourceRasterDesc, iSourceBase, iSourceBounds,
					iDestRasterDesc, iDestBase, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Gray,
					iDestRasterDesc, iDestBase, *destPixelDescRep_Gray,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, *sourcePixelDescRep_Gray,
				iDestRasterDesc, iDestBase, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, iSourcePixelDesc,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, iSourcePixelDesc,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, iSourcePixelDesc,
				iDestRasterDesc, iDestBase, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceRasterDesc, iSourceBase, iSourcePixelDesc,
				iDestRasterDesc, iDestBase, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	}

template <class S, class D>
static void sBlitRowWithMaps_T(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGBA,
	int iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int iDestH,
	int iCount, bool iInvertColors)
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

// =================================================================================================
#pragma mark - sBlit

typedef PixelDescRep PDRep;
typedef PixelDescRep_Indexed PDRep_Indexed;
typedef PixelDescRep_Gray PDRep_Gray;
typedef PixelDescRep_Color PDRep_Color;

// =================================================================================================
// MARK: - Public API

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOp)
	{
	sBlit_T(iSourceRD, iSource, iSourceB, iSourcePD,
		iSourceOrigin,
		iDestRD, oDest, iDestB, iDestPD,
		iOp);
	}

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	EOp iOp)
	{
	RectPOD realDest = iDestB;
	realDest.right = min(realDest.right, Ord(realDest.left + W(iSourceB)));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + H(iSourceB)));

	PointPOD sourceStart = LT(iSourceB);
	sBlit_T(iSourceRD, iSource, iSourcePD,
		sourceStart,
		iDestRD, oDest, realDest, iDestPD,
		iOp);

	}

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	sBlit_T(iSourceRD, iSource, iSourceB, iSourcePD,
		iSourceOrigin,
		iMatteRD, iMatte, iMatteB, iMattePD,
		iMatteOrigin,
		iDestRD, oDest, iDestB, iDestPD,
		iSourcePremultiplied, iOp);
	}

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	PointPOD iSourceOrigin,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	RectPOD realDest = iDestB;
	realDest.right = min(realDest.right, Ord(realDest.left + W(iMatteB)));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + H(iMatteB)));

	PointPOD matteStart = LT(iMatteB);

	sBlit_T(iSourceRD, iSource, iSourceB, iSourcePD,
		iSourceOrigin,
		iMatteRD, iMatte, iMattePD,
		matteStart,
		iDestRD, oDest, realDest, iDestPD,
		iSourcePremultiplied, iOp);
	}

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	RectPOD realDest = iDestB;
	realDest.right = min(realDest.right, Ord(realDest.left + W(iSourceB)));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + H(iSourceB)));

	PointPOD sourceStart = LT(iSourceB);

	ZRef<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* sourcePDRep_Color = sourcePDRep.DynamicCast<PDRep_Color>())
		{
		if (PDRep_Color* mattePDRep_Color = mattePDRep.DynamicCast<PDRep_Color>())
			{
			if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
				{
				sBlit_T(iSourceRD, iSource, *sourcePDRep_Color,
					sourceStart,
					iMatteRD, iMatte, iMatteB, *mattePDRep_Color,
					iMatteOrigin,
					iDestRD, oDest, realDest, *destPDRep_Color,
					iSourcePremultiplied, iOp);
				return;
				}
			}
		}

	sBlit_T(iSourceRD, iSource, iSourcePD,
		sourceStart,
		iMatteRD, iMatte, iMatteB, iMattePD,
		iMatteOrigin,
		iDestRD, oDest, realDest, iDestPD,
		iSourcePremultiplied, iOp);
	}

void sBlit(
	const RD& iSourceRD, const void* iSource, const RectPOD& iSourceB, const PD& iSourcePD,
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	RectPOD realDest = iDestB;
	int realWidth = min(W(iMatteB), W(iSourceB));
	realDest.right = min(realDest.right, Ord(realDest.left + realWidth));

	int realHeight = min(H(iMatteB), H(iSourceB));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + realHeight));

	PointPOD sourceStart = LT(iSourceB);
	PointPOD matteStart = LT(iMatteB);

	ZRef<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* sourcePDRep_Color = sourcePDRep.DynamicCast<PDRep_Color>())
		{
		if (PDRep_Color* mattePDRep_Color = mattePDRep.DynamicCast<PDRep_Color>())
			{
			if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
				{
				sBlit_T(iSourceRD, iSource, *sourcePDRep_Color,
					sourceStart,
					iMatteRD, iMatte, *mattePDRep_Color,
					matteStart,
					iDestRD, oDest, realDest, *destPDRep_Color,
					iSourcePremultiplied, iOp);
				return;
				}
			}
		}
	sBlit_T(iSourceRD, iSource, iSourcePD, sourceStart,
			iMatteRD, iMatte, iMattePD, matteStart,
			iDestRD, oDest, realDest, iDestPD,
			iSourcePremultiplied, iOp);
	}

void sColor(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
		{
		sColor_T(iDestRD, oDest, iDestB, *destPDRep_Color, iColor, iOp);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = destPDRep.DynamicCast<PDRep_Indexed>())
		{
		sColor_T(iDestRD, oDest, iDestB, *destPDRep_Indexed, iColor, iOp);
		}
	else if (PDRep_Gray* destPDRep_Gray = destPDRep.DynamicCast<PDRep_Gray>())
		{
		sColor_T(iDestRD, oDest, iDestB, *destPDRep_Gray, iColor, iOp);
		}
	else
		{
		sColor_T(iDestRD, oDest, iDestB, iDestPD, iColor, iOp);
		}
	}

void sColor(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	RectPOD realDest = iDestB;
	realDest.right = min(realDest.right, Ord(realDest.left + W(iMatteB)));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + H(iMatteB)));

	PointPOD matteStart = LT(iMatteB);

	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* mattePDRep_Color = mattePDRep.DynamicCast<PDRep_Color>())
		{
		if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
			{
			sColor_T(iMatteRD, iMatte, *mattePDRep_Color,
				matteStart,
				iDestRD, oDest, realDest, *destPDRep_Color,
				iColor,
				iOp);
			return;
			}
		}
	else if (PDRep_Gray* mattePDRep_Gray = mattePDRep.DynamicCast<PDRep_Gray>())
		{
		if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
			{
			sColor_T(iMatteRD, iMatte, *mattePDRep_Gray,
				matteStart,
				iDestRD, oDest, realDest, *destPDRep_Color,
				iColor,
				iOp);
			return;
			}
		}

	sColor_T(iMatteRD, iMatte, iMattePD,
		matteStart,
		iDestRD, oDest, realDest, iDestPD,
		iColor,
		iOp);
	}

void sColor(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	PointPOD iMatteOrigin,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	sColorTile_T(iMatteRD, iMatte, iMatteB, iMattePD,
		iMatteOrigin,
		iDestRD, oDest, iDestB, iDestPD,
		iColor,
		iOp);
	}

void sInvert(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
		{
		sInvert_T(iDestRD, oDest, iDestB, *destPDRep_Color);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = destPDRep.DynamicCast<PDRep_Indexed>())
		{
		sInvert_T(iDestRD, oDest, iDestB, *destPDRep_Indexed);
		}
	else if (PDRep_Gray* destPDRep_Gray = destPDRep.DynamicCast<PDRep_Gray>())
		{
		sInvert_T(iDestRD, oDest, iDestB, *destPDRep_Gray);
		}
	else
		{
		sInvert_T(iDestRD, oDest, iDestB, iDestPD);
		}
	}

void sOpaque(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
		{
		sOpaque_T(iDestRD, oDest, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = destPDRep.DynamicCast<PDRep_Indexed>())
		{
		sOpaque_T(iDestRD, oDest, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = destPDRep.DynamicCast<PDRep_Gray>())
		{
		sOpaque_T(iDestRD, oDest, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sOpaque_T(iDestRD, oDest, iDestB, iDestPD, iAmount);
		}
	}

void sDarken(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
		{
		sDarken_T(iDestRD, oDest, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = destPDRep.DynamicCast<PDRep_Indexed>())
		{
		sDarken_T(iDestRD, oDest, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = destPDRep.DynamicCast<PDRep_Gray>())
		{
		sDarken_T(iDestRD, oDest, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sDarken_T(iDestRD, oDest, iDestB, iDestPD, iAmount);
		}
	}

void sFade(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
		{
		sFade_T(iDestRD, oDest, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = destPDRep.DynamicCast<PDRep_Indexed>())
		{
		sFade_T(iDestRD, oDest, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = destPDRep.DynamicCast<PDRep_Gray>())
		{
		sFade_T(iDestRD, oDest, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sFade_T(iDestRD, oDest, iDestB, iDestPD, iAmount);
		}
	}

void sApplyMatte(
	const RD& iMatteRD, const void* iMatte, const RectPOD& iMatteB, const PD& iMattePD,
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD)
	{
	ZAssertStop(1, iMattePD.HasAlpha());
	ZAssertStop(1, iDestPD.HasAlpha());
	RectPOD realDest = iDestB;
	realDest.right = min(realDest.right, Ord(realDest.left + W(iMatteB)));
	realDest.bottom = min(realDest.bottom, Ord(realDest.top + H(iMatteB)));

	PointPOD matteStart = LT(iMatteB);

	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* mattePDRep_Color = mattePDRep.DynamicCast<PDRep_Color>())
		{
		if (PDRep_Color* destPDRep_Color = destPDRep.DynamicCast<PDRep_Color>())
			{
			sApplyMatte_T(iMatteRD, iMatte, *mattePDRep_Color,
				matteStart,
				iDestRD, oDest, realDest, *destPDRep_Color);
			return;
			}
		}

	sApplyMatte_T(iMatteRD, iMatte, iMattePD,
		matteStart,
		iDestRD, oDest, iDestB, iDestPD);
	}

} // namespace Pixels
} // namespace ZooLib
