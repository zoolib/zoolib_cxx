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

#include "zoolib/Pixels/PixmapCoder_PNG.h"

#include "zoolib/Pixels/Formats.h"

#include <vector>

#define PNG_DEPSTRUCT

#include "png.h"

using std::runtime_error;
using std::vector;

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - Static functions

//static void spThrowToStream()
//	{
//	throw runtime_error("ZDCPixmapCoder_PNG, to stream");
//	}

//static void spPNG_Write(png_structp png_ptr, png_bytep iSource, png_size_t iSize)
//	{
//	try
//		{
//		sEWrite(*static_cast<ChanW_Bin*>(png_get_io_ptr(png_ptr)), iSource, iSize);
//		return;
//		}
//	catch (...)
//		{}
//
//	longjmp(png_jmpbuf(png_ptr), 1);
//	}
//
//static void spPNG_Write_Flush(png_structp png_ptr)
//	{
//	static_cast<ChanW_Bin*>(png_get_io_ptr(png_ptr))->Flush();
//	}
//
static void spThrowFromStream()
	{
	throw runtime_error("ZDCPixmapCoder_PNG, from stream");
	}

static void spPNG_Read(png_structp png_ptr, png_bytep iDestAddress, png_size_t iSize)
	{
	try
		{
		sERead(*static_cast<ChanR_Bin*>(png_get_io_ptr(png_ptr)), iDestAddress, iSize);
		return;
		}
	catch (...)
		{}

	longjmp(png_jmpbuf(png_ptr), 1);
	}

// =================================================================================================
#pragma mark - sReadPixmap_PNG

Pixmap sReadPixmap_PNG(const ChanR_Bin& iStream)
	{
	Pixmap thePixmap;

	png_structp read_ptr =
		::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = nullptr;

	try
		{
		info_ptr = ::png_create_info_struct(read_ptr);
		::png_set_read_fn(read_ptr, &const_cast<ChanR_Bin&>(iStream), spPNG_Read);

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

				vector<RGBA> theColorTable(num_palette);
				for (int xx = 0; xx < num_palette; ++xx)
					{
					theColorTable[xx] = sRGBA(
						png_palette[xx].red / 255.0,
						png_palette[xx].green / 255.0,
						png_palette[xx].blue / 255.0,
						1.0);
					}
				thePixelDesc = sPixelDesc(&theColorTable[0], theColorTable.size());
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
				thePixelDesc = sPixelDesc(maskGray, maskAlpha);
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
				thePixelDesc = sPixelDesc(maskRed, maskGreen, maskBlue, maskAlpha);
				break;
				}
			default:
				{
				ZUnimplemented();
				}
			}
		::png_read_update_info(read_ptr, info_ptr);

		RasterDesc theRasterDesc;
		theRasterDesc.fPixvalDesc.fDepth = realDepth;
		theRasterDesc.fPixvalDesc.fBigEndian = true;
		theRasterDesc.fRowBytes = sCalcRowBytes(thePNGWidth, realDepth, 4);
		theRasterDesc.fRowCount = thePNGHeight;
		theRasterDesc.fFlipped = false;

		thePixmap = sPixmap(theRasterDesc, sPointPOD(thePNGWidth, thePNGHeight), thePixelDesc);
		void* baseAddress = thePixmap.GetRaster()->GetBaseAddress();
		vector<void*> theRowPointers(thePNGHeight);
		for (size_t y = 0; y < thePNGHeight; ++y)
			theRowPointers[y] = sCalcRowAddress(theRasterDesc, baseAddress, y);

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

	return thePixmap;
	}

} // namespace Pixels
} // namespace ZooLib
