/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#include "zoolib/ZDCPixmap_CGImage.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

namespace ZooLib {
namespace ZDCPixmap_CGImage {

using namespace ZDCPixmapNS;

// =================================================================================================
// MARK: - ZDCPixmap_CGImage::PixmapRaster

class PixmapRaster : public ZDCPixmapRaster
	{
public:
	PixmapRaster(const RasterDesc& iRasterDesc,
		const ZRef<CGImageRef>& iImageRef, const ZRef<CFDataRef>& iDataRef)
	:	ZDCPixmapRaster(iRasterDesc)
	,	fImageRef(iImageRef)
	,	fDataRef(iDataRef)
		{
		fBaseAddress = sNonConst(::CFDataGetBytePtr(fDataRef));
		}

	virtual ~PixmapRaster()
		{
		fBaseAddress = nullptr;
		}

private:
	ZRef<CGImageRef> fImageRef;
	ZRef<CFDataRef> fDataRef;
	};

// =================================================================================================
// MARK: - ZDCPixmap_CGImage

ZDCPixmap sPixmap(ZRef<CGImageRef> iImageRef)
	{
	if (not iImageRef)
		{}
	else if (ZRef<CGDataProviderRef,false> theProvider = ::CGImageGetDataProvider(iImageRef))
		{}
	else if (ZRef<CFDataRef,false> theDataRef = sAdopt& ::CGDataProviderCopyData(theProvider))
		{}
	else
		{
//		const CGBitmapInfo theBI = ::CGImageGetBitmapInfo(iImageRef);
//		const CGImageAlphaInfo theAI = ::CGImageGetAlphaInfo(iImageRef);
//		const size_t theBPC = ::CGImageGetBitsPerComponent(iImageRef);
//		const size_t theBPP = ::CGImageGetBitsPerPixel(iImageRef);

//		const OSType thePFT = ::CVPixelBufferGetPixelFormatType(iPBR);
//		const EFormatStandard theFormat = spAsFormatStandard(thePFT);

		// Just fake it out for now.
		const EFormatStandard theFormat = eFormatStandard_RGBA_32;

		const PixvalDesc thePixvalDesc(theFormat);

		const size_t theRowBytes = ::CGImageGetBytesPerRow(iImageRef);

		const size_t theHeight = ::CGImageGetHeight(iImageRef);

		const RasterDesc theRasterDesc(thePixvalDesc, theRowBytes, theHeight, false);

		ZRef<PixmapRaster> thePR = new PixmapRaster(theRasterDesc, iImageRef, theDataRef);

		const PixelDesc thePixelDesc(theFormat);

		const size_t theWidth = ::CGImageGetWidth(iImageRef);

		return ZDCPixmapRep::sCreate(thePR, sRectPOD(theWidth, theHeight), thePixelDesc);
		}

	return ZDCPixmap();
	}

} // namespace ZDCPixmap_CGImage
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
