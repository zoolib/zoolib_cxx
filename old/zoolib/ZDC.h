/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, ic.
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

#ifndef __ZDC_h__
#define __ZDC_h__ 1
#include "zconfig.h"

#include "zoolib/ZCounted.h"
#include "zoolib/ZDCFont.h"
#include "zoolib/ZDCInk.h"
#include "zoolib/ZDCPoly.h"
#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZDCRgn.h"
#include "zoolib/ZGeom.h"
#include "zoolib/ZRGBColor.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCState

/// Holds the state in the context of which drawing should be done by a ZDC.
struct ZDCState
	{
	long fChangeCount_Origin;
	long fChangeCount_PatternOrigin;
	long fChangeCount_Ink;
	long fChangeCount_PenWidth;
	long fChangeCount_Mode;
	long fChangeCount_Clip;
	long fChangeCount_Font;
	long fChangeCount_TextColor;

	ZPoint fOriginComp;
	ZPoint fOrigin;
	ZPoint fPatternOrigin;
	ZDCInk fInk;
	ZCoord fPenWidth;
	short fMode;
	ZDCRgn fClip;
	ZPoint fClipOrigin;
	ZDCFont fFont;
	ZRGBColor fTextColor;
	};

// =================================================================================================
// MARK: - ZDC

class ZDCCanvas;

/// ZDC is a drawing context.

class ZDC
	{
public:
/** \name The mantra (constructor, copy constructor, destructor, assignment) */
	//@{
	ZDC();
	ZDC(const ZDC& iOther);
	~ZDC();
	ZDC& operator=(const ZDC& iOther);
	//@}

/** \name More useful constructors */
	//@{
	/// Constructs a new offscreen pixmap of the size and format specified.
	ZDC(ZPoint iSize, ZDCPixmapNS::EFormatEfficient iFormat);

	/// Reference an existing canvas.
	ZDC(const ZRef<ZDCCanvas>& iCanvas);

	/// Reference an existing canvas, but map our origin to the canvas' local \a iOriginComp.
	ZDC(const ZRef<ZDCCanvas>& iCanvas, ZPoint iOriginComp);
	//@}


/** \name Changing and fetching state */
	//@{
	/// Reset the pen width, mode and ink to defaults.
	void PenNormal();

	void SetInk(const ZDCInk& iInk);
	const ZDCInk& GetInk() const;

	void SetPenWidth(const ZCoord& iWidth);
	ZCoord GetPenWidth() const;

	enum { modeCopy, modeOr, modeXor };
	void SetMode(short iMode);
	short GetMode() const;

	void SetFont(const ZDCFont& iFont);
	const ZDCFont& GetFont() const;

	void SetTextColor(const ZRGBColor& iTextColor);
	const ZRGBColor& GetTextColor() const;

	void SetClip(const ZDCRgn& iRgn);
	ZDCRgn GetClip() const;

	void SetOrigin(const ZPoint& iOrigin);
	void OffsetOrigin(const ZPoint& iDelta);
	ZPoint GetOrigin() const;

	void SetPatternOrigin(const ZPoint& iPatternOrigin);
	void OffsetPatternOrigin(const ZPoint& iDelta);
	ZPoint GetPatternOrigin() const;

	short GetDepth() const;
	bool IsOffScreen() const;
	bool IsPrinting() const;
	//@}

// ------------------------------------------------------------------------------------------
// Imaging stuff

/** \name Single pixel */
	//@{
	void Pixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBColor& iColor) const;
	void Pixel(const ZPoint& iPoint, const ZRGBColor& iColor) const;
	ZRGBColor GetPixel(ZPoint iLocation) const;
	ZRGBColor GetPixel(ZCoord iLocationH, ZCoord iLocationV) const;
	//@}

/** \name Multiple pixels from a pseudo pixmap */
	//@{
	void Pixels(ZCoord iLocationH, ZCoord iLocationV,
		ZCoord iWidth, ZCoord iHeight,
		const char* iPixvals, const ZRGBColorMap* iColorMap, size_t iColorMapSize) const;

	void Pixels(ZPoint iLocation, ZPoint iSize,
		const char* iPixvals, const ZRGBColorMap* iColorMap, size_t iColorMapSize) const;
	//@}

/** \name Line */
	//@{
	void Line(ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV) const;
	void Line(const ZPoint& iStart, const ZPoint& iEnd) const;
	//@}

/** \name Rectangle */
	//@{
	void Frame(const ZRect& iRect) const;
	void Fill(const ZRect& iRect) const;
	void Invert(const ZRect& iRect) const;
	void Hilite(const ZRect& iRect, const ZRGBColor& iBackColor) const;
	//@}

/** \name Region */
	//@{
	void Frame(const ZDCRgn& iRgn) const;
	void Fill(const ZDCRgn& iRgn) const;
	void Invert(const ZDCRgn& iRgn) const;
	void Hilite(const ZDCRgn& iRgn, const ZRGBColor& iBackColor) const;
	//@}

/** \name Round cornered rect */
	//@{
	void Frame(const ZRect& iRect, const ZPoint& iCornerSize) const;
	void Fill(const ZRect& iRect, const ZPoint& iCornerSize) const;
	void Invert(const ZRect& iRect, const ZPoint& iCornerSize) const;
	void Hilite(const ZRect& iRect, const ZPoint& iCornerSize, const ZRGBColor& iBackColor) const;
	//@}

/** \name Ellipse */
	//@{
	void FrameEllipse(const ZRect& iBounds) const;
	void FillEllipse(const ZRect& iBounds) const;
	void InvertEllipse(const ZRect& iBounds) const;
	void HiliteEllipse(const ZRect& iBounds, const ZRGBColor& iBackColor) const;
	//@}

/** \name Poly
We do not provide a call to frame a poly. This is because it is an operation whose effect is
hard to define rigorously and consistently (think about framing a figure eight, which pixels
do we touch?). However, I'm open to comments and suggestions on this issue. */
	//@{
	void Fill(const ZDCPoly& iPoly) const;
	void Invert(const ZDCPoly& iPoly) const;
	void Hilite(const ZDCPoly& iPoly, const ZRGBColor& iBackColor) const;
	//@}

/** \name Pixmap */
	//@{
	void Fill(const ZPoint& iLocation, const ZDCPixmap& iPixmap) const;

	void Draw(const ZPoint& iLocation, const ZDCPixmap& iPixmap) const;
	void Draw(const ZPoint& iLocation, const ZDCPixmap& iPixmap, const ZDCPixmap& iMask) const;
	void Draw(const ZPoint& iLocation, const ZDCPixmap& iPixmap, bool iUseAlpha) const;

	ZDCPixmap GetPixmap(const ZRect& iBounds) const;
	//@}

/** \name Flood Fill */
	//@{
	void FloodFill(const ZPoint& iSeedLocation) const;
	//@}

/** \name Moving blocks of pixels */
	//@{
	void Scroll(const ZRect& iRect, const ZPoint& iDelta) const;
	void Scroll(const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV) const;

	void CopyFrom(const ZPoint& iLocation, const ZDC& iSource, const ZRect& iSourceBounds) const;
	//@}

/** \name Text */
	//@{
	void Draw(const ZPoint& iLocation, const char* iText, size_t iTextLength) const;
	void Draw(const ZPoint& iLocation, const std::string& iString) const;
	void DrawText(const ZPoint& iLocation, const char* iText, size_t iTextLength) const;
	void DrawText(const ZPoint& iLocation, const std::string& iString) const;

	ZCoord GetTextWidth(const std::string& iString) const;
	ZCoord GetTextWidth(const char* iText, size_t iTextLength) const;
	void GetFontInfo(ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading) const;
	ZCoord GetLineHeight() const;
	void BreakLine(const char* iLineStart, size_t iRemainingTextLength,
		ZCoord iTargetWidth, size_t& oNextLineDelta) const;
	//@}

/** \name Syncing */
	//@{
	void Sync() const;
	void Flush() const;
	//@}

protected:
/** \name Protected stuff */
	//@{
	void CopySettings(const ZDC& iOther);
	void ZeroSettings();
	void ZeroChangeCounts();
	//@}

// Maintenance of state, and change counts to maintain synchronization with underlying canvas
	mutable ZDCState fState;
	ZRef<ZDCCanvas> fCanvas;

public:
/** \name Escape hatch
Exposing our canvas and state -- DO NOT MISUSE!!! */
	//@{
	ZRef<ZDCCanvas> GetCanvas() const;
	ZDCState& GetState() const;
	//@}
	};

// =================================================================================================
// MARK: - ZDCCanvas

class ZDCCanvas : public ZCounted
	{
protected:
	ZDCCanvas();
	virtual ~ZDCCanvas();

public:
	 static inline bool sCheckAccessEnabled() { return false; }

// The full mode "suite". Only three even close to defined for all platforms so far.
//	enum { modeCopy, modeOr, modeXor, modeBic, modeNotCopy, modeNotOr, modeNotXor, modeNotBic };
	enum { modeCopy, modeOr, modeXor };

// Simple pixel and lines
	virtual void Pixel(ZDCState& ioState, ZCoord iLocationH, ZCoord iLocationV,
		const ZRGBColor& iColor) = 0;

	virtual ZRGBColor GetPixel(ZDCState& ioState, ZCoord iLocationH, ZCoord iLocationV);

	virtual void Pixels(ZDCState& ioState,
		ZCoord iLocationH, ZCoord iLocationV,
		ZCoord iWidth, ZCoord iHeight,
		const char* iPixvals, const ZRGBColorMap* iColorMap, size_t iColorMapSize);

	virtual void Line(ZDCState& ioState, ZCoord iStartH, ZCoord iStartV,
		ZCoord iEndH, ZCoord iEndV) = 0;

// Rectangle
	virtual void FrameRect(ZDCState& ioState, const ZRect& iRect) = 0;
	virtual void FillRect(ZDCState& ioState, const ZRect& iRect) = 0;
	virtual void InvertRect(ZDCState& ioState, const ZRect& iRect) = 0;
	virtual void HiliteRect(ZDCState& ioState, const ZRect& iRect, const ZRGBColor& iBackColor);

// Region
	virtual void FrameRegion(ZDCState& ioState, const ZDCRgn& iRgn) = 0;
	virtual void FillRegion(ZDCState& ioState, const ZDCRgn& iRgn) = 0;
	virtual void InvertRegion(ZDCState& ioState, const ZDCRgn& iRgn) = 0;
	virtual void HiliteRegion(ZDCState& ioState, const ZDCRgn& iRgn, const ZRGBColor& iBackColor);

// Round cornered rect
	virtual void FrameRoundRect(ZDCState& ioState,
		const ZRect& iRect, const ZPoint& iCornerSize) = 0;

	virtual void FillRoundRect(ZDCState& ioState,
		const ZRect& iRect, const ZPoint& iCornerSize) = 0;

	virtual void InvertRoundRect(ZDCState& ioState,
		const ZRect& iRect, const ZPoint& iCornerSize) = 0;

	virtual void HiliteRoundRect(ZDCState& ioState,
		const ZRect& iRect, const ZPoint& iCornerSize, const ZRGBColor& iBackColor);

// Ellipse
	virtual void FrameEllipse(ZDCState& ioState, const ZRect& iBounds) = 0;
	virtual void FillEllipse(ZDCState& ioState, const ZRect& iBounds) = 0;
	virtual void InvertEllipse(ZDCState& ioState, const ZRect& iBounds) = 0;
	virtual void HiliteEllipse(ZDCState& ioState, const ZRect& iBounds, const ZRGBColor& iBackColor);

// Poly
// See note below regarding framing polygons
	virtual void FillPoly(ZDCState& ioState, const ZDCPoly& iPoly) = 0;
	virtual void InvertPoly(ZDCState& ioState, const ZDCPoly& iPoly) = 0;
	virtual void HilitePoly(ZDCState& ioState, const ZDCPoly& iPoly, const ZRGBColor& iBackColor);

// Pixmap
	virtual void FillPixmap(ZDCState& ioState, const ZPoint& iLocation, const ZDCPixmap& iPixmap);

	virtual void DrawPixmap(ZDCState& ioState,
		const ZPoint& iLocation, const ZDCPixmap& iSourcePixmap, const ZDCPixmap* iMaskPixmap) = 0;

	virtual void DrawPixmap(ZDCState& ioState,
		const ZPoint& iLocation, const ZDCPixmap& iPixmap, bool iUseAlpha);

	virtual ZDCPixmap GetPixmap(ZDCState& ioState, const ZRect& iBounds);

// Flood Fill
	virtual void FloodFill(ZDCState& ioState, const ZPoint& iSeedLocation);

// Moving blocks of pixels
	virtual void Scroll(ZDCState& ioState, const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV) = 0;
	virtual void CopyFrom(ZDCState& ioState, const ZPoint& iLocation,
		ZRef<ZDCCanvas> iSourceCanvas, const ZDCState& iSourceState, const ZRect& iSourceBounds) = 0;

// Text
	virtual void DrawText(ZDCState& ioState, const ZPoint& iLocation,
		const char* iText, size_t iTextLength) = 0;

	virtual ZCoord GetTextWidth(ZDCState& ioState, const char* iText, size_t iTextLength) = 0;

	virtual void GetFontInfo(ZDCState& ioState,
		ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading) = 0;

	virtual ZCoord GetLineHeight(ZDCState& ioState) = 0;

	virtual void BreakLine(ZDCState& theState, const char* iLineStart, size_t iRemainingTextLength,
		ZCoord iTargetWidth, size_t& oNextLineDelta) = 0;

// Escape hatch
	virtual void InvalCache() = 0;

// Syncing
	virtual void Sync();
	virtual void Flush();

	virtual short GetDepth() = 0;
	virtual bool IsOffScreen() = 0;
	virtual bool IsPrinting() = 0;

	virtual ZRef<ZDCCanvas> CreateOffScreen(const ZRect& iCanvasBounds);
	virtual ZRef<ZDCCanvas> CreateOffScreen(ZPoint iDimensions,
		ZDCPixmapNS::EFormatEfficient iFormat) = 0;
	};

// =================================================================================================
// MARK: - ZDCCanvasFactory

class ZDCCanvasFactory
	{
protected:
	ZDCCanvasFactory();
	~ZDCCanvasFactory();

public:
	static ZRef<ZDCCanvas> sCreateCanvas(ZPoint iSize,
		bool iBigEndian, const ZDCPixmapNS::PixelDesc& iPixelDesc);

	virtual ZRef<ZDCCanvas> CreateCanvas(ZPoint iSize,
		bool iBigEndian, const ZDCPixmapNS::PixelDesc& iPixelDesc) = 0;
	
private:
	static ZDCCanvasFactory* spHead;
	ZDCCanvasFactory* fNext;
	};

// =================================================================================================
// MARK: - ZDCScratch

/** ZDCScratch provides access to a scratch ZDC -- primarily used for finding font metrics
when there is no ZDC available or appropriate to use. ZDCScratch::sSet is called by ZOSApp_XX
implementations during startup and shutdown. Application code will generally only need to
call sGet. */

class ZDCScratch
	{
public:
	static const ZDC& sGet();
	static void sSet(ZRef<ZDCCanvas> iCanvas);
	};

// =================================================================================================
// MARK: - ZDC_Off

class ZDC_Off : public ZDC
	{
public:
	ZDC_Off(const ZDC& iOther, bool iForceOffscreen);
	ZDC_Off(const ZDC& iOther, ZPoint iDimensions, ZDCPixmapNS::EFormatEfficient iFormat);
	};

// =================================================================================================
// MARK: - ZDC_OffAuto

class ZDC_OffAuto : public ZDC_Off
	{
public:
	ZDC_OffAuto(const ZDC& iOther, bool iCopyFromOther);
	virtual ~ZDC_OffAuto();
private:
	ZDC fDest;
	};

// =================================================================================================
// MARK: - ZDC Inines

inline const ZDCInk& ZDC::GetInk() const
	{ return fState.fInk; }

inline ZCoord ZDC::GetPenWidth() const
	{ return fState.fPenWidth; }

inline short ZDC::GetMode() const
	{ return fState.fMode; }

inline const ZDCFont& ZDC::GetFont() const
	{ return fState.fFont; }

inline ZPoint ZDC::GetOrigin() const
	{ return fState.fOrigin + fState.fOriginComp; }

inline const ZRGBColor& ZDC::GetTextColor() const
	{ return fState.fTextColor; }

inline ZPoint ZDC::GetPatternOrigin() const
	{ return fState.fPatternOrigin - fState.fOriginComp; }

inline short ZDC::GetDepth() const
	{ return fCanvas->GetDepth(); }

inline bool ZDC::IsOffScreen() const
	{ return fCanvas->IsOffScreen(); }

inline bool ZDC::IsPrinting() const
	{ return fCanvas->IsPrinting(); }

inline void ZDC::Pixel(ZCoord iLocationH, ZCoord iLocationV, const ZRGBColor& iColor) const
	{ fCanvas->Pixel(fState, iLocationH, iLocationV, iColor); }

inline void ZDC::Pixel(const ZPoint& iPoint, const ZRGBColor& iColor) const
	{ fCanvas->Pixel(fState, iPoint.h, iPoint.v, iColor); }

inline void ZDC::Pixels(ZCoord iLocationH, ZCoord iLocationV,
	ZCoord iWidth, ZCoord iHeight,
	const char* iPixvals, const ZRGBColorMap* iColorMap, size_t iColorMapSize) const
	{
	fCanvas->Pixels(fState, iLocationH, iLocationV,
		iWidth, iHeight,
		iPixvals, iColorMap, iColorMapSize);
	}

inline void ZDC::Pixels(ZPoint iLocation, ZPoint iSize,
	const char* iPixvals, const ZRGBColorMap* iColorMap, size_t iColorMapSize) const
	{
	fCanvas->Pixels(fState, iLocation.h, iLocation.v,
		iSize.h, iSize.v,
		iPixvals, iColorMap, iColorMapSize);
	}

inline ZRGBColor ZDC::GetPixel(ZPoint iLocation) const
	{ return fCanvas->GetPixel(fState, iLocation.h, iLocation.v); }

inline ZRGBColor ZDC::GetPixel(ZCoord iLocationH, ZCoord iLocationV) const
	{ return fCanvas->GetPixel(fState, iLocationH, iLocationV); }

inline void ZDC::Line(ZCoord iStartH, ZCoord iStartV, ZCoord iEndH, ZCoord iEndV) const
	{ fCanvas->Line(fState, iStartH, iStartV, iEndH, iEndV); }

inline void ZDC::Line(const ZPoint& iStart, const ZPoint& iEnd) const
	{ fCanvas->Line(fState, iStart.h, iStart.v, iEnd.h, iEnd.v); }

inline void ZDC::Frame(const ZRect& iRect) const
	{ fCanvas->FrameRect(fState, iRect); }

inline void ZDC::Fill(const ZRect& iRect) const
	{ fCanvas->FillRect(fState, iRect); }

inline void ZDC::Invert(const ZRect& iRect) const
	{ fCanvas->InvertRect(fState, iRect); }

inline void ZDC::Hilite(const ZRect& iRect, const ZRGBColor& iBackColor) const
	{ fCanvas->HiliteRect(fState, iRect, iBackColor); }

inline void ZDC::Frame(const ZDCRgn& iRgn) const
	{ fCanvas->FrameRegion(fState, iRgn); }

inline void ZDC::Fill(const ZDCRgn& iRgn) const
	{ fCanvas->FillRegion(fState, iRgn); }

inline void ZDC::Invert(const ZDCRgn& iRgn) const
	{ fCanvas->InvertRegion(fState, iRgn); }

inline void ZDC::Hilite(const ZDCRgn& iRgn, const ZRGBColor& iBackColor) const
	{ fCanvas->HiliteRegion(fState, iRgn, iBackColor); }

inline void ZDC::Frame(const ZRect& iRect, const ZPoint& iCornerSize) const
	{ fCanvas->FrameRoundRect(fState, iRect, iCornerSize); }

inline void ZDC::Fill(const ZRect& iRect, const ZPoint& iCornerSize) const
	{ fCanvas->FillRoundRect(fState, iRect, iCornerSize); }

inline void ZDC::Invert(const ZRect& iRect, const ZPoint& iCornerSize) const
	{ fCanvas->InvertRoundRect(fState, iRect, iCornerSize); }

inline void ZDC::Hilite(const ZRect& iRect,
	const ZPoint& iCornerSize, const ZRGBColor& iBackColor) const
	{ fCanvas->HiliteRoundRect(fState, iRect, iCornerSize, iBackColor); }

inline void ZDC::FrameEllipse(const ZRect& iBounds) const
	{ fCanvas->FrameEllipse(fState, iBounds); }

inline void ZDC::FillEllipse(const ZRect& iBounds) const
	{ fCanvas->FillEllipse(fState, iBounds); }

inline void ZDC::InvertEllipse(const ZRect& iBounds) const
	{ fCanvas->InvertEllipse(fState, iBounds); }

inline void ZDC::HiliteEllipse(const ZRect& iBounds, const ZRGBColor& iBackColor) const
	{ fCanvas->HiliteEllipse(fState, iBounds, iBackColor); }

inline void ZDC::Fill(const ZDCPoly& iPoly) const
	{ fCanvas->FillPoly(fState, iPoly); }

inline void ZDC::Invert(const ZDCPoly& iPoly) const
	{ fCanvas->InvertPoly(fState, iPoly); }

inline void ZDC::Hilite(const ZDCPoly& iPoly, const ZRGBColor& iBackColor) const
	{ fCanvas->HilitePoly(fState, iPoly, iBackColor); }

inline void ZDC::Fill(const ZPoint& iLocation, const ZDCPixmap& iPixmap) const
	{ fCanvas->FillPixmap(fState, iLocation, iPixmap); }

inline void ZDC::Draw(const ZPoint& iLocation, const ZDCPixmap& iPixmap) const
	{ fCanvas->DrawPixmap(fState, iLocation, iPixmap, (const ZDCPixmap*)(0)); }

inline void ZDC::Draw(const ZPoint& iLocation,
	const ZDCPixmap& iPixmap, const ZDCPixmap& iMask) const
	{ fCanvas->DrawPixmap(fState, iLocation, iPixmap, &iMask); }

inline void ZDC::Draw(const ZPoint& iLocation, const ZDCPixmap& iPixmap, bool iUseAlpha) const
	{ fCanvas->DrawPixmap(fState, iLocation, iPixmap, iUseAlpha); }

inline ZDCPixmap ZDC::GetPixmap(const ZRect& iBounds) const
	{ return fCanvas->GetPixmap(fState, iBounds); }

inline void ZDC::FloodFill(const ZPoint& iSeedLocation) const
	{ fCanvas->FloodFill(fState, iSeedLocation); }

inline void ZDC::Scroll(const ZRect& iRect, const ZPoint& iDelta) const
	{ fCanvas->Scroll(fState, iRect, iDelta.h, iDelta.v); }

inline void ZDC::Scroll(const ZRect& iRect, ZCoord iDeltaH, ZCoord iDeltaV) const
	{ fCanvas->Scroll(fState, iRect, iDeltaH, iDeltaV); }

inline void ZDC::CopyFrom(const ZPoint& iLocation,
	const ZDC& iSource, const ZRect& iSourceBounds) const
	{ fCanvas->CopyFrom(fState, iLocation, iSource.GetCanvas(), iSource.fState, iSourceBounds); }

inline void ZDC::Draw(const ZPoint& iLocation, const char* iText, size_t iTextLength) const
	{ fCanvas->DrawText(fState, iLocation, iText, iTextLength); }

inline void ZDC::Draw(const ZPoint& iLocation, const std::string& iString) const
	{
	if (iString.size())
		fCanvas->DrawText(fState, iLocation, iString.data(), iString.size());
	}

inline void ZDC::DrawText(const ZPoint& iLocation, const char* iText, size_t iTextLength) const
	{ fCanvas->DrawText(fState, iLocation, iText, iTextLength); }

inline void ZDC::DrawText(const ZPoint& iLocation, const std::string& iString) const
	{
	if (iString.size())
		fCanvas->DrawText(fState, iLocation, iString.data(), iString.size());
	}

inline ZCoord ZDC::GetTextWidth(const std::string& iString) const
	{ return fCanvas->GetTextWidth(fState, iString.data(), iString.size()); }

inline ZCoord ZDC::GetTextWidth(const char* iText, size_t iTextLength) const
	{ return fCanvas->GetTextWidth(fState, iText, iTextLength); }

inline void ZDC::GetFontInfo(ZCoord& oAscent, ZCoord& oDescent, ZCoord& oLeading) const
	{ fCanvas->GetFontInfo(fState, oAscent, oDescent, oLeading); }

inline ZCoord ZDC::GetLineHeight() const
	{ return fCanvas->GetLineHeight(fState); }

inline void ZDC::BreakLine(const char* iLineStart, size_t iRemainingTextLength,
	ZCoord iTargetWidth, size_t& oNextLineDelta) const
	{ fCanvas->BreakLine(fState, iLineStart, iRemainingTextLength, iTargetWidth, oNextLineDelta); }

inline void ZDC::Sync() const
	{ fCanvas->Sync(); }

inline void ZDC::Flush() const
	{ fCanvas->Flush(); }

inline ZRef<ZDCCanvas> ZDC::GetCanvas() const
	{ return fCanvas; }

inline ZDCState& ZDC::GetState() const
	{ return fState; }

// =================================================================================================

} // namespace ZooLib

#endif // __ZDC_h__
