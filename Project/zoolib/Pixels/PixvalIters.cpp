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

#include "zoolib/Pixels/PixvalIters.h"

namespace ZooLib {
namespace Pixels {

// =================================================================================================
#pragma mark - PixvalIterR

PixvalIterR::PixvalIterR(const PixvalDesc& iPixvalDesc, const void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

Pixval PixvalIterR::ReadInc()
	{ return fAccessor.GetPixval(fAddress, fH++); }

Pixval PixvalIterR::Read()
	{ return fAccessor.GetPixval(fAddress, fH); }

void PixvalIterR::Inc()
	{ ++fH; }

void PixvalIterR::Reset(const void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

// =================================================================================================
#pragma mark - PixvalIterRW

PixvalIterRW::PixvalIterRW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

Pixval PixvalIterRW::ReadInc()
	{ return fAccessor.GetPixval(fAddress, fH++); }

Pixval PixvalIterRW::Read()
	{ return fAccessor.GetPixval(fAddress, fH); }

void PixvalIterRW::WriteInc(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH++, iPixval); }

void PixvalIterRW::Write(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH, iPixval); }

void PixvalIterRW::Inc()
	{ ++fH; }

void PixvalIterRW::Reset(void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

// =================================================================================================
#pragma mark - PixvalIterW

PixvalIterW::PixvalIterW(const PixvalDesc& iPixvalDesc, void* iAddress, int iH)
:	fAccessor(iPixvalDesc)
,	fAddress(iAddress)
,	fH(iH)
	{}

void PixvalIterW::WriteInc(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH++, iPixval); }

void PixvalIterW::Write(Pixval iPixval)
	{ fAccessor.SetPixval(fAddress, fH, iPixval); }

void PixvalIterW::Inc()
	{ ++fH; }

void PixvalIterW::Reset(void* iAddress, int iH)
	{
	fAddress = iAddress;
	fH = iH;
	}

} // namespace Pixels
} // namespace ZooLib
