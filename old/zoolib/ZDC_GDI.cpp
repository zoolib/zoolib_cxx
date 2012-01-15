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

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_cpp(DC_GDI)

#include "zoolib/ZCompat_algorithm.h" // For min
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_Win.h"

#include <string.h> // For memcpy

#include <stdexcept> // For runtime_error
#include <vector>

using std::min;
using std::runtime_error;
using std::string;
using std::vector;

namespace ZooLib {

static short spModeLookup[] = { R2_COPYPEN, R2_MERGEPEN, R2_NOTXORPEN, R2_MASKNOTPEN };

// =================================================================================================
// MARK: - kDebug

#define kDebug_GDI 1

// =================================================================================================
// MARK: - Static helper functions

static inline COLORREF spAsCOLORREF_Palette(HDC iHDC, const ZRGBColor& iColor)
	{ return PALETTERGB((iColor.red >> 8), (iColor.green >> 8), (iColor.blue >> 8)); }

static WORD spAllBitsSet[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static HBITMAP spHBITMAP_AllBitsSet = ::CreateBitmap(8, 8, 1, 1, &sAllBitsSet[0]);

static HBRUSH spCreateHBRUSH(HDC iHDC, const ZRef<ZDCInk::Rep>& iRep, ZPoint iRealPatternOrigin)
	{
	HBRUSH theHBRUSH = nullptr;
	if (iRep->fType == ZDCInk::eTypeSolidColor)
		{
		theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);
		}
	else if (iRep->fType == ZDCInk::eTypeTwoColor)
		{
		uint8 tempPattern[8];
		ZDCPattern::sOffset(iRealPatternOrigin.h, iRealPatternOrigin.v,
			iRep->fAsTwoColor.fPattern.pat, tempPattern);
		WORD localBits[8];
		for (size_t x = 0; x < 8; ++x)
			localBits[x] = tempPattern[x];
		HBITMAP theHBITMAP = ::CreateBitmap(8, 8, 1, 1, &localBits[0]);
		theHBRUSH = ::CreatePatternBrush(theHBITMAP);
		::DeleteObject(theHBITMAP);
		}
	else if (iRep->fType == ZDCInk::eTypeMultiColor)
		{
		// We should not be called if iRep is MultiColor, but if we
		// do, create something garish and obvious.
		theHBRUSH = ::CreateSolidBrush(PALETTERGB(0xFF, 0xFF, 0));
		}
	else
		ZDebugStopf(1, ("sCreateHBrush, unknown ink type"));
	return theHBRUSH;
	}

static HFONT spCreateHFONT(const ZDCFont& iFont)
	{
	HFONT theHFONT;
	if (ZUtil_Win::sUseWAPI())
		{
		LOGFONTW theLOGFONT;
		ZMemZero_T(theLOGFONT);

		theLOGFONT.lfHeight = -iFont.GetSize();

		ZDCFont::Style theStyle = iFont.GetStyle();
		if (theStyle & ZDCFont::bold)
			theLOGFONT.lfWeight = FW_SEMIBOLD;
		else
			theLOGFONT.lfWeight = FW_NORMAL;
	
		if (theStyle & ZDCFont::italic)
			theLOGFONT.lfItalic = 0xFF;

		if (theStyle & ZDCFont::underline)
			theLOGFONT.lfUnderline = 0xFF;

		theLOGFONT.lfQuality = ANTIALIASED_QUALITY;

		string16 fontName = ZUnicode::sAsUTF16(iFont.GetName());
		ZMemCopy(theLOGFONT.lfFaceName, fontName.c_str(), sizeof(UTF16) * (fontName.size() + 1));
		theHFONT = ::CreateFontIndirectW(&theLOGFONT);
		}
	else
		{
		LOGFONTA theLOGFONT;
		ZMemZero_T(theLOGFONT);

		theLOGFONT.lfHeight = -iFont.GetSize();

		ZDCFont::Style theStyle = iFont.GetStyle();
		if (theStyle & ZDCFont::bold)
			theLOGFONT.lfWeight = FW_SEMIBOLD;
		else
			theLOGFONT.lfWeight = FW_NORMAL;

		if (theStyle & ZDCFont::italic)
			theLOGFONT.lfItalic = 0xFF;

		if (theStyle & ZDCFont::underline)
			theLOGFONT.lfUnderline = 0xFF;
		
		string fontName = iFont.GetName();
		ZMemCopy(theLOGFONT.lfFaceName, fontName.c_str(), (fontName.size() + 1));
		theHFONT = ::CreateFontIndirectA(&theLOGFONT);
		}
	return theHFONT;
	}

// =================================================================================================
// MARK: - ZDCCanvas_GDI::SetupDC

ZDCCanvas_GDI::SetupDC::SetupDC(ZDCCanvas_GDI* iCanvas, const ZDCState& iState)
:	fCanvas(iCanvas)
	{
	fState = &iState;
	ZAssertStop(kDebug_GDI, fCanvas->fMutexToCheck == nullptr || fCanvas->fMutexToCheck->IsLocked());
	fCanvas->fMutexToLock->Acquire();
	if (!fCanvas->fOriginValid)
		{
		fCanvas->fOriginValid = true;
		if (fCanvas->IsPrinting())
			{
			::SetMapMode(iCanvas->fHDC, MM_ANISOTROPIC);
			int deviceHRes = ::GetDeviceCaps(iCanvas->fHDC, LOGPIXELSX);
			int deviceVRes = ::GetDeviceCaps(iCanvas->fHDC, LOGPIXELSY);
			::SetViewportExtEx(iCanvas->fHDC, deviceHRes, deviceVRes, nullptr);
			::SetWindowExtEx(iCanvas->fHDC, 72, 72, nullptr);
			}
		else
			{
			::SetMapMode(iCanvas->fHDC, MM_TEXT);
			}

		::SetWindowOrgEx(iCanvas->fHDC, 0, 0, nullptr);
		}

	if (iState.fChangeCount_Clip != fCanvas->fChangeCount_Clip)
		{
		ZDCRgn theClipRgn = fCanvas->Internal_CalcClipRgn(iState);
		if (fCanvas->IsPrinting())
			{
			int deviceHRes = ::GetDeviceCaps(fCanvas->fHDC, LOGPIXELSX);
			int deviceVRes = ::GetDeviceCaps(fCanvas->fHDC, LOGPIXELSY);
			float hScale = float(deviceHRes) / 72.0f;
			float vScale = float(deviceVRes) / 72.0f;
			theClipRgn.MakeScale(hScale, 0.0f, 0.0f, vScale, 0.0f, 0.0f);
			}
		::SelectClipRgn(fCanvas->fHDC, theClipRgn.GetHRGN());
		const_cast<ZDCState&>(iState).fChangeCount_Clip = ++fCanvas->fChangeCount_Clip;
		}
	}

ZDCCanvas_GDI::SetupDC::~SetupDC()
	{
	ZAssertStop(0, fState->fChangeCount_Clip == fCanvas->fChangeCount_Clip);
	fCanvas->fMutexToLock->Release();
	}

// =================================================================================================
// MARK: - ZDCCanvas_GDI::SetupHPEN

class ZDCCanvas_GDI::SetupHPEN
	{
public:
	SetupHPEN(ZDCCanvas_GDI* iCanvas, ZDCState& ioState);
	SetupHPEN(HDC iHDC, const ZRGBColor& iColor, ZCoord iPenWidth);
	~SetupHPEN();

protected:
	HDC fHDC;

	HBRUSH fHBRUSH_Saved;
	HPEN fHPEN_Saved;
	int fROP2_Saved;
	};

ZDCCanvas_GDI::SetupHPEN::SetupHPEN(ZDCCanvas_GDI* iCanvas, ZDCState& ioState)
:	fHDC(iCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep && theRep->fType == ZDCInk::eTypeSolidColor);
	HPEN theHPEN = ::CreatePen(PS_INSIDEFRAME, ioState.fPenWidth,
		sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));

	fHPEN_Saved = (HPEN)::SelectObject(fHDC, theHPEN);
	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, ::GetStockObject(NULL_BRUSH));
	fROP2_Saved = ::SetROP2(fHDC, spModeLookup[ioState.fMode]);
	}

ZDCCanvas_GDI::SetupHPEN::SetupHPEN(HDC iHDC, const ZRGBColor& iColor, ZCoord iPenWidth)
:	fHDC(iHDC)
	{
	HPEN theHPEN = ::CreatePen(PS_INSIDEFRAME, iPenWidth, sAsCOLORREF_Palette(fHDC, iColor));
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
// MARK: - ZDCCanvas_GDI::SetupHBRUSH

class ZDCCanvas_GDI::SetupHBRUSH
	{
public:
	SetupHBRUSH(ZDCCanvas_GDI* iCanvas, ZDCState& ioState);
	SetupHBRUSH(HDC iHDC, const ZRGBColor& iColor);
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

ZDCCanvas_GDI::SetupHBRUSH::SetupHBRUSH(ZDCCanvas_GDI* iCanvas, ZDCState& ioState)
:	fHDC(iCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep);
	HBRUSH theHBRUSH;
	if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);

		fSetTextColor = false;
		fSetBackColor = true;
		fBackColor_Saved = ::SetBkColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));
		}
	else
		{
		ZAssertStop(kDebug_GDI, theRep->fType == ZDCInk::eTypeTwoColor);
		fSetTextColor = true;
		fSetBackColor = true;
		// The seeming reversal of terms here is because GDI applies the
		// back color to a set bit and the fore/text color to a cleared bit.
		fTextColor_Saved = ::SetTextColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fBackColor));

		fBackColor_Saved = ::SetBkColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fForeColor));

		uint8 tempPattern[8];
		ZDCPattern::sOffset(ioState.fPatternOrigin.h, ioState.fPatternOrigin.v,
			theRep->fAsTwoColor.fPattern.pat, tempPattern);
		WORD localBits[8];
		for (size_t x = 0; x < 8; ++x)
			localBits[x] = tempPattern[x];

		HBITMAP theHBITMAP = ::CreateBitmap(8, 8, 1, 1, &localBits[0]);
		theHBRUSH = ::CreatePatternBrush(theHBITMAP);
		::DeleteObject(theHBITMAP);
		}

	fHBRUSH_Saved = (HBRUSH)::SelectObject(fHDC, theHBRUSH);
	fHPEN_Saved = (HPEN)::SelectObject(fHDC, ::GetStockObject(NULL_PEN));
	fROP2_Saved = ::SetROP2(fHDC, spModeLookup[ioState.fMode]);
	}

ZDCCanvas_GDI::SetupHBRUSH::SetupHBRUSH(HDC iHDC, const ZRGBColor& iColor)
:	fHDC(iHDC)
	{
	HBRUSH theHBRUSH = ::CreatePatternBrush(sHBITMAP_AllBitsSet);

	fSetTextColor = false;
	fSetBackColor = true;
	fBackColor_Saved = ::SetBkColor(fHDC, sAsCOLORREF_Palette(fHDC, iColor));

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
// MARK: - ZDCCanvas_GDI::SetupModeColor

class ZDCCanvas_GDI::SetupModeColor
	{
public:
	SetupModeColor(ZDCCanvas_GDI* iCanvas, ZDCState& ioState);
	~SetupModeColor();

protected:
	HDC fHDC;
	int fROP2_Saved;
	bool fSetTextColor;
	bool fSetBackColor;
	COLORREF fTextColor_Saved;
	COLORREF fBackColor_Saved;
	};

ZDCCanvas_GDI::SetupModeColor::SetupModeColor(ZDCCanvas_GDI* iCanvas, ZDCState& ioState)
:	fHDC(iCanvas->fHDC)
	{
	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	ZAssertStop(kDebug_GDI, theRep);
	if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		fSetTextColor = false;
		fSetBackColor = true;
		fBackColor_Saved = ::SetBkColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsSolidColor.fColor));
		}
	else
		{
		ZAssertStop(kDebug_GDI, theRep->fType == ZDCInk::eTypeTwoColor);
		fSetTextColor = true;
		fSetBackColor = true;
		// GDI has a different nomenclature for fore/back colors, hence the seeming reversal here.
		fTextColor_Saved = ::SetTextColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fBackColor));
		fBackColor_Saved = ::SetBkColor(fHDC,
			sAsCOLORREF_Palette(fHDC, theRep->fAsTwoColor.fForeColor));
		}

	fROP2_Saved = ::SetROP2(fHDC, spModeLookup[ioState.fMode]);
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
// MARK: - ZDCCanvas_GDI::SetupHFONT

class ZDCCanvas_GDI::SetupHFONT
	{
public:
	SetupHFONT(ZDCCanvas_GDI* iCanvas, ZDCState& ioState);
	~SetupHFONT();

protected:
	HDC fHDC;

	HFONT fHFONT_Saved;
	int fROP2_Saved;
	COLORREF fTextColor_Saved;
	};

ZDCCanvas_GDI::SetupHFONT::SetupHFONT(ZDCCanvas_GDI* iCanvas, ZDCState& ioState)
:	fHDC(iCanvas->fHDC)
	{
	HFONT theHFONT = sCreateHFONT(ioState.fFont);
	ZAssertStop(kDebug_GDI, theHFONT);
	fHFONT_Saved = (HFONT)::SelectObject(fHDC, theHFONT);
	fROP2_Saved = ::SetROP2(fHDC, spModeLookup[ioState.fMode]);

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
// MARK: - ZDCCanvas_GDI_Native

class ZDCCanvas_GDI_Native : public ZDCCanvas_GDI_NonWindow
	{
public:
	ZDCCanvas_GDI_Native(HDC iHDC);
	virtual ~ZDCCanvas_GDI_Native();
	};

ZDCCanvas_GDI_Native::ZDCCanvas_GDI_Native(HDC iHDC)
	{
	fHDC = iHDC;
	this->Internal_Link();
	}

ZDCCanvas_GDI_Native::~ZDCCanvas_GDI_Native()
	{
	fHDC = nullptr;
	}

// =================================================================================================
// MARK: - ZDCCanvas_GDI

ZMutex sMutex_List;

ZDCCanvas_GDI* ZDCCanvas_GDI::sCanvas_Head;

ZRef<ZDCCanvas_GDI> ZDCCanvas_GDI::sFindCanvasOrCreateNative(HDC iHDC)
	{
	ZMutexLocker locker(sMutex_List);
	ZDCCanvas_GDI* theCanvas = sCanvas_Head;
	while (theCanvas)
		{
		if (theCanvas->fHDC == iHDC)
			return theCanvas;
		theCanvas = theCanvas->fCanvas_Next;
		}
	theCanvas = new ZDCCanvas_GDI_Native(iHDC);
	return theCanvas;
	}

ZDCCanvas_GDI::ZDCCanvas_GDI()
	{
	fCanvas_Prev = nullptr;
	fCanvas_Next = nullptr;

	fOriginValid = false;
	fChangeCount_Clip = 1;

	fHDC = nullptr;

	fMutexToLock = nullptr;
	fMutexToCheck = nullptr;
	}


ZDCCanvas_GDI::~ZDCCanvas_GDI()
	{
	// Subclasses must unlink themselves and detach/destroy
	// grafports before this destructor is called.
	ZAssertStop(kDebug_GDI, fCanvas_Prev == nullptr);
	ZAssertStop(kDebug_GDI, fCanvas_Next == nullptr);
	ZAssertStop(kDebug_GDI, fHDC == nullptr);
	ZAssertStop(kDebug_GDI, fMutexToLock == nullptr);
	ZAssertStop(kDebug_GDI, fMutexToCheck == nullptr);
	}

void ZDCCanvas_GDI::Internal_Link()
	{
	ZAssertStop(kDebug_GDI, sMutex_List.IsLocked());
	fCanvas_Prev = nullptr;
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
	fCanvas_Prev = nullptr;
	fCanvas_Next = nullptr;
	}

// =================================================================================================

void ZDCCanvas_GDI::Pixel(ZDCState& ioState,
	ZCoord iLocationH, ZCoord iLocationV, const ZRGBColor& iColor)
	{
	if (!fHDC)
		return;

	SetupDC theSetupDC(this, ioState);
	::SetPixelV(fHDC,
		iLocationH + ioState.fOrigin.h, iLocationV + ioState.fOrigin.v,
		sAsCOLORREF_Palette(fHDC, iColor));
	}

static void spBuildPolygonForLine(
	ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV, ZCoord iPenWidth, POINT* oPOINTS)
	{
	// For notes on this see AG's log book, 97-03-05
	ZPoint leftMostPoint, rightMostPoint;
	if (iStartH < iEndH)
		{
		leftMostPoint.h = iStartH;
		leftMostPoint.v = iStartV;
		rightMostPoint.h = iEndH;
		rightMostPoint.v = iEndV;
		}
	else
		{
		leftMostPoint.h = iEndH;
		leftMostPoint.v = iEndV;
		rightMostPoint.h = iStartH;
		rightMostPoint.v = iStartV;
		}
	// Two cases, from top left down to bottom right, or from bottom left up to top right
	if (leftMostPoint.v < rightMostPoint.v)
		{
		oPOINTS[0].x = leftMostPoint.h;
		oPOINTS[0].y = leftMostPoint.v;

		oPOINTS[1].x = leftMostPoint.h + iPenWidth;
		oPOINTS[1].y = leftMostPoint.v;

		oPOINTS[2].x = rightMostPoint.h + iPenWidth;
		oPOINTS[2].y = rightMostPoint.v;

		oPOINTS[3].x = rightMostPoint.h + iPenWidth;
		oPOINTS[3].y = rightMostPoint.v + iPenWidth;

		oPOINTS[4].x = rightMostPoint.h;
		oPOINTS[4].y = rightMostPoint.v + iPenWidth;

		oPOINTS[5].x = leftMostPoint.h;
		oPOINTS[5].y = leftMostPoint.v + iPenWidth;
		}
	else
		{
		oPOINTS[0].x = leftMostPoint.h;
		oPOINTS[0].y = leftMostPoint.v;

		oPOINTS[1].x = rightMostPoint.h;
		oPOINTS[1].y = rightMostPoint.v;

		oPOINTS[2].x = rightMostPoint.h + iPenWidth;
		oPOINTS[2].y = rightMostPoint.v;

		oPOINTS[3].x = rightMostPoint.h + iPenWidth;
		oPOINTS[3].y = rightMostPoint.v + iPenWidth;

		oPOINTS[4].x = leftMostPoint.h + iPenWidth;
		oPOINTS[4].y = leftMostPoint.v + iPenWidth;

		oPOINTS[5].x = leftMostPoint.h;
		oPOINTS[5].y = leftMostPoint.v + iPenWidth;
		}
	}

void ZDCCanvas_GDI::Line(ZDCState& ioState,
	ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV)
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
		if (iStartH == iEndH)
			{
			// Vertical line
			ZRect theRect;
			theRect.left = iStartH;
			theRect.right = iEndH + ioState.fPenWidth;
			if (iStartV < iEndV)
				{
				theRect.top = iStartV;
				theRect.bottom = iEndV + ioState.fPenWidth;
				}
			else
				{
				theRect.top = iEndV;
				theRect.bottom = iStartV + ioState.fPenWidth;
				}
			this->Internal_TileRegion(ioState, theRect, *theRep->fAsMultiColor.fPixmap);
			}
		else if (iStartV == iEndV)
			{
			// Horizontal line
			ZRect theRect;
			theRect.top = iStartV;
			theRect.bottom = iStartV + ioState.fPenWidth;
			if (iStartH < iEndH)
				{
				theRect.left = iStartH;
				theRect.right = iEndH + ioState.fPenWidth;
				}
			else
				{
				theRect.left = iEndH;
				theRect.right = iStartH + ioState.fPenWidth;
				}
			this->Internal_TileRegion(ioState, theRect, *theRep->fAsMultiColor.fPixmap);
			}
		else
			{
			// Some kind of diagonal
			ZDCRgn theRgn = ZDCRgn::sLine(iStartH, iStartV, iEndH, iEndV, ioState.fPenWidth);
			this->Internal_TileRegion(ioState, theRgn, *theRep->fAsMultiColor.fPixmap);
			}
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		if (theRep->fType == ZDCInk::eTypeSolidColor
			&& ioState.fPenWidth == 1 && !this->IsPrinting())
			{
			SetupHPEN theSetupHPEN(this, ioState);
			ZPoint leftMostPoint, rightMostPoint;
			if (iStartH <= iEndH)
				{
				leftMostPoint.h = iStartH;
				leftMostPoint.v = iStartV;
				rightMostPoint.h = iEndH;
				rightMostPoint.v = iEndV;
				}
			else
				{
				leftMostPoint.h = iEndH;
				leftMostPoint.v = iEndV;
				rightMostPoint.h = iStartH;
				rightMostPoint.v = iStartV;
				}
			POINT dummyOldPoint;
			::MoveToEx(fHDC, leftMostPoint.h + ioState.fOrigin.h,
				leftMostPoint.v + ioState.fOrigin.v, &dummyOldPoint);
			ZCoord hDiff = rightMostPoint.h - leftMostPoint.h;
			if (leftMostPoint.v <= rightMostPoint.v)
				{
				// Down and to right
				ZCoord vDiff = rightMostPoint.v - leftMostPoint.v;
				if (vDiff < hDiff)
					{
					// Less than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1,
						rightMostPoint.v + ioState.fOrigin.v);
					}
				else if (vDiff > hDiff)
					{
					// Greater than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h,
						rightMostPoint.v + ioState.fOrigin.v + 1);
					}
				else
					{
					// Exactly 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1,
						rightMostPoint.v + ioState.fOrigin.v + 1);
					}
				}
			else
				{
				// Up and to right
				ZCoord vDiff = leftMostPoint.v - rightMostPoint.v;
				if (vDiff < hDiff)
					{
					// Less than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1,
						rightMostPoint.v + ioState.fOrigin.v);
					}
				else if (vDiff > hDiff)
					{
					// Greater than 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h,
						rightMostPoint.v + ioState.fOrigin.v - 1);
					}
				else
					{
					// Exactly 45 degrees
					::LineTo(fHDC, rightMostPoint.h + ioState.fOrigin.h + 1,
						rightMostPoint.v + ioState.fOrigin.v - 1);
					}
				}

			}
		else
			{
			SetupHBRUSH theSetupHBRUSH(this, ioState);
			if (iStartH == iEndH)
				{
				// Vertical line
				ZCoord top, bottom;
				if (iStartV < iEndV)
					{
					top = iStartV;
					bottom = iEndV;
					}
				else
					{
					top = iEndV;
					bottom = iStartV;
					}
				::Rectangle(fHDC,
					iStartH + ioState.fOrigin.h,
					top + ioState.fOrigin.v,
					iStartH + ioState.fOrigin.h + ioState.fPenWidth + 1,
					bottom + ioState.fOrigin.v + ioState.fPenWidth + 1);
				}
			else if (iStartV == iEndV)
				{
				// Horizontal line
				ZCoord left, right;
				if (iStartH < iEndH)
					{
					left = iStartH;
					right = iEndH;
					}
				else
					{
					left = iEndH;
					right = iStartH;
					}
				::Rectangle(fHDC,
					left + ioState.fOrigin.h,
					iStartV + ioState.fOrigin.v,
					right + ioState.fOrigin.h + ioState.fPenWidth + 1,
					iStartV + ioState.fOrigin.v + ioState.fPenWidth + 1);
				}
			else
				{
				// Some kind of diagonal
				POINT thePOINTS[6];
				sBuildPolygonForLine(iStartH + ioState.fOrigin.h,
					iStartV + ioState.fOrigin.v,
					iEndH + ioState.fOrigin.h,
					iEndV + ioState.fOrigin.v,
					ioState.fPenWidth, thePOINTS);
				::Polygon(fHDC, thePOINTS, 6);
				}
			}
		}
	}

void ZDCCanvas_GDI::FrameRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (iRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		if (iRect.Width() <= ioState.fPenWidth * 2 || iRect.Height() <= ioState.fPenWidth * 2)
			this->Internal_TileRegion(ioState, iRect, *theRep->fAsMultiColor.fPixmap);
		else
			{
			// T
			this->Internal_TileRegion(ioState,
				ZRect(iRect.left, iRect.top, iRect.right, iRect.top + ioState.fPenWidth),
				*theRep->fAsMultiColor.fPixmap);
			// L
			this->Internal_TileRegion(ioState,
				ZRect(iRect.left, iRect.top + ioState.fPenWidth,
				iRect.left + ioState.fPenWidth, iRect.bottom - ioState.fPenWidth),
				*theRep->fAsMultiColor.fPixmap);
			// B
			this->Internal_TileRegion(ioState,
				ZRect(iRect.left, iRect.bottom - ioState.fPenWidth,
				iRect.right, iRect.bottom),
				*theRep->fAsMultiColor.fPixmap);
			// R
			this->Internal_TileRegion(ioState,
				ZRect(iRect.right - ioState.fPenWidth, iRect.top + ioState.fPenWidth,
				iRect.right, iRect.bottom - ioState.fPenWidth),
				*theRep->fAsMultiColor.fPixmap);
			}
		}
	else if (theRep->fType == ZDCInk::eTypeSolidColor)
		{
		SetupDC theSetupDC(this, ioState);
		SetupHPEN theSetupHPEN(this, ioState);
		::Rectangle(fHDC,
			iRect.left + ioState.fOrigin.h, iRect.top + ioState.fOrigin.v,
			iRect.right + ioState.fOrigin.h, iRect.bottom + ioState.fOrigin.v);
		}
	else
		{
		ZRect realRect = iRect + ioState.fOrigin;
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		if (iRect.Width() <= ioState.fPenWidth * 2 || iRect.Height() <= ioState.fPenWidth * 2)
			::Rectangle(fHDC, iRect.left, realRect.top, realRect.right + 1, realRect.bottom + 1);
		else
			{
			// Top
			::Rectangle(fHDC, realRect.left, realRect.top,
				realRect.right + 1, realRect.top + ioState.fPenWidth + 1);
			// Left
			::Rectangle(fHDC, realRect.left, realRect.top,
				realRect.left + ioState.fPenWidth + 1, realRect.bottom + 1);
			// Bottom
			::Rectangle(fHDC, realRect.left, realRect.bottom - ioState.fPenWidth,
				realRect.right + 1, realRect.bottom + 1);
			// Right
			::Rectangle(fHDC, realRect.right - ioState.fPenWidth, realRect.top,
				realRect.right + 1, realRect.bottom + 1);
			}
		}
	}

void ZDCCanvas_GDI::FillRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (!fHDC)
		return;

	if (iRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, iRect, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		ZRect realRect = iRect + ioState.fOrigin;
		::Rectangle(fHDC, realRect.left, realRect.top, realRect.right + 1, realRect.bottom + 1);
		}
	}

void ZDCCanvas_GDI::InvertRect(ZDCState& ioState, const ZRect& iRect)
	{
	if (!fHDC)
		return;

	if (iRect.IsEmpty())
		return;

	RECT realRECT = iRect + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRect(fHDC, &realRECT);
	}

void ZDCCanvas_GDI::FrameRegion(ZDCState& ioState, const ZDCRgn& iRgn)
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
		this->Internal_TileRegion(ioState,
			(iRgn - iRgn.Inset(ioState.fPenWidth, ioState.fPenWidth)),
			*theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FrameRgn(fHDC, (iRgn + ioState.fOrigin).GetHRGN(),
			theHBRUSH, ioState.fPenWidth, ioState.fPenWidth);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillRegion(ZDCState& ioState, const ZDCRgn& iRgn)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, iRgn, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, (iRgn + ioState.fOrigin).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertRegion(ZDCState& ioState, const ZDCRgn& iRgn)
	{
	if (!fHDC)
		return;

	ZDCRgn realRgn = iRgn + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, realRgn.GetHRGN());
	}

void ZDCCanvas_GDI::FrameRoundRect(ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (iRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZDCRgn outerRgn = ZDCRgn::sRoundRect(iRect, iCornerSize);
		ZDCRgn innerRgn =
			ZDCRgn::sRoundRect(iRect.Inset(ioState.fPenWidth, ioState.fPenWidth), iCornerSize);
		this->Internal_TileRegion(ioState, (outerRgn - innerRgn), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		ZRect realBounds = iRect + (ioState.fOrigin);
		ZDCRgn outerRgn = ZDCRgn::sRoundRect(realBounds, iCornerSize);
		ZDCRgn innerRgn =
			ZDCRgn::sRoundRect(realBounds.left + ioState.fPenWidth,
			realBounds.top + ioState.fPenWidth,
			realBounds.right - ioState.fPenWidth,
			realBounds.bottom - ioState.fPenWidth,
			iCornerSize.h - ioState.fPenWidth, iCornerSize.v - ioState.fPenWidth);
		::FillRgn(fHDC, (outerRgn - innerRgn).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillRoundRect(ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize)
	{
	if (!fHDC)
		return;

	if (iRect.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState,
			ZDCRgn::sRoundRect(iRect, iCornerSize), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC,
			ZDCRgn::sRoundRect(iRect + ioState.fOrigin, iCornerSize).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertRoundRect(
	ZDCState& ioState, const ZRect& iRect, const ZPoint& iCornerSize)
	{
	if (!fHDC)
		return;

	ZRect realRect = iRect + ioState.fOrigin;
	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, ZDCRgn::sRoundRect(iRect + ioState.fOrigin, iCornerSize).GetHRGN());
	}

void ZDCCanvas_GDI::FrameEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	if (!fHDC)
		return;

	if (ioState.fPenWidth <= 0)
		return;

	if (iBounds.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	// Windows is inconsistent in how it renders ellipse frames and interiors. So we have
	// to do them manually, otherwise interiors leak over frames and frames are drawn
	// outside of the bounds we've been passed.
	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZDCRgn outerRgn = ZDCRgn::sEllipse(iBounds);
		ZDCRgn innerRgn = ZDCRgn::sEllipse(iBounds.Inset(ioState.fPenWidth, ioState.fPenWidth));
		this->Internal_TileRegion(ioState, outerRgn - innerRgn, *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);

		ZRect realBounds = iBounds + ioState.fOrigin;
		ZDCRgn outerRgn = ZDCRgn::sEllipse(realBounds);
		ZDCRgn innerRgn = ZDCRgn::sEllipse(realBounds.Inset(ioState.fPenWidth, ioState.fPenWidth));

		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, (outerRgn - innerRgn).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::FillEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	if (!fHDC)
		return;

	if (iBounds.IsEmpty())
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	// Windows is inconsistent in how it renders ellipse frames and interiors. So
	// we have to do them manually, otherwise interiors leak over frames and frames
	// are drawn outside of the bounds we've been passed.
	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState,
			ZDCRgn::sEllipse(iBounds), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		SetupDC theSetupDC(this, ioState);
		SetupModeColor theSetupModeColor(this, ioState);
		HBRUSH theHBRUSH = sCreateHBRUSH(fHDC, theRep, ioState.fPatternOrigin);
		::FillRgn(fHDC, ZDCRgn::sEllipse(iBounds + ioState.fOrigin).GetHRGN(), theHBRUSH);
		::DeleteObject(theHBRUSH);
		}
	}

void ZDCCanvas_GDI::InvertEllipse(ZDCState& ioState, const ZRect& iBounds)
	{
	if (!fHDC)
		return;

	if (iBounds.IsEmpty())
		return;

	SetupDC theSetupDC(this, ioState);
	::InvertRgn(fHDC, ZDCRgn::sEllipse(iBounds + ioState.fOrigin).GetHRGN());
	}

void ZDCCanvas_GDI::FillPoly(ZDCState& ioState, const ZDCPoly& iPoly)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		this->Internal_TileRegion(ioState, ZDCRgn::sPoly(iPoly), *theRep->fAsMultiColor.fPixmap);
		}
	else
		{
		size_t theCount;
		POINT* thePOINTs;
		iPoly.GetPOINTs(ioState.fOrigin, thePOINTs, theCount);
		SetupDC theSetupDC(this, ioState);
		SetupHBRUSH theSetupHBRUSH(this, ioState);
		::Polygon(fHDC, thePOINTs, theCount);
		}
	}

void ZDCCanvas_GDI::InvertPoly(ZDCState& ioState, const ZDCPoly& iPoly)
	{
	if (!fHDC)
		return;

	size_t theCount;
	POINT* thePOINTs;
	iPoly.GetPOINTs(ioState.fOrigin, thePOINTs, theCount);

	SetupDC theSetupDC(this, ioState);
	// Do the inversion by using an ink mode
	int oldMode = ::GetROP2(fHDC);
	::SetROP2(fHDC, R2_NOT);
	::Polygon(fHDC, thePOINTs, theCount);
	::SetROP2(fHDC, oldMode);
	}

void ZDCCanvas_GDI::FillPixmap(ZDCState& ioState, const ZPoint& iLocation, const ZDCPixmap& iPixmap)
	{
	// Placeholder for code to tile the current ink using inPixmap as a mask
	ZUnimplemented();
	}

void ZDCCanvas_GDI::DrawPixmap(ZDCState& ioState,
	const ZPoint& iLocation, const ZDCPixmap& iSourcePixmap, const ZDCPixmap* iMaskPixmap)
	{
	if (fHDC == nullptr)
		return;

	if (!iSourcePixmap)
		return;

	SetupDC theSetupDC(this, ioState);

	ZPoint realLocation = iLocation + ioState.fOrigin;

	ZRef<ZDCPixmapRep_DIB> sourceRepDIB
		= ZDCPixmapRep_DIB::sAsPixmapRep_DIB(iSourcePixmap.GetRep());
	BITMAPINFO* sourceBMI;
	char* sourceBits;
	ZRect sourceDIBBounds;
	sourceRepDIB->GetDIBStuff(sourceBMI, sourceBits, &sourceDIBBounds);

	ZRect localSourceBounds = sourceDIBBounds.Size();

	if (iMaskPixmap && *iMaskPixmap && !this->IsPrinting())
		{
		// Take a copy of the source
		HDC sourceCopyHDC = ::CreateCompatibleDC(fHDC);
		HBITMAP sourceCopyHBITMAP =
			::CreateCompatibleBitmap(fHDC, localSourceBounds.Width(), localSourceBounds.Height());
		HBITMAP sourceCopyOriginalHBITMAP =
			(HBITMAP)::SelectObject(sourceCopyHDC, sourceCopyHBITMAP);

		::StretchDIBits(sourceCopyHDC,
			0, 0, localSourceBounds.Width(), localSourceBounds.Height(),
			localSourceBounds.left + sourceDIBBounds.left,
			localSourceBounds.top + sourceDIBBounds.top,
			localSourceBounds.Width(),
			localSourceBounds.Height(),
			sourceBits, sourceBMI, DIB_RGB_COLORS, SRCCOPY);


		// Draw the mask *over* the source copy -- every
		// place the mask is black, so will the source be.
		ZRef<ZDCPixmapRep_DIB> maskRepDIB
			= ZDCPixmapRep_DIB::sAsPixmapRep_DIB(iMaskPixmap->GetRep());
		BITMAPINFO* maskBMI;
		char* maskBits;
		ZRect maskDIBBounds;
		maskRepDIB->GetDIBStuff(maskBMI, maskBits, &maskDIBBounds);

		::StretchDIBits(sourceCopyHDC,
			0, 0, localSourceBounds.Width(), localSourceBounds.Height(),
			localSourceBounds.left + maskDIBBounds.left,
			localSourceBounds.top + maskDIBBounds.top,
			localSourceBounds.Width(),
			localSourceBounds.Height(),
			maskBits, maskBMI, DIB_RGB_COLORS, SRCAND);

		// And the inverse of the mask with the destination -- white pixels in the mask will be
		// black in the destination. Black pixels in the mask will leave the destination alone.
		::StretchDIBits(fHDC,
			realLocation.h, realLocation.v, localSourceBounds.Width(), localSourceBounds.Height(),
			localSourceBounds.left + maskDIBBounds.left,
			localSourceBounds.top + maskDIBBounds.top,
			localSourceBounds.Width(),
			localSourceBounds.Height(),
			maskBits, maskBMI, DIB_RGB_COLORS, 0x220326); // ~S & D

		// Draw the copy of the source to the screen, using SRCPAINT. Anything
		// black won't touch the screen, any other color will be ored in
		::BitBlt(fHDC, realLocation.h, realLocation.v,
			localSourceBounds.Width(), localSourceBounds.Height(), sourceCopyHDC, 0, 0, SRCPAINT);

		// All done, clean up
		::SelectObject(sourceCopyHDC, sourceCopyOriginalHBITMAP);
		::DeleteDC(sourceCopyHDC);
		::DeleteObject(sourceCopyHBITMAP);
		}
	else
		{
		::StretchDIBits(fHDC,
			realLocation.h, realLocation.v, localSourceBounds.Width(), localSourceBounds.Height(),
			localSourceBounds.left + sourceDIBBounds.left,
			localSourceBounds.top + sourceDIBBounds.top,
			localSourceBounds.Width(),
			localSourceBounds.Height(),
			sourceBits, sourceBMI, DIB_RGB_COLORS, SRCCOPY);
		}
	}

void ZDCCanvas_GDI::DrawText(ZDCState& ioState,
	const ZPoint& iLocation, const char* iText, size_t iTextLength)
	{
	if (!fHDC || iTextLength == 0)
		return;

	SetupDC theSetupDC(this, ioState);
	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString16 = ZUnicode::sAsUTF16(iText, iTextLength);
		::ExtTextOutW(fHDC,
			iLocation.h + ioState.fOrigin.h,
			iLocation.v + ioState.fOrigin.v, 0, nullptr, theString16.data(), theString16.size(), nullptr);
		}
	else
		{
		::TextOutA(fHDC,
			iLocation.h + ioState.fOrigin.h, iLocation.v + ioState.fOrigin.v, iText, iTextLength);
		}
	}

ZCoord ZDCCanvas_GDI::GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength)
	{
	if (!fHDC || iTextLength == 0)
		return 0;

	ZMutexLocker locker(*fMutexToLock);

	SetupHFONT theSetupHFONT(this, ioState);
	SIZE theTextSize;
	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString16 = ZUnicode::sAsUTF16(iText, iTextLength);
		::GetTextExtentPoint32W(fHDC, theString16.data(), theString16.size(), &theTextSize);
		}
	else
		{
		::GetTextExtentPoint32A(fHDC, iText, iTextLength, &theTextSize);
		}

	return theTextSize.cx;
	}

void ZDCCanvas_GDI::GetFontInfo(
	ZDCState& ioState, ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading)
	{
	if (!fHDC)
		{
		oAscent = 0;
		oDescent = 0;
		oLeading = 0;
		return;
		}

	ZMutexLocker locker(*fMutexToLock);

	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		TEXTMETRICW theTextMetrics;
		::GetTextMetricsW(fHDC, &theTextMetrics);
		oAscent = theTextMetrics.tmAscent;
		oDescent = theTextMetrics.tmDescent;
		oLeading = theTextMetrics.tmInternalLeading;
		}
	else
		{
		TEXTMETRICA theTextMetrics;
		::GetTextMetricsA(fHDC, &theTextMetrics);
		oAscent = theTextMetrics.tmAscent;
		oDescent = theTextMetrics.tmDescent;
		oLeading = theTextMetrics.tmInternalLeading;
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
		TEXTMETRICW theTM;
		::GetTextMetricsW(fHDC, &theTM);
		return theTM.tmAscent + theTM.tmDescent + theTM.tmInternalLeading;
		}
	else
		{
		TEXTMETRICA theTM;
		::GetTextMetricsA(fHDC, &theTM);
		return theTM.tmAscent + theTM.tmDescent + theTM.tmInternalLeading;
		}
	}

void ZDCCanvas_GDI::BreakLine(ZDCState& ioState,
	const char* iLineStart, size_t iRemainingTextLength,
	ZCoord iTargetWidth, size_t& oNextLineDelta)
	{
	if (!fHDC)
		{
		oNextLineDelta = iRemainingTextLength;
		return;
		}

	if (iRemainingTextLength == 0)
		{
		oNextLineDelta = 0;
		return;
		}

	if (iTargetWidth < 0)
		iTargetWidth = 0;

	SetupDC theSetupDC(this, ioState);
	SetupHFONT theSetupHFONT(this, ioState);

	if (ZUtil_Win::sUseWAPI())
		{
		string16 theString = ZUnicode::sAsUTF16(iLineStart, iRemainingTextLength);
		SIZE dummySIZE;
		int brokenLength = 0;
		::GetTextExtentExPointW(fHDC, theString.data(), theString.size(),
			iTargetWidth, &brokenLength, nullptr, &dummySIZE);
		size_t codePointOffset = ZUnicode::sCUToCP(theString.data(), brokenLength);
		oNextLineDelta = ZUnicode::sCPToCU(iLineStart, iRemainingTextLength, codePointOffset, nullptr);
		}
	else
		{
		SIZE dummySIZE;
		int brokenLength = 0;
		::GetTextExtentExPointA(fHDC, iLineStart, iRemainingTextLength,
			iTargetWidth, &brokenLength, nullptr, &dummySIZE);
		oNextLineDelta = brokenLength;
		}

	// Check for a '\n' or '\r' before the natural breaking
	// point of the line (walking from beginning to end)
	for (size_t x = 0; x < oNextLineDelta; ++x)
		{
		char theChar = iLineStart[x];
		if (theChar == '\n' || theChar == '\r')
			{
			// Found one. Use its position as the break length.
			oNextLineDelta = x + 1;
			return;
			}
		}
	if (oNextLineDelta < iRemainingTextLength)
		{
		for (size_t x = oNextLineDelta; x > 0; --x)
			{
			char theChar = iLineStart[x - 1];
			if (isspace(theChar))
				{
				// Found one. Use its position as the break length.
				oNextLineDelta = x;
				break;
				}
			}
		}

	if (oNextLineDelta == 0)
		oNextLineDelta = 1;
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

ZRef<ZDCCanvas> ZDCCanvas_GDI::CreateOffScreen(
	ZPoint dimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	if (!fHDC)
		return ZRef<ZDCCanvas>();

	ZMutexLocker locker(*fMutexToLock);

	return new ZDCCanvas_GDI_OffScreen(fHDC, dimensions, iFormat);
	}

ZPoint ZDCCanvas_GDI::Internal_GDIStart(ZDCState& ioState, bool iZeroOrigin)
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
	if (!iZeroOrigin)
		theOffset = ioState.fOrigin;
	::SetWindowOrgEx(fHDC, -theOffset.h, -theOffset.v, nullptr);

	return theOffset;
	}

void ZDCCanvas_GDI::Internal_GDIEnd()
	{
	this->InvalCache();
	}

void ZDCCanvas_GDI::Internal_TileRegion(
	ZDCState& ioState, const ZDCRgn& iRgn, const ZDCPixmap& iPixmap)
	{
	ZMutexLocker locker(*fMutexToLock);

	if (!fOriginValid)
		{
		fOriginValid = true;
		::SetMapMode(fHDC, MM_TEXT);
		::SetWindowOrgEx(fHDC, 0, 0, nullptr);
		}

	ZDCRgn realRgn = (iRgn + ioState.fOrigin) & this->Internal_CalcClipRgn(ioState);
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

	ZRef<ZDCPixmapRep_DIB> pixmapRepDIB = ZDCPixmapRep_DIB::sAsPixmapRep_DIB(iPixmap.GetRep());
	BITMAPINFO* sourceBMI;
	char* sourceBits;
	ZRect rasterBounds;
	pixmapRepDIB->GetDIBStuff(sourceBMI, sourceBits, &rasterBounds);
	ZPoint pixmapSize = rasterBounds.Size();
	ZRect drawnBounds = realRgn.Bounds();
	ZCoord drawnOriginH = drawnBounds.left
		- ((((drawnBounds.left + ioState.fPatternOrigin.h)
			% pixmapSize.h) + pixmapSize.h) % pixmapSize.h);
	ZCoord drawnOriginV = drawnBounds.top
		- ((((drawnBounds.top + ioState.fPatternOrigin.v)
			% pixmapSize.v) + pixmapSize.v) % pixmapSize.v);

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
// MARK: - ZDCCanvas_GDI_NonWindow

ZDCCanvas_GDI_NonWindow::ZDCCanvas_GDI_NonWindow()
	{
	fMutexToLock = &fMutex;
	}

ZDCCanvas_GDI_NonWindow::~ZDCCanvas_GDI_NonWindow()
	{
	ZAssertStop(kDebug_GDI, fHDC == nullptr);
	fMutexToLock = nullptr;
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

void ZDCCanvas_GDI_NonWindow::Scroll(ZDCState& ioState,
	const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV)
	{
	if (!fHDC)
		return;

	SetupDC theSetupDC(this, ioState);
	RECT realRECT = iRect + ioState.fOrigin;
	::ScrollDC(fHDC, iDeltaH, iDeltaV, &realRECT, &realRECT, nullptr, nullptr);
	}

void ZDCCanvas_GDI_NonWindow::CopyFrom(ZDCState& ioState, const ZPoint& iDestLocation,
	ZRef<ZDCCanvas> iSourceCanvas, const ZDCState& iSourceState, const ZRect& iSourceRect)
	{
	ZRef<ZDCCanvas_GDI> sourceCanvasGDI = ZRefDynamicCast<ZDCCanvas_GDI>(iSourceCanvas);

	ZAssertStop(kDebug_GDI, sourceCanvasGDI);

	if (fHDC == nullptr || sourceCanvasGDI->Internal_GetHDC() == nullptr)
		return;

	SetupDC theSetupDC(this, ioState);
	SetupDC theSetupDCSource(sourceCanvasGDI.GetObject(), iSourceState);

	ZPoint realSource = iSourceRect.TopLeft() + iSourceState.fOrigin;
	ZRect realDest = ZRect(iSourceRect.Size()) + (iDestLocation + ioState.fOrigin);
	::BitBlt(fHDC, realDest.left, realDest.top, realDest.Width(), realDest.Height(),
		sourceCanvasGDI->Internal_GetHDC(), realSource.h, realSource.v, SRCCOPY);
	}

ZDCRgn ZDCCanvas_GDI_NonWindow::Internal_CalcClipRgn(const ZDCState& iState)
	{
	return iState.fClip + iState.fClipOrigin;
	}

// =================================================================================================
// MARK: - ZDCCanvas_GDI_OffScreen

static HPALETTE spHPALETTE_Offscreen = ::CreateHalftonePalette(nullptr);

ZDCCanvas_GDI_OffScreen::ZDCCanvas_GDI_OffScreen(HDC iOther, const ZRect& iGlobalRect)
	{
	if (iGlobalRect.IsEmpty())
		throw runtime_error("ZDCCanvas_GDI_OffScreen, inGlobalRect.IsEmpty()");

	fOriginValid = false;

	fHBITMAP = ::CreateCompatibleBitmap(iOther, iGlobalRect.Width(), iGlobalRect.Height());
	fHDC = ::CreateCompatibleDC(iOther);
	HBITMAP theOriginalHBITMAP = (HBITMAP)::SelectObject(fHDC, fHBITMAP);
	::DeleteObject(theOriginalHBITMAP);

	::SelectPalette(fHDC, sHPALETTE_Offscreen, true);
	::RealizePalette(fHDC);

	ZAssertStop(kDebug_GDI, fHDC);
	ZAssertStop(kDebug_GDI, fHBITMAP);

	ZMutexLocker locker(sMutex_List);
	this->Internal_Link();
	}

static short spFormatEfficientToDepth(ZDCPixmapNS::EFormatEfficient iFormat)
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

ZDCCanvas_GDI_OffScreen::ZDCCanvas_GDI_OffScreen(
	HDC iOther, ZPoint iDimensions, ZDCPixmapNS::EFormatEfficient iFormat)
	{
	using namespace ZDCPixmapNS;

	if (iDimensions.h <= 0 || iDimensions.v <= 0)
		throw runtime_error("ZDCCanvas_GDI_OffScreen, zero sized offscreen");

	short depth = spFormatEfficientToDepth(iFormat);

	BITMAPINFO theBITMAPINFO;
	theBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	theBITMAPINFO.bmiHeader.biWidth = iDimensions.h;
	theBITMAPINFO.bmiHeader.biHeight = iDimensions.v;
	theBITMAPINFO.bmiHeader.biPlanes = 1;
	theBITMAPINFO.bmiHeader.biBitCount = depth;
	theBITMAPINFO.bmiHeader.biCompression = BI_RGB;
	short rowBytes = ((depth * iDimensions.h + 31) / 32) * 4;
	theBITMAPINFO.bmiHeader.biSizeImage = rowBytes * iDimensions.v;
	theBITMAPINFO.bmiHeader.biXPelsPerMeter = 0;
	theBITMAPINFO.bmiHeader.biYPelsPerMeter = 0;
	theBITMAPINFO.bmiHeader.biClrUsed = 0;
	theBITMAPINFO.bmiHeader.biClrImportant = 0;

	void* thePixelStorage;
	fHBITMAP = ::CreateDIBSection(iOther, &theBITMAPINFO, DIB_RGB_COLORS, &thePixelStorage, nullptr, 0);
	fHDC = ::CreateCompatibleDC(iOther);
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
	fHDC = nullptr;
	if (fHBITMAP)
		::DeleteObject(fHBITMAP);
	}

ZRGBColor ZDCCanvas_GDI_OffScreen::GetPixel(ZDCState& ioState, ZCoord iLocationH, ZCoord iLocationV)
	{
	SetupDC theSetupDC(this, ioState);
	return ZRGBColor::sFromCOLORREF(::GetPixel(fHDC,
		iLocationH + ioState.fOrigin.h, iLocationV + ioState.fOrigin.v));
	}

void ZDCCanvas_GDI_OffScreen::FloodFill(ZDCState& ioState, const ZPoint& iSeedLocation)
	{
	if (!fHDC)
		return;

	ZRef<ZDCInk::Rep> theRep = ioState.fInk.GetRep();
	if (!theRep)
		return;

	ZPoint realSeedLocation = iSeedLocation + ioState.fOrigin;
	ZDCRgn realClip = ioState.fClip + ioState.fClipOrigin;
	if (!realClip.Contains(iSeedLocation))
		return;

	SetupDC theSetupDC(this, ioState);
	COLORREF theSeedCOLORREF = ::GetPixel(fHDC, realSeedLocation.h, realSeedLocation.v);

	if (theRep->fType == ZDCInk::eTypeMultiColor)
		{
		ZRect clipBounds = realClip.Bounds();

		// To do a flood fill with a multi color ink (ie a pixmap) we have to help out some.
		// First we take a one bit copy of our HDC, where every pixel that matches the seed
		// pixel's color is set to white, and all other pixels are set to black.
		HDC theHDC1 = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(theHDC1,
			::CreateBitmap(clipBounds.Width(), clipBounds.Height(), 1, 1, nullptr)));

		// Set our back color to the seed color, so the StretchBlt
		// knows which color should be mapped to white
		COLORREF oldBackCOLORREF = ::SetBkColor(fHDC, theSeedCOLORREF);
		::StretchBlt(theHDC1, 0, 0, clipBounds.Width(), clipBounds.Height(),
			fHDC, clipBounds.left, clipBounds.top,
			clipBounds.Width(), clipBounds.Height(), SRCCOPY);

		// Create a second HDC with a copy of the 1 bit HDC
		HDC theHDC2 = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(theHDC2,
			::CreateBitmap(clipBounds.Width(), clipBounds.Height(), 1, 1, nullptr)));
		::BitBlt(theHDC2, 0, 0, clipBounds.Width(), clipBounds.Height(),
			theHDC1, 0, 0, SRCCOPY);

		// Now we do a flood fill to black of theHDC1, the first monochrome copy of our HDC
		HBRUSH tempHBRUSH = (HBRUSH)::SelectObject(
			theHDC1, ::CreateSolidBrush(PALETTERGB(0, 0, 0)));

		::ExtFloodFill(theHDC1,
			realSeedLocation.h - clipBounds.left, realSeedLocation.v - clipBounds.top,
				PALETTERGB(0xFF, 0xFF, 0xFF), FLOODFILLSURFACE);
		::DeleteObject(::SelectObject(theHDC1, tempHBRUSH));

		// We now have theHDC2 containing white *everywhere* fHDC contains our seed color, and
		// theHDC1 containing white everywhere fHDC contains our seed color *except* for those
		// pixels reachable from the seed location. The exclusive or of the two HDCs thus identify
		// exactly those pixels in fHDC that match the seed color, and are reachable from the
		// seed location.
		::BitBlt(theHDC1, 0, 0, clipBounds.Width(), clipBounds.Height(),
			theHDC2, 0, 0, SRCINVERT);
		// We can discard theHDC2
		::DeleteDC(theHDC2);

		// Use theHDC1 to set to black every pixel we're going to be filling
		::BitBlt(fHDC, clipBounds.left, clipBounds.top, clipBounds.Width(), clipBounds.Height(),
			theHDC1, 0, 0, 0x00220326); // (NOT src) AND dest -- DSna

		// Get our ink's pixmap into an HDC
		ZRef<ZDCPixmapRep_DIB> inkRepDIB
			= ZDCPixmapRep_DIB::sAsPixmapRep_DIB(theRep->fAsMultiColor.fPixmap->GetRep());
		BITMAPINFO* inkBMI;
		char* inkBits;
		ZRect theInkBounds;
		inkRepDIB->GetDIBStuff(inkBMI, inkBits, &theInkBounds);
		ZPoint theInkPixmapSize = theInkBounds.Size();
		HDC inkHDC = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(inkHDC,
			::CreateCompatibleBitmap(fHDC, theInkPixmapSize.h, theInkPixmapSize.v)));

		::StretchDIBits(inkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v,
			theInkBounds.left, theInkBounds.top, theInkPixmapSize.h, theInkPixmapSize.v,
			inkBits, inkBMI, DIB_RGB_COLORS, SRCCOPY);

		// Create another HDC for the ink, one that will hold each tile before it is blitted in
		HDC intermediateInkHDC = ::CreateCompatibleDC(fHDC);
		::DeleteObject(::SelectObject(intermediateInkHDC,
			::CreateCompatibleBitmap(fHDC, theInkPixmapSize.h, theInkPixmapSize.v)));

		// Now tile our ink into fHDC by taking a copy of the ink for each tile, setting to
		// black all pixels that aren't in theHDC1 and ORing the result into fHDC.

		ZCoord drawnOriginH = clipBounds.left
			- ((((clipBounds.left + ioState.fPatternOrigin.h)
				% theInkPixmapSize.h) + theInkPixmapSize.h) % theInkPixmapSize.h);

		ZCoord drawnOriginV = clipBounds.top
			- ((((clipBounds.top + ioState.fPatternOrigin.v)
				% theInkPixmapSize.v) + theInkPixmapSize.v) % theInkPixmapSize.v);

		for (ZCoord y = drawnOriginV; y < clipBounds.bottom; y += theInkPixmapSize.v)
			{
			for (ZCoord x = drawnOriginH; x < clipBounds.right; x += theInkPixmapSize.h)
				{
				::BitBlt(intermediateInkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v,
					inkHDC, 0, 0, SRCCOPY);

				::BitBlt(intermediateInkHDC, 0, 0, theInkPixmapSize.h, theInkPixmapSize.v,
					theHDC1, x - clipBounds.left, y - clipBounds.top, SRCAND);

				::BitBlt(fHDC, x, y, theInkPixmapSize.h, theInkPixmapSize.v,
					intermediateInkHDC, 0, 0, SRCPAINT);
				}
			}

		::DeleteDC(inkHDC);
		::DeleteDC(intermediateInkHDC);
		::DeleteDC(theHDC1);
		}
	else
		{
		SetupHBRUSH theSetupHBRUSH(this, ioState);

		::ExtFloodFill(fHDC, realSeedLocation.h, realSeedLocation.v,
			theSeedCOLORREF, FLOODFILLSURFACE);
		}
	}

ZDCPixmap ZDCCanvas_GDI_OffScreen::GetPixmap(ZDCState& ioState, const ZRect& iBounds)
	{
	if (fHDC == nullptr || iBounds.IsEmpty())
		return ZDCPixmap();

	SetupDC theSetupDC(this, ioState);

	ZRect realBounds = iBounds + ioState.fOrigin;

	HDC resultHDC = ::CreateCompatibleDC(fHDC);
	HBITMAP resultHBITMAP = ::CreateCompatibleBitmap(fHDC, realBounds.Width(), realBounds.Height());
	HBITMAP oldResultHBITMAP = (HBITMAP)::SelectObject(resultHDC, resultHBITMAP);
	::BitBlt(resultHDC, 0, 0, realBounds.Width(), realBounds.Height(),
		fHDC, realBounds.Left(), realBounds.Top(), SRCCOPY);
	::SelectObject(resultHDC, oldResultHBITMAP);

	ZRef<ZDCPixmapRep_DIB> theRep_DIB = new ZDCPixmapRep_DIB(resultHDC, resultHBITMAP);

	::DeleteDC(resultHDC);
	::DeleteObject(resultHBITMAP);

	return ZDCPixmap(theRep_DIB.GetObject());
	}

bool ZDCCanvas_GDI_OffScreen::IsOffScreen()
	{ return true; }

// =================================================================================================
// MARK: - ZDCCanvas_GDI_IC

ZDCCanvas_GDI_IC::ZDCCanvas_GDI_IC()
	{
	if (ZUtil_Win::sUseWAPI())
		fHDC = ::CreateICW(L"DISPLAY", nullptr, nullptr, nullptr);
	else
		fHDC = ::CreateICA("DISPLAY", nullptr, nullptr, nullptr);

	ZMutexLocker locker(sMutex_List);
	this->Internal_Link();
	}

ZDCCanvas_GDI_IC::~ZDCCanvas_GDI_IC()
	{
	::DeleteDC(fHDC);
	fHDC = nullptr;
	}

// =================================================================================================
// MARK: - ZDC_NativeGDI

ZDC_NativeGDI::ZDC_NativeGDI(HDC iHDC)
	{
	fHDC = iHDC;
	fSavedDCIndex = ::SaveDC(fHDC);
	fCanvas = ZDCCanvas_GDI::sFindCanvasOrCreateNative(fHDC);

	ZDCRgn clipRgn;
	if (::GetClipRgn(fHDC, clipRgn.GetHRGN()) == 0)
		{
		// If no clipRgn is set then create one -- this is arbitrary,
		// but use 2000 pixels for now. This will break when printing, when we
		// should determine just how big the DC we're dealing with is
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
// MARK: - ZDCSetupForGDI

ZDCSetupForGDI::ZDCSetupForGDI(const ZDC& iDC, bool iZeroOrigin)
:	fCanvas(ZRefDynamicCast<ZDCCanvas_GDI>(iDC.GetCanvas()))
	{
	fOffset = fCanvas->Internal_GDIStart(iDC.GetState(), iZeroOrigin);
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
// MARK: - ZDCPixmapRep_DIB

static bool spCheckDesc(const ZDCPixmapNS::RasterDesc& iRasterDesc, const ZRect& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
	{
	using namespace ZDCPixmapNS;

	if ((iRasterDesc.fRowBytes % 4) != 0)
		return false;

	bool isOkay = false;
	ZRef<PixelDescRep> theRep = iPixelDesc.GetRep();
	if (PixelDescRep_Color* thePixelDescRep_Color = ZRefDynamicCast<PixelDescRep_Color>(theRep))
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
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = ZRefDynamicCast<PixelDescRep_Gray>(theRep))
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
	else if (PixelDescRep_Indexed* thePixelDescRep_Indexed
		= ZRefDynamicCast<PixelDescRep_Indexed>(theRep))
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
	const ZRect& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
	{
	if (sCheckDesc(iRaster->GetRasterDesc(), iBounds, iPixelDesc))
		return new ZDCPixmapRep_DIB(iRaster, iBounds, iPixelDesc);

	return ZRef<ZDCPixmapRep_DIB>();
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
	if (PixelDescRep_Color* thePixelDescRep_Color = ZRefDynamicCast<PixelDescRep_Color>(theRep))
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
					oBITMAPINFO = sAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 16,
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
					oBITMAPINFO = sAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 24,
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
					oBITMAPINFO = sAllocateBITMAPINFOColor(iRasterDesc.fRowBytes, 32,
						iRasterDesc.fRowCount, iRasterDesc.fFlipped);
					return true;
					}
				break;
				}
			}
		}
	else if (PixelDescRep_Gray* thePixelDescRep_Gray = ZRefDynamicCast<PixelDescRep_Gray>(theRep))
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
		= ZRefDynamicCast<PixelDescRep_Indexed>(theRep))
		{
		switch (iRasterDesc.fPixvalDesc.fDepth)
			{
			case 1:
			case 4:
			case 8:
				{
				const ZRGBColorPOD* sourceColors;
				size_t sourceColorsCount;
				thePixelDescRep_Indexed->GetColors(sourceColors, sourceColorsCount);

				size_t colorTableSize = 1 << iRasterDesc.fPixvalDesc.fDepth;
				sourceColorsCount = min(sourceColorsCount, colorTableSize);

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
	const ZRect& iBounds,
	const ZDCPixmapNS::PixelDesc& iPixelDesc)
:	ZDCPixmapRep(iRaster, iBounds, iPixelDesc)
	{
	bool result = sSetupDIB(iRaster->GetRasterDesc(), iPixelDesc, fBITMAPINFO);
	ZAssertStop(kDebug_GDI, result);
	fChangeCount = fPixelDesc.GetChangeCount() - 1;
	}

ZDCPixmapRep_DIB::ZDCPixmapRep_DIB(HDC iHDC, HBITMAP iHBITMAP, bool iForce32BPP)
	{
	using namespace ZDCPixmapNS;

	ZAssertStop(kDebug_GDI, iHBITMAP);

	// Get information about inHBITMAP
	BITMAP theBITMAP;
	if (ZUtil_Win::sUseWAPI())
		::GetObjectW(iHBITMAP, sizeof(theBITMAP), &theBITMAP);
	else
		::GetObjectA(iHBITMAP, sizeof(theBITMAP), &theBITMAP);

	int32 absoluteHeight = abs(theBITMAP.bmHeight);

	fBounds = ZPoint(theBITMAP.bmWidth, absoluteHeight);

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
					vector<ZRGBColorPOD> colorTable(colorTableSize);
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

void ZDCPixmapRep_DIB::GetDIBStuff(BITMAPINFO*& oBITMAPINFO, char*& oBits, ZRect* oBounds)
	{
	using namespace ZDCPixmapNS;

	if (fChangeCount != fPixelDesc.GetChangeCount())
		{
		fChangeCount = fPixelDesc.GetChangeCount();
		if (PixelDescRep_Indexed* thePixelDesc
			= ZRefDynamicCast<PixelDescRep_Indexed>(fPixelDesc.GetRep()))
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

	oBITMAPINFO = fBITMAPINFO;
	oBits = reinterpret_cast<char*>(fRaster->GetBaseAddress());
	if (oBounds)
		*oBounds = fBounds;
	}

ZRef<ZDCPixmapRep_DIB> ZDCPixmapRep_DIB::sAsPixmapRep_DIB(ZRef<ZDCPixmapRep> iRep)
	{
	using namespace ZDCPixmapNS;

	// Let's see if it's a DIB rep already
	ZRef<ZDCPixmapRep_DIB> theRep_DIB = ZRefDynamicCast<ZDCPixmapRep_DIB>(iRep);
	if (!theRep_DIB)
		{
		ZRef<ZDCPixmapRaster> theRaster = iRep->GetRaster();
		ZRect theBounds = iRep->GetBounds();
		PixelDesc thePixelDesc = iRep->GetPixelDesc();
		theRep_DIB = sCreateRepForDesc(theRaster, theBounds, thePixelDesc);
		if (!theRep_DIB)
			{
			EFormatStandard fallbackFormat = eFormatStandard_BGRx_32;
			if (ZRefDynamicCast<PixelDescRep_Gray>(thePixelDesc.GetRep()))
				fallbackFormat = eFormatStandard_Gray_8;

			RasterDesc newRasterDesc(theBounds.Size(), fallbackFormat);
			PixelDesc newPixelDesc(fallbackFormat);
			theRep_DIB = new ZDCPixmapRep_DIB(
				new ZDCPixmapRaster_Simple(newRasterDesc), theBounds.Size(), newPixelDesc);

			theRep_DIB->CopyFrom(ZPoint(0, 0), iRep, theBounds);
			}
		}
	return theRep_DIB;
	}

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_CreateRaster
:	public ZFunctionChain_T<ZRef<ZDCPixmapRep>, const ZDCPixmapRep::CreateRaster_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = sCreateRepForDesc(iParam.f0, iParam.f1, iParam.f2);
		return oResult;
		}	
	} sMaker0;
	

class Make_CreateRasterDesc
:	public ZFunctionChain_T<ZRef<ZDCPixmapRep>, const ZDCPixmapRep::CreateRasterDesc_t&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (sCheckDesc(iParam.f0, iParam.f1, iParam.f2))
			{
			oResult = new ZDCPixmapRep_DIB(
				new ZDCPixmapRaster_Simple(iParam.f0), iParam.f1, iParam.f2);
			return true;
			}
		return false;
		}	
	} sMaker1;


class Make_EfficientToStandard
:	public ZFunctionChain_T<ZDCPixmapNS::EFormatStandard, ZDCPixmapNS::EFormatEfficient>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		using namespace ZDCPixmapNS;

		switch (iParam)
			{
			case eFormatEfficient_Gray_1: oResult = eFormatStandard_Gray_1; return true;
			case eFormatEfficient_Gray_8: oResult = eFormatStandard_Gray_8; return true;

			case eFormatEfficient_Color_16: oResult = eFormatStandard_xRGB_16_LE; return true;
			case eFormatEfficient_Color_24: oResult = eFormatStandard_BGR_24; return true;
			case eFormatEfficient_Color_32: oResult = eFormatStandard_BGRA_32; return true;
			}

		return false;
		}	
	} sMaker2;

} // anonymous namespace

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(DC_GDI)
