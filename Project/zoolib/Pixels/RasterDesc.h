// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_RasterDesc_h__
#define __ZooLib_Pixels_RasterDesc_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/PixvalDesc.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - RasterDesc

/** RasterDesc describes the layout of pixel values in a raster. It does not address which
components of a pixel are encoded in which bits of a pixel value, it simply maintains enough
information to determine which cluster of bits correspond to an (h, v) pair. */

class RasterDesc
	{
public:
	RasterDesc() {}

	RasterDesc(const PixvalDesc& iPixvalDesc, size_t iRowBytes, size_t iRowCount, bool iFlipped)
	:	fPixvalDesc(iPixvalDesc), fRowBytes(iRowBytes), fRowCount(iRowCount), fFlipped(iFlipped)
		{}

	bool operator==(const RasterDesc& iOther) const
		{
		return fPixvalDesc == iOther.fPixvalDesc
			&& fRowBytes == iOther.fRowBytes
			&& fRowCount == iOther.fRowCount
			&& fFlipped == iOther.fFlipped;
		}

	PixvalDesc fPixvalDesc;
	size_t fRowBytes;
	size_t fRowCount;
	bool fFlipped;
	};

int sCalcRowBytes(int iWidth, int iDepth, int iByteRound);

const void* sCalcRowAddress(const RasterDesc& iRD, const void* iBaseAddress, int iRow);

void* sCalcRowAddress(const RasterDesc& iRD, void* iBaseAddress, int iRow);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_RasterDesc_h__
