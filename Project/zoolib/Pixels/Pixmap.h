// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_Pixmap_h__
#define __ZooLib_Pixels_Pixmap_h__ 1
#include "zconfig.h"

#include "zoolib/Compat_operator_bool.h"

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
	PixmapRep(const ZP<Raster>& iRaster, const RectPOD& iBounds, const PixelDesc& iPixelDesc);

	PointPOD GetSize();

	const RectPOD& GetBounds();

	const PixelDesc& GetPixelDesc();

	const ZP<Raster>& GetRaster();

	ZP<PixmapRep> Touch();

protected:
	ZP<Raster> fRaster;
	RectPOD fBounds;
	PixelDesc fPixelDesc;
	};

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

	Pixmap(const ZP<PixmapRep>& iRep);
	Pixmap& operator=(const ZP<PixmapRep>& iOther);

	Pixmap(const null_t&);
	Pixmap& operator=(const null_t&);

	ZMACRO_operator_bool(Pixmap, operator_bool) const
		{ return operator_bool_gen::translate(!!fRep); }

	PointPOD Size() const;
	Ord Width() const;
	Ord Height() const;

	void Touch();

	const ZP<PixmapRep>& GetRep() const;

	const ZP<Raster>& GetRaster() const;
	const RasterDesc& GetRasterDesc() const;
	const void* GetBaseAddress() const;
	void* GetBaseAddress();
	const RectPOD& GetBounds() const;
	const PixelDesc& GetPixelDesc() const;

protected:
	ZP<PixmapRep> fRep;
	};

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc);

// =================================================================================================
#pragma mark - Pixmap

void sMunge(Pixmap& ioPixmap, MungeProc iMungeProc, void* iRefcon);

void sBlit(const Pixmap& iSource, const RectPOD& iSourceBounds,
	Pixmap& ioDest, PointPOD iDestLoc);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Pixmap_h__
