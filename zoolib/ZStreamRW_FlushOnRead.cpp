/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStreamRW_FlushOnRead.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRW_FlushOnRead

/**
\class ZStreamRW_FlushOnRead
\ingroup stream
ZStreamRW_FlushOnRead wraps a read stream and a write stream. It tracks the last
operation applied to it, so that when a write is followed by a read Flush is called
on the real write stream first. Why would you need to use this? Imagine that the write
stream is buffering a network endpoint stream. If you were to write to it then read
from the endpoint's read stream there's no guarantee that what you wrote has been seen
by the other end yet, and thus there may be nothing to read and never will be as the
info that would cause the far end to write to you is still sitting in our local write
buffer. By placing a ZStreamRW_FlushOnRead between you and the endpoint's read stream
and the buffered write stream we're able to ensure that anytime we read from it the
write stream will have been flushed. \note Calls that are nominally read calls can
also cause writes to occur. This makes the read and write aspects of this stream
dependent in a way that's not usually the case for ZStreamRW variants. To protected
against weirdness (that took me a couple of hours to track down) we track the last
operation with a thread safe variable, and all write calls are protected by a mutex.
*/

ZStreamRW_FlushOnRead::ZStreamRW_FlushOnRead(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
:	fStreamR(iStreamR),
	fStreamW(iStreamW),
	fLastWasWrite(0)
	{}

ZStreamRW_FlushOnRead::~ZStreamRW_FlushOnRead()
	{}

void ZStreamRW_FlushOnRead::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	fStreamR.Read(oDest, iCount, oCountRead);
	}

size_t ZStreamRW_FlushOnRead::Imp_CountReadable()
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	return fStreamR.CountReadable();
	}

bool ZStreamRW_FlushOnRead::Imp_WaitReadable(double iTimeout)
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	return fStreamR.WaitReadable(iTimeout);
	}

void ZStreamRW_FlushOnRead::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	fStreamR.CopyTo(iStreamW, iCount, oCountRead, oCountWritten);
	}

void ZStreamRW_FlushOnRead::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	fStreamR.CopyTo(iStreamW, iCount, oCountRead, oCountWritten);
	}

void ZStreamRW_FlushOnRead::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (ZThreadSafe_Swap(fLastWasWrite, 0))
		{
		ZGuardMtx locker_Write(fMutex_Write);
		fStreamW.Flush();
		}

	fStreamR.Skip(iCount, oCountSkipped);
	}

void ZStreamRW_FlushOnRead::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZThreadSafe_Set(fLastWasWrite, 1);
	ZGuardMtx locker_Write(fMutex_Write);
	fStreamW.Write(iSource, iCount, oCountWritten);
	}

void ZStreamRW_FlushOnRead::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	ZThreadSafe_Set(fLastWasWrite, 1);
	ZGuardMtx locker_Write(fMutex_Write);
	fStreamW.CopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamRW_FlushOnRead::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	ZThreadSafe_Set(fLastWasWrite, 1);
	ZGuardMtx locker_Write(fMutex_Write);
	fStreamW.CopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
	}

void ZStreamRW_FlushOnRead::Imp_Flush()
	{
	ZThreadSafe_Set(fLastWasWrite, 0);
	ZGuardMtx locker_Write(fMutex_Write);
	fStreamW.Flush();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRW_FlushOnRead

ZStreamerRW_FlushOnRead::ZStreamerRW_FlushOnRead(
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR),
	fStreamerW(iStreamerW),
	fStream(iStreamerR->GetStreamR(), iStreamerW->GetStreamW())
	{}

ZStreamerRW_FlushOnRead::~ZStreamerRW_FlushOnRead()
	{}

const ZStreamR& ZStreamerRW_FlushOnRead::GetStreamR()
	{ return fStream; }

const ZStreamW& ZStreamerRW_FlushOnRead::GetStreamW()
	{ return fStream; }

} // namespace ZooLib
