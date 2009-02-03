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

#include "zoolib/ZStreamRW_MemoryPipe.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy

using std::min;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRW_MemoryPipe

ZStreamRW_MemoryPipe::ZStreamRW_MemoryPipe()
	{
	fWriteClosed = false;

	fSource = nil;
	fSourceEnd = nil;

	fDest = nil;
	fDestCount = 0;
	}

ZStreamRW_MemoryPipe::~ZStreamRW_MemoryPipe()
	{
	fMutex.Acquire();
	ZAssertStop(2, fSource == nil && fDest == nil);
	}

void ZStreamRW_MemoryPipe::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);
	uint8* localEnd = localDest + iCount;

	fMutex.Acquire();
	while (localDest < localEnd)
		{
		if (fSource && fSource < fSourceEnd)
			{
			// We've got a source waiting to give us data.
			size_t countToCopy = min(localEnd - localDest, fSourceEnd - fSource);
			ZBlockCopy(fSource, localDest, countToCopy);
			localDest += countToCopy;
			fSource += countToCopy;
			fCondition_Write.Broadcast();
			}
		else
			{
			// Register ourselves as waiting for data.
			if (fWriteClosed)
				break;
			ZAssertStop(2, fDest == nil && fDestCount == 0);
			fDest = localDest;
			fDestCount = localEnd - localDest;
			fCondition_Write.Broadcast();
			fCondition_Read.Wait(fMutex);
			localDest = fDest;
			fDest = nil;
			fDestCount = 0;
			}

		if (localDest != static_cast<uint8*>(iDest))
			{
			// We were able to get *some* data. Let's give up for now
			// and possibly allow a writer to build up a head of steam, as it were.
			break;
			}
		}
	fMutex.Release();

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t ZStreamRW_MemoryPipe::Imp_CountReadable()
	{
	fMutex.Acquire();
	size_t countReadable = 0;
	if (fSource)
		countReadable = fSourceEnd - fSource;	
	fMutex.Release();
	return countReadable;
	}

void ZStreamRW_MemoryPipe::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRW_MemoryPipe::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten); }

void ZStreamRW_MemoryPipe::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;

	fMutex.Acquire();
	while (countRemaining && !fWriteClosed)
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
			ZAssertStop(2, fDest == nil && fDestCount == 0);
			fDestCount = countRemaining;
			fCondition_Write.Broadcast();
			fCondition_Read.Wait(fMutex);
			countRemaining = fDestCount;
			fDestCount = 0;
			}
		}
	fMutex.Release();

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

void ZStreamRW_MemoryPipe::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localEnd = localSource + iCount;

	fMutex.Acquire();
	while (localSource < localEnd && !fWriteClosed)
		{
		if (fDestCount)
			{
			// A reader is waiting for data, so copy straight
			// from our source into the reader's dest.
			size_t countToCopy = min(fDestCount, size_t(localEnd - localSource));
			if (fDest)
				{
				ZBlockCopy(localSource, fDest, countToCopy);
				fDest += countToCopy;
				}
			localSource += countToCopy;
			fDestCount -= countToCopy;
			fCondition_Read.Broadcast();
			}
		else
			{
			// Register ourselves as having data to provide.
			ZAssertStop(2, fSource == nil && fSourceEnd == nil);
			fSource = localSource;
			fSourceEnd = localEnd;
			fCondition_Read.Broadcast();
			fCondition_Write.Wait(fMutex);
			localSource = fSource;
			fSource = nil;
			fSourceEnd = nil;
			}
		
		if (localSource != static_cast<const uint8*>(iSource))
			{
			// We were able to write *some* data. Let's give up for now
			// and possibly allow a reader to come in.
			break;
			}
		}
	fMutex.Release();

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const uint8*>(iSource);
	}

void ZStreamRW_MemoryPipe::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZStreamRW_MemoryPipe::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ this->pCopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZStreamRW_MemoryPipe::Close()
	{
	fMutex.Acquire();
	if (!fWriteClosed)
		{
		fWriteClosed = true;
		fCondition_Read.Broadcast();
		fCondition_Write.Broadcast();
		}
	fMutex.Release();
	}

void ZStreamRW_MemoryPipe::pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;

	uint64 countRemaining = iCount;
	fMutex.Acquire();
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
	fMutex.Release();
	}

void ZStreamRW_MemoryPipe::pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;

	fMutex.Acquire();
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
			ZAssertStop(2, fSource == nil && fSourceEnd == nil);
			fCondition_Read.Broadcast();
			fCondition_Write.Wait(fMutex);
			}
		}

	fMutex.Release();
	}

NAMESPACE_ZOOLIB_END
