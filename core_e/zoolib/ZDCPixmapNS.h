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

#ifndef __ZDCPixmapNS_h__
#define __ZDCPixmapNS_h__ 1
#include "zconfig.h"

#include "zoolib/CountedWithoutFinalize.h"

#include "zoolib/ZGeomPOD.h"
#include "zoolib/ZRGBA.h"
#include "zoolib/ZRef.h"

// For documentation, see ZDCPixmapNS.cpp

namespace ZooLib {

// =================================================================================================
// MARK: - Declarations of names in namespace ZDCPixmapNS

/// ZDCPixmapNS encompasses a suite of types and methods for working with fairly generic pixmaps.

namespace ZDCPixmapNS {

int sCalcRowBytes(int iWidth, int iDepth, int iByteRound);

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
	ZRGBA_POD& ioColor, void* iRefcon);

class PixelDescRep;
class PixelDescRep_Indexed;
class PixelDescRep_Gray;
class PixelDescRep_Color;
class PixelDesc;

} // namespace ZDCPixmapNS

// =================================================================================================
// MARK: - ZDCPixmapNS::EFormatEfficient

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
// MARK: - ZDCPixmapNS::EFormatStandard

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
	eFormatStandard_RGBA_16_BE,
	eFormatStandard_RGB_16_BE,

	eFormatStandard_xRGB_16_LE,
	eFormatStandard_ARGB_16_LE,
	eFormatStandard_RGBA_16_LE,
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
// MARK: - sMapEfficientToStandard

namespace ZDCPixmapNS {

EFormatStandard sMapEfficientToStandard(EFormatEfficient iFormat);

} // namespace ZDCPixmapNS

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalDesc

/** PixvalDesc describes how a single pixval is laid out. */

class ZDCPixmapNS::PixvalDesc
	{
public:
	PixvalDesc() {}
	PixvalDesc(EFormatStandard iFormat);

	PixvalDesc(int32 iDepth, bool iBigEndian)
	:	fDepth(iDepth), fBigEndian(iBigEndian)
		{}

	bool operator==(const PixvalDesc& iOther) const
		{ return fDepth == iOther.fDepth && fBigEndian == iOther.fBigEndian; }

	int32 fDepth;
	bool fBigEndian;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::RasterDesc

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

	RasterDesc(ZPointPOD iSize, EFormatStandard iFormat, bool iFlipped = false);

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
// MARK: - ZDCPixmapNS::PixvalAccessor

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
// MARK: - ZDCPixmapNS::PixvalIterR

class ZDCPixmapNS::PixvalIterR
	{
public:
	PixvalIterR(const void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	uint32 ReadInc();
	uint32 Read();

	void Inc();

	void Reset(const void* iAddress, int iCoord);

protected:
	const void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	};

// =================================================================================================
// MARK: - PixvalIterRW

class ZDCPixmapNS::PixvalIterRW
	{
public:
	PixvalIterRW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	uint32 ReadInc();
	uint32 Read();

	void WriteInc(uint32 iPixval);
	void Write(uint32 iPixval);

	void Inc();

	void Reset(void* iAddress, int iCoord);

private:
	void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixvalIterW

class ZDCPixmapNS::PixvalIterW
	{
public:
	PixvalIterW(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord);

	void WriteInc(uint32 iPixval);
	void Write(uint32 iPixval);

	void Inc();

	void Reset(void* iAddress, int iCoord);

private:
	void* fAddress;
	PixvalAccessor fAccessor;

	int fCoord;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Indexed

class ZDCPixmapNS::MapPixvalToRGB_Indexed
	{
protected:
	MapPixvalToRGB_Indexed() {}
	MapPixvalToRGB_Indexed(const MapPixvalToRGB_Indexed&) {}
	~MapPixvalToRGB_Indexed() {}
	MapPixvalToRGB_Indexed& operator=(const MapPixvalToRGB_Indexed&) { return *this; }

public:
	void AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
		{
		if (fPixvals)
			this->pAsRGBA(iPixval, oColor);
		else
			oColor = fColors[iPixval];
		}

	uint16 AsAlpha(uint32 iPixval) const
		{
		if (fPixvals)
			return this->pAsAlpha(iPixval);
		else
			return fColors[iPixval].alpha;
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;

protected:
	void pAsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const;
	uint16 pAsAlpha(uint32 iPixval) const;

	ZRGBA_POD* fColors;
	size_t fCount;
	uint32* fPixvals;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Gray

class ZDCPixmapNS::MapPixvalToRGB_Gray
	{
protected:
	MapPixvalToRGB_Gray() {}
	MapPixvalToRGB_Gray(const MapPixvalToRGB_Gray&) {}
	~MapPixvalToRGB_Gray() {}
	MapPixvalToRGB_Gray& operator=(const MapPixvalToRGB_Gray&) { return *this; }

public:
	static uint32 sConvert(
		uint32 iPixval, uint32 iMask, int32 iShift, uint32 iMultiplier, uint32 iAdd)
		{
		return iAdd + (((iPixval & iMask) >> iShift) * iMultiplier);
		}

	void AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
		{
		oColor.red = oColor.green = oColor.blue =
			sConvert(iPixval, fMaskL, fShiftL, fMultiplierL, fAddL) >> 16;

		oColor.alpha = sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}

	uint16 AsAlpha(uint32 iPixval) const
		{
		return sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;

protected:
	int32 fShiftL;
	uint32 fMultiplierL;
	uint32 fMaskL;
	uint32 fAddL;

	int32 fShiftA;
	uint32 fMultiplierA;
	uint32 fMaskA;
	uint32 fAddA;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapPixvalToRGB_Color

class ZDCPixmapNS::MapPixvalToRGB_Color
	{
protected:
	MapPixvalToRGB_Color() {}
	MapPixvalToRGB_Color(const MapPixvalToRGB_Color&) {}
	~MapPixvalToRGB_Color() {}
	MapPixvalToRGB_Color& operator=(const MapPixvalToRGB_Color&) { return *this; }

public:
	static uint32 sConvert(
		uint32 iPixval, uint32 iMask, int32 iShift, uint32 iMultiplier, uint32 iAdd)
		{
		return iAdd + (((iPixval & iMask) >> iShift) * iMultiplier);
		}

	void AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const
		{
		oColor.red = sConvert(iPixval, fMaskR, fShiftR, fMultiplierR, fAddR) >> 16;
		oColor.green = sConvert(iPixval, fMaskG, fShiftG, fMultiplierG, fAddG) >> 16;
		oColor.blue = sConvert(iPixval, fMaskB, fShiftB, fMultiplierB, fAddB) >> 16;
		oColor.alpha = sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}

	void AsRGBAs(const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;

	uint16 AsAlpha(uint32 iPixval) const
		{
		return sConvert(iPixval, fMaskA, fShiftA, fMultiplierA, fAddA) >> 16;
		}

protected:
	int32 fShiftR;
	uint32 fMultiplierR;
	uint32 fMaskR;
	uint32 fAddR;

	int32 fShiftG;
	uint32 fMultiplierG;
	uint32 fMaskG;
	uint32 fAddG;

	int32 fShiftB;
	uint32 fMultiplierB;
	uint32 fMaskB;
	uint32 fAddB;

	int32 fShiftA;
	uint32 fMultiplierA;
	uint32 fMaskA;
	uint32 fAddA;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Indexed

class ZDCPixmapNS::MapRGBToPixval_Indexed
	{
protected:
	MapRGBToPixval_Indexed() {}
	MapRGBToPixval_Indexed(const MapRGBToPixval_Indexed&) {}
	~MapRGBToPixval_Indexed() {}
	MapRGBToPixval_Indexed& operator=(const MapRGBToPixval_Indexed&) { return *this; }

public:
	uint32 AsPixval(const ZRGBA_POD& iRGBA) const
		{
		uint8 index = fReverseLookup[(iRGBA.blue >> 12)
			+ 16 * ((iRGBA.green >> 12)
			+ 16 * (iRGBA.red >> 12))];

		if (fPixvals)
			return fPixvals[index];
		return index;
		}

	void AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	mutable uint8* fReverseLookup;
	uint32* fPixvals;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Gray

class ZDCPixmapNS::MapRGBToPixval_Gray
	{
protected:
	MapRGBToPixval_Gray() {}
	MapRGBToPixval_Gray(const MapRGBToPixval_Gray&) {}
	~MapRGBToPixval_Gray() {}
	MapRGBToPixval_Gray& operator=(const MapRGBToPixval_Gray&) { return *this; }

public:
	uint32 AsPixval(const ZRGBA_POD& iRGBA) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32((uint32(iRGBA.red)
			+ uint32(iRGBA.green)
			+ uint32(iRGBA.blue)) / 3) >> fShiftRightL) << fShiftLeftL);

		theValue |= ((uint32(iRGBA.alpha) >> fShiftRightA) << fShiftLeftA);
		return theValue;
		}

	void AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int32 fShiftRightL;
	int32 fShiftLeftL;

	int32 fShiftRightA;
	int32 fShiftLeftA;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::MapRGBToPixval_Color

class ZDCPixmapNS::MapRGBToPixval_Color
	{
protected:
	MapRGBToPixval_Color() {}
	MapRGBToPixval_Color(const MapRGBToPixval_Color&) {}
	~MapRGBToPixval_Color() {}
	MapRGBToPixval_Color& operator=(const MapRGBToPixval_Color&) { return *this; }

public:
	uint32 AsPixval(const ZRGBA_POD& iRGBA) const
		{
		uint32 theValue = 0;
		theValue |= ((uint32(iRGBA.red) >> fShiftRightR) << fShiftLeftR);
		theValue |= ((uint32(iRGBA.green) >> fShiftRightG) << fShiftLeftG);
		theValue |= ((uint32(iRGBA.blue) >> fShiftRightB) << fShiftLeftB);
		theValue |= ((uint32(iRGBA.alpha) >> fShiftRightA) << fShiftLeftA);
		return theValue;
		}

	void AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	int32 fShiftRightR;
	int32 fShiftLeftR;

	int32 fShiftRightG;
	int32 fShiftLeftG;

	int32 fShiftRightB;
	int32 fShiftLeftB;

	int32 fShiftRightA;
	int32 fShiftLeftA;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep

class ZDCPixmapNS::PixelDescRep : public CountedWithoutFinalize
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

	virtual void Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const = 0;
	virtual void Imp_AsRGBAs(const uint32* iPixvals,
		size_t iCount, ZRGBA_POD* oColors) const = 0;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const = 0;

	virtual uint32 Imp_AsPixval(const ZRGBA_POD& iRGBA) const = 0;
	virtual void Imp_AsPixvals(const ZRGBA_POD* iColors,
		size_t iCount, uint32* oPixvals) const = 0;

protected:
	ThreadSafe_t fChangeCount;
	static ThreadSafe_t sChangeCount;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Indexed

class ZDCPixmapNS::PixelDescRep_Indexed
:	public ZDCPixmapNS::PixelDescRep,
	public ZDCPixmapNS::MapPixvalToRGB_Indexed,
	public ZDCPixmapNS::MapRGBToPixval_Indexed
	{
public:
	PixelDescRep_Indexed(const ZRGBA_POD* iColors, size_t iCount);
	PixelDescRep_Indexed(const ZRGBA_POD* iColors, size_t iCount, bool iStripAlpha);

	PixelDescRep_Indexed(const ZRGBA_POD* iColors, uint32* iPixvals, size_t iCount);
	PixelDescRep_Indexed(const ZRGBA_POD* iColors,
		uint32* iPixvals, size_t iCount, bool iStripAlpha);

	PixelDescRep_Indexed(const ZRGBAMap* iColorMap, size_t iCount);

	virtual ~PixelDescRep_Indexed();

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const;
	virtual void Imp_AsRGBAs(
		const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBA_POD& iRGBA) const;
	virtual void Imp_AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

// Our protocol
	void BuildReverseLookupIfNeeded() const;

	void GetColors(const ZRGBA_POD*& oColors, size_t& oCount) const;

	bool Matches(const PixelDescRep_Indexed* iOther);

protected:
	bool fCheckedAlpha;
	bool fHasAlpha;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Gray

class ZDCPixmapNS::PixelDescRep_Gray
:	public ZDCPixmapNS::PixelDescRep,
	public ZDCPixmapNS::MapPixvalToRGB_Gray,
	public ZDCPixmapNS::MapRGBToPixval_Gray
	{
public:
	PixelDescRep_Gray(uint32 iMaskGray, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const;
	virtual void Imp_AsRGBAs(
		const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBA_POD& iRGBA) const;
	virtual void Imp_AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

// Our protocol
	void GetMasks(uint32& oMaskL, uint32& oMaskA) const;

	bool Matches(const PixelDescRep_Gray* iOther);
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDescRep_Color

class ZDCPixmapNS::PixelDescRep_Color
:	public ZDCPixmapNS::PixelDescRep,
	public ZDCPixmapNS::MapPixvalToRGB_Color,
	public ZDCPixmapNS::MapRGBToPixval_Color
	{
public:
	PixelDescRep_Color(uint32 iMaskRed, uint32 iMaskGreen, uint32 iMaskBlue, uint32 iMaskAlpha);

// From PixelDescRep
	virtual bool HasAlpha();
	virtual ZRef<PixelDescRep> WithoutAlpha();

	virtual void Imp_AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const;
	virtual void Imp_AsRGBAs(
		const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;
	virtual uint16 Imp_AsAlpha(uint32 iPixval) const;

	virtual uint32 Imp_AsPixval(const ZRGBA_POD& iRGBA) const;
	virtual void Imp_AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

	void GetMasks(uint32& oMaskR, uint32& oMaskG, uint32& oMaskB, uint32& oMaskA) const;

	bool Matches(const PixelDescRep_Color* iOther);
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelDesc

class ZDCPixmapNS::PixelDesc
	{
public:
	PixelDesc();
	PixelDesc(const PixelDesc& iOther);
	PixelDesc(EFormatStandard iFormat);
	PixelDesc(const ZRGBA_POD* iColors, size_t iCount); // indexed
	PixelDesc(const ZRGBAMap* iColorMap, size_t iCount); // indexed
	PixelDesc(uint32 iMaskL, uint32 iMaskA); // Gray (Luminance/Alpha)
	PixelDesc(uint32 iMaskR, uint32 iMaskG, uint32 iMaskB, uint32 iMaskA); // Color
	PixelDesc(const ZRef<PixelDescRep>& iPixelDescRep); // Generic
	~PixelDesc();
	PixelDesc& operator=(const PixelDesc& iOther);
	PixelDesc& operator=(const ZRef<PixelDescRep>& iPixelDescRep);

	ZRef<PixelDescRep> GetRep() const;

	int GetChangeCount() const;

	bool HasAlpha() const;
	PixelDesc WithoutAlpha() const;

	void AsRGBA(uint32 iPixval, ZRGBA_POD& oColor) const;
	void AsRGBAs(const uint32* iPixvals, size_t iCount, ZRGBA_POD* oColors) const;
	uint16 AsAlpha(uint32 iPixval) const;

	uint32 AsPixval(const ZRGBA_POD& iRGBA) const;
	void AsPixvals(const ZRGBA_POD* iColors, size_t iCount, uint32* oPixvals) const;

protected:
	ZRef<PixelDescRep> fRep;
	};

// =================================================================================================
// MARK: - ZDCPixmapNS::PixelIterR_T

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

	void ReadInc(ZRGBA_POD& oColor)
		{ fPixelDesc.AsRGBA(fIter.ReadInc(), oColor); }

	void Read(ZRGBA_POD& oColor)
		{ fPixelDesc.AsRGBA(fIter.Read(), oColor); }

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
// MARK: - ZDCPixmapNS::PixelIterRW_T

namespace ZDCPixmapNS {

template <class P>
class PixelIterRW_T
	{
public:
	PixelIterRW_T(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord, const P& iPixelDesc)
	:	fIter(iAddress, iPixvalDesc, iCoord),
		fPixelDesc(iPixelDesc)
		{}

	void ReadInc(ZRGBA_POD& oColor)
		{ fPixelDesc.AsRGBA(fIter.ReadInc(), oColor); }

	void Read(ZRGBA_POD& oColor)
		{ fPixelDesc.AsRGBA(fIter.Read(), oColor); }

	void WriteInc(const ZRGBA_POD& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const ZRGBA_POD& iColor)
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
// MARK: - ZDCPixmapNS::PixelIterW_T

namespace ZDCPixmapNS {

template <class P>
class PixelIterW_T
	{
public:
	PixelIterW_T(void* iAddress, const PixvalDesc& iPixvalDesc, int iCoord, const P& iPixelDesc)
	:	fIter(iAddress, iPixvalDesc, iCoord),
		fPixelDesc(iPixelDesc)
		{}

	void WriteInc(const ZRGBA_POD& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const ZRGBA_POD& iColor)
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
// MARK: - ZDCPixmapNS utilities, filling, munging and blitting

namespace ZDCPixmapNS {

void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, uint32 iPixval);
void sFill(void* iBaseAddress, const RasterDesc& iRasterDesc, const ZRectPOD& iBounds, uint32 iPixval);

// ==================================================

void sMunge(void* iBaseAddress,
	const RasterDesc& iRasterDesc, const PixelDesc& iPixelDesc, const ZRectPOD& iBounds,
	MungeProc iMungeProc, void* iRefcon);

// ==================================================

void sBlitPixvals(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc,
	const ZRectPOD& iSourceBounds, ZPointPOD iDestLocation);

void sBlit(
	const void* iSourceBase, const RasterDesc& iSourceRasterDesc, const PixelDesc& iSourcePixelDesc,
	void* iDestBase, const RasterDesc& iDestRasterDesc, const PixelDesc& iDestPixelDesc,
	const ZRectPOD& iSourceBounds, ZPointPOD iDestLocation);

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

} // namespace ZooLib

#endif // __ZDCPixmapNS_h__
