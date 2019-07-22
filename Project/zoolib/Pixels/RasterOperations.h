// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_RasterOperations_h__
#define __ZooLib_Pixels_RasterOperations_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Geom.h"
#include "zoolib/Pixels/Pixval.h"
#include "zoolib/Pixels/RasterDesc.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

Pixval sGetPixval(const RasterDesc& iRD, const void* iBaseAddress, int iH, int iV);

void sSetPixval(const RasterDesc& iRD, void* oBaseAddress, int iH, int iV, Pixval iPixval);

void sFill(const RasterDesc& iRD, void* oBaseAddress, Pixval iPixval);

void sFill(const RasterDesc& iRD, void* oBaseAddress, const RectPOD& iBounds, Pixval iPixval);

void sBlitRowPixvals(
	const PixvalDesc& iSourcePvD, const void* iSourceBase, int iSourceH,
	const PixvalDesc& iDestPvD, void* oDestBase, int iDestH,
	int iCount);

void sBlitPixvals(
	const RasterDesc& iSourceRD, const void* iSourceBase, const RectPOD& iSourceBounds,
	const RasterDesc& iDestRD, void* oDestBase, const PointPOD& iDestLocation);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_RasterOperations_h__
