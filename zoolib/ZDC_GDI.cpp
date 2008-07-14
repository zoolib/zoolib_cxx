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

#include "zoolib/ZDC_GDI.h"

#if ZCONFIG_API_Enabled(DC_GDI)

#include "zoolib/ZCompat_algorithm.h" // For min
#include "zoolib/ZMemory.h"
#include "zoolib/ZUtil_Win.h"

#include <string.h> // For memcpy

#include <stdexcept> // For runtime_error
#include <vector>

using std::min;
using std::runtime_error;
using std::string;
using std::vector;

static short sModeLookup[] = { R2_COPYPEN, R2_MERGEPEN, R2_NOTXORPEN, R2_MASKNOTPEN };

// Forward declarations of utility methods
static BITMAPINFO* sCreateBITMAPINFO(const ZDCPixmapNS::RasterDesc& inRasterDesc, size_t inColorTableSize);

// =================================================================================================
#pragma mark -
#pragma mark * kDebug

#define kDebug_GDI 1

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static inline COLORREF sAsCOLORREF_Palette(HDC inHDC, const ZRGBColor& inColor)
	{ return PALETTERGB((inColor.red >> 8), (inColor.green >> 8), (inColor.blue >> 8)); }

//static COLORREF sAsCOLORREF_Palette(HDC inHDC, const ZRGBColor& inColor)
//	{
//	COLORREF theColorRef = GetNearestColor(inHDC, RGB((inColor.red >> 8), (inColor.green >> 8), (inColor.blue >> 8)));
//	return theColorRef | 0x02000000;
//	return PALETTERGB((inColor.red >> 8), (inColor.green >> 8), (inColor.blue >> 8));
//	}

static WORD sAllBitsSet[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static HBITMAP sHBITMAP_AllBitsSet = ::CreateBitmap(8, 8, 1, 1, &sAllBitsSet[0]);

static HBRUSH sCreateHBRUSH(HDC inHDC, const ZRef<ZDCInk::Rep>& inRep, ZPoint inRealPatternOrigin)
	{
	HBRUSH theHBRUSH = nil;
	if (inRep->fType == ZDCInk::eTypeSolidColor)
		{
		theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);
		}
	else if (inRep->fType == ZDCInk::eTypeTwoColor)
		{
		uint8 tempPattern[8];
		ZDCPattern::sOffset(inRealPatternOrigin.h, inRealPatternOrigin.v, inRep->fAsTwoColor.fPattern.pat, tempPattern);
		WORD localBits[8];
		for (size_t x = 0; x < 8; ++x)
			localBits[x] = tempPattern[x];
		HBITMAP theHBITMAP = ::CreateBitmap(8, 8, 1, 1, &localBits[0]);
		theHBRUSH = ::CreatePatternBrush(theHBITMAP);
		::DeleteObject(theHBITMAP);
		}
	else if (inRep->fType == ZDCInk::eTypeMultiColor)
		{
		// We should not be called if inRep is MultiColor, but if we do, create something garish and obvious.
		theHBRUSH = ::CreateSolidBrush(PALETTERGB(0xFF, 0xFF, 0));
		}
	else
		ZDebugStopf(1, ("sCreateHBrush, unknown ink type"));
	return theHBRUSH;
	}

static HFONT sCreateHFONT(const ZDCFont& inFont)
	{
	HFONT theHFONT;
	if (ZUtil_Win::sUseWAPI())
		{
		LOGFONTW theLOGFONT;
		ZBlockZero(&theLOGFONT, sizeof(theLOGFONT));

		theLOGFONT.lfHeight = -inFont.GetSize();

		ZDCFont::Style theStyle = inFont.GetStyle();
		if (theStyle & ZDCFont::bold)
			theLOGFONT.lfWeight = FW_SEMIBOLD;
		else
			theLOGFONT.lfWeight = FW_NORMAL;
	
		if (theStyle & ZDCFont::italic)
			theLOGFONT.lfItalic = 0xFF;

		if (theStyle & ZDCFont::underline)
			theLOGFONT.lfUnderline = 0xFF;

		theLOGFONT.lfQuality = ANTIALIASED_QUALITY;

		string16 fontName = ZUnicode::sAsUTF16(inFont.GetName());
		ZBlockCopy(fontName.c_str(), theLOGFONT.lfFaceName, sizeof(UTF16) * (fontName.size() + 1));
		theHFONT = ::CreateFontIndirectW(&theLOGFONT);
		}
	else
		{
		LOGFONTA theLOGFONT;
		ZBlockZero(&theLOGFONT, sizeof(theLOGFONT));

		theLOGFONT.lfHeight = -inFont.GetSize();

		ZDCFont::Style theStyle = inFont.GetStyle();
		if (theStyle & ZDCFont::bold)
			theLOGFONT.lfWeight = FW_SEMIBOLD;
		else
			theLOGFONT.lfWeight = FW_NORMAL;

		if (theStyle & ZDCFont::italic)
			theLOGFONT.lfItalic = 0xFF;

		if (theStyle & ZDCFont::underline)
			theLOGFONT.lfUnderline = 0xFF;
		
		string fontName = inFont.GetName();
		ZBlockCopy(fontName.c_str(), theLOGFONT.lfFaceName, (fontName.size() + 1));
		theHFONT = ::CreateFontIndirectA(&theLOGFONT);
		}
	return theHFONT;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupDC

ZDCCanvas_GDI::SetupDC::SetupDC(ZDCCanvas_GDI* inCanvas, const ZDCState& inState)
:	fCanvas(inCanvas)
	{
	fState = &inState;
	ZAssertStop(kDebug_GDI, fCanvas->fMutexToCheck == nil || fCanvas->fMutexToCheck->IsLocked());
	fCanvas->fMutexToLock->Acquire();
	if (!fCanvas->fOriginValid)
		{
		fCanvas->fOriginValid = true;
		if (fCanvas->IsPrinting())
			{
			::SetMapMode(inCanvas->fHDC, MM_ANISOTROPIC);
			int deviceHRes = ::GetDeviceCaps(inCanvas->fHDC, LOGPIXELSX);
			int deviceVRes = ::GetDeviceCaps(inCanvas->fHDC, LOGPIXELSY);
			::SetViewportExtEx(inCanvas->fHDC, deviceHRes, deviceVRes, nil);
			::SetWindowExtEx(inCanvas->fHDC, 72, 72, nil);
			}
		else
			{
			::SetMapMode(inCanvas->fHDC, MM_TEXT);
			}

		::SetWindowOrgEx(inCanvas->fHDC, 0, 0, nil);
		}

	if (inState.fChangeCount_Clip != fCanvas->fChangeCount_Clip)
		{
		ZDCRgn theClipRgn = fCanvas->Internal_CalcClipRgn(inState);
		if (fCanvas->IsPrinting())
			{
			int deviceHRes = ::GetDeviceCaps(fCanvas->fHDC, LOGPIXELSX);
			int deviceVRes = ::GetDeviceCaps(fCanvas->fHDC, LOGPIXELSY);
			float hScale = float(deviceHRes) / 72.0f;
			float vScale = float(deviceVRes) / 72.0f;
			theClipRgn.MakeScale(hScale, 0.0f, 0.0f, vScale, 0.0f, 0.0f);
			}
		::SelectClipRgn(fCanvas->fHDC, theClipRgn.GetHRGN());
		const_cast<ZDCState&>(inState).fChangeCount_Clip = ++fCanvas->fChangeCount_Clip;
		}
	}

ZDCCanvas_GDI::SetupDC::~SetupDC()
	{
	ZAssertStop(0, fState->fChangeCount_Clip == fCanvas->fChangeCount_Clip);
	fCanvas->fMutexToLock->Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupHPEN

class ZDCCanvas_GDI::SetupHPEN
	{
public:
	SetupHPEN(ZDCCanvas_GDI* inCanvas, ZDCState& ioState);
	SetupHPEN(HDC inHDC, const ZRGBColor& inColor, ZCoord inPenWidth);
	~SetupHPEN();
protected:
	HDC fHDC;

	HBRUSH fHBRUSH_Saved;
	HPEN fHPEN_Saved;
	int fROP2_Saved;
	};

ZDCCanvas_GDI::SetupHPEN::SetupHPEN(ZDCCanvas_GDI* inCanvas, ZDCState& ioState)
:	fHDC(inCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep && theRep->fType == ZDCInk::eTypeSolidColor);
	HPEN theHPEN = ::CreatePen(PS_INSIDEFRAME, ioState.fPenWidth, sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));
	fHPEN_Saved = (HPEN)::SelectObject(fHDC, theHPEN);
	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, ::GetStockObject(NULL_BRUSH));
	fROP2_Saved = ::SetROP2(fHDC, sModeLookup[ioState.fMode]);
	}

ZDCCanvas_GDI::SetupHPEN::SetupHPEN(HDC inHDC, const ZRGBColor& inColor, ZCoord inPenWidth)
:	fHDC(inHDC)
	{
	HPEN theHPEN = ::CreatePen(PS_INSIDEFRAME, inPenWidth, sAsCOLORREF_Palette(fHDC, inColor));
	fHPEN_Saved = (HPEN)::SelectObject(fHDC, theHPEN);
	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, ::GetStockObject(NULL_BRUSH));
	fROP2_Saved = ::SetROP2(fHDC, R2_COPYPEN);
	}

ZDCCanvas_GDI::SetupHPEN::~SetupHPEN()
	{
	::DeleteObject(::SelectObject(fHDC, fHPEN_Saved));
	::SelectObject(fHDC, fHBRUSH_Saved);
	::SetROP2(fHDC, fROP2_Saved);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupHBRUSH

class ZDCCanvas_GDI::SetupHBRUSH
	{
public:
	SetupHBRUSH(ZDCCanvas_GDI* inCanvas, ZDCState& ioState);
	SetupHBRUSH(HDC inHDC, const ZRGBColor& inColor);
	~SetupHBRUSH();

protected:
	HDC fHDC;

	HBRUSH fHBRUSH_Saved;
	HPEN fHPEN_Saved;
	int fROP2_Saved;
	bool fSetTextColor;
	bool fSetBackColor;
	COLORREF fTextColor_Saved;
	COLORREF fBackColor_Saved;
	};

ZDCCanvas_GDI::SetupHBRUSH::SetupHBRUSH(ZDCCanvas_GDI* inCanvas, ZDCState& ioState)
:	fHDC(inCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep);
	HBRUSH theHBRUSH;
	if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);

		fSetTextColor = false;
		fSetBackColor = true;
		fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));
		}
	else
		{
		ZAssertStop(kDebug_GDI, theRep->fType == ZDCInk::eTypeTwoColor);
		fSetTextColor = true;
		fSetBackColor = true;
		// The seeming reversal of terms here is because GDI applies the back color to a set bit and the
		// fore/text color to a cleared bit.
		fTextColor_Saved = ::SetTextColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fBackColor));
		fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fForeColor));

		uint8 tempPattern[8];
		ZDCPattern::sOffset(ioState.fPatternOrigin.h, ioState.fPatternOrigin.v, theRep->fAsTwoColor.fPattern.pat, tempPattern);
		WORD localBits[8];
		for (size_t x = 0; x < 8; ++x)
			localBits[x] = tempPattern[x];

		HBITMAP theHBITMAP = ::CreateBitmap(8, 8, 1, 1, &localBits[0]);
		theHBRUSH = ::CreatePatternBrush(theHBITMAP);
		::DeleteObject(theHBITMAP);
		}

	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, theHBRUSH);
	fHPEN_Saved = (HPEN)::SelectObject(fHDC, ::GetStockObject(NULL_PEN));
	fROP2_Saved = ::SetROP2(fHDC, sModeLookup[ioState.fMode]);
	}

ZDCCanvas_GDI::SetupHBRUSH::SetupHBRUSH(HDC inHDC, const ZRGBColor& inColor)
:	fHDC(inHDC)
	{
	HBRUSH theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);

	fSetTextColor = false;
	fSetBackColor = true;
	fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, inColor));

	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, theHBRUSH);
	fHPEN_Saved = (HPEN)::SelectObject(fHDC, ::GetStockObject(NULL_PEN));
	fROP2_Saved = ::SetROP2(fHDC, R2_COPYPEN);
	}

ZDCCanvas_GDI::SetupHBRUSH::~SetupHBRUSH()
	{
	::DeleteObject(::SelectObject(fHDC, fHBRUSH_Saved));
	::SelectObject(fHDC, fHPEN_Saved);
	::SetROP2(fHDC, fROP2_Saved);
	if (fSetTextColor)
		::SetTextColor(fHDC, fTextColor_Saved);
	if (fSetBackColor)
		::SetBkColor(fHDC, fBackColor_Saved);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupModeColor

class ZDCCanvas_GDI::SetupModeColor
	{
public:
	SetupModeColor(ZDCCanvas_GDI* inCanvas, ZDCState& ioState);
	~SetupModeColor();

protected:
	HDC fHDC;
	int fROP2_Saved;
	bool fSetTextColor;
	bool fSetBackColor;
	COLORREF fTextColor_Saved;
	COLORREF fBackColor_Saved;
	};

ZDCCanvas_GDI::SetupModeColor::SetupModeColor(ZDCCanvas_GDI* inCanvas, ZDCState& ioState)
:	fHDC(inCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep);
	if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		fSetTextColor = false;
		fSetBackColor = true;
		fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));
		}
	else
		{
		ZAssertStop(kDebug_GDI, theRep->fType == ZDCInk::eTypeTwoColor);
		fSetTextColor = true;
		fSetBackColor = true;
		// GDI has a different nomenclature for fore/back colors, hence the seeming reversal here.
		fTextColor_Saved = ::SetTextColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fBackColor));
		fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fForeColor));
		}

	fROP2_Saved = ::SetROP2(fHDC, sModeLookup[ioState.fMode]);
	}

ZDCCanvas_GDI::SetupModeColor::~SetupModeColor()
	{
	::SetROP2(fHDC, fROP2_Saved);
	if (fSetTextColor)
		::SetTextColor(fHDC, fTextColor_Saved);
	if (fSetBackColor)
		::SetBkColor(fHDC, fBackColor_Saved);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI::SetupHFONT

class ZDCCanvas_GDI::SetupHFONT
	{
public:
	SetupHFONT(ZDCCanvas_GDI* inCanvas, ZDCState& ioState);
	~SetupHFONT();

protected:
	HDC fHDC;

	HFONT fHFONT_Saved;
	int fROP2_Saved;
	COLORREF fTextColor_Saved;
	};

ZDCCanvas_GDI::SetupHFONT::SetupHFONT(ZDCCanvas_GDI* inCanvas, ZDCState& ioState)
:	fHDC(inCanvas->fHDC)
	{
	HFONT theHFONT = ::sCreateHFONT(ioState.fFont);
	ZAssertStop(kDebug_GDI, theHFONT);
	fHFONT_Saved = (HFONT)::SelectObject(fHDC, theHFONT);
	fROP2_Saved = ::SetROP2(fHDC, sModeLookup[ioState.fMode]);

	fTextColor_Saved = ::SetTextColor(fHDC, sAsCOLORREF_Palette(fHDC, ioState.fTextColor));

	::SetBkMode(fHDC, TRANSPARENT);
	::SetTextAlign(fHDC, TA_BASELINE | TA_LEFT);
	}

ZDCCanvas_GDI::SetupHFONT::~SetupHFONT()
	{
	::DeleteObject(::SelectObject(fHDC, fHFONT_Saved));
	::SetROP2(fHDC, fROP2_Saved);
	::SetTextColor(fHDC, fTextColor_Saved);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_Native

class ZDCCanvas_GDI_Native : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_Native(HDC inHDC);
	virtual ~ZDCCanvas_GDI_Native();
	};

ZDCCanvas_GDI_Native::ZDCCanvas_GDI_Native(HDC inHDC)
	{
	fHDC = inHDC;
	this->Internal_Link();
	}

ZDCCanvas_GDI_Native::~ZDCCanvas_GDI_Native()
	{
	fHDC = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI

ZMutex ZDCCanvas_GDI::sMutex_List;
ZDCCanvas_GDI* ZDCCanvas_GDI::sCanvas_Head;

ZRef<ZDCCanvas_GDI> ZDCCanvas_GDI::sFindCanvasOrCreateNative(HDC inHDC)
	{
	ZMutexLocker locker(sMutex_List);
	ZDCCanvas_GDI* theCanvas = sCanvas_Head;
	while (theCanvas)
		{
		if (theCanvas->fHDC == inHDC)
			return theCanvas;
		theCanvas = theCanvas->fCanvas_Next;
		}
	theCanvas = new ZDCCanvas_GDI_Native(inHDC);
	return theCanvas;
	}

ZDCCanvas_GDI::ZDCCanvas_GDI()
	{
	fCanvas_Prev = nil;
	fCanvas_Next = nil;

	fOriginValid = false;
	fChangeCount_Clip = 1;

	fHDC = nil;

	fMutexToLock = nil;
	fMutexToCheck = nil;
	}


ZDCCanvas_GDI::~ZDCCanvas_GDI()
	{
	// Subclasses must unlink themselves and detach/destroy grafports before this destructor is called.
	ZAssertStop(kDebug_GDI, fCanvas_Prev == nil);
	ZAssertStop(kDebug_GDI, fCanvas_Next == nil);
	ZAssertStop(kDebug_GDI, fHDC == nil);
	ZAssertStop(kDebug_GDI, fMutexToLock == nil);
	ZAssertStop(kDebug_GDI, fMutexToCheck == nil);
	}

void ZDCCanvas_GDI::Internal_Link()
	{
	ZAssertStop(kDebug_GDI, sMutex_List.IsLocked());
	fCanvas_Prev = nil;
	if (sCanvas_Head)
		sCanvas_Head->fCanvas_Prev = this;
	fCanvas_Next = sCanvas_Head;
	sCanvas_Head = this;
	}

void ZDCCanvas_GDI::Internal_Unlink()
	{
	ZAssertStop(kDebug_GDI, sMutex_List.IsLocked());
	if (fCanvas_Prev)
		fCanvas_Prev->fCanvas_Next = fCanvas_Next;
	if (fCanvas_Next)
		fCanvas_Next->fCanvas_Prev = fCanvas_Prev;
	if (sCanvas_Head == this)
		sCanvas_Head = fCanvas_Next;
	fCanvas_Prev = nil;
	fCanvas_Next = nil;
	}

// =================================================================================================

void ZDCCanvas_GDI::Pixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV, const ZRGBColor& inColor)
	{
	if (!fHDC)
		return;

	SetupDC theSetupDC(this, ioState);
	::SetPixelV(fHDC, inLocationH + ioState.fOrigin.h, inLocationV + ioState.fOrigin.v, sAsCOLORREF_Palette(fHDC, inColor));
	}

static void sBuildPolygonForLine(ZCoord inStartH, ZCoord inStartV, ZCoord inEndH, ZCoord inEndV, ZCoord inPenWidth, POINT* outPOINTS)
	{
	// For notes on this see AG's log book, 97-03-05
	ZPoint leftMostPoint, rightMostPoint;
	if (inStartH < inEndH)
		{
		leftMostPoint.h = inStartH;
		leftMostPoint.v = inStartV;
		rightMostPoint.h = inEndH;
		rightMostPoint.v = inEndV;
		}
	else
		{
		leftMostPoint.h = inEndH;
		leftMostPoint.v = inEndV;
		rightMostPoint.h = inStartH;
		rightMostPoint.v = inStartV;
		}
	// Two cases, from top left down to bottom right, or from bottom left up to top right
	if (leftMostPoint.v < rightMostPoint.v)
		{
		outPOINTS[0].x = leftMostPoint.h;
		outPOINTS[0].y = leftMostPoint.v;

		outPOINTS[1].x = leftMostPoint.h + inPenWidth;
		outPOINTS[1].y = leftMostPoint.v;

		outPOINTS[2].x = rightMostPoint.h + inPenWidth;
		outPOINTS[2].y = rightMostPoint.v;

		outPOINTS[3].x = rightMostPoint.h + inPenWidth;
		outPOINTS[3].y = rightMostPoint.v + inPenWidth;

		outPOINTS[4].x = rightMostPoint.h;
		outPOINTS[4].y = rightMostPoint.v + inPenWidth;

		outPOINTS[5].x = leftMostPoint.h;
		outPOINTS[5].y = leftMostPoint.v + inPenWidth;
		}
	else
		{
		outPOINTS[0].x = leftMostPoint.h;
		outPOINTS[0].y = leftMostPoint.v;

		outPOINTS[1].x = rightMostPoint.h;
		outPOINTS[1].y = rightMostPoint.v;

		outPOINTS[2].x = rightMostPoint.h + inPenWidth;
		outPOINTS[2].y = rightMostPoint.v;

		outPOINTS[3].x = rightMostPoint.h + inPenWidth;
		outPOINTS[3].y = rightMostPoint.v + inPenWidth;

		outPOINTS[4].x = leftMostPoint.h + inPenWidth;
		outPOINTS[4].y = leftMostPoint.v + inPenWidth;

		outPOINTS[5].x = leftMostPoint.h;
		outPOINTS[5].y = leftMostPoint.v + inPenWidth;
		}
	}

void ZDCCanvas_GDI::Line(ZDCState& ioState, ZCoord inStartH, ZCoord inStartV, ZCoord inEndH, ZCoord inEndV)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;
	if (ioState.fPenWidth <= 0)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		if (inStartH == inEndH)
			{
			// Vertical line
			ZRect theRect;
			theRect.left = inStartH;
			theRect.right = inEndH + ioState.fPenWidth;
			if (inStartV < inEndV)
				{
				theRect.top = inStartV;
				theRect.bottom = inEndV + ioState.fPenWidth;
				}
			else
				{
				theRect.top = inEndV;
				theRect.bottom = inStartV + ioState.fPenWidth;
				}
			this->Internal_TileRegion(ioState, theRect, *theRep->fAsMultiColor.fPixmap);
			}
		else if (inStartV == inEndV)
			{
			// Horizontal line
			ZRect theRect;
			theRect.top = inStartV;
			theRect.bottom = inStartV + ioState.fPenWidth;
			if (inStartH < inEndH)
				{
				theRect.left = inStartH;
				theRect.right = inEndH + ioState.fPenWidth;
				}
			else
				{
				theRect.left = inEndH;
				theRect.right = inStartH + ioState.fPenWidth;
				}
			this->Internal_TileRegion(ioState, theRect, *theRep->fAsMultiColor.fPixmap);
			}
		else
			{
			// Some kind of diagonal
			ZDCRgn theRgn = ZDCRgn::sLine(inStartH, inStartV, inEndH, inEndV, ioState.fPenWidth);
			this->Internal_TileRegion(ioState, theRgn, *theRep->fAsMultiColor.fPixmap);
			}
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		if (theRep->fType == ZDCInk::eTypeSolidColor && ioState.fPenWidth == 1 && !this->IsPrinting())
			{
			SetupHPEN theSetupHPEN(this, ioState);
			ZPoint leftMostPoint, rightMostPoint;
			if (inStartH <= inEndH)
				{
				leftMostPoint.h = inStartH;
				leftMostPoint.v = inStartV;
				rightMostPoint.h = inEndH;
				rightMostPoint.v = inEndV;
				}
			else
				{
				leftMostPoint.h = inEndH;
				leftMostPoint.v = inEndV;
				rightMostPoint.h = inStartH;
				rightMostPoint.v = inStartV;
				}
			POINT dummyOldPoint;
			::MoveToEx(fHDC, leftMostPoint.h + ioState.fOrigin.h, leftMostPoint.v + ioState.fOrigin.v, &dummyOldPoint);
			ZCoord hDiff = rightMostPoint.h - leftMostPoint.h;
			if (leftMostPoint.v <= rightMostPoint.v)
				{
				// Down and to right
				ZCoord vDiff = rightMostPoint.v - leftMostPoint.v;
				if (vDiff < hDiff)
					{
					// Less than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1, rightMostPoint.v + ioState.fOrigin.v);
					}
				else if (vDiff > hDiff)
					{
					// Greater than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h, rightMostPoint.v + ioState.fOrigin.v + 1);
					}
				else
					{
					// Exactly 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1, rightMostPoint.v + ioState.fOrigin.v + 1);
					}
				}
			else
				{
				// Up and to right
				ZCoord vDiff = leftMostPoint.v - rightMostPoint.v;
				if (vDiff < hDiff)
					{
					// Less than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1, rightMostPoint.v + ioState.fOrigin.v);
					}
				else if (vDiff > hDiff)
					{
					// Greater than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h, rightMostPoint.v + ioState.fOrigin.v - 1);
					}
				else
					{
					// Exactly 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1, rightMostPoint.v + ioState.fOrigin.v - 1);
					}
				}

			}
		else
			{
			SetupHBRUSH theSetupHBRUSH(this, ioState);
			if (inStartH == inEndH)
				{
				// Vertical line
				ZCoord top, bottom;
				if (inStartV < inEndV)
					{
					top = inStartV;
					bottom = inEndV;
					}
				else
					{
					top = inEndV;
					bottom = inStartV;
					}
				::Rectangle(fHDC, inStartH + ioState.fOrigin.h, top + ioState.fOrigin.v, inStartH + ioState.fOrigin.h + ioState.fPenWidth + 1, bottom + ioState.fOrigin.v + ioState.fPenWidth + 1);
				}
			else if (inStartV == inEndV)
				{
				// Horizontal line
				ZCoord left, right;
				if (inStartH < inEndH)
					{
					left = inStartH;
					right = inEndH;
					}
				else
					{
					left = inEndH;
					right = inStartH;
					}
				::Rectangle(fHDC, left + ioState.fOrigin.h, inStartV + ioState.fOrigin.v, right + ioState.fOrigin.h + ioState.fPenWidth + 1, inStartV + ioState.fOrigin.v + ioState.fPenWidth + 1);
				}
			else
				{
				// Some kind of diagonal
				POINT thePOINTS[6];
				::sBuildPolygonForLine(inStartH + ioState.fOrigin.h, inStartV + ioState.fOrigin.v, inEndH + ioState.fOrigin.h, inEndV + ioState.fOrigin.v, ioState.fPenWidth, thePOINTS);
				::Polygon(fHDC, thePOINTS, 6);
				}
			}
		}
	}

// Rectangle
void ZDCCanvas_GDI::FrameRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (inRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		if (inRect.Width() <= ioState.fPenWidth * 2 || inRect.Height() <= ioState.fPenWidth * 2)
			this->Internal_TileRegion(ioState, inRect, *theRep->fAsMultiColor.fPixmap);
		else
			{
			// T
			this->Internal_TileRegion(ioState, ZRect(inRect.left, inRect.top, inRect.right, inRect.top + ioState.fPenWidth), *theRep->fAsMultiColor.fPixmap);
			// L
			this->Internal_TileRegion(ioState, ZRect(inRect.left, inRect.top + ioState.fPenWidth, inRect.left + ioState.fPenWidth, inRect.bottom - ioState.fPenWidth), *theRep->fAsMultiColor.fPixmap);
			// B
			this->Internal_TileRegion(ioState, ZRect(inRect.left, inRect.bottom - ioState.fPenWidth, inRect.right, inRect.bottom), *theRep->fAsMultiColor.fPixmap);
			// R
			this->Internal_TileRegion(ioState, ZRect(inRect.right - ioState.fPenWidth, inRect.top + ioState.fPenWidth, inRect.right, inRect.bottom - ioState.fPenWidth), *theRep->fAsMultiColor.fPixmap);
			}
		}
	else if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		SetupDC theSetupDC(this, ioState);
		SetupHPEN theSetupHPEN(this, ioState);
		::Rectangle(fHDC, inRect.left + ioState.fOrigin.h, inRect.top + ioState.fOrigin.v, inRect.right + ioState.fOrigin.h, inRect.bottom + ioState.fOrigin.v);
		}
	else
		{
		ZRect realRect = inRect + ioState.fOrigin;
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		if (inRect.Width() <= ioState.fPenWidth * 2 || inRect.Height() <= ioState.fPenWidth * 2)
			::Rectangle(fHDC, inRect.left, realRect.top, realRect.right + 1, realRect.bottom + 1);
		else
			{
			// Top
			::Rectangle(fHDC, realRect.left, realRect.top, realRect.right + 1, realRect.top + ioState.fPenWidth + 1);
			// Left
			::Rectangle(fHDC, realRect.left, realRect.top, realRect.left + ioState.fPenWidth + 1, realRect.bottom + 1);
			// Bottom
			::Rectangle(fHDC, realRect.left, realRect.bottom - ioState.fPenWidth, realRect.right + 1, realRect.bottom + 1);
			// Right
			::Rectangle(fHDC, realRect.right - ioState.fPenWidth, realRect.top, realRect.right + 1, realRect.bottom + 1);
			}
		}
	}

void ZDCCanvas_GDI::FillRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fHDC)
		return;

	if (inRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, inRect, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		ZRect realRect = inRect + ioState.fOrigin;
		::Rectangle(fHDC, realRect.left, realRect.top, realRect.right + 1, realRect.bottom + 1);
		}
	}

void ZDCCanvas_GDI::InvertRect(ZDCState& ioState, const ZRect& inRect)
	{
	if (!fHDC)
		return;

	if (inRect.IsEmpty())
		return;

	RECT realRECT = inRect + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRect(fHDC, &realRECT);
	}

// Region
void ZDCCanvas_GDI::FrameRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, (inRgn - inRgn.Inset(ioState.fPenWidth, ioState.fPenWidth)), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = ::sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FrameRgn(fHDC, (inRgn + ioState.fOrigin).GetHRGN(), theHBRUSH, ioState.fPenWidth, ioState.fPenWidth);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, inRgn, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, (inRgn + ioState.fOrigin).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertRegion(ZDCState& ioState, const ZDCRgn& inRgn)
	{
	if (!fHDC)
		return;

	ZDCRgn realRgn = inRgn + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, realRgn.GetHRGN());
	}

// Round cornered rect
void ZDCCanvas_GDI::FrameRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (inRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZDCRgn outerRgn = ZDCRgn::sRoundRect(inRect, inCornerSize);
		ZDCRgn innerRgn = ZDCRgn::sRoundRect(inRect.Inset(ioState.fPenWidth, ioState.fPenWidth), inCornerSize);
		this->Internal_TileRegion(ioState, (outerRgn - innerRgn), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = ::sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		ZRect realBounds = inRect + (ioState.fOrigin);
		ZDCRgn outerRgn = ZDCRgn::sRoundRect(realBounds, inCornerSize);
		ZDCRgn innerRgn = ZDCRgn::sRoundRect(realBounds.left + ioState.fPenWidth, realBounds.top + ioState.fPenWidth,
							realBounds.right - ioState.fPenWidth, realBounds.bottom - ioState.fPenWidth,
							inCornerSize.h - ioState.fPenWidth, inCornerSize.v - ioState.fPenWidth);
		::FillRgn(fHDC, (outerRgn - innerRgn).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fHDC)
		return;

	if (inRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, ZDCRgn::sRoundRect(inRect, inCornerSize), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = ::sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, ZDCRgn::sRoundRect(inRect + ioState.fOrigin, inCornerSize).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertRoundRect(ZDCState& ioState, const ZRect& inRect, const ZPoint& inCornerSize)
	{
	if (!fHDC)
		return;

	ZRect realRect = inRect + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, ZDCRgn::sRoundRect(inRect + ioState.fOrigin, inCornerSize).GetHRGN());
	}

// Ellipse
void ZDCCanvas_GDI::FrameEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (inBounds.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	// Windows is inconsistent in how it renders ellipse frames and interiors. So we have to do them
	// manually, otherwise interiors leak over frames and frames are drawn outside of the bounds we've been passed.
	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZDCRgn outerRgn = ZDCRgn::sEllipse(inBounds);
		ZDCRgn innerRgn = ZDCRgn::sEllipse(inBounds.Inset(ioState.fPenWidth, ioState.fPenWidth));
		this->Internal_TileRegion(ioState, outerRgn - innerRgn, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);

		ZRect realBounds = inBounds + ioState.fOrigin;
		ZDCRgn outerRgn = ZDCRgn::sEllipse(realBounds);
		ZDCRgn innerRgn = ZDCRgn::sEllipse(realBounds.Inset(ioState.fPenWidth, ioState.fPenWidth));

		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, (outerRgn - innerRgn).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fHDC)
		return;

	if (inBounds.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	// Windows is inconsistent in how it renders ellipse frames and interiors. So we have to do them
	// manually, otherwise interiors leak over frames and frames are drawn outside of the bounds we've been passed.
	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, ZDCRgn::sEllipse(inBounds), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, ZDCRgn::sEllipse(inBounds + ioState.fOrigin).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertEllipse(ZDCState& ioState, const ZRect& inBounds)
	{
	if (!fHDC)
		return;

	if (inBounds.IsEmpty())
		return;

	// Windows is inconsistent in how it renders ellipse frames and interiors. So we have to do them
	// manually, otherwise interiors leak over frames and frames are drawn outside of the bounds we've been passed.
	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, ZDCRgn::sEllipse(inBounds + ioState.fOrigin).GetHRGN());
	}

// Poly
void ZDCCanvas_GDI::FillPoly(ZDCState& ioState, const ZDCPoly& inPoly)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, ZDCRgn::sPoly(inPoly), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		size_t theCount;
		POINT* thePOINTs;
		inPoly.GetPOINTs(ioState.fOrigin, thePOINTs, theCount);
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		::Polygon(fHDC, thePOINTs, theCount);
		}
	}

void ZDCCanvas_GDI::InvertPoly(ZDCState& ioState, const ZDCPoly& inPoly)
	{
	if (!fHDC)
		return;

	size_t theCount;
	POINT* thePOINTs;
	inPoly.GetPOINTs(ioState.fOrigin, thePOINTs, theCount);

	SetupDC theSetupDC(this, ioState);
	// Do the inversion by using an ink mode
	int oldMode = ::GetROP2(fHDC);
	::SetROP2(fHDC, R2_NOT);
	::Polygon(fHDC, thePOINTs, theCount);
	::SetROP2(fHDC, oldMode);
	}

// Pixmap
void ZDCCanvas_GDI::FillPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inPixmap)
	{
	// Placeholder for code to tile the current ink using inPixmap as a mask
	ZUnimplemented();
	}

void ZDCCanvas_GDI::DrawPixmap(ZDCState& ioState, const ZPoint& inLocation, const ZDCPixmap& inSourcePixmap, const ZDCPixmap* inMaskPixmap)
	{
	if (fHDC == nil)
		return;

	if (!inSourcePixmap)
		return;

	SetupDC theSetupDC(this, ioState);

	ZPoint realLocation = inLocation + ioState.fOrigin;

	ZRef<ZDCPixmapRep_DIB> sourceRepDIB = ZDCPixmapRep_DIB::sAsPixmapRep_DIB(inSourcePixmap.GetRep());
	BITMAPINFO* sourceBMI;
	char* sourceBits;
	ZRect sourceDIBBounds;
	sourceRepDIB->GetDIBStuff(sourceBMI, sourceBits, &sourceDIBBounds);

	ZRect localSourceBounds = sourceDIBBounds.Size();

	if (inMaskPixmap && *inMaskPixmap && !this->IsPrinting())
		{
		// Take a copy of the source
		HDC sourceCopyHDC = ::CreateCompatibleDC(fHDC);
		HBITMAP sourceCopyHBITMAP = ::CreateCompatibleBitmap(fHDC, localSourceBounds.Width(), localSourceBounds.Height());
		HBITMAP sourceCopyOriginalHBITMAP = (HBITMAP)::SelectObject(sourceCopyHDC, sourceCopyHBITMAP);

		::StretchDIBits(sourceCopyHDC, 0, 0, localSourceBounds.Width(), localSourceBounds.Height(),
					localSourceBounds.left + sourceDIBBounds.left, localSourceBounds.top + sourceDIBBounds.top, localSourceBounds.Width(), localSourceBounds.Height(),
					sourceBits, sourceBMI, DIB_RGB_COLORS, SRCCOPY);


		// Draw the mask *over* the source copy -- every place the mask is black, so will the source be.
		ZRef<ZDCPixmapRep_DIB> maskRepDIB = ZDCPixmapRep_DIB::sAsPixmapRep_DIB(inMaskPixmap->GetRep());
		BITMAPINFO* maskBMI;
		char* maskBits;
		ZRect maskDIBBounds;
		maskRepDIB->GetDIBStuff(maskBMI, maskBits, &maskDIBBounds);

		::StretchDIBits(sourceCopyHDC, 0, 0, localSourceBounds.Width(), localSourceBounds.Height(),
					localSourceBounds.left + maskDIBBounds.left, localSourceBounds.top + maskDIBBounds.top, localSourceBounds.Width(), localSourceBounds.Height(),
					maskBits, maskBMI, DIB_RGB_COLORS, SRCAND);

		// And the inverse of the mask with the destination -- white pixels in the mask will be black in the destination. Black pixels in the mask
		// will leave the destination alone
		::StretchDIBits(fHDC, realLocation.h, realLocation.v, localSourceBounds.Width(), localSourceBounds.Height(),
					localSourceBounds.left + maskDIBBounds.left, localSourceBounds.top + maskDIBBounds.top, localSourceBounds.Width(), localSourceBounds.Height(),
					maskBits, maskBMI, DIB_RGB_COLORS, 0x220326); // ~S & D

		// Draw the copy of the source to the screen, using SRCPAINT. Anything black won't touch the screen, any other color will be ored in
		::BitBlt(fHDC, realLocation.h, realLocation.v, localSourceBounds.Width(), localSourceBounds.Height(), sourceCopyHDC, 0, 0, SRCPAINT);

		// All done, clean up
		::SelectObject(sourceCopyHDC, sourceCopyOriginalHBITMAP);
		::DeleteDC(sourceCopyHDC);
		::DeleteObject(sourceCopyHBITMAP);
		}
	else
		{
		::StretchDIBits(fHDC, realLocation.h, realLocation.v, localSourceBounds.Width(), localSourceBounds.Height(),
					localSourceBounds.left + sourceDIBBounds.left, localSourceBounds.top + sourceDIBBounds.top, localSourceBounds.Width(), localSourceBounds.Height(),
					sourceBits, sourceBMI, DIB_RGB_COLORS, SRCCOPY);
		}
	}

// Text
void ZDCCanvas_GDI::DrawText(ZDCState& ioState, const ZPoint& inLocation, const char* inText, size_t inTextLength)
	{
	if (!fHDC || inTextLength == 0)
		return;

	SetupDC theSetupDC(this, ioState);
	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString16 = ZUnicode::sAsUTF16(inText, inTextLength);
//		::TextOutW(fHDC, inLocation.h + ioState.fOrigin.h, inLocation.v + ioState.fOrigin.v, theString16.data(), theString16.size());
		::ExtTextOutW(fHDC, inLocation.h + ioState.fOrigin.h, inLocation.v + ioState.fOrigin.v, 0, nil, theString16.data(), theString16.size(), nil);
		}
	else
		{
		::TextOutA(fHDC, inLocation.h + ioState.fOrigin.h, inLocation.v + ioState.fOrigin.v, inText, inTextLength);
		}
	}

ZCoord ZDCCanvas_GDI::GetTextWidth(ZDCState& ioState, const char* inText, size_t inTextLength)
	{
	if (!fHDC || inTextLength == 0)
		return 0;

	ZMutexLocker locker(*fMutexToLock);

	SetupHFONT theSetupHFONT(this, ioState);
	SIZE theTextSize;
	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString16 = ZUnicode::sAsUTF16(inText, inTextLength);
		::GetTextExtentPoint32W(fHDC, theString16.data(), theString16.size(), &theTextSize);
		}
	else
		{
		::GetTextExtentPoint32A(fHDC, inText, inTextLength, &theTextSize);
		}

	return theTextSize.cx;
	}

void ZDCCanvas_GDI::GetFontInfo(ZDCState& ioState, ZCoord& outAscent, ZCoord& outDescent, ZCoord& outLeading)
	{
	if (!fHDC)
		{
		outAscent = 0;
		outDescent = 0;
		outLeading = 0;
		return;
		}

	ZMutexLocker locker(*fMutexToLock);

	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		TEXTMETRICW theTextMetrics;
		::GetTextMetricsW(fHDC, &theTextMetrics);
		outAscent = theTextMetrics.tmAscent;
		outDescent = theTextMetrics.tmDescent;
		outLeading = theTextMetrics.tmInternalLeading;
		}
	else
		{
		TEXTMETRICA theTextMetrics;
		::GetTextMetricsA(fHDC, &theTextMetrics);
		outAscent = theTextMetrics.tmAscent;
		outDescent = theTextMetrics.tmDescent;
		outLeading = theTextMetrics.tmInternalLeading;
		}
	}

ZCoord ZDCCanvas_GDI::GetLineHeight(ZDCState& ioState)
	{
	if (!fHDC)
		return 0;

	ZMutexLocker locker(*fMutexToLock);

	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		TEXTMETRICW theTextMetrics;
		::GetTextMetricsW(fHDC, &theTextMetrics);
		return theTextMetrics.tmAscent + theTextMetrics.tmDescent + theTextMetrics.tmInternalLeading;
		}
	else
		{
		TEXTMETRICA theTextMetrics;
		::GetTextMetricsA(fHDC, &theTextMetrics);
		return theTextMetrics.tmAscent + theTextMetrics.tmDescent + theTextMetrics.tmInternalLeading;
		}
	}

void ZDCCanvas_GDI::BreakLine(ZDCState& ioState, const char* inLineStart, size_t inRemainingTextLength, ZCoord inTargetWidth, size_t& outNextLineDelta)
	{
	if (!fHDC)
		{
		outNextLineDelta = inRemainingTextLength;
		return;
		}

	if (inRemainingTextLength == 0)
		{
		outNextLineDelta = 0;
		return;
		}

	if (inTargetWidth < 0)
		inTargetWidth = 0;

	SetupDC theSetupDC(this, ioState);
	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString = ZUnicode::sAsUTF16(inLineStart, inRemainingTextLength);
		SIZE dummySIZE;
		int brokenLength = 0;
		::GetTextExtentExPointW(fHDC, theString.data(), theString.size(), inTargetWidth, &brokenLength, nil, &dummySIZE);
		size_t codePointOffset = ZUnicode::sCUToCP(theString.data(), brokenLength);
		outNextLineDelta = ZUnicode::sCPToCU(inLineStart, inRemainingTextLength, codePointOffset, nil);
		}
	else
		{
		SIZE dummySIZE;
		int brokenLength = 0;
		::GetTextExtentExPointA(fHDC, inLineStart, inRemainingTextLength, inTargetWidth, &brokenLength, nil, &dummySIZE);
		outNextLineDelta = brokenLength;
		}

	// Check for a '\n' or '\r' before the natural breaking point of the line (walking from beginning to end)
	for (size_t x = 0; x < outNextLineDelta; ++x)
		{
		char theChar = inLineStart[x];
		if (theChar == '\n' || theChar == '\r')
			{
			// Found one. Use its position as the break length.
			outNextLineDelta = x + 1;
			return;
			}
		}
	if (outNextLineDelta < inRemainingTextLength)
		{
		for (size_t x = outNextLineDelta; x > 0; --x)
			{
			char theChar = inLineStart[x - 1];
			if (isspace(theChar))
				{
				// Found one. Use its position as the break length.
				outNextLineDelta = x;
				break;
				}
			}
		}

	if (outNextLineDelta == 0)
		outNextLineDelta = 1;
	}

void ZDCCanvas_GDI::Flush()
	{
	// On my machine GdiFlush does not eieio.
	::GdiFlush();
	}

void ZDCCanvas_GDI::Sync()
	{}

void ZDCCanvas_GDI::InvalCache()
	{
	if (!fHDC)
		return;

	ZMutexLocker locker(*fMutexToLock);

	fOriginValid = false;
	++fChangeCount_Clip;
	}

short ZDCCanvas_GDI::GetDepth()
	{
	if (!fHDC)
		return 0;

	ZMutexLocker locker(*fMutexToLock);

	return ::GetDeviceCaps(fHDC, BITSPIXEL);
	}

bool ZDCCanvas_GDI::IsOffScreen()
	{ return false; }

bool ZDCCanvas_GDI::IsPrinting()
	{ return false; }

ZRef<ZDCCanvas> ZDCCanvas_GDI::CreateOffScreen(const ZRect& canvasRect)
	{
	if (!fHDC)
		return ZRef<ZDCCanvas>();

	ZMutexLocker locker(*fMutexToLock);

	return new ZDCCanvas_GDI_OffScreen(fHDC, canvasRect);
	}

ZRef<ZDCCanvas> ZDCCanvas_GDI::CreateOffScreen(ZPoint dimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	if (!fHDC)
		return ZRef<ZDCCanvas>();

	ZMutexLocker locker(*fMutexToLock);

	return new ZDCCanvas_GDI_OffScreen(fHDC, dimensions, iFormat);
	}

ZPoint ZDCCanvas_GDI::Internal_GDIStart(ZDCState& ioState, bool inZeroOrigin)
	{
	ZMutexLocker locker(*fMutexToLock);

	fOriginValid = false;
	++fChangeCount_Clip;

	ZDCRgn theClipRgn = this->Internal_CalcClipRgn(ioState);
	if (this->IsPrinting())
		{
		int deviceHRes = ::GetDeviceCaps(fHDC, LOGPIXELSX);
		int deviceVRes = ::GetDeviceCaps(fHDC, LOGPIXELSY);
		float hScale = (float)deviceHRes / 72.0f;
		float vScale = (float)deviceVRes / 72.0f;
		theClipRgn.MakeScale(hScale, 0.0f, 0.0f, vScale, 0.0f, 0.0f);
		}
	::SelectClipRgn(fHDC, theClipRgn.GetHRGN());

	::SetMapMode(fHDC, MM_TEXT);
	ZPoint theOffset = ZPoint::sZero;
	if (!inZeroOrigin)
		theOffset = ioState.fOrigin;
	::SetWindowOrgEx(fHDC, -theOffset.h, -theOffset.v, nil);

	return theOffset;
	}

void ZDCCanvas_GDI::Internal_GDIEnd()
	{
	this->InvalCache();
	}

void ZDCCanvas_GDI::Internal_TileRegion(ZDCState& ioState, const ZDCRgn& inRgn, const ZDCPixmap& inPixmap)
	{
	ZMutexLocker locker(*fMutexToLock);

	if (!fOriginValid)
		{
		fOriginValid = true;
		::SetMapMode(fHDC, MM_TEXT);
		::SetWindowOrgEx(fHDC, 0, 0, nil);
		}

	ZDCRgn realRgn = (inRgn + ioState.fOrigin) & this->Internal_CalcClipRgn(ioState);
	if (this->IsPrinting())
		{
		int deviceHRes = ::GetDeviceCaps(fHDC, LOGPIXELSX);
		int deviceVRes = ::GetDeviceCaps(fHDC, LOGPIXELSY);
		float hScale = (float)deviceHRes / 72.0f;
		float vScale = (float)deviceVRes / 72.0f;
		realRgn.MakeScale(hScale, 0.0f, 0.0f, vScale, 0.0f, 0.0f);
		}
	::SelectClipRgn(fHDC, realRgn.GetHRGN());
	++fChangeCount_Clip;

	ZRef<ZDCPixmapRep_DIB> pixmapRepDIB = ZDCPixmapRep_DIB::sAsPixmapRep_DIB(inPixmap.GetRep());
	BITMAPINFO* sourceBMI;
	char* sourceBits;
	ZRect rasterBounds;
	pixmapRepDIB->GetDIBStuff(sourceBMI, sourceBits, &rasterBounds);
	ZPoint pixmapSize = rasterBounds.Size();
	ZRect drawnBounds = realRgn.Bounds();
	ZCoord drawnOriginH = drawnBounds.left - ((((drawnBounds.left + ioState.fPatternOrigin.h) % pixmapSize.h) + pixmapSize.h) % pixmapSize.h);
	ZCoord drawnOriginV = drawnBounds.top - ((((drawnBounds.top + ioState.fPatternOrigin.v) % pixmapSize.v) + pixmapSize.v) % pixmapSize.v);

	for (ZCoord y = drawnOriginV; y < drawnBounds.bottom; y += pixmapSize.v)
		{
		for (ZCoord x = drawnOriginH; x < drawnBounds.right; x += pixmapSize.h)
			{
			::StretchDIBits(fHDC, x, y, pixmapSize.h, pixmapSize.v,
						rasterBounds.left, rasterBounds.top, pixmapSize.h, pixmapSize.v,
						sourceBits, sourceBMI, DIB_RGB_COLORS, SRCCOPY);
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_NonWindow

ZDCCanvas_GDI_NonWindow::ZDCCanvas_GDI_NonWindow()
	{
	fMutexToLock = &fMutex;
	}

ZDCCanvas_GDI_NonWindow::~ZDCCanvas_GDI_NonWindow()
	{
	ZAssertStop(kDebug_GDI, fHDC == nil);
	fMutexToLock = nil;
	}

void ZDCCanvas_GDI_NonWindow::Finalize()
	{
	fMutexToLock->Acquire();

	ZMutexLocker lockerList(sMutex_List);
	if (this->GetRefCount() > 1)
		{
		this->FinalizationComplete();
		fMutexToLock->Release();
		}
	else
		{
		this->Internal_Unlink();
		this->FinalizationComplete();
		fMutexToLock->Release();
		delete this;
		}
	}

void ZDCCanvas_GDI_NonWindow::Scroll(ZDCState& ioState, const ZRect& inRect, ZCoord inDeltaH, ZCoord inDeltaV)
	{
	if (!fHDC)
		return;

	SetupDC theSetupDC(this, ioState);
	RECT realRECT = inRect + ioState.fOrigin;
	::ScrollDC(fHDC, inDeltaH, inDeltaV, &realRECT, &realRECT, nil, nil);
	}

void ZDCCanvas_GDI_NonWindow::CopyFrom(ZDCState& ioState, const ZPoint& inDestLocation, ZRef<ZDCCanvas> inSourceCanvas, const ZDCState& inSourceState, const ZRect& inSourceRect)
	{
	ZRef<ZDCCanvas_GDI> sourceCanvasGDI = ZRefDynamicCast<ZDCCanvas_GDI>(inSourceCanvas);

	ZAssertStop(kDebug_GDI, sourceCanvasGDI);

	if (fHDC == nil || sourceCanvasGDI->Internal_GetHDC() == nil)
		return;

	SetupDC theSetupDC(this, ioState);
	SetupDC theSetupDCSource(sourceCanvasGDI.GetObject(), inSourceState);

	ZPoint realSource = inSourceRect.TopLeft() + inSourceState.fOrigin;
	ZRect realDest = ZRect(inSourceRect.Size()) + (inDestLocation + ioState.fOrigin);
	::BitBlt(fHDC, realDest.left, realDest.top, realDest.Width(), realDest.Height(),
						sourceCanvasGDI->Internal_GetHDC(), realSource.h, realSource.v, SRCCOPY);
	}

ZDCRgn ZDCCanvas_GDI_NonWindow::Internal_CalcClipRgn(const ZDCState& inState)
	{
	return inState.fClip + inState.fClipOrigin;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_OffScreen

static HPALETTE sHPALETTE_Offscreen = ::CreateHalftonePalette(nil);

ZDCCanvas_GDI_OffScreen::ZDCCanvas_GDI_OffScreen(HDC inOther, const ZRect& inGlobalRect)
	{
	if (inGlobalRect.IsEmpty())
		throw runtime_error("ZDCCanvas_GDI_OffScreen, inGlobalRect.IsEmpty()");

	fOriginValid = false;

	fHBITMAP = ::CreateCompatibleBitmap(inOther, inGlobalRect.Width(), inGlobalRect.Height());
	fHDC = ::CreateCompatibleDC(inOther);
	HBITMAP theOriginalHBITMAP = (HBITMAP)::SelectObject(fHDC, fHBITMAP);
	::DeleteObject(theOriginalHBITMAP);

	::SelectPalette(fHDC, sHPALETTE_Offscreen, true);
	::RealizePalette(fHDC);

	ZAssertStop(kDebug_GDI, fHDC);
	ZAssertStop(kDebug_GDI, fHBITMAP);

	ZMutexLocker locker(sMutex_List);
	this->Internal_Link();
	}

static short sFormateEfficientToDepth(ZDCPixmapNS::EFormatEfficient iFormat)
	{
	using namespace ZDCPixmapNS;

	switch (iFormat)
		{
//		case eFormatEfficient_Indexed_1: return 1;
//		case eFormatEfficient_Indexed_8: return 8;

//		case eFormatEfficient_Gray_1: return 1;
//		case eFormatEfficient_Gray_8: return ;

		case eFormatEfficient_Color_16: return 16;
		case eFormatEfficient_Color_24: return 24;
		case eFormatEfficient_Color_32: return 32;
		}
	ZUnimplemented();
	return 0;
	}

ZDCCanvas_GDI_OffScreen::ZDCCanvas_GDI_OffScreen(HDC inOther, ZPoint inDimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	using namespace ZDCPixmapNS;

	if (inDimensions.h <= 0 || inDimensions.v <= 0)
		throw runtime_error("ZDCCanvas_GDI_OffScreen, zero sized offscreen");

	short depth = sFormateEfficientToDepth(iFormat);

	BITMAPINFO theBITMAPINFO;
	theBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	theBITMAPINFO.bmiHeader.biWidth = inDimensions.h;
	theBITMAPINFO.bmiHeader.biHeight = inDimensions.v;
	theBITMAPINFO.bmiHeader.biPlanes = 1;
	theBITMAPINFO.bmiHeader.biBitCount = depth;
	theBITMAPINFO.bmiHeader.biCompression = BI_RGB;
	short rowBytes = ((depth * inDimensions.h + 31) / 32) * 4;
	theBITMAPINFO.bmiHeader.biSizeImage = rowBytes * inDimensions.v;
	theBITMAPINFO.bmiHeader.biXPelsPerMeter = 0;
	theBITMAPINFO.bmiHeader.biYPelsPerMeter = 0;
	theBITMAPINFO.bmiHeader.biClrUsed = 0;
	theBITMAPINFO.bmiHeader.biClrImportant = 0;

	void* thePixelStorage;
	fHBITMAP = ::CreateDIBSection(inOther, &theBITMAPINFO, DIB_RGB_COLORS, &thePixelStorage, nil, 0);
	fHDC = ::CreateCompatibleDC(inOther);
	HBITMAP theOriginalHBITMAP = (HBITMAP)::SelectObject(fHDC, fHBITMAP);
	::DeleteObject(theOriginalHBITMAP);
	ZAssertStop(kDebug_GDI, fHDC);
	ZAssertStop(kDebug_GDI, fHBITMAP);

	::SelectPalette(fHDC, sHPALETTE_Offscreen, true);
	::RealizePalette(fHDC);

	ZMutexLocker locker(sMutex_List);
	this->Internal_Link();
	}

ZDCCanvas_GDI_OffScreen::~ZDCCanvas_GDI_OffScreen()
	{
	if (fHDC)
		::DeleteDC(fHDC);
	fHDC = nil;
	if (fHBITMAP)
		::DeleteObject(fHBITMAP);
	}

ZRGBColor ZDCCanvas_GDI_OffScreen::GetPixel(ZDCState& ioState, ZCoord inLocationH, ZCoord inLocationV)
	{
	SetupDC theSetupDC(this, ioState);
	return ZRGBColor::sFromCOLORREF(::GetPixel(fHDC, inLocationH + ioState.fOrigin.h, inLocationV + ioState.fOrigin.v));
	}

void ZDCCanvas_GDI_OffScreen::FloodFill(ZDCState& ioState, const ZPoint& inSeedLocation)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZPoint realSeedLocation = inSeedLocation + ioState.fOrigin;
	ZDCRgn realClip = ioState.fClip + ioState.fClipOrigin;
	if (!realClip.Contains(inSeedLocation))
		return;

	SetupDC theSetupDC(this, ioState);
	COLORREF theSeedCOLORREF = ::GetPixel(fHDC, realSeedLocation.h, realSeedLocation.v);

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZRect clipBounds = realClip.Bounds();

		// To do a flood fill with a multi color ink (ie a pixmap) we have to help out some. First we take a
		// one bit copy of our HDC, where every pixel that matches the seed pixel's color is set to white,
		// and all other pixels are set to black.
		HDC theHDC1 = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(theHDC1, ::CreateBitmap(clipBounds.Width(), clipBounds.Height(), 1, 1, nil)));

		// Set our back color to the seed color, so the StretchBlt knows which color should be mapped to white
		COLORREF oldBackCOLORREF = ::SetBkColor(fHDC, theSeedCOLORREF);
		::StretchBlt(theHDC1, 0, 0, clipBounds.Width(), clipBounds.Height(), fHDC, clipBounds.left, clipBounds.top,
							clipBounds.Width(), clipBounds.Height(), SRCCOPY);

		// Create a second HDC with a copy of the 1 bit HDC
		HDC theHDC2 = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(theHDC2, ::CreateBitmap(clipBounds.Width(), clipBounds.Height(), 1, 1, nil)));
		::BitBlt(theHDC2, 0, 0, clipBounds.Width(), clipBounds.Height(),
							theHDC1, 0, 0, SRCCOPY);

		// Now we do a flood fill to black of theHDC1, the first monochrome copy of our HDC
		HBRUSH tempHBRUSH = (HBRUSH)::SelectObject(theHDC1, ::CreateSolidBrush(PALETTERGB(0, 0, 0)));
		::ExtFloodFill(theHDC1, realSeedLocation.h - clipBounds.left, realSeedLocation.v - clipBounds.top, PALETTERGB(0xFF, 0xFF, 0xFF), FLOODFILLSURFACE);
		::DeleteObject(::SelectObject(theHDC1, tempHBRUSH));

		// We now have theHDC2 containing white *everywhere* fHDC contains our seed color, and theHDC1 containing
		// white everywhere fHDC contains our seed color *except* for those pixels reachable from the seed location. The
		// exclusive or of the two HDCs thus identify exactly those pixels in fHDC that match the seed color, and are reachable
		// from the seed location.
		::BitBlt(theHDC1, 0, 0, clipBounds.Width(), clipBounds.Height(),
							theHDC2, 0, 0, SRCINVERT);
		// We can discard theHDC2
		::DeleteDC(theHDC2);

		// Use theHDC1 to set to black every pixel we're going to be filling
		::BitBlt(fHDC, clipBounds.left, clipBounds.top, clipBounds.Width(), clipBounds.Height(),
							theHDC1, 0, 0, 0x00220326); // (NOT src) AND dest -- DSna

		// Get our ink's pixmap into an HDC
		ZRef<ZDCPixmapRep_DIB> inkRepDIB = ZDCPixmapRep_DIB::sAsPixmapRep_DIB(theRep->fAsMultiColor.fPixmap->GetRep());
		BITMAPINFO* inkBMI;
		char* inkBits;
		ZRect theInkBounds;
		inkRepDIB->GetDIBStuff(inkBMI, inkBits, &theInkBounds);
		ZPoint theInkPixmapSize = theInkBounds.Size();
		HDC inkHDC = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(inkHDC, ::CreateCompatibleBitmap(fHDC, theInkPixmapSize.h, theInkPixmapSize.v)));

		::StretchDIBits(inkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v,
					theInkBounds.left, theInkBounds.top, theInkPixmapSize.h, theInkPixmapSize.v,
					inkBits, inkBMI, DIB_RGB_COLORS, SRCCOPY);

		// Create another HDC for the ink, one that will hold each tile before it is blitted in
		HDC intermediateInkHDC = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(intermediateInkHDC, ::CreateCompatibleBitmap(fHDC, theInkPixmapSize.h, theInkPixmapSize.v)));

		// Now tile our ink into fHDC by taking a copy of the ink for each tile, setting to black all pixels that aren't
		// in theHDC1 and ORing the result into fHDC.

		ZCoord drawnOriginH = clipBounds.left - ((((clipBounds.left + ioState.fPatternOrigin.h) % theInkPixmapSize.h) + theInkPixmapSize.h) % theInkPixmapSize.h);
		ZCoord drawnOriginV = clipBounds.top - ((((clipBounds.top + ioState.fPatternOrigin.v) % theInkPixmapSize.v) + theInkPixmapSize.v) % theInkPixmapSize.v);

		for (ZCoord y = drawnOriginV; y < clipBounds.bottom; y += theInkPixmapSize.v)
			{
			for (ZCoord x = drawnOriginH; x < clipBounds.right; x += theInkPixmapSize.h)
				{
				::BitBlt(intermediateInkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v, inkHDC, 0, 0, SRCCOPY);

				::BitBlt(intermediateInkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v, theHDC1, x - clipBounds.left, y - clipBounds.top, SRCAND);

				::BitBlt(fHDC, x, y, theInkPixmapSize.h, theInkPixmapSize.v, intermediateInkHDC, 0, 0, SRCPAINT);
				}
			}

		::DeleteDC(inkHDC);
		::DeleteDC(intermediateInkHDC);
		::DeleteDC(theHDC1);
		}
	else
		{
		SetupHBRUSH theSetupHBRUSH(this, ioState);

		::ExtFloodFill(fHDC, realSeedLocation.h, realSeedLocation.v, theSeedCOLORREF, FLOODFILLSURFACE);
		}
	}

ZDCPixmap ZDCCanvas_GDI_OffScreen::GetPixmap(ZDCState& ioState, const ZRect& inBounds)
	{
	if (fHDC == nil || inBounds.IsEmpty())
		return ZDCPixmap();

	SetupDC theSetupDC(this, ioState);

	ZRect realBounds = inBounds + ioState.fOrigin;

	HDC resultHDC = ::CreateCompatibleDC(fHDC);
	HBITMAP resultHBITMAP = ::CreateCompatibleBitmap(fHDC, realBounds.Width(), realBounds.Height());
	HBITMAP oldResultHBITMAP = (HBITMAP)::SelectObject(resultHDC, resultHBITMAP);
	::BitBlt(resultHDC, 0, 0, realBounds.Width(), realBounds.Height(), fHDC, realBounds.Left(), realBounds.Top(), SRCCOPY);
	::SelectObject(resultHDC, oldResultHBITMAP);

	ZRef<ZDCPixmapRep_DIB> theRep_DIB = new ZDCPixmapRep_DIB(resultHDC, resultHBITMAP);

	::DeleteDC(resultHDC);
	::DeleteObject(resultHBITMAP);

	return ZDCPixmap(theRep_DIB.GetObject());
	}

bool ZDCCanvas_GDI_OffScreen::IsOffScreen()
	{ return true; }

// =================================================================================================
#pragma mark -
#pragma mark * ZDCCanvas_GDI_IC

ZDCCanvas_GDI_IC::ZDCCanvas_GDI_IC()
	{
	if (ZUtil_Win::sUseWAPI())
		fHDC = ::CreateICW(L"DISPLAY", nil, nil, nil);
	else
		fHDC = ::CreateICA("DISPLAY", nil, nil, nil);

	ZMutexLocker locker(sMutex_List);
	this->Internal_Link();
	}

ZDCCanvas_GDI_IC::~ZDCCanvas_GDI_IC()
	{
	::DeleteDC(fHDC);
	fHDC = nil;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDC_NativeGDI

ZDC_NativeGDI::ZDC_NativeGDI(HDC inHDC)
	{
	fHDC = inHDC;
	fSavedDCIndex = ::SaveDC(fHDC);
	fCanvas = ZDCCanvas_GDI::sFindCanvasOrCreateNative(fHDC);

	ZDCRgn clipRgn;
	if (::GetClipRgn(fHDC, clipRgn.GetHRGN()) == 0)
		{
		// If no clipRgn is set then create one -- this is arbitrary, but use 2000 pixels for now. This will break
		// when printing, when we should determine just how big the DC we're dealing with is
		clipRgn = ZRect(0, 0, 2000, 2000);
		}
	this->SetClip(clipRgn);

	POINT windowOrg;
	::GetWindowOrgEx(fHDC, &windowOrg);
	this->SetOrigin(-ZPoint(windowOrg));
	}

ZDC_NativeGDI::~ZDC_NativeGDI()
	{
	// Ensure that the canvas's settings are invalidated
	fCanvas->InvalCache();
	// Copy back the state we saved in our constructor
	if (fHDC)
		::RestoreDC(fHDC, fSavedDCIndex);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCSetupForGDI

ZDCSetupForGDI::ZDCSetupForGDI(const ZDC& inDC, bool inZeroOrigin)
:	fCanvas(ZRefDynamicCast<ZDCCanvas_GDI>(inDC.GetCanvas()))
	{
	fOffset = fCanvas->Internal_GDIStart(inDC.GetState(), inZeroOrigin);
	}

ZDCSetupForGDI::~ZDCSetupForGDI()
	{
	fCanvas->Internal_GDIEnd();
	}

ZPoint ZDCSetupForGDI::GetOffset()
	{ return fOffset; }

HDC ZDCSetupForGDI::GetHDC()
	{ return ((ZDCCanvas_GDI*)fCanvas.GetObject())->fHDC; }

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapRep_DIB

static bool sCheckDesc(const ZDCPixmapNS::RasterDesc& inRasterDesc, const ZRect& inBounds,
							const ZDCPixmapNS::PixelDesc& inPixelDesc)
	{
	using namespace ZDCPixmapNS;

	if ((inRasterDesc.fRowBytes % 4) != 0)
		return false;

	bool isOkay = false;
	ZRef<PixelDescRep> theRep = inPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color = ZRefDynamicCast<PixelDescRep_Color>(theRep))
		{
		uint32 maskRed, maskGreen, maskBlue, maskAlpha;
		thePixelDescRep_Color->GetMasks(maskRed, maskGreen, maskBlue, maskAlpha);
		switch (inRasterDesc.fPixvalDesc.fDepth)
			{
			case 16:
				{
				if (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0x7C00 && maskGreen == 0x03E0 && maskBlue == 0x001F)
					{
					isOkay = true;
					}
				break;
				}
			case 24:
				{
				if ((inRasterDesc.fPixvalDesc.fBigEndian == true && maskRed == 0x0000FF && maskGreen == 0x00FF00 && maskBlue == 0xFF0000)
					|| (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0xFF0000 && maskGreen == 0x00FF00 && maskBlue == 0x0000FF))
					{
					isOkay = true;
					}
				break;
				}
			case 32:
				{
				if ((inRasterDesc.fPixvalDesc.fBigEndian == true && maskRed == 0x0000FF00 && maskGreen == 0x00FF0000 && maskBlue == 0xFF000000)
					|| (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0x00FF0000 && maskGreen == 0x0000FF00 && maskBlue == 0x000000FF))
					{
					isOkay = true;
					}
				break;
				}
			}
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = ZRefDynamicCast<PixelDescRep_Gray>(theRep))
		{
		switch (inRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				isOkay = true;
				}
			}
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed = ZRefDynamicCast<PixelDescRep_Indexed>(theRep))
		{
		switch (inRasterDesc.fPixvalDesc.fDepth)
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

static ZRef<ZDCPixmapRep_DIB> sCreateRepForDesc(ZRef<ZDCPixmapRaster> inRaster, const ZRect& inBounds,
							const ZDCPixmapNS::PixelDesc& inPixelDesc)
	{
	if (::sCheckDesc(inRaster->GetRasterDesc(), inBounds, inPixelDesc))
		return new ZDCPixmapRep_DIB(inRaster, inBounds, inPixelDesc);

	return ZRef<ZDCPixmapRep_DIB>();
	}

static BITMAPINFO* sAllocateBITMAPINFOColor(size_t inRowBytes, int32 inDepth, int32 inHeight, bool inFlipped)
	{
	BITMAPINFO* theBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER)]);

	theBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	theBITMAPINFO->bmiHeader.biWidth = (inRowBytes * 8) / inDepth;
	if (inFlipped)
		theBITMAPINFO->bmiHeader.biHeight = inHeight;
	else
		theBITMAPINFO->bmiHeader.biHeight = -inHeight;
	theBITMAPINFO->bmiHeader.biSizeImage = inRowBytes * inHeight;
	theBITMAPINFO->bmiHeader.biPlanes = 1;
	theBITMAPINFO->bmiHeader.biBitCount = inDepth;
	theBITMAPINFO->bmiHeader.biCompression = BI_RGB;
	theBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
	theBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
	theBITMAPINFO->bmiHeader.biClrUsed = 0;
	theBITMAPINFO->bmiHeader.biClrImportant = 0;
	return theBITMAPINFO;
	}

static bool sSetupDIB(const ZDCPixmapNS::RasterDesc& inRasterDesc, const ZDCPixmapNS::PixelDesc& inPixelDesc, BITMAPINFO*& outBITMAPINFO)
	{
	using namespace ZDCPixmapNS;

	if ((inRasterDesc.fRowBytes % 4) != 0)
		return false;

	bool isOkay = false;
	ZRef<PixelDescRep> theRep = inPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color = ZRefDynamicCast<PixelDescRep_Color>(theRep))
		{
		uint32 maskRed, maskGreen, maskBlue, maskAlpha;
		thePixelDescRep_Color->GetMasks(maskRed, maskGreen, maskBlue, maskAlpha);
		switch (inRasterDesc.fPixvalDesc.fDepth)
			{
			case 16:
				{
				if (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0x7C00 && maskGreen == 0x03E0 && maskBlue == 0x001F)
					{
					outBITMAPINFO = sAllocateBITMAPINFOColor(inRasterDesc.fRowBytes, 16, inRasterDesc.fRowCount, inRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			case 24:
				{
				if ((inRasterDesc.fPixvalDesc.fBigEndian == true && maskRed == 0x0000FF && maskGreen == 0x00FF00 && maskBlue == 0xFF0000)
					|| (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0xFF0000 && maskGreen == 0x00FF00 && maskBlue == 0x0000FF))
					{
					outBITMAPINFO = sAllocateBITMAPINFOColor(inRasterDesc.fRowBytes, 24, inRasterDesc.fRowCount, inRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			case 32:
				{
				if ((inRasterDesc.fPixvalDesc.fBigEndian == true && maskRed == 0x0000FF00 && maskGreen == 0x00FF0000 && maskBlue == 0xFF000000)
					|| (inRasterDesc.fPixvalDesc.fBigEndian == false && maskRed == 0x00FF0000 && maskGreen == 0x0000FF00 && maskBlue == 0x000000FF))
					{
					outBITMAPINFO = sAllocateBITMAPINFOColor(inRasterDesc.fRowBytes, 32, inRasterDesc.fRowCount, inRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			}
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = ZRefDynamicCast<PixelDescRep_Gray>(theRep))
		{
		switch (inRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				uint32 colorTableSize = 1 << inRasterDesc.fPixvalDesc.fDepth;
				outBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);
				outBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				outBITMAPINFO->bmiHeader.biWidth = (inRasterDesc.fRowBytes * 8) / inRasterDesc.fPixvalDesc.fDepth;
				if (inRasterDesc.fFlipped)
					outBITMAPINFO->bmiHeader.biHeight = inRasterDesc.fRowCount;
				else
					outBITMAPINFO->bmiHeader.biHeight = -inRasterDesc.fRowCount;
				outBITMAPINFO->bmiHeader.biSizeImage = inRasterDesc.fRowBytes * inRasterDesc.fRowCount;
				outBITMAPINFO->bmiHeader.biPlanes = 1;
				outBITMAPINFO->bmiHeader.biBitCount = inRasterDesc.fPixvalDesc.fDepth;
				outBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				outBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				outBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				outBITMAPINFO->bmiHeader.biClrUsed = colorTableSize;
				outBITMAPINFO->bmiHeader.biClrImportant = 0;

				uint32 multiplier = 0xFFFFFFFFU / (colorTableSize - 1);
				for (size_t x = 0; x < colorTableSize; ++x)
					{
					uint32 temp = (x * multiplier) >> 16;
					outBITMAPINFO->bmiColors[x].rgbRed = temp;
					outBITMAPINFO->bmiColors[x].rgbGreen = temp;
					outBITMAPINFO->bmiColors[x].rgbBlue = temp;
					outBITMAPINFO->bmiColors[x].rgbReserved = 0;
					}
				return true;
				}
			}
		}
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed = ZRefDynamicCast<PixelDescRep_Indexed>(theRep))
		{
		switch (inRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				const ZRGBColorPOD* sourceColors;
				size_t sourceColorsCount;
				thePixelDescRep_Indexed->GetColors(sourceColors, sourceColorsCount);

				size_t colorTableSize = 1 << inRasterDesc.fPixvalDesc.fDepth;
				sourceColorsCount = min(sourceColorsCount, colorTableSize);

				outBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);
				outBITMAPINFO->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				outBITMAPINFO->bmiHeader.biWidth = (inRasterDesc.fRowBytes * 8) / inRasterDesc.fPixvalDesc.fDepth;
				if (inRasterDesc.fFlipped)
					outBITMAPINFO->bmiHeader.biHeight = inRasterDesc.fRowCount;
				else
					outBITMAPINFO->bmiHeader.biHeight = -inRasterDesc.fRowCount;
				outBITMAPINFO->bmiHeader.biSizeImage = inRasterDesc.fRowBytes * inRasterDesc.fRowCount;
				outBITMAPINFO->bmiHeader.biPlanes = 1;
				outBITMAPINFO->bmiHeader.biBitCount = inRasterDesc.fPixvalDesc.fDepth;
				outBITMAPINFO->bmiHeader.biCompression = BI_RGB;
				outBITMAPINFO->bmiHeader.biXPelsPerMeter = 0;
				outBITMAPINFO->bmiHeader.biYPelsPerMeter = 0;
				outBITMAPINFO->bmiHeader.biClrUsed = sourceColorsCount;
				outBITMAPINFO->bmiHeader.biClrImportant = 0;
				for (size_t x = 0; x < sourceColorsCount; ++x)
					{
					outBITMAPINFO->bmiColors[x].rgbRed = sourceColors[x].red;
					outBITMAPINFO->bmiColors[x].rgbGreen = sourceColors[x].green;
					outBITMAPINFO->bmiColors[x].rgbBlue = sourceColors[x].blue;
					outBITMAPINFO->bmiColors[x].rgbReserved = 0;
					}
				return true;
				}
			}
		}
	return false;
	}

ZDCPixmapRep_DIB::ZDCPixmapRep_DIB(ZRef<ZDCPixmapRaster> inRaster,
								const ZRect& inBounds,
								const ZDCPixmapNS::PixelDesc& inPixelDesc)
:	ZDCPixmapRep(inRaster, inBounds, inPixelDesc)
	{
	bool result = ::sSetupDIB(inRaster->GetRasterDesc(), inPixelDesc, fBITMAPINFO);
	ZAssertStop(kDebug_GDI, result);
	fChangeCount = fPixelDesc.GetChangeCount() - 1;
	}

ZDCPixmapRep_DIB::ZDCPixmapRep_DIB(HDC inHDC, HBITMAP inHBITMAP, bool inForce32BPP)
	{
	using namespace ZDCPixmapNS;

	ZAssertStop(kDebug_GDI, inHBITMAP);

	// Get information about inHBITMAP
	BITMAP theBITMAP;
	if (ZUtil_Win::sUseWAPI())
		::GetObjectW(inHBITMAP, sizeof(theBITMAP), &theBITMAP);
	else
		::GetObjectA(inHBITMAP, sizeof(theBITMAP), &theBITMAP);

	int32 absoluteHeight = abs(theBITMAP.bmHeight);

	fBounds = ZPoint(theBITMAP.bmWidth, absoluteHeight);

	if (inForce32BPP)
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

		::GetDIBits(inHDC, inHBITMAP, 0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_PAL_COLORS);

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
				theRasterDesc.fRowBytes =  (((theBITMAP.bmBitsPixel * theBITMAP.bmWidth) + 31) / 32) * 4;
				theRasterDesc.fRowCount = absoluteHeight;
				theRasterDesc.fFlipped = true;
				fRaster = new ZDCPixmapRaster_Simple(theRasterDesc);

				uint32 colorTableSize = 1 << theRasterDesc.fPixvalDesc.fDepth;

				fBITMAPINFO = reinterpret_cast<BITMAPINFO*>(new char[sizeof(BITMAPINFOHEADER) + colorTableSize * sizeof(RGBQUAD)]);
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

				::GetDIBits(inHDC, inHBITMAP, 0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_RGB_COLORS);

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
					vector<ZRGBColorSmallPOD> colorTable(colorTableSize);
					for (size_t x = 0; x < colorTable.size(); ++x)
						{
						colorTable[x].red = fBITMAPINFO->bmiColors[x].rgbRed;
						colorTable[x].green = fBITMAPINFO->bmiColors[x].rgbGreen;
						colorTable[x].blue = fBITMAPINFO->bmiColors[x].rgbBlue;
						colorTable[x].alpha = 0xFFU;
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
				theRasterDesc.fRowBytes =  (((theBITMAP.bmBitsPixel * theBITMAP.bmWidth) + 31) / 32) * 4;
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

				::GetDIBits(inHDC, inHBITMAP, 0, absoluteHeight, fRaster->GetBaseAddress(), fBITMAPINFO, DIB_PAL_COLORS);

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
	fBITMAPINFO = nil;
	}

void ZDCPixmapRep_DIB::GetDIBStuff(BITMAPINFO*& outBITMAPINFO, char*& outBits, ZRect* outBounds)
	{
	using namespace ZDCPixmapNS;

	if (fChangeCount != fPixelDesc.GetChangeCount())
		{
		fChangeCount = fPixelDesc.GetChangeCount();
		if (PixelDescRep_Indexed* thePixelDesc = ZRefDynamicCast<PixelDescRep_Indexed>(fPixelDesc.GetRep()))
			{
			const ZRGBColorPOD* theColors;
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

	outBITMAPINFO = fBITMAPINFO;
	outBits = reinterpret_cast<char*>(fRaster->GetBaseAddress());
	if (outBounds)
		*outBounds = fBounds;
	}

ZRef<ZDCPixmapRep_DIB> ZDCPixmapRep_DIB::sAsPixmapRep_DIB(ZRef<ZDCPixmapRep> inRep)
	{
	using namespace ZDCPixmapNS;

	// Let's see if it's a DIB rep already
	ZRef<ZDCPixmapRep_DIB> theRep_DIB = ZRefDynamicCast<ZDCPixmapRep_DIB>(inRep);
	if (!theRep_DIB)
		{
		ZRef<ZDCPixmapRaster> theRaster = inRep->GetRaster();
		ZRect theBounds = inRep->GetBounds();
		PixelDesc thePixelDesc = inRep->GetPixelDesc();
		theRep_DIB = ::sCreateRepForDesc(theRaster, theBounds, thePixelDesc);
		if (!theRep_DIB)
			{
			EFormatStandard fallbackFormat = eFormatStandard_BGRx_32;
			if (ZRefDynamicCast<PixelDescRep_Gray>(thePixelDesc.GetRep()))
				fallbackFormat = eFormatStandard_Gray_8;

			RasterDesc newRasterDesc(theBounds.Size(), fallbackFormat);
			PixelDesc newPixelDesc(fallbackFormat);
			theRep_DIB = new ZDCPixmapRep_DIB(new ZDCPixmapRaster_Simple(newRasterDesc), theBounds.Size(), newPixelDesc);
			theRep_DIB->CopyFrom(ZPoint(0, 0), inRep, theBounds);
			}
		}
	return theRep_DIB;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapFactory_GDI

class ZDCPixmapFactory_GDI : public ZDCPixmapFactory
	{
public:
	virtual ZRef<ZDCPixmapRep> CreateRep(const ZRef<ZDCPixmapRaster>& inRaster, const ZRect& inBounds, const ZDCPixmapNS::PixelDesc& inPixelDesc);
	virtual ZRef<ZDCPixmapRep> CreateRep(const ZDCPixmapNS::RasterDesc& inRasterDesc, const ZRect& inBounds, const ZDCPixmapNS::PixelDesc& inPixelDesc);
	virtual ZDCPixmapNS::EFormatStandard MapEfficientToStandard(ZDCPixmapNS::EFormatEfficient inFormat);
	};

static ZDCPixmapFactory_GDI sZDCPixmapFactory_GDI;

ZRef<ZDCPixmapRep> ZDCPixmapFactory_GDI::CreateRep(const ZRef<ZDCPixmapRaster>& inRaster, const ZRect& inBounds, const ZDCPixmapNS::PixelDesc& inPixelDesc)
	{
	// This may return nil, which is okay. The factory method will
	// create a non-platform specific rep.
	return ::sCreateRepForDesc(inRaster, inBounds, inPixelDesc);
	}

ZRef<ZDCPixmapRep> ZDCPixmapFactory_GDI::CreateRep(const ZDCPixmapNS::RasterDesc& inRasterDesc, const ZRect& inBounds, const ZDCPixmapNS::PixelDesc& inPixelDesc)
	{
	if (::sCheckDesc(inRasterDesc, inBounds, inPixelDesc))
		return new ZDCPixmapRep_DIB(new ZDCPixmapRaster_Simple(inRasterDesc), inBounds, inPixelDesc);

	return ZRef<ZDCPixmapRep>();
	}

ZDCPixmapNS::EFormatStandard ZDCPixmapFactory_GDI::MapEfficientToStandard(ZDCPixmapNS::EFormatEfficient inFormat)
	{
	using namespace ZDCPixmapNS;

	EFormatStandard standardFormat = eFormatStandard_Invalid;
	switch (inFormat)
		{
//		case eFormatEfficient_Indexed_1: standardFormat = eFormatStandard_Indexed_1; break;
//		case eFormatEfficient_Indexed_8: standardFormat = eFormatStandard_Indexed_8; break;

		case eFormatEfficient_Gray_1: standardFormat = eFormatStandard_Gray_1; break;
		case eFormatEfficient_Gray_8: standardFormat = eFormatStandard_Gray_8; break;

		case eFormatEfficient_Color_16: standardFormat = eFormatStandard_xRGB_16_LE; break;
		case eFormatEfficient_Color_24: standardFormat = eFormatStandard_BGR_24; break;
		case eFormatEfficient_Color_32: standardFormat = eFormatStandard_BGRA_32; break;
		}

	return standardFormat;
	}

#endif // ZCONFIG_API_Enabled(DC_GDI)
