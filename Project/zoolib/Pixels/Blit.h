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

#ifndef __ZooLib_Pixels_Blit_h__
#define __ZooLib_Pixels_Blit_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixmap.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark -

void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval);
void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, const RectPOD& iBounds, uint32 iPixval);

// =================================================================================================
#pragma mark -

void sMunge(void* iBaseAddress,
	const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc, const RectPOD& iBounds,
	MungeProc iMungeProc, void* iRefcon);

// =================================================================================================
#pragma mark -

void sBlitPixvals(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation);

void sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDesc,
	const RectPOD& iSourceBounds, PointPOD iDestLocation);

void sBlitRowPixvals(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, int32 iDestH,
	int32 iCount);

void sBlitRow(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const PixelDesc& iSourcePixelDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const PixelDesc& iDestPixelDesc,
	int32 iDestH,
	int32 iCount);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Blit_h__
