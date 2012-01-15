/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZDCRgn_h__
#define __ZDCRgn_h__ 1
#include "zconfig.h"

#include "zoolib/ZGRgn.h"
#include "zoolib/ZGRgnRep_HRGN.h"
#include "zoolib/ZGRgnRep_RgnHandle.h"
#include "zoolib/ZGRgnRep_XRegion.h"

namespace ZooLib {

class ZDCPoly;

// =================================================================================================
// MARK: - ZDCRgn

class ZDCRgn
	{
public:
	static ZDCRgn sLine(ZCoord iStartH, ZCoord iStartV,
		ZCoord iEndH, ZCoord iEndV, ZCoord iLineWidth);

	static ZDCRgn sLine(const ZPoint& iStart, const ZPoint& iEnd, ZCoord iLineWidth)
		{ return sLine(iStart.h, iStart.v, iEnd.h, iEnd.v, iLineWidth); }

// Rect
	static ZDCRgn sRect(const ZRect& iRect)
		{ return ZDCRgn(iRect); }

	static ZDCRgn sRect(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
		{ return ZDCRgn(iLeft, iTop, iRight, iBottom); }

	static ZDCRgn sRects(const ZRect* iRects, size_t iCount, bool iAlreadySorted);

// Round rect
	static ZDCRgn sRoundRect(const ZRect& iRect, const ZPoint& iCornerSize);

	static ZDCRgn sRoundRect(ZCoord iLeft, ZCoord iTop,
		ZCoord iRight, ZCoord iBottom, ZCoord iCornerSizeH, ZCoord iCornerSizeV);

// Ellipse
	static ZDCRgn sEllipse(const ZRect& iBounds)
		{ return sEllipse(iBounds.left, iBounds.top, iBounds.right, iBounds.bottom); }
	static ZDCRgn sEllipse(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom);

// Polygon
	static ZDCRgn sPoly(const ZDCPoly& iPoly, bool iEvenOdd, ZCoord iOffsetH, ZCoord iOffsetV);

	static ZDCRgn sPoly(const ZDCPoly& iPoly, ZCoord iOffsetH, ZCoord iOffsetV)
		{ return sPoly(iPoly, true, iOffsetH, iOffsetV); }

	static ZDCRgn sPoly(const ZDCPoly& iPoly, bool iEvenOdd)
		{ return sPoly(iPoly, iEvenOdd, 0, 0); }

	static ZDCRgn sPoly(const ZDCPoly& iPoly)
		{ return sPoly(iPoly, true, 0, 0); }

	static ZDCRgn sPoly(const ZPoint* iPoints, size_t iCount, bool iEvenOdd,
		ZCoord iOffsetH, ZCoord iOffsetV);

	static ZDCRgn sPoly(const ZPoint* iPoints, size_t iCount, ZCoord iOffsetH, ZCoord iOffsetV)
		{ return sPoly(iPoints, iCount, true, iOffsetH, iOffsetV); }

	static ZDCRgn sPoly(const ZPoint* iPoints, size_t iCount, bool iEvenOdd)
		{ return sPoly(iPoints, iCount, iEvenOdd, 0, 0); }

	static ZDCRgn sPoly(const ZPoint* iPoints, size_t iCount)
		{ return sPoly(iPoints, iCount, true, 0, 0); }

    ZOOLIB_DEFINE_OPERATOR_BOOL_TYPES(ZDCRgn, operator_bool_generator_type, operator_bool_type);
	operator operator_bool_type() const;

	ZDCRgn();
	ZDCRgn(const ZDCRgn& iOther);
	ZDCRgn& operator=(const ZDCRgn& iOther);
	~ZDCRgn();

	ZDCRgn(const ZGRgn& iGRgn);

	ZDCRgn(const ZRect& iRect);
	ZDCRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom);

	ZDCRgn& operator=(const ZRect& iRect);

	#if ZCONFIG_API_Enabled(GRgnRep_RgnHandle)
	ZDCRgn(RgnHandle iRgnHandle, bool iAdopt);
	ZDCRgn& operator=(RgnHandle iRgnHandle);
	RgnHandle GetRgnHandle();
	#endif

	#if ZCONFIG_API_Enabled(GRgnRep_HRGN)
	ZDCRgn(HRGN iHRGN, bool iAdopt);
	ZDCRgn& operator=(HRGN iHRGN);
	HRGN GetHRGN();
	#endif

	#if ZCONFIG_API_Enabled(GRgnRep_XRegion)
	ZDCRgn(Region iRegion, bool iAdopt);
	ZDCRgn& operator=(Region iRegion);
	Region GetRegion();
	#endif

	#if ZCONFIG_SPI_Enabled(GDI)
	void MakeScale(float m11, float m12, float m21, float m22, float hOffset, float vOffset);
	#endif

	void Decompose(std::vector<ZRect>& oRects) const;
	typedef ZGRgnRep::DecomposeProc DecomposeProc;
	size_t Decompose(DecomposeProc iProc, void* iRefcon) const;

	bool Contains(const ZPoint& iPoint) const;
	bool Contains(ZCoord iH, ZCoord iV) const;

	void MakeOutline();
	ZDCRgn Outline() const;

	ZDCRgn Inset(const ZPoint& iPoint) const;
	ZDCRgn Inset(ZCoord iH, ZCoord iV) const;

	void MakeInset(const ZPoint& iPoint);
	void MakeInset(ZCoord iH, ZCoord iV);

	void Clear();
	bool IsEmpty() const;

	ZRect Bounds() const;
	bool IsSimpleRect() const;

// Equality
	bool operator==(const ZDCRgn& iOther) const;
	bool operator!=(const ZDCRgn& iOther) const;

// Algebraic API
	ZDCRgn& operator+=(const ZPoint& iOffset);
	ZDCRgn operator+(const ZPoint& iOffset) const;

	ZDCRgn& operator-=(const ZPoint& iOffset);
	ZDCRgn operator-(const ZPoint& iOffset) const;

// Combining with rectangles
	ZDCRgn& operator|=(const ZRect& iRect);
	ZDCRgn operator|(const ZRect& iRect) const;

	ZDCRgn& operator&=(const ZRect& iRect);
	ZDCRgn operator&(const ZRect& iRect) const;

	ZDCRgn& operator-=(const ZRect& iRect);
	ZDCRgn operator-(const ZRect& iRect) const;

// Combining with regions
	ZDCRgn& operator|=(const ZDCRgn& iOther);
	ZDCRgn operator|(const ZDCRgn& iOther) const;

	ZDCRgn& operator&=(const ZDCRgn& iOther);
	ZDCRgn operator&(const ZDCRgn& iOther) const;

	ZDCRgn& operator-=(const ZDCRgn& iOther);
	ZDCRgn operator-(const ZDCRgn& iOther) const;

	ZDCRgn& operator^=(const ZDCRgn& iOther);
	ZDCRgn operator^(const ZDCRgn& iOther) const;

private:
	ZGRgn fGRgn;
	};

// =================================================================================================
// MARK: - ZDCRgnAccumulator

class ZDCRgnUnioner_t
	{
public:
	void operator()(ZDCRgn& ioRgn, const ZDCRgn& iOther) const
		{ ioRgn |= iOther; }
	};

typedef ZAccumulator_T<ZDCRgn, ZDCRgnUnioner_t, std::vector<ZDCRgn> > ZDCRgnAccumulator;

// =================================================================================================

} // namespace ZooLib

#endif // __ZDCRgn_h__
