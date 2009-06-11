/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZDCPixmap_CoreVideo.h"

#if ZCONFIG_SPI_Enabled(MacOSX)

#if !defined(MAC_OS_X_VERSION_10_5) || MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_5
	// Target OS is pre-10.5, so we don't have the kCVPixelFormatType_XXX enums.
	#include ZMACINCLUDE3(ApplicationServices,QD,QuickDraw.h)
	#define CVPIXELFORMAT(a) k##a##PixelFormat
#else
	#define CVPIXELFORMAT(a) kCVPixelFormatType_##a
#endif

NAMESPACE_ZOOLIB_BEGIN

namespace ZDCPixmap_CoreVideo {

using namespace ZDCPixmapNS;

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmap_CoreVideo::PixmapRaster

class PixmapRaster : public ZDCPixmapRaster
	{
public:
	PixmapRaster(const RasterDesc& iRasterDesc, ZRef<CVPixelBufferRef> iPBR);
	virtual ~PixmapRaster();

private:
	ZRef<CVPixelBufferRef> fPBR;
	};

PixmapRaster::PixmapRaster(const RasterDesc& iRasterDesc, ZRef<CVPixelBufferRef> iPBR)
:	ZDCPixmapRaster(iRasterDesc)
,	fPBR(iPBR)
	{
	::CVPixelBufferLockBaseAddress(fPBR, 0);
	fBaseAddress = ::CVPixelBufferGetBaseAddress(iPBR);
	}

PixmapRaster::~PixmapRaster()
	{
	::CVPixelBufferUnlockBaseAddress(fPBR, 0);
	fBaseAddress = nullptr;
	}

EFormatStandard sAsFormatStandard(OSType iOSType)
	{
	switch (iOSType)
		{
		case CVPIXELFORMAT(16BE555): return eFormatStandard_xRGB_16_BE;
		case CVPIXELFORMAT(16LE555): return eFormatStandard_xRGB_16_LE;
		case CVPIXELFORMAT(16LE5551): return eFormatStandard_ARGB_16_BE;
		case CVPIXELFORMAT(16BE565): return eFormatStandard_RGB_16_BE;
		case CVPIXELFORMAT(16LE565): return eFormatStandard_RGB_16_LE;

		case CVPIXELFORMAT(24RGB): return eFormatStandard_RGB_24;
		case CVPIXELFORMAT(24BGR): return eFormatStandard_BGR_24;

		case CVPIXELFORMAT(32ARGB): return eFormatStandard_ARGB_32;
		case CVPIXELFORMAT(32BGRA): return eFormatStandard_BGRA_32;
		case CVPIXELFORMAT(32ABGR): return eFormatStandard_ABGR_32;
		case CVPIXELFORMAT(32RGBA): return eFormatStandard_RGBA_32;
		}
	return eFormatStandard_Invalid;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmap_CoreVideo

ZDCPixmap sPixmap(ZRef<CVPixelBufferRef> iPBR)
	{
	const OSType thePFT = ::CVPixelBufferGetPixelFormatType(iPBR);
	const EFormatStandard theFormat = sAsFormatStandard(thePFT);

	const size_t theRowBytes = ::CVPixelBufferGetBytesPerRow(iPBR);
	const size_t theHeight = ::CVPixelBufferGetHeight(iPBR);

	const PixvalDesc thePixvalDesc(theFormat);
	const RasterDesc theRasterDesc(thePixvalDesc, theRowBytes, theHeight, false);

	ZRef<PixmapRaster> thePMR = new PixmapRaster(theRasterDesc, iPBR);

	const size_t theWidth = ::CVPixelBufferGetWidth(iPBR);

	const PixelDesc thePixelDesc(theFormat);

	return ZDCPixmapRep::sCreate(thePMR, ZRect(theWidth, theHeight), thePixelDesc);
	}

} // namespace ZDCPixmap_CoreVideo

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(MacOSX)
