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

#ifndef __ZDCPixmapBlitPriv_h__
#define __ZDCPixmapBlitPriv_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/ZDCPixmapBlit.h" // For EOp and abbreviating typedefs
#include "zoolib/Pixels/PixelIters.h"
#include "zoolib/Pixels/Cartesian_Geom.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
// MARK: - Helpers

// Returns a number between 0 and iRange - 1. The effect is as if enough multiples
// of iRange are added to or subtracted from iAmount to bring it into that range.
// Note that iRange should be positive.

inline int sPositiveModulus(int iAmount, int iRange)
	{
	return ((iAmount % iRange) + iRange) % iRange;
	}

// =================================================================================================
// MARK: - Porter-Duff composition functors

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

inline void sMul(RGBA& ioColor, uint32 r);
//	{
//	if (r == 0xFFFFU)
//		{
//		}
//	else if (r == 0)
//		{
//		ioColor.red = 0;
//		ioColor.green = 0;
//		ioColor.blue = 0;
//		ioColor.alpha = 0;
//		}
//	else
//		{
//		ioColor.red = sMul(ioColor.red, r);
//		ioColor.green = sMul(ioColor.green, r);
//		ioColor.blue = sMul(ioColor.blue, r);
//		ioColor.alpha = sMul(ioColor.alpha, r);
//		}
//	}

inline uint16 sAdd(uint16 l, uint16 r)
	{
	return l + r;
	}

// u32b sat_addu32b(u32b x, u32b y)
// {
// 	u32b res = x + y;
// 	res |= -(res < x);
// 	
// 	return res;
// }

//inline uint16 sAddSat(uint16 l, uint16 r)
//	{
//	uint32 result = l + r;
//	if (result > 0xFFFFU)
//		result = 0xFFFFU;
//	return result;
//	}

struct Compose_Over
	{
	void operator() (const RGBA& f, RGBA& b) const;
//		{
//		if (f.alpha == 0)
//			{
//			// Don't touch b
//			}
//		else if (f.alpha == 0xFFFFU)
//			{
//			b.red = f.red;
//			b.green = f.green;
//			b.blue = f.blue;
//			b.alpha = 0xFFFFU;
//			}
//		else
//			{
//			uint16 oneMinusAlpha = 0xFFFFU - f.alpha;
//			if (b.red)
//				b.red = sAdd(sMul(b.red, oneMinusAlpha), f.red);
//			else
//				b.red = f.red;
//
//			if (b.green)
//				b.green = sAdd(sMul(b.green, oneMinusAlpha), f.green);
//			else
//				b.green = f.green;
//
//			if (b.blue)
//				b.blue = sAdd(sMul(b.blue, oneMinusAlpha), f.blue);
//			else
//				b.blue = f.blue;
//
//			if (b.alpha != 0xFFFF)
//				{
//				if (b.alpha)
//					b.alpha = sAdd(sMul(b.alpha, oneMinusAlpha), f.alpha);
//				else
//					b.alpha = f.alpha;
//				}
//			}
//		}
	};

struct Compose_Plus
	{
	void operator() (const RGBA& f, RGBA& b) const;
//		{
//		// This will crap out with luminescent pixels.
//		b.red = sAdd(b.red, f.red);
//		b.green = sAdd(b.green, f.green);
//		b.blue = sAdd(b.blue, f.blue);
//		b.alpha = sAdd(b.alpha, f.alpha);
//
//		// b += f;
//		}
	};

// =================================================================================================
// MARK: - Tile variants

template <class S, class D>
void sTile_SD_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;
	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		while (--destCountH)
			{
			RGBA thePixel = sourceIter.ReadInc();
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

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);

 		++sourceV;
		if (sourceV >= sourceBottom)
			sourceV = sourceTop;

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);
		}
	}

template <class S, class D, class O>
void sTile_SDO_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const O& iOp)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int sourceCountH = sourceCountHStart;

		while (--destCountH)
			{
			RGBA thePixel = sourceIter.ReadInc();

			RGBA destPixel = destIter.Read();

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

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D>
void sTile_SMD_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;

	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();
				sAlpha(thePixel) = matteIter.ReadAlphaInc();
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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

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

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sTile_SMDO_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, const O& iOp)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;

	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();

				sAlpha(thePixel) = matteIter.ReadAlphaInc();

				RGBA destPixel = destIter.Read();

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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

				RGBA destPixel = destIter.Read();

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

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
// MARK: - Copy variants

template <class S, class D>
void sCopy_SD_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, iSourceStart.v),
		iSourceStart.h,
		iSourcePD);

	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA thePixel = sourceIter.ReadInc();
			destIter.WriteInc(thePixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			sCalcRowAddress(iSourceRD, iSource, iSourceStart.v + row), iSourceStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class D, class O>
void sCopy_SDO_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const O& iOp)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, iSourceStart.v),
		iSourceStart.h,
		iSourcePD);

	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA thePixel = sourceIter.ReadInc();

			RGBA destPixel = destIter.Read();

			iOp(thePixel, destPixel);

			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			sCalcRowAddress(iSourceRD, iSource, iSourceStart.v + row), iSourceStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class M, class D>
void sCopy_SMD_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, iSourceStart.v),
		iSourceStart.h,
		iSourcePD);

	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v),
		iMatteStart.h,
		iMattePD);

	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				RGBA thePixel = sourceIter.ReadInc();
				sAlpha(thePixel) = matteIter.ReadAlphaInc();
				destIter.WriteInc(thePixel);
				}
			}
		else
			{
			while (--destCountH)
				{
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

				destIter.WriteInc(thePixel);
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			sCalcRowAddress(iSourceRD, iSource, iSourceStart.v + row), iSourceStart.h);

		matteIter.Reset(
			sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sCopy_SMDO_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied,
	const O& iOp)
	{
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, iSourceStart.v),
		iSourceStart.h,
		iSourcePD);

	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v),
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		if (iSourcePremultiplied)
			{
			while (--destCountH)
				{
				RGBA thePixel = sourceIter.ReadInc();

				sAlpha(thePixel) = matteIter.ReadAlphaInc();

				RGBA destPixel = destIter.Read();

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);
				}
			}
		else
			{
			while (--destCountH)
				{
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

				RGBA destPixel = destIter.Read();

				iOp(thePixel, destPixel);

				destIter.WriteInc(destPixel);
				}
			}

 		++row;
		if (row >= destHeight)
			break;

		sourceIter.Reset(
			sCalcRowAddress(iSourceRD, iSource, iSourceStart.v + row), iSourceStart.h);

		matteIter.Reset(
			sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

// =================================================================================================
// MARK: - Tile source, untiled matte

template <class S, class M, class D>
void sTileSource_SMD_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	int matteV = iMatteStart.v;
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, matteV),
		iMatteStart.h,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();
				sAlpha(thePixel) = matteIter.ReadAlphaInc();
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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

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

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteIter.Reset(sCalcRowAddress(iMatteRD, iMatte, matteV), matteV);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

template <class S, class M, class D, class O>
void sTileSource_SMDO_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied,
	const O& iOp)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int sourceLeft = iSourceB.left;
	int sourceWidth = W(iSourceB);
	int sourceHStart = iSourceB.left + sPositiveModulus(iSourceOrigin.h, sourceWidth);
	int sourceCountHStart = iSourceB.right - sourceHStart;
	int sourceCountHReset = sourceWidth;

	int sourceTop = iSourceB.top;
	int sourceBottom = iSourceB.bottom;
	int sourceHeight = H(iSourceB);
	int sourceVStart = iSourceB.top + sPositiveModulus(iSourceOrigin.v, sourceHeight);

	int sourceV = sourceVStart;

	const void* sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sourceRowAddress,
		sourceHStart,
		iSourcePD);

	int matteV = iMatteStart.v;
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, matteV),
		iMatteStart.h,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();

				sAlpha(thePixel) = matteIter.ReadAlphaInc();

				RGBA destPixel = destIter.Read();

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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

				RGBA destPixel = destIter.Read();

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

		sourceRowAddress = sCalcRowAddress(iSourceRD, iSource, sourceV);
		sourceIter.Reset(sourceRowAddress, sourceHStart);

		matteIter.Reset(sCalcRowAddress(iMatteRD, iMatte, matteV), matteV);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
// MARK: - Tile matte, untiled source

template <class S, class M, class D>
void sTileMatte_SMD_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int sourceV = iSourceStart.v;
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, sourceV),
		iSourceStart.h,
		iSourcePD);

	int matteV = matteVStart;
	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();
				sAlpha(thePixel) = matteIter.ReadAlphaInc();
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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

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

		sourceIter.Reset(sCalcRowAddress(iSourceRD, iSource, sourceV), iSourceStart.h);

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

// Trinary with operation
template <class S, class M, class D, class O>
void sTileMatte_SMDO_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, const O& iOp)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int sourceV = iSourceStart.v;
	PixelIterR_T<S> sourceIter(
		iSourceRD.fPixvalDesc,
		sCalcRowAddress(iSourceRD, iSource, sourceV),
		iSourceStart.h,
		iSourcePD);

	int matteV = matteVStart;
	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
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
				RGBA thePixel = sourceIter.ReadInc();

				sAlpha(thePixel) = matteIter.ReadAlphaInc();

				RGBA destPixel = destIter.Read();

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
				RGBA thePixel = sourceIter.ReadInc();

//				uint16 alpha = matteIter.ReadAlphaInc();
//				thePixel.red = sMul(thePixel.red, alpha);
//				thePixel.green = sMul(thePixel.green, alpha);
//				thePixel.blue = sMul(thePixel.blue, alpha);
//				sAlpha(thePixel) = alpha;

				RGBA destPixel = destIter.Read();

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

		sourceIter.Reset(sCalcRowAddress(iSourceRD, iSource, sourceV), iSourceStart.h);

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
// MARK: - Fill/color variants

static void sFillPixval(void* oDest, const RD& iDestRD, const RectPOD& iDestB,
			uint32 iPixval)
	{
	PixvalIterW destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
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
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class D, class O>
void sColor_DO_T(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor,
	const O& iOp)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA destPixel = destIter.Read();
			iOp(iColor, destPixel);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D>
void sColor_MD_T(
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v),
		iMatteStart.h,
		iMattePD);

	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				RGBA tempColor = iColor;
//				sMul(tempColor, alpha);
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
			sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D, class O>
void sColor_MDO_T(
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor,
	const O& iOp)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v),
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				RGBA destPixel = destIter.Read();
				RGBA tempColor = iColor;
//				sMul(tempColor, alpha);
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
			sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

template <class M, class D>
void sColorTile_MD_T(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;
	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				RGBA tempColor = iColor;
//				sMul(tempColor, alpha);
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

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

template <class M, class D, class O>
void sColorTile_MDO_T(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor, const O& iOp)
	{
	int destWidth = W(iDestB);
	int destBottom = iDestB.bottom;

	int matteLeft = iMatteB.left;
	int matteWidth = W(iMatteB);
	int matteHStart = iMatteB.left + sPositiveModulus(iMatteOrigin.h, matteWidth);
	int matteCountHStart = iMatteB.right - matteHStart;
	int matteCountHReset = matteWidth;

	int matteTop = iMatteB.top;
	int matteBottom = iMatteB.bottom;
	int matteHeight = H(iMatteB);
	int matteVStart = iMatteB.top + sPositiveModulus(iMatteOrigin.v, matteHeight);

	int matteV = matteVStart;
	const void* matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		matteRowAddress,
		matteHStart,
		iMattePD);

	int currentDestV = iDestB.top;
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, currentDestV),
		iDestB.left,
		iDestPD);

	for (;;)
		{
		int destCountH = destWidth + 1;
		int matteCountH = matteCountHStart;

		while (--destCountH)
			{
			if (uint16 alpha = matteIter.ReadAlphaInc())
				{
				RGBA tempColor = iColor;
//				sMul(tempColor, alpha);
				RGBA destPixel = destIter.Read();
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

		matteRowAddress = sCalcRowAddress(iMatteRD, iMatte, matteV);
		matteIter.Reset(matteRowAddress, matteHStart);

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, currentDestV), iDestB.left);
		}
	}

// =================================================================================================
// MARK: - Blit dispatchers

// Replicate iSourceB over iDestB, aligning iSourceOrigin with iDestB.TopLeft()
template <class S, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sTile_SD_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				oDest, iDestRD, iDestB, iDestPD);
			break;
			}
		case eOp_Over:
			{
			sTile_SDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTile_SDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				Compose_Plus());
			break;
			}
		}
	}

// Copy source to iDestB without replication. Actual drawn rectangle will be smaller
// than iDestB if iSourceB is smaller.
template <class S, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sCopy_SD_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				oDest, iDestRD, iDestB, iDestPD);
			break;
			}
		case eOp_Over:
			{
			sCopy_SDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				oDest, iDestRD, iDestB, iDestPD,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sCopy_SDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				oDest, iDestRD, iDestB, iDestPD,
				Compose_Plus());
			break;
			}
		}
	}

// Replicate iSourceB masked by replicated iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sTile_SMD_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTile_SMDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTile_SMDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Replicate iSourceB masked by iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const RectPOD& iSourceB, const S& iSourcePD,
	PointPOD iSourceOrigin,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sTileSource_SMD_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTileSource_SMDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTileSource_SMDO_T(iSource, iSourceRD, iSourceB, iSourcePD,
				iSourceOrigin,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Draw iSourceB masked by replicated iMatteB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sTileMatte_SMD_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sTileMatte_SMDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sTileMatte_SMDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Draw iSourceB masked by iMatteB into iDestB.
template <class S, class M, class D>
void sBlit_T(
	const void* iSource, const RD& iSourceRD, const S& iSourcePD,
	PointPOD iSourceStart,
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	bool iSourcePremultiplied, EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sCopy_SMD_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied);
			break;
			}
		case eOp_Over:
			{
			sCopy_SMDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sCopy_SMDO_T(iSource, iSourceRD, iSourcePD,
				iSourceStart,
				iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iSourcePremultiplied,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestB with iColor
template <class D>
void sColor_T(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sFillPixval(oDest, iDestRD, iDestB, iDestPD.AsPixval(iColor));
			break;
			}
		case eOp_Over:
			{
			sColor_DO_T(oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColor_DO_T(oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestB matted
template <class M, class D>
void sColor_T(
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sColor_MD_T(iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iColor);
			break;
			}
		case eOp_Over:
			{
			sColor_MDO_T(iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColor_MDO_T(iMatte, iMatteRD, iMattePD,
				iMatteStart,
				oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Fill iDestbounds with tiled matte
template <class M, class D>
void sColorTile_T(
	const void* iMatte, const RD& iMatteRD, const RectPOD& iMatteB, const M& iMattePD,
	PointPOD iMatteOrigin,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD,
	const RGBA& iColor,
	EOp iOp)
	{
	switch (iOp)
		{
		default:
		case eOp_Copy:
			{
			sColorTile_MD_T(iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iColor);
			break;
			}
		case eOp_Over:
			{
			sColorTile_MDO_T(iMatte, iMatteRD, iMatteB, iMattePD,
			iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Over());
			break;
			}
		case eOp_Plus:
			{
			sColorTile_MDO_T(iMatte, iMatteRD, iMatteB, iMattePD,
				iMatteOrigin,
				oDest, iDestRD, iDestB, iDestPD,
				iColor,
				Compose_Plus());
			break;
			}
		}
	}

// Invert iDestbounds
template <class D>
void sInvert_T(void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA destPixel = destIter.Read();
//			destPixel.red = destPixel.alpha - destPixel.red;
//			destPixel.green = destPixel.alpha - destPixel.green;
//			destPixel.blue = destPixel.alpha - destPixel.blue;
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

// Multiply r,g, b & alpha by iAmount/65536
template <class D>
void sOpaque_T(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA destPixel = destIter.Read();
//			destPixel.red = sMul(destPixel.red, iAmount);
//			destPixel.green = sMul(destPixel.green, iAmount);
//			destPixel.blue = sMul(destPixel.blue, iAmount);
//			destPixel.alpha = sMul(destPixel.alpha, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

// Multiply r, g, b by iAmount/65536, leaving alpha alone.
template <class D>
void sDarken_T(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA destPixel = destIter.Read();
//			destPixel.red = sMul(destPixel.red, iAmount);
//			destPixel.green = sMul(destPixel.green, iAmount);
//			destPixel.blue = sMul(destPixel.blue, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

// Multiply alpha by iAmount/65536, leaving r,g,b alone
template <class D>
void sFade_T(
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD, uint16 iAmount)
	{
	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA destPixel = destIter.Read();
//			destPixel.alpha = sMul(destPixel.alpha, iAmount);
			destIter.WriteInc(destPixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		destIter.Reset(sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

// Apply the alpha values in matte to dest
template <class M, class D>
void sApplyMatte_T(
	const void* iMatte, const RD& iMatteRD, const M& iMattePD,
	PointPOD iMatteStart,
	void* oDest, const RD& iDestRD, const RectPOD& iDestB, const D& iDestPD)
	{
	PixelIterR_T<M> matteIter(
		iMatteRD.fPixvalDesc,
		sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v),
		iMatteStart.h,
		iMattePD);

	PixelIterRW_T<D> destIter(
		iDestRD.fPixvalDesc,
		sCalcRowAddress(iDestRD, oDest, iDestB.top),
		iDestB.left,
		iDestPD);

	int destWidth = W(iDestB);
	int destHeight = H(iDestB);
	int row = 0;
	for (;;)
		{
		int destCountH = destWidth + 1;

		while (--destCountH)
			{
			RGBA thePixel = destIter.Read();

			Comp alpha = matteIter.ReadAlphaInc();
//			thePixel.red = sMul(thePixel.red, alpha);
//			thePixel.green = sMul(thePixel.green, alpha);
//			thePixel.blue = sMul(thePixel.blue, alpha);
			sAlpha(thePixel) = alpha;

			destIter.WriteInc(thePixel);
			}

 		++row;
		if (row >= destHeight)
			break;

		matteIter.Reset(
			sCalcRowAddress(iMatteRD, iMatte, iMatteStart.v + row), iMatteStart.h);

		destIter.Reset(
			sCalcRowAddress(iDestRD, oDest, iDestB.top + row), iDestB.left);
		}
	}

} // namespace Pixels
} // namespace ZooLib

#endif // __ZDCPixmapBlitPriv_h__
