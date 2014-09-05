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

#include "zoolib/ZDCPixmapCoder_BMP.h"

#include <vector>

using std::runtime_error;
using std::vector;

namespace ZooLib {

using namespace ZDCPixmapNS;

#ifndef BI_RGB
	#define BI_RGB 0L
	#define BI_RLE8 1L
	#define BI_RLE4 2L
	#define BI_BITFIELDS 3L
#endif

// =================================================================================================
// MARK: - ZDCPixmapEncoder_BMP

void ZDCPixmapEncoder_BMP::sWritePixmap(const ZStreamW& iStream,
	const ZDCPixmap& iPixmap, bool iWithFileHeader)
	{
	ZDCPixmapEncoder_BMP theEncoder(iWithFileHeader);
	ZDCPixmapEncoder::sWritePixmap(iStream, iPixmap, theEncoder);
	}

ZDCPixmapEncoder_BMP::ZDCPixmapEncoder_BMP(bool iWithFileHeader)
:	fWithFileHeader(iWithFileHeader)
	{}

ZDCPixmapEncoder_BMP::~ZDCPixmapEncoder_BMP()
	{}

static int spNearestIndexedDepth(int iDepth)
	{
	ZAssert(iDepth >= 1 && iDepth <= 8);
	if (iDepth <= 1)
		return 1;
	if (iDepth <= 4)
		return 4;
	return 8;
	}

void ZDCPixmapEncoder_BMP::Imp_Write(const ZStreamW& iStream,
	const void* iBaseAddress,
	const RasterDesc& iRasterDesc,
	const PixelDesc& iPixelDesc,
	const ZRectPOD& iBounds)
	{
	if (fWithFileHeader)
		{
		iStream.WriteUInt16LE(0x4D42); // bfType
		iStream.WriteUInt32(0); // bfSize
		iStream.WriteUInt16(0); // bfReserved1
		iStream.WriteUInt16(0); // bfReserved2
		iStream.WriteUInt32LE(54); // bfOffBits
		}

	ZPointPOD theSize = WH(iBounds);

	iStream.WriteUInt32LE(0x28); // biSize
	iStream.WriteUInt32LE(theSize.h); // biWidth
	iStream.WriteUInt32LE(theSize.v); // biHeight
	iStream.WriteUInt16LE(1); // biPlanes

	PixvalDesc destPixvalDesc(0, true);
	// Depth is worked out below.

	PixelDesc destPixelDesc;

	const ZRGBA_POD* theColors;
	size_t theColorsCount = 0;

	ZRef<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();
	if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
		thePixelDescRep.DynamicCast<PixelDescRep_Indexed>())
		{
		destPixvalDesc.fDepth = spNearestIndexedDepth(iRasterDesc.fPixvalDesc.fDepth);
		destPixelDesc = thePixelDescRep_Indexed;
		thePixelDescRep_Indexed->GetColors(theColors, theColorsCount);
		}
	else
		{
		destPixvalDesc.fDepth = 24;
		destPixelDesc = PixelDesc(0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000);
		}

	iStream.WriteUInt16LE(destPixvalDesc.fDepth); // biBitCount

	// BI_RGB indicates an uncompressed image
	iStream.WriteUInt32LE(int32(BI_RGB)); // biCompression

	// A zero size indicates that the image consumes rowbytes * height
	iStream.WriteUInt32LE(0); // biSizeImage

	// We have no resolution information
	iStream.WriteUInt32LE(0); // biXPelsPerMeter
	iStream.WriteUInt32LE(0); // biYPelsPerMeter

	iStream.WriteUInt32LE(theColorsCount); // biClrUsed

	// biClrImportant indicates which colors in the table are most 'important'
	iStream.WriteUInt32(0); // biClrImportant

#if 0
	// This variant is quite a bit slower.
	for (size_t x = 0; x < theColorsCount; ++x)
		{
		iStream.WriteUInt8(theColors[x].blue >> 8);
		iStream.WriteUInt8(theColors[x].green >> 8);
		iStream.WriteUInt8(theColors[x].red >> 8);
		iStream.WriteUInt8(0);
		}
#else
	if (theColorsCount)
		{
		vector<uint8> tempColorsVector(theColorsCount * 4);

		const ZRGBA_POD* currentSource = theColors;
		uint8* currentDest = &tempColorsVector[0];

		for (size_t x = 0; x < theColorsCount; ++x)
			{
			*currentDest++ = currentSource->blue >> 8;
			*currentDest++ = currentSource->green >> 8;
			*currentDest++ = currentSource->red >> 8;
			++currentSource;
			*currentDest++ = 0;
			}
		iStream.Write(&tempColorsVector[0], tempColorsVector.size());
		}
#endif
	size_t rowBytes = sCalcRowBytes(theSize.h, destPixvalDesc.fDepth, 1);
	vector<uint8> theRowBufferVector(rowBytes);

	void* theRowBuffer = &theRowBufferVector[0];

	for (int y = iBounds.bottom; y > iBounds.top; --y)
		{
		const void* sourceRowAddress = iRasterDesc.CalcRowAddress(iBaseAddress, y - 1);
		sBlitRow(
			sourceRowAddress, iRasterDesc.fPixvalDesc, iPixelDesc, iBounds.left,
			theRowBuffer, destPixvalDesc, destPixelDesc, 0,
			W(iBounds));
		iStream.Write(theRowBuffer, rowBytes);
		}
	}

// =================================================================================================
// MARK: - ZDCPixmapDecoder_BMP

ZDCPixmap ZDCPixmapDecoder_BMP::sReadPixmap(const ZStreamR& iStream, bool iWithFileHeader)
	{
	ZDCPixmapDecoder_BMP theDecoder(iWithFileHeader);
	return ZDCPixmapDecoder::sReadPixmap(iStream, theDecoder);
	}

ZDCPixmapDecoder_BMP::ZDCPixmapDecoder_BMP(bool iWithFileHeader)
:	fWithFileHeader(iWithFileHeader)
	{}

ZDCPixmapDecoder_BMP::~ZDCPixmapDecoder_BMP()
	{}

static void spReadRLE8(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer);

static void spReadRLE4(const ZStreamR& iStream,
	ZCoord iWidth, ZCoord iHeight, size_t iRowBytes, bool iFlip, uint8* iBuffer);

void ZDCPixmapDecoder_BMP::Imp_Read(const ZStreamR& iStream, ZDCPixmap& oPixmap)
	{
	size_t bfOffBits = 0;
	if (fWithFileHeader)
		{
		uint16 bfType = iStream.ReadUInt16LE();
		// Ignore bfSize, bfReserved1 and bfReserved2.
		iStream.Skip(8);
		bfOffBits = iStream.ReadUInt32LE() - 54;
		if (bfType != 0x4D42)
			throw runtime_error("ZDCPixmapDecoder_BMP::Imp_Read, invalid BMP");
		}

	uint32 biSize = iStream.ReadUInt32LE();
	if (biSize != 0x28)
		throw runtime_error("ZDCPixmapDecoder_BMP::Imp_Read, invalid BMP");

	int32 biWidth = iStream.ReadUInt32LE();
	int32 biHeight = iStream.ReadUInt32LE();

	uint16 biPlanes = iStream.ReadUInt16LE();
	// We only handle single planes
	ZAssertStop(0, biPlanes == 1);

	uint16 biBitCount = iStream.ReadUInt16LE();
	uint32 biCompression = iStream.ReadUInt32LE();

	// Ignore biSizeImage, biXPelsPerMeter and biYPelsPerMeter.
	iStream.Skip(12);

	uint32 biClrUsed = iStream.ReadUInt32LE();
	/*uint32 biClrImportant =*/ iStream.ReadUInt32LE();

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
	vector<ZRGBA_POD> sourceColorVector;
	if (biClrUsed > 0)
		{
		// We reduce the number of calls to the stream substantially by
		// reading the color table en masse.

		sourceColorVector.resize(biClrUsed);

		vector<uint8> readColorTable(biClrUsed * 4);
		iStream.Read(&readColorTable[0], readColorTable.size());

		const uint8* readColor = &readColorTable[0];
		ZRGBA_POD* sourceColor = &sourceColorVector[0];
		for (size_t x = 0; x < biClrUsed; ++x)
			{
			sourceColor->blue = (*readColor++) * 0x101;
			sourceColor->green = (*readColor++) * 0x101;
			sourceColor->red = (*readColor++) * 0x101;
			++readColor; // Ignore rgbReserved
			sourceColor->alpha = 0xFFFFU;
			++sourceColor;
			}
		}

	// If we're coming from a BMP file bfOffBits (the munged version) will be non zero,
	// and might tell us that the pixels are actually at some different offset than
	// immediately after the color table
	if (bfOffBits)
		iStream.Skip(bfOffBits - (biClrUsed * 4));

	RasterDesc sourceRasterDesc;
	sourceRasterDesc.fPixvalDesc.fDepth = biBitCount;
	sourceRasterDesc.fPixvalDesc.fBigEndian = true;
	sourceRasterDesc.fRowBytes = ((biWidth * biBitCount + 31) / 32) * 4;
	sourceRasterDesc.fRowCount = biHeight;
	sourceRasterDesc.fFlipped = false;

	PixelDesc sourcePixelDesc;

	ZDCPixmap thePixmap;
	// For shallow/indexed/RLE images we create a rep that exactly matches the source format
	// For deep images we create a rep that is 'efficient', and transcribe each row as it comes in.
	switch (biBitCount)
		{
		case 1:
		case 4:
		case 8:
			{
			sourcePixelDesc =
				PixelDesc(&sourceColorVector[0], sourceColorVector.size());
			thePixmap = ZDCPixmap(sourceRasterDesc, sPointPOD(biWidth, biHeight), sourcePixelDesc);
			break;
			}
		case 16:
			{
			sourcePixelDesc = PixelDesc(0x7C00, 0x03E0, 0x001F, 0);
			sourceRasterDesc.fPixvalDesc.fBigEndian = false;
			thePixmap =
				ZDCPixmap(sPointPOD(biWidth, biHeight), eFormatEfficient_Color_16);
			break;
			}
		case 24:
			{
			sourcePixelDesc =
				PixelDesc(0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000);
			thePixmap =
				ZDCPixmap(sPointPOD(biWidth, biHeight), eFormatEfficient_Color_24);
			break;
			}
		case 32:
			{
			sourcePixelDesc =
				PixelDesc(0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000);
			thePixmap =
				ZDCPixmap(sPointPOD(biWidth, biHeight), eFormatEfficient_Color_32);
			break;
			}
		}

	switch (biCompression)
		{
		case BI_RGB:
			{
			sourceRasterDesc.fRowCount = 1;
			vector<uint8> rowBufferVector(sourceRasterDesc.fRowBytes);
			uint8* rowBuffer = &rowBufferVector[0];
			ZRectPOD sourceRect = sRectPOD(biWidth, 1);
			for (long y = 0; y < biHeight; ++y)
				{
				iStream.Read(rowBuffer, sourceRasterDesc.fRowBytes);
				if (sourceFlipped)
					{
					thePixmap.CopyFrom(sPointPOD(0, biHeight - y - 1),
						rowBuffer, sourceRasterDesc, sourcePixelDesc, sourceRect);
					}
				else
					{
					thePixmap.CopyFrom(sPointPOD(0, y),
						rowBuffer, sourceRasterDesc, sourcePixelDesc, sourceRect);
					}
				}
			break;
			}
		case BI_RLE8:
			{
			thePixmap.GetRaster()->Fill(0);
			bool destFlipped = (thePixmap.GetRasterDesc().fFlipped);
			spReadRLE8(iStream,
				biWidth, biHeight,
				sourceRasterDesc.fRowBytes,
				sourceFlipped != destFlipped,
				reinterpret_cast<uint8*>(thePixmap.GetRaster()->GetBaseAddress()));
			break;
			}
		case BI_RLE4:
			{
			thePixmap.GetRaster()->Fill(0);
			bool destFlipped = (thePixmap.GetRasterDesc().fFlipped);
			spReadRLE4(iStream,
				biWidth, biHeight,
				sourceRasterDesc.fRowBytes,
				destFlipped,
				reinterpret_cast<uint8*>(thePixmap.GetRaster()->GetBaseAddress()));
			break;
			}
		}

	oPixmap = thePixmap;
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
				+ iRowBytes * (iFlip ? iHeight - currentRow - 1 : currentRow)
				+ currentCol;

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

			for (int ii = 0; ii < count; ++ii)
				{
				if ((currentCol & 1) == 0)
					{
					if ((ii & 1) == 0)
						rowStart[currentCol / 2] = hi << 4;
					else
						rowStart[currentCol / 2] = lo << 4;
					}
				else
					{
					if ((ii & 1) == 0)
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
