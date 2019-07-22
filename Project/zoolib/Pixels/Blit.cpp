// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

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

// =================================================================================================
#pragma mark -

typedef PixelDescRep PDRep;
typedef PixelDescRep_Indexed PDRep_Indexed;
typedef PixelDescRep_Gray PDRep_Gray;
typedef PixelDescRep_Color PDRep_Color;

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
	const ZP<PDRep> thePDRep = iPixelDesc.GetRep();
	if (PDRep_Color* thePDRep_Color =
		thePDRep.DynamicCast<PDRep_Color>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePDRep_Color, *thePDRep_Color, iBounds, iMungeProc, iRefcon);
		}
	else if (PDRep_Indexed* thePDRep_Indexed =
		thePDRep.DynamicCast<PDRep_Indexed>())
		{
		thePDRep_Indexed->BuildReverseLookupIfNeeded();
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePDRep_Indexed, *thePDRep_Indexed, iBounds, iMungeProc, iRefcon);
		}
	else if (PDRep_Gray* thePDRep_Gray =
		thePDRep.DynamicCast<PDRep_Gray>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePDRep_Gray, *thePDRep_Gray, iBounds, iMungeProc, iRefcon);
		}
	else
		{
		sMunge_T(iBaseAddress, iRasterDesc, iPixelDesc, iPixelDesc, iBounds, iMungeProc, iRefcon);
		}
	}

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

	ZP<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZP<PDRep> mattePDRep = iMattePD.GetRep();
	ZP<PDRep> destPDRep = iDestPD.GetRep();

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

	ZP<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZP<PDRep> mattePDRep = iMattePD.GetRep();
	ZP<PDRep> destPDRep = iDestPD.GetRep();

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
	ZP<PDRep> destPDRep = iDestPD.GetRep();
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

	ZP<PDRep> mattePDRep = iMattePD.GetRep();
	ZP<PDRep> destPDRep = iDestPD.GetRep();

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

// =================================================================================================
#pragma mark -

void sInvert(
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD)
	{
	ZP<PDRep> destPDRep = iDestPD.GetRep();
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
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount)
	{
	ZP<PDRep> destPDRep = iDestPD.GetRep();
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
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount)
	{
	ZP<PDRep> destPDRep = iDestPD.GetRep();
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
	const RD& iDestRD, void* oDest, const RectPOD& iDestB, const PD& iDestPD, Comp iAmount)
	{
	ZP<PDRep> destPDRep = iDestPD.GetRep();
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

	ZP<PDRep> mattePDRep = iMattePD.GetRep();
	ZP<PDRep> destPDRep = iDestPD.GetRep();

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
