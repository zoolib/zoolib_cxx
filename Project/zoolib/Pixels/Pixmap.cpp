// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/Pixmap.h"

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Cartesian_Geom.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

static void spBlit(const ZP<PixmapRep>& iSource, const RectPOD& iSourceFrame,
	const ZP<PixmapRep>& ioDest, PointPOD iDestLoc)
	{
	ZP<Raster> sourceRaster = iSource->GetRaster();
	ZP<Raster> destRaster = ioDest->GetRaster();
	ZAssert(destRaster->GetMutable());

	const RectPOD theDestFrame = (iSourceFrame - LT(iSourceFrame)) + iDestLoc;

	sBlit(
		sourceRaster->GetRasterDesc(), sourceRaster->GetBaseAddress(),
			iSourceFrame, iSource->GetPixelDesc(),
		destRaster->GetRasterDesc(), destRaster->GetBaseAddress(),
			theDestFrame, ioDest->GetPixelDesc(),
		eOp_Copy);
	}

// =================================================================================================
#pragma mark - PixmapRep

PixmapRep::PixmapRep(
	const ZP<Raster>& iRaster,const RectPOD& iFrame, const PixelDesc& iPixelDesc)
:	fRaster(iRaster)
,	fFrame(iFrame)
,	fPixelDesc(iPixelDesc)
	{}

PointPOD PixmapRep::GetSize()
	{ return WH(fFrame); }

const RectPOD& PixmapRep::GetFrame()
	{ return fFrame; }

const PixelDesc& PixmapRep::GetPixelDesc()
	{ return fPixelDesc; }

const ZP<Raster>& PixmapRep::GetRaster()
	{ return fRaster; }

ZP<PixmapRep> PixmapRep::Touch()
	{
	if (this->IsShared() || fRaster->IsShared() || not fRaster->GetMutable())
		{
		RasterDesc newRasterDesc = fRaster->GetRasterDesc();
		newRasterDesc.fRowBytes = sCalcRowBytes(W(fFrame), newRasterDesc.fPixvalDesc.fDepth, 4);
		newRasterDesc.fRowCount = H(fFrame);

		ZP<PixmapRep> newRep = sPixmapRep(newRasterDesc, WH(fFrame), fPixelDesc);

		spBlit(this, fFrame, newRep, sPointPOD(0,0));

		return newRep;
		}
	return this;
	}

ZP<PixmapRep> sPixmapRep(const ZP<Raster>& iRaster,
	const RectPOD& iFrame,
	const PixelDesc& iPixelDesc)
	{ return new PixmapRep(iRaster, iFrame, iPixelDesc); }

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

Pixmap::Pixmap(const ZP<PixmapRep>& iOther)
:	fRep(iOther)
	{}

Pixmap& Pixmap::operator=(const ZP<PixmapRep>& iOther)
	{
	fRep = iOther;
	return *this;
	}

Pixmap::Pixmap(PixmapRep* iOther)
:	fRep(iOther)
	{}

Pixmap& Pixmap::operator=(PixmapRep* iOther)
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
	{ return WH(fRep->GetFrame()); }

Ord Pixmap::Width() const
	{ return W(fRep->GetFrame()); }

Ord Pixmap::Height() const
	{ return H(fRep->GetFrame()); }

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

void* Pixmap::MutBaseAddress()
	{
	this->Touch(); //¿ Should we do this ?
	return fRep->GetRaster()->GetBaseAddress();
	}

const RectPOD& Pixmap::GetFrame() const
	{ return fRep->GetFrame(); }

const PixelDesc& Pixmap::GetPixelDesc() const
	{ return fRep->GetPixelDesc(); }

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc)
	{ return sPixmapRep(iRasterDesc, iSize, iPixelDesc); }

// =================================================================================================
#pragma mark - Pixmap

Pixmap sPixmap(const Pixmap& iSource, RectPOD iBounds)
	{
	if (ZP<PixmapRep> sourceRep = iSource.GetRep())
		{
		return new PixmapRep(sourceRep->GetRaster(),
			iBounds + LT(sourceRep->GetFrame()),
			sourceRep->GetPixelDesc());
		}
	return Pixmap();
	}

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon)
	{
	sMunge(ioPixmap.MutBaseAddress(),
		ioPixmap.GetRasterDesc(),
		ioPixmap.GetPixelDesc(),
		ioPixmap.GetFrame(),
		iMungeProc, iRefcon);
	}

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc)
	{
	if (ZP<PixmapRep> sourceRep = iSource.GetRep())
		{
		ioDest.Touch();
		spBlit(sourceRep, iSourceBounds + LT(sourceRep->GetFrame()), ioDest.GetRep(), iDestLoc);
		}
	}

} // namespace Pixels
} // namespace ZooLib
