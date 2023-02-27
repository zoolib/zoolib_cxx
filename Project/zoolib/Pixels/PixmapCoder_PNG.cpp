// Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/Pixels/PixmapCoder_PNG.h"

#include "zoolib/Pixels/Blit.h"
#include "zoolib/Pixels/Cartesian_Geom.h"
#include "zoolib/Pixels/Formats.h"

#include <vector>

#define PNG_DEPSTRUCT

#include "png.h"

using std::runtime_error;
using std::vector;

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - sReadPixmap_PNG

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

Pixmap sReadPixmap_PNG(const ChanR_Bin& iChanR)
	{
	Pixmap thePixmap;

	png_structp read_ptr =
		::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = nullptr;

	try
		{
		info_ptr = ::png_create_info_struct(read_ptr);
		::png_set_read_fn(read_ptr, &const_cast<ChanR_Bin&>(iChanR), spPNG_Read);

		if (setjmp(png_jmpbuf(read_ptr)))
			sThrow_ExhaustedR();

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
			sThrow_ExhaustedR();
		::png_read_image(read_ptr, reinterpret_cast<png_byte**>(&theRowPointers[0]));

		if (setjmp(png_jmpbuf(read_ptr)))
			sThrow_ExhaustedR();
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

// =================================================================================================
#pragma mark - sWritePixmap_PNG

static void spPNG_Write(png_structp png_ptr, png_bytep iSource, png_size_t iSize)
	{
	try
		{
		sEWrite(*static_cast<ChanW_Bin*>(png_get_io_ptr(png_ptr)), iSource, iSize);
		return;
		}
	catch (...)
		{}

	longjmp(png_jmpbuf(png_ptr), 1);
	}

static void spPNG_Write_Flush(png_structp png_ptr)
	{
	sFlush(*static_cast<ChanW_Bin*>(png_get_io_ptr(png_ptr)));
	}

void sWritePixmap_PNG(
	const void* iBaseAddress,
	const RasterDesc& iRasterDesc,
	const PixelDesc& iPixelDesc,
	const RectPOD& iFrame,
	const ChanW_Bin& iChanW)
	{
	png_structp write_ptr =
		::png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	png_infop info_ptr = nullptr;

	vector<uint8> theRowBufferVector;

	try
		{
		info_ptr = ::png_create_info_struct(write_ptr);
		::png_set_write_fn(write_ptr,
			&const_cast<ChanW_Bin&>(iChanW), spPNG_Write, spPNG_Write_Flush);

		// const PixvalDesc sourcePixvalDesc = iRasterDesc.fPixvalDesc;

		PixvalDesc destPixvalDesc;
		PixelDesc destPixelDesc;

		// This vector has to persist until png_write_info is called.
		vector<png_color> thePNGPaletteVector;

		ZP<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();

		if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
			thePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			::png_set_IHDR(write_ptr, info_ptr, W(iFrame), H(iFrame), 8,
				PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);

			const RGBA* sourceColors;
			size_t sourceColorCount;
			thePixelDescRep_Indexed->GetColors(sourceColors, sourceColorCount);

			thePNGPaletteVector.resize(sourceColorCount);
			for (size_t x = 0; x < sourceColorCount; ++x)
				{
				thePNGPaletteVector[x].red = 0.5 + 255 * sRed(sourceColors[x]);
				thePNGPaletteVector[x].green = 0.5 + 255 * sGreen(sourceColors[x]);
				thePNGPaletteVector[x].blue = 0.5 + 255 * sBlue(sourceColors[x]);
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
				destPixelDesc = sPixelDesc(EFormatStandard::RGBA_32);
				destPixvalDesc = sPixvalDesc(EFormatStandard::RGBA_32);
				}
			else
				{
				colorType = PNG_COLOR_TYPE_RGB;
				destPixelDesc = sPixelDesc(EFormatStandard::RGB_24);
				destPixvalDesc = sPixvalDesc(EFormatStandard::RGB_24);
				}
			::png_set_IHDR(write_ptr, info_ptr, W(iFrame), H(iFrame), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else if (thePixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			int colorType;
			if (thePixelDescRep->HasAlpha())
				{
				colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
				destPixelDesc = sPixelDesc(EFormatStandard::GA_16);
				destPixvalDesc = sPixvalDesc(EFormatStandard::GA_16);
				}
			else
				{
				colorType = PNG_COLOR_TYPE_GRAY;
				destPixelDesc = sPixelDesc(EFormatStandard::Gray_8);
				destPixvalDesc = sPixvalDesc(EFormatStandard::Gray_8);
				}
			::png_set_IHDR(write_ptr, info_ptr, W(iFrame), H(iFrame), 8,
				colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
			}
		else
			{
			ZUnimplemented();
			}

		theRowBufferVector.resize(W(iFrame) * (destPixvalDesc.fDepth / 8));

		RasterDesc destRD(destPixvalDesc, theRowBufferVector.size(), 1, false);
		const RectPOD destFrame = sRectPOD(0, 0, W(iFrame), 1);

		const RectPOD sourceSingleRowFrame = sRectPOD(iFrame.left, 0, iFrame.right, 1);

		if (setjmp(png_jmpbuf(write_ptr)))
			sThrow_ExhaustedW();
		::png_write_info(write_ptr, info_ptr);

		uint8* theRowBuffer = &theRowBufferVector[0];
		int numberOfPasses = ::png_set_interlace_handling(write_ptr);
		for (int currentPass = 0; currentPass < numberOfPasses; ++currentPass)
			{
			for (Ord y = iFrame.top; y < iFrame.bottom; ++y)
				{
				const void* sourceRowAddress = sCalcRowAddress(iRasterDesc, iBaseAddress, y);

				sCopy(iRasterDesc, sourceRowAddress, sourceSingleRowFrame, iPixelDesc,
					destRD, theRowBuffer, destFrame, destPixelDesc,
					eOp_Copy);

				if (setjmp(png_jmpbuf(write_ptr)))
					sThrow_ExhaustedW();
				::png_write_row(write_ptr, theRowBuffer);
				}
			}
		if (setjmp(png_jmpbuf(write_ptr)))
			sThrow_ExhaustedW();
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

void sWritePixmap_PNG(const Pixmap& iPixmap, const ChanW_Bin& iChanW)
	{
	sWritePixmap_PNG(
		iPixmap.GetBaseAddress(),
		iPixmap.GetRasterDesc(),
		iPixmap.GetPixelDesc(),
		iPixmap.GetFrame(),
		iChanW);
	}

} // namespace Pixels
} // namespace ZooLib
