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

#ifndef __ZooLib_Pixels_Raster_h__
#define __ZooLib_Pixels_Raster_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"

#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/RasterDesc.h"

#include <vector>

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - Raster

class Raster
:	public ZCounted
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

void sFresh(ZRef<Raster>& ioRaster);

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

Pixval sGetPixval(const ZRef<Raster>& iRaster, int iH, int iV);
void sSetPixval(const ZRef<Raster>& iRaster, int iH, int iV, Pixval iPixval);
void sFill(const ZRef<Raster>& iRaster, Pixval iPixval);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Raster_h__
