/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZGRgn.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Factory functions

static ZRef<ZGRgnRep> spMake(const ZRectPOD& iBounds)
	{
	return FunctionChain<ZRef<ZGRgnRep>, const ZRectPOD&>
		::sInvoke(iBounds);
	}

// =================================================================================================
// MARK: - ZGRgn

ZGRgnRep::ZGRgnRep()
	{}

// =================================================================================================
// MARK: - ZGRgn

ZGRgn::ZGRgn()
	{}

ZGRgn::~ZGRgn()
	{}

ZGRgn::ZGRgn(const ZGRgn& iOther)
:	fRep(iOther.fRep)
	{}

ZGRgn& ZGRgn::operator=(const ZGRgn& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZGRgn::ZGRgn(const ZRef<ZGRgnRep>& iRep)
:	fRep(iRep)
	{}

ZGRgn& ZGRgn::operator=(const ZRef<ZGRgnRep>& iRep)
	{
	fRep = iRep;
	return *this;
	}

ZGRgn::ZGRgn(const ZRectPOD& iRect)
:	fRep(spMake(iRect))
	{}

ZGRgn::ZGRgn(ZCoord iLeft, ZCoord iTop, ZCoord iRight, ZCoord iBottom)
:	fRep(spMake(sRectPOD(iLeft, iTop, iRight, iBottom)))
	{}

ZGRgn& ZGRgn::operator=(const ZRectPOD& iRect)
	{
	fRep = spMake(iRect);
	return *this;
	}

ZRef<ZGRgnRep> ZGRgn::GetRep() const
	{ return fRep; }

size_t ZGRgn::Decompose(DecomposeProc iProc, void* iRefcon) const
	{
	if (fRep)
		return fRep->Decompose(iProc, iRefcon);
	return 0;
	}

void ZGRgn::Clear()
	{ fRep.Clear(); }

bool ZGRgn::IsEmpty() const
	{ return !fRep && fRep->IsEmpty(); }

ZRectPOD ZGRgn::Bounds() const
	{
	if (fRep)
		return fRep->Bounds();
	return sRectPOD(0, 0, 0, 0);
	}

bool ZGRgn::IsSimpleRect() const
	{
	if (fRep)
		return fRep->IsSimpleRect();
	return true;
	}

bool ZGRgn::Contains(ZCoord iH, ZCoord iV) const
	{
	if (fRep)
		return fRep->Contains(iH, iV);
	return false;
	}

bool ZGRgn::Contains(const ZPointPOD& iPoint) const
	{
	if (fRep)
		return fRep->Contains(iPoint.h, iPoint.v);
	return false;
	}

bool ZGRgn::IsEqualTo(const ZGRgn& iOther) const
	{
	if (fRep)
		{
		if (iOther.fRep)
			fRep->IsEqualTo(iOther.fRep);
		return fRep->IsEmpty();
		}
	else if (iOther.fRep)
		{
		return iOther.fRep->IsEmpty();
		}
	return true;
	}

void ZGRgn::Outline()
	{ *this -= this->Insetted(1, 1); }

ZGRgn ZGRgn::Outlined() const
	{ return *this - this->Insetted(1, 1); }

void ZGRgn::Inset(ZCoord iH, ZCoord iV)
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Insetted(iH, iV);
		else
			fRep->Inset(iH, iV);
		}
	}

ZGRgn ZGRgn::Insetted(ZCoord iH, ZCoord iV) const
	{
	if (fRep)
		return ZGRgn(fRep->Insetted(iH, iV));
	return *this;
	}

void ZGRgn::Offset(ZCoord iOffsetH, ZCoord iOffsetV)
	{
	if (iOffsetH || iOffsetV)
		{
		if (fRep)
			{
			if (fRep->IsShared())
				fRep = fRep->Offsetted(iOffsetH, iOffsetV);
			else
				fRep->Offset(iOffsetH, iOffsetV);
			}
		}
	}

ZGRgn ZGRgn::Offsetted(ZCoord iOffsetH, ZCoord iOffsetV) const
	{
	if (iOffsetH || iOffsetV)
		{
		if (fRep)
			ZGRgn(fRep->Offsetted(iOffsetH, iOffsetV));
		}
	return *this;
	}

void ZGRgn::Include(const ZRectPOD& iRect)
	{
	if (not fRep)
		fRep = spMake(iRect);
	else if (fRep->IsShared())
		fRep = fRep->Including(iRect);
	else
		fRep->Include(iRect);
	}

ZGRgn ZGRgn::Including(const ZRectPOD& iRect) const
	{
	if (not fRep)
		return ZGRgn(spMake(iRect));
	else
		return ZGRgn(fRep->Including(iRect));
	}

void ZGRgn::Intersect(const ZRectPOD& iRect)
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Intersecting(iRect);
		else
			fRep->Intersect(iRect);
		}
	}

ZGRgn ZGRgn::Intersecting(const ZRectPOD& iRect) const
	{
	if (fRep)
		return ZGRgn(fRep->Intersecting(iRect));
	return *this;
	}

void ZGRgn::Exclude(const ZRectPOD& iRect)
	{
	if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Excluding(iRect);
		else
			fRep->Exclude(iRect);
		}
	}

ZGRgn ZGRgn::Excluding(const ZRectPOD& iRect) const
	{
	if (fRep)
		return ZGRgn(fRep->Excluding(iRect));
	return *this;
	}

void ZGRgn::Include(const ZGRgn& iOther)
	{
	if (not fRep)
		fRep = iOther.fRep;
	else if (iOther.fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Including(iOther.fRep);
		else
			fRep->Include(iOther.fRep);
		}
	}

ZGRgn ZGRgn::Including(const ZGRgn& iOther) const
	{
	if (not fRep)
		return iOther;
	else if (iOther.fRep)
		return ZGRgn(fRep->Including(iOther.fRep));
	else
		return *this;
	}

void ZGRgn::Intersect(const ZGRgn& iOther)
	{
	if (not iOther.fRep)
		{
		fRep.Clear();
		}
	else if (fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Intersecting(iOther.fRep);
		else
			fRep->Intersect(iOther.fRep);
		}
	}

ZGRgn ZGRgn::Intersecting(const ZGRgn& iOther) const
	{
	if (fRep && iOther.fRep)
		return ZGRgn(fRep->Intersecting(iOther.fRep));
	return ZGRgn();
	}

void ZGRgn::Exclude(const ZGRgn& iOther)
	{
	if (fRep && iOther.fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Excluding(iOther.fRep);
		else
			fRep->Exclude(iOther.fRep);
		}
	}

ZGRgn ZGRgn::Excluding(const ZGRgn& iOther) const
	{
	if (fRep && iOther.fRep)
		return ZGRgn(fRep->Excluding(iOther.fRep));
	return ZGRgn();
	}

void ZGRgn::Xor(const ZGRgn& iOther)
	{
	if (not fRep)
		{
		fRep = iOther.fRep;
		}
	else if (iOther.fRep)
		{
		if (fRep->IsShared())
			fRep = fRep->Xoring(iOther.fRep);
		else
			fRep->Xor(iOther.fRep);
		}
	}

ZGRgn ZGRgn::Xoring(const ZGRgn& iOther) const
	{
	if (not fRep)
		return iOther;
	else if (iOther.fRep)
		return ZGRgn(fRep->Xoring(iOther.fRep));
	else
		return *this;
	}

} // namespace ZooLib
