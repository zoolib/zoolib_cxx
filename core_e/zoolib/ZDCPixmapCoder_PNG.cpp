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

#include <vector>

#define PNG_DEPSTRUCT

#include "png.h"

using std::runtime_error;
using std::vector;

namespace ZooLib {

using namespace ZDCPixmapNS;

// =================================================================================================
// MARK: - Static functions

static void spThrowToStream()
	{
	throw runtime_error("ZDCPixmapCoder_PNG, to stream");
	}

static void spPNG_Write(png_structp png_ptr, png_bytep iSource, png_size_t iSize)
	{
	try
		{
		static_cast<ZStreamW*>(png_get_io_ptr(png_ptr))->Write(iSource, iSize);
		return;
		}
	catch (...)
		{}

	longjmp(png_jmpbuf(png_ptr), 1);
	}

static void spPNG_Write_Flush(png_structp png_ptr)
	{
	static_cast<ZStreamW*>(png_get_io_ptr(png_ptr))->Flush();
	}

static void spThrowFromStream()
	{
	throw runtime_error("ZDCPixmapCoder_PNG, from stream");
	}

static void spPNG_Read(png_structp png_ptr, png_bytep iDestAddress, png_size_t iSize)
	{
	try
		{
		static_cast<ZStreamR*>(png_get_io_ptr(png_ptr))->Read(iDestAddress, iSize);
		return;
		}
	catch (...)
		{}

	longjmp(png_jmpbuf(png_ptr), 1);
	}

// =================================================================================================
// MARK: - ZDCPixmapEncoder_PNG

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
	const RasterDesc& iRasterDesc,
	const PixelDesc& iPixelDesc,
	const ZRectPOD& iBounds)
	{
	png_structp write_ptr =
		::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = nullptr;

	vector<uint8> theRowBufferVector;

	try
		{
		info_ptr = ::png_create_info_struct(write_ptr);
		::png_set_write_fn(write_ptr,
			&const_cast<ZStreamW&>(iStream), spPNG_Write, spPNG_Write_Flush);

		PixvalDesc sourcePixvalDesc = iRasterDesc.fPixvalDesc;

		PixvalDesc destPixvalDesc;
		PixelDesc destPixelDesc;

		// This vector has to persist until png_write_info is called.
		vector<png_color> thePNGPaletteVector;

		ZRef<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();

		if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
			thePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			::png_set_IHDR(write_ptr, info_ptr, W(iBounds), H(iBounds), 8,
				PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);

			const ZRGBA_POD* sourceColors;
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
			destPixvalDesc.fBigEndian = true;
			}
		else if (thePixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			int colorType;
			if (thePixelDescRep->HasAlpha())
				{
				colorType = PNG_COLOR_TYPE_RGB_ALPHA;
				destPixelDesc = PixelDesc(eFormatStandard_RGBA_32);
				destPixvalDesc = PixvalDesc(eFormatStandard_RGBA_32);
				}
			else
				{
				colorType = PNG_COLOR_TYPE_RGB;
				destPixelDesc = PixelDesc(eFormatStandard_RGB_24);
				destPixvalDesc = PixvalDesc(eFormatStandard_RGB_24);
				}
			::png_set_IHDR(write_ptr, info_ptr, W(iBounds), H(iBounds), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else if (thePixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			int colorType;
			if (thePixelDescRep->HasAlpha())
				{
				colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
				destPixelDesc = PixelDesc(eFormatStandard_GA_16);
				destPixvalDesc = PixvalDesc(eFormatStandard_GA_16);
				}
			else
				{
				colorType = PNG_COLOR_TYPE_GRAY;
				destPixelDesc = PixelDesc(eFormatStandard_Gray_8);
				destPixvalDesc = PixvalDesc(eFormatStandard_Gray_8);
				}
			::png_set_IHDR(write_ptr, info_ptr, W(iBounds), H(iBounds), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else
			{
			ZUnimplemented();
			}

		theRowBufferVector.resize(W(iBounds) * (destPixvalDesc.fDepth / 8));

		if (setjmp(png_jmpbuf(write_ptr)))
			spThrowToStream();
		::png_write_info(write_ptr, info_ptr);

		uint8* theRowBuffer = &theRowBufferVector[0];
		int numberOfPasses = ::png_set_interlace_handling(write_ptr);
		for (int currentPass = 0; currentPass < numberOfPasses; ++currentPass)
			{
			for (size_t y = iBounds.top; y < iBounds.bottom; ++y)
				{
				const void* sourceRowAddress = iRasterDesc.CalcRowAddress(iBaseAddress, y);
				sBlitRow(
					sourceRowAddress, sourcePixvalDesc, iPixelDesc, iBounds.left,
					theRowBuffer, destPixvalDesc, destPixelDesc, 0,
					W(iBounds));

				if (setjmp(png_jmpbuf(write_ptr)))
					spThrowToStream();
				::png_write_row(write_ptr, theRowBuffer);
				}
			}
		if (setjmp(png_jmpbuf(write_ptr)))
			spThrowToStream();
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
// MARK: - ZDCPixmapDecoder_PNG

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
	png_structp read_ptr =
		::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = nullptr;

	try
		{
		info_ptr = ::png_create_info_struct(read_ptr);
		::png_set_read_fn(read_ptr, &const_cast<ZStreamR&>(iStream), spPNG_Read);

		if (setjmp(png_jmpbuf(read_ptr)))
			spThrowFromStream();

		::png_read_info(read_ptr, info_ptr);

		// Determine the depth we need our pixmap to have
		png_byte thePNGDepth = ::png_get_bit_depth(read_ptr, info_ptr);
		png_byte thePNGColorType = ::png_get_color_type(read_ptr, info_ptr);
		png_uint_32 thePNGWidth = ::png_get_image_width(read_ptr, info_ptr);
		png_uint_32 thePNGHeight = ::png_get_image_height(read_ptr, info_ptr);

		PixelDesc thePixelDesc;
		int32 realDepth;

		switch (thePNGColorType)
			{
			case PNG_COLOR_TYPE_PALETTE:
				{
				// Turn png's palette into something we can understand
				int num_palette;
				png_color* png_palette;
				::png_get_PLTE(read_ptr, info_ptr, &png_palette, &num_palette);

				vector<ZRGBA_POD> theColorTable(num_palette);
				for (size_t x = 0; x < num_palette; ++x)
					{
					theColorTable[x].red = png_palette[x].red * 0x101;
					theColorTable[x].green = png_palette[x].green * 0x101;
					theColorTable[x].blue = png_palette[x].blue * 0x101;
					theColorTable[x].alpha = 0xFFFF;
					}
				thePixelDesc = PixelDesc(&theColorTable[0], theColorTable.size());
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
				thePixelDesc = PixelDesc(maskGray, maskAlpha);
				break;
				}
			case PNG_COLOR_TYPE_RGB:
			case PNG_COLOR_TYPE_RGB_ALPHA:
				{
				// Force 8 bits per component
				if (thePNGDepth == 16)
					{
					thePNGDepth = 8;
					::png_set_strip_16(read_ptr);
					}
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
				thePixelDesc = PixelDesc(maskRed, maskGreen, maskBlue, maskAlpha);
				break;
				}
			}
		::png_read_update_info(read_ptr, info_ptr);

		RasterDesc theRasterDesc;
		theRasterDesc.fPixvalDesc.fDepth = realDepth;
		theRasterDesc.fPixvalDesc.fBigEndian = true;
		theRasterDesc.fRowBytes = sCalcRowBytes(thePNGWidth, realDepth, 4);
		theRasterDesc.fRowCount = thePNGHeight;
		theRasterDesc.fFlipped = false;

		oPixmap = ZDCPixmap(theRasterDesc, sPointPOD(thePNGWidth, thePNGHeight), thePixelDesc);
		void* baseAddress = oPixmap.GetRaster()->GetBaseAddress();
		vector<void*> theRowPointers(thePNGHeight);
		for (size_t y = 0; y < thePNGHeight; ++y)
			theRowPointers[y] = theRasterDesc.CalcRowAddressDest(baseAddress, y);

		if (setjmp(png_jmpbuf(read_ptr)))
			spThrowFromStream();
		::png_read_image(read_ptr, reinterpret_cast<png_byte**>(&theRowPointers[0]));

		if (setjmp(png_jmpbuf(read_ptr)))
			spThrowFromStream();
		::png_read_end(read_ptr, info_ptr);
		}
	catch (...)
		{
		if (read_ptr)
			::png_destroy_read_struct(&read_ptr, &info_ptr, nullptr);
		throw;
		}

	if (read_ptr)
		::png_destroy_read_struct(&read_ptr, &info_ptr, nullptr);
	}

} // namespace ZooLib
