// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/Raster.h"
#include "zoolib/Pixels/RasterOperations.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - Raster

Raster::Raster(const RasterDesc& iRasterDesc)
:	fRasterDesc(iRasterDesc)
,	fMutable(false)
,	fBaseAddress(nullptr)
	{}

Raster::~Raster()
	{
	// Sanity check. Subclasses are required to nil out our fBaseAddress field.
	// This helps to ensure that they have a proper constructor by effectively
	// requiring that they have a destructor.
	ZAssert(fBaseAddress == nullptr);
	}

// =================================================================================================
#pragma mark - Raster_Simple

Raster_Simple::Raster_Simple(const RasterDesc& iRasterDesc)
:	Raster(iRasterDesc)
,	fBuffer(fRasterDesc.fRowBytes * fRasterDesc.fRowCount + 4, 0)
	{
	fMutable = true;
	fBaseAddress = &fBuffer[0];	
	}

Raster_Simple::~Raster_Simple()
	{ fBaseAddress = nullptr; }

// =================================================================================================
#pragma mark - Raster_Static

Raster_Static::Raster_Static(const RasterDesc& iRasterDesc, const void* iBaseAddress)
:	Raster(iRasterDesc)
	{
	fBaseAddress = const_cast<void*>(iBaseAddress);
	}

Raster_Static::~Raster_Static()
	{ fBaseAddress = nullptr; }

// =================================================================================================
#pragma mark - Raster Operations

Pixval sGetPixval(const ZP<Raster>& iRaster, int iH, int iV)
	{ return sGetPixval(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iH, iV); }

void sSetPixval(const ZP<Raster>& iRaster, int iH, int iV, Pixval iPixval)
	{ sSetPixval(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iH, iV, iPixval); }

void sFill(const ZP<Raster>& iRaster, Pixval iPixval)
	{ return sFill(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iPixval); }

} // namespace Pixels
} // namespace ZooLib
