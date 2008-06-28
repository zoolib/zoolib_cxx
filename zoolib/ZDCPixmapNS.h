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

#ifndef __ZDCPixmapNS__
#define __ZDCPixmapNS__ 1
#include "zconfig.h"

#include "zoolib/ZGeom.h"
#include "zoolib/ZRefCount.h"
#include "zoolib/ZRGBColor.h"

// For documentation, see ZDCPixmapNS.cpp

// =================================================================================================
#pragma mark -
#pragma mark * Declarations of names in namespace ZDCPixmapNS

/// ZDCPixmapNS encompasses a suite of types and methods for working with fairly generic pixmaps.

namespace ZDCPixmapNS {

int sCalcRowBytes(int iWidth, int iDepth);

class RasterDesc;
class PixvalDesc;
class PixvalAccessor;

class PixvalIterR;
class PixvalIterRW;
class PixvalIterW;

class MapPixvalToRGB_Indexed;
class MapPixvalToRGB_Gray;
class MapPixvalToRGB_Color;

class MapRGBToPixval_Indexed;
class MapRGBToPixval_Gray;
class MapRGBToPixval_Color;

typedef bool (*MungeProc)(ZCoord iLocationH, ZCoord iLocationV,
	ZRGBColorPOD& ioColor, void* iRefcon);

typedef bool (*MungeProcSmall)(ZCoord iLocationH, ZCoord iLocationV,
	ZRGBColorSmallPOD& ioColor, void* iRefcon);

class PixelDescRep;
class PixelDescRep_Indexed;
class PixelDescRep_Gray;
class PixelDescRep_Color;
class PixelDesc;

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::EFormatEfficient

namespace ZDCPixmapNS {

/** EFormatEfficient allows one to specify a pixel format that meets representational criteria
without having to determine for oneself what exact format to use. Platform-specific factory
functions actually handle the creation of an appropriate ZDCPixmapRep, one that will be 'efficient'
for that platform, for some definition of efficient. */

enum EFormatEfficient
	{
	eFormatEfficient_Invalid,

	eFormatEfficient_Gray_1,
	eFormatEfficient_Gray_8,

	eFormatEfficient_Color_16,
	eFormatEfficient_Color_24,
	eFormatEfficient_Color_32
	};

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::EFormatStandard

namespace ZDCPixmapNS {

/** EFormatStandard allows one to specify that a pixmap be created with *exactly* the pixel
format required, without having to do the work of setting up a RasterDesc and PixelDesc oneself. */

enum EFormatStandard
	{
	eFormatStandard_Invalid,

	eFormatStandard_Gray_1,
	eFormatStandard_Gray_2,
	eFormatStandard_Gray_4,
	eFormatStandard_Gray_8,

	eFormatStandard_GA_16,
	eFormatStandard_AG_16,

	eFormatStandard_xRGB_16_BE,
	eFormatStandard_ARGB_16_BE,
	eFormatStandard_RGB_16_BE,

	eFormatStandard_xRGB_16_LE,
	eFormatStandard_ARGB_16_LE,
	eFormatStandard_RGB_16_LE,

	eFormatStandard_RGB_24,
	eFormatStandard_BGR_24,

	eFormatStandard_xRGB_32,
	eFormatStandard_ARGB_32,
	eFormatStandard_xBGR_32,
	eFormatStandard_ABGR_32,
	eFormatStandard_RGBx_32,
	eFormatStandard_RGBA_32,
	eFormatStandard_BGRx_32,
	eFormatStandard_BGRA_32
	};

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixvalDesc

/** PixvalDesc describes how a single pixval is laid out. */

class ZDCPixmapNS::PixvalDesc
	{
public:
	PixvalDesc() {}
	PixvalDesc(int32 iDepth, bool iBigEndian)
	:	fDepth(iDepth), fBigEndian(iBigEndian)
		{}

	bool operator==(const PixvalDesc& iOther) const
		{ return fDepth == iOther.fDepth && fBigEndian == iOther.fBigEndian; }

	int32 fDepth;
	bool fBigEndian;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::RasterDesc

/** RasterDesc describes the layout of pixel values in a raster. It does not address which
components of a pixel are encoded in which bits of a pixel value, it simply maintains enough
information to determine which cluster of bits correspond to an (h, v) pair. */

class ZDCPixmapNS::RasterDesc
	{
public:
	RasterDesc() {}

	RasterDesc(const PixvalDesc& iPixvalDesc, size_t iRowBytes, size_t iRowCount, bool iFlipped)
	:	fPixvalDesc(iPixvalDesc), fRowBytes(iRowBytes), fRowCount(iRowCount), fFlipped(iFlipped)
		{}

	RasterDesc(ZPoint iSize, EFormatStandard iFormat, bool iFlipped = false);

	bool operator==(const RasterDesc& iOther) const
		{
		return fPixvalDesc == iOther.fPixvalDesc
			&& fRowBytes == iOther.fRowBytes
			&& fRowCount == iOther.fRowCount
			&& fFlipped == iOther.fFlipped;
		}

	const void* CalcRowAddress(const void* iBaseAddress, int iRow) const;
	void* CalcRowAddressDest(void* iBaseAddress, int iRow) const;

	PixvalDesc fPixvalDesc;
	size_t fRowBytes;
	size_t fRowCount;
	bool fFlipped;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixvalAccessor

/** PixvalAccessor provides for efficient access to
pixvals by precomputing and reusing key information. */

class ZDCPixmapNS::PixvalAccessor
	{
public:
	PixvalAccessor(const PixvalDesc& iPixvalDesc);
	PixvalAccessor(int32 iDepth, bool iBigEndian);

	uint32 GetPixval(const void* iRowAddress, int32 iHCoord) const;
	void SetPixval(void* iRowAddress, int32 iHCoord, uint32 iPixval) const;

	void GetPixvals(const void* iRowAddress, int32 iStartH,
		size_t iCount, uint32* oPixvals) const;

	void SetPixvals(void* iRowAddress, int32 iStartH,
		size_t iCount, const uint32* iPixvals) const;

protected:
	void Initialize(int32 iDepth, bool iBigEndian);

	int32 fDepth;
	int32 fDivisorShift;
	int32 fModMask;
	const uint8* fBitMask;
	const uint8* fBitNotMask;
	const uint8* fBitShift;
	enum ECase
		{
		eCase1Byte, eCase1ByteWithShiftBE, eCase1ByteWithShiftLE,
		eCase2Bytes, eCase2BytesWithSwap,
		eCase3BytesBEHostBE, eCase3BytesBEHostLE, eCase3BytesLEHostBE, eCase3BytesLEHostLE,
		eCase4Bytes, eCase4BytesWithSwap
		};
	ECase fWhichCase;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixvalIterR

class ZDCPixmapNS::PixvalIterR
	{
public:
	PixvalIterR(const void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	uint32 ReadInc();
	uint32 Read()
		{ return fCurrent; }

	void Reset(const void* iAddress, int iCoord);

protected:
	const void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	uint32 fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * PixvalIterRW

class ZDCPixmapNS::PixvalIterRW
	{
public:
	PixvalIterRW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	uint32 ReadInc();
	uint32 Read()
		{ return fCurrent; }


	void WriteInc(uint32 iPixval);
	void Write(uint32 iPixval)
		{
		fCurrent = iPixval;
		fAccessor.SetPixval(fAddress, fCoord, iPixval);
		}

	void Inc();

	void Reset(void* iAddress, int iCoord);

private:
	void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	uint32 fCurrent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixvalIterW

class ZDCPixmapNS::PixvalIterW
	{
public:
	PixvalIterW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	void WriteInc(uint32 iPixval);
	void Write(uint32 iPixval)
		{ fAccessor.SetPixval(fAddress, fCoord, iPixval); }

	void Inc()
		{ ++fCoord; }

	void Reset(void* iAddress, int iCoord);

private:
	void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapPixvalToRGB_Indexed

class ZDCPixmapNS::MapPixvalToRGB_Indexed
	{
protected:
	MapPixvalToRGB_Indexed() {}
	MapPixvalToRGB_Indexed(const MapPixvalToRGB_Indexed&) {}
	~MapPixvalToRGB_Indexed() {}
	MapPixvalToRGB_Indexed& operator=(const MapPixvalToRGB_Indexed&) { return *this; }

public:
	void AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const
		{
		if (fPixvals)
			this->Internal_AsRGBColor(iPixval, oColor);
		else
			oColor = fColors[iPixval];
		}

	void AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const
		{
		if (fPixvals)
			{
			this->Internal_AsRGBColorSmall(iPixval, oColor);
			}
		else
			{
			oColor.red = uint8(fColors[iPixval].red >> 8);
			oColor.green = uint8(fColors[iPixval].green >> 8);
			oColor.blue = uint8(fColors[iPixval].blue >> 8);
			oColor.alpha = uint8(fColors[iPixval].alpha >> 8);
			}
		}

	uint16 AsAlpha(uint32 iPixval) const
		{
		if (fPixvals)
			return this->Internal_AsAlpha(iPixval);
		else
			return fColors[iPixval].alpha;
		}

	void AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;

protected:
	void Internal_AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const;
	void Internal_AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const;
	uint16 Internal_AsAlpha(uint32 iPixval) const;

	ZRGBColorPOD* fColors;
	size_t fCount;
	uint32* fPixvals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapPixvalToRGB_Gray

class ZDCPixmapNS::MapPixvalToRGB_Gray
	{
protected:
	MapPixvalToRGB_Gray() {}
	MapPixvalToRGB_Gray(const MapPixvalToRGB_Gray&) {}
	~MapPixvalToRGB_Gray() {}
	MapPixvalToRGB_Gray& operator=(const MapPixvalToRGB_Gray&) { return *this; }

public:
	void AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const
		{
		oColor.red = oColor.green = oColor.blue
			= uint16((((iPixval & fMaskGray) >> fShiftGray) *  fMultiplierGray) >> 16);

		if (fMaskAlpha)
			{
			oColor.alpha
				= uint16((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 16);
			}
		else
			{
			oColor.alpha = 0xFFFFU;
			}
		}

	void AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const
		{
		oColor.red = oColor.green = oColor.blue
			= uint8((((iPixval & fMaskGray) >> fShiftGray) *  fMultiplierGray) >> 24);

		if (fMaskAlpha)
			{
			oColor.alpha
				= uint8((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 24);
			}
		else
			{
			oColor.alpha = 0xFFU;
			}
		}

	uint16 AsAlpha(uint32 iPixval) const
		{
		if (fMaskAlpha)
			return uint16((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 16);
		else
			return 0xFFFFU;
		}

	void AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;

protected:
	int32 fShiftGray;
	uint32 fMultiplierGray;
	uint32 fMaskGray;

	int32 fShiftAlpha;
	uint32 fMultiplierAlpha;
	uint32 fMaskAlpha;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapPixvalToRGB_Color

class ZDCPixmapNS::MapPixvalToRGB_Color
	{
protected:
	MapPixvalToRGB_Color() {}
	MapPixvalToRGB_Color(const MapPixvalToRGB_Color&) {}
	~MapPixvalToRGB_Color() {}
	MapPixvalToRGB_Color& operator=(const MapPixvalToRGB_Color&) { return *this; }

public:
	void AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const
		{
		oColor.red = uint16((((iPixval & fMaskRed) >> fShiftRed) *  fMultiplierRed) >> 16);
		oColor.green = uint16((((iPixval & fMaskGreen) >> fShiftGreen) * fMultiplierGreen) >> 16);
		oColor.blue = uint16((((iPixval & fMaskBlue) >> fShiftBlue) * fMultiplierBlue) >> 16);

		if (fMaskAlpha)
			{
			oColor.alpha
				= uint16((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 16);
			}
		else
			{
			oColor.alpha = 0xFFFFU;
			}
		}

	void AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const
		{
		oColor.red = uint8((((iPixval & fMaskRed) >> fShiftRed) *  fMultiplierRed) >> 24);
		oColor.green = uint8((((iPixval & fMaskGreen) >> fShiftGreen) * fMultiplierGreen) >> 24);
		oColor.blue = uint8((((iPixval & fMaskBlue) >> fShiftBlue) * fMultiplierBlue) >> 24);

		if (fMaskAlpha)
			{
			oColor.alpha =
			uint8((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 24);
			}
		else
			{
			oColor.alpha = 0xFFU;
			}
		}

	void AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;

	uint16 AsAlpha(uint32 iPixval) const
		{
		if (fMaskAlpha)
			return uint16((((iPixval & fMaskAlpha) >> fShiftAlpha) * fMultiplierAlpha) >> 16);
		else
			return 0xFFFFU;
		}

protected:
	int32 fShiftRed;
	uint32 fMultiplierRed;
	uint32 fMaskRed;

	int32 fShiftGreen;
	uint32 fMultiplierGreen;
	uint32 fMaskGreen;

	int32 fShiftBlue;
	uint32 fMultiplierBlue;
	uint32 fMaskBlue;

	int32 fShiftAlpha;
	uint32 fMultiplierAlpha;
	uint32 fMaskAlpha;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapRGBToPixval_Indexed

class ZDCPixmapNS::MapRGBToPixval_Indexed
	{
protected:
	MapRGBToPixval_Indexed() {}
	MapRGBToPixval_Indexed(const MapRGBToPixval_Indexed&) {}
	~MapRGBToPixval_Indexed() {}
	MapRGBToPixval_Indexed& operator=(const MapRGBToPixval_Indexed&) { return *this; }

public:
	uint32 AsPixval(const ZRGBColorPOD& iRGBColor) const
		{
		uint8 index = fReverseLookup[(iRGBColor.blue >> 12)
			+ 16 * ((iRGBColor.green >> 12)
			+ 16 * (iRGBColor.red >> 12))];

		if (fPixvals)
			return fPixvals[index];
		return index;
		}

	uint32 AsPixval(const ZRGBColorSmallPOD& iRGBColor) const
		{
		uint8 index = fReverseLookup[(iRGBColor.blue >> 4)
			+ 16 * ((iRGBColor.green >> 4) + 16 * (iRGBColor.red >> 4))];

		if (fPixvals)
			return fPixvals[index];
		return index;
		}

	void AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	mutable uint8* fReverseLookup;
	uint32* fPixvals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapRGBToPixval_Gray

class ZDCPixmapNS::MapRGBToPixval_Gray
	{
protected:
	MapRGBToPixval_Gray() {}
	MapRGBToPixval_Gray(const MapRGBToPixval_Gray&) {}
	~MapRGBToPixval_Gray() {}
	MapRGBToPixval_Gray& operator=(const MapRGBToPixval_Gray&) { return *this; }

public:
	uint32 AsPixval(const ZRGBColorPOD& iRGBColor) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32((uint32(iRGBColor.red)
			+ uint32(iRGBColor.green)
			+ uint32(iRGBColor.blue)) / 3) >> fShiftRightGray) << fShiftLeftGray);

		theValue |= ((uint32(iRGBColor.alpha) >> fShiftRightAlpha) << fShiftLeftAlpha);
		return theValue;
		}

	uint32 AsPixval(const ZRGBColorSmallPOD& iRGBColor) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32((uint32(iRGBColor.red)
			+ uint32(iRGBColor.green)
			+ uint32(iRGBColor.blue)) * 0x101U / 3) >> fShiftRightGray) << fShiftLeftGray);

		theValue |= ((uint32(iRGBColor.alpha * 0x101U) >> fShiftRightAlpha) << fShiftLeftAlpha);
		return theValue;
		}

	void AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int32 fShiftRightGray;
	int32 fShiftLeftGray;

	int32 fShiftRightAlpha;
	int32 fShiftLeftAlpha;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::MapRGBToPixval_Color

class ZDCPixmapNS::MapRGBToPixval_Color
	{
protected:
	MapRGBToPixval_Color() {}
	MapRGBToPixval_Color(const MapRGBToPixval_Color&) {}
	~MapRGBToPixval_Color() {}
	MapRGBToPixval_Color& operator=(const MapRGBToPixval_Color&) { return *this; }

public:
	uint32 AsPixval(const ZRGBColorPOD& iRGBColor) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32(iRGBColor.red) >> fShiftRightRed) << fShiftLeftRed);
		theValue |= ((uint32(iRGBColor.green) >> fShiftRightGreen) << fShiftLeftGreen);
		theValue |= ((uint32(iRGBColor.blue) >> fShiftRightBlue) << fShiftLeftBlue);
		theValue |= ((uint32(iRGBColor.alpha) >> fShiftRightAlpha) << fShiftLeftAlpha);
		return theValue;
		}

	uint32 AsPixval(const ZRGBColorSmallPOD& iRGBColor) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32(iRGBColor.red * 0x101U) >> fShiftRightRed) << fShiftLeftRed);
		theValue |= ((uint32(iRGBColor.green * 0x101U) >> fShiftRightGreen) << fShiftLeftGreen);
		theValue |= ((uint32(iRGBColor.blue * 0x101U) >> fShiftRightBlue) << fShiftLeftBlue);
		theValue |= ((uint32(iRGBColor.alpha * 0x101U) >> fShiftRightAlpha) << fShiftLeftAlpha);
		return theValue;
		}

	void AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int32 fShiftRightRed;
	int32 fShiftLeftRed;

	int32 fShiftRightGreen;
	int32 fShiftLeftGreen;

	int32 fShiftRightBlue;
	int32 fShiftLeftBlue;

	int32 fShiftRightAlpha;
	int32 fShiftLeftAlpha;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelDescRep

class ZDCPixmapNS::PixelDescRep : public ZRefCounted
	{
protected:
	PixelDescRep();
	PixelDescRep(const PixelDescRep& iOther) {}
	PixelDescRep& operator=(const PixelDescRep&) { return *this; }

public:
	virtual ~PixelDescRep();

	static bool sCheckAccessEnabled() { return false; }

	int GetChangeCount() const;
	int MarkChanged();

	virtual bool HasAlpha() = 0;
	virtual ZRef<PixelDescRep> WithoutAlpha() = 0;

	virtual void Imp_AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const = 0;
	virtual void Imp_AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const = 0;
	virtual void Imp_AsRGBColors(const uint32* iPixvals,
		size_t iCount, ZRGBColorPOD* oColors) const = 0;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const = 0;

	virtual uint32 Imp_AsPixval(const ZRGBColorPOD& iRGBColor) const = 0;
	virtual uint32 Imp_AsPixval(const ZRGBColorSmallPOD& iRGBColorSmall) const = 0;
	virtual void Imp_AsPixvals(const ZRGBColorPOD* iColors,
		size_t iCount, uint32* oPixvals) const = 0;

protected:
	ZThreadSafe_t fChangeCount;
	static ZThreadSafe_t sChangeCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelDescRep_Indexed

class ZDCPixmapNS::PixelDescRep_Indexed : public ZDCPixmapNS::PixelDescRep,
								public ZDCPixmapNS::MapPixvalToRGB_Indexed,
								public ZDCPixmapNS::MapRGBToPixval_Indexed
	{
public:
	PixelDescRep_Indexed(const ZRGBColorPOD* iColors, size_t iCount);
	PixelDescRep_Indexed(const ZRGBColorPOD* iColors, size_t iCount, bool iStripAlpha);

	PixelDescRep_Indexed(const ZRGBColorPOD* iColors, uint32* iPixvals, size_t iCount);
	PixelDescRep_Indexed(const ZRGBColorPOD* iColors,
		uint32* iPixvals, size_t iCount, bool iStripAlpha);

	PixelDescRep_Indexed(const ZRGBColorSmallPOD* iColors, size_t iCount);
	PixelDescRep_Indexed(const ZRGBColorSmallPOD* iColors, uint32* iPixvals, size_t iCount);

	PixelDescRep_Indexed(const ZRGBColorMap* iColorMap, size_t iCount);

	virtual ~PixelDescRep_Indexed();

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const;
	virtual void Imp_AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const;
	virtual void Imp_AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBColorPOD& iRGBColor) const;
	virtual uint32 Imp_AsPixval(const ZRGBColorSmallPOD& iRGBColorSmall) const;
	virtual void Imp_AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

// Our protocol
	void BuildReverseLookupIfNeeded() const;

	void GetColors(const ZRGBColorPOD*& oColors, size_t& oCount) const;

	bool Matches(const PixelDescRep_Indexed* iOther);

protected:
	bool fCheckedAlpha;
	bool fHasAlpha;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelDescRep_Gray

class ZDCPixmapNS::PixelDescRep_Gray : public ZDCPixmapNS::PixelDescRep,
								public ZDCPixmapNS::MapPixvalToRGB_Gray,
								public ZDCPixmapNS::MapRGBToPixval_Gray
	{
public:
	PixelDescRep_Gray(uint32 iMaskGray, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const;
	virtual void Imp_AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const;
	virtual void Imp_AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBColorPOD& iRGBColor) const;
	virtual uint32 Imp_AsPixval(const ZRGBColorSmallPOD& iRGBColorSmall) const;
	virtual void Imp_AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

// Our protocol
	void GetMasks(uint32& oMaskGray, uint32& oMaskAlpha) const;

	bool Matches(const PixelDescRep_Gray* iOther);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelDescRep_Color

class ZDCPixmapNS::PixelDescRep_Color : public ZDCPixmapNS::PixelDescRep,
								public ZDCPixmapNS::MapPixvalToRGB_Color,
								public ZDCPixmapNS::MapRGBToPixval_Color
	{
public:
	PixelDescRep_Color(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const;
	virtual void Imp_AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const;
	virtual void Imp_AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBColorPOD& iRGBColor) const;
	virtual uint32 Imp_AsPixval(const ZRGBColorSmallPOD& iRGBColorSmall) const;
	virtual void Imp_AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

	void GetMasks(uint32& oMaskRed, uint32& oMaskGreen, uint32& oMaskBlue, uint32& oMaskAlpha) const;

	bool Matches(const PixelDescRep_Color* iOther);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelDesc

class ZDCPixmapNS::PixelDesc
	{
public:
	PixelDesc();
	PixelDesc(const PixelDesc& iOther);
	PixelDesc(EFormatStandard iFormat);
	PixelDesc(const ZRGBColorPOD* iColors, size_t iCount); // indexed
	PixelDesc(const ZRGBColorSmallPOD* iColors, size_t iCount); // indexed
	PixelDesc(const ZRGBColorMap* iColorMap, size_t iCount); // indexed
	PixelDesc(uint32 iMaskGray, uint32 iMaskAlpha); // Gray
	PixelDesc(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha); // Color
	PixelDesc(const ZRef<PixelDescRep>& iPixelDescRep); // Generic
	~PixelDesc();
	PixelDesc& operator=(const PixelDesc& iOther);
	PixelDesc& operator=(const ZRef<PixelDescRep>& iPixelDescRep);

	ZRef<PixelDescRep> GetRep() const;

	int GetChangeCount() const;

	bool HasAlpha() const;
	PixelDesc WithoutAlpha() const;	

	void AsRGBColor(uint32 iPixval, ZRGBColorPOD& oColor) const;
	void AsRGBColorSmall(uint32 iPixval, ZRGBColorSmallPOD& oColor) const;
	void AsRGBColors(const uint32* iPixvals, size_t iCount, ZRGBColorPOD* oColors) const;
	uint16 AsAlpha(uint32 iPixval) const;

	uint32 AsPixval(const ZRGBColorPOD& iRGBColor) const;
	uint32 AsPixval(const ZRGBColorSmallPOD& iRGBColorSmall) const;
	void AsPixvals(const ZRGBColorPOD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	ZRef<PixelDescRep> fRep;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelIterR_T

namespace ZDCPixmapNS {

template <class P>
class PixelIterR_T
	{
public:
	PixelIterR_T(const void* iAddress,
		const PixvalDesc& iPixvalDesc, int iCoord, const P& iPixelDesc)
	:	fIter(iAddress, iPixvalDesc, iCoord),
		fPixelDesc(iPixelDesc)
		{}

	void ReadInc(ZRGBColorPOD& oColor)
		{ fPixelDesc.AsRGBColor(fIter.ReadInc(), oColor); }

	void Read(ZRGBColorPOD& oColor)
		{ fPixelDesc.AsRGBColor(fIter.Read(), oColor); }

	uint16 ReadAlphaInc()
		{ return fPixelDesc.AsAlpha(fIter.ReadInc()); }

	uint16 ReadAlpha()
		{ return fPixelDesc.AsAlpha(fIter.Read()); }

	void Reset(const void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterR fIter;
	const P& fPixelDesc;
	};

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelIterRW_T

namespace ZDCPixmapNS {

template <class P>
class PixelIterRW_T
	{
public:
	PixelIterRW_T(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord, const P& iPixelDesc)
	:	fIter(iAddress, iPixvalDesc, iCoord),
		fPixelDesc(iPixelDesc)
		{}

	void ReadInc(ZRGBColorPOD& oColor)
		{ fPixelDesc.AsRGBColor(fIter.ReadInc(), oColor); }

	void Read(ZRGBColorPOD& oColor)
		{ fPixelDesc.AsRGBColor(fIter.Read(), oColor); }

	void WriteInc(const ZRGBColorPOD& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const ZRGBColorPOD& iColor)
		{ fIter.Write(fPixelDesc.AsPixval(iColor)); }

	void Inc()
		{ fIter.Inc(); }

	void Reset(void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterRW fIter;
	const P& fPixelDesc;
	};

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS::PixelIterW_T

namespace ZDCPixmapNS {

template <class P>
class PixelIterW_T
	{
public:
	PixelIterW_T(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord, const P& iPixelDesc)
	:	fIter(iAddress, iPixvalDesc, iCoord),
		fPixelDesc(iPixelDesc)
		{}

	void WriteInc(const ZRGBColorPOD& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const ZRGBColorPOD& iColor)
		{ fIter.Write(fPixelDesc.AsPixval(iColor)); }

	void Inc()
		{ fIter.Inc(); }

	void Reset(void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterW fIter;
	const P& fPixelDesc;
	};

} // namespace ZDCPixmapNS

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapNS utilities, filling, munging and blitting

namespace ZDCPixmapNS {

void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval);
void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, const ZRect& iBounds, uint32 iPixval);

// ==================================================

void sMunge(void* iBaseAddress,
	const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc, const ZRect& iBounds,
	MungeProc iMungeProc, void* iRefcon);

// ==================================================

void sBlitPixvals(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const ZRect& iSourceBounds, ZPoint iDestLocation);

void sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDesc,
	const ZRect& iSourceBounds, ZPoint iDestLocation);

void sBlitRowPixvals(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, int32 iDestH,
	int32 iCount);

void sBlitRow(
	const void* iSourceBase, const PixvalDesc& iSourcePixvalDesc, const PixelDesc& iSourcePixelDesc,
	int32 iSourceH,
	void* iDestBase, const PixvalDesc& iDestPixvalDesc, const PixelDesc& iDestPixelDesc,
	int32 iDestH,
	int32 iCount);

} // namespace ZDCPixmapNS

// =================================================================================================

#endif // __ZDCPixmapNS__
