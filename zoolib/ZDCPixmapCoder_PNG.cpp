/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZDCPixmapCoder_PNG.h"

#if ZCONFIG_SPI_Enabled(libpng)

#include <vector>
#include <png.h>

NAMESPACE_ZOOLIB_USING

using std::runtime_error;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Static functions

static void sThrowToStream()
	{
	throw runtime_error("ZDCPixmapCoder_PNG, to stream");
	}

static void sPNG_Write(png_structp png_ptr, png_bytep inDestAddress, png_size_t inSize)
	{
	static_cast<ZStreamW*>(png_ptr->io_ptr)->Write(inDestAddress, inSize);
	}

static void sPNG_Write_Flush(png_structp png_ptr)
	{
	static_cast<ZStreamW*>(png_ptr->io_ptr)->Flush();
	}

static void sThrowFromStream()
	{
	throw runtime_error("ZDCPixmapCoder_PNG, from stream");
	}

static void sPNG_Read(png_structp png_ptr, png_bytep inDestAddress, png_size_t inSize)
	{
	static_cast<ZStreamR*>(png_ptr->io_ptr)->Read(inDestAddress, inSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapEncoder_PNG

void ZDCPixmapEncoder_PNG::sWritePixmap(const ZStreamW& iStream, const ZDCPixmap& iPixmap)
	{
	ZDCPixmapEncoder_PNG theEncoder;
	ZDCPixmapEncoder::sWritePixmap(iStream, iPixmap, theEncoder); 
	}

ZDCPixmapEncoder_PNG::ZDCPixmapEncoder_PNG()
	{}

ZDCPixmapEncoder_PNG::~ZDCPixmapEncoder_PNG()
	{}

void ZDCPixmapEncoder_PNG::Imp_Write(const ZStreamW& iStream,
	const void* iBaseAddress,
	const ZDCPixmapNS::RasterDesc& iRasterDesc,
	const ZDCPixmapNS::PixelDesc& iPixelDesc,
	const ZRect& iBounds)
	{
	png_structp write_ptr = ::png_create_write_struct(PNG_LIBPNG_VER_STRING, nil, nil, nil);
	png_infop info_ptr = nil;

	vector<uint8> theRowBufferVector;

	try
		{
		info_ptr = ::png_create_info_struct(write_ptr);
		::png_set_write_fn(write_ptr,
			&const_cast<ZStreamW&>(iStream), sPNG_Write, sPNG_Write_Flush);

		ZDCPixmapNS::PixvalDesc sourcePixvalDesc = iRasterDesc.fPixvalDesc;

		ZDCPixmapNS::PixvalDesc destPixvalDesc;
		ZDCPixmapNS::PixelDesc destPixelDesc;
		destPixvalDesc.fBigEndian = true;

		// This vector has to persist until png_write_info is called.
		vector<png_color> thePNGPaletteVector;

		ZRef<ZDCPixmapNS::PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();

		if (ZDCPixmapNS::PixelDescRep_Indexed* thePixelDescRep_Indexed
			= ZRefDynamicCast<ZDCPixmapNS::PixelDescRep_Indexed>(thePixelDescRep))
			{
			::png_set_IHDR(write_ptr, info_ptr, iBounds.Width(), iBounds.Height(), 8,
				PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);

			const ZRGBColorPOD* sourceColors;
			size_t sourceColorCount;
			thePixelDescRep_Indexed->GetColors(sourceColors, sourceColorCount);

			thePNGPaletteVector.resize(sourceColorCount);
			for (size_t x = 0; x < sourceColorCount; ++x)
				{
				thePNGPaletteVector[x].red = sourceColors[x].red >> 8;
				thePNGPaletteVector[x].green = sourceColors[x].green >> 8;
				thePNGPaletteVector[x].blue = sourceColors[x].blue >> 8;
				}
			::png_set_PLTE(write_ptr, info_ptr, &thePNGPaletteVector[0], sourceColorCount);

			destPixelDesc = iPixelDesc;
			destPixvalDesc.fDepth = 8;
			}
		else if (ZRefDynamicCast<ZDCPixmapNS::PixelDescRep_Color>(thePixelDescRep))
			{
			int colorType;
			if (thePixelDescRep->HasAlpha())
				{
				colorType = PNG_COLOR_TYPE_RGB_ALPHA;
				destPixelDesc = ZDCPixmapNS::PixelDesc(ZDCPixmapNS::eFormatStandard_RGBA_32);
				destPixvalDesc.fDepth = 32;
				}
			else
				{
				colorType = PNG_COLOR_TYPE_RGB;
				destPixelDesc = ZDCPixmapNS::PixelDesc(ZDCPixmapNS::eFormatStandard_RGB_24);
				destPixvalDesc.fDepth = 24;
				}
			::png_set_IHDR(write_ptr, info_ptr, iBounds.Width(), iBounds.Height(), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else if (ZRefDynamicCast<ZDCPixmapNS::PixelDescRep_Gray>(thePixelDescRep))
			{
			int colorType;
			if (thePixelDescRep->HasAlpha())
				{
				colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
				destPixelDesc = ZDCPixmapNS::PixelDesc(ZDCPixmapNS::eFormatStandard_GA_16);
				destPixvalDesc.fDepth = 16;
				}
			else
				{
				colorType = PNG_COLOR_TYPE_GRAY;
				destPixelDesc = ZDCPixmapNS::PixelDesc(ZDCPixmapNS::eFormatStandard_Gray_8);
				destPixvalDesc.fDepth = 8;
				}
			::png_set_IHDR(write_ptr, info_ptr, iBounds.Width(), iBounds.Height(), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else
			{
			ZUnimplemented();
			}

		theRowBufferVector.resize(iBounds.Width() * (destPixvalDesc.fDepth / 8));

		if (setjmp(write_ptr->jmpbuf))
			sThrowToStream();
		::png_write_info(write_ptr, info_ptr);

		uint8* theRowBuffer = &theRowBufferVector[0];
		int numberOfPasses = ::png_set_interlace_handling(write_ptr);
		for (int currentPass = 0; currentPass < numberOfPasses; ++currentPass)
			{
			for (size_t y = iBounds.top; y < iBounds.bottom; ++y)
				{
				const void* sourceRowAddress = iRasterDesc.CalcRowAddress(iBaseAddress, y);
				ZDCPixmapNS::sBlitRow(
					sourceRowAddress, sourcePixvalDesc, iPixelDesc, iBounds.left,
					theRowBuffer, destPixvalDesc, destPixelDesc, 0,
					iBounds.Width());

				if (setjmp(write_ptr->jmpbuf))
					sThrowToStream();
				::png_write_row(write_ptr, theRowBuffer);
				}
			}
		if (setjmp(write_ptr->jmpbuf))
			sThrowToStream();
		::png_write_end(write_ptr, info_ptr);
		}
	catch (...)
		{
		if (write_ptr)
			::png_destroy_write_struct(&write_ptr, &info_ptr);
		throw;
		}

	if (write_ptr)
		::png_destroy_write_struct(&write_ptr, &info_ptr);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapDecoder_PNG

ZDCPixmap ZDCPixmapDecoder_PNG::sReadPixmap(const ZStreamR& iStream)
	{
	ZDCPixmapDecoder_PNG theDecoder;
	return ZDCPixmapDecoder::sReadPixmap(iStream, theDecoder);
	}

ZDCPixmapDecoder_PNG::ZDCPixmapDecoder_PNG()
	{}

ZDCPixmapDecoder_PNG::~ZDCPixmapDecoder_PNG()
	{}

void ZDCPixmapDecoder_PNG::Imp_Read(const ZStreamR& iStream, ZDCPixmap& oPixmap)
	{
	png_structp read_ptr = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nil, nil, nil);
	png_infop info_ptr = nil;

	try
		{
		info_ptr = ::png_create_info_struct(read_ptr);
		::png_set_read_fn(read_ptr, &const_cast<ZStreamR&>(iStream), sPNG_Read);

		if (setjmp(read_ptr->jmpbuf))
			sThrowFromStream();

		::png_read_info(read_ptr, info_ptr);

		// Determine the depth we need our pixmap to have
		png_byte thePNGDepth = ::png_get_bit_depth(read_ptr, info_ptr);
		png_byte thePNGColorType = ::png_get_color_type(read_ptr, info_ptr);
		png_uint_32 thePNGWidth = ::png_get_image_width(read_ptr, info_ptr);
		png_uint_32 thePNGHeight = ::png_get_image_height(read_ptr, info_ptr);

		ZDCPixmapNS::PixelDesc thePixelDesc;
		int32 realDepth;

		switch (thePNGColorType)
			{
			case PNG_COLOR_TYPE_PALETTE:
				{
				// Turn png's palette into something we can understand
				vector<ZRGBColorPOD> theColorTable(info_ptr->num_palette);
				for (size_t x = 0; x < info_ptr->num_palette; ++x)
					{
					theColorTable[x].red = info_ptr->palette[x].red * 0x101;
					theColorTable[x].green = info_ptr->palette[x].green * 0x101;
					theColorTable[x].blue = info_ptr->palette[x].blue * 0x101;
					theColorTable[x].alpha = 0xFFFF;
					}
				thePixelDesc = ZDCPixmapNS::PixelDesc(&theColorTable[0], theColorTable.size());
				realDepth = thePNGDepth;
				break;
				}
			case PNG_COLOR_TYPE_GRAY:
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				{
				uint32 maskGray;
				uint32 maskAlpha;
				if (thePNGColorType & PNG_COLOR_MASK_ALPHA)
					{
					maskAlpha = (1 << thePNGDepth) - 1;
					maskGray = maskAlpha << thePNGDepth;
					realDepth = thePNGDepth * 2;
					}
				else
					{
					maskAlpha = 0;
					maskGray = (1 << thePNGDepth) - 1;
					realDepth = thePNGDepth;
					}
				thePixelDesc = ZDCPixmapNS::PixelDesc(maskGray, maskAlpha);
				break;
				}
			case PNG_COLOR_TYPE_RGB:
			case PNG_COLOR_TYPE_RGB_ALPHA:
				{
				// Force 8 bits per component
				::png_set_strip_16(read_ptr);
				// Build the component masks
				uint32 maskAlpha;
				uint32 maskBlue;
				if (thePNGColorType & PNG_COLOR_MASK_ALPHA)
					{
					maskAlpha = (1 << thePNGDepth) - 1;
					maskBlue = maskAlpha << thePNGDepth;
					realDepth = thePNGDepth * 4;
					}
				else
					{
					maskAlpha = 0;
					maskBlue = (1 << thePNGDepth) - 1;
					realDepth = thePNGDepth * 3;
					}
				uint32 maskGreen = maskBlue << thePNGDepth;
				uint32 maskRed = maskGreen << thePNGDepth;
				thePixelDesc = ZDCPixmapNS::PixelDesc(maskRed, maskGreen, maskBlue, maskAlpha);
				break;
				}
			}
		::png_read_update_info(read_ptr, info_ptr);

		ZDCPixmapNS::RasterDesc theRasterDesc;
		theRasterDesc.fPixvalDesc.fDepth = realDepth;
		theRasterDesc.fPixvalDesc.fBigEndian = true;
		theRasterDesc.fRowBytes = ZDCPixmapNS::sCalcRowBytes(thePNGWidth, realDepth, 4);
		theRasterDesc.fRowCount = thePNGHeight;
		theRasterDesc.fFlipped = false;

		oPixmap = ZDCPixmap(theRasterDesc, ZPoint(thePNGWidth, thePNGHeight), thePixelDesc);
		void* baseAddress = oPixmap.GetRaster()->GetBaseAddress();
		vector<void*> theRowPointers(thePNGHeight);
		for (size_t y = 0; y < thePNGHeight; ++y)
			theRowPointers[y] = theRasterDesc.CalcRowAddressDest(baseAddress, y);

		if (setjmp(read_ptr->jmpbuf))
			sThrowFromStream();
		::png_read_image(read_ptr, reinterpret_cast<png_byte**>(&theRowPointers[0]));

		if (setjmp(read_ptr->jmpbuf))
			sThrowFromStream();
		::png_read_end(read_ptr, info_ptr);
		}
	catch (...)
		{
		if (read_ptr)
			::png_destroy_read_struct(&read_ptr, &info_ptr, nil);
		throw;
		}

	if (read_ptr)
		::png_destroy_read_struct(&read_ptr, &info_ptr, nil);
	}

#endif // ZCONFIG_SPI_Enabled(libpng)
