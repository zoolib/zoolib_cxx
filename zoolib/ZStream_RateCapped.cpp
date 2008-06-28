/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_RateCapped.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib

namespace ZooLib {

RateLimiter::RateLimiter(double iRate, size_t iQuantum)
:	fRate(iRate),
	fQuantum(iQuantum),
	fLastTime(ZTime(0))
	{}

size_t RateLimiter::GetCount(size_t iLastCount, size_t iCount)
	{
	ZTime now = ZTime::sSystem();
	if (now <= fLastTime)
		return 0;

	const double lastConsumed = fRate * (now - fLastTime);
	if (iLastCount > lastConsumed)
		{
		const double remaining = double(iLastCount) - lastConsumed;
		ZThread::sSleep(int32(1000 * (remaining / fRate)));
		}

	fLastTime = ZTime::sSystem();
	return std::min(iCount, fQuantum);
	}

} // namespace ZooLib

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_RateCapped

ZStreamR_RateCapped::ZStreamR_RateCapped(
	double iRate, size_t iQuantum, const ZStreamR& iStreamReal)
:	ZStreamR_Filter(iStreamReal),
	fLimiter(iRate, iQuantum),
	fLastCount(0)
	{}

ZStreamR_RateCapped::~ZStreamR_RateCapped()
	{}

void ZStreamR_RateCapped::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t realCount = fLimiter.GetCount(fLastCount, iCount);
	ZStreamR_Filter::Imp_Read(iDest, realCount, &fLastCount);
	if (oCountRead)
		*oCountRead = fLastCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_RateCapped

ZStreamerR_RateCapped::ZStreamerR_RateCapped(
	double iRate, size_t iQuantum, ZRef<ZStreamerR> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iRate, iQuantum, iStreamerReal->GetStreamR())
	{}

ZStreamerR_RateCapped::~ZStreamerR_RateCapped()
	{}

const ZStreamR& ZStreamerR_RateCapped::GetStreamR()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamU_RateCapped

ZStreamU_RateCapped::ZStreamU_RateCapped(
	double iRate, size_t iQuantum, const ZStreamU& iStreamReal)
:	ZStreamU_Filter(iStreamReal),
	fLimiter(iRate, iQuantum),
	fLastCount(0)
	{}

ZStreamU_RateCapped::~ZStreamU_RateCapped()
	{}

void ZStreamU_RateCapped::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t realCount = fLimiter.GetCount(fLastCount, iCount);
	ZStreamU_Filter::Imp_Read(iDest, realCount, &fLastCount);
	if (oCountRead)
		*oCountRead = fLastCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerU_RateCapped

ZStreamerU_RateCapped::ZStreamerU_RateCapped(
	double iRate, size_t iQuantum, ZRef<ZStreamerU> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iRate, iQuantum, iStreamerReal->GetStreamU())
	{}

ZStreamerU_RateCapped::~ZStreamerU_RateCapped()
	{}

const ZStreamU& ZStreamerU_RateCapped::GetStreamU()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_RateCapped

ZStreamRPos_RateCapped::ZStreamRPos_RateCapped(
	double iRate, size_t iQuantum, const ZStreamRPos& iStreamReal)
:	ZStreamRPos_Filter(iStreamReal),
	fLimiter(iRate, iQuantum),
	fLastCount(0)
	{}

ZStreamRPos_RateCapped::~ZStreamRPos_RateCapped()
	{}

void ZStreamRPos_RateCapped::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t realCount = fLimiter.GetCount(fLastCount, iCount);
	ZStreamRPos_Filter::Imp_Read(iDest, realCount, &fLastCount);
	if (oCountRead)
		*oCountRead = fLastCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_RateCapped

ZStreamerRPos_RateCapped::ZStreamerRPos_RateCapped(
	double iRate, size_t iQuantum, ZRef<ZStreamerRPos> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iRate, iQuantum, iStreamerReal->GetStreamRPos())
	{}

ZStreamerRPos_RateCapped::~ZStreamerRPos_RateCapped()
	{}

const ZStreamRPos& ZStreamerRPos_RateCapped::GetStreamRPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_RateCapped

ZStreamW_RateCapped::ZStreamW_RateCapped(
	double iRate, size_t iQuantum, const ZStreamW& iStreamReal)
:	ZStreamW_Filter(iStreamReal),
	fLimiter(iRate, iQuantum),
	fLastCount(0)
	{}

ZStreamW_RateCapped::~ZStreamW_RateCapped()
	{}

void ZStreamW_RateCapped::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t realCount = fLimiter.GetCount(fLastCount, iCount);
	ZStreamW_Filter::Imp_Write(iSource, realCount, &fLastCount);
	if (oCountWritten)
		*oCountWritten = fLastCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_RateCapped

ZStreamerW_RateCapped::ZStreamerW_RateCapped(
	double iRate, size_t iQuantum, ZRef<ZStreamerW> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iRate, iQuantum, iStreamerReal->GetStreamW())
	{}

ZStreamerW_RateCapped::~ZStreamerW_RateCapped()
	{}

const ZStreamW& ZStreamerW_RateCapped::GetStreamW()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWPos_RateCapped

ZStreamWPos_RateCapped::ZStreamWPos_RateCapped(
	double iRate, size_t iQuantum, const ZStreamWPos& iStreamReal)
:	ZStreamWPos_Filter(iStreamReal),
	fLimiter(iRate, iQuantum),
	fLastCount(0)
	{}

ZStreamWPos_RateCapped::~ZStreamWPos_RateCapped()
	{}

void ZStreamWPos_RateCapped::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t realCount = fLimiter.GetCount(fLastCount, iCount);
	ZStreamWPos_Filter::Imp_Write(iSource, realCount, &fLastCount);
	if (oCountWritten)
		*oCountWritten = fLastCount;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerWPos_RateCapped

ZStreamerWPos_RateCapped::ZStreamerWPos_RateCapped(
	double iRate, size_t iQuantum, ZRef<ZStreamerWPos> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iRate, iQuantum, iStreamerReal->GetStreamWPos())
	{}

ZStreamerWPos_RateCapped::~ZStreamerWPos_RateCapped()
	{}

const ZStreamWPos& ZStreamerWPos_RateCapped::GetStreamWPos()
	{ return fStream; }
