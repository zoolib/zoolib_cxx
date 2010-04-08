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

#include "zoolib/ZStreamRWCon_MemoryPipe.h"
#include "zoolib/ZMemory.h" // For ZMemCopy
#include "zoolib/ZTime.h"

using std::min;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_MemoryPipe

ZStreamRWCon_MemoryPipe::ZStreamRWCon_MemoryPipe()
	{
	fWriteClosed = false;

	fSource = nullptr;
	fSourceEnd = nullptr;

	fDest = nullptr;
	fDestCount = 0;
	}

ZStreamRWCon_MemoryPipe::~ZStreamRWCon_MemoryPipe()
	{
	fMutex.Acquire();
	ZAssertStop(2, fSource == nullptr && fDest == nullptr);
	}

void ZStreamRWCon_MemoryPipe::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);
	uint8* localEnd = localDest + iCount;

	ZGuardMtx locker(fMutex);
	while (localDest < localEnd)
		{
		if (fSource && fSource < fSourceEnd)
			{
			// We've got a source waiting to give us data.
			size_t countToCopy = min(localEnd - localDest, fSourceEnd - fSource);
			ZMemCopy(localDest, fSource, countToCopy);
			localDest += countToCopy;
			fSource += countToCopy;
			fCondition_Write.Broadcast();
			}
		else
			{
			// Register ourselves as waiting for data.
			if (fWriteClosed)
				break;
			ZAssertStop(2, fDest == nullptr && fDestCount == 0);
			fDest = localDest;
			fDestCount = localEnd - localDest;
			fCondition_Write.Broadcast();
			fCondition_Read.Wait(fMutex);
			localDest = fDest;
			fDest = nullptr;
			fDestCount = 0;
			}

		if (localDest != static_cast<uint8*>(iDest))
			{
			// We were able to get *some* data. Let's give up for now
			// and possibly allow a writer to build up a head of steam, as it were.
			break;
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t ZStreamRWCon_MemoryPipe::Imp_CountReadable()
	{
	ZGuardMtx locker(fMutex);
	if (fSource)
		return fSourceEnd - fSource;	
	return 0;
	}

bool ZStreamRWCon_MemoryPipe::Imp_WaitReadable(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardMtx locker(fMutex);
	for (;;)
		{
		if (fSource && fSource < fSourceEnd || fWriteClosed)
			return true;

		if (!fCondition_Read.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZStreamRWCon_MemoryPipe::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRWCon_MemoryPipe::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRWCon_MemoryPipe::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;

	ZGuardMtx locker(fMutex);
	while (countRemaining)
		{
		if (fSource && fSource < fSourceEnd)
			{
			// We've got a source waiting to give us data.
			uint64 countToCopy = min(countRemaining, uint64(fSourceEnd - fSource));
			countRemaining -= countToCopy;
			fSource += countToCopy;
			fCondition_Write.Broadcast();
			}
		else
			{
			// Register ourselves as waiting for data.
			if (fWriteClosed)
				break;
			ZAssertStop(2, fDest == nullptr && fDestCount == 0);
			fDestCount = countRemaining;
			fCondition_Write.Broadcast();
			fCondition_Read.Wait(fMutex);
			countRemaining = fDestCount;
			fDestCount = 0;
			}
		}

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

bool ZStreamRWCon_MemoryPipe::Imp_ReceiveDisconnect(double iTimeout)
	{
	const ZTime deadline = ZTime::sSystem() + iTimeout;
	ZGuardMtx locker(fMutex);
	for (;;)
		{
		if (fSource && fSource < fSourceEnd)
			{
			fSource = fSourceEnd;
			fCondition_Write.Broadcast();
			}

		if (fWriteClosed)
			return true;

		if (!fCondition_Read.WaitUntil(fMutex, deadline))
			return false;
		}
	}

void ZStreamRWCon_MemoryPipe::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localEnd = localSource + iCount;

	ZGuardMtx locker(fMutex);
	while (localSource < localEnd && !fWriteClosed)
		{
		if (fDestCount)
			{
			// A reader is waiting for data, so copy straight
			// from our source into the reader's dest.
			size_t countToCopy = min(fDestCount, size_t(localEnd - localSource));
			if (fDest)
				{
				ZMemCopy(fDest, localSource, countToCopy);
				fDest += countToCopy;
				}
			localSource += countToCopy;
			fDestCount -= countToCopy;
			fCondition_Read.Broadcast();
			}
		else
			{
			// Register ourselves as having data to provide.
			ZAssertStop(2, fSource == nullptr && fSourceEnd == nullptr);
			fSource = localSource;
			fSourceEnd = localEnd;
			fCondition_Read.Broadcast();
			fCondition_Write.Wait(fMutex);
			localSource = fSource;
			fSource = nullptr;
			fSourceEnd = nullptr;
			}
		
		if (localSource != static_cast<const uint8*>(iSource))
			{
			// We were able to write *some* data. Let's give up for now
			// and possibly allow a reader to come in.
			break;
			}
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);
	}

void ZStreamRWCon_MemoryPipe::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZStreamRWCon_MemoryPipe::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZStreamRWCon_MemoryPipe::Imp_SendDisconnect()
	{
	ZGuardMtx locker(fMutex);
	if (!fWriteClosed)
		{
		fWriteClosed = true;
		fCondition_Read.Broadcast();
		fCondition_Write.Broadcast();
		}
	}

void ZStreamRWCon_MemoryPipe::Imp_Abort()
	{
	ZGuardMtx locker(fMutex);
	if (!fWriteClosed)
		{
		fWriteClosed = true;
		fCondition_Read.Broadcast();
		fCondition_Write.Broadcast();
		}
	}

void ZStreamRWCon_MemoryPipe::pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;

	uint64 countRemaining = iCount;
	ZGuardMtx locker(fMutex);
	while (countRemaining)
		{
		if (fSource && fSource < fSourceEnd)
			{
			// We've got a source waiting to give us data.
			uint64 countToWrite = min(countRemaining, uint64(fSourceEnd - fSource));

			size_t countWritten;
			iStreamW.Write(fSource, countToWrite, &countWritten);

			fSource += countWritten;
			countRemaining -= countWritten;

			if (oCountRead)
				*oCountRead += countWritten;
			if (oCountWritten)
				*oCountWritten += countWritten;

			fCondition_Write.Broadcast();

			if (countWritten == 0)
				break;
			}
		else
			{
			if (fWriteClosed)
				break;
			// Wait till more data shows up.
			fCondition_Write.Broadcast();
			fCondition_Read.Wait(fMutex);
			}
		}
	}

void ZStreamRWCon_MemoryPipe::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;

	ZGuardMtx locker(fMutex);
	uint64 countRemaining = iCount;
	while (countRemaining && !fWriteClosed)
		{
		if (fDestCount)
			{
			// A reader is waiting for data so we can just copy straight
			// from our source into the reader's dest.
			uint64 countRead = 0;
			uint64 countToRead = min(countRemaining, uint64(fDestCount));
			if (fDest)
				{
				size_t reallyRead;
				iStreamR.Read(fDest, countToRead, &reallyRead);
				fDest += reallyRead;
				countRead = reallyRead;
				}
			else
				{
				iStreamR.Skip(countToRead, &countRead);
				}

			countRemaining -= countRead;
			fDestCount -= countRead;

			if (oCountRead)
				*oCountRead += countRead;
			if (oCountWritten)
				*oCountWritten += countRead;

			fCondition_Read.Broadcast();
			if (countRead == 0)
				break;
			}
		else
			{
			// Wait till we get read from.
			ZAssertStop(2, fSource == nullptr && fSourceEnd == nullptr);
			fCondition_Read.Broadcast();
			fCondition_Write.Wait(fMutex);
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWCon_MemoryPipe

ZStreamerRWCon_MemoryPipe::ZStreamerRWCon_MemoryPipe()
	{}

ZStreamerRWCon_MemoryPipe::~ZStreamerRWCon_MemoryPipe()
	{}

const ZStreamRCon& ZStreamerRWCon_MemoryPipe::GetStreamRCon()
	{ return fStream; }

const ZStreamWCon& ZStreamerRWCon_MemoryPipe::GetStreamWCon()
	{ return fStream; }

NAMESPACE_ZOOLIB_END
