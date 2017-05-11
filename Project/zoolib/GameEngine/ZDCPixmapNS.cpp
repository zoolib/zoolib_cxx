/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/FunctionChain.h"
#include "zoolib/Memory.h" // For sMemCopy

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDCPixmapNS.h"
#include "zoolib/ZDebug.h"

#include <vector>

#define kDebug_PixmapNS 2

namespace ZooLib {

using namespace ZDCPixmapNS;

using std::lower_bound;
using std::min;
using std::vector;

/*! \namespace ZooLib::ZDCPixmapNS
ZDCPixmapNS encompasses a suite of types and methods primarily used by ZDCPixmap but that can also
be called explicitly, which is useful when working with pixmap data that's owned by other code.

AG 2000-11-21. The all new ZDCPixmap.

We work with pixmaps at two levels. The lower level treats the pixmap as a raster of pixel values,
where each pixel value is simply a sequence of between 1 and 32 bits. There are accessor and
manipulation functions that work purely in terms of pixel values for those situations where client
code knows what it is doing, for example the platform-specific rendering code. The second level
applies an interpretation to the pixel values, treating each as representing an index into an
associated color table, as a gray scale value or as a RGBA tuple.

So, from a traditional perspective, ZDCPixmap supports three types of image:

1) Indexed. Each pixel value represents an index into a color table associated with the Pixmap.
Pixel values can be between 1 and 16 bits in size, although efficient packing occurs only for pixel
values that are 1, 2, 4, 8 or 16 bits in size, which support color tables of 2, 4, 16,
256 and 65536 entries respectively.

2) Gray. Each pixel value directly represents a gray shade, where 0 maps to black and all ones
represents white. As with the indexed case pixel values can be between 1 and 16 bits in size,
although 1, 2, 4, and 8 are most commonly used. Note that a 1 bit gray pixmap is the traditional
monochrome pixmap, although the sense of the bits for mono pixmaps is the inverse of that used on
MacOS where a zero bit usually represents white and a one bit represents black. ZooLib deals with
this mismatch by judicious use of transfer modes or inverting the use of fore and back colors.

3) Color. Each pixel value represents a tuple holding red, green, blue and alpha components. Each
component can be between 0 and 16 bits in size, with 8888, 8880, 5551, 5650 and 3320 being the
most commonly used, given that those formats are most often directly supported by hardware graphics
devices. ZooLib currently does *not* make direct use of the alpha component -- our support of it is
purely for the purposes of preserving information that a platform might need to do its work, for
example to allow masked blits on BeOS. We specify which bits represent which components by passing
in non-overlapping contiguous bit masks for each, and thus can represent and work with *any*
ordering of red, green, blue and alpha components, where each component can be between 0 and
16 bits in size -- a size of 0 bits indicates that the component is absent. Note that the masks
are expressed as 32 bit values, and as pixel values are also normalized to 32 bits in size we can't
actually support pixmaps with 16 bits for every component -- the maximum evenly sized RGB triple
would have 10 bits per component, with 2 bits left unused.

ZDCPixmap itself is just a smart pointer. Its suite of constructors allow one to create pixmaps of
specified depths, sizes and types (indexed, gray or color), and with contents initialized or left
unitialized. ZDCPixmap wraps a ZRef to a ZDCPixmapRep, and its mutating methods will make a fresh
copy of the ZDCPixmapRep when necessary. ZDCPixmapRep holds the description of the raster: the bit
count of each pixel value, the bounding rectangle in the raster that the rep references, the offset
between rows, whether the pixel values are organized in big or little endian fashion (this applies
to both multi-byte pixel values and sub-byte pixel values). ZDCPixmapRep also stores a ZRef to a
ZDCPixmapRaster, which holds the raster itself. ZDCPixmapRaster is a base class with appropriate
concrete implementations, which allows us to do a couple of things. First we have a subclass that
references pixel data kept in static storage, or in non-modifiable resource data, so that when the
buffer object is finalized it does *not* attempt to free the storage. Second we have a sub classes
that reference shared memory Pixmaps and XImages, which when finalized take the appropriate steps
to release the shared memory.

ZDCPixmapRep describes the raster, references the buffer and also maintains a bounding rectangle,
so we can initialize a ZDCPixmap to be a subset of another ZDCPixmap and actually have them
continue to reference the *same* buffer, until one or other of them has a mutating operation
applied. At that time the data is transcribed into a new buffer, one which is just big enough to
hold all the referenced pixels. For example, one could load a large pixmap from a resource (using
the non-disposing ZDCPixmapRaster to reference the resource data), then instantiate pixmaps which
are subsets of that pixmap and no actual pixel data will be copied.

In the past all the generic manipulation functions and data types were declared as members of
class ZDCPixmap. This complicated the declaration of ZDCPixmap enormously, it is after all simply a
smart pointer. So we have a namespace ZDCPixmapNS to scope those entities that are referenced by
different parts of the ZDCPixmap suite.
*/

// =================================================================================================

namespace { // anonymous

struct StandardToInfoGray_t
	{
	EFormatStandard fFormat;
	uint32 fMaskGray;
	uint32 fMaskAlpha;
	int32 fDepth;
	bool fBigEndian;
	};

StandardToInfoGray_t sStandardToInfoGray[] =
	{
	{ eFormatStandard_Gray_1, 0x1, 0x0, 1, true },
	{ eFormatStandard_Gray_2, 0x3, 0x0, 2, true },
	{ eFormatStandard_Gray_4, 0xF, 0x0, 4, true },
	{ eFormatStandard_Gray_8, 0xFF, 0x00, 8, true },

#if ZCONFIG(Endian, Big)

	{ eFormatStandard_GA_16, 0xFF00, 0x00FF, 16, true },
	{ eFormatStandard_AG_16, 0x00FF, 0xFF00, 16, true }

#else

	{ eFormatStandard_GA_16, 0x00FF, 0xFF00, 16, false },
	{ eFormatStandard_AG_16, 0xFF00, 0x00FF, 16, false }

#endif
	};

struct StandardToInfoColor_t
	{
	EFormatStandard fFormat;
	uint32 fMaskRed;
	uint32 fMaskGreen;
	uint32 fMaskBlue;
	uint32 fMaskAlpha;
	int32 fDepth;
	bool fBigEndian;
	};

StandardToInfoColor_t sStandardToInfoColor[] =
	{
	{ eFormatStandard_xRGB_16_BE, 0x7C00, 0x03E0, 0x001F, 0x0000, 16, true },
	{ eFormatStandard_ARGB_16_BE, 0x7C00, 0x03E0, 0x001F, 0x8000, 16, true },
	{ eFormatStandard_RGBA_16_BE, 0xF800, 0x07C0, 0x003E, 0x0001, 16, true },
	{ eFormatStandard_RGB_16_BE, 0xF800, 0x07E0, 0x001F, 0x0000, 16, true },

	{ eFormatStandard_xRGB_16_LE, 0x7C00, 0x03E0, 0x001F, 0x0000, 16, false },
	{ eFormatStandard_ARGB_16_LE, 0x7C00, 0x03E0, 0x001F, 0x8000, 16, false },
	{ eFormatStandard_RGBA_16_LE, 0xF800, 0x07C0, 0x003E, 0x0001, 16, false },
	{ eFormatStandard_RGB_16_LE, 0xF800, 0x07E0, 0x001F, 0x0000, 16, false },

#if ZCONFIG(Endian, Big)

	{ eFormatStandard_RGB_24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 24, true },
	{ eFormatStandard_BGR_24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 24, true },

	{ eFormatStandard_xRGB_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 32, true },
	{ eFormatStandard_ARGB_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 32, true },
	{ eFormatStandard_xBGR_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 32, true },
	{ eFormatStandard_ABGR_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 32, true },
	{ eFormatStandard_RGBx_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000, 32, true },
	{ eFormatStandard_RGBA_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 32, true },
	{ eFormatStandard_BGRx_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000, 32, true },
	{ eFormatStandard_BGRA_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, 32, true }

#else

	{ eFormatStandard_RGB_24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 24, false },
	{ eFormatStandard_BGR_24, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 24, false },

	{ eFormatStandard_xRGB_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000, 32, false },
	{ eFormatStandard_ARGB_32, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF, 32, false },
	{ eFormatStandard_xBGR_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000, 32, false },
	{ eFormatStandard_ABGR_32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF, 32, false },
	{ eFormatStandard_RGBx_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000, 32, false },
	{ eFormatStandard_RGBA_32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 32, false },
	{ eFormatStandard_BGRx_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000, 32, false },
	{ eFormatStandard_BGRA_32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000, 32, false }

#endif
	};

static void spMaskToShiftsAndMultiplier(uint32 iMask,
	int32& oShiftLeft, int32& oShiftRight, uint32& oMultiplier, uint32& oAdd)
	{
	oShiftLeft = 0;
	if (iMask == 0)
		{
		// If the mask is empty we use a right shift of 16. When a uint32 with bits set in
		// only its low 16 bits is shifted by 16, the result will always be zero. It is
		// important that the right shift be large enough to do this (ie at least 16), but
		// that it be smaller than the size of a uint32 (ie 32), because the result of a
		// shift by y positions of a value of size x bits is undefined when y >= x (it
		// actually does what we want on PPC, but not on x86).
		oShiftRight = 16;
		oMultiplier = 0;
		oAdd = 0xFFFFFFFFU;
		}
	else
		{
		while (0 == (iMask & 1))
			{
			++oShiftLeft;
			iMask >>= 1;
			}
		uint32 theDepth = 0;
		while (iMask & 1)
			{
			++theDepth;
			iMask >>= 1;
			}
		ZAssertStop(kDebug_PixmapNS, iMask == 0);
		ZAssertStop(kDebug_PixmapNS, theDepth <= 16);
		if (theDepth)
			{
			// theDepth must be non-zero, but Clang's analysis disagrees.
			oMultiplier = 0xFFFFFFFFU / ((1U << theDepth) - 1);
			oShiftRight = 16 - theDepth;
			oAdd = 0;
			}
		else
			{
			ZUnimplemented();
			}
		}
	}

static uint8* spBuildReverseLookup(const ZRGBA_POD* iColors, size_t iCount)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);
	uint8* lookupTable = new uint8[4096];
	for (int32 red = 0; red < 16; ++red)
		{
		for (int32 green = 0; green < 16; ++green)
			{
			for (int32 blue = 0; blue < 16; ++blue)
				{
				int32 bestDistance = 0x7FFFFFFF;
				uint8 bestIndex = 0;
				for (size_t x = 0; x < iCount; ++x)
					{
					int32 redDelta = red - (iColors[x].red >> 12);
					int32 greenDelta = green - (iColors[x].green >> 12);
					int32 blueDelta = blue - (iColors[x].blue >> 12);

					int32 currentDistance = (redDelta * redDelta)
						+ (greenDelta * greenDelta) + (blueDelta * blueDelta);

					if (bestDistance > currentDistance)
						{
						bestDistance = currentDistance;
						bestIndex = x;
						}
					}
				lookupTable[blue + 16 * (green + 16 * red)] = bestIndex;
				}
			}
		}
	return lookupTable;
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ZDCPixmapNS

int ZDCPixmapNS::sCalcRowBytes(int iWidth, int iDepth, int iByteRound)
	{
	const int bitRound = iByteRound * 8;
	return ((iWidth * iDepth + bitRound - 1) / bitRound) * iByteRound;
	}

EFormatStandard ZDCPixmapNS::sMapEfficientToStandard(EFormatEfficient iFormat)
	{
	EFormatStandard result;
	if (FunctionChain<EFormatStandard, EFormatEfficient>::sInvoke(result, iFormat))
		{
		return result;
		}

	switch (iFormat)
		{
		case eFormatEfficient_Gray_1: return eFormatStandard_Gray_1;
		case eFormatEfficient_Gray_8: return eFormatStandard_Gray_8;

		case eFormatEfficient_Color_16: return eFormatStandard_RGB_16_BE;
		case eFormatEfficient_Color_24: return eFormatStandard_RGB_24;
		case eFormatEfficient_Color_32: return eFormatStandard_ARGB_32;
		default: break;
		}

	return eFormatStandard_Invalid;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalDesc

ZDCPixmapNS::PixvalDesc::PixvalDesc(EFormatStandard iFormat)
	{
	for (size_t x = 0; x < countof(sStandardToInfoColor); ++x)
		{
		if (sStandardToInfoColor[x].fFormat == iFormat)
			{
			fDepth = sStandardToInfoColor[x].fDepth;
			fBigEndian = sStandardToInfoColor[x].fBigEndian;
			return;
			}
		}

	for (size_t x = 0; x < countof(sStandardToInfoGray); ++x)
		{
		if (sStandardToInfoGray[x].fFormat == iFormat)
			{
			fDepth = sStandardToInfoGray[x].fDepth;
			fBigEndian = sStandardToInfoGray[x].fBigEndian;
			return;
			}
		}

	ZUnimplemented();
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::RasterDesc

ZDCPixmapNS::RasterDesc::RasterDesc(ZPointPOD iSize, EFormatStandard iFormat, bool iFlipped)
	{
	for (size_t x = 0; x < countof(sStandardToInfoColor); ++x)
		{
		if (sStandardToInfoColor[x].fFormat == iFormat)
			{
			int32 depth = sStandardToInfoColor[x].fDepth;
			fPixvalDesc.fDepth = depth;
			fPixvalDesc.fBigEndian = sStandardToInfoColor[x].fBigEndian;
			fRowBytes = sCalcRowBytes(iSize.h, depth, 4);
			fRowCount = iSize.v;
			fFlipped = iFlipped;
			return;
			}
		}

	for (size_t x = 0; x < countof(sStandardToInfoGray); ++x)
		{
		if (sStandardToInfoGray[x].fFormat == iFormat)
			{
			int32 depth = sStandardToInfoGray[x].fDepth;
			fPixvalDesc.fDepth = depth;
			fPixvalDesc.fBigEndian = sStandardToInfoGray[x].fBigEndian;
			fRowBytes = sCalcRowBytes(iSize.h, depth, 4);
			fRowCount = iSize.v;
			fFlipped = iFlipped;
			return;
			}
		}

	ZUnimplemented();
	}

const void* ZDCPixmapNS::RasterDesc::CalcRowAddress(const void* iBaseAddress, int iRow) const
	{
	return static_cast<const uint8*>(iBaseAddress)
		+ fRowBytes * (fFlipped ? fRowCount - iRow - 1 : iRow);
	}

void* ZDCPixmapNS::RasterDesc::CalcRowAddressDest(void* iBaseAddress, int iRow) const
	{
	return static_cast<uint8*>(iBaseAddress)
		+ fRowBytes * (fFlipped ? fRowCount - iRow - 1 : iRow);
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalAccessor

ZDCPixmapNS::PixvalAccessor::PixvalAccessor(const PixvalDesc& iPixvalDesc)
	{
	this->Initialize(iPixvalDesc.fDepth, iPixvalDesc.fBigEndian);
	}

ZDCPixmapNS::PixvalAccessor::PixvalAccessor(int32 inDepth, bool inBigEndian)
	{
	this->Initialize(inDepth, inBigEndian);
	}

static const uint8 s1BitMaskBE[] = { 0x80U, 0x40U, 0x20U, 0x10U, 0x08U, 0x04U, 0x02U, 0x01U };
static const uint8 s1BitNotMaskBE[] = { 0x7FU, 0xBFU, 0xDFU, 0xEFU, 0xF7U, 0xFBU, 0xFDU, 0xFEU };
static const uint8 s1BitShiftBE[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

static const uint8 s1BitMaskLE[] = { 0x01U, 0x02U, 0x04U, 0x08U, 0x10U, 0x20U, 0x40U, 0x80U};
static const uint8 s1BitNotMaskLE[] = { 0xFEU, 0xFDU, 0xFBU, 0xF7U, 0xEFU, 0xDFU, 0xBFU, 0x7FU };
static const uint8 s1BitShiftLE[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

static const uint8 s2BitMaskBE[] = { 0xC0U, 0x30U, 0x0CU, 0x03U };
static const uint8 s2BitNotMaskBE[] = { 0x3FU, 0xCFU, 0xF3U, 0xFCU };
static const uint8 s2BitShiftBE[] = { 6, 4, 2, 0 };

static const uint8 s2BitMaskLE[] = { 0x03U, 0x0CU, 0x30U, 0xC0U };
static const uint8 s2BitNotMaskLE[] = { 0xFCU, 0xF3U, 0xCFU, 0x3FU };
static const uint8 s2BitShiftLE[] = { 0, 2, 4, 6 };

static const uint8 s4BitMaskBE[] = { 0xF0U, 0x0FU };
static const uint8 s4BitNotMaskBE[] = { 0x0FU, 0xF0U };
static const uint8 s4BitShiftBE[] = { 4, 0 };

static const uint8 s4BitMaskLE[] = { 0x0FU, 0xF0U };
static const uint8 s4BitNotMaskLE[] = { 0xF0U, 0x0FU };
static const uint8 s4BitShiftLE[] = { 0, 4 };

void ZDCPixmapNS::PixvalAccessor::Initialize(int32 inDepth, bool inBigEndian)
	{
	fDepth = inDepth;
	switch (inDepth)
		{
		case 1:
			{
			fDivisorShift = 3;
			fModMask = 0x07;
			if (inBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s1BitMaskBE;
				fBitNotMask = s1BitNotMaskBE;
				fBitShift = s1BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s1BitMaskLE;
				fBitNotMask = s1BitNotMaskLE;
				fBitShift = s1BitShiftLE;
				}
			break;
			}
		case 2:
			{
			fDivisorShift = 2;
			fModMask = 0x03;
			if (inBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s2BitMaskBE;
				fBitNotMask = s2BitNotMaskBE;
				fBitShift = s2BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s2BitMaskLE;
				fBitNotMask = s2BitNotMaskLE;
				fBitShift = s2BitShiftLE;
				}
			break;
			}
		case 3: case 4:
			{
			fDivisorShift = 1;
			fModMask = 0x01;
			if (inBigEndian)
				{
				fWhichCase = eCase1ByteWithShiftBE;
				fBitMask = s4BitMaskBE;
				fBitNotMask = s4BitNotMaskBE;
				fBitShift = s4BitShiftBE;
				}
			else
				{
				fWhichCase = eCase1ByteWithShiftLE;
				fBitMask = s4BitMaskLE;
				fBitNotMask = s4BitNotMaskLE;
				fBitShift = s4BitShiftLE;
				}
			break;
			}
		case 5: case 6: case 7: case 8:
			{
			fWhichCase = eCase1Byte;
			break;
			}
		case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
			{
			if (inBigEndian == ZCONFIG(Endian, Big))
				fWhichCase = eCase2Bytes;
			else
				fWhichCase = eCase2BytesWithSwap;
			break;
			}
		case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24:
			{
			if ZCONFIG(Endian, Big)
				{
				if (inBigEndian)
					fWhichCase = eCase3BytesBEHostBE;
				else
					fWhichCase = eCase3BytesLEHostBE;
				}
			else
				{
				if (inBigEndian)
					fWhichCase = eCase3BytesBEHostLE;
				else
					fWhichCase = eCase3BytesLEHostLE;
				}
			break;
			}
		case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
			{
			if (inBigEndian == ZCONFIG(Endian, Big))
				fWhichCase = eCase4Bytes;
			else
				fWhichCase = eCase4BytesWithSwap;
			break;
			}
		default:
			{
			ZDebugStopf(0, "%d depth out of range", inDepth);
			}
		}
	}

uint32 ZDCPixmapNS::PixvalAccessor::GetPixval(const void* iRowAddress, int32 inHCoord) const
	{
	uint32 value;
	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			value = static_cast<const uint8*>(iRowAddress)[inHCoord];
			break;
			}
		case eCase1ByteWithShiftBE:
		case eCase1ByteWithShiftLE:
			{
			value = static_cast<const uint8*>(iRowAddress)[inHCoord >> fDivisorShift];
			value &= fBitMask[inHCoord & fModMask];
			value >>= fBitShift[inHCoord & fModMask];
			break;
			}
		case eCase2Bytes:
			{
			value = static_cast<const uint16*>(iRowAddress)[inHCoord];
			break;
			}
		case eCase2BytesWithSwap:
			{
			value = ZByteSwap_Read16(static_cast<const uint16*>(iRowAddress) + inHCoord);
			break;
			}
		case eCase3BytesBEHostBE:
		case eCase3BytesBEHostLE:
			{
			const uint8* temp = static_cast<const uint8*>(iRowAddress) + (inHCoord * 3);
			value = temp[0] << 16;
			value |= temp[1] << 8;
			value |= temp[2];
			break;
			}
		case eCase3BytesLEHostBE:
		case eCase3BytesLEHostLE:
			{
			const uint8* temp = static_cast<const uint8*>(iRowAddress) + (inHCoord * 3);
			value = temp[0];
			value |= temp[1] << 8;
			value |= temp[2] << 16;
			break;
			}
		case eCase4Bytes:
			{
			value = static_cast<const uint32*>(iRowAddress)[inHCoord];
			break;
			}
		case eCase4BytesWithSwap:
			{
			value = ZByteSwap_Read32(static_cast<const uint32*>(iRowAddress) + inHCoord);
			break;
			}
		default:
			ZUnimplemented();
		}
	return value;
	}

void ZDCPixmapNS::PixvalAccessor::SetPixval(void* iRowAddress, int32 inHCoord, uint32 iPixval) const
	{
	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			static_cast<uint8*>(iRowAddress)[inHCoord] = iPixval;
			break;
			}
		case eCase1ByteWithShiftBE:
		case eCase1ByteWithShiftLE:
			{
			uint8* destAddress =
				static_cast<uint8*>(iRowAddress) + (inHCoord >> fDivisorShift);
			*destAddress = (*destAddress & fBitNotMask[inHCoord & fModMask])
				| (iPixval << fBitShift[inHCoord & fModMask]);
			break;
			}
		case eCase2Bytes:
			{
			static_cast<uint16*>(iRowAddress)[inHCoord] = iPixval;
			break;
			}
		case eCase2BytesWithSwap:
			{
			ZByteSwap_Write16(static_cast<uint16*>(iRowAddress) + inHCoord, iPixval);
			break;
			}
		case eCase3BytesBEHostBE:
		case eCase3BytesBEHostLE:
			{
			uint8* temp = static_cast<uint8*>(iRowAddress) + (inHCoord * 3);
			temp[0] = iPixval >> 16;
			temp[1] = iPixval >> 8;
			temp[2] = iPixval;
			break;
			}
		case eCase3BytesLEHostBE:
		case eCase3BytesLEHostLE:
			{
			uint8* temp = static_cast<uint8*>(iRowAddress) + (inHCoord * 3);
			temp[0] = iPixval;
			temp[1] = iPixval >> 8;
			temp[2] = iPixval >> 16;
			break;
			}
		case eCase4Bytes:
			{
			static_cast<uint32*>(iRowAddress)[inHCoord] = iPixval;
			break;
			}
		case eCase4BytesWithSwap:
			{
			ZByteSwap_Write32(static_cast<uint32*>(iRowAddress) + inHCoord, iPixval);
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

#if 0
// GetPixvals and SetPixvals effectively do the equivalent of the following, but do so
// substantially more efficiently.
void ZDCPixmapNS::PixvalAccessor::GetPixvals(const void* iRowAddress,
	int32 iStartH, size_t iCount, uint32* oPixvals) const
	{
	while (iCount--)
		*oPixvals++ = this->GetPixval(iRowAddress, iStartH++);
	}

void ZDCPixmapNS::PixvalAccessor::SetPixvals(void* iRowAddress,
	int32 iStartH, size_t iCount, const uint32* iPixvals) const
	{
	while (iCount--)
		this->SetPixval(iRowAddress, iStartH++, *iPixvals++);
	}

#endif

#if __MWERKS__ && __MACH__
static uint32 spByteSwap_Read32(const uint32* iValueAddress)
	{
	return ZByteSwap_Read32(iValueAddress);
	}

static void spByteSwap_Write32(uint32* iValueAddress, uint32 iValue)
	{
	ZByteSwap_Write32(iValueAddress, iValue);
	}
#endif

void ZDCPixmapNS::PixvalAccessor::GetPixvals(const void* iRowAddress,
	int32 iStartH, size_t iCount, uint32* oPixvals) const
	{
	// There are multiple cases handled by this function and by SetPixvals. Many of the cases
	// want to use variables with the same names and types. Normally I'm a strong advogate of
	// declaring variables at their point of first use, but that makes it hard to know which
	// variable to examine when looking at this code in a source level debugger, so I'm
	// declaring the common names here.

	size_t localCount;
	const uint8* localSource8;
	const uint16* localSource16;
	const uint32* localSource32;
	uint32 value;

	uint32 a0b0c0a1;
	uint32 b1c1a2b2;
	uint32 c2a3b3c3;
	uint32 c1a0b0c0;
	uint32 b2c2a1b1;
	uint32 a3b3c3a2;

	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			localCount = iCount + 1;
			localSource8 = static_cast<const uint8*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = *localSource8++;
			break;
			}
		case eCase1ByteWithShiftBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH >> fDivisorShift);
			const int32 leftShift = fDepth;
			const int32 rightShift = 8 - fDepth;
			const size_t masterLoadCount = 8 / fDepth;
			uint8 currentSourceValue = *localSource8++;
			currentSourceValue = currentSourceValue << (leftShift * (iStartH & fModMask));
			size_t currentLoadCount = masterLoadCount - (iStartH & fModMask);
			localCount = iCount;
			while (localCount)
				{
				--localCount;
				--currentLoadCount;
				uint8 newSourceValue = currentSourceValue << leftShift;
				uint32 value = currentSourceValue >> rightShift;
				if (currentLoadCount)
					{
					currentSourceValue = newSourceValue;
					}
				else
					{
					currentSourceValue = *localSource8++;
					currentLoadCount = masterLoadCount;
					}
				*oPixvals++ = value;
				}
			break;
			}
		case eCase1ByteWithShiftLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH >> fDivisorShift);
			const int32 rightShift = fDepth;
			const uint32 mask = (1 << fDepth) - 1;
			const size_t masterLoadCount = 8 / fDepth;
			uint8 currentSourceValue = *localSource8++;
			currentSourceValue = currentSourceValue >> (rightShift * (iStartH & fModMask));
			size_t currentLoadCount = masterLoadCount - (iStartH & fModMask);
			localCount = iCount;
			while (localCount)
				{
				--localCount;
				--currentLoadCount;
				uint8 newSourceValue = currentSourceValue >> rightShift;
				uint32 value = currentSourceValue & mask;
				if (currentLoadCount)
					{
					currentSourceValue = newSourceValue;
					}
				else
					{
					currentSourceValue = *localSource8++;
					currentLoadCount = masterLoadCount;
					}
				*oPixvals++ = value;
				}
			break;
			}
		case eCase2Bytes:
			{
			localCount = iCount + 1;
			localSource16 = static_cast<const uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = *localSource16++;
			break;
			}
		case eCase2BytesWithSwap:
			{
			localCount = iCount + 1;
			localSource16 = static_cast<const uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = uint16(ZByteSwap_Read16(localSource16++));
			break;
			}
		case eCase3BytesBEHostBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0a1 = localSource32[0];
					b1c1a2b2 = localSource32[1];
					c2a3b3c3 = localSource32[2];
					localSource32 += 3;

					oPixvals[0] = a0b0c0a1 >> 8;
					oPixvals[1] = ((a0b0c0a1 & 0x000000FF) << 16) | (b1c1a2b2 >> 16);
					oPixvals[2] = ((b1c1a2b2 & 0x0000FFFF) << 8) | (c2a3b3c3 >> 24);
					oPixvals[3] = c2a3b3c3 & 0x00FFFFFF;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase3BytesBEHostLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0a1 = ZByteSwap_Read32(localSource32++);
					b1c1a2b2 = ZByteSwap_Read32(localSource32++);
					c2a3b3c3 = ZByteSwap_Read32(localSource32++);

					oPixvals[0] = a0b0c0a1 >> 8;
					oPixvals[1] = ((a0b0c0a1 & 0x000000FF) << 16) | (b1c1a2b2 >> 16);
					oPixvals[2] = ((b1c1a2b2 & 0x0000FFFF) << 8) | (c2a3b3c3 >> 24);
					oPixvals[3] = c2a3b3c3 & 0x00FFFFFF;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0] << 16;
				value |= localSource8[1] << 8;
				value |= localSource8[2];
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase3BytesLEHostBE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;

					// AG 2002-04-27. We run out of registers when compiling this
					// with CW Mach, hence the use of spByteSwap_Read32.
					#if __MWERKS__ && __MACH__
					c1a0b0c0 = spByteSwap_Read32(localSource32++);
					b2c2a1b1 = spByteSwap_Read32(localSource32++);
					a3b3c3a2 = spByteSwap_Read32(localSource32++);
					#else
					c1a0b0c0 = ZByteSwap_Read32(localSource32++);
					b2c2a1b1 = ZByteSwap_Read32(localSource32++);
					a3b3c3a2 = ZByteSwap_Read32(localSource32++);
					#endif

					oPixvals[0] = c1a0b0c0 & 0x00FFFFFF;
					oPixvals[1] = (c1a0b0c0 >> 24) | ((b2c2a1b1 & 0x0000FFFF) << 8);
					oPixvals[2] = (b2c2a1b1 >> 16) | ((a3b3c3a2 & 0x000000FF) << 16);
					oPixvals[3] = a3b3c3a2 >> 8;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				--localCount;
				}
			break;
			}
		case eCase3BytesLEHostLE:
			{
			localSource8 = static_cast<const uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localSource8) & 0x03) && localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}

			if (localCount >= 4)
				{
				localSource32 = reinterpret_cast<const uint32*>(localSource8);
				while (localCount >= 4)
					{
					localCount -= 4;
					c1a0b0c0 = localSource32[0];
					b2c2a1b1 = localSource32[1];
					a3b3c3a2 = localSource32[2];
					localSource32 += 3;

					oPixvals[0] = c1a0b0c0 & 0x00FFFFFF;
					oPixvals[1] = (c1a0b0c0 >> 24) | ((b2c2a1b1 & 0x0000FFFF) << 8);
					oPixvals[2] = (b2c2a1b1 >> 16) | ((a3b3c3a2 & 0x000000FF) << 16);
					oPixvals[3] = a3b3c3a2 >> 8;
					oPixvals += 4;
					}
				localSource8 = reinterpret_cast<const uint8*>(localSource32);
				}

			while (localCount)
				{
				--localCount;
				value = localSource8[0];
				value |= localSource8[1] << 8;
				value |= localSource8[2] << 16;
				localSource8 += 3;
				*oPixvals++ = value;
				}
			break;
			}
		case eCase4Bytes:
			{
			sMemCopy(oPixvals,
				static_cast<const uint32*>(iRowAddress) + iStartH, iCount * sizeof(uint32));
			break;
			}
		case eCase4BytesWithSwap:
			{
			localCount = iCount + 1;
			localSource32 = static_cast<const uint32*>(iRowAddress) + iStartH;
			while (--localCount)
				*oPixvals++ = uint32(ZByteSwap_Read32(localSource32++));
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

void ZDCPixmapNS::PixvalAccessor::SetPixvals(void* iRowAddress,
	int32 iStartH, size_t iCount, const uint32* iPixvals) const
	{
	size_t localCount;
	uint8* localDest8;
	uint16* localDest16;
	uint32* localDest32;
	uint32 a0b0c0;
	uint32 a1b1c1;
	uint32 a2b2c2;
	uint32 a3b3c3;
	uint32 value;

	switch (fWhichCase)
		{
		case eCase1Byte:
			{
			localCount = iCount + 1;
			localDest8 = static_cast<uint8*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest8++ = *iPixvals++;
			break;
			}
		case eCase1ByteWithShiftBE:
			{
			uint8 destVal8;
			uint8 destMask8;
			uint8 cumuMask8;
			localCount = iCount;
			switch (fDepth)
				{
				case 1:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 3);
					if (iStartH & 0x07)
						{
						destVal8 = 0;
						destMask8 = 0x80 >> (iStartH & 0x07);
						cumuMask8 = 0;
						while ((iStartH & 0x07) && localCount)
							{
							++iStartH;
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 8)
						{
						localCount -= 8;
						destVal8 = 0;
						if (*iPixvals++ & 0x01) destVal8 |= 0x80;
						if (*iPixvals++ & 0x01) destVal8 |= 0x40;
						if (*iPixvals++ & 0x01) destVal8 |= 0x20;
						if (*iPixvals++ & 0x01) destVal8 |= 0x10;
						if (*iPixvals++ & 0x01) destVal8 |= 0x08;
						if (*iPixvals++ & 0x01) destVal8 |= 0x04;
						if (*iPixvals++ & 0x01) destVal8 |= 0x02;
						if (*iPixvals++ & 0x01) destVal8 |= 0x01;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x80;
						cumuMask8 = 0;
						while (localCount)
							{
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 2:
					{
					int32 sourceShift;
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 2);
					if (iStartH & 0x03)
						{
						destVal8 = 0;
						destMask8 = 0xC0 >> (2 * (iStartH & 0x03));
						cumuMask8 = 0;
						sourceShift = 6 - (2 * (iStartH & 0x03));
						while ((iStartH & 0x03) && localCount)
							{
							++iStartH;
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift -= 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;						
						}

					while (localCount >= 4)
						{
						localCount -= 4;
						destVal8 = (*iPixvals++ & 0x03) << 6;
						destVal8 |= (*iPixvals++ & 0x03) << 4;
						destVal8 |= (*iPixvals++ & 0x03) << 2;
						destVal8 |= (*iPixvals++ & 0x03);
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0xC0;
						cumuMask8 = 0;
						sourceShift = 6;
						while (localCount)
							{
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 >> 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift -= 2;
							}
						*localDest8 = ((*localDest8 & (0xFF ^ cumuMask8))) | destVal8;
						}
					break;
					}
				case 3: case 4:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 1);
					if (iStartH & 0x01)
						{
						--localCount;
						*localDest8 = (*localDest8 & 0xF0) | (*iPixvals++ & 0x0F);
						++localDest8;
						}

					while (localCount >= 2)
						{
						localCount -= 2;
						uint8 temp = ((*iPixvals++ & 0x0F) << 4);
						*localDest8++ = temp | (*iPixvals++ & 0x0F);
						}

					if (localCount)
						*localDest8 = (*localDest8 & 0x0F) | ((*iPixvals++ & 0x0F) << 4);
					break;
					}
				}
			break;
			}
		case eCase1ByteWithShiftLE:
			{
			uint8 destVal8;
			uint8 destMask8;
			uint8 cumuMask8;
			localCount = iCount;
			switch (fDepth)
				{
				case 1:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 3);
					if (iStartH & 0x07)
						{
						destVal8 = 0;
						destMask8 = 0x01 << (iStartH & 0x07);
						cumuMask8 = 0;
						while ((iStartH & 0x07) && localCount)
							{
							++iStartH;
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 8)
						{
						localCount -= 8;
						destVal8 = 0;
						if (*iPixvals++ & 0x01) destVal8 |= 0x01;
						if (*iPixvals++ & 0x01) destVal8 |= 0x02;
						if (*iPixvals++ & 0x01) destVal8 |= 0x04;
						if (*iPixvals++ & 0x01) destVal8 |= 0x08;
						if (*iPixvals++ & 0x01) destVal8 |= 0x10;
						if (*iPixvals++ & 0x01) destVal8 |= 0x20;
						if (*iPixvals++ & 0x01) destVal8 |= 0x40;
						if (*iPixvals++ & 0x01) destVal8 |= 0x80;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x01;
						cumuMask8 = 0;
						while (localCount)
							{
							--localCount;
							if (*iPixvals++ & 0x01)
								destVal8 |= destMask8;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 1;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 2:
					{
					int32 sourceShift;
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 2);
					if (iStartH & 0x03)
						{
						destVal8 = 0;
						destMask8 = 0x03 << (2 * (iStartH & 0x03));
						cumuMask8 = 0;
						sourceShift = 0;
						while ((iStartH & 0x03) && localCount)
							{
							++iStartH;
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift += 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						++localDest8;
						}

					while (localCount >= 4)
						{
						localCount -= 4;
						destVal8 = (*iPixvals++ & 0x03);
						destVal8 |= (*iPixvals++ & 0x03) << 2;
						destVal8 |= (*iPixvals++ & 0x03) << 4;
						destVal8 |= (*iPixvals++ & 0x03) << 6;
						*localDest8++ = destVal8;
						}

					if (localCount)
						{
						destVal8 = 0;
						destMask8 = 0x03;
						cumuMask8 = 0;
						sourceShift = 0;
						while (localCount)
							{
							--localCount;
							cumuMask8 |= destMask8;
							destMask8 = destMask8 << 2;
							destVal8 |= (*iPixvals++ & 0x03) << sourceShift;
							sourceShift += 2;
							}
						*localDest8 = (*localDest8 & (0xFF ^ cumuMask8)) | destVal8;
						}
					break;
					}
				case 3: case 4:
					{
					localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH >> 1);
					if (iStartH & 0x01)
						{
						--localCount;
						*localDest8 = (*localDest8 & 0x0F) | ((*iPixvals++ & 0x0F) << 4);
						++localDest8;
						}

					while (localCount >= 2)
						{
						localCount -= 2;
						uint8 temp = (*iPixvals++ & 0x0F);
						*localDest8++ = temp | ((*iPixvals++ & 0x0F) << 4);
						}

					if (localCount)
						*localDest8 = (*localDest8 & 0xF0) | (*iPixvals++ & 0x0F);
					break;
					}
				}
			break;
			}
		case eCase2Bytes:
			{
			localCount = iCount + 1;
			localDest16 = static_cast<uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest16++ = *iPixvals++;
			break;
			}
		case eCase2BytesWithSwap:
			{
			localCount = iCount + 1;
			localDest16 = static_cast<uint16*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest16++ = uint16(ZByteSwap_Read16(iPixvals++));
			break;
			}
		case eCase3BytesBEHostBE:
			{
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			localCount = iCount;
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0];
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					localDest32[0] = (a0b0c0 << 8) | (a1b1c1 >> 16);
					localDest32[1] = (a1b1c1 << 16) | (a2b2c2 >> 8);
					localDest32[2] = (a2b2c2 << 24) | a3b3c3;
					localDest32 += 3;
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesBEHostLE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0];
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					ZByteSwap_Write32(localDest32++, (a0b0c0 << 8) | (a1b1c1 >> 16));
					ZByteSwap_Write32(localDest32++, (a1b1c1 << 16) | (a2b2c2 >> 8));
					ZByteSwap_Write32(localDest32++, (a2b2c2 << 24) | a3b3c3);
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value >> 16;
				localDest8[1] = value >> 8;
				localDest8[2] = value;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesLEHostBE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0] & 0x00FFFFFF;
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					#if __MWERKS__ && __MACH__
					spByteSwap_Write32(localDest32++, a0b0c0 | (a1b1c1 << 24));
					spByteSwap_Write32(localDest32++, (a1b1c1 >> 8) | (a2b2c2 << 16));
					spByteSwap_Write32(localDest32++, (a2b2c2 >> 16) | (a3b3c3 << 8));
					#else
					ZByteSwap_Write32(localDest32++, a0b0c0 | (a1b1c1 << 24));
					ZByteSwap_Write32(localDest32++, (a1b1c1 >> 8) | (a2b2c2 << 16));
					ZByteSwap_Write32(localDest32++, (a2b2c2 >> 16) | (a3b3c3 << 8));
					#endif
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}
			break;
			}
		case eCase3BytesLEHostLE:
			{
			localCount = iCount;
			localDest8 = static_cast<uint8*>(iRowAddress) + (iStartH * 3);
			while ((reinterpret_cast<intptr_t>(localDest8) & 0x03) && localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}

			if (localCount >= 4)
				{
				uint32* localDest32 = reinterpret_cast<uint32*>(localDest8);
				while (localCount >= 4)
					{
					localCount -= 4;
					a0b0c0 = iPixvals[0] & 0x00FFFFFF;
					a1b1c1 = iPixvals[1] & 0x00FFFFFF;
					a2b2c2 = iPixvals[2] & 0x00FFFFFF;
					a3b3c3 = iPixvals[3] & 0x00FFFFFF;
					iPixvals += 4;
					localDest32[0] = a0b0c0 | (a1b1c1 << 24);
					localDest32[1] = (a1b1c1 >> 8) | (a2b2c2 << 16);
					localDest32[2] = (a2b2c2 >> 16) | (a3b3c3 << 8);
					localDest32 += 3;
					}
				localDest8 = reinterpret_cast<uint8*>(localDest32);
				}

			while (localCount)
				{
				--localCount;
				value = *iPixvals++;
				localDest8[0] = value;
				localDest8[1] = value >> 8;
				localDest8[2] = value >> 16;
				localDest8 += 3;
				}
			break;
			}
		case eCase4Bytes:
			{
			sMemCopy(static_cast<uint32*>(iRowAddress) + iStartH,
				iPixvals, iCount * sizeof(uint32));
			break;
			}
		case eCase4BytesWithSwap:
			{
			localCount = iCount + 1;
			localDest32 = static_cast<uint32*>(iRowAddress) + iStartH;
			while (--localCount)
				*localDest32++ = uint32(ZByteSwap_Read32(iPixvals++));
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalIterR

ZDCPixmapNS::PixvalIterR::PixvalIterR(const void* iAddress,
	const PixvalDesc& iPixvalDesc, int iCoord)
:	fAddress(iAddress),
	fAccessor(iPixvalDesc),
	fCoord(iCoord)
	{}

uint32 ZDCPixmapNS::PixvalIterR::ReadInc()
	{
	uint32 result = fAccessor.GetPixval(fAddress, fCoord);
	++fCoord;
	return result;
	}

uint32 ZDCPixmapNS::PixvalIterR::Read()
	{
	return fAccessor.GetPixval(fAddress, fCoord);
	}

void ZDCPixmapNS::PixvalIterR::Inc()
	{
	++fCoord;
	}

void ZDCPixmapNS::PixvalIterR::Reset(const void* iAddress, int iCoord)
	{
	fAddress = iAddress;
	fCoord = iCoord;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalIterRW

ZDCPixmapNS::PixvalIterRW::PixvalIterRW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord)
:	fAddress(iAddress),
	fAccessor(iPixvalDesc),
	fCoord(iCoord)
	{}

uint32 ZDCPixmapNS::PixvalIterRW::ReadInc()
	{
	uint32 result = fAccessor.GetPixval(fAddress, fCoord);
	++fCoord;
	return result;
	}

uint32 ZDCPixmapNS::PixvalIterRW::Read()
	{
	return fAccessor.GetPixval(fAddress, fCoord);
	}

void ZDCPixmapNS::PixvalIterRW::WriteInc(uint32 iPixval)
	{
	fAccessor.SetPixval(fAddress, fCoord, iPixval);
	++fCoord;
	}

void ZDCPixmapNS::PixvalIterRW::Write(uint32 iPixval)
	{
	fAccessor.SetPixval(fAddress, fCoord, iPixval);
	}

void ZDCPixmapNS::PixvalIterRW::Inc()
	{
	++fCoord;
	}

void ZDCPixmapNS::PixvalIterRW::Reset(void* iAddress, int iCoord)
	{
	fAddress = iAddress;
	fCoord = iCoord;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalIterW

ZDCPixmapNS::PixvalIterW::PixvalIterW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord)
:	fAddress(iAddress),
	fAccessor(iPixvalDesc),
	fCoord(iCoord)
	{}

void ZDCPixmapNS::PixvalIterW::WriteInc(uint32 iPixval)
	{
	fAccessor.SetPixval(fAddress, fCoord, iPixval);
	++fCoord;
	}

void ZDCPixmapNS::PixvalIterW::Write(uint32 iPixval)
	{
	fAccessor.SetPixval(fAddress, fCoord, iPixval);
	}

void ZDCPixmapNS::PixvalIterW::Inc()
	{
	++fCoord;
	}

void ZDCPixmapNS::PixvalIterW::Reset(void* iAddress, int iCoord)
	{
	fAddress = iAddress;
	fCoord = iCoord;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Indexed

void ZDCPixmapNS::MapPixvalToRGB_Indexed::AsRGBAs(const uint32* iPixvals,
	size_t iCount, ZRGBA_POD* oColors) const
	{
	size_t localCount = iCount + 1;
	if (fPixvals)
		{
		uint32* pixValsEnd = fPixvals + fCount;
		while (--localCount)
			{
			size_t offset = lower_bound(fPixvals, pixValsEnd, *iPixvals++) - fPixvals;
			*oColors++ = fColors[offset];
			}
		}
	else
		{
		while (--localCount)
			*oColors++ = fColors[*iPixvals++];
		}
	}

void ZDCPixmapNS::MapPixvalToRGB_Indexed::pAsRGBA(uint32 iPixval,
	ZRGBA_POD& oColor) const
	{
	uint32* iter = lower_bound(fPixvals, fPixvals + fCount, iPixval);
	oColor = fColors[iter - fPixvals];
	}

uint16 ZDCPixmapNS::MapPixvalToRGB_Indexed::pAsAlpha(uint32 iPixval) const
	{
	uint32* iter = lower_bound(fPixvals, fPixvals + fCount, iPixval);
	return fColors[iter - fPixvals].alpha;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Gray

void ZDCPixmapNS::MapPixvalToRGB_Gray::AsRGBAs(const uint32* iPixvals,
	size_t iCount, ZRGBA_POD* oColors) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 pixVal = *iPixvals++;

		oColors->red = oColors->green = oColors->blue =
			sConvert(pixVal, fMaskL, fShiftL, fMultiplierL, fAddL) >> 16;

		oColors->alpha = sConvert(pixVal, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Color

void ZDCPixmapNS::MapPixvalToRGB_Color::AsRGBAs(const uint32* iPixvals,
	size_t iCount, ZRGBA_POD* oColors) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 pixVal = *iPixvals++;
		oColors->red = sConvert(pixVal, fMaskR, fShiftR, fMultiplierR, fAddR) >> 16;
		oColors->green = sConvert(pixVal, fMaskG, fShiftG, fMultiplierG, fAddG) >> 16;
		oColors->blue = sConvert(pixVal, fMaskB, fShiftB, fMultiplierB, fAddB) >> 16;
		oColors->alpha = sConvert(pixVal, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Indexed

void ZDCPixmapNS::MapRGBToPixval_Indexed::AsPixvals(const ZRGBA_POD* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	if (fPixvals)
		{
		while (--localCount)
			{
			const uint8 index = uint8(fReverseLookup[(iColors->blue >> 12)
				+ 16 * ((iColors->green >> 12) + 16 * (iColors->red >> 12))]);

			++iColors;
			 *oPixvals++ = fPixvals[index];
			}
		}
	else
		{
		while (--localCount)
			{
			*oPixvals++ = uint8(fReverseLookup[(iColors->blue >> 12)
				+ 16 * ((iColors->green >> 12) + 16 * (iColors->red >> 12))]);

			++iColors;
			}
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Gray

void ZDCPixmapNS::MapRGBToPixval_Gray::AsPixvals(const ZRGBA_POD* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 theValue = ((uint32((uint32(iColors->red)
			+ uint32(iColors->green)
			+ uint32(iColors->blue)) * 0x101U / 3) >> fShiftRightL) << fShiftLeftL);

		theValue |= ((uint32(iColors->alpha) >> fShiftRightA) << fShiftLeftA);

		++iColors;
		*oPixvals++ = theValue;
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Color

void ZDCPixmapNS::MapRGBToPixval_Color::AsPixvals(const ZRGBA_POD* iColors,
	size_t iCount, uint32* oPixvals) const
	{
	size_t localCount = iCount + 1;
	while (--localCount)
		{
		uint32 theValue = ((uint32(iColors->red) >> fShiftRightR) << fShiftLeftR);
		theValue |= ((uint32(iColors->green) >> fShiftRightG) << fShiftLeftG);
		theValue |= ((uint32(iColors->blue) >> fShiftRightB) << fShiftLeftB);
		theValue |= ((uint32(iColors->alpha) >> fShiftRightA) << fShiftLeftA);
		++iColors;
		*oPixvals++ = theValue;
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep

ThreadSafe_t ZDCPixmapNS::PixelDescRep::sChangeCount;

ZDCPixmapNS::PixelDescRep::PixelDescRep()
	{
	ZMACRO_ThreadSafe_Set(fChangeCount, ZMACRO_ThreadSafe_IncReturnOld(sChangeCount));
	}

ZDCPixmapNS::PixelDescRep::~PixelDescRep()
	{}

int ZDCPixmapNS::PixelDescRep::GetChangeCount() const
	{
	return ZMACRO_ThreadSafe_Get(fChangeCount);
	}

int ZDCPixmapNS::PixelDescRep::MarkChanged()
	{
	return ZMACRO_ThreadSafe_Swap(fChangeCount, ZMACRO_ThreadSafe_IncReturnOld(sChangeCount));
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Indexed

ZDCPixmapNS::PixelDescRep_Indexed::PixelDescRep_Indexed(const ZRGBA_POD* iColors, size_t iCount)
:	fCheckedAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	MapPixvalToRGB_Indexed::fPixvals = nullptr;
	MapRGBToPixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	fCount = iCount;
	fColors = new ZRGBA_POD[iCount];
	sMemCopy(fColors, iColors, iCount * sizeof(ZRGBA_POD));
	}

ZDCPixmapNS::PixelDescRep_Indexed::PixelDescRep_Indexed(const ZRGBA_POD* iColors,
	size_t iCount, bool iStripAlpha)
:	fCheckedAlpha(true),
	fHasAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	ZAssertStopf(1, iStripAlpha, "This constructor is only used when stripping alpha");

	MapPixvalToRGB_Indexed::fPixvals = nullptr;
	MapRGBToPixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	fCount = iCount;
	fColors = new ZRGBA_POD[iCount];
	sMemCopy(fColors, iColors, iCount * sizeof(ZRGBA_POD));

	// Set the alpha value to be completely opaque.
	for (size_t x = 0; x < iCount; ++x)
		fColors[x].alpha = 0xFFFFU;
	}

ZDCPixmapNS::PixelDescRep_Indexed::PixelDescRep_Indexed(const ZRGBA_POD* iColors,
	uint32* iPixvals, size_t iCount)
:	fCheckedAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	MapPixvalToRGB_Indexed::fPixvals = nullptr;
	MapRGBToPixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	vector<ZRGBA_POD> vectorColors;
	vector<uint32> vectorPixvals;
	for (size_t x = 0; x < iCount; ++x)
		{
		vector<uint32>::iterator insertIter =
			lower_bound(vectorPixvals.begin(), vectorPixvals.end(), iPixvals[x]);

		insertIter = vectorPixvals.insert(insertIter, iPixvals[x]);
		vectorColors.insert(
			vectorColors.begin() + (insertIter - vectorPixvals.begin()), iColors[x]);
		}

	fCount = iCount;
	fColors = new ZRGBA_POD[iCount];

	try
		{
		MapPixvalToRGB_Indexed::fPixvals = new uint32[iCount];
		MapRGBToPixval_Indexed::fPixvals = MapPixvalToRGB_Indexed::fPixvals;
		}
	catch (...)
		{
		delete fColors;
		throw;
		}

	sMemCopy(fColors, &vectorColors[0], iCount * sizeof(ZRGBA_POD));
	sMemCopy(MapPixvalToRGB_Indexed::fPixvals, &vectorPixvals[0], iCount * sizeof(uint32));
	}

ZDCPixmapNS::PixelDescRep_Indexed::PixelDescRep_Indexed(const ZRGBA_POD* iColors,
	uint32* iPixvals, size_t iCount, bool iStripAlpha)
:	fCheckedAlpha(true),
	fHasAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	ZAssertStopf(1, iStripAlpha, ("This constructor is only used when stripping alpha"));

	MapPixvalToRGB_Indexed::fPixvals = nullptr;
	MapRGBToPixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	vector<ZRGBA_POD> vectorColors;
	vector<uint32> vectorPixvals;
	for (size_t x = 0; x < iCount; ++x)
		{
		vector<uint32>::iterator insertIter =
			lower_bound(vectorPixvals.begin(), vectorPixvals.end(), iPixvals[x]);

		insertIter = vectorPixvals.insert(insertIter, iPixvals[x]);
		vectorColors.insert(
			vectorColors.begin() + (insertIter - vectorPixvals.begin()), iColors[x]);
		}

	fCount = iCount;
	fColors = new ZRGBA_POD[iCount];

	try
		{
		MapPixvalToRGB_Indexed::fPixvals = new uint32[iCount];
		MapRGBToPixval_Indexed::fPixvals = MapPixvalToRGB_Indexed::fPixvals;
		}
	catch (...)
		{
		delete fColors;
		throw;
		}

	sMemCopy(fColors, &vectorColors[0], iCount * sizeof(ZRGBA_POD));
	sMemCopy(MapPixvalToRGB_Indexed::fPixvals, &vectorPixvals[0], iCount * sizeof(uint32));

	// Set the alpha value to be completely opaque.
	for (size_t x = 0; x < iCount; ++x)
		fColors[x].alpha = 0xFFFFU;
	}

ZDCPixmapNS::PixelDescRep_Indexed::PixelDescRep_Indexed(
	const ZRGBAMap* iColorMap, size_t iCount)
:	fCheckedAlpha(false)
	{
	ZAssertStop(kDebug_PixmapNS, iCount <= 256);

	MapPixvalToRGB_Indexed::fPixvals = nullptr;
	MapRGBToPixval_Indexed::fPixvals = nullptr;
	fReverseLookup = nullptr;

	fCount = 256;
	fColors = new ZRGBA_POD[256];
	for (size_t x = 0; x < 256; ++x)
		{
		fColors[x].red = fColors[x].green = fColors[x].blue = 0;
		fColors[x].alpha = 0xFFFFU;
		}

	for (size_t x = 0; x < iCount; ++x)
		fColors[iColorMap[x].fPixval] = iColorMap[x].fColor;
	}

ZDCPixmapNS::PixelDescRep_Indexed::~PixelDescRep_Indexed()
	{
	delete[] fColors;
	delete[] MapPixvalToRGB_Indexed::fPixvals;
	delete[] fReverseLookup;
	}

bool ZDCPixmapNS::PixelDescRep_Indexed::HasAlpha()
	{
	if (!fCheckedAlpha)
		{
		fHasAlpha = false;
		ZRGBA_POD* current = fColors;
		size_t count = fCount + 1;
		while (--count)
			{
			if (current->alpha != 0xFFFFU)
				{
				fHasAlpha = true;
				break;
				}
			++current;
			}
		fCheckedAlpha = true;
		}
	return fHasAlpha;
	}

ZRef<ZDCPixmapNS::PixelDescRep> ZDCPixmapNS::PixelDescRep_Indexed::WithoutAlpha()
	{
	if (fCheckedAlpha && !fHasAlpha)
		return this;

	if (MapPixvalToRGB_Indexed::fPixvals)
		return new PixelDescRep_Indexed(fColors, MapPixvalToRGB_Indexed::fPixvals, fCount, true);
	else
		return new PixelDescRep_Indexed(fColors, fCount);
	}

void ZDCPixmapNS::PixelDescRep_Indexed::Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
	{ MapPixvalToRGB_Indexed::AsRGBA(iPixval, oColor); }

void ZDCPixmapNS::PixelDescRep_Indexed::Imp_AsRGBAs(
	const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const
	{ MapPixvalToRGB_Indexed::AsRGBAs(iPixvals, iCount, oColors); }

uint16 ZDCPixmapNS::PixelDescRep_Indexed::Imp_AsAlpha(uint32 iPixval) const
	{ return MapPixvalToRGB_Indexed::AsAlpha(iPixval); }

uint32 ZDCPixmapNS::PixelDescRep_Indexed::Imp_AsPixval(const ZRGBA_POD& iRGBA) const
	{
	this->BuildReverseLookupIfNeeded();
	return MapRGBToPixval_Indexed::AsPixval(iRGBA);
	}

void ZDCPixmapNS::PixelDescRep_Indexed::Imp_AsPixvals(
	const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const
	{
	this->BuildReverseLookupIfNeeded();
	MapRGBToPixval_Indexed::AsPixvals(iColors, iCount, oPixvals);
	}

void ZDCPixmapNS::PixelDescRep_Indexed::BuildReverseLookupIfNeeded() const
	{
	if (!fReverseLookup)
		fReverseLookup = spBuildReverseLookup(fColors, fCount);
	}

void ZDCPixmapNS::PixelDescRep_Indexed::GetColors(
	const ZRGBA_POD*& oColors, size_t& oCount) const
	{
	oColors = fColors;
	oCount = fCount;
	}

bool ZDCPixmapNS::PixelDescRep_Indexed::Matches(const PixelDescRep_Indexed* iOther)
	{
	return this == iOther;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Gray

ZDCPixmapNS::PixelDescRep_Gray::PixelDescRep_Gray(uint32 iMaskL, uint32 iMaskA)
	{
	spMaskToShiftsAndMultiplier(iMaskL, fShiftLeftL, fShiftRightL, fMultiplierL, fAddL);
	fShiftL = fShiftLeftL;
	fMaskL = iMaskL;

	spMaskToShiftsAndMultiplier(iMaskA, fShiftLeftA, fShiftRightA, fMultiplierA, fAddA);
	fShiftA = fShiftLeftA;
	fMaskA = iMaskA;
	}

bool ZDCPixmapNS::PixelDescRep_Gray::HasAlpha()
	{ return fMaskA; }

ZRef<ZDCPixmapNS::PixelDescRep> ZDCPixmapNS::PixelDescRep_Gray::WithoutAlpha()
	{
	if (!fMaskA)
		return this;
	return new PixelDescRep_Gray(fMaskL, 0);
	}

void ZDCPixmapNS::PixelDescRep_Gray::Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
	{ MapPixvalToRGB_Gray::AsRGBA(iPixval, oColor); }

void ZDCPixmapNS::PixelDescRep_Gray::Imp_AsRGBAs(
	const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const
	{ MapPixvalToRGB_Gray::AsRGBAs(iPixvals, iCount, oColors); }

uint16 ZDCPixmapNS::PixelDescRep_Gray::Imp_AsAlpha(uint32 iPixval) const
	{ return MapPixvalToRGB_Gray::AsAlpha(iPixval); }

uint32 ZDCPixmapNS::PixelDescRep_Gray::Imp_AsPixval(const ZRGBA_POD& iRGBA) const
	{ return MapRGBToPixval_Gray::AsPixval(iRGBA); }

void ZDCPixmapNS::PixelDescRep_Gray::Imp_AsPixvals(
	const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const
	{ MapRGBToPixval_Gray::AsPixvals(iColors, iCount, oPixvals); }

void ZDCPixmapNS::PixelDescRep_Gray::GetMasks(uint32& oMaskL, uint32& oMaskA) const
	{
	oMaskL = fMaskL;
	oMaskA = fMaskA;
	}

bool ZDCPixmapNS::PixelDescRep_Gray::Matches(const PixelDescRep_Gray* iOther)
	{
	if (fMaskL && iOther->fMaskL && fMaskL != iOther->fMaskL)
		return false;
	if (fMaskA && iOther->fMaskA && fMaskA != iOther->fMaskA)
		return false;
	return true;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Color

ZDCPixmapNS::PixelDescRep_Color::PixelDescRep_Color(
	uint32 iMaskR, uint32 iMaskG, uint32 iMaskB, uint32 iMaskA)
	{
	// Ensure that the masks are all non-overlapping
	ZAssertStop(kDebug_PixmapNS, 0 == ((iMaskR & iMaskG) | (iMaskR & iMaskB) | (iMaskR & iMaskA)
		| (iMaskG & iMaskB) | (iMaskG & iMaskA) | (iMaskB & iMaskA)));

	spMaskToShiftsAndMultiplier(iMaskR, fShiftLeftR, fShiftRightR, fMultiplierR, fAddR);
	fShiftR = fShiftLeftR;
	fMaskR = iMaskR;

	spMaskToShiftsAndMultiplier(iMaskG, fShiftLeftG, fShiftRightG, fMultiplierG, fAddG);
	fShiftG = fShiftLeftG;
	fMaskG = iMaskG;

	spMaskToShiftsAndMultiplier(iMaskB, fShiftLeftB, fShiftRightB, fMultiplierB, fAddB);
	fShiftB = fShiftLeftB;
	fMaskB = iMaskB;

	spMaskToShiftsAndMultiplier(iMaskA, fShiftLeftA, fShiftRightA, fMultiplierA, fAddA);
	fShiftA = fShiftLeftA;
	fMaskA = iMaskA;
	}

bool ZDCPixmapNS::PixelDescRep_Color::HasAlpha()
	{ return fMaskA; }

ZRef<ZDCPixmapNS::PixelDescRep> ZDCPixmapNS::PixelDescRep_Color::WithoutAlpha()
	{
	if (!fMaskA)
		return this;
	return new PixelDescRep_Color(fMaskR, fMaskG, fMaskB, 0);
	}

void ZDCPixmapNS::PixelDescRep_Color::Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
	{ MapPixvalToRGB_Color::AsRGBA(iPixval, oColor); }

void ZDCPixmapNS::PixelDescRep_Color::Imp_AsRGBAs(
	const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const
	{ MapPixvalToRGB_Color::AsRGBAs(iPixvals, iCount, oColors); }

uint16 ZDCPixmapNS::PixelDescRep_Color::Imp_AsAlpha(uint32 iPixval) const
	{ return MapPixvalToRGB_Color::AsAlpha(iPixval); }

uint32 ZDCPixmapNS::PixelDescRep_Color::Imp_AsPixval(const ZRGBA_POD& iRGBA) const
	{ return MapRGBToPixval_Color::AsPixval(iRGBA); }

void ZDCPixmapNS::PixelDescRep_Color::Imp_AsPixvals(
	const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const
	{ MapRGBToPixval_Color::AsPixvals(iColors, iCount, oPixvals); }

void ZDCPixmapNS::PixelDescRep_Color::GetMasks(
	uint32& oMaskR, uint32& oMaskG, uint32& oMaskB, uint32& oMaskA) const
	{
	oMaskR = fMaskR;
	oMaskG = fMaskG;
	oMaskB = fMaskB;
	oMaskA = fMaskA;
	}

bool ZDCPixmapNS::PixelDescRep_Color::Matches(const PixelDescRep_Color* iOther)
	{
	if (fMaskR && iOther->fMaskR && fMaskR != iOther->fMaskR)
		return false;
	if (fMaskG && iOther->fMaskG && fMaskG != iOther->fMaskG)
		return false;
	if (fMaskB && iOther->fMaskB && fMaskB != iOther->fMaskB)
		return false;
	if (fMaskA && iOther->fMaskA && fMaskA != iOther->fMaskA)
		return false;
	return true;
	}

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDesc

ZDCPixmapNS::PixelDesc::PixelDesc()
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(const PixelDesc& iOther)
:	fRep(iOther.fRep)
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(EFormatStandard iFormat)
	{
	for (size_t x = 0; x < countof(sStandardToInfoColor); ++x)
		{
		if (sStandardToInfoColor[x].fFormat == iFormat)
			{
			fRep = new PixelDescRep_Color(
				sStandardToInfoColor[x].fMaskRed,
				sStandardToInfoColor[x].fMaskGreen,
				sStandardToInfoColor[x].fMaskBlue,
				sStandardToInfoColor[x].fMaskAlpha);
			return;
			}
		}

	for (size_t x = 0; x < countof(sStandardToInfoGray); ++x)
		{
		if (sStandardToInfoGray[x].fFormat == iFormat)
			{
			fRep = new PixelDescRep_Gray(
				sStandardToInfoGray[x].fMaskGray,
				sStandardToInfoGray[x].fMaskAlpha);
			return;
			}
		}

	ZUnimplemented();
	}

ZDCPixmapNS::PixelDesc::PixelDesc(const ZRGBA_POD* iColors, size_t iCount)
:	fRep(new PixelDescRep_Indexed(iColors, iCount))
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(const ZRGBAMap* iColorMap, size_t iCount)
:	fRep(new PixelDescRep_Indexed(iColorMap, iCount))
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(uint32 iMaskGray, uint32 iMaskAlpha)
:	fRep(new PixelDescRep_Gray(iMaskGray, iMaskAlpha))
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(
	uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha)
:	fRep(new PixelDescRep_Color(iMaskRed, iMaskGreen, iMaskBlue, iMaskAlpha))
	{}

ZDCPixmapNS::PixelDesc::PixelDesc(const ZRef<PixelDescRep>& iPixelDescRep)
:	fRep(iPixelDescRep)
	{}

ZDCPixmapNS::PixelDesc::~PixelDesc()
	{}

ZDCPixmapNS::PixelDesc& ZDCPixmapNS::PixelDesc::operator=(const PixelDesc& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZDCPixmapNS::PixelDesc& ZDCPixmapNS::PixelDesc::operator=(const ZRef<PixelDescRep>& iPixelDescRep)
	{
	fRep = iPixelDescRep;
	return *this;
	}

ZRef<ZDCPixmapNS::PixelDescRep> ZDCPixmapNS::PixelDesc::GetRep() const
	{ return fRep; }

int ZDCPixmapNS::PixelDesc::GetChangeCount() const
	{
	if (fRep)
		return fRep->GetChangeCount();
	return 0;
	}

bool ZDCPixmapNS::PixelDesc::HasAlpha() const
	{ return fRep->HasAlpha(); }

ZDCPixmapNS::PixelDesc ZDCPixmapNS::PixelDesc::WithoutAlpha() const
	{ return fRep->WithoutAlpha(); }

void ZDCPixmapNS::PixelDesc::AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
	{ fRep->Imp_AsRGBA(iPixval, oColor); }

void ZDCPixmapNS::PixelDesc::AsRGBAs(
	const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const
	{ fRep->Imp_AsRGBAs(iPixvals, iCount, oColors); }

uint16 ZDCPixmapNS::PixelDesc::AsAlpha(uint32 iPixval) const
	{ return fRep->Imp_AsAlpha(iPixval); }

uint32 ZDCPixmapNS::PixelDesc::AsPixval(const ZRGBA_POD& iRGBA) const
	{ return fRep->Imp_AsPixval(iRGBA); }

void ZDCPixmapNS::PixelDesc::AsPixvals(
	const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const
	{ fRep->Imp_AsPixvals(iColors, iCount, oPixvals); }

// =================================================================================================
// MARK: - ZDCPixmapNS

void ZDCPixmapNS::sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval)
	{
	if (iRasterDesc.fPixvalDesc.fDepth >= 17 && iRasterDesc.fPixvalDesc.fDepth <= 24)
		{
		// For 17 to 24 bit pixVals we may have to write separately each
		// of the three bytes used per pixVal.
		uint8 byte1, byte2, byte3;
		if (iRasterDesc.fPixvalDesc.fBigEndian)
			{
			byte1 = (iPixval >> 16) & 0xFF;
			byte2 = (iPixval >> 8) & 0xFF;
			byte3 = iPixval & 0xFF;
			}
		else
			{
			byte1 = iPixval & 0xFF;
			byte2 = (iPixval >> 8) & 0xFF;
			byte3 = (iPixval >> 16) & 0xFF;
			}

		if (byte1 == byte2 && byte2 == byte3)
			{
			// We're lucky -- all the bytes of the pixVal are the same, so we can use sMemSet.
			sMemSet(iBaseAddress, byte1, iRasterDesc.fRowBytes * iRasterDesc.fRowCount);
			}
		else
			{
			size_t localRowCount = iRasterDesc.fRowCount + 1;
			size_t localChunkCount = (iRasterDesc.fRowBytes / 3) + 1;
			uint8* localBaseAddress = static_cast<uint8*>(iBaseAddress);
			while (--localRowCount)
				{
				size_t chunkCount = localChunkCount;
				uint8* chunkAddress = localBaseAddress;
				localBaseAddress += iRasterDesc.fRowBytes;
				while (--chunkCount)
					{
					*chunkAddress++ = byte1;
					*chunkAddress++ = byte2;
					*chunkAddress++ = byte3;
					}
				}
			}
		}
	else
		{
		// We're going to build a uint32 to fill the raster with, but this will only
		// work correctly if rowBytes is a multiple of four.
		ZAssertStop(1, (iRasterDesc.fRowBytes & 0x03) == 0);
		int32 effectiveDepth;
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
				effectiveDepth = 1;
				break;
			case 2:
				effectiveDepth = 2;
				break;
			case 3: case 4:
				effectiveDepth = 4;
				break;
			case 5: case 6: case 7: case 8:
				effectiveDepth = 8;
				break;
			case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16:
				effectiveDepth = 16;
				break;
			case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
				effectiveDepth = 32;
				break;
			default:
				ZUnimplemented();
			}

		uint32 storedValue = iPixval;
		if (effectiveDepth < 32)
			{
			// Smear the < 32 bits across all 32 bits (we'll only do
			// more than one iteration if effective depth was 16 or less).
			storedValue &= (1 << effectiveDepth) - 1;
			while (effectiveDepth < 32)
				{
				storedValue = storedValue | (storedValue << effectiveDepth);
				effectiveDepth *= 2;
				}
			}

		if (((storedValue & 0xFFFF) == ((storedValue >> 16) & 0xFFFF))
			&& ((storedValue & 0xFF) == ((storedValue >> 8) & 0xFF)))
			{
			// We're lucky -- all the bytes of the pixVal are the same so we can use sMemSet.
			sMemSet(iBaseAddress,
				storedValue & 0xFF, iRasterDesc.fRowBytes * iRasterDesc.fRowCount);
			}
		else
			{
			if (iRasterDesc.fPixvalDesc.fBigEndian != ZCONFIG(Endian, Big))
				storedValue = ZByteSwap_32(storedValue);
			size_t chunkCount = ((iRasterDesc.fRowBytes * iRasterDesc.fRowCount) / 4) + 1;
			uint32* chunkAddress = static_cast<uint32*>(iBaseAddress);
			while (--chunkCount)
				*chunkAddress++ = storedValue;
			}
		}
	}

void ZDCPixmapNS::sFill(
	void* iBaseAddress, const RasterDesc& iRasterDesc, const ZRectPOD& iBounds, uint32 iPixval)
	{
	int32 hSize = W(iBounds);

	vector<uint32> sourcePixvals(hSize, iPixval);

	PixvalAccessor destAccessor(iRasterDesc.fPixvalDesc);

	int32 vSize = H(iBounds);

	for (int32 vCurrent = 0; vCurrent < vSize; ++vCurrent)
		{
		void* rowAddress = iRasterDesc.CalcRowAddressDest(iBaseAddress, iBounds.top + vCurrent);
		destAccessor.SetPixvals(rowAddress, iBounds.left, hSize, &sourcePixvals[0]);
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS:sMunge variants

template <class S, class D>
static void sMungeRow_T(
	void* iRowAddress, const PixvalDesc& iPixvalDesc,
	const S& iMapPixvalToRGB, const D& iMapRGBToPixval,
	ZCoord inStartH, int32 inCount, ZCoord inCoordV,
	MungeProc iMungeProc, void* iRefcon)
	{
	PixvalAccessor sourceAccessor(iPixvalDesc);
	int32 hCurrent = 0;
	ZRGBA_POD theRGBA;
	while (hCurrent < inCount)
		{
		uint32 sourceValue = sourceAccessor.GetPixval(iRowAddress, inStartH + hCurrent);
		iMapPixvalToRGB.AsRGBA(sourceValue, theRGBA);
		if (iMungeProc(hCurrent, inCoordV, theRGBA, iRefcon))
			{
			uint32 destValue = iMapRGBToPixval.AsPixval(theRGBA);
			sourceAccessor.SetPixval(iRowAddress, inStartH + hCurrent, destValue);
			}
		++hCurrent;
		}
	}

template <class S, class D>
static void sMunge_T(
	void* iBaseAddress, const RasterDesc& iRasterDesc,
	const S& iMapPixvalToRGB, const D& iMapRGBToPixval,
	const ZRectPOD& iBounds, MungeProc iMungeProc, void* iRefcon)
	{
	int32 hSize = W(iBounds);
	int32 vSize = H(iBounds);

	for (int32 vCurrent = 0; vCurrent < vSize; ++vCurrent)
		{
		void* rowAddress = iRasterDesc.CalcRowAddressDest(iBaseAddress, iBounds.top + vCurrent);
		sMungeRow_T(rowAddress, iRasterDesc.fPixvalDesc,
				iMapPixvalToRGB, iMapRGBToPixval,
				iBounds.left, hSize, vCurrent,
				iMungeProc, iRefcon);
		}
	}

void ZDCPixmapNS::sMunge(
	void* iBaseAddress, const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc,
	const ZRectPOD& iBounds, MungeProc iMungeProc, void* iRefcon)
	{
	ZRef<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color =
		thePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Color, *thePixelDescRep_Color, iBounds, iMungeProc, iRefcon);
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
		thePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		thePixelDescRep_Indexed->BuildReverseLookupIfNeeded();
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Indexed, *thePixelDescRep_Indexed, iBounds, iMungeProc, iRefcon);
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray =
		thePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		sMunge_T(iBaseAddress, iRasterDesc,
			*thePixelDescRep_Gray, *thePixelDescRep_Gray, iBounds, iMungeProc, iRefcon);
		}
	else
		{
		sMunge_T(iBaseAddress, iRasterDesc, iPixelDesc, iPixelDesc, iBounds, iMungeProc, iRefcon);
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapNS:sBlit variants

static const size_t kBufSize = sStackBufferSize;

template <class S, class D>
static void sBlitRow_T(
	const void* iSourceRowAddress, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGB,
	int32 iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int32 hCurrent = 0;
	ZRGBA theRGBA;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int32 tempCount = count + 1;
		uint32* currentPixval = buffer;
		while (--tempCount)
			{
			iSourcePixvalToRGB.AsRGBA(*currentPixval, theRGBA);
			*currentPixval++ = iDestRGBToPixval.AsPixval(theRGBA);
			}
		destAccessor.SetPixvals(iDestRowAddress, iDestH + hCurrent, count, buffer);
		hCurrent += count;
		}
	}

template <class S, class D>
static void sBlitRowInvert_T(
	const void* iSourceRowAddress, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGB,
	int32 iSourceH,
	void* iDestRowAddress, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 inHCount)
	{
	PixvalAccessor sourceAccessor(iSourcePixvalDesc);
	PixvalAccessor destAccessor(iDestPixvalDesc);

	int32 hCurrent = 0;
	ZRGBA theRGBA;
	while (hCurrent < inHCount)
		{
		uint32 buffer[kBufSize];
		const size_t count = min(size_t(inHCount - hCurrent), kBufSize);
		sourceAccessor.GetPixvals(iSourceRowAddress, iSourceH + hCurrent, count, buffer);

		int32 tempCount = count + 1;
		uint32* currentPixval = buffer;
		while (--tempCount)
			{
			iSourcePixvalToRGB.AsRGBA(*currentPixval, theRGBA);
			theRGBA.red = 0xFFFFU - theRGBA.red;
			theRGBA.green = 0xFFFFU - theRGBA.green;
			theRGBA.blue = 0xFFFFU - theRGBA.blue;
			*currentPixval++ = iDestRGBToPixval.AsPixval(theRGBA);
			}
		destAccessor.SetPixvals(iDestRowAddress, iDestH + hCurrent, count, buffer);
		hCurrent += count;
		}
	}

template <class S, class D>
static void sBlitWithMaps_T(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const S& iSourcePixvalToRGB,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const D& iDestRGBToPixval,
	const ZRectPOD& iSourceBounds, ZPointPOD iDestLocation, bool iInvertColors)
	{
	int32 vCount = H(iSourceBounds);
	int32 hCount = W(iSourceBounds);

	if (iInvertColors)
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				iSourceRasterDesc.CalcRowAddress(iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				iDestRasterDesc.CalcRowAddressDest(iDestBase, iDestLocation.v + vCurrent);

			sBlitRowInvert_T(
				sourceRowAddress, iSourceRasterDesc.fPixvalDesc, iSourcePixvalToRGB,
				iSourceBounds.left,
				destRowAddress, iDestRasterDesc.fPixvalDesc, iDestRGBToPixval,
				iDestLocation.h,
				hCount);
			}
		}
	else
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				iSourceRasterDesc.CalcRowAddress(iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				iDestRasterDesc.CalcRowAddressDest(iDestBase, iDestLocation.v + vCurrent);

			sBlitRow_T(
				sourceRowAddress, iSourceRasterDesc.fPixvalDesc, iSourcePixvalToRGB,
				iSourceBounds.left,
				destRowAddress, iDestRasterDesc.fPixvalDesc, iDestRGBToPixval,
				iDestLocation.h,
				hCount);
			}
		}
	}

void ZDCPixmapNS::sBlitPixvals(const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const ZRectPOD& iSourceBounds, ZPointPOD iDestLocation)
	{
	int32 vCount = H(iSourceBounds);
	int32 hCount = W(iSourceBounds);

	if (iSourceRasterDesc.fPixvalDesc == iDestRasterDesc.fPixvalDesc
		&& (iSourceRasterDesc.fPixvalDesc.fDepth >= 8
			|| ((iSourceBounds.left | iDestLocation.h | hCount) & 0x07) == 0))
		{
		int32 hOffsetSource = iSourceBounds.left * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		int32 hOffsetDest = iDestLocation.h * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		int32 countToCopy = hCount * iSourceRasterDesc.fPixvalDesc.fDepth / 8;
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const uint8* sourceRowAddress = static_cast<const uint8*>(
				iSourceRasterDesc.CalcRowAddress(iSourceBase, iSourceBounds.top + vCurrent))
				+ hOffsetSource;

			uint8* destRowAddress = static_cast<uint8*>(
					iDestRasterDesc.CalcRowAddressDest(iDestBase, iDestLocation.v + vCurrent))
					+ hOffsetDest;

			sMemCopy(destRowAddress, sourceRowAddress, countToCopy);
			}
		}
	else
		{
		for (int32 vCurrent = 0; vCurrent < vCount; ++vCurrent)
			{
			const void* sourceRowAddress =
				iSourceRasterDesc.CalcRowAddress(iSourceBase, iSourceBounds.top + vCurrent);

			void* destRowAddress =
				iDestRasterDesc.CalcRowAddressDest(iDestBase, iDestLocation.v + vCurrent);

			PixvalAccessor sourceAccessor(iSourceRasterDesc.fPixvalDesc);
			PixvalAccessor destAccessor(iDestRasterDesc.fPixvalDesc);

			int32 hCurrent = 0;
			ZRGBA theRGBA;
			while (hCurrent < hCount)
				{
				uint32 buffer[kBufSize];
				const size_t count = min(size_t(hCount - hCurrent), kBufSize);

				sourceAccessor.GetPixvals(sourceRowAddress,
					hCurrent + iSourceBounds.left, count, buffer);

				destAccessor.SetPixvals(destRowAddress, hCurrent + iDestLocation.h, count, buffer);
				hCurrent += count;
				}
			}
		}
	}

void ZDCPixmapNS::sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDesc,
	const ZRectPOD& iSourceBounds, ZPointPOD iDestLocation)
	{
	bool iInvertColors = false;
	ZRef<PixelDescRep> sourcePixelDescRep = iSourcePixelDesc.GetRep();
	ZRef<PixelDescRep> destPixelDescRep = iDestPixelDesc.GetRep();

	if (PixelDescRep_Color* sourcePixelDescRep_Color =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			if (not iInvertColors && sourcePixelDescRep_Color->Matches(destPixelDescRep_Color))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Color,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Indexed* sourcePixelDescRep_Indexed =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			if (not iInvertColors && sourcePixelDescRep_Indexed->Matches(destPixelDescRep_Indexed))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Indexed,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else if (PixelDescRep_Gray* sourcePixelDescRep_Gray =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			if (not iInvertColors && sourcePixelDescRep_Gray->Matches(destPixelDescRep_Gray))
				{
				sBlitPixvals(iSourceBase, iSourceRasterDesc,
					iDestBase, iDestRasterDesc,
					iSourceBounds, iDestLocation);
				}
			else
				{
				sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
					iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
					iSourceBounds, iDestLocation, iInvertColors);
				}
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, *sourcePixelDescRep_Gray,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	else
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Color,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Indexed,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, *destPixelDescRep_Gray,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		else
			{
			sBlitWithMaps_T(iSourceBase, iSourceRasterDesc, iSourcePixelDesc,
				iDestBase, iDestRasterDesc, iDestPixelDesc,
				iSourceBounds, iDestLocation, iInvertColors);
			}
		}
	}

template <class S, class D>
static void sBlitRowWithMaps_T(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const S& iSourcePixvalToRGB,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const D& iDestRGBToPixval,
	int32 iDestH,
	int32 iCount, bool iInvertColors)
	{
	if (iInvertColors)
		{
		sBlitRowInvert_T(iSourceBase, iSourcePixvalDesc, iSourcePixvalToRGB, iSourceH,
			iDestBase, iDestPixvalDesc, iDestRGBToPixval, iDestH,
			iCount);
		}
	else
		{
		sBlitRow_T(iSourceBase, iSourcePixvalDesc, iSourcePixvalToRGB, iSourceH,
			iDestBase, iDestPixvalDesc, iDestRGBToPixval, iDestH,
			iCount);
		}
	}

void ZDCPixmapNS::sBlitRowPixvals(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc,
	int32 iDestH,
	int32 iCount)
	{
	if (iSourcePixvalDesc.fDepth == iDestPixvalDesc.fDepth
		&& iSourcePixvalDesc.fBigEndian == iDestPixvalDesc.fBigEndian
		&& (iSourcePixvalDesc.fDepth >= 8
			|| ((iSourceH | iDestH | iCount) & 0x07) == 0))
		{
		int32 hOffsetSource = iSourceH * iSourcePixvalDesc.fDepth / 8;
		int32 hOffsetDest = iDestH * iDestPixvalDesc.fDepth / 8;
		int32 countToCopy = iCount * iSourcePixvalDesc.fDepth / 8;
		const uint8* sourceAddress = static_cast<const uint8*>(iSourceBase) + hOffsetSource;
		uint8* destAddress = static_cast<uint8*>(iDestBase) + hOffsetDest;
		sMemCopy(destAddress, sourceAddress, countToCopy);
		}
	else
		{
		PixvalAccessor sourceAccessor(iSourcePixvalDesc);
		PixvalAccessor destAccessor(iDestPixvalDesc);

		int32 hCurrent = 0;
		ZRGBA theRGBA;
		while (hCurrent < iCount)
			{
			uint32 buffer[kBufSize];
			const int32 count = min(int32(iCount - hCurrent), int32(kBufSize));
			sourceAccessor.GetPixvals(iSourceBase, iSourceH + hCurrent, count, buffer);
			destAccessor.SetPixvals(iDestBase, iDestH + hCurrent, count, buffer);
			hCurrent += count;
			}
		}
	}

void ZDCPixmapNS::sBlitRow(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const PixelDesc& iSourcePixelDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const PixelDesc& iDestPixelDesc,
	int32 iDestH,
	int32 iCount)
	{
	bool iInvertColors = false;
	ZRef<PixelDescRep> sourcePixelDescRep = iSourcePixelDesc.GetRep();
	ZRef<PixelDescRep> destPixelDescRep = iDestPixelDesc.GetRep();

	if (PixelDescRep_Color* sourcePixelDescRep_Color =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Color>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			if (not iInvertColors && sourcePixelDescRep_Color->Matches(destPixelDescRep_Color))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc, iDestH,
					iCount);
				}
			else
				{
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Color, iDestH,
					iCount, iInvertColors);
				}
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Color,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else if (PixelDescRep_Indexed* sourcePixelDescRep_Indexed =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			if (not iInvertColors && sourcePixelDescRep_Indexed->Matches(destPixelDescRep_Indexed))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc,
					iDestH,
					iCount);
				}
			else
				{
				destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
					iDestH,
					iCount, iInvertColors);
				}
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Indexed,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else if (PixelDescRep_Gray* sourcePixelDescRep_Gray =
		sourcePixelDescRep.DynamicCast<PixelDescRep_Gray>())
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			if (not iInvertColors && sourcePixelDescRep_Gray->Matches(destPixelDescRep_Gray))
				{
				sBlitRowPixvals(
					iSourceBase, iSourcePixvalDesc,
					iSourceH,
					iDestBase, iDestPixvalDesc,
					iDestH,
					iCount);
				}
			else
				{
				sBlitRowWithMaps_T(
					iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
					iSourceH,
					iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
					iDestH,
					iCount, iInvertColors);
				}
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, *sourcePixelDescRep_Gray,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	else
		{
		if (PixelDescRep_Color* destPixelDescRep_Color =
			destPixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Color,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Indexed* destPixelDescRep_Indexed =
			destPixelDescRep.DynamicCast<PixelDescRep_Indexed>())
			{
			destPixelDescRep_Indexed->BuildReverseLookupIfNeeded();
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Indexed,
				iDestH,
				iCount, iInvertColors);
			}
		else if (PixelDescRep_Gray* destPixelDescRep_Gray =
			destPixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, *destPixelDescRep_Gray,
				iDestH,
				iCount, iInvertColors);
			}
		else
			{
			sBlitRowWithMaps_T(
				iSourceBase, iSourcePixvalDesc, iSourcePixelDesc,
				iSourceH,
				iDestBase, iDestPixvalDesc, iDestPixelDesc,
				iDestH,
				iCount, iInvertColors);
			}
		}
	}

// =================================================================================================

} // namespace ZooLib
