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

#ifndef __ZDCPixmap_h__
#define __ZDCPixmap_h__ 1
#include "zconfig.h"

#include "zoolib/Multi.h"

#include "zoolib/ZCompat_NonCopyable.h"
#include "zoolib/ZCounted.h"
#include "zoolib/ZDCPixmapNS.h"

// For documentation, see ZDCPixmapNS.cpp

namespace ZooLib {

class ZDCPixmapCache;
class ZDCPixmapRep;
class ZDCPixmapRaster;

// =================================================================================================
// MARK: - ZDCPixmap

/// Stuff about ZDCPixmap

class ZDCPixmap
	{
public:
// The usual mantra: constructor, copy constructor, destructor, assignment
	ZDCPixmap();
	ZDCPixmap(const ZDCPixmap& other);
	~ZDCPixmap();
	ZDCPixmap& operator=(const ZDCPixmap& other);
	ZDCPixmap& operator=(const ZRef<ZDCPixmapRep>& iRep);

	ZDCPixmap(const null_t&);
	ZDCPixmap& operator=(const null_t&);

	/** This is the most general and flexible constructor, but it does require
	that client code create the ZDCPixmapRep, and thus that code has to deal with
	the details of ZDCPixmapRep's API. */
	ZDCPixmap(const ZRef<ZDCPixmapRep>& iRep);

	/** Temporary rescuscitation of old constructor. This method will be
	replaced with a more general composition API ASAP. */
	ZDCPixmap(const ZDCPixmap& iSource1, const ZDCPixmap& iSource2, const ZDCPixmap& iMask);

	/** Generally this constructor is the best to use. It takes an EFormatEfficient enum that
	specifies the depth and type of pixmap that's required, but makes no comment on the layout
	of the pixmap. Platform specific code does the work of interpreting what 'efficient'
	means for its purposes. */
	ZDCPixmap(ZPointPOD iSize, ZDCPixmapNS::EFormatEfficient iFormat);

	/** \overload */
	ZDCPixmap(ZPointPOD iSize, ZDCPixmapNS::EFormatEfficient iFormat, const ZRGBA_POD& iFillColor);

	/** This constructor is a shorthand for creating a pixmap that has a known pixel layout, where
	that layout is specified by the EFormatStandard enum that is passed in. The pixmap *may* be the
	same as what the 'efficient' would create, but that will depend entirely on which platform the
	code executes on. If iFillColor is non-nullptr, the pixmap's contents will be initialized to that
	color, otherwise its contents will be undefined. */
	ZDCPixmap(ZPointPOD iSize, ZDCPixmapNS::EFormatStandard iFormat);

	/** \overload */
	ZDCPixmap(ZPointPOD iSize, ZDCPixmapNS::EFormatStandard iFormat, const ZRGBA_POD& iFillColor);

	/** \overload */
	ZDCPixmap(const ZDCPixmapNS::RasterDesc& iRasterDesc,
		ZPointPOD iSize, const ZDCPixmapNS::PixelDesc& iPixelDesc);

	/** Initialized from a subset of a source. The actual bounds to be copied will be the
	intersection of \a iSourceBounds and the bounds of \a iSource. The new pixmap will most
	likely share iSource's raster, making this a very inexpensive operation, until a mutating
	operation occurs. */
	ZDCPixmap(const ZDCPixmap& iSource, const ZRectPOD& iSourceBounds);

	/** Referencing pixvals and copying a color table. */
	ZDCPixmap(ZPointPOD iSize, const uint8* iPixvals,
		const ZRGBA_POD* iColorTable, size_t iColorTableSize);

	/** Referencing pixVals and copying a color map. */
	ZDCPixmap(ZPointPOD iSize, const char* iPixvals,
		const ZRGBAMap* iColorMap, size_t iColorMapSize);

	bool operator==(const ZDCPixmap& other) const;

	/** Are we valid, i.e. do we have any pixels at all? */
	ZMACRO_operator_bool(ZDCPixmap, operator_bool) const
		{ return operator_bool_gen::translate(!!fRep); }

/** \name Dimensions
	The number of pixels horizontally and vertically.
*/	//@{
	ZPointPOD Size() const;
	ZCoord Width() const;
	ZCoord Height() const;
	//@}

/** \name Access to pixels, by RGB.
*/	//@{
	ZRGBA_POD GetPixel(const ZPointPOD& iLocation) const;
	ZRGBA_POD GetPixel(ZCoord iLocationH, ZCoord iLocationV) const;

	void SetPixel(const ZPointPOD& iLocation, const ZRGBA_POD& iColor);
	void SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBA_POD& iColor);
	//@}

/** \name Access to pixels, by pixel value (aka pixval.)
*/	//@{
	uint32 GetPixval(const ZPointPOD& iLocation) const;
	uint32 GetPixval(ZCoord iLocationH, ZCoord iLocationV) const;

	void SetPixval(const ZPointPOD& iLocation, uint32 iPixval);
	void SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval);
	//@}

/** \name Blitting rectangles of pixels
*/	//@{
	void CopyFrom(ZPointPOD iDestLocation, const ZDCPixmap& iSource, const ZRectPOD& iSourceBounds);

	void CopyFrom(ZPointPOD iDestLocation,
		const void* iSourceBaseAddress,
		const ZDCPixmapNS::RasterDesc& iSourceRasterDesc,
		const ZDCPixmapNS::PixelDesc& iSourcePixelDesc,
		const ZRectPOD& iSourceBounds);

	void CopyTo(ZPointPOD iDestLocation,
		void* iDestBaseAddress,
		const ZDCPixmapNS::RasterDesc& iDestRasterDesc,
		const ZDCPixmapNS::PixelDesc& iDestPixelDesc,
		const ZRectPOD& iSourceBounds) const;
	//@}

/** \name Applying a function to every pixel
*/	//@{
	void Munge(ZDCPixmapNS::MungeProc iMungeProc, void* iRefcon);
	void Munge(bool iMungeColorTable, ZDCPixmapNS::MungeProc iMungeProc, void* iRefcon);
	//@}

	/** Ensure the pixmap's representation is not shared. */
	void Touch();

	/** Access to our rep. */
	const ZRef<ZDCPixmapRep>& GetRep() const;

/** \name Access to other info
*/	//@{
	const ZRef<ZDCPixmapRaster>& GetRaster() const;

	const void* GetBaseAddress() const;
	void* GetBaseAddress();
	const ZDCPixmapNS::RasterDesc& GetRasterDesc() const;
	const ZDCPixmapNS::PixelDesc& GetPixelDesc() const;
	const ZRectPOD& GetBounds() const;
	//@}

private:
	void pTouch();
	ZRef<ZDCPixmapRep> fRep;
	};

// =================================================================================================
// MARK: - ZDCPixmapRaster

class ZDCPixmapRaster : public ZCounted, NonCopyable
	{
protected:
	ZDCPixmapRaster(const ZDCPixmapNS::RasterDesc& iRasterDesc);

public:
	static bool sCheckAccessEnabled() { return false; }

	virtual ~ZDCPixmapRaster();

	void* GetBaseAddress() { return fBaseAddress; }
	bool GetCanModify() { return fCanModify; }
	const ZDCPixmapNS::RasterDesc& GetRasterDesc() { return fRasterDesc; }

	void Fill(uint32 iPixval);

	uint32 GetPixval(ZCoord iLocationH, ZCoord iLocationV);
	void SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval);

protected:
	ZDCPixmapNS::RasterDesc fRasterDesc;
	bool fCanModify;
	void* fBaseAddress;
	};

// =================================================================================================
// MARK: - ZDCPixmapRaster_Simple

class ZDCPixmapRaster_Simple : public ZDCPixmapRaster
	{
public:
	ZDCPixmapRaster_Simple(const ZDCPixmapNS::RasterDesc& iRasterDesc);
	ZDCPixmapRaster_Simple(ZRef<ZDCPixmapRaster> iOther);
	virtual ~ZDCPixmapRaster_Simple();

protected:
	uint8* fBuffer;
	};

// =================================================================================================
// MARK: - ZDCPixmapRaster_StaticData

class ZDCPixmapRaster_StaticData : public ZDCPixmapRaster
	{
public:
	ZDCPixmapRaster_StaticData(const void* iBaseAddress,
		const ZDCPixmapNS::RasterDesc& iRasterDesc);

	ZDCPixmapRaster_StaticData(const uint8* iBaseAddress, ZCoord iWidth, ZCoord iHeight);

	ZDCPixmapRaster_StaticData(const char* iBaseAddress, ZCoord iWidth, ZCoord iHeight);

	virtual ~ZDCPixmapRaster_StaticData();
	};

// =================================================================================================
// MARK: - ZDCPixmapCache

class ZDCPixmapCache : public ZCounted
	{
protected:
	ZDCPixmapCache();
	ZDCPixmapCache(const ZDCPixmapCache& iOther);
	ZDCPixmapCache& operator=(const ZDCPixmapCache& iOther);

public:
	virtual ~ZDCPixmapCache();
	};

// =================================================================================================
// MARK: - ZDCPixmapRep

class ZDCPixmapRep : public ZCounted, NonCopyable
	{
protected:
	ZDCPixmapRep();

public:
	static bool sCheckAccessEnabled() { return false; }

// Factories
	typedef Multi3<ZRef<ZDCPixmapRaster>, ZRectPOD, ZDCPixmapNS::PixelDesc>
		CreateRaster_t;

	static ZRef<ZDCPixmapRep> sCreate(const ZRef<ZDCPixmapRaster>& iRaster,
		const ZRectPOD& iBounds, const ZDCPixmapNS::PixelDesc& iPixelDesc);

	typedef Multi3<ZDCPixmapNS::RasterDesc, ZRectPOD, ZDCPixmapNS::PixelDesc>
		CreateRasterDesc_t;

	static ZRef<ZDCPixmapRep> sCreate(const ZDCPixmapNS::RasterDesc& iRasterDesc,
		const ZRectPOD& iBounds, const ZDCPixmapNS::PixelDesc& iPixelDesc);

// ctor/dtor
	ZDCPixmapRep(const ZRef<ZDCPixmapRaster>& iRaster,
		const ZRectPOD& iBounds, const ZDCPixmapNS::PixelDesc& iPixelDesc);

	virtual ~ZDCPixmapRep();

// Access to pixels
	ZRGBA_POD GetPixel(ZCoord iLocationH, ZCoord iLocationV) const;
	void SetPixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBA_POD& iColor);

	uint32 GetPixval(ZCoord iLocationH, ZCoord iLocationV);
	void SetPixval(ZCoord iLocationH, ZCoord iLocationV, uint32 iPixval);

// Blitting
	void CopyFrom(ZPointPOD iDestLocation,
		const ZRef<ZDCPixmapRep>& iSourceRep, const ZRectPOD& iSourceBounds);

	void CopyFrom(ZPointPOD iDestLocation,
		const void* iSourceBaseAddress,
		const ZDCPixmapNS::RasterDesc& iSourceRasterDesc,
		const ZDCPixmapNS::PixelDesc& iSourcePixelDesc,
		const ZRectPOD& iSourceBounds);

	void CopyTo(ZPointPOD iDestLocation,
		void* iDestBaseAddress,
		const ZDCPixmapNS::RasterDesc& iDestRasterDesc,
		const ZDCPixmapNS::PixelDesc& iDestPixelDesc,
		const ZRectPOD& iSourceBounds);

// Attributes
	ZPointPOD GetSize();

	const ZRectPOD& GetBounds();

	const ZDCPixmapNS::PixelDesc& GetPixelDesc();

	const ZRef<ZDCPixmapRaster>& GetRaster();

	const ZRef<ZDCPixmapCache>& GetCache();
	void SetCache(ZRef<ZDCPixmapCache> iCache);

	virtual ZRef<ZDCPixmapRep> Touch();

protected:
	ZRef<ZDCPixmapRaster> fRaster;
	ZRectPOD fBounds;
	ZDCPixmapNS::PixelDesc fPixelDesc;
	ZRef<ZDCPixmapCache> fCache;
	};

// =================================================================================================
// MARK: - ZDCPixmap inline definitions

inline ZDCPixmap::ZDCPixmap()
	{}

inline ZDCPixmap::ZDCPixmap(const ZDCPixmap& other)
:	fRep(other.fRep)
	{}

inline ZDCPixmap::~ZDCPixmap()
	{}

inline ZDCPixmap& ZDCPixmap::operator=(const ZDCPixmap& other)
	{
	fRep = other.fRep;
	return *this;
	}

inline ZDCPixmap& ZDCPixmap::operator=(const ZRef<ZDCPixmapRep>& iRep)
	{
	fRep = iRep;
	return *this;
	}

inline
ZDCPixmap::ZDCPixmap(const null_t&)
	{}

inline
ZDCPixmap& ZDCPixmap::operator=(const null_t&)
	{
	fRep.Clear();
	return *this;
	}

inline ZDCPixmap::ZDCPixmap(const ZRef<ZDCPixmapRep>& iRep)
:	fRep(iRep)
	{}

inline ZRGBA_POD ZDCPixmap::GetPixel(const ZPointPOD& iLocation) const
	{ return this->GetPixel(iLocation.h, iLocation.v); }

inline uint32 ZDCPixmap::GetPixval(const ZPointPOD& iLocation) const
	{ return this->GetPixval(iLocation.h, iLocation.v); }

inline void ZDCPixmap::SetPixel(const ZPointPOD& iLocation, const ZRGBA_POD& iColor)
	{ this->SetPixel(iLocation.h, iLocation.v, iColor); }

inline void ZDCPixmap::SetPixval(const ZPointPOD& iLocation, uint32 iPixval)
	{ this->SetPixval(iLocation.h, iLocation.v, iPixval); }

inline void ZDCPixmap::Munge(ZDCPixmapNS::MungeProc iMungeProc, void* iRefcon)
	{ this->Munge(true, iMungeProc, iRefcon); }

inline const ZRef<ZDCPixmapRep>& ZDCPixmap::GetRep() const
	{ return fRep; }

inline const ZRef<ZDCPixmapRaster>& ZDCPixmap::GetRaster() const
	{ return fRep->GetRaster(); }

inline const void* ZDCPixmap::GetBaseAddress() const
	{ return fRep->GetRaster()->GetBaseAddress(); }

inline void* ZDCPixmap::GetBaseAddress()
	{ return fRep->GetRaster()->GetBaseAddress(); }

inline const ZDCPixmapNS::RasterDesc& ZDCPixmap::GetRasterDesc() const
	{ return fRep->GetRaster()->GetRasterDesc(); }

inline const ZDCPixmapNS::PixelDesc& ZDCPixmap::GetPixelDesc() const
	{ return fRep->GetPixelDesc(); }

inline const ZRectPOD& ZDCPixmap::GetBounds() const
	{ return fRep->GetBounds(); }

// =================================================================================================
// MARK: - ZDCPixmapRep inline definitions

inline const ZRectPOD& ZDCPixmapRep::GetBounds()
	{ return fBounds; }

inline ZPointPOD ZDCPixmapRep::GetSize()
	{ return WH(fBounds); }

inline const ZDCPixmapNS::PixelDesc& ZDCPixmapRep::GetPixelDesc()
	{ return fPixelDesc; }

inline const ZRef<ZDCPixmapRaster>& ZDCPixmapRep::GetRaster()
	{ return fRaster; }

// =================================================================================================

} // namespace ZooLib

#endif // __ZDCPixmap_h__
