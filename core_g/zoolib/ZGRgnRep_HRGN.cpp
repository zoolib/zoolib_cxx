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

#include "zoolib/ZGRgnRep_HRGN.h"

#if ZCONFIG_API_Enabled(GRgnRep_HRGN)

ZMACRO_MSVCStaticLib_cpp(GRgnRep_HRGN)

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - Helper functions

namespace { // anonymous

HRGN sCopyHRGN(HRGN iSource)
	{
	HRGN result = ::CreateRectRgn(0,0,0,0);
	::CombineRgn(result, iSource, nullptr, RGN_COPY);
	return result;
	}

HRGN sNewRectHRGN(const ZRectPOD& iRect)
	{ return ::CreateRectRgn(iRect.left, iRect.top, iRect.right, iRect.bottom); }

struct State_t
	{
	HRGN fResult;
	HRGN fTemp;
	};

bool sDecomposeRepProc(const ZRectPOD& iRect, void* iRefcon)
	{
	// Use ZAccumulator_T at some point.
	State_t* theState = static_cast<State_t*>(iRefcon);
	::SetRectRgn(theState->fTemp, iRect.left, iRect.top, iRect.right, iRect.bottom);
	::CombineRgn(theState->fResult, theState->fResult, theState->fTemp, RGN_OR);

	// Return false to indicate we don't want to abort.
	return false;
	}

HRGN sDecomposeRepIntoHRGN(const ZRef<ZGRgnRep>& iRep)
	{
	State_t theState;
	theState.fResult = ::CreateRectRgn(0,0,0,0);
	theState.fTemp = ::CreateRectRgn(0,0,0,0);
	iRep->Decompose(sDecomposeRepProc, &theState);
	::DeleteObject(theState.fTemp);
	return theState.fResult;
	}

HRGN sGetHRGN(const ZRef<ZGRgnRep>& iRep, bool& oDispose)
	{
	if (ZRef<ZGRgnRep_HRGN> other = iRep.DynamicCast<ZGRgnRep_HRGN>())
		{
		oDispose = false;
		return other->GetHRGN();
		}

	oDispose = true;

	return sDecomposeRepIntoHRGN(iRep);
	}

HRGN sMakeHRGN(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_HRGN> other = iRep.DynamicCast<ZGRgnRep_HRGN>())
		return sCopyHRGN(other->GetHRGN());

	return sDecomposeRepIntoHRGN(iRep);
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
		HRGN theHRGN = ::CreateRectRgn(iParam.left, iParam.top, iParam.right, iParam.bottom);
		oResult = new ZGRgnRep_HRGN(theHRGN);
		return true;
		}
	} sMaker0;

} // anonymous namespace

template <>
ZRef<ZGRgnRep> ZGRgnRepCreator_T<HRGN>::sCreate(HRGN iNative, bool iAdopt)
	{ return new ZGRgnRep_HRGN(iNative, iAdopt); }

// =================================================================================================
// MARK: - ZGRgnRep_HRGN

ZRef<ZGRgnRep_HRGN> ZGRgnRep_HRGN::sGetRep(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_HRGN> other = iRep.DynamicCast<ZGRgnRep_HRGN>())
		return other;
	HRGN theHRGN = sDecomposeRepIntoHRGN(iRep);
	return new ZGRgnRep_HRGN(theHRGN);
	}

ZGRgnRep_HRGN::ZGRgnRep_HRGN(HRGN iHRGN, bool iAdopt)
:	fHRGN(iAdopt ? iHRGN : sCopyHRGN(iHRGN))
	{}

ZGRgnRep_HRGN::ZGRgnRep_HRGN(HRGN iHRGN)
:	fHRGN(iHRGN)
	{}

ZGRgnRep_HRGN::~ZGRgnRep_HRGN()
	{ ::DeleteObject(fHRGN); }

size_t ZGRgnRep_HRGN::Decompose(DecomposeProc iProc, void* iRefcon)
	{
	RECT bounds;
	int regionType = ::GetRgnBox(fHRGN, &bounds);
	if (regionType == NULLREGION)
		return 0 ;
	if (regionType == SIMPLEREGION)
		{
		iProc(sRectPOD(bounds.left, bounds.top, bounds.right, bounds.bottom), iRefcon);
		return 1;
		}
	if (regionType == COMPLEXREGION)
		{
		size_t regionLength = ::GetRegionData(fHRGN, 0, nullptr);
		std::vector<char> rawStorage(regionLength);
		RGNDATA* regionData = reinterpret_cast<RGNDATA*>(&rawStorage[0]);
		::GetRegionData(fHRGN, regionLength, regionData);
		RECT* rectArray = (RECT*)(regionData->Buffer);
		size_t callbacksMade = 0;
		for (size_t x = 0; x < regionData->rdh.nCount; ++x)
			{
			++callbacksMade;
			if (iProc(sRectPOD(rectArray[x].left, rectArray[x].top, rectArray[x].right, rectArray[x].bottom), iRefcon))
				break;
			}
		return callbacksMade;
		}
	ZDebugStopf(1, ("ZGRgnRep_HRGN::Decompose, unknown region type"));
	return 0;
	}

bool ZGRgnRep_HRGN::Contains(ZCoord iH, ZCoord iV)
	{ return ::PtInRegion(fHRGN, iH, iV); }

bool ZGRgnRep_HRGN::IsEmpty()
	{
	RECT dummyRect;
	return NULLREGION == ::GetRgnBox(fHRGN, &dummyRect);
	}

ZRectPOD ZGRgnRep_HRGN::Bounds()
	{
	RECT theRECT;
	::GetRgnBox(fHRGN, &theRECT);
	return sRectPOD(theRECT.left, theRECT.top, theRECT.right, theRECT.bottom);
	}

bool ZGRgnRep_HRGN::IsSimpleRect()
	{
	RECT dummyRect;
	return COMPLEXREGION != ::GetRgnBox(fHRGN, &dummyRect);
	}

bool ZGRgnRep_HRGN::IsEqualTo(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	HRGN other = sGetHRGN(iRep, dispose);
	bool result = ::EqualRgn(fHRGN, other);
	if (dispose)
		::DeleteObject(other);
	return result;
	}

namespace { // anonymous

void sCompressHRGN(HRGN ioTarget, HRGN ioTemp1, HRGN ioTemp2,
	ZCoord iDelta, bool iHorizontal, bool iGrow)
	{
	::CombineRgn(ioTemp1, ioTarget, nullptr, RGN_COPY);

	const int combineOperation = iGrow ? RGN_OR : RGN_AND;
	ZCoord shift = 1;
	while (iDelta)
		{
		if (iDelta & shift)
			{
			if (iHorizontal)
				::OffsetRgn(ioTarget, -shift, 0);
			else
				::OffsetRgn(ioTarget, 0, -shift);
			::CombineRgn(ioTarget, ioTarget, ioTemp1, combineOperation);
			iDelta -= shift;
			if (iDelta == 0)
				break;
			}
		::CombineRgn(ioTemp2, ioTemp1, nullptr, RGN_COPY);
		if (iHorizontal)
			::OffsetRgn(ioTemp1, -shift, 0);
		else
			::OffsetRgn(ioTemp1, 0, -shift);
		::CombineRgn(ioTemp1, ioTemp1, ioTemp2, combineOperation);
		shift <<= 1;
		}
	}

void sInsetHRGN(HRGN ioHRGN, ZCoord iH, ZCoord iV)
	{
	HRGN temp1 = ::CreateRectRgn(0, 0, 0, 0);
	HRGN temp2 = ::CreateRectRgn(0, 0, 0, 0);

	bool grow = iH < 0;
	if (grow)
		iH = -iH;
	sCompressHRGN(ioHRGN, temp1, temp2, 2 * iH, true, grow);

	grow = iV < 0;
	if (grow)
		iV = -iV;
	sCompressHRGN(ioHRGN, temp1, temp2, 2 * iV, false, grow);

	::OffsetRgn(ioHRGN, iH, iV);

	::DeleteObject(temp1);
	::DeleteObject(temp2);
	}

} // anonymous namespace

void ZGRgnRep_HRGN::Inset(ZCoord iH, ZCoord iV)
	{
	RECT theBounds;
	int regionType = ::GetRgnBox(fHRGN, &theBounds);
	if (regionType == SIMPLEREGION)
		{
		::SetRectRgn(fHRGN, theBounds.left + iH, theBounds.top + iV,
			theBounds.right - iH, theBounds.bottom - iV);
		}
	else
		{
		sInsetHRGN(fHRGN, iH, iV);
		}
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Insetted(ZCoord iH, ZCoord iV)
	{
	RECT theBounds;
	int regionType = ::GetRgnBox(fHRGN, &theBounds);
	if (regionType == SIMPLEREGION)
		{
		HRGN theHRGN = ::CreateRectRgn(theBounds.left + iH, theBounds.top + iV,
			theBounds.right - iH, theBounds.bottom - iV);
		return new ZGRgnRep_HRGN(theHRGN);
		}
	else
		{
		HRGN result = sCopyHRGN(fHRGN);
		sInsetHRGN(result, iH, iV);
		return new ZGRgnRep_HRGN(result);
		}
	}

void ZGRgnRep_HRGN::Offset(ZCoord iH, ZCoord iV)
	{ ::OffsetRgn(fHRGN, iH, iV); }

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Offsetted(ZCoord iH, ZCoord iV)
	{
	HRGN result = sCopyHRGN(fHRGN);
	::OffsetRgn(result, iH, iV);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Include(const ZRectPOD& iRect)
	{
	HRGN temp = sNewRectHRGN(iRect);
	::CombineRgn(fHRGN, fHRGN, temp, RGN_OR);
	::DeleteObject(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Including(const ZRectPOD& iRect)
	{
	HRGN result = sNewRectHRGN(iRect);
	::CombineRgn(result, fHRGN, result, RGN_OR);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Intersect(const ZRectPOD& iRect)
	{
	HRGN temp = sNewRectHRGN(iRect);
	::CombineRgn(fHRGN, fHRGN, temp, RGN_AND);
	::DeleteObject(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Intersecting(const ZRectPOD& iRect)
	{
	HRGN result = sNewRectHRGN(iRect);
	::CombineRgn(result, fHRGN, result, RGN_AND);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Exclude(const ZRectPOD& iRect)
	{
	HRGN temp = sNewRectHRGN(iRect);
	::CombineRgn(fHRGN, fHRGN, temp, RGN_DIFF);
	::DeleteObject(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Excluding(const ZRectPOD& iRect)
	{
	HRGN result = sNewRectHRGN(iRect);
	::CombineRgn(result, fHRGN, result, RGN_DIFF);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Include(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	HRGN other = sGetHRGN(iRep, dispose);
	::CombineRgn(fHRGN, fHRGN, other, RGN_OR);
	if (dispose)
		::DeleteObject(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Including(const ZRef<ZGRgnRep>& iRep)
	{
	HRGN result = sMakeHRGN(iRep);
	::CombineRgn(result, fHRGN, result, RGN_OR);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Intersect(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	HRGN other = sGetHRGN(iRep, dispose);
	::CombineRgn(fHRGN, fHRGN, other, RGN_AND);
	if (dispose)
		::DeleteObject(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Intersecting(const ZRef<ZGRgnRep>& iRep)
	{
	HRGN result = sMakeHRGN(iRep);
	::CombineRgn(result, fHRGN, result, RGN_AND);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Exclude(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	HRGN other = sGetHRGN(iRep, dispose);
	::CombineRgn(fHRGN, fHRGN, other, RGN_DIFF);
	if (dispose)
		::DeleteObject(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Excluding(const ZRef<ZGRgnRep>& iRep)
	{
	HRGN result = sMakeHRGN(iRep);
	::CombineRgn(result, fHRGN, result, RGN_DIFF);
	return new ZGRgnRep_HRGN(result);
	}

void ZGRgnRep_HRGN::Xor(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	HRGN other = sGetHRGN(iRep, dispose);
	::CombineRgn(fHRGN, fHRGN, other, RGN_XOR);
	if (dispose)
		::DeleteObject(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_HRGN::Xoring(const ZRef<ZGRgnRep>& iRep)
	{
	HRGN result = sMakeHRGN(iRep);
	::CombineRgn(result, fHRGN, result, RGN_XOR);
	return new ZGRgnRep_HRGN(result);
	}

HRGN ZGRgnRep_HRGN::GetHRGN()
	{ return fHRGN; }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(GRgnRep_HRGN)
