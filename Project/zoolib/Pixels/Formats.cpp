// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Pixels/Formats.h"
#include "zoolib/Pixels/Cartesian_Geom.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h" // For countof

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - anonymous

namespace { // anonymous

struct Standard2Gray_t
	{
	EFormatStandard fFormat;
	uint32 fMaskGray;
	uint32 fMaskAlpha;
	int fDepth;
	bool fBigEndian;
	};

Standard2Gray_t sStandard2Gray[] =
	{
	{ EFormatStandard::Gray_1, 0x1, 0x0, 1, true },
	{ EFormatStandard::Gray_2, 0x3, 0x0, 2, true },
	{ EFormatStandard::Gray_4, 0xF, 0x0, 4, true },
	{ EFormatStandard::Gray_8, 0xFF, 0x00, 8, true },

	{ EFormatStandard::Alpha_1, 0x0, 0x1, 1, true },
	{ EFormatStandard::Alpha_8, 0x00, 0xFF, 8, true },

#if ZCONFIG(Endian, Big)

	{ EFormatStandard::LA_16, 0xFF00, 0x00FF, 16, true },
	{ EFormatStandard::AL_16, 0x00FF, 0xFF00, 16, true }

#else

	{ EFormatStandard::LA_16, 0x00FF, 0xFF00, 16, false },
	{ EFormatStandard::AL_16, 0xFF00, 0x00FF, 16, false }

#endif
	};

struct Standard2Color_t
	{
	EFormatStandard fFormat;
	uint32 fMaskRed;
	uint32 fMaskGreen;
	uint32 fMaskBlue;
	uint32 fMaskAlpha;
	int fDepth;
	bool fBigEndian;
	};

Standard2Color_t sStandard2Color[] =
	{
	{ EFormatStandard::xRGB_16_BE, 0x7C00, 0x03E0, 0x001F, 0x0000, 16, true },
	{ EFormatStandard::ARGB_16_BE, 0x7C00, 0x03E0, 0x001F, 0x8000, 16, true },
	{ EFormatStandard::RGBA_16_BE, 0xF800, 0x07C0, 0x003E, 0x0001, 16, true },
	{ EFormatStandard::RGB_16_BE, 0xF800, 0x07E0, 0x001F, 0x0000, 16, true },

	{ EFormatStandard::xRGB_16_LE, 0x7C00, 0x03E0, 0x001F, 0x0000, 16, false },
	{ EFormatStandard::ARGB_16_LE, 0x7C00, 0x03E0, 0x001F, 0x8000, 16, false },
	{ EFormatStandard::RGBA_16_LE, 0xF800, 0x07C0, 0x003E, 0x0001, 16, false },
	{ EFormatStandard::RGB_16_LE, 0xF800, 0x07E0, 0x001F, 0x0000, 16, false },

#if ZCONFIG(Endian, Big)

	{ EFormatStandard::RGB_24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 24, true },
	{ EFormatStandard::BGR_24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 24, true },

	{ EFormatStandard::xRGB_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 32, true },
	{ EFormatStandard::ARGB_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 32, true },
	{ EFormatStandard::xBGR_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 32, true },
	{ EFormatStandard::ABGR_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 32, true },
	{ EFormatStandard::RGBx_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000, 32, true },
	{ EFormatStandard::RGBA_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 32, true },
	{ EFormatStandard::BGRx_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000, 32, true },
	{ EFormatStandard::BGRA_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, 32, true }

#else

	{ EFormatStandard::RGB_24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 24, false },
	{ EFormatStandard::BGR_24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 24, false },

	{ EFormatStandard::xRGB_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000, 32, false },
	{ EFormatStandard::ARGB_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, 32, false },
	{ EFormatStandard::xBGR_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000, 32, false },
	{ EFormatStandard::ABGR_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 32, false },
	{ EFormatStandard::RGBx_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 32, false },
	{ EFormatStandard::RGBA_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 32, false },
	{ EFormatStandard::BGRx_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 32, false },
	{ EFormatStandard::BGRA_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 32, false }

#endif
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -

EFormatStandard sEfficientToStandard(EFormatEfficient iFormat)
	{
//	EFormatStandard result;
// 	if (FunctionChain<EFormatStandard, EFormatEfficient>::sInvoke(result, iFormat))
// 		{
// 		return result;
// 		}

	switch (iFormat)
		{
		case EFormatEfficient::Gray_1: return EFormatStandard::Gray_1;
		case EFormatEfficient::Gray_8: return EFormatStandard::Gray_8;
		case EFormatEfficient::Color_16: return EFormatStandard::RGB_16_BE;
		case EFormatEfficient::Color_24: return EFormatStandard::RGB_24;
		case EFormatEfficient::Color_32: return EFormatStandard::ARGB_32;
		default: break;
		}

	return EFormatStandard::Invalid;
	}

// =================================================================================================
#pragma mark -

PixvalDesc sPixvalDesc(EFormatStandard iFormat)
	{
	for (size_t xx = 0; xx < countof(sStandard2Color); ++xx)
		{
		const Standard2Color_t& entry = sStandard2Color[xx];
		if (entry.fFormat == iFormat)
			return PixvalDesc(entry.fDepth, entry.fBigEndian);
		}

	for (size_t xx = 0; xx < countof(sStandard2Gray); ++xx)
		{
		const Standard2Gray_t& entry = sStandard2Gray[xx];
		if (entry.fFormat == iFormat)
			return PixvalDesc(entry.fDepth, entry.fBigEndian);
		}

	ZUnimplemented();
	}

// =================================================================================================
#pragma mark -

RasterDesc sRasterDesc(EFormatStandard iFormat, int iWidth, int iHeight, bool iFlipped)
	{
	for (size_t xx = 0; xx < countof(sStandard2Color); ++xx)
		{
		const Standard2Color_t& entry = sStandard2Color[xx];
		if (entry.fFormat == iFormat)
			{
			return RasterDesc(PixvalDesc(entry.fDepth, entry.fBigEndian),
				sCalcRowBytes(iWidth, entry.fDepth, 4),
				iHeight,
				iFlipped);
			}
		}

	for (size_t xx = 0; xx < countof(sStandard2Gray); ++xx)
		{
		const Standard2Gray_t& entry = sStandard2Gray[xx];
		if (entry.fFormat == iFormat)
			{
			return RasterDesc(PixvalDesc(entry.fDepth, entry.fBigEndian),
				sCalcRowBytes(iWidth, entry.fDepth, 4),
				iHeight,
				iFlipped);
			}
		}

	ZUnimplemented();
	}

// =================================================================================================
#pragma mark -

PixelDesc sPixelDesc(EFormatStandard iFormat)
	{
	for (size_t xx = 0; xx < countof(sStandard2Color); ++xx)
		{
		if (sStandard2Color[xx].fFormat == iFormat)
			{
			return sPixelDesc(
				sStandard2Color[xx].fMaskRed,
				sStandard2Color[xx].fMaskGreen,
				sStandard2Color[xx].fMaskBlue,
				sStandard2Color[xx].fMaskAlpha);
			}
		}

	for (size_t xx = 0; xx < countof(sStandard2Gray); ++xx)
		{
		if (sStandard2Gray[xx].fFormat == iFormat)
			{
			return sPixelDesc(
				sStandard2Gray[xx].fMaskGray,
				sStandard2Gray[xx].fMaskAlpha);
			}
		}

	ZUnimplemented();
	}

Pixmap sPixmap(PointPOD iSize, EFormatEfficient iFormat)
	{ return sPixmap(iSize, sEfficientToStandard(iFormat)); }

Pixmap sPixmap(PointPOD iSize, EFormatEfficient iFormat, const RGBA& iFillColor)
	{ return sPixmap(iSize, sEfficientToStandard(iFormat), iFillColor); }

Pixmap sPixmap(PointPOD iSize, EFormatStandard iFormat)
	{ return sPixmapRep(sRasterDesc(iFormat, X(iSize), Y(iSize)), iSize, sPixelDesc(iFormat)); }

Pixmap sPixmap(PointPOD iSize, EFormatStandard iFormat, const RGBA& iFillColor)
	{
	Pixmap thePixmap = sPixmap(iSize, iFormat);

	uint32 fillPixval = thePixmap.GetPixelDesc().AsPixval(iFillColor);

	sFill(thePixmap.GetRaster(), fillPixval);

	return thePixmap;
	}

} // namespace Pixels
} // namespace ZooLib
