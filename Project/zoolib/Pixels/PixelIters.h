/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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
