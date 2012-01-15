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

#include "zoolib/ZDCRgn.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - ZDCRgn

ZDCRgn ZDCRgn::sLine(ZCoord iStartH, ZCoord iStartV,
	ZCoord iEndH, ZCoord iEndV, ZCoord iLineWidth)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sRects(const ZRect* iRects, size_t iCount, bool iAlreadySorted)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sRoundRect(const ZRect& iRect, const ZPoint& iCornerSize)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sRoundRect(ZCoord iLeft, ZCoord iTop,
	ZCoord iRight, ZCoord iBottom, ZCoord iCornerSizeH, ZCoord iCornerSizeV)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sEllipse(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sPoly(const ZDCPoly& iPoly, bool iEvenOdd, ZCoord iOffsetH, ZCoord iOffsetV)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

ZDCRgn ZDCRgn::sPoly(const ZPoint* iPoints, size_t iCount, bool iEvenOdd,
	ZCoord iOffsetH, ZCoord iOffsetV)
	{
	ZUnimplemented();
	return ZDCRgn();
	}

// =================================================================================================
// MARK: - ZDCRgn

ZDCRgn::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fGRgn); }

ZDCRgn::ZDCRgn()
	{}

ZDCRgn::ZDCRgn(const ZDCRgn& iOther)
:	fGRgn(iOther.fGRgn)
	{}

ZDCRgn& ZDCRgn::operator=(const ZDCRgn& iOther)
	{
	fGRgn = iOther.fGRgn;
	return *this;
	}

ZDCRgn::~ZDCRgn()
	{}
	
ZDCRgn::ZDCRgn(const ZGRgn& iGRgn)
:	fGRgn(iGRgn)
	{}

ZDCRgn::ZDCRgn(const ZRect& iRect)
:	fGRgn(iRect)
	{}

ZDCRgn::ZDCRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
:	fGRgn(iLeft, iTop, iRight, iBottom)
	{}
	
ZDCRgn& ZDCRgn::operator=(const ZRect& iRect)
	{
	fGRgn = iRect;
	return *this;
	}

#if ZCONFIG_API_Enabled(GRgnRep_RgnHandle)
ZDCRgn::ZDCRgn(RgnHandle iRgnHandle, bool iAdopt)
:	fGRgn(new ZGRgnRep_RgnHandle(iRgnHandle, iAdopt))
	{}

ZDCRgn& ZDCRgn::operator=(RgnHandle iRgnHandle)
	{
	fGRgn = new ZGRgnRep_RgnHandle(iRgnHandle);
	return *this;
	}

RgnHandle ZDCRgn::GetRgnHandle()
	{
	ZRef<ZGRgnRep_RgnHandle> theRep = ZGRgnRep_RgnHandle::sGetRep(fGRgn.GetRep());
	fGRgn = theRep;
	return theRep->GetRgnHandle();
	}
#endif

#if ZCONFIG_API_Enabled(GRgnRep_HRGN)
ZDCRgn::ZDCRgn(HRGN iHRGN, bool iAdopt)
:	fGRgn(new ZGRgnRep_HRGN(iHRGN, iAdopt))
	{}

ZDCRgn& ZDCRgn::operator=(HRGN iHRGN)
	{
	fGRgn = new ZGRgnRep_HRGN(iHRGN);
	return *this;
	}

HRGN ZDCRgn::GetHRGN()
	{
	ZRef<ZGRgnRep_HRGN> theRep = ZGRgnRep_HRGN::sGetRep(fGRgn.GetRep());
	fGRgn = theRep;
	return theRep->GetHRGN();
	}
#endif

static bool spDecompose_IntoVector(const ZRect& iRect, void* iRefcon)
	{
	reinterpret_cast<vector<ZRect>*>(iRefcon)->push_back(iRect);
	return false;
	}

void ZDCRgn::Decompose(vector<ZRect>& oRects) const
	{
	oRects.clear();
	this->Decompose(spDecompose_IntoVector, &oRects);
	}

size_t ZDCRgn::Decompose(DecomposeProc iProc, void* iRefcon) const
	{ return fGRgn.Decompose(iProc, iRefcon); }

bool ZDCRgn::Contains(const ZPoint& iPoint) const
	{ return fGRgn.Contains(iPoint); }

bool ZDCRgn::Contains(ZCoord iH, ZCoord iV) const
	{ return fGRgn.Contains(iH, iV); }

void ZDCRgn::MakeOutline()
	{ fGRgn.Outline(); }

ZDCRgn ZDCRgn::Outline() const
	{ return fGRgn.Outlined(); }

ZDCRgn ZDCRgn::Inset(const ZPoint& iPoint) const
	{ return fGRgn.Insetted(iPoint); }

ZDCRgn ZDCRgn::Inset(ZCoord iH, ZCoord iV) const
	{ return fGRgn.Insetted(iH, iV); }

void ZDCRgn::MakeInset(const ZPoint& iPoint)
	{ fGRgn.Inset(iPoint); }

void ZDCRgn::MakeInset(ZCoord iH, ZCoord iV)
	{ fGRgn.Inset(iH, iV); }

void ZDCRgn::Clear()
	{ fGRgn.Clear(); }

bool ZDCRgn::IsEmpty() const
	{ return fGRgn.IsEmpty(); }

ZRect ZDCRgn::Bounds() const
	{ return fGRgn.Bounds(); }

bool ZDCRgn::IsSimpleRect() const
	{ return fGRgn.IsSimpleRect(); }

bool ZDCRgn::operator==(const ZDCRgn& iOther) const
	{ return fGRgn == iOther.fGRgn; }

bool ZDCRgn::operator!=(const ZDCRgn& iOther) const
	{ return ! (*this == iOther); }

ZDCRgn& ZDCRgn::operator+=(const ZPoint& iOffset)
	{
	fGRgn += iOffset;
	return *this;
	}

ZDCRgn ZDCRgn::operator+(const ZPoint& iOffset) const
	{ return fGRgn + iOffset; }

ZDCRgn& ZDCRgn::operator-=(const ZPoint& iOffset)
	{
	fGRgn -= iOffset;
	return *this;
	}

ZDCRgn ZDCRgn::operator-(const ZPoint& iOffset) const
	{ return fGRgn - iOffset; }

ZDCRgn& ZDCRgn::operator|=(const ZRect& iRect)
	{
	fGRgn |= iRect;
	return *this;
	}

ZDCRgn ZDCRgn::operator|(const ZRect& iRect) const
	{ return fGRgn | iRect; }

ZDCRgn& ZDCRgn::operator&=(const ZRect& iRect)
	{
	fGRgn &= iRect;
	return *this;
	}

ZDCRgn ZDCRgn::operator&(const ZRect& iRect) const
	{ return fGRgn & iRect; }

ZDCRgn& ZDCRgn::operator-=(const ZRect& iRect)
	{
	fGRgn -= iRect;
	return *this;
	}

ZDCRgn ZDCRgn::operator-(const ZRect& iRect) const
	{ return fGRgn - iRect; }

ZDCRgn& ZDCRgn::operator|=(const ZDCRgn& iOther)
	{
	fGRgn |= iOther.fGRgn;
	return *this;
	}

ZDCRgn ZDCRgn::operator|(const ZDCRgn& iOther) const
	{ return fGRgn | iOther.fGRgn; }

ZDCRgn& ZDCRgn::operator&=(const ZDCRgn& iOther)
	{
	fGRgn &= iOther.fGRgn;
	return *this;
	}

ZDCRgn ZDCRgn::operator&(const ZDCRgn& iOther) const
	{ return fGRgn & iOther.fGRgn; }

ZDCRgn& ZDCRgn::operator-=(const ZDCRgn& iOther)
	{
	fGRgn -= iOther.fGRgn;
	return *this;
	}

ZDCRgn ZDCRgn::operator-(const ZDCRgn& iOther) const
	{ return fGRgn - iOther.fGRgn; }

ZDCRgn& ZDCRgn::operator^=(const ZDCRgn& iOther)
	{
	fGRgn ^= iOther.fGRgn;
	return *this;
	}

ZDCRgn ZDCRgn::operator^(const ZDCRgn& iOther) const
	{ return fGRgn ^ iOther.fGRgn; }

} // namespace ZooLib
