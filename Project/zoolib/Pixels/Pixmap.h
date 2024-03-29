// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_Pixmap_h__
#define __ZooLib_Pixels_Pixmap_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Geom.h"
#include "zoolib/Pixels/PixelDesc.h"
#include "zoolib/Pixels/Raster.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixmapRep

class PixmapRep
:	public Counted
	{
public:
	PixmapRep(const ZP<Raster>& iRaster, const RectPOD& iFrame, const PixelDesc& iPixelDesc);

	const ZP<Raster>& GetRaster();

	const RectPOD& GetFrame();

	const PixelDesc& GetPixelDesc();

	ZP<PixmapRep> Touched();

protected:
	ZP<Raster> fRaster;
	RectPOD fFrame;
	PixelDesc fPixelDesc;
	};

ZP<PixmapRep> sPixmapRep(const ZP<Raster>& iRaster,
	const RectPOD& iFrame,
	const PixelDesc& iPixelDesc);

ZP<PixmapRep> sPixmapRep(const RasterDesc& iRasterDesc,
	const PointPOD& iSize,
	const PixelDesc& iPixelDesc);

// =================================================================================================
#pragma mark - Pixmap

class Pixmap
	{
public:
	Pixmap();
	Pixmap(const Pixmap& iOther);
	~Pixmap();
	Pixmap& operator=(const Pixmap& iOther);

	Pixmap(const ZP<PixmapRep>& iOther);
	Pixmap& operator=(const ZP<PixmapRep>& iOther);

	Pixmap(PixmapRep* iOther);
	Pixmap& operator=(PixmapRep* iOther);

	Pixmap(const null_t&);
	Pixmap& operator=(const null_t&);

	explicit operator bool() const
		{ return true && fRep; }

	PointPOD Size() const;
	Ord Width() const;
	Ord Height() const;

	void Touch();
	Pixmap Touched() const;

	const ZP<PixmapRep>& GetRep() const;

	const ZP<Raster>& GetRaster() const;
	const RasterDesc& GetRasterDesc() const;
	const void* GetBaseAddress() const;
	void* MutBaseAddress();
	const RectPOD& GetFrame() const;
	const PixelDesc& GetPixelDesc() const;

protected:
	ZP<PixmapRep> fRep;
	};

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc);

// =================================================================================================
#pragma mark - Pixmap

Pixmap sPixmap(const Pixmap& iSource, RectPOD iBounds);

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon);

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc);

uint32 sGetPixval(const Pixmap& iSource, Ord iX, Ord iY);
void sSetPixval(Pixmap& oDest, Ord iX, Ord iY, uint32 iPixval);

uint32 sGetPixval(const Pixmap& iSource, PointPOD iLoc);
void sSetPixval(Pixmap& oDest, PointPOD iLoc, uint32 iPixval);

RGBA sGetPixel(const Pixmap& iSource, Ord iX, Ord iY);
void sSetPixel(Pixmap& oDest, Ord iX, Ord iY, RGBA iPixel);

RGBA sGetPixel(const Pixmap& iSource, PointPOD iLoc);
void sSetPixel(Pixmap& oDest, PointPOD iLoc, RGBA iPixel);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Pixmap_h__
