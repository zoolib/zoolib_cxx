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
