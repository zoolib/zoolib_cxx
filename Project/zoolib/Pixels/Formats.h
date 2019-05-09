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

#ifndef __ZooLib_Pixels_Formats_h__
#define __ZooLib_Pixels_Formats_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/Pixmap.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - EFormatEfficient

/** EFormatEfficient allows one to specify a pixel format that meets representational criteria
without having to determine for oneself what exact format to use. Platform-specific factory
functions actually handle the creation of an appropriate ZDCPixmapRep, one that will be 'efficient'
for that platform, for some definition of efficient. */

enum class EFormatEfficient
	{
	Invalid,

	Gray_1,
	Gray_8,

	Color_16,
	Color_24,
	Color_32
	};

// =================================================================================================
#pragma mark - EFormatStandard

/** EFormatStandard allows one to specify that a pixmap be created with *exactly* the pixel
format required, without having to do the work of setting up a RasterDesc and PixelDesc oneself. */

enum class EFormatStandard
	{
	Invalid,

	Gray_1,
	Gray_2,
	Gray_4,
	Gray_8,

	Alpha_1,
	Alpha_8,

	GA_16,
	AG_16,

	xRGB_16_BE,
	ARGB_16_BE,
	RGBA_16_BE,
	RGB_16_BE,

	xRGB_16_LE,
	ARGB_16_LE,
	RGBA_16_LE,
	RGB_16_LE,

	RGB_24,
	BGR_24,

	xRGB_32,
	ARGB_32,
	xBGR_32,
	ABGR_32,
	RGBx_32,
	RGBA_32,
	BGRx_32,
	BGRA_32
	};

// =================================================================================================
#pragma mark -

EFormatStandard sEfficientToStandard(EFormatEfficient iFormat);

PixvalDesc sPixvalDesc(EFormatStandard iFormat);

RasterDesc sRasterDesc(EFormatStandard iFormat, int iWidth, int iHeight, bool iFlipped);

inline RasterDesc sRasterDesc(EFormatStandard iFormat, int iWidth, int iHeight)
	{ return sRasterDesc(iFormat, iWidth, iHeight, false); }

PixelDesc sPixelDesc(EFormatStandard iFormat);

Pixmap sPixmap(PointPOD iSize, EFormatEfficient iFormat);
Pixmap sPixmap(PointPOD iSize, EFormatEfficient iFormat, const RGBA& iFillColor);

Pixmap sPixmap(PointPOD iSize, EFormatStandard iFormat);
Pixmap sPixmap(PointPOD iSize, EFormatStandard iFormat, const RGBA& iFillColor);

Pixmap sPixmap(const RasterDesc& iRasterDesc, PointPOD iSize, const PixelDesc& iPixelDesc);

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_Formats_h__
