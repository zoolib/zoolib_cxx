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

#include "zoolib/ZDCPixmap_Asset_BMP.h"
#include "zoolib/ZAsset.h"
#include "zoolib/ZDCPixmap.h"

using std::runtime_error;
using std::vector;

namespace ZooLib {

// From wingdi.h
#ifndef BI_RGB
	#define BI_RGB 0L
	#define BI_RLE8 1L
	#define BI_RLE4 2L
	#define BI_BITFIELDS 3L
#endif

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
// MARK: - ZDCPixmap_Asset_BMP::sGetPixmap

static void spReadRLE8(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer);

static void spReadRLE4(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer);

ZDCPixmap ZDCPixmap_Asset_BMP::sGetPixmap(const ZAsset& iAsset)
	{
	if (!iAsset)
		return ZDCPixmap();

	// Get the data loaded before we call OpenRPos -- if the asset is file-based
	// then it will create a memory based streamer if the data is already loaded.
	
	const void* theData;
	size_t theSize;
	iAsset.GetData(&theData, & theSize);

	ZRef<ZStreamerRPos> theStreamer = iAsset.OpenRPos();
	if (!theStreamer)
		return ZDCPixmap();
	
	const ZStreamRPos& theStream = theStreamer->GetStreamRPos();

	// Parse the header
	uint16 bfType = theStream.ReadUInt16LE();
	if (bfType != 0x4D42)
		throw runtime_error("ZDCPixmap_Asset_BMP::sGetPixmap, invalid BMP file");

	// Ignore bfSize, bfReserved1 and bfReserved2.
	theStream.Skip(8);

	size_t bfOffBits = theStream.ReadUInt32LE() - 54;

	uint32 biSize = theStream.ReadUInt32LE();
	if (biSize != 0x28)
		throw runtime_error("ZDCPixmap_Asset_BMP::sGetPixmap, invalid BMP file");

	int32 biWidth = theStream.ReadUInt32LE();
	int32 biHeight = theStream.ReadUInt32LE();

	uint16 biPlanes = theStream.ReadUInt16LE();
	// We only handle single planes
	ZAssertStop(0, biPlanes == 1);

	uint16 biBitCount = theStream.ReadUInt16LE();
	uint32 biCompression = theStream.ReadUInt32LE();

	// Ignore biSizeImage, biXPelsPerMeter and biYPelsPerMeter.
	theStream.Skip(12);

	uint32 biClrUsed = theStream.ReadUInt32LE();
	/*uint32 biClrImportant =*/ theStream.ReadUInt32LE();

	bool sourceFlipped = true;
	if (biHeight < 0)
		{
		sourceFlipped = false;
		biHeight = -biHeight;
		}

	if (biClrUsed == 0)
		{
		if (biBitCount <= 8)
			biClrUsed = 1 << biBitCount;
		}

	// Read in the color table, if any
	vector<ZRGBA_POD> localColorVector;
	if (biClrUsed > 0)
		{
		localColorVector.resize(biClrUsed);

		const uint8* theColors = static_cast<const uint8*>(theData) + theStream.GetPosition();
		theStream.Skip(biClrUsed * 4);

		for (size_t x = 0; x < biClrUsed; ++x)
			{
			ZRGBA_POD& theColor = localColorVector[x];
			theColor.blue = (*theColors++) * 0x101;
			theColor.green = (*theColors++) * 0x101;
			theColor.red = (*theColors++) * 0x101;
			++theColors; // Ignore rgbReserved
			theColor.alpha = 0xFFFFU;
			}
		}

	// If we're coming from a BMP file bfOffBits (the munged version) will be non zero,
	// and might tell us that the pixels are actually at some different offset than
	// immediately after the color table
	if (bfOffBits)
		theStream.Skip(bfOffBits - (biClrUsed * 4));

	if (biCompression == BI_RGB)
		{
		// Uncompressed pixel data can be used as is -- we just have to set up an
		// appropriate RasterDesc and PixelDesc, and pass a PixmapRaster_Asset to
		// ZDCPixmap's constructor. When the raster is no longer in use by the pixmap
		// (or anyone else) the ZAsset's destructor will be called and the underlying
		// asset rep will be one step closer to being released.

		ZDCPixmapNS::RasterDesc theRasterDesc;
		theRasterDesc.fPixvalDesc.fDepth = biBitCount;
		theRasterDesc.fPixvalDesc.fBigEndian = true;
		theRasterDesc.fRowBytes = ((biWidth * biBitCount + 31) / 32) * 4;
		theRasterDesc.fRowCount = biHeight;
		theRasterDesc.fFlipped = sourceFlipped;

		ZDCPixmapNS::PixelDesc thePixelDesc;
		switch (biBitCount)
			{
			case 1:
			case 4:
			case 8:
				{
				thePixelDesc
					= ZDCPixmapNS::PixelDesc(&localColorVector[0], localColorVector.size());
				break;
				}
			case 16:
				{
				thePixelDesc = ZDCPixmapNS::PixelDesc(0x7C00, 0x03E0, 0x001F, 0);
				theRasterDesc.fPixvalDesc.fBigEndian = false;
				break;
				}
			case 24:
				{
				thePixelDesc
					= ZDCPixmapNS::PixelDesc(0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000);
				break;
				}
			case 32:
				{
				thePixelDesc
					= ZDCPixmapNS::PixelDesc(0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000);
				break;
				}
			}

		ZRef<ZDCPixmapRaster> theRaster = new PixmapRaster_Asset(iAsset,
			static_cast<const char*>(theData) + theStream.GetPosition(), theRasterDesc);

		return ZDCPixmap(new ZDCPixmapRep(theRaster, ZRect(biWidth, biHeight), thePixelDesc));
		}
	else
		{
		// We've got RLE data and so can't use the pixel data as is -- it has
		// to be expanded into a real raster.
		ZAssertStop(2, biBitCount == 1 || biBitCount == 4 || biBitCount == 8);

		ZDCPixmapNS::RasterDesc sourceRasterDesc;
		sourceRasterDesc.fPixvalDesc.fDepth = biBitCount;
		sourceRasterDesc.fPixvalDesc.fBigEndian = true;
		sourceRasterDesc.fRowBytes = ((biWidth * biBitCount + 31) / 32) * 4;
		sourceRasterDesc.fRowCount = biHeight;
		sourceRasterDesc.fFlipped = false;

		ZDCPixmapNS::PixelDesc sourcePixelDesc(&localColorVector[0], localColorVector.size());

		ZDCPixmap thePixmap(sourceRasterDesc, ZPoint(biWidth, biHeight), sourcePixelDesc);

		if (biCompression == BI_RLE8)
			{
			thePixmap.GetRaster()->Fill(0);
			bool destFlipped = (thePixmap.GetRasterDesc().fFlipped);
			spReadRLE8(theStream, biWidth, biHeight,
				sourceRasterDesc.fRowBytes, sourceFlipped != destFlipped,
				reinterpret_cast<uint8*>(thePixmap.GetRaster()->GetBaseAddress()));
			}
		else
			{
			ZAssertStop(2, biCompression == BI_RLE4);
			thePixmap.GetRaster()->Fill(0);
			bool destFlipped = (thePixmap.GetRasterDesc().fFlipped);
			spReadRLE4(theStream, biWidth, biHeight,
				sourceRasterDesc.fRowBytes, destFlipped,
				reinterpret_cast<uint8*>(thePixmap.GetRaster()->GetBaseAddress()));
			}
		return thePixmap;
		}
	}

static void spReadRLE8(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer)
	{
	ZCoord currentRow = 0;
	ZCoord currentCol = 0;
	bool done = false;
	while (!done)
		{
		uint8 count = iStream.ReadUInt8();
		uint8 command = iStream.ReadUInt8();
		if (count == 0)
			{
			switch (command)
				{
				case 0: // Move to start of next row
					{
					currentRow += 1;
					currentCol = 0;
					break;
					}
				case 1: // All done
					{
					done = true;
					break;
					}
				case 2: // Offset by some relative amount
					{
					currentCol += iStream.ReadUInt8();
					currentRow += iStream.ReadUInt8();
					break;
					}
				default: // Absolute data follows -- the length is the value of 'command'
					{
					uint8* destAddress = iBuffer
						+ iRowBytes * (iFlip ? iHeight - currentRow - 1 : currentRow)
						+ currentCol;

					iStream.Read(destAddress, command);
					currentCol += command;
					// An odd number of bytes is followed by a pad byte.
					if ((command & 1) != 0)
						iStream.Skip(1);
					break;
					}
				}
			}
		else
			{
			// Store a run of bytes. The count is in 'count', the value is in 'command'.
			uint8* destAddress = iBuffer
				+ iRowBytes * (iFlip ? iHeight - currentRow - 1 : currentRow) + currentCol;

			for (int x = 0; x < count; ++x)
				*destAddress++ = command;
			currentCol += count;
			}
		}
	}

static void spReadRLE4(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer)
	{
	ZCoord currentRow = 0;
	ZCoord currentCol = 0;
	bool done = false;
	while (!done)
		{
		uint8 count = iStream.ReadUInt8();
		uint8 command = iStream.ReadUInt8();
		if (count == 0)
			{
			switch (command)
				{
				case 0: // Move to start of next row
					{
					currentRow += 1;
					currentCol = 0;
					break;
					}
				case 1: // All done
					{
					done = true;
					break;
					}
				case 2: // Offset by some relative amount
					{
					currentCol += iStream.ReadUInt8();
					currentRow += iStream.ReadUInt8();
					break;
					}
				default: // Absolute data follows -- the length is the value of 'command'
					{
					uint8* rowStart = iBuffer
						+ iRowBytes * (iFlip ? iHeight - currentRow - 1 : currentRow);

					uint8 hi, lo;
					for (int i = 0; i < command; ++i)
						{
						if ((i & 1) == 0)
							{
							uint8 data = iStream.ReadUInt8();
							hi = data >> 4;
							lo = data & 0x0f;
							}
						if ((currentCol & 1) == 0)
							{
							if ((i & 1) == 0)
								rowStart[currentCol / 2] = hi << 4;
							else
								rowStart[currentCol / 2] = lo << 4;
							}
						else
							{
							if ((i & 1) == 0)
								rowStart[currentCol / 2] |= hi;
							else
								rowStart[currentCol / 2] |= lo;
							}
						++currentCol;
						}
					switch (command & 0x03)
						{
						case 1: case 2:
							iStream.Skip(1);
							break;
						}
					break;
					}
				}
			}
		else
			{
			// Store a run of nibbles. The count is in 'count',
			// the values are packed into 'command'.
			uint8* rowStart = iBuffer
				+ iRowBytes * (iFlip ? iHeight - currentRow - 1 : currentRow);

			uint8 hi = command >> 4;
			uint8 lo = command & 0x0F;
			for (int i = 0; i < count; ++i)
				{
				if ((currentCol & 1) == 0)
					{
					if ((i & 1) == 0)
						rowStart[currentCol / 2] = hi << 4;
					else
						rowStart[currentCol / 2] = lo << 4;
					}
				else
					{
					if ((i & 1) == 0)
						rowStart[currentCol / 2] |= hi;
					else
						rowStart[currentCol / 2] |= lo;
					}
				++currentCol;
				}
			}
		}
	}

} // namespace ZooLib
