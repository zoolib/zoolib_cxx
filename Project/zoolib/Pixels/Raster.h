// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_Raster_h__
#define __ZooLib_Pixels_Raster_h__ 1
#include "zconfig.h"

#include "zoolib/Counted.h"

#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/RasterDesc.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - Raster

class Raster
:	public Counted
	{
protected:
	Raster(const RasterDesc& iRasterDesc);

public:
	virtual ~Raster();

	const RasterDesc& GetRasterDesc()
		{ return fRasterDesc; }

	bool GetMutable()
		{ return fMutable; }

	void* GetBaseAddress()
		{ return fBaseAddress; }

protected:
	const RasterDesc fRasterDesc;
	bool fMutable;
	void* fBaseAddress;
	};

void sFresh(ZP<Raster>& ioRaster);

// =================================================================================================
#pragma mark - Raster_Simple

class Raster_Simple
:	public Raster
	{
public:
	Raster_Simple(const RasterDesc& iRasterDesc);
	virtual ~Raster_Simple();

protected:
	std::vector<uint8> fBuffer;
	};

// =================================================================================================
#pragma mark - Raster_Static

class Raster_Static
:	public Raster
	{
public:
	Raster_Static(const RasterDesc& iRasterDesc, const void* iBaseAddress);
	virtual ~Raster_Static();
	};

// =================================================================================================
#pragma mark - Raster Operations

Pixval sGetPixval(const ZP<Raster>& iRaster, int iH, int iV);
void sSetPixval(const ZP<Raster>& iRaster, int iH, int iV, Pixval iPixval);
void sFill(const ZP<Raster>& iRaster, Pixval iPixval);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Raster_h__
