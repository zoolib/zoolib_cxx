// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/Pixmap.h"

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Cartesian_Geom.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

static void spBlit(const ZP<PixmapRep>& iSource, const RectPOD& iSourceBounds,
	const ZP<PixmapRep>& ioDest, PointPOD iDestLoc)
	{
	ZP<Raster> sourceRaster = iSource->GetRaster();
	ZP<Raster> destRaster = ioDest->GetRaster();
	ZAssert(destRaster->GetMutable());

	const RectPOD theDestBounds = iSourceBounds - LT(iSourceBounds) + iDestLoc;

	sBlit(
		sourceRaster->GetRasterDesc(), sourceRaster->GetBaseAddress(),
			iSourceBounds, iSource->GetPixelDesc(),
		destRaster->GetRasterDesc(), destRaster->GetBaseAddress(),
			theDestBounds, ioDest->GetPixelDesc(),
		eOp_Copy);
	}

// =================================================================================================
#pragma mark - PixmapRep

PixmapRep::PixmapRep(
	const ZP<Raster>& iRaster,const RectPOD& iBounds, const PixelDesc& iPixelDesc)
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

const ZP<Raster>& PixmapRep::GetRaster()
	{ return fRaster; }

ZP<PixmapRep> PixmapRep::Touch()
	{
	if (this->IsShared() || fRaster->IsShared() || not fRaster->GetMutable())
		{
		RasterDesc newRasterDesc = fRaster->GetRasterDesc();
		newRasterDesc.fRowBytes = sCalcRowBytes(W(fBounds), newRasterDesc.fPixvalDesc.fDepth, 4);
		newRasterDesc.fRowCount = H(fBounds);

		ZP<PixmapRep> newRep = sPixmapRep(newRasterDesc, WH(fBounds), fPixelDesc);

		spBlit(this, fBounds, newRep, sPointPOD(0,0));

		return newRep;
		}
	return this;
	}

ZP<PixmapRep> sPixmapRep(const RasterDesc& iRasterDesc,
	const PointPOD& iSize,
	const PixelDesc& iPixelDesc)
	{
	ZP<Raster> theRaster = new Raster_Simple(iRasterDesc);
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

Pixmap::Pixmap(const ZP<PixmapRep>& iRep)
:	fRep(iRep)
	{}

Pixmap& Pixmap::operator=(const ZP<PixmapRep>& iOther)
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

const ZP<PixmapRep>& Pixmap::GetRep() const
	{ return fRep; }

const ZP<Raster>& Pixmap::GetRaster() const
	{ return fRep->GetRaster(); }

const RasterDesc& Pixmap::GetRasterDesc() const
	{ return fRep->GetRaster()->GetRasterDesc(); }

const void* Pixmap::GetBaseAddress() const
	{ return fRep->GetRaster()->GetBaseAddress(); }

void* Pixmap::GetBaseAddress()
	{
	this->Touch(); //¿ Should we do this ?
	return fRep->GetRaster()->GetBaseAddress();
	}

const RectPOD& Pixmap::GetBounds() const
	{ return fRep->GetBounds(); }

const PixelDesc& Pixmap::GetPixelDesc() const
	{ return fRep->GetPixelDesc(); }

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc)
	{ return sPixmapRep(iRasterDesc, iSize, iPixelDesc); }

// =================================================================================================
#pragma mark - Pixmap

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon)
	{
	ioPixmap.Touch();
	sMunge(ioPixmap.GetBaseAddress(),
		ioPixmap.GetRasterDesc(),
		ioPixmap.GetPixelDesc(),
		ioPixmap.GetBounds(),
		iMungeProc, iRefcon);
	}

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc)
	{
	ioDest.Touch();
	spBlit(iSource.GetRep(), iSourceBounds, ioDest.GetRep(), iDestLoc);
	}

} // namespace Pixels
} // namespace ZooLib
