/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZDCPixmapBlit.h"

#if ZCONFIG(Compiler, CodeWarrior)
// Enforce higher level of optimization for this code.
#	pragma push
#	pragma optimization_level 4
#	pragma opt_common_subs on
#	pragma opt_lifetimes on
#	pragma peephole on
#	if ZCONFIG(Processor, x86)
#		pragma register_coloring on
#	endif
#endif

#include "zoolib/ZDCPixmapBlitPriv.h"

NAMESPACE_ZOOLIB_USING

using std::min;

typedef ZDCPixmapNS::PixelDescRep PDRep;
typedef ZDCPixmapNS::PixelDescRep_Indexed PDRep_Indexed;
typedef ZDCPixmapNS::PixelDescRep_Gray PDRep_Gray;
typedef ZDCPixmapNS::PixelDescRep_Color PDRep_Color;

// =================================================================================================
#pragma mark -
#pragma mark * Public API

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	ZPoint iSourceOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	EOp iOp)
	{
	sBlit_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
		iSourceOrigin,
		iDestAddress, iDestRD, iDestB, iDestPD,
		iOp);
	}

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	EOp iOp)
	{
	ZRect realDest = iDestB;
	realDest.right = min(realDest.right, ZCoord(realDest.left + iSourceB.Width()));
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + iSourceB.Height()));

	ZPoint sourceStart = iSourceB.TopLeft();
	sBlit_T(iSourceAddress, iSourceRD, iSourcePD,
		sourceStart,
		iDestAddress, iDestRD, realDest, iDestPD,
		iOp);

	}

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	ZPoint iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	ZPoint iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	sBlit_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
		iSourceOrigin,
		iMatteAddress, iMatteRD, iMatteB, iMattePD,
		iMatteOrigin,
		iDestAddress, iDestRD, iDestB, iDestPD,
		iSourcePremultiplied, iOp);
	}

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	ZPoint iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	ZRect realDest = iDestB;
	realDest.right = min(realDest.right, ZCoord(realDest.left + iMatteB.Width()));
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + iMatteB.Height()));

	ZPoint matteStart = iMatteB.TopLeft();

	sBlit_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
		iSourceOrigin,
		iMatteAddress, iMatteRD, iMattePD,
		matteStart,
		iDestAddress, iDestRD, realDest, iDestPD,
		iSourcePremultiplied, iOp);
	}

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	ZPoint iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	ZRect realDest = iDestB;
	realDest.right = min(realDest.right, ZCoord(realDest.left + iSourceB.Width()));
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + iSourceB.Height()));

	ZPoint sourceStart = iSourceB.TopLeft();

	ZRef<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* sourcePDRep_Color = ZRefDynamicCast<PDRep_Color>(sourcePDRep))
		{
		if (PDRep_Color* mattePDRep_Color = ZRefDynamicCast<PDRep_Color>(mattePDRep))
			{
			if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
				{
				sBlit_T(iSourceAddress, iSourceRD, *sourcePDRep_Color,
					sourceStart,
					iMatteAddress, iMatteRD, iMatteB, *mattePDRep_Color,
					iMatteOrigin,
					iDestAddress, iDestRD, realDest, *destPDRep_Color,
					iSourcePremultiplied, iOp);
				return;
				}
			}
		}

	sBlit_T(iSourceAddress, iSourceRD, iSourcePD,
		sourceStart,
		iMatteAddress, iMatteRD, iMatteB, iMattePD,
		iMatteOrigin,
		iDestAddress, iDestRD, realDest, iDestPD,
		iSourcePremultiplied, iOp);
	}

void ZDCPixmapBlit::sBlit(
	const void* iSourceAddress, const RD& iSourceRD, const ZRect& iSourceB, const PD& iSourcePD,
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	ZRect realDest = iDestB;
	int realWidth = min(iMatteB.Width(), iSourceB.Width());
	realDest.right = min(realDest.right, ZCoord(realDest.left + realWidth));

	int realHeight = min(iMatteB.Height(), iSourceB.Height());
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + realHeight));

	ZPoint sourceStart = iSourceB.TopLeft();
	ZPoint matteStart = iMatteB.TopLeft();

	ZRef<PDRep> sourcePDRep = iSourcePD.GetRep();
	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* sourcePDRep_Color = ZRefDynamicCast<PDRep_Color>(sourcePDRep))
		{
		if (PDRep_Color* mattePDRep_Color = ZRefDynamicCast<PDRep_Color>(mattePDRep))
			{
			if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
				{
				sBlit_T(iSourceAddress, iSourceRD, *sourcePDRep_Color,
					sourceStart,
					iMatteAddress, iMatteRD, *mattePDRep_Color,
					matteStart,
					iDestAddress, iDestRD, realDest, *destPDRep_Color,
					iSourcePremultiplied, iOp);
				return;
				}
			}
		}
	sBlit_T(iSourceAddress, iSourceRD, iSourcePD, sourceStart,
			iMatteAddress, iMatteRD, iMattePD, matteStart,
			iDestAddress, iDestRD, realDest, iDestPD,
			iSourcePremultiplied, iOp);
	}

void ZDCPixmapBlit::sColor(
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	const ZRGBColor& iColor,
	EOp iOp)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
		{
		sColor_T(iDestAddress, iDestRD, iDestB, *destPDRep_Color, iColor, iOp);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = ZRefDynamicCast<PDRep_Indexed>(destPDRep))
		{
		sColor_T(iDestAddress, iDestRD, iDestB, *destPDRep_Indexed, iColor, iOp);
		}
	else if (PDRep_Gray* destPDRep_Gray = ZRefDynamicCast<PDRep_Gray>(destPDRep))
		{
		sColor_T(iDestAddress, iDestRD, iDestB, *destPDRep_Gray, iColor, iOp);
		}
	else
		{
		sColor_T(iDestAddress, iDestRD, iDestB, iDestPD, iColor, iOp);
		}
	}

void ZDCPixmapBlit::sColor(
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	const ZRGBColor& iColor,
	EOp iOp)
	{
	ZRect realDest = iDestB;
	realDest.right = min(realDest.right, ZCoord(realDest.left + iMatteB.Width()));
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + iMatteB.Height()));

	ZPoint matteStart = iMatteB.TopLeft();

	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* mattePDRep_Color = ZRefDynamicCast<PDRep_Color>(mattePDRep))
		{
		if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
			{
			sColor_T(iMatteAddress, iMatteRD, *mattePDRep_Color,
				matteStart,
				iDestAddress, iDestRD, realDest, *destPDRep_Color,
				iColor,
				iOp);
			return;
			}
		}
	else if (PDRep_Gray* mattePDRep_Gray = ZRefDynamicCast<PDRep_Gray>(mattePDRep))
		{
		if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
			{
			sColor_T(iMatteAddress, iMatteRD, *mattePDRep_Gray,
				matteStart,
				iDestAddress, iDestRD, realDest, *destPDRep_Color,
				iColor,
				iOp);
			return;
			}
		}

	sColor_T(iMatteAddress, iMatteRD, iMattePD,
		matteStart,
		iDestAddress, iDestRD, realDest, iDestPD,
		iColor,
		iOp);
	}

void ZDCPixmapBlit::sColor(
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	ZPoint iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD,
	const ZRGBColor& iColor,
	EOp iOp)
	{
	sColorTile_T(iMatteAddress, iMatteRD, iMatteB, iMattePD,
		iMatteOrigin,
		iDestAddress, iDestRD, iDestB, iDestPD,
		iColor,
		iOp);
	}

void ZDCPixmapBlit::sInvert(
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
		{
		sInvert_T(iDestAddress, iDestRD, iDestB, *destPDRep_Color);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = ZRefDynamicCast<PDRep_Indexed>(destPDRep))
		{
		sInvert_T(iDestAddress, iDestRD, iDestB, *destPDRep_Indexed);
		}
	else if (PDRep_Gray* destPDRep_Gray = ZRefDynamicCast<PDRep_Gray>(destPDRep))
		{
		sInvert_T(iDestAddress, iDestRD, iDestB, *destPDRep_Gray);
		}
	else
		{
		sInvert_T(iDestAddress, iDestRD, iDestB, iDestPD);
		}
	}

void ZDCPixmapBlit::sOpaque(
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
		{
		sOpaque_T(iDestAddress, iDestRD, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = ZRefDynamicCast<PDRep_Indexed>(destPDRep))
		{
		sOpaque_T(iDestAddress, iDestRD, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = ZRefDynamicCast<PDRep_Gray>(destPDRep))
		{
		sOpaque_T(iDestAddress, iDestRD, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sOpaque_T(iDestAddress, iDestRD, iDestB, iDestPD, iAmount);
		}
	}

void ZDCPixmapBlit::sDarken(
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
		{
		sDarken_T(iDestAddress, iDestRD, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = ZRefDynamicCast<PDRep_Indexed>(destPDRep))
		{
		sDarken_T(iDestAddress, iDestRD, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = ZRefDynamicCast<PDRep_Gray>(destPDRep))
		{
		sDarken_T(iDestAddress, iDestRD, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sDarken_T(iDestAddress, iDestRD, iDestB, iDestPD, iAmount);
		}
	}

void ZDCPixmapBlit::sFade(
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD, uint16 iAmount)
	{
	ZRef<PDRep> destPDRep = iDestPD.GetRep();
	if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
		{
		sFade_T(iDestAddress, iDestRD, iDestB, *destPDRep_Color, iAmount);
		}
	else if (PDRep_Indexed* destPDRep_Indexed = ZRefDynamicCast<PDRep_Indexed>(destPDRep))
		{
		sFade_T(iDestAddress, iDestRD, iDestB, *destPDRep_Indexed, iAmount);
		}
	else if (PDRep_Gray* destPDRep_Gray = ZRefDynamicCast<PDRep_Gray>(destPDRep))
		{
		sFade_T(iDestAddress, iDestRD, iDestB, *destPDRep_Gray, iAmount);
		}
	else
		{
		sFade_T(iDestAddress, iDestRD, iDestB, iDestPD, iAmount);
		}
	}

void ZDCPixmapBlit::sApplyMatte(
	const void* iMatteAddress, const RD& iMatteRD, const ZRect& iMatteB, const PD& iMattePD,
	void* iDestAddress, const RD& iDestRD, const ZRect& iDestB, const PD& iDestPD)
	{
	ZAssertStop(1, iMattePD.HasAlpha());
	ZAssertStop(1, iDestPD.HasAlpha());
	ZRect realDest = iDestB;
	realDest.right = min(realDest.right, ZCoord(realDest.left + iMatteB.Width()));
	realDest.bottom = min(realDest.bottom, ZCoord(realDest.top + iMatteB.Height()));

	ZPoint matteStart = iMatteB.TopLeft();

	ZRef<PDRep> mattePDRep = iMattePD.GetRep();
	ZRef<PDRep> destPDRep = iDestPD.GetRep();

	if (PDRep_Color* mattePDRep_Color = ZRefDynamicCast<PDRep_Color>(mattePDRep))
		{
		if (PDRep_Color* destPDRep_Color = ZRefDynamicCast<PDRep_Color>(destPDRep))
			{
			sApplyMatte_T(iMatteAddress, iMatteRD, *mattePDRep_Color,
				matteStart,
				iDestAddress, iDestRD, realDest, *destPDRep_Color);
			return;
			}
		}

	sApplyMatte_T(iMatteAddress, iMatteRD, iMattePD,
		matteStart,
		iDestAddress, iDestRD, iDestB, iDestPD);
	}
