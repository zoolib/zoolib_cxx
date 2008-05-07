/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZDCPixmap.h"
#include "ZMemory.h" // For ZBlockCopy
#include "ZStream.h"

using namespace ZDCPixmapNS;

using std::min;
using std::max;
using std::vector;

/*
ZDCPixmamp has value semantics. It holds a refcounted pointer to a ZDCPixmapRep. A ZDCPixmapRep
holds a refcounted pointer to a ZDCPixmapRaster. fBounds indicates which rectangle of pixvals
within the raster are considered to be "our" pixvals. How the pixvals are turned into ZRGBColors
is described by fPixelDesc.

Finally a ZDCPixmapRep has a refcounted pointer to a ZDCPimapCache object. Anything which uses
the ZDCPixmapRep can put a ZDCPixmapCache object into the rep's fCache field. This allows us to
use arbitrary reps, but still let something like an x server attach cached info to the rep. When
the rep is being touched we either create a new rep and return it, or if it's not shared we
clear the cache.
*/

// =================================================================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_Pixmap 1

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapFactory

ZDCPixmapFactory* ZDCPixmapFactory::sHead;

ZDCPixmapFactory::ZDCPixmapFactory()
	{
	fNext = sHead;
	sHead = this;
	}

ZDCPixmapFactory::~ZDCPixmapFactory()
	{}

ZRef<ZDCPixmapRep> ZDCPixmapFactory::sCreateRep(const ZRef<ZDCPixmapRaster>& iRaster,
	const ZRect& iBounds, const PixelDesc& iPixelDesc)
	{
	for (ZDCPixmapFactory* current = sHead; current; current = current->fNext)
		{
		if (ZRef<ZDCPixmapRep> theRep = current->CreateRep(iRaster, iBounds, iPixelDesc))
			return theRep;
		}
	return new ZDCPixmapRep(iRaster, iBounds, iPixelDesc);	
	}

ZRef<ZDCPixmapRep> ZDCPixmapFactory::sCreateRep(const RasterDesc& iRasterDesc,
	const ZRect& iBounds, const PixelDesc& iPixelDesc)
	{
	for (ZDCPixmapFactory* current = sHead; current; current = current->fNext)
		{
		if (ZRef<ZDCPixmapRep> theRep = current->CreateRep(iRasterDesc, iBounds, iPixelDesc))
			return theRep;
		}

	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_Simple(iRasterDesc);
	return new ZDCPixmapRep(theRaster, iBounds, iPixelDesc);
	}

EFormatStandard ZDCPixmapFactory::sMapEfficientToStandard(EFormatEfficient iFormat)
	{
	using namespace ZDCPixmapNS;

	for (ZDCPixmapFactory* current = sHead; current; current = current->fNext)
		{
		EFormatStandard result = current->MapEfficientToStandard(iFormat);
		if (result != eFormatStandard_Invalid)
			return result;
		}

	EFormatStandard standardFormat = eFormatStandard_Invalid;
	switch (iFormat)
		{
		case eFormatEfficient_Gray_1: standardFormat = eFormatStandard_Gray_1; break;
		case eFormatEfficient_Gray_8: standardFormat = eFormatStandard_Gray_8; break;

		case eFormatEfficient_Color_16: standardFormat = eFormatStandard_RGB_16_BE; break;
		case eFormatEfficient_Color_24: standardFormat = eFormatStandard_RGB_24; break;
		case eFormatEfficient_Color_32: standardFormat = eFormatStandard_ARGB_32; break;
		default: ZUnimplemented();
		}
	return standardFormat;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmap

ZDCPixmap::ZDCPixmap(const ZDCPixmap& iSource1, const ZDCPixmap& iSource2, const ZDCPixmap& iMask)
	{
	// An undersized or missing image is considered to be all black, so a
	// missing mask means we're an exact copy of iSource1.
	if (!iMask)
		{
		fRep = iSource1.GetRep();
		return;
		}

	ZPoint source1Size = iSource1.Size();
	ZPoint source2Size = iSource2.Size();
	ZPoint maskSize = iMask.Size();
	ZPoint resultSize;
	resultSize.h = max(source1Size.h, max(source2Size.h, maskSize.h));
	resultSize.v = max(source1Size.v, max(source2Size.v, maskSize.v));

	EFormatStandard theStandardFormat
		= ZDCPixmapFactory::sMapEfficientToStandard(eFormatEfficient_Color_32);

	RasterDesc theRasterDesc(resultSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapFactory::sCreateRep(theRasterDesc, resultSize, thePixelDesc);

	// Do a simple version for now
	for (ZCoord y = 0; y < resultSize.v; ++y)
		{
		for (ZCoord x = 0; x < resultSize.h; ++x)
			{
			ZRGBColor source1Pixel = iSource1.GetPixel(x, y);
			ZRGBColor source2Pixel = iSource2.GetPixel(x, y);
			ZRGBColor maskPixel = iMask.GetPixel(x, y);
			this->SetPixel(x, y,
				source1Pixel * (ZRGBColor::sWhite - maskPixel) + source2Pixel * maskPixel);
			}
		}
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, EFormatEfficient iFormat)
	{
	EFormatStandard theStandardFormat = ZDCPixmapFactory::sMapEfficientToStandard(iFormat);

	RasterDesc theRasterDesc(iSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapFactory::sCreateRep(theRasterDesc, iSize, thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, EFormatEfficient iFormat, const ZRGBColorPOD& iFillColor)
	{
	EFormatStandard theStandardFormat = ZDCPixmapFactory::sMapEfficientToStandard(iFormat);

	RasterDesc theRasterDesc(iSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapFactory::sCreateRep(theRasterDesc, iSize, thePixelDesc);

	uint32 fillPixval = fRep->GetPixelDesc().AsPixval(iFillColor);
	fRep->GetRaster()->Fill(fillPixval);
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, EFormatStandard iFormat)
	{
	RasterDesc theRasterDesc(iSize, iFormat);
	PixelDesc thePixelDesc(iFormat);

	fRep = ZDCPixmapFactory::sCreateRep(theRasterDesc, iSize, thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, EFormatStandard iFormat, const ZRGBColorPOD& iFillColor)
	{
	RasterDesc theRasterDesc(iSize, iFormat);
	PixelDesc thePixelDesc(iFormat);

	fRep = ZDCPixmapFactory::sCreateRep(theRasterDesc, iSize, thePixelDesc);

	uint32 fillPixval = fRep->GetPixelDesc().AsPixval(iFillColor);
	fRep->GetRaster()->Fill(fillPixval);
	}

ZDCPixmap::ZDCPixmap(const RasterDesc& iRasterDesc, ZPoint iSize, const PixelDesc& iPixelDesc)
	{
	fRep = ZDCPixmapFactory::sCreateRep(iRasterDesc, iSize, iPixelDesc);
	}

ZDCPixmap::ZDCPixmap(const ZDCPixmap& iSource, const ZRect& iSourceBounds)
	{
	ZRef<ZDCPixmapRep> sourceRep = iSource.GetRep();
	if (!sourceRep)
		return;

	ZRect originalBounds = sourceRep->GetBounds();

	ZRect realBounds = (iSourceBounds + originalBounds.TopLeft()) & originalBounds;
	if (realBounds.IsEmpty())
		return;

	fRep = ZDCPixmapFactory::sCreateRep(sourceRep->GetRaster(),
		realBounds, sourceRep->GetPixelDesc());
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, const uint8* iPixvals,
	const ZRGBColorPOD* iColorTable, size_t iColorTableSize)
	{
	RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 8;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = iSize.h;
	theRasterDesc.fRowCount = iSize.v;
	theRasterDesc.fFlipped = false;

	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_StaticData(iPixvals, theRasterDesc);
	PixelDesc thePixelDesc(iColorTable, iColorTableSize);
	fRep = ZDCPixmapFactory::sCreateRep(theRaster, iSize, thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPoint iSize, const char* iPixvals,
	const ZRGBColorMap* iColorMap, size_t iColorMapSize)
	{
	RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 8;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = iSize.h;
	theRasterDesc.fRowCount = iSize.v;
	theRasterDesc.fFlipped = false;
	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_StaticData(iPixvals, theRasterDesc);
	PixelDesc thePixelDesc(iColorMap, iColorMapSize);
	fRep = ZDCPixmapFactory::sCreateRep(theRaster, iSize, thePixelDesc);
	}

bool ZDCPixmap::operator==(const ZDCPixmap& other) const
	{ return fRep == other.fRep; }

ZPoint ZDCPixmap::Size() const
	{
	if (fRep)
		return fRep->GetSize();
	return ZPoint::sZero;
	}

ZCoord ZDCPixmap::Width() const
	{
	if (fRep)
		return fRep->GetSize().h;
	return 0;
	}

ZCoord ZDCPixmap::Height() const
	{
	if (fRep)
		return fRep->GetSize().v;
	return 0;
	}

ZRGBColorPOD ZDCPixmap::GetPixel(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (!fRep)
		return ZRGBColor::sBlack;
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	return fRep->GetPixel(iLocationH, iLocationV);
	}

void ZDCPixmap::SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBColorPOD& iColor)
	{
	if (!fRep)
		return;

	this->pTouch();
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	fRep->SetPixel(iLocationH, iLocationV, iColor);
	}

uint32 ZDCPixmap::GetPixval(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (!fRep)
		return 0;
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	return fRep->GetPixval(iLocationH, iLocationV);
	}

void ZDCPixmap::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	if (!fRep)
		return;
	this->pTouch();
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	fRep->SetPixval(iLocationH, iLocationV, iPixval);
	}

void ZDCPixmap::CopyFrom(ZPoint iDestLocation, const ZDCPixmap& iSource, const ZRect& iSourceBounds)
	{
	if (!fRep)
		return;

	ZPoint destSize = fRep->GetSize();
	if (iDestLocation.h >= destSize.h || iDestLocation.v >= destSize.v)
		return;

	ZRef<ZDCPixmapRep> sourceRep = iSource.fRep;
	if (!sourceRep)
		return;

	// Copy parameters into modifiable locals
	ZRect realSourceBounds = iSourceBounds;
	ZPoint realDestLocation = iDestLocation;

	// Clip location and top left of bounds against zero
	if (realDestLocation.h < 0)
		{
		realSourceBounds.left -= realDestLocation.h;
		realDestLocation.h = 0;
		}
	if (realDestLocation.v < 0)
		{
		realSourceBounds.top -= realDestLocation.v;
		realDestLocation.v = 0;
		}

	if (realSourceBounds.left < 0)
		{
		realDestLocation.h -= realSourceBounds.left;
		realSourceBounds.left = 0;
		}
	if (realSourceBounds.top < 0)
		{
		realDestLocation.v -= realSourceBounds.top;
		realSourceBounds.top = 0;
		}

	// Get the limiting bounds for our source and dest
	ZRect availSourceBounds = sourceRep->GetBounds();
	ZRect availDestBounds = fRep->GetBounds();

	// Offset into actual coordinates
	realSourceBounds += availSourceBounds.TopLeft();
	realDestLocation += availDestBounds.TopLeft();

	realSourceBounds.right = min(realSourceBounds.right, availSourceBounds.right);
	realSourceBounds.bottom = min(realSourceBounds.bottom, availSourceBounds.bottom);

	// Bail if we've ended up with an empty rectangle
	if (realSourceBounds.IsEmpty())
		return;

	// Make sure we're not sharing our rep with anyone
	this->pTouch();

	fRep->CopyFrom(realDestLocation, sourceRep, realSourceBounds);
	}

void ZDCPixmap::CopyFrom(ZPoint iDestLocation,
			const void* iSourceBaseAddress,
			const RasterDesc& iSourceRasterDesc,
			const PixelDesc& iSourcePixelDesc,
			const ZRect& iSourceBounds)
	{
	if (!fRep)
		return;

	// Copy parameters into modifiable locals
	ZRect realSourceBounds = iSourceBounds;
	ZPoint realDestLocation = iDestLocation;

	// Clip location and top left of bounds against zero
	if (realDestLocation.h < 0)
		{
		realSourceBounds.left -= realDestLocation.h;
		realDestLocation.h = 0;
		}
	if (realDestLocation.v < 0)
		{
		realSourceBounds.top -= realDestLocation.v;
		realDestLocation.v = 0;
		}

	if (realSourceBounds.left < 0)
		{
		realDestLocation.h -= realSourceBounds.left;
		realSourceBounds.left = 0;
		}
	if (realSourceBounds.top < 0)
		{
		realDestLocation.v -= realSourceBounds.top;
		realSourceBounds.top = 0;
		}

	// Get the limiting bounds for our dest
	ZRect availDestBounds = fRep->GetBounds();

	// Offset into actual coordinates
	realDestLocation += availDestBounds.TopLeft();

	// Bail if we've ended up with an empty rectangle
	if (realSourceBounds.IsEmpty())
		return;

	// Make sure we're not sharing our rep with anyone
	this->pTouch();

	fRep->CopyFrom(realDestLocation,
		iSourceBaseAddress, iSourceRasterDesc, iSourcePixelDesc, realSourceBounds);
	}

void ZDCPixmap::CopyTo(ZPoint iDestLocation,
	void* iDestBaseAddress,
	const RasterDesc& iDestRasterDesc,
	const PixelDesc& iDestPixelDesc,
	const ZRect& iSourceBounds) const
	{
	if (!fRep)
		return;

	// Copy parameters into modifiable locals
	ZRect realSourceBounds = iSourceBounds;
	ZPoint realDestLocation = iDestLocation;

	// Clip location and top left of bounds against zero
	if (realDestLocation.h < 0)
		{
		realSourceBounds.left -= realDestLocation.h;
		realDestLocation.h = 0;
		}
	if (realDestLocation.v < 0)
		{
		realSourceBounds.top -= realDestLocation.v;
		realDestLocation.v = 0;
		}

	if (realSourceBounds.left < 0)
		{
		realDestLocation.h -= realSourceBounds.left;
		realSourceBounds.left = 0;
		}
	if (realSourceBounds.top < 0)
		{
		realDestLocation.v -= realSourceBounds.top;
		realSourceBounds.top = 0;
		}

	// Get the limiting bounds for our source
	ZRect availSourceBounds = fRep->GetBounds();

	// Offset into actual coordinates
	realSourceBounds += availSourceBounds.TopLeft();

	realSourceBounds.right = min(realSourceBounds.right, availSourceBounds.right);
	realSourceBounds.bottom = min(realSourceBounds.bottom, availSourceBounds.bottom);

	// Bail if we've ended up with an empty rectangle
	if (realSourceBounds.IsEmpty())
		return;

	fRep->CopyTo(realDestLocation,
		iDestBaseAddress, iDestRasterDesc, iDestPixelDesc, realSourceBounds);
	}

void ZDCPixmap::Munge(bool iMungeColorTable, MungeProc iMungeProc, void* iRefcon)
	{
	if (!fRep)
		return;

	if (iMungeColorTable)
		{
		if (PixelDescRep_Indexed* thePixelDescRep_Indexed
			= ZRefDynamicCast<PixelDescRep_Indexed>(fRep->GetPixelDesc().GetRep()))
			{
			const ZRGBColorPOD* theColors;
			size_t theCount;
			thePixelDescRep_Indexed->GetColors(theColors, theCount);
			vector<ZRGBColor> theVector;
			theVector.reserve(theCount);
			bool changedAny = false;
			for (size_t x = 0; x < theCount; ++x)
				{
				ZRGBColor currentRGBColor = theColors[x];
				if (iMungeProc(-1, -1, currentRGBColor, iRefcon))
					changedAny = true;
				theVector.push_back(currentRGBColor);
				}

			if (changedAny)
				{
				fRep = ZDCPixmapFactory::sCreateRep(fRep->GetRaster(),
					fRep->GetBounds(), PixelDesc(&theVector[0], theCount));
				}
			return;
			}
		}

	this->pTouch();

	sMunge(
		fRep->GetRaster()->GetBaseAddress(),
		fRep->GetRaster()->GetRasterDesc(),
		fRep->GetPixelDesc(),
		fRep->GetBounds(),
		iMungeProc, iRefcon);
	}

void ZDCPixmap::Touch()
	{
	if (fRep)
		fRep = fRep->Touch();
	}

void ZDCPixmap::pTouch()
	{
	if (fRep)
		fRep = fRep->Touch();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRaster

ZDCPixmapRaster::ZDCPixmapRaster(const RasterDesc& iRasterDesc)
:	fRasterDesc(iRasterDesc),
	fCanModify(false),
	fBaseAddress(nil)
	{}

ZDCPixmapRaster::~ZDCPixmapRaster()
	{
	// Sanity check. Subclasses are required to nil out our fBaseAddress field.
	// This helps to ensure that they have a proper constructor by effectively
	// requiring that they have a destructor.
	ZAssertStop(kDebug_Pixmap, fBaseAddress == nil);
	}

uint32 ZDCPixmapRaster::GetPixval(ZCoord iLocationH, ZCoord iLocationV)
	{
	const void* rowAddress = fRasterDesc.CalcRowAddress(fBaseAddress, iLocationV);
	return PixvalAccessor(fRasterDesc.fPixvalDesc).GetPixval(rowAddress, iLocationH);
	}

void ZDCPixmapRaster::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	void* rowAddress = fRasterDesc.CalcRowAddress(fBaseAddress, iLocationV);
	PixvalAccessor(fRasterDesc.fPixvalDesc).SetPixval(rowAddress, iLocationH, iPixval);
	}

void ZDCPixmapRaster::Fill(uint32 iPixval)
	{
	ZDCPixmapNS::sFill(fBaseAddress, fRasterDesc, iPixval);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRaster_Simple

ZDCPixmapRaster_Simple::ZDCPixmapRaster_Simple(const RasterDesc& iRasterDesc)
:	ZDCPixmapRaster(iRasterDesc)
	{
	fCanModify = true;
	fBuffer = new uint8[fRasterDesc.fRowBytes * fRasterDesc.fRowCount + 4];
	fBaseAddress = fBuffer;
	}

ZDCPixmapRaster_Simple::ZDCPixmapRaster_Simple(ZRef<ZDCPixmapRaster> iOther)
:	ZDCPixmapRaster(iOther->GetRasterDesc())
	{
	fCanModify = true;
	size_t bufferSize = fRasterDesc.fRowBytes * fRasterDesc.fRowCount;

	// Oversize the buffer slightly, to allow for some code in ZDCPixmapBlit that
	// may *read* past the end of a buffer in some situations.
	fBuffer = new uint8[bufferSize + 4];
	fBaseAddress = fBuffer;
	ZBlockCopy(iOther->GetBaseAddress(), fBuffer, bufferSize);
	}

ZDCPixmapRaster_Simple::~ZDCPixmapRaster_Simple()
	{
	delete[] fBuffer;
	fBaseAddress = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRaster_StaticData

ZDCPixmapRaster_StaticData::ZDCPixmapRaster_StaticData(const void* iBaseAddress,
	const RasterDesc& iRasterDesc)
:	ZDCPixmapRaster(iRasterDesc)
	{
	fBaseAddress = const_cast<void*>(iBaseAddress);
	}

ZDCPixmapRaster_StaticData::ZDCPixmapRaster_StaticData(const uint8* iBaseAddress,
	ZCoord iWidth, ZCoord iHeight)
:	ZDCPixmapRaster(RasterDesc(PixvalDesc(8, true), iWidth, iHeight, false))
	{
	fBaseAddress = const_cast<uint8*>(iBaseAddress);
	}

ZDCPixmapRaster_StaticData::ZDCPixmapRaster_StaticData(const char* iBaseAddress,
	ZCoord iWidth, ZCoord iHeight)
:	ZDCPixmapRaster(RasterDesc(PixvalDesc(8, true), iWidth, iHeight, false))
	{
	fBaseAddress = const_cast<char*>(iBaseAddress);
	}

ZDCPixmapRaster_StaticData::~ZDCPixmapRaster_StaticData()
	{
	fBaseAddress = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapCache

ZDCPixmapCache::ZDCPixmapCache()
	{}

ZDCPixmapCache::ZDCPixmapCache(const ZDCPixmapCache& iOther)
	{}

ZDCPixmapCache& ZDCPixmapCache::operator=(const ZDCPixmapCache& iOther)
	{ return *this; }

ZDCPixmapCache::~ZDCPixmapCache()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRep

ZDCPixmapRep::ZDCPixmapRep()
	{}

ZDCPixmapRep::ZDCPixmapRep(const ZRef<ZDCPixmapRaster>& iRaster,
	const ZRect& iBounds, const PixelDesc& iPixelDesc)
:	fRaster(iRaster),
	fBounds(iBounds),
	fPixelDesc(iPixelDesc)
	{}

ZDCPixmapRep::~ZDCPixmapRep()
	{}

ZRGBColorPOD ZDCPixmapRep::GetPixel(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (!fBounds.Contains(iLocationH, iLocationV))
		return ZRGBColor::sBlack;

	ZRGBColor result;
	fPixelDesc.AsRGBColor(fRaster->GetPixval(iLocationH, iLocationV), result);
	return result;
	}

void ZDCPixmapRep::SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBColorPOD& iColor)
	{
	if (!fBounds.Contains(iLocationH, iLocationV))
		return;

	fRaster->SetPixval(iLocationH, iLocationV, fPixelDesc.AsPixval(iColor));
	}

uint32 ZDCPixmapRep::GetPixval(ZCoord iLocationH, ZCoord iLocationV)
	{
	if (!fBounds.Contains(iLocationH, iLocationV))
		return 0;

	return fRaster->GetPixval(iLocationH, iLocationV);
	}

void ZDCPixmapRep::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	if (!fBounds.Contains(iLocationH, iLocationV))
		return;

	fRaster->SetPixval(iLocationH, iLocationV, iPixval);
	}

void ZDCPixmapRep::CopyFrom(ZPoint iDestLocation,
	const ZRef<ZDCPixmapRep>& iSourceRep, const ZRect& iSourceBounds)
	{
	if (kDebug_Pixmap <= ZCONFIG_Debug)
		{
		ZAssertStop(kDebug_Pixmap, iSourceBounds == (iSourceBounds & iSourceRep->GetBounds()));
	
		ZRect destBounds = iSourceBounds + (iDestLocation - iSourceBounds.TopLeft());
		ZAssertStop(kDebug_Pixmap, destBounds == (destBounds & fBounds));
		}

	ZDCPixmapNS::sBlit(
		iSourceRep->GetRaster()->GetBaseAddress(),
		iSourceRep->GetRaster()->GetRasterDesc(),
		iSourceRep->GetPixelDesc(),
		fRaster->GetBaseAddress(),
		fRaster->GetRasterDesc(),
		this->GetPixelDesc(),
		iSourceBounds,
		iDestLocation);
	}

void ZDCPixmapRep::CopyFrom(ZPoint iDestLocation,
	const void* iSourceBaseAddress,
	const RasterDesc& iSourceRasterDesc,
	const PixelDesc& iSourcePixelDesc,
	const ZRect& iSourceBounds)
	{
	if (kDebug_Pixmap <= ZCONFIG_Debug)
		{
		ZRect destBounds = iSourceBounds + (iDestLocation - iSourceBounds.TopLeft());
		ZAssertStop(kDebug_Pixmap, destBounds == (destBounds & fBounds));
		}

	ZDCPixmapNS::sBlit(
		iSourceBaseAddress, iSourceRasterDesc, iSourcePixelDesc,
		fRaster->GetBaseAddress(), fRaster->GetRasterDesc(), this->GetPixelDesc(),
		iSourceBounds, iDestLocation);
	}

void ZDCPixmapRep::CopyTo(ZPoint iDestLocation,
	void* iDestBaseAddress,
	const RasterDesc& iDestRasterDesc,
	const PixelDesc& iDestPixelDesc,
	const ZRect& iSourceBounds)
	{
	ZAssertStop(kDebug_Pixmap, iSourceBounds == (iSourceBounds & fBounds));

	ZDCPixmapNS::sBlit(
		fRaster->GetBaseAddress(), fRaster->GetRasterDesc(), this->GetPixelDesc(),
		iDestBaseAddress, iDestRasterDesc, iDestPixelDesc,
		iSourceBounds, iDestLocation);
	}

 
const ZRef<ZDCPixmapCache>& ZDCPixmapRep::GetCache()
	{ return fCache; }

void ZDCPixmapRep::SetCache(ZRef<ZDCPixmapCache> iCache)
	{ fCache = iCache; }

ZRef<ZDCPixmapRep> ZDCPixmapRep::Touch()
	{
	if (this->GetRefCount() > 1 || fRaster->GetRefCount() > 1 || fRaster->GetCanModify() == false)
		{
		const RasterDesc& ourRasterDesc = fRaster->GetRasterDesc();
		RasterDesc newRasterDesc = ourRasterDesc;
		newRasterDesc.fRowBytes = sCalcRowBytes(fBounds.Width(), newRasterDesc.fPixvalDesc.fDepth);
		newRasterDesc.fRowCount = fBounds.Height();

		ZRef<ZDCPixmapRep> newRep
			= ZDCPixmapFactory::sCreateRep(newRasterDesc, fBounds.Size(), fPixelDesc);

		newRep->CopyFrom(ZPoint(0,0),
			fRaster->GetBaseAddress(), ourRasterDesc, fPixelDesc, fBounds);

		return newRep;
		}
	else
		{
		// Clear any cache we might have.
		fCache.Clear();
		}
	return this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapCombo

ZDCPixmapCombo::ZDCPixmapCombo()
	{}

ZDCPixmapCombo::ZDCPixmapCombo(const ZDCPixmap& iColorPixmap,
	const ZDCPixmap& iMonoPixmap,
	const ZDCPixmap& iMaskPixmap)
:	fColorPixmap(iColorPixmap),
	fMonoPixmap(iMonoPixmap),
	fMaskPixmap(iMaskPixmap)
	{}

ZDCPixmapCombo::ZDCPixmapCombo(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap)
:	fColorPixmap(iMainPixmap),
	fMaskPixmap(iMaskPixmap)
	{}

ZDCPixmapCombo& ZDCPixmapCombo::operator=(const ZDCPixmapCombo& iOther)
	{
	fColorPixmap = iOther.fColorPixmap;
	fMonoPixmap = iOther.fMonoPixmap;
	fMaskPixmap = iOther.fMaskPixmap;
	return *this;
	}

bool ZDCPixmapCombo::operator==(const ZDCPixmapCombo& iOther) const
	{
	return fColorPixmap == iOther.fColorPixmap
		&& fMonoPixmap == iOther.fMonoPixmap
		&& fMaskPixmap == iOther.fMaskPixmap;
	}

void ZDCPixmapCombo::SetPixmaps(const ZDCPixmap& iColorPixmap,
	const ZDCPixmap& iMonoPixmap,
	const ZDCPixmap& iMaskPixmap)
	{
	fColorPixmap = iColorPixmap;
	fMonoPixmap = iMonoPixmap;
	fMaskPixmap = iMaskPixmap;
	}

void ZDCPixmapCombo::SetPixmaps(const ZDCPixmap& iMainPixmap, const ZDCPixmap& iMaskPixmap)
	{
	fColorPixmap = iMainPixmap;
	fMonoPixmap = ZDCPixmap();
	fMaskPixmap = iMaskPixmap;
	}

void ZDCPixmapCombo::GetPixmaps(ZDCPixmap& oColorPixmap,
	ZDCPixmap& oMonoPixmap,
	ZDCPixmap& oMaskPixmap) const
	{
	oColorPixmap = fColorPixmap;
	oMonoPixmap = fMonoPixmap;
	oMaskPixmap = fMaskPixmap;
	}

ZPoint ZDCPixmapCombo::Size() const
	{
	ZPoint theSize = ZPoint::sZero;
	ZPoint tempSize = fColorPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	tempSize = fMonoPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	tempSize = fMaskPixmap.Size();
	theSize.h = max(theSize.h, tempSize.h);
	theSize.v = max(theSize.v, tempSize.v);

	return theSize;
	}

ZDCPixmapCombo::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fColorPixmap || fMonoPixmap); }
	
// =================================================================================================
