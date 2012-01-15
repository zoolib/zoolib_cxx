/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZDCGlyphServer_Asset.h"
#include "zoolib/ZString.h"

using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - Helper functions

static int16 spReadAsset_Int16(const ZAsset& iAsset)
	{
	try
		{
		if (ZRef<ZStreamerR> theStreamer = iAsset.GetChild("!binary").OpenR())
			return theStreamer->GetStreamR().ReadInt16();
		}
	catch (...)
		{}

	return 0;
	}

static int16 spReadAsset_Int8(const ZAsset& iAsset)
	{
	try
		{
		if (ZRef<ZStreamerR> theStreamer = iAsset.GetChild("!binary").OpenR())
			return theStreamer->GetStreamR().ReadInt8();
		}
	catch (...)
		{}

	return 0;
	}

static string spReadAsset_String(const ZAsset& iAsset)
	{
	try
		{
		if (ZAsset innerAsset = iAsset.GetChild("!string"))
			{
			const void* data;
			size_t size;
			innerAsset.GetData(&data, &size);
			if (data && size)
				return string(static_cast<const char*>(data), size - 1);
			}
		}
	catch (...)
		{}

	return string();
	}

static ZPoint spReadAsset_ZPoint(const ZAsset& iAsset)
	{
	try
		{
		if (ZRef<ZStreamerR> theStreamer = iAsset.GetChild("!binary").OpenR())
			{
			ZPoint result;
			result.h = theStreamer->GetStreamR().ReadInt16();
			result.v = theStreamer->GetStreamR().ReadInt16();
			return result;
			}
		}
	catch (...)
		{}

	return ZPoint(0, 0);
	}

// =================================================================================================
// MARK: - PixmapRaster_Asset

namespace { // anonymous

class PixmapRaster_Asset : public ZDCPixmapRaster
	{
public:
	PixmapRaster_Asset(const ZAsset& iAsset,
		const void* iBaseAddress, const ZDCPixmapNS::RasterDesc& iRasterDesc);
	virtual ~PixmapRaster_Asset();

private:
	ZAsset fAsset;
	};

PixmapRaster_Asset::PixmapRaster_Asset(const ZAsset& iAsset,
	const void* iBaseAddress, const ZDCPixmapNS::RasterDesc& iRasterDesc)
:	ZDCPixmapRaster(iRasterDesc),
	fAsset(iAsset)
	{
	fBaseAddress = const_cast<void*>(iBaseAddress);
	}

PixmapRaster_Asset::~PixmapRaster_Asset()
	{
	fBaseAddress = nullptr;
	}

} // anonymous namespace

// =================================================================================================
// MARK: - ZDCGlyphServer_Asset

ZDCGlyphServer_Asset::ZDCGlyphServer_Asset(const ZAsset& iAsset)
:	fAsset(iAsset)
	{}

ZDCGlyphServer_Asset::~ZDCGlyphServer_Asset()
	{}

static ZAsset spGetFontAsset(const ZAsset& iParent, const string& iName)
	{
	if (iName.size())
		{
		if (ZAsset theAsset = iParent.GetChild(iName))
			return theAsset;
		}

	if (ZAssetIter theIter = iParent)
		{
		// Return the first child
		return theIter.Current();
		}
	return ZAsset();
	}

bool ZDCGlyphServer_Asset::GetGlyph(const ZDCFont& iFont, UTF32 iCP,
	ZPoint& oOrigin, ZCoord& oEscapement, ZDCPixmap& oPixmap)
	{
	if (ZAsset fontAsset = spGetFontAsset(fAsset, iFont.GetName()))
		{
		if (ZAsset metricsAsset
			= fontAsset.GetChild("metrics").GetChild(ZString::sFormat("%06X", iCP)))
			{
			oOrigin = spReadAsset_ZPoint(metricsAsset.GetChild("origin"));
			oEscapement = spReadAsset_Int16(metricsAsset.GetChild("escapement"));
			string strikeName = spReadAsset_String(metricsAsset.GetChild("strikename"));
			if (ZAsset strikeAsset = fontAsset.GetChild("strikes").GetChild(strikeName))
				{
				ZCoord offsetInStrike = spReadAsset_Int16(metricsAsset.GetChild("strikeoffset"));
				ZCoord width = spReadAsset_Int16(metricsAsset.GetChild("width"));

				ZDCPixmapNS::RasterDesc theRasterDesc;
				theRasterDesc.fPixvalDesc.fDepth = spReadAsset_Int8(strikeAsset.GetChild("depth"));
				theRasterDesc.fPixvalDesc.fBigEndian = true;
				theRasterDesc.fRowBytes = spReadAsset_Int16(strikeAsset.GetChild("rowBytes"));
				theRasterDesc.fRowCount = spReadAsset_Int16(strikeAsset.GetChild("height"));
				theRasterDesc.fFlipped = false;
				if (ZAsset strikeDataAsset = strikeAsset.GetChild("data|!binary"))
					{
					const void* assetData;
					strikeDataAsset.GetData(&assetData, nullptr);

					ZRef<ZDCPixmapRaster> theRaster
						= new PixmapRaster_Asset(strikeDataAsset, assetData, theRasterDesc);
					
					ZRect theBounds(width, theRasterDesc.fRowCount);
					theBounds += ZPoint(offsetInStrike, 0);

					ZDCPixmapNS::PixelDesc thePixelDesc(uint32(0),
						(1 << theRasterDesc.fPixvalDesc.fDepth) - 1);

					oPixmap = ZDCPixmapRep::sCreate(theRaster, theBounds, thePixelDesc);
					return true;
					}
				}
			}
		}
	return false;
	}

ZCoord ZDCGlyphServer_Asset::GetEscapement(const ZDCFont& iFont, UTF32 iCP)
	{
	if (ZAsset fontAsset = spGetFontAsset(fAsset, iFont.GetName()))
		{
		if (ZAsset metricsAsset
			= fontAsset.GetChild("metrics").GetChild(ZString::sFormat("%06X", iCP)))
			{
			return spReadAsset_Int16(metricsAsset.GetChild("escapement"));
			}
		}
	return 0;
	}

void ZDCGlyphServer_Asset::GetFontInfo(const ZDCFont& iFont,
	ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading)
	{
	oAscent = 1;
	oDescent = 1;
	oLeading = 0;
	if (ZAsset fontAsset = spGetFontAsset(fAsset, iFont.GetName()))
		{
		oAscent = spReadAsset_Int16(fontAsset.GetChild("ascent"));
		oDescent = spReadAsset_Int16(fontAsset.GetChild("descent"));
		oLeading = spReadAsset_Int16(fontAsset.GetChild("leading"));
		}	
	}

} // namespace ZooLib
