/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZGRgnRep_XRegion.h"

#if ZCONFIG_API_Enabled(GRgnRep_XRegion)

ZMACRO_MSVCStaticLib_cpp(GRgnRep_XRegion)

namespace ZooLib {

// =================================================================================================
// MARK: - Helper functions

namespace { // anonymous

Region sCopyRegion(Region iSource)
	{
	Region result = ::XCreateRegion();
	::XUnionRegion(iSource, result, result);
	return result;
	}

Region sNewRectRegion(const ZRectPOD& iRect)
	{
	Region result = ::XCreateRegion();
	XRectangle tempRect = iRect;
	::XUnionRectWithRegion(&tempRect, result, result);
	return result;
	}

bool sDecomposeRepProc(const ZRectPOD& iRect, void* iRefcon)
	{
	// Use ZAccumulator_T at some point.
	Region theRegion = static_cast<Region>(iRefcon);
	XRectangle tempRect = iRect;
	::XUnionRectWithRegion(&tempRect, theRegion, theRegion);

	// Return false to indicate we don't want to abort.
	return false;
	}

Region sDecomposeRepIntoRegion(const ZRef<ZGRgnRep>& iRep)
	{
	Region theResult = ::XCreateRegion();
	iRep->Decompose(sDecomposeRepProc, theResult);
	return theResult;
	}

Region sGetRegion(const ZRef<ZGRgnRep>& iRep, bool& oDispose)
	{
	if (ZRef<ZGRgnRep_XRegion> other = iRep.DynamicCast<ZGRgnRep_XRegion>())
		{
		oDispose = false;
		return other->GetRegion();
		}

	oDispose = true;

	return sDecomposeRepIntoRegion(iRep);
	}

Region sMakeRegion(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_XRegion> other = iRep.DynamicCast<ZGRgnRep_XRegion>())
		return sCopyRegion(other->GetRegion());

	return sDecomposeRepIntoRegion(iRep);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_Rect
:	public FunctionChain<ZRef<ZGRgnRep>, const ZRectPOD&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		Region theRegion = ::XCreateRegion();
		XRectangle tempRect = iParam;
		::XUnionRectWithRegion(&tempRect, theRegion, theRegion);
		oResult = new ZGRgnRep_XRegion(theRegion);
		return true;
		}
	} sMaker0;

} // anonymous namespace

template <>
ZRef<ZGRgnRep> ZGRgnRepCreator_T<Region>::sCreate(Region iNative, bool iAdopt)
	{ return new ZGRgnRep_XRegion(iNative, iAdopt); }

// =================================================================================================
// MARK: - ZGRgnRep_XRegion

ZRef<ZGRgnRep_XRegion> ZGRgnRep_XRegion::sGetRep(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_XRegion> other = iRep.DynamicCast<ZGRgnRep_XRegion>())
		return other;
	Region theXRegion = sDecomposeRepIntoRegion(iRep);
	return new ZGRgnRep_XRegion(theXRegion);
	}

ZGRgnRep_XRegion::ZGRgnRep_XRegion(Region iRegion, bool iAdopt)
:	fRegion(iAdopt ? iRegion : sCopyRegion(iRegion))
	{}

ZGRgnRep_XRegion::ZGRgnRep_XRegion(Region iRegion)
:	fRegion(iRegion)
	{}

ZGRgnRep_XRegion::~ZGRgnRep_XRegion()
	{ ::XDestroyRegion(fRegion);	}

size_t ZGRgnRep_XRegion::Decompose(DecomposeProc iProc, void* iRefcon)
	{
	// These should generally be correct but the code will break
	// silently if used by an unusal xlib implementation.
	struct _BOX
		{
		short x1;
		short x2;
		short y1;
		short y2;
		};

	struct _XRegion
		{
		long size;
		long numRects;
		_BOX* rects;
		_BOX extents;
		};

	_XRegion* skanky = reinterpret_cast<_XRegion*>(fRegion);
	size_t callbacksMade = 0;
	for (size_t x = 0; x < skanky->numRects; ++x)
		{
		++callbacksMade;
		ZRectPOD theRect(skanky->rects[x].x1, skanky->rects[x].y1,
			skanky->rects[x].x2, skanky->rects[x].y2);
		if (iProc(theRect, iRefcon))
			break;
		}
	return callbacksMade;
	}

bool ZGRgnRep_XRegion::Contains(ZCoord iH, ZCoord iV)
	{ return ::XPointInRegion(fRegion, iH, iV); }

bool ZGRgnRep_XRegion::IsEmpty()
	{ return ::XEmptyRegion(fRegion); }

ZRectPOD ZGRgnRep_XRegion::Bounds()
	{
	XRectangle theBounds;
	::XClipBox(fRegion, &theBounds);
	return theBounds;
	}

bool ZGRgnRep_XRegion::IsSimpleRect()
	{
	// AG 98-08-11. If regions are implemented as a list of rectangles then we could
	// gain access to that list. But there's no guarantee that that's how any particular
	// xlib implementation works. For now we xor our real region with a rectangular
	// region matching our bounds. If the result is empty then our region
	// must match the rectangular region.
	XRectangle theBounds;
	::XClipBox(fRegion, &theBounds);
	Region tempRgn = ::XCreateRegion();
	::XUnionRectWithRegion(&theBounds, tempRgn, tempRgn);
	::XXorRegion(tempRgn, fRegion, tempRgn);
	bool isEmpty = ::XEmptyRegion(tempRgn);
	::XDestroyRegion(tempRgn);
	return isEmpty;
	}

bool ZGRgnRep_XRegion::IsEqualTo(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	Region other = sGetRegion(iRep, dispose);
	bool result = ::XEqualRegion(fRegion, other);
	if (dispose)
		::XDestroyRegion(other);
	return result;
	}

void ZGRgnRep_XRegion::Inset(ZCoord iH, ZCoord iV)
	{ ::XShrinkRegion(fRegion, iH, iV); }

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Insetted(ZCoord iH, ZCoord iV)
	{
	Region result = sCopyRegion(fRegion);
	::XShrinkRegion(result, iH, iV);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Offset(ZCoord iH, ZCoord iV)
	{ ::XOffsetRegion(fRegion, iH, iV); }

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Offsetted(ZCoord iH, ZCoord iV)
	{
	Region result = sCopyRegion(fRegion);
	::XOffsetRegion(result, iH, iV);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Include(const ZRectPOD& iRect)
	{
	XRectangle temp = iRect;
	::XUnionRectWithRegion(&temp, fRegion, fRegion);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Including(const ZRectPOD& iRect)
	{
	Region result = sCopyRegion(fRegion);
	XRectangle temp = iRect;
	::XUnionRectWithRegion(&temp, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Intersect(const ZRectPOD& iRect)
	{
	Region temp = sNewRectRegion(iRect);
	::XIntersectRegion(fRegion, temp, fRegion);
	::XDestroyRegion(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Intersecting(const ZRectPOD& iRect)
	{
	Region result = sNewRectRegion(iRect);
	::XIntersectRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Exclude(const ZRectPOD& iRect)
	{
	Region temp = sNewRectRegion(iRect);
	::XSubtractRegion(fRegion, temp, fRegion);
	::XDestroyRegion(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Excluding(const ZRectPOD& iRect)
	{
	Region result = sNewRectRegion(iRect);
	::XSubtractRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Include(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	Region other = sGetRegion(iRep, dispose);
	::XUnionRegion(fRegion, other, fRegion);
	if (dispose)
		::XDestroyRegion(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Including(const ZRef<ZGRgnRep>& iRep)
	{
	Region result = sMakeRegion(iRep);
	::XUnionRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Intersect(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	Region other = sGetRegion(iRep, dispose);
	::XIntersectRegion(fRegion, other, fRegion);
	if (dispose)
		::XDestroyRegion(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Intersecting(const ZRef<ZGRgnRep>& iRep)
	{
	Region result = sMakeRegion(iRep);
	::XIntersectRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Exclude(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	Region other = sGetRegion(iRep, dispose);
	::XSubtractRegion(fRegion, other, fRegion);
	if (dispose)
		::XDestroyRegion(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Excluding(const ZRef<ZGRgnRep>& iRep)
	{
	Region result = sMakeRegion(iRep);
	::XSubtractRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

void ZGRgnRep_XRegion::Xor(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	Region other = sGetRegion(iRep, dispose);
	::XXorRegion(fRegion, other, fRegion);
	if (dispose)
		::XDestroyRegion(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_XRegion::Xoring(const ZRef<ZGRgnRep>& iRep)
	{
	Region result = sMakeRegion(iRep);
	::XXorRegion(fRegion, result, result);
	return new ZGRgnRep_XRegion(result);
	}

Region ZGRgnRep_XRegion::GetRegion()
	{ return fRegion; }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(GRgnRep_XRegion)
