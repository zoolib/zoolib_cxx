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

#ifndef __ZDCPixmapBlitPriv__
#define __ZDCPixmapBlitPriv__ 1
#include "zconfig.h"

#include "zoolib/ZDCPixmapNS.h"
#include "zoolib/ZDCPixmapBlit.h" // For EOp and abbreviating typedefs

namespace ZooLib {
namespace ZDCPixmapBlit {

// =================================================================================================
#pragma mark -
#pragma mark * Using declarations

// Pull in PixelIter names, to reduce repetitive clutter.
using ZDCPixmapNS::PixelIterR_T;
using ZDCPixmapNS::PixelIterW_T;
using ZDCPixmapNS::PixelIterRW_T;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

// Returns a number between 0 and iRange - 1. The effect is as if enough multiples
// of iRange are added to or subtracted from iAmount to bring it into that range.
// Note that iRange should be positive.

inline int sPositiveModulus(int iAmount, int iRange)
	{
	return ((iAmount % iRange) + iRange) % iRange;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Porter-Duff composition functors

/*
Porter-Duff operators

f, b are rgb tuples
color is red, green or blue component, premultiplied with alpha
alpha is alpha component

f over b	f.color + (1 - f.alpha) * b.color
f in b		f.color * b.alpha
f out b		f.color * (1 - b.alpha)
f atop b	f.color * b.alpha + b.color * (1 - f.alpha)
f xor b		f.color.color * (1 - b.alpha) + b.color * (1 - f.alpha)
f plus b	f.color + b.color

*/

inline uint16 sMul(uint32 l, uint32 r)
	{
	uint32 temp = l * r + 32768;
	return (temp + (temp >> 16)) >> 16;
	}

inline void sMul(ZRGBA_POD& ioColor, uint32 r)
	{
	if (r == 0xFFFFU)
		{
		}
	else if (r == 0)
		{
		ioColor.red = 0;
		ioColor.green = 0;
		ioColor.blue = 0;
		ioColor.alpha = 0;
		}
	else
		{
		ioColor.red = sMul(ioColor.red, r);
		ioColor.green = sMul(ioColor.green, r);
		ioColor.blue = sMul(ioColor.blue, r);
		ioColor.alpha = sMul(ioColor.alpha, r);
		}
	}

inline uint16 sAdd(uint16 l, uint16 r)
	{
	return l + r;
	}

inline uint16 sAddSat(uint16 l, uint16 r)
	{
	uint32 result = l + r;
	if (result > 0xFFFFU)
		result = 0xFFFFU;
	return result;
	}

struct Compose_Over
	{
	void operator() (const ZRGBA_POD& f, ZRGBA_POD& b) const
		{
		if (f.alpha == 0)
			{
			// Don't touch b
			}
		else if (f.alpha == 0xFFFFU)
			{
			b.red = f.red;
			b.green = f.green;
			b.blue = f.blue;
			b.alpha = 0xFFFFU;
			}
		else
			{
			uint16 oneMinusAlpha = 0xFFFFU - f.alpha;
			if (b.red)
				b.red = sAdd(sMul(b.red, oneMinusAlpha), f.red);
			else
				b.red = f.red;

			if (b.green)
				b.green = sAdd(sMul(b.green, oneMinusAlpha), f.green);
			else
				b.green = f.green;

			if (b.blue)
				b.blue = sAdd(sMul(b.blue, oneMinusAlpha), f.blue);
			else
				b.blue = f.blue;

			if (b.alpha != 0xFFFF)
				{
				if (b.alpha)
					b.alpha = sAdd(sMul(b.alpha, oneMinusAlpha), f.alpha);
				else
					b.alpha = f.alpha;
				}
			}
		}
	};

struct Compose_Plus
	{
	void operator() (const ZRGBA_POD& f, ZRGBA_POD& b) const
		{
		// This will crap out with luminescent pixels.
		b.red = sAdd(b.red, f.red);
		b.green = sAdd(b.green, f.green);
		b.blue = sAdd(b.blue, f.blue);
		b.alpha = sAdd(b.alpha, f.alpha);

		// b += f;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * Tile variants

template <class S, class D>
void sTile_SD_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;
	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		while (--destCountH)
			{
			ZRGBA_POD thePixel;
			sourceIter.ReadInc(thePixel);
			destIter.WriteInc(thePixel);

			if (!--sourceCountH)
				{
				sourceIter.Reset(sourceRowAddress, sourceLeft);
				sourceCountH = sourceCountHReset;
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);
		}
	}

template <class S, class D, class O>
void sTile_SDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const O& iOp)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		while (--destCountH)
			{
			ZRGBA_POD thePixel;
			sourceIter.ReadInc(thePixel);

			ZRGBA_POD destPixel;
			destIter.Read(destPixel);

			iOp(thePixel, destPixel);

			destIter.WriteInc(destPixel);

			if (!--sourceCountH)
				{
				sourceIter.Reset(sourceRowAddress, sourceLeft);
				sourceCountH = sourceCountHReset;
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D>
void sTile_SMD_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;

	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;
		int matteCountH = matteCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);
				thePixel.alpha = matteIter.ReadAlphaInc();
				destIter.WriteInc(thePixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				destIter.WriteInc(thePixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sTile_SMDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, const O& iOp)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;

	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;
		int matteCountH = matteCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				thePixel.alpha = matteIter.ReadAlphaInc();

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Copy variants

template <class S, class D>
void sCopy_SD_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD thePixel;
			sourceIter.ReadInc(thePixel);
			destIter.WriteInc(thePixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v + row), iSourceStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class D, class O>
void sCopy_SDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const O& iOp)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD thePixel;
			sourceIter.ReadInc(thePixel);

			ZRGBA_POD destPixel;
			destIter.Read(destPixel);

			iOp(thePixel, destPixel);

			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v + row), iSourceStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class M, class D>
void sCopy_SMD_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);
				thePixel.alpha = matteIter.ReadAlphaInc();
				destIter.WriteInc(thePixel);
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				destIter.WriteInc(thePixel);
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v + row), iSourceStart.h);

		matteIter.Reset(
			iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sCopy_SMDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied,
	const O& iOp)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				thePixel.alpha = matteIter.ReadAlphaInc();

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();

				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			iSourceRD.CalcRowAddress(iSourceAddress, iSourceStart.v + row), iSourceStart.h);

		matteIter.Reset(
			iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Tile source, untiled matte

template <class S, class M, class D>
void sTileSource_SMD_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	int matteV = iMatteStart.v;
	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, matteV),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);
				thePixel.alpha = matteIter.ReadAlphaInc();
				destIter.WriteInc(thePixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				destIter.WriteInc(thePixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

 		++matteV;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteIter.Reset(iMatteRD.CalcRowAddress(iMatteAddress, matteV), matteV);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sTileSource_SMDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied,
	const O& iOp)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = iSourceB.Width();
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = iSourceB.Height();
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
	PixelIterR_T<S> sourceIter(
		sourceRowAddress,
		iSourceRD.fPixvalDesc,
		sourceHStart,
		iSourcePD);

	int matteV = iMatteStart.v;
	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, matteV),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				thePixel.alpha = matteIter.ReadAlphaInc();

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--sourceCountH)
					{
					sourceIter.Reset(sourceRowAddress, sourceLeft);
					sourceCountH = sourceCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

 		++matteV;

		sourceRowAddress = iSourceRD.CalcRowAddress(iSourceAddress, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteIter.Reset(iMatteRD.CalcRowAddress(iMatteAddress, matteV), matteV);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Tile matte, untiled source

template <class S, class M, class D>
void sTileMatte_SMD_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int sourceV = iSourceStart.v;
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, sourceV),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	int matteV = matteVStart;
	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);
				thePixel.alpha = matteIter.ReadAlphaInc();
				destIter.WriteInc(thePixel);

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				destIter.WriteInc(thePixel);

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		sourceIter.Reset(iSourceRD.CalcRowAddress(iSourceAddress, sourceV), iSourceStart.h);

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

// Trinary with operation
template <class S, class M, class D, class O>
void sTileMatte_SMDO_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, const O& iOp)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int sourceV = iSourceStart.v;
	PixelIterR_T<S> sourceIter(
		iSourceRD.CalcRowAddress(iSourceAddress, sourceV),
		iSourceRD.fPixvalDesc,
		iSourceStart.h,
		iSourcePD);

	int matteV = matteVStart;
	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				thePixel.alpha = matteIter.ReadAlphaInc();

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}
		else
			{
			while (--destCountH)
				{
				ZRGBA_POD thePixel;
				sourceIter.ReadInc(thePixel);

				uint16 alpha = matteIter.ReadAlphaInc();
				thePixel.red = sMul(thePixel.red, alpha);
				thePixel.green = sMul(thePixel.green, alpha);
				thePixel.blue = sMul(thePixel.blue, alpha);
				thePixel.alpha = alpha;

				ZRGBA_POD destPixel;
				destIter.Read(destPixel);

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);

				if (!--matteCountH)
					{
					matteIter.Reset(matteRowAddress, matteLeft);
					matteCountH = matteCountHReset;
					}
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++sourceV;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		sourceIter.Reset(iSourceRD.CalcRowAddress(iSourceAddress, sourceV), iSourceStart.h);

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Fill/color variants

static void sFillPixval(void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB,
			uint32 iPixval)
	{
	ZDCPixmapNS::PixvalIterW destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			destIter.WriteInc(iPixval);

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class D, class O>
void sColor_DO_T(
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor,
	const O& iOp)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD destPixel;
			destIter.Read(destPixel);
			iOp(iColor, destPixel);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D>
void sColor_MD_T(
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				ZRGBA_POD tempColor = iColor;
				sMul(tempColor, alpha);
				destIter.WriteInc(tempColor);
				}
			else
				{
				destIter.Inc();
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		matteIter.Reset(
			iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D, class O>
void sColor_MDO_T(
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor,
	const O& iOp)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				ZRGBA_POD destPixel;
				destIter.Read(destPixel);
				ZRGBA_POD tempColor = iColor;
				sMul(tempColor, alpha);
				iOp(tempColor, destPixel);
				destIter.WriteInc(destPixel);
				}
			else
				{
				destIter.Inc();
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		matteIter.Reset(
			iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D>
void sColorTile_MD_T(
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;
	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	ZRGBA_POD theColor = iColor;
	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				ZRGBA_POD tempColor = iColor;
				sMul(tempColor, alpha);
				destIter.WriteInc(tempColor);
				}
			else
				{
				destIter.Inc();
				}

			if (!--matteCountH)
				{
				matteIter.Reset(matteRowAddress, matteLeft);
				matteCountH = matteCountHReset;
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

template <class M, class D, class O>
void sColorTile_MDO_T(
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor, const O& iOp)
	{
	int destWidth = iDestB.Width();
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = iMatteB.Width();
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = iMatteB.Height();
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;
	const void* matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
	PixelIterR_T<M> matteIter(
		matteRowAddress,
		iMatteRD.fPixvalDesc,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, currentDestV),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	ZRGBA_POD theColor = iColor;
	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				ZRGBA_POD tempColor = iColor;
				sMul(tempColor, alpha);
				ZRGBA_POD destPixel;
				destIter.Read(destPixel);
				iOp(tempColor, destPixel);
				destIter.WriteInc(destPixel);
				}
			else
				{
				destIter.Inc();
				}

			if (!--matteCountH)
				{
				matteIter.Reset(matteRowAddress, matteLeft);
				matteCountH = matteCountHReset;
				}
			}

 		++currentDestV;
		if (currentDestV >= destBottom)
			break;

 		++matteV;
		if (matteV >= matteBottom)
			matteV = matteTop;

		matteRowAddress = iMatteRD.CalcRowAddress(iMatteAddress, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Blit dispatchers

// Replicate iSourceB over iDestB, aligning iSourceOrigin with iDestB.TopLeft()
template <class S, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sTile_SD_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD);
			break;
			}
		case eOp_Over:
			{
			sTile_SDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTile_SDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				Compose_Plus());
			break;
			}
		}
	}

// Copy source to iDestB without replication. Actual drawn rectangle will be smaller
// than iDestB if iSourceB is smaller.
template <class S, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sCopy_SD_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iDestAddress, iDestRD, iDestB, iDestPD);
			break;
			}
		case eOp_Over:
			{
			sCopy_SDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sCopy_SDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				Compose_Plus());
			break;
			}
		}
	}

// Replicate iSourceB masked by replicated iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sTile_SMD_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTile_SMDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTile_SMDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Replicate iSourceB masked by iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const ZRectPOD& iSourceB, const S& iSourcePD,
	ZPointPOD iSourceOrigin,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sTileSource_SMD_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTileSource_SMDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTileSource_SMDO_T(iSourceAddress, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Draw iSourceB masked by replicated iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sTileMatte_SMD_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTileMatte_SMDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTileMatte_SMDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Draw iSourceB masked by iMatteB into iDestB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSourceAddress, const RD& iSourceRD, const S& iSourcePD,
	ZPointPOD iSourceStart,
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sCopy_SMD_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sCopy_SMDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sCopy_SMDO_T(iSourceAddress, iSourceRD, iSourcePD,
				iSourceStart,
				iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestB with iColor
template <class D>
void sColor_T(
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sFillPixval(iDestAddress, iDestRD, iDestB, iDestPD.AsPixval(iColor));
			break;
			}
		case eOp_Over:
			{
			sColor_DO_T(iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColor_DO_T(iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestB matted
template <class M, class D>
void sColor_T(
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sColor_MD_T(iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor);
			break;
			}
		case eOp_Over:
			{
			sColor_MDO_T(iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColor_MDO_T(iMatteAddress, iMatteRD, iMattePD,
				iMatteStart,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestbounds with tiled matte
template <class M, class D>
void sColorTile_T(
	const void* iMatteAddress, const RD& iMatteRD, const ZRectPOD& iMatteB, const M& iMattePD,
	ZPointPOD iMatteOrigin,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD,
	const ZRGBA_POD& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		case eOp_Copy:
			{
			sColorTile_MD_T(iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor);
			break;
			}
		case eOp_Over:
			{
			sColorTile_MDO_T(iMatteAddress, iMatteRD, iMatteB, iMattePD,
			iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColorTile_MDO_T(iMatteAddress, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				iDestAddress, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Invert iDestbounds
template <class D>
void sInvert_T(void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD destPixel;
			destIter.Read(destPixel);
			destPixel.red = destPixel.alpha - destPixel.red;
			destPixel.green = destPixel.alpha - destPixel.green;
			destPixel.blue = destPixel.alpha - destPixel.blue;
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

// Multiply r,g, b & alpha by iAmount/65536
template <class D>
void sOpaque_T(
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD destPixel;
			destIter.Read(destPixel);
			destPixel.red = sMul(destPixel.red, iAmount);
			destPixel.green = sMul(destPixel.green, iAmount);
			destPixel.blue = sMul(destPixel.blue, iAmount);
			destPixel.alpha = sMul(destPixel.alpha, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

// Multiply r, g, b by iAmount/65536, leaving alpha alone.
template <class D>
void sDarken_T(
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD destPixel;
			destIter.Read(destPixel);
			destPixel.red = sMul(destPixel.red, iAmount);
			destPixel.green = sMul(destPixel.green, iAmount);
			destPixel.blue = sMul(destPixel.blue, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

// Multiply alpha by iAmount/65536, leaving r,g,b alone
template <class D>
void sFade_T(
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD destPixel;
			destIter.Read(destPixel);
			destPixel.alpha = sMul(destPixel.alpha, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

// Apply the alpha values in matte to dest
template <class M, class D>
void sApplyMatte_T(
	const void* iMatteAddress, const RD& iMatteRD, const M& iMattePD,
	ZPointPOD iMatteStart,
	void* iDestAddress, const RD& iDestRD, const ZRectPOD& iDestB, const D& iDestPD)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v),
		iMatteRD.fPixvalDesc,
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top),
		iDestRD.fPixvalDesc,
		iDestB.left,
		iDestPD);

	int destWidth = iDestB.Width();
	int destHeight = iDestB.Height();
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			ZRGBA_POD thePixel;
			destIter.Read(thePixel);

			uint16 alpha = matteIter.ReadAlphaInc();
			thePixel.red = sMul(thePixel.red, alpha);
			thePixel.green = sMul(thePixel.green, alpha);
			thePixel.blue = sMul(thePixel.blue, alpha);
			thePixel.alpha = alpha;

			destIter.WriteInc(thePixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		matteIter.Reset(
			iMatteRD.CalcRowAddress(iMatteAddress, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			iDestRD.CalcRowAddressDest(iDestAddress, iDestB.top + row), iDestB.left);
		}
	}

} // namespace ZDCPixmapBlit
} // namespace ZooLib

#endif // __ZDCPixmapBlitPriv__
