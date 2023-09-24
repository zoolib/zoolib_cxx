// Copyright (c) 2012 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Pixmap_CGImage.h"

#if ZCONFIG_SPI_Enabled(CoreGraphics)

namespace ZooLib {

using namespace Pixels;

// =================================================================================================
#pragma mark - PixmapRaster

class Raster_CGImage : public Raster
	{
public:
	Raster_CGImage(const RasterDesc& iRasterDesc,
		const ZP<CGImageRef>& iImageRef, const ZP<CFDataRef>& iDataRef)
	:	Raster(iRasterDesc)
	,	fImageRef(iImageRef)
	,	fDataRef(iDataRef)
		{ fBaseAddress = sNonConst(::CFDataGetBytePtr(fDataRef)); }

	virtual ~Raster_CGImage()
		{ fBaseAddress = nullptr; }

private:
	ZP<CGImageRef> fImageRef;
	ZP<CFDataRef> fDataRef;
	};

// =================================================================================================
#pragma mark - sPixmap

Pixmap sPixmap(ZP<CGImageRef> iImageRef)
	{
	if (iImageRef)
		{
		if (ZP<CGDataProviderRef> theProvider = ::CGImageGetDataProvider(iImageRef))
			{
			if (ZP<CFDataRef> theDataRef = sAdopt& ::CGDataProviderCopyData(theProvider))
				{
				const PixvalDesc thePixvalDesc(
					int(::CGImageGetBitsPerPixel(iImageRef)), ZCONFIG_Endian != ZCONFIG_Endian_Big);

				const RasterDesc theRasterDesc(
					thePixvalDesc,
					::CGImageGetBytesPerRow(iImageRef),
					::CGImageGetHeight(iImageRef),
					false);

				ZP<Raster> theRaster = new Raster_CGImage(theRasterDesc, iImageRef, theDataRef);

				// Figure out mask
				const size_t theBPC = ::CGImageGetBitsPerComponent(iImageRef);

				const CGImageAlphaInfo theAI = ::CGImageGetAlphaInfo(iImageRef);

				const uint32 theMask = (1 << theBPC) - 1;
				uint32 theStart = theMask;
				uint32 theA;
				if (theAI & 1)
					{
					theA = theMask;
					theStart = theMask << theBPC;
					}
				else if (0 == (theAI & 6) || 6 == (theAI & 6))
					{
					theA = 0;
					}
				else
					{
					theA = theMask << (theBPC * 3);
					}

				const uint32 theB = theStart;
				const uint32 theG = theB << theBPC;
				const uint32 theR = theG << theBPC;

				return Pixmap(new PixmapRep(theRaster,
					sRectPOD(Ord(::CGImageGetWidth(iImageRef)), Ord(theRaster->GetRasterDesc().fRowCount)),
					sPixelDesc(theR, theG, theB, theA)));
				}
			}
		}

	return Pixmap();
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreGraphics)
