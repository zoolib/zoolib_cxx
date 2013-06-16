/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#include "zoolib/ZDCPixmapRep_DIB.h"

#if ZCONFIG_SPI_Enabled(GDI)

#include <vector>

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - ZDCPixmapRep_DIB definition

static bool spCheckDesc(const ZDCPixmapNS::RasterDesc& iRasterDesc, const ZRectPOD& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
	{
	using namespace ZDCPixmapNS;

	if ((iRasterDesc.fRowBytes % 4) != 0)
		return false;

	bool isOkay = false;
	ZRef<PixelDescRep> theRep = iPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color = theRep.DynamicCast<PixelDescRep_Color>())
		{
		uint32 maskRed, maskGreen, maskBlue, maskAlpha;
		thePixelDescRep_Color->GetMasks(maskRed, maskGreen, maskBlue, maskAlpha);
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 16:
				{
				if (iRasterDesc.fPixvalDesc.fBigEndian == false
					&& maskRed == 0x7C00 && maskGreen == 0x03E0 && maskBlue == 0x001F)
					{
					isOkay = true;
					}
				break;
				}
			case 24:
				{
				if ((iRasterDesc.fPixvalDesc.fBigEndian == true
						&& maskRed == 0x0000FF && maskGreen == 0x00FF00 && maskBlue == 0xFF0000)
					|| (iRasterDesc.fPixvalDesc.fBigEndian == false
						&& maskRed == 0xFF0000 && maskGreen == 0x00FF00 && maskBlue == 0x0000FF))
					{
					isOkay = true;
					}
				break;
				}
			case 32:
				{
				if ((iRasterDesc.fPixvalDesc.fBigEndian == true
						&& maskRed == 0x0000FF00 && maskGreen == 0x00FF0000 && maskBlue == 0xFF000000)
					|| (iRasterDesc.fPixvalDesc.fBigEndian == false
						&& maskRed == 0x00FF0000 && maskGreen == 0x0000FF00 && maskBlue == 0x000000FF))
					{
					isOkay = true;
					}
				break;
				}
			}
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = theRep.DynamicCast<PixelDescRep_Gray>())
		{
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				isOkay = true;
				}
			}
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed =
		theRep.DynamicCast<PixelDescRep_Indexed>())
		{
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				isOkay = true;
				}
			}
		}
	return isOkay;
	}

static ZRef<ZDCPixmapRep_DIB> spCreateRepForDesc(
	ZRef<ZDCPixmapRaster> iRaster,
	const ZRectPOD& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
	{
	if (spCheckDesc(iRaster->GetRasterDesc(), iBounds, iPixelDesc))
		return new ZDCPixmapRep_DIB(iRaster, iBounds, iPixelDesc);

	return null;
	}

static BITMAPINFO* spAllocateBITMAPINFOColor(
	size_t iRowBytes, int32 iDepth, int32 iHeight, bool iFlipped)
	{
	BITMAPINFO* theBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER)]);

	theBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	theBITMAPINFO->bmiHeader.biWidth = (iRowBytes * 8) / iDepth;
	if (iFlipped)
		theBITMAPINFO->bmiHeader.biHeight = iHeight;
	else
		theBITMAPINFO->bmiHeader.biHeight = -iHeight;
	theBITMAPINFO->bmiHeader.biSizeImage = iRowBytes * iHeight;
	theBITMAPINFO->bmiHeader.biPlanes = 1;
	theBITMAPINFO->bmiHeader.biBitCount = iDepth;
	theBITMAPINFO->bmiHeader.biCompression = BI_RGB;
	theBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
	theBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
	theBITMAPINFO->bmiHeader.biClrUsed = 0;
	theBITMAPINFO->bmiHeader.biClrImportant = 0;
	return theBITMAPINFO;
	}

static bool spSetupDIB(
	const ZDCPixmapNS::RasterDesc& iRasterDesc, const ZDCPixmapNS::PixelDesc& iPixelDesc,
	BITMAPINFO*& oBITMAPINFO)
	{
	using namespace ZDCPixmapNS;

	if ((iRasterDesc.fRowBytes % 4) != 0)
		return false;

	bool isOkay = false;
	ZRef<PixelDescRep> theRep = iPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color = theRep.DynamicCast<PixelDescRep_Color>())
		{
		uint32 maskRed, maskGreen, maskBlue, maskAlpha;
		thePixelDescRep_Color->GetMasks(maskRed, maskGreen, maskBlue, maskAlpha);
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 16:
				{
				if (iRasterDesc.fPixvalDesc.fBigEndian == false
					&& maskRed == 0x7C00 && maskGreen == 0x03E0 && maskBlue == 0x001F)
					{
					oBITMAPINFO = spAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 16,
						iRasterDesc.fRowCount, iRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			case 24:
				{
				if ((iRasterDesc.fPixvalDesc.fBigEndian == true
					&& maskRed == 0x0000FF && maskGreen == 0x00FF00 && maskBlue == 0xFF0000)
					|| (iRasterDesc.fPixvalDesc.fBigEndian == false
					&& maskRed == 0xFF0000 && maskGreen == 0x00FF00 && maskBlue == 0x0000FF))
					{
					oBITMAPINFO = spAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 24,
						iRasterDesc.fRowCount, iRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			case 32:
				{
				if ((iRasterDesc.fPixvalDesc.fBigEndian == true
					&& maskRed == 0x0000FF00 && maskGreen == 0x00FF0000 && maskBlue == 0xFF000000)
					|| (iRasterDesc.fPixvalDesc.fBigEndian == false
					&& maskRed == 0x00FF0000 && maskGreen == 0x0000FF00 && maskBlue == 0x000000FF))
					{
					oBITMAPINFO = spAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 32,
						iRasterDesc.fRowCount, iRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			}
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = theRep.DynamicCast<PixelDescRep_Gray>())
		{
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				uint32 colorTableSize = 1 << iRasterDesc.fPixvalDesc.fDepth;
				oBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char
					[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);

				oBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				oBITMAPINFO->bmiHeader.biWidth
					= (iRasterDesc.fRowBytes * 8) / iRasterDesc.fPixvalDesc.fDepth;

				if (iRasterDesc.fFlipped)
					oBITMAPINFO->bmiHeader.biHeight = iRasterDesc.fRowCount;
				else
					oBITMAPINFO->bmiHeader.biHeight = -int(iRasterDesc.fRowCount);
				oBITMAPINFO->bmiHeader.biSizeImage = iRasterDesc.fRowBytes * iRasterDesc.fRowCount;
				oBITMAPINFO->bmiHeader.biPlanes = 1;
				oBITMAPINFO->bmiHeader.biBitCount = iRasterDesc.fPixvalDesc.fDepth;
				oBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				oBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				oBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				oBITMAPINFO->bmiHeader.biClrUsed = colorTableSize;
				oBITMAPINFO->bmiHeader.biClrImportant = 0;

				uint32 multiplier = 0xFFFFFFFFU / (colorTableSize - 1);
				for (size_t x = 0; x < colorTableSize; ++x)
					{
					uint32 temp = (x * multiplier) >> 16;
					oBITMAPINFO->bmiColors[x].rgbRed = temp;
					oBITMAPINFO->bmiColors[x].rgbGreen = temp;
					oBITMAPINFO->bmiColors[x].rgbBlue = temp;
					oBITMAPINFO->bmiColors[x].rgbReserved = 0;
					}
				return true;
				}
			}
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed
		= theRep.DynamicCast<PixelDescRep_Indexed>())
		{
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				const ZRGBA_POD* sourceColors;
				size_t sourceColorsCount;
				thePixelDescRep_Indexed->GetColors(sourceColors, sourceColorsCount);

				size_t colorTableSize = 1 << iRasterDesc.fPixvalDesc.fDepth;
				sourceColorsCount = std::min(sourceColorsCount, colorTableSize);

				oBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char
					[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);

				oBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				oBITMAPINFO->bmiHeader.biWidth =
					(iRasterDesc.fRowBytes * 8) / iRasterDesc.fPixvalDesc.fDepth;

				if (iRasterDesc.fFlipped)
					oBITMAPINFO->bmiHeader.biHeight = iRasterDesc.fRowCount;
				else
					oBITMAPINFO->bmiHeader.biHeight = -int(iRasterDesc.fRowCount);
				oBITMAPINFO->bmiHeader.biSizeImage = iRasterDesc.fRowBytes * iRasterDesc.fRowCount;
				oBITMAPINFO->bmiHeader.biPlanes = 1;
				oBITMAPINFO->bmiHeader.biBitCount = iRasterDesc.fPixvalDesc.fDepth;
				oBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				oBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				oBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				oBITMAPINFO->bmiHeader.biClrUsed = sourceColorsCount;
				oBITMAPINFO->bmiHeader.biClrImportant = 0;
				for (size_t x = 0; x < sourceColorsCount; ++x)
					{
					oBITMAPINFO->bmiColors[x].rgbRed = sourceColors[x].red;
					oBITMAPINFO->bmiColors[x].rgbGreen = sourceColors[x].green;
					oBITMAPINFO->bmiColors[x].rgbBlue = sourceColors[x].blue;
					oBITMAPINFO->bmiColors[x].rgbReserved = 0;
					}
				return true;
				}
			}
		}
	return false;
	}

ZDCPixmapRep_DIB::ZDCPixmapRep_DIB(ZRef<ZDCPixmapRaster> iRaster,
	const ZRectPOD& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
:	ZDCPixmapRep(iRaster, iBounds, iPixelDesc)
	{
	bool result = spSetupDIB(iRaster->GetRasterDesc(), iPixelDesc, fBITMAPINFO);
	ZAssertStop(1, result);
	fChangeCount = fPixelDesc.GetChangeCount() - 1;
	}

ZDCPixmapRep_DIB::ZDCPixmapRep_DIB(HDC iHDC, HBITMAP iHBITMAP, bool iForce32BPP)
	{
	using namespace ZDCPixmapNS;

	ZAssertStop(1, iHBITMAP);

	// Get information about inHBITMAP
	BITMAP theBITMAP;
	::GetObjectW(iHBITMAP, sizeof(theBITMAP), &theBITMAP);

	const int32 absoluteHeight = abs(theBITMAP.bmHeight);

	fBounds = sRectPOD(theBITMAP.bmWidth, absoluteHeight);

	if (iForce32BPP)
		{
		RasterDesc theRasterDesc;
		theRasterDesc.fPixvalDesc.fDepth = 32;
		theRasterDesc.fPixvalDesc.fBigEndian = false;
		theRasterDesc.fRowBytes =  (((32 * theBITMAP.bmWidth) + 31) / 32) * 4;
		theRasterDesc.fRowCount = absoluteHeight;
		theRasterDesc.fFlipped = true;
		fRaster = new ZDCPixmapRaster_Simple(theRasterDesc);

		fBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER)]);

		fBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		fBITMAPINFO->bmiHeader.biWidth = theBITMAP.bmWidth;
		fBITMAPINFO->bmiHeader.biHeight = absoluteHeight;
		fBITMAPINFO->bmiHeader.biSizeImage = theRasterDesc.fRowBytes * absoluteHeight;
		fBITMAPINFO->bmiHeader.biPlanes = 1;
		fBITMAPINFO->bmiHeader.biBitCount = 32;
		fBITMAPINFO->bmiHeader.biCompression = BI_RGB;
		fBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
		fBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
		fBITMAPINFO->bmiHeader.biClrUsed = 0;
		fBITMAPINFO->bmiHeader.biClrImportant = 0;

		::GetDIBits(iHDC, iHBITMAP,
			0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_PAL_COLORS);

		fPixelDesc = PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0);
		fChangeCount = fPixelDesc.GetChangeCount();
		}
	else
		{
		switch (theBITMAP.bmBitsPixel)
			{
			case 1:
			case 4:
			case 8:
				{
				RasterDesc theRasterDesc;
				theRasterDesc.fPixvalDesc.fDepth = theBITMAP.bmBitsPixel;
				theRasterDesc.fPixvalDesc.fBigEndian = true;
				theRasterDesc.fRowBytes
					= (((theBITMAP.bmBitsPixel * theBITMAP.bmWidth) + 31) / 32) * 4;
				theRasterDesc.fRowCount = absoluteHeight;
				theRasterDesc.fFlipped = true;
				fRaster = new ZDCPixmapRaster_Simple(theRasterDesc);

				uint32 colorTableSize = 1 << theRasterDesc.fPixvalDesc.fDepth;

				fBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char
					[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);
				fBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				fBITMAPINFO->bmiHeader.biWidth = theBITMAP.bmWidth;
				fBITMAPINFO->bmiHeader.biHeight = absoluteHeight;
				fBITMAPINFO->bmiHeader.biSizeImage = theRasterDesc.fRowBytes * absoluteHeight;
				fBITMAPINFO->bmiHeader.biPlanes = 1;
				fBITMAPINFO->bmiHeader.biBitCount = theBITMAP.bmBitsPixel;
				fBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				fBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				fBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				fBITMAPINFO->bmiHeader.biClrUsed = colorTableSize;
				fBITMAPINFO->bmiHeader.biClrImportant = 0;

				::GetDIBits(iHDC, iHBITMAP,
					0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_RGB_COLORS);

				if (theBITMAP.bmBitsPixel == 1
					&& fBITMAPINFO->bmiColors[0].rgbRed == 0
					&& fBITMAPINFO->bmiColors[0].rgbGreen == 0
					&& fBITMAPINFO->bmiColors[0].rgbBlue == 0
					&& fBITMAPINFO->bmiColors[1].rgbRed == 0xFFU
					&& fBITMAPINFO->bmiColors[1].rgbGreen == 0xFFU
					&& fBITMAPINFO->bmiColors[1].rgbBlue == 0xFFU
					)
					{
					fPixelDesc = PixelDesc(0x01, 0); // 1 bit grayscale
					}
				else
					{
					vector<ZRGBA_POD> colorTable(colorTableSize);
					for (size_t x = 0; x < colorTable.size(); ++x)
						{
						colorTable[x].red = 0x101 * fBITMAPINFO->bmiColors[x].rgbRed;
						colorTable[x].green = 0x101 * fBITMAPINFO->bmiColors[x].rgbGreen;
						colorTable[x].blue = 0x101 * fBITMAPINFO->bmiColors[x].rgbBlue;
						colorTable[x].alpha = 0xFFFFU;
						}
					fPixelDesc = PixelDesc(&colorTable[0], colorTable.size());
					}
				fChangeCount = fPixelDesc.GetChangeCount();
				break;
				}
			case 16:
			case 24:
			case 32:
				{
				RasterDesc theRasterDesc;
				theRasterDesc.fPixvalDesc.fDepth = theBITMAP.bmBitsPixel;
				theRasterDesc.fPixvalDesc.fBigEndian = false;
				theRasterDesc.fRowBytes =
					(((theBITMAP.bmBitsPixel * theBITMAP.bmWidth) + 31) / 32) * 4;
				theRasterDesc.fRowCount = absoluteHeight;
				theRasterDesc.fFlipped = true;
				fRaster = new ZDCPixmapRaster_Simple(theRasterDesc);

				fBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER)]);

				fBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				fBITMAPINFO->bmiHeader.biWidth = theBITMAP.bmWidth;
				fBITMAPINFO->bmiHeader.biHeight = absoluteHeight;
				fBITMAPINFO->bmiHeader.biSizeImage = theRasterDesc.fRowBytes * absoluteHeight;
				fBITMAPINFO->bmiHeader.biPlanes = 1;
				fBITMAPINFO->bmiHeader.biBitCount = theBITMAP.bmBitsPixel;
				fBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				fBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				fBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				fBITMAPINFO->bmiHeader.biClrUsed = 0;
				fBITMAPINFO->bmiHeader.biClrImportant = 0;

				::GetDIBits(iHDC, iHBITMAP,
					0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_PAL_COLORS);

				if (theBITMAP.bmBitsPixel == 16)
					fPixelDesc = PixelDesc(0x7C00, 0x03E0, 0x001F, 0);
				else
					fPixelDesc = PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0);
				fChangeCount = fPixelDesc.GetChangeCount();
				break;
				}
			}
		}
	}

ZDCPixmapRep_DIB::~ZDCPixmapRep_DIB()
	{
	delete[] reinterpret_cast<char*>(fBITMAPINFO);
	fBITMAPINFO = nullptr;
	}

void ZDCPixmapRep_DIB::GetDIBStuff(BITMAPINFO*& oBITMAPINFO, char*& oBits, ZRectPOD* oBounds)
	{
	using namespace ZDCPixmapNS;

	if (fChangeCount != fPixelDesc.GetChangeCount())
		{
		fChangeCount = fPixelDesc.GetChangeCount();
		if (PixelDescRep_Indexed* thePixelDesc
			= fPixelDesc.GetRep().DynamicCast<PixelDescRep_Indexed>())
			{
			const ZRGBA_POD* theColors;
			size_t theSize;
			thePixelDesc->GetColors(theColors, theSize);
			for (size_t x = 0; x < theSize; ++x)
				{
				fBITMAPINFO->bmiColors[x].rgbRed = theColors[x].red >> 8;
				fBITMAPINFO->bmiColors[x].rgbGreen = theColors[x].green >> 8;
				fBITMAPINFO->bmiColors[x].rgbBlue = theColors[x].blue >> 8;
				fBITMAPINFO->bmiColors[x].rgbReserved = 0;
				}
			}
		}

	oBITMAPINFO = fBITMAPINFO;
	oBits = reinterpret_cast<char*>(fRaster->GetBaseAddress());
	if (oBounds)
		*oBounds = fBounds;
	}

ZRef<ZDCPixmapRep_DIB> sPixmapRep_DIB(ZRef<ZDCPixmapRep> iRep)
	{
	using namespace ZDCPixmapNS;

	if (not iRep)
		return null;

	if (ZRef<ZDCPixmapRep_DIB> theRep = iRep.DynamicCast<ZDCPixmapRep_DIB>())
		return theRep;

	const ZRef<ZDCPixmapRaster> theRaster = iRep->GetRaster();
	const ZRectPOD theBounds = iRep->GetBounds();
	const PixelDesc thePixelDesc = iRep->GetPixelDesc();
	if (ZRef<ZDCPixmapRep_DIB> theRep = spCreateRepForDesc(theRaster, theBounds, thePixelDesc))
		return theRep;

	EFormatStandard fallbackFormat = eFormatStandard_BGRx_32;
	if (thePixelDesc.GetRep().DynamicCast<PixelDescRep_Gray>())
		fallbackFormat = eFormatStandard_Gray_8;

	RasterDesc newRasterDesc(WH(theBounds), fallbackFormat);
	PixelDesc newPixelDesc(fallbackFormat);
	ZRef<ZDCPixmapRep_DIB> theRep = new ZDCPixmapRep_DIB(
		new ZDCPixmapRaster_Simple(newRasterDesc), sRect(WH(theBounds)), newPixelDesc);

	theRep->CopyFrom(sPointPOD(0, 0), iRep, theBounds);
	return theRep;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(GDI)
