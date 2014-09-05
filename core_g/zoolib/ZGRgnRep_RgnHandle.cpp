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

//###include "zoolib/ZMacFixup.h"

#include "zoolib/ZGRgnRep_RgnHandle.h"

#if ZCONFIG_API_Enabled(GRgnRep_RgnHandle)

#include "zoolib/ZFunctionChain.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Helper functions

namespace { // anonymous

RgnHandle sCopyRgn(RgnHandle iSource)
	{
	RgnHandle result = ::NewRgn();
	::MacCopyRgn(iSource, result);
	return result;
	}

RgnHandle sNewRectRgn(const ZRectPOD& iRect)
	{
	RgnHandle result = ::NewRgn();
	::MacSetRectRgn(result, iRect.left, iRect.top, iRect.right, iRect.bottom);
	return result;
	}

struct State_t
	{
	RgnHandle fResult;
	RgnHandle fTemp;
	};

bool sDecomposeRepProc(const ZRectPOD& iRect, void* iRefcon)
	{
	// Use ZAccumulator_T at some point.
	State_t* theState = static_cast<State_t*>(iRefcon);
	::MacSetRectRgn(theState->fTemp, iRect.left, iRect.top, iRect.right, iRect.bottom);
	::MacUnionRgn(theState->fTemp, theState->fResult, theState->fResult);

	// Return false to indicate we don't want to abort.
	return false;
	}

RgnHandle sDecomposeRepIntoRgnHandle(const ZRef<ZGRgnRep>& iRep)
	{
	State_t theState;
	theState.fResult = ::NewRgn();
	if (iRep)
		{
		theState.fTemp = ::NewRgn();
		iRep->Decompose(sDecomposeRepProc, &theState);
		::DisposeRgn(theState.fTemp);
		}
	return theState.fResult;
	}

RgnHandle sGetRgnHandle(const ZRef<ZGRgnRep>& iRep, bool& oDispose)
	{
	if (ZRef<ZGRgnRep_RgnHandle> other = iRep.DynamicCast<ZGRgnRep_RgnHandle>())
		{
		oDispose = false;
		return other->GetRgnHandle();
		}

	oDispose = true;

	return sDecomposeRepIntoRgnHandle(iRep);
	}

RgnHandle sMakeRgnHandle(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_RgnHandle> other = iRep.DynamicCast<ZGRgnRep_RgnHandle>())
		return sCopyRgn(other->GetRgnHandle());

	return sDecomposeRepIntoRgnHandle(iRep);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_Rect
:	public ZFunctionChain_T<ZRef<ZGRgnRep>, const ZRectPOD&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		RgnHandle theRgnHandle = ::NewRgn();
		::MacSetRectRgn(theRgnHandle, iParam.left, iParam.top, iParam.right, iParam.bottom);
		oResult = new ZGRgnRep_RgnHandle(theRgnHandle);
		return true;
		}
	} sMaker0;

} // anonymous namespace

template <>
ZRef<ZGRgnRep> ZGRgnRepCreator_T<RgnHandle>::sCreate(RgnHandle iNative, bool iAdopt)
	{ return new ZGRgnRep_RgnHandle(iNative, iAdopt); }

// =================================================================================================
// MARK: - ZGRgnRep_RgnHandle

ZRef<ZGRgnRep_RgnHandle> ZGRgnRep_RgnHandle::sGetRep(const ZRef<ZGRgnRep>& iRep)
	{
	if (ZRef<ZGRgnRep_RgnHandle> other = iRep.DynamicCast<ZGRgnRep_RgnHandle>())
		return other;
	RgnHandle theRgnHandle = sDecomposeRepIntoRgnHandle(iRep);
	return new ZGRgnRep_RgnHandle(theRgnHandle);
	}

ZGRgnRep_RgnHandle::ZGRgnRep_RgnHandle(RgnHandle iRgnHandle, bool iAdopt)
:	fRgnHandle(iAdopt ? iRgnHandle : sCopyRgn(iRgnHandle))
	{}

ZGRgnRep_RgnHandle::ZGRgnRep_RgnHandle(RgnHandle iRgnHandle)
:	fRgnHandle(iRgnHandle)
	{}

ZGRgnRep_RgnHandle::~ZGRgnRep_RgnHandle()
	{ ::DisposeRgn(fRgnHandle); }

namespace { // anonymous

struct DecomposeRgnHandle_t
	{
	ZGRgnRep::DecomposeProc fDecomposeProc;
	void* fRefcon;
	size_t fCallsMade;
	};

OSStatus sDecompose_RgnHandle(UInt16 iMessage, RgnHandle iRgnHandle,
	const Rect* iRect, void* iRefcon)
	{
	if (iMessage == kQDRegionToRectsMsgParse)
		{
		DecomposeRgnHandle_t* theStruct = reinterpret_cast<DecomposeRgnHandle_t*>(iRefcon);
		++theStruct->fCallsMade;
		if (theStruct->fDecomposeProc(
			sRectPOD(iRect->left, iRect->top, iRect->right, iRect->bottom),
			theStruct->fRefcon))
			{
			return userCanceledErr;
			}
		}
	return noErr;
	}

RegionToRectsUPP sDecompose_RgnHandleUPP = NewRegionToRectsUPP(sDecompose_RgnHandle);

} // anonymous namespace

size_t ZGRgnRep_RgnHandle::Decompose(DecomposeProc iProc, void* iRefcon)
	{
	DecomposeRgnHandle_t theStruct;
	theStruct.fDecomposeProc = iProc;
	theStruct.fRefcon = iRefcon;
	theStruct.fCallsMade = 0;
	::QDRegionToRects(fRgnHandle, kQDParseRegionFromTopLeft,
		sDecompose_RgnHandleUPP, &theStruct);
	return theStruct.fCallsMade;
	}

bool ZGRgnRep_RgnHandle::Contains(ZCoord iH, ZCoord iV)
	{
	Point thePoint;
	thePoint.h = iH;
	thePoint.v = iV;
	return ::PtInRgn(thePoint, fRgnHandle);
	}

bool ZGRgnRep_RgnHandle::IsEmpty()
	{ return ::EmptyRgn(fRgnHandle); }

ZRectPOD ZGRgnRep_RgnHandle::Bounds()
	{
	Rect theRect;
	::GetRegionBounds(fRgnHandle, &theRect);
	return sRectPOD(theRect.left, theRect.top, theRect.right, theRect.bottom);
	}

bool ZGRgnRep_RgnHandle::IsSimpleRect()
	{ return ::IsRegionRectangular(fRgnHandle); }

bool ZGRgnRep_RgnHandle::IsEqualTo(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	RgnHandle other = sGetRgnHandle(iRep, dispose);
	bool result = ::MacEqualRgn(fRgnHandle, other);
	if (dispose)
		::DisposeRgn(other);
	return result;
	}

void ZGRgnRep_RgnHandle::Inset(ZCoord iH, ZCoord iV)
	{ ::InsetRgn(fRgnHandle, iH, iV); }

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Insetted(ZCoord iH, ZCoord iV)
	{
	RgnHandle result = sCopyRgn(fRgnHandle);
	::InsetRgn(result, iH, iV);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Offset(ZCoord iH, ZCoord iV)
	{ ::OffsetRgn(fRgnHandle, iH, iV); }

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Offsetted(ZCoord iH, ZCoord iV)
	{
	RgnHandle result = sCopyRgn(fRgnHandle);
	::OffsetRgn(result, iH, iV);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Include(const ZRectPOD& iRect)
	{
	RgnHandle temp = sNewRectRgn(iRect);
	::MacUnionRgn(fRgnHandle, temp, fRgnHandle);
	::DisposeRgn(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Including(const ZRectPOD& iRect)
	{
	RgnHandle result = sNewRectRgn(iRect);
	::MacUnionRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Intersect(const ZRectPOD& iRect)
	{
	RgnHandle temp = sNewRectRgn(iRect);
	::SectRgn(fRgnHandle, temp, fRgnHandle);
	::DisposeRgn(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Intersecting(const ZRectPOD& iRect)
	{
	RgnHandle result = sNewRectRgn(iRect);
	::SectRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Exclude(const ZRectPOD& iRect)
	{
	RgnHandle temp = sNewRectRgn(iRect);
	::DiffRgn(fRgnHandle, temp, fRgnHandle);
	::DisposeRgn(temp);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Excluding(const ZRectPOD& iRect)
	{
	RgnHandle result = sNewRectRgn(iRect);
	::DiffRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Include(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	RgnHandle other = sGetRgnHandle(iRep, dispose);
	::MacUnionRgn(fRgnHandle, other, fRgnHandle);
	if (dispose)
		::DisposeRgn(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Including(const ZRef<ZGRgnRep>& iRep)
	{
	RgnHandle result = sMakeRgnHandle(iRep);
	::MacUnionRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Intersect(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	RgnHandle other = sGetRgnHandle(iRep, dispose);
	::SectRgn(fRgnHandle, other, fRgnHandle);
	if (dispose)
		::DisposeRgn(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Intersecting(const ZRef<ZGRgnRep>& iRep)
	{
	RgnHandle result = sMakeRgnHandle(iRep);
	::SectRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Exclude(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	RgnHandle other = sGetRgnHandle(iRep, dispose);
	::DiffRgn(fRgnHandle, other, fRgnHandle);
	if (dispose)
		::DisposeRgn(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Excluding(const ZRef<ZGRgnRep>& iRep)
	{
	RgnHandle result = sMakeRgnHandle(iRep);
	::DiffRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

void ZGRgnRep_RgnHandle::Xor(const ZRef<ZGRgnRep>& iRep)
	{
	bool dispose;
	RgnHandle other = sGetRgnHandle(iRep, dispose);
	::XorRgn(fRgnHandle, other, fRgnHandle);
	if (dispose)
		::DisposeRgn(other);
	}

ZRef<ZGRgnRep> ZGRgnRep_RgnHandle::Xoring(const ZRef<ZGRgnRep>& iRep)
	{
	RgnHandle result = sMakeRgnHandle(iRep);
	::XorRgn(fRgnHandle, result, result);
	return new ZGRgnRep_RgnHandle(result);
	}

RgnHandle ZGRgnRep_RgnHandle::GetRgnHandle()
	{ return fRgnHandle; }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(GRgnRep_RgnHandle)
