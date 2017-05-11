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

#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZStream.h"

#include <vector>

namespace ZooLib {

using namespace ZDCPixmapNS;

using std::max;
using std::min;
using std::vector;

/*
ZDCPixmamp has value semantics. It holds a refcounted pointer to a ZDCPixmapRep. A ZDCPixmapRep
holds a refcounted pointer to a ZDCPixmapRaster. fBounds indicates which rectangle of pixvals
within the raster are considered to be "our" pixvals. How the pixvals are turned into ZRGBAs
is described by fPixelDesc.

Finally a ZDCPixmapRep has a refcounted pointer to a ZDCPimapCache object. Anything which uses
the ZDCPixmapRep can put a ZDCPixmapCache object into the rep's fCache field. This allows us to
use arbitrary reps, but still let something like an x server attach cached info to the rep. When
the rep is being touched we either create a new rep and return it, or if it's not shared we
clear the cache.
*/

// =================================================================================================
// MARK: - kDebug

#define kDebug_Pixmap 1

// =================================================================================================
// MARK: - ZDCPixmap

ZDCPixmap::ZDCPixmap(const ZDCPixmap& iSource1, const ZDCPixmap& iSource2, const ZDCPixmap& iMask)
	{
	// An undersized or missing image is considered to be all black, so a
	// missing mask means we're an exact copy of iSource1.
	if (not iMask)
		{
		fRep = iSource1.GetRep();
		return;
		}

	ZPointPOD source1Size = iSource1.Size();
	ZPointPOD source2Size = iSource2.Size();
	ZPointPOD maskSize = iMask.Size();
	ZPointPOD resultSize;
	resultSize.h = max(source1Size.h, max(source2Size.h, maskSize.h));
	resultSize.v = max(source1Size.v, max(source2Size.v, maskSize.v));

	EFormatStandard theStandardFormat =
		sMapEfficientToStandard(eFormatEfficient_Color_32);

	RasterDesc theRasterDesc(resultSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapRep::sCreate(theRasterDesc, sRect(resultSize), thePixelDesc);

	// Do a simple version for now
	for (ZCoord y = 0; y < resultSize.v; ++y)
		{
		for (ZCoord x = 0; x < resultSize.h; ++x)
			{
			ZRGBA source1Pixel = iSource1.GetPixel(x, y);
			ZRGBA source2Pixel = iSource2.GetPixel(x, y);
			ZRGBA maskPixel = iMask.GetPixel(x, y);
			this->SetPixel(x, y,
				source1Pixel * (ZRGBA::sWhite - maskPixel) + source2Pixel * maskPixel);
			}
		}
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, EFormatEfficient iFormat)
	{
	EFormatStandard theStandardFormat = sMapEfficientToStandard(iFormat);

	RasterDesc theRasterDesc(iSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapRep::sCreate(theRasterDesc, sRect(iSize), thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, EFormatEfficient iFormat, const ZRGBA_POD& iFillColor)
	{
	EFormatStandard theStandardFormat = sMapEfficientToStandard(iFormat);

	RasterDesc theRasterDesc(iSize, theStandardFormat);
	PixelDesc thePixelDesc(theStandardFormat);

	fRep = ZDCPixmapRep::sCreate(theRasterDesc, sRect(iSize), thePixelDesc);

	uint32 fillPixval = fRep->GetPixelDesc().AsPixval(iFillColor);
	fRep->GetRaster()->Fill(fillPixval);
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, EFormatStandard iFormat)
	{
	RasterDesc theRasterDesc(iSize, iFormat);
	PixelDesc thePixelDesc(iFormat);

	fRep = ZDCPixmapRep::sCreate(theRasterDesc, sRect(iSize), thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, EFormatStandard iFormat, const ZRGBA_POD& iFillColor)
	{
	RasterDesc theRasterDesc(iSize, iFormat);
	PixelDesc thePixelDesc(iFormat);

	fRep = ZDCPixmapRep::sCreate(theRasterDesc, sRect(iSize), thePixelDesc);

	uint32 fillPixval = fRep->GetPixelDesc().AsPixval(iFillColor);
	fRep->GetRaster()->Fill(fillPixval);
	}

ZDCPixmap::ZDCPixmap(const RasterDesc& iRasterDesc, ZPointPOD iSize, const PixelDesc& iPixelDesc)
	{
	fRep = ZDCPixmapRep::sCreate(iRasterDesc, sRect(iSize), iPixelDesc);
	}

ZDCPixmap::ZDCPixmap(const ZDCPixmap& iSource, const ZRectPOD& iSourceBounds)
	{
	ZRef<ZDCPixmapRep> sourceRep = iSource.GetRep();
	if (not sourceRep)
		return;

	ZRectPOD originalBounds = sourceRep->GetBounds();

	ZRectPOD realBounds = (iSourceBounds + LT(originalBounds)) & originalBounds;
	if (sIsEmpty(realBounds))
		return;

	fRep = ZDCPixmapRep::sCreate(
		sourceRep->GetRaster(), realBounds, sourceRep->GetPixelDesc());
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, const uint8* iPixvals,
	const ZRGBA_POD* iColorTable, size_t iColorTableSize)
	{
	RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 8;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = iSize.h;
	theRasterDesc.fRowCount = iSize.v;
	theRasterDesc.fFlipped = false;

	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_StaticData(iPixvals, theRasterDesc);
	PixelDesc thePixelDesc(iColorTable, iColorTableSize);
	fRep = ZDCPixmapRep::sCreate(theRaster, sRect(iSize), thePixelDesc);
	}

ZDCPixmap::ZDCPixmap(ZPointPOD iSize, const char* iPixvals,
	const ZRGBAMap* iColorMap, size_t iColorMapSize)
	{
	RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 8;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = iSize.h;
	theRasterDesc.fRowCount = iSize.v;
	theRasterDesc.fFlipped = false;
	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_StaticData(iPixvals, theRasterDesc);
	PixelDesc thePixelDesc(iColorMap, iColorMapSize);
	fRep = ZDCPixmapRep::sCreate(theRaster, sRect(iSize), thePixelDesc);
	}

bool ZDCPixmap::operator==(const ZDCPixmap& other) const
	{ return fRep == other.fRep; }

ZPointPOD ZDCPixmap::Size() const
	{
	if (fRep)
		return fRep->GetSize();
	return sPointPOD(0, 0);
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

ZRGBA_POD ZDCPixmap::GetPixel(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (not fRep)
		return ZRGBA::sBlack;
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	return fRep->GetPixel(iLocationH, iLocationV);
	}

void ZDCPixmap::SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBA_POD& iColor)
	{
	if (not fRep)
		return;

	this->pTouch();
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	fRep->SetPixel(iLocationH, iLocationV, iColor);
	}

uint32 ZDCPixmap::GetPixval(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (not fRep)
		return 0;
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	return fRep->GetPixval(iLocationH, iLocationV);
	}

void ZDCPixmap::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	if (not fRep)
		return;
	this->pTouch();
	iLocationH += fRep->GetBounds().left;
	iLocationV += fRep->GetBounds().top;
	fRep->SetPixval(iLocationH, iLocationV, iPixval);
	}

void ZDCPixmap::CopyFrom(ZPointPOD iDestLocation, const ZDCPixmap& iSource, const ZRectPOD& iSourceBounds)
	{
	if (ZRef<ZDCPixmapRep> sourceRep = iSource.fRep)
		{
		this->CopyFrom(iDestLocation,
			sourceRep->GetRaster()->GetBaseAddress(),
			sourceRep->GetRaster()->GetRasterDesc(),
			sourceRep->GetPixelDesc(),
			iSourceBounds + LT(sourceRep->GetBounds()));
		}
	}

void ZDCPixmap::CopyFrom(ZPointPOD iDestLocation,
	const void* iSourceBaseAddress,
	const RasterDesc& iSourceRasterDesc,
	const PixelDesc& iSourcePixelDesc,
	const ZRectPOD& iSourceBounds)
	{
	if (not fRep)
		return;

	const ZRectPOD repBounds = fRep->GetBounds();

	ZPointPOD realDestLocation = iDestLocation + LT(repBounds);

	ZRectPOD realSourceBounds = iSourceBounds;

	realSourceBounds =
		sWithWR(sMin(W(realSourceBounds), W(repBounds) - X(realDestLocation)), realSourceBounds);

	realSourceBounds =
		sWithHB(sMin(H(realSourceBounds), H(repBounds) - Y(realDestLocation)), realSourceBounds);

	if (L(realSourceBounds) < 0)
		{
		X(realDestLocation) = X(realDestLocation) + L(realSourceBounds);
		L(realSourceBounds) = 0;
		}

	if (T(realSourceBounds) < 0)
		{
		Y(realDestLocation) = Y(realDestLocation) + T(realSourceBounds);
		T(realSourceBounds) = 0;
		}

	// Bail if we've ended up with an empty rectangle
	if (sIsEmpty(realSourceBounds))
		return;

	// Make sure we're not sharing our rep with anyone
	this->pTouch();

	fRep->CopyFrom(realDestLocation,
		iSourceBaseAddress, iSourceRasterDesc, iSourcePixelDesc, realSourceBounds);
	}

void ZDCPixmap::CopyTo(ZPointPOD iDestLocation,
	void* iDestBaseAddress,
	const RasterDesc& iDestRasterDesc,
	const PixelDesc& iDestPixelDesc,
	const ZRectPOD& iSourceBounds) const
	{
	if (not fRep)
		return;

	// Copy parameters into modifiable locals
	ZRectPOD realSourceBounds = iSourceBounds;
	ZPointPOD realDestLocation = iDestLocation;

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
	ZRectPOD availSourceBounds = fRep->GetBounds();

	// Offset into actual coordinates
	realSourceBounds += LT(availSourceBounds);

	realSourceBounds.right = min(realSourceBounds.right, availSourceBounds.right);
	realSourceBounds.bottom = min(realSourceBounds.bottom, availSourceBounds.bottom);

	// Bail if we've ended up with an empty rectangle
	if (sIsEmpty(realSourceBounds))
		return;

	fRep->CopyTo(realDestLocation,
		iDestBaseAddress, iDestRasterDesc, iDestPixelDesc, realSourceBounds);
	}

void ZDCPixmap::Munge(bool iMungeColorTable, MungeProc iMungeProc, void* iRefcon)
	{
	if (not fRep)
		return;

	if (iMungeColorTable)
		{
		if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
			fRep->GetPixelDesc().GetRep().DynamicCast<PixelDescRep_Indexed>())
			{
			const ZRGBA_POD* theColors;
			size_t theCount;
			thePixelDescRep_Indexed->GetColors(theColors, theCount);
			vector<ZRGBA> theVector;
			theVector.reserve(theCount);
			bool changedAny = false;
			for (size_t x = 0; x < theCount; ++x)
				{
				ZRGBA currentRGBColor = theColors[x];
				if (iMungeProc(-1, -1, currentRGBColor, iRefcon))
					changedAny = true;
				theVector.push_back(currentRGBColor);
				}

			if (changedAny)
				{
				fRep = ZDCPixmapRep::sCreate(fRep->GetRaster(),
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
// MARK: - ZDCPixmapRaster

ZDCPixmapRaster::ZDCPixmapRaster(const RasterDesc& iRasterDesc)
:	fRasterDesc(iRasterDesc),
	fCanModify(false),
	fBaseAddress(nullptr)
	{}

ZDCPixmapRaster::~ZDCPixmapRaster()
	{
	// Sanity check. Subclasses are required to nil out our fBaseAddress field.
	// This helps to ensure that they have a proper constructor by effectively
	// requiring that they have a destructor.
	ZAssertStop(kDebug_Pixmap, fBaseAddress == nullptr);
	}

uint32 ZDCPixmapRaster::GetPixval(ZCoord iLocationH, ZCoord iLocationV)
	{
	const void* rowAddress = fRasterDesc.CalcRowAddress(fBaseAddress, iLocationV);
	return PixvalAccessor(fRasterDesc.fPixvalDesc).GetPixval(rowAddress, iLocationH);
	}

void ZDCPixmapRaster::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	void* rowAddress = fRasterDesc.CalcRowAddressDest(fBaseAddress, iLocationV);
	PixvalAccessor(fRasterDesc.fPixvalDesc).SetPixval(rowAddress, iLocationH, iPixval);
	}

void ZDCPixmapRaster::Fill(uint32 iPixval)
	{
	ZDCPixmapNS::sFill(fBaseAddress, fRasterDesc, iPixval);
	}

// =================================================================================================
// MARK: - ZDCPixmapRaster_Simple

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
	const size_t bufferSize = fRasterDesc.fRowBytes * fRasterDesc.fRowCount;

	// Oversize the buffer slightly, to allow for some code in ZDCPixmapBlit that
	// may *read* past the end of a buffer in some situations.
	fBuffer = new uint8[bufferSize + 4];
	fBaseAddress = fBuffer;
	sMemCopy(fBuffer, iOther->GetBaseAddress(), bufferSize);
	}

ZDCPixmapRaster_Simple::~ZDCPixmapRaster_Simple()
	{
	delete[] fBuffer;
	fBaseAddress = nullptr;
	}

// =================================================================================================
// MARK: - ZDCPixmapRaster_StaticData

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
	fBaseAddress = nullptr;
	}

// =================================================================================================
// MARK: - ZDCPixmapCache

ZDCPixmapCache::ZDCPixmapCache()
	{}

ZDCPixmapCache::ZDCPixmapCache(const ZDCPixmapCache& iOther)
	{}

ZDCPixmapCache& ZDCPixmapCache::operator=(const ZDCPixmapCache& iOther)
	{ return *this; }

ZDCPixmapCache::~ZDCPixmapCache()
	{}

// =================================================================================================
// MARK: - ZDCPixmapRep

ZRef<ZDCPixmapRep> ZDCPixmapRep::sCreate(
	const ZRef<ZDCPixmapRaster>& iRaster,
	const ZRectPOD& iBounds,
	const PixelDesc& iPixelDesc)
	{
	ZRef<ZDCPixmapRep> result;
	if (FunctionChain<ZRef<ZDCPixmapRep>, const CreateRaster_t&>
		::sInvoke(result, CreateRaster_t(iRaster, iBounds, iPixelDesc)))
		{
		return result;
		}
	return new ZDCPixmapRep(iRaster, iBounds, iPixelDesc);
	}

ZRef<ZDCPixmapRep> ZDCPixmapRep::sCreate(
	const RasterDesc& iRasterDesc,
	const ZRectPOD& iBounds,
	const PixelDesc& iPixelDesc)
	{
	ZRef<ZDCPixmapRep> result;
	if (FunctionChain<ZRef<ZDCPixmapRep>, const CreateRasterDesc_t&>
		::sInvoke(result, CreateRasterDesc_t(iRasterDesc, iBounds, iPixelDesc)))
		{
		return result;
		}

	ZRef<ZDCPixmapRaster> theRaster = new ZDCPixmapRaster_Simple(iRasterDesc);
	return new ZDCPixmapRep(theRaster, iBounds, iPixelDesc);
	}

ZDCPixmapRep::ZDCPixmapRep()
	{}

ZDCPixmapRep::ZDCPixmapRep(const ZRef<ZDCPixmapRaster>& iRaster,
	const ZRectPOD& iBounds, const PixelDesc& iPixelDesc)
:	fRaster(iRaster),
	fBounds(iBounds),
	fPixelDesc(iPixelDesc)
	{}

ZDCPixmapRep::~ZDCPixmapRep()
	{}

ZRGBA_POD ZDCPixmapRep::GetPixel(ZCoord iLocationH, ZCoord iLocationV) const
	{
	if (not sContains(fBounds, iLocationH, iLocationV))
		return ZRGBA::sBlack;

	ZRGBA result;
	fPixelDesc.AsRGBA(fRaster->GetPixval(iLocationH, iLocationV), result);
	return result;
	}

void ZDCPixmapRep::SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBA_POD& iColor)
	{
	if (sContains(fBounds, iLocationH, iLocationV))
		fRaster->SetPixval(iLocationH, iLocationV, fPixelDesc.AsPixval(iColor));
	}

uint32 ZDCPixmapRep::GetPixval(ZCoord iLocationH, ZCoord iLocationV)
	{
	if (sContains(fBounds, iLocationH, iLocationV))
		return fRaster->GetPixval(iLocationH, iLocationV);
	return 0;
	}

void ZDCPixmapRep::SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval)
	{
	if (sContains(fBounds, iLocationH, iLocationV))
		fRaster->SetPixval(iLocationH, iLocationV, iPixval);
	}

void ZDCPixmapRep::CopyFrom(ZPointPOD iDestLocation,
	const ZRef<ZDCPixmapRep>& iSourceRep, const ZRectPOD& iSourceBounds)
	{
	if (kDebug_Pixmap <= ZCONFIG_Debug)
		{
		ZAssertStop(kDebug_Pixmap, iSourceBounds == (iSourceBounds & iSourceRep->GetBounds()));

		ZRectPOD destBounds = iSourceBounds + (iDestLocation - LT(iSourceBounds));
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

void ZDCPixmapRep::CopyFrom(ZPointPOD iDestLocation,
	const void* iSourceBaseAddress,
	const RasterDesc& iSourceRasterDesc,
	const PixelDesc& iSourcePixelDesc,
	const ZRectPOD& iSourceBounds)
	{
	if (kDebug_Pixmap <= ZCONFIG_Debug)
		{
		ZRectPOD destBounds = iSourceBounds + (iDestLocation - LT(iSourceBounds));
		ZAssertStop(kDebug_Pixmap, destBounds == (destBounds & fBounds));
		}

	ZDCPixmapNS::sBlit(
		iSourceBaseAddress, iSourceRasterDesc, iSourcePixelDesc,
		fRaster->GetBaseAddress(), fRaster->GetRasterDesc(), this->GetPixelDesc(),
		iSourceBounds, iDestLocation);
	}

void ZDCPixmapRep::CopyTo(ZPointPOD iDestLocation,
	void* iDestBaseAddress,
	const RasterDesc& iDestRasterDesc,
	const PixelDesc& iDestPixelDesc,
	const ZRectPOD& iSourceBounds)
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
	if (this->IsShared() || fRaster->IsShared() || fRaster->GetCanModify() == false)
		{
		const RasterDesc& ourRasterDesc = fRaster->GetRasterDesc();
		RasterDesc newRasterDesc = ourRasterDesc;
		newRasterDesc.fRowBytes =
			sCalcRowBytes(W(fBounds), newRasterDesc.fPixvalDesc.fDepth, 4);
		newRasterDesc.fRowCount = H(fBounds);

		ZRef<ZDCPixmapRep> newRep =
			sCreate(newRasterDesc, sRect(WH(fBounds)), fPixelDesc);

		newRep->CopyFrom(sPointPOD(0,0),
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

} // namespace ZooLib
