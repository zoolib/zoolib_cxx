// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Pixels_PixelIters_h__
#define __ZooLib_Pixels_PixelIters_h__ 1
#include "zconfig.h"

#include "zoolib/Pixels/PixvalIters.h"
#include "zoolib/Pixels/PixelDesc.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixelIterR_T

template <class P>
class PixelIterR_T
	{
public:
	PixelIterR_T(const PixvalDesc& iPixvalDesc, const void* iAddress, int iCoord,
		const P& iPixelDesc)
	:	fIter(iPixvalDesc, iAddress, iCoord)
	,	fPixelDesc(iPixelDesc)
		{}

	RGBA ReadInc()
		{ return fPixelDesc.AsRGBA(fIter.ReadInc()); }

	RGBA Read()
		{ return fPixelDesc.AsRGBA(fIter.Read()); }

	Comp ReadAlphaInc()
		{ return fPixelDesc.AsAlpha(fIter.ReadInc()); }

	Comp ReadAlpha()
		{ return fPixelDesc.AsAlpha(fIter.Read()); }

	void Reset(const void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterR fIter;
	const P& fPixelDesc;
	};

// =================================================================================================
#pragma mark - PixelIterRW_T

template <class P>
class PixelIterRW_T
	{
public:
	PixelIterRW_T(const PixvalDesc& iPixvalDesc, void* iAddress, int iCoord,
		const P& iPixelDesc)
	:	fIter(iPixvalDesc, iAddress, iCoord)
	,	fPixelDesc(iPixelDesc)
		{}

	RGBA ReadInc()
		{ return fPixelDesc.AsRGBA(fIter.ReadInc()); }

	RGBA Read()
		{ return fPixelDesc.AsRGBA(fIter.Read()); }

	void WriteInc(const RGBA& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const RGBA& iColor)
		{ fIter.Write(fPixelDesc.AsPixval(iColor)); }

	void Inc()
		{ fIter.Inc(); }

	void Reset(void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterRW fIter;
	const P& fPixelDesc;
	};

// =================================================================================================
#pragma mark - PixelIterW_T

template <class P>
class PixelIterW_T
	{
public:
	PixelIterW_T(const PixvalDesc& iPixvalDesc, void* iAddress, int iCoord,
		const P& iPixelDesc)
	:	fIter(iPixvalDesc, iAddress, iCoord)
	,	fPixelDesc(iPixelDesc)
		{}

	void WriteInc(const RGBA& iColor)
		{ fIter.WriteInc(fPixelDesc.AsPixval(iColor)); }

	void Write(const RGBA& iColor)
		{ fIter.Write(fPixelDesc.AsPixval(iColor)); }

	void Inc()
		{ fIter.Inc(); }

	void Reset(void* iAddress, int iCoord)
		{ fIter.Reset(iAddress, iCoord); }

private:
	PixvalIterW fIter;
	const P& fPixelDesc;
	};

} // namespace Pixels
} // namespace ZooLib

#endif // __ZooLib_Pixels_PixvalIters_h__
