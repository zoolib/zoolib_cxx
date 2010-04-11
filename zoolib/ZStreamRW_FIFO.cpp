/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZStreamRW_FIFO.h"
#include "zoolib/ZUtil_STL.h"

NAMESPACE_ZOOLIB_BEGIN

using std::deque;
using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRW_FIFO

ZStreamRW_FIFO::ZStreamRW_FIFO()
:	fClosed(false),
	fMaxSize(0),
	fUserCount(0)
	{}

ZStreamRW_FIFO::ZStreamRW_FIFO(size_t iMaxSize)
:	fClosed(false),
	fMaxSize(iMaxSize),
	fUserCount(0)
	{}

ZStreamRW_FIFO::~ZStreamRW_FIFO()
	{
	ZGuardMtx locker(fMutex);

	fClosed = true;
	fCondition_Read.Broadcast();
	fCondition_Write.Broadcast();

	while (fUserCount)
		fCondition_UserCount.Wait(fMutex);
	}

void ZStreamRW_FIFO::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	ZGuardMtx locker(fMutex);
	++fUserCount;
	uint8* localDest = static_cast<uint8*>(oDest);
	while (iCount)
		{
		const size_t countToCopy = min(iCount, fBuffer.size());
		if (countToCopy == 0)
			{
			if (fClosed)
				break;
			fCondition_Read.Wait(fMutex);
			}
		else
			{
			copy(fBuffer.begin(), fBuffer.begin() + countToCopy, localDest);
			fBuffer.erase(fBuffer.begin(), fBuffer.begin() + countToCopy);
			localDest += countToCopy;
			iCount -= countToCopy;
			fCondition_Write.Broadcast();
			break;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(oDest);

	--fUserCount;
	fCondition_UserCount.Broadcast();
	}

size_t ZStreamRW_FIFO::Imp_CountReadable()
	{
	ZGuardMtx locker(fMutex);
	return fBuffer.size();
	}

bool ZStreamRW_FIFO::Imp_WaitReadable(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardMtx locker(fMutex);
	for (;;)
		{
		if (fBuffer.size())
			return true;
		if (!fCondition_Read.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZStreamRW_FIFO::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	ZGuardMtx locker(fMutex);
	++fUserCount;
	const uint8* localSource = static_cast<const uint8*>(iSource);
	while (iCount && !fClosed)
		{
		size_t countToInsert = iCount;
		if (fMaxSize)
			{
			if (fMaxSize >= fBuffer.size())
				countToInsert = min(iCount, fMaxSize - fBuffer.size());
			else
				countToInsert = 0;
			}

		if (countToInsert == 0)
			{
			fCondition_Write.Wait(fMutex);
			}
		else
			{
			fBuffer.insert(fBuffer.end(), localSource, localSource + countToInsert);
			localSource += countToInsert;
			iCount -= countToInsert;
			fCondition_Read.Broadcast();
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);

	--fUserCount;
	fCondition_UserCount.Broadcast();
	}

void ZStreamRW_FIFO::Close()
	{
	ZGuardMtx locker(fMutex);
// Hmmm.
//	ZAssert(!fClosed);
	fClosed = true;
	fCondition_Read.Broadcast();
	fCondition_Write.Broadcast();
	}

bool ZStreamRW_FIFO::IsClosed()
	{
	ZGuardMtx locker(fMutex);
	return fClosed;
	}

void ZStreamRW_FIFO::Reset()
	{
	ZGuardMtx locker(fMutex);
	fBuffer.clear();
	fClosed = false;
	fCondition_Read.Broadcast();
	fCondition_Write.Broadcast();
	}

NAMESPACE_ZOOLIB_END
