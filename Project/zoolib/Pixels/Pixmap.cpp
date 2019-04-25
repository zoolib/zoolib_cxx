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

#include "zoolib/Pixels/Pixmap.h"

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Cartesian_Geom.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixmapRep

PixmapRep::PixmapRep(const ZRef<Raster>& iRaster, const RectPOD& iBounds, const PixelDesc& iPixelDesc)
:	fRaster(iRaster)
,	fBounds(iBounds)
,	fPixelDesc(iPixelDesc)
	{}

PointPOD PixmapRep::GetSize()
	{ return WH(fBounds); }

const RectPOD& PixmapRep::GetBounds()
	{ return fBounds; }

const PixelDesc& PixmapRep::GetPixelDesc()
	{ return fPixelDesc; }

const ZRef<Raster>& PixmapRep::GetRaster()
	{ return fRaster; }

ZRef<PixmapRep> PixmapRep::Touch()
	{
	if (this->IsShared() || fRaster->IsShared() || not fRaster->GetMutable())
		{
		const RasterDesc& ourRasterDesc = fRaster->GetRasterDesc();
		RasterDesc newRasterDesc = ourRasterDesc;
		newRasterDesc.fRowBytes =
			sCalcRowBytes(W(fBounds), newRasterDesc.fPixvalDesc.fDepth, 4);
		newRasterDesc.fRowCount = H(fBounds);

		ZRef<PixmapRep> newRep =
			sPixmapRep(newRasterDesc, WH(fBounds), fPixelDesc);

		sBlit(this, fBounds, newRep, sPointPOD(0,0));

		return newRep;
		}
	return this;
	}

ZRef<PixmapRep> sPixmapRep(const RasterDesc& iRasterDesc,
	const PointPOD& iSize,
	const PixelDesc& iPixelDesc)
	{
	ZRef<Raster> theRaster = new Raster_Simple(iRasterDesc);
	return new PixmapRep(theRaster, sRect<RectPOD>(iSize), iPixelDesc);
	}

// =================================================================================================
#pragma mark - Pixmap

Pixmap::Pixmap()
	{}

Pixmap::Pixmap(const Pixmap& iOther)
:	fRep(iOther.fRep)
	{}

Pixmap::~Pixmap()
	{}

Pixmap& Pixmap::operator=(const Pixmap& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Pixmap::Pixmap(const ZRef<PixmapRep>& iRep)
:	fRep(iRep)
	{}

Pixmap& Pixmap::operator=(const ZRef<PixmapRep>& iOther)
	{
	fRep = iOther;
	return *this;
	}

Pixmap::Pixmap(const null_t&)
	{}

Pixmap& Pixmap::operator=(const null_t&)
	{
	fRep = null;
	return *this;
	}

PointPOD Pixmap::Size() const
	{ return WH(fRep->GetBounds()); }

Ord Pixmap::Width() const
	{ return W(fRep->GetBounds()); }

Ord Pixmap::Height() const
	{ return H(fRep->GetBounds()); }

void Pixmap::Touch()
	{
	if (fRep)
		fRep = fRep->Touch();
	}

const ZRef<PixmapRep>& Pixmap::GetRep() const
	{ return fRep; }

const ZRef<Raster>& Pixmap::GetRaster() const
	{ return fRep->GetRaster(); }

const void* Pixmap::GetBaseAddress() const
	{ return fRep->GetRaster()->GetBaseAddress(); }

void* Pixmap::GetBaseAddress()
	{
	this->Touch();
	return fRep->GetRaster()->GetBaseAddress();
	}

const RasterDesc& Pixmap::GetRasterDesc() const
	{ return fRep->GetRaster()->GetRasterDesc(); }

const PixelDesc& Pixmap::GetPixelDesc() const
	{ return fRep->GetPixelDesc(); }

const RectPOD& Pixmap::GetBounds() const
	{ return fRep->GetBounds(); }


Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc)
	{ return sPixmapRep(iRasterDesc, iSize, iPixelDesc); }

// =================================================================================================
#pragma mark - Pixmap

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon)
	{
	sMunge(ioPixmap.GetBaseAddress(),
		ioPixmap.GetRasterDesc(),
		ioPixmap.GetPixelDesc(),
		ioPixmap.GetBounds(),
		iMungeProc, iRefcon);
	}

void sBlit(const ZRef<PixmapRep>& iSource, const RectPOD& iSourceBounds,
	const ZRef<PixmapRep>& ioDest, PointPOD iDestLoc)
	{
	ZRef<Raster> sourceRaster = iSource->GetRaster();
	ZRef<Raster> destRaster = ioDest->GetRaster();
	sBlit(sourceRaster->GetRasterDesc(), sourceRaster->GetBaseAddress(), iSource->GetPixelDesc(),
		destRaster->GetRasterDesc(), destRaster->GetBaseAddress(), ioDest->GetPixelDesc(),
		iSourceBounds, iDestLoc);
	}

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc)
	{
	ioDest.Touch();
	sBlit(iSource.GetRep(), iSourceBounds, ioDest.GetRep(), iDestLoc);
	}

} // namespace Pixels
} // namespace ZooLib
