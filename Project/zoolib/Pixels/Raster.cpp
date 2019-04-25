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

Pixval sGetPixval(const ZRef<Raster>& iRaster, int iH, int iV)
	{ return sGetPixval(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iH, iV); }

void sSetPixval(const ZRef<Raster>& iRaster, int iH, int iV, Pixval iPixval)
	{ sSetPixval(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iH, iV, iPixval); }

void sFill(const ZRef<Raster>& iRaster, Pixval iPixval)
	{ return sFill(iRaster->GetRasterDesc(), iRaster->GetBaseAddress(), iPixval); }

} // namespace Pixels
} // namespace ZooLib
