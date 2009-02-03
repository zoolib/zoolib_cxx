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

#include "zoolib/ZStreamRWPos_RAM.h"

#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZUtil_STL.h"

#define kDebug_StreamRWPos_RAM 2

using std::min;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_RAM

/**
\class ZStreamRWPos_RAM
\ingroup stream
ZStreamRWPos_RAM maintainins its contents in a list of fixed size chunks. A similar capability
exists with ZStreamRWPos_Handle, but there the backing store is a single block of memory which
must be resized as it is written to, leading to a great deal of copying overhead.
*/

ZStreamRWPos_RAM::ZStreamRWPos_RAM(size_t iChunkSize)
	{
	// Ensure the chunk size is within sensible limits: 32 bytes to 128K
	ZAssertStop(kDebug_StreamRWPos_RAM, iChunkSize >= 32 && iChunkSize < 128 * 1024);
	fChunkSize = iChunkSize;
	fPosition = 0;
	fSize = 0;
	}

ZStreamRWPos_RAM::~ZStreamRWPos_RAM()
	{
	ZUtil_STL::sDeleteAll(fVector_Chunks.begin(), fVector_Chunks.end());
	}

void ZStreamRWPos_RAM::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(iDest);
	while (iCount)
		{
		size_t currentChunkOffset = fPosition % fChunkSize; 
		size_t currentChunkIndex = fPosition / fChunkSize;
		size_t countToMove = min(iCount, fChunkSize - currentChunkOffset);
		countToMove = ZStream::sClampedSize(countToMove, fSize, fPosition);
		if (countToMove == 0)
			break;
		ZBlockCopy(fVector_Chunks[currentChunkIndex] + currentChunkOffset, localDest, countToMove);
		iCount -= countToMove;
		localDest += countToMove;
		fPosition += countToMove;
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(iDest);
	}

size_t ZStreamRWPos_RAM::Imp_CountReadable()
	{ return ZStream::sClampedSize(fSize, fPosition); }

void ZStreamRWPos_RAM::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	size_t countSkipped = ZStream::sClampedSize(iCount, fSize, fPosition);
	fPosition += countSkipped;
	if (oCountSkipped)
		*oCountSkipped = countSkipped;
	}

void ZStreamRWPos_RAM::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const char* localSource = reinterpret_cast<const char*>(iSource);
	while (iCount)
		{
		size_t currentChunkOffset = fPosition % fChunkSize; 
		size_t currentChunkIndex = fPosition / fChunkSize;
		while (currentChunkIndex >= fVector_Chunks.size())
			fVector_Chunks.push_back(new char[fChunkSize]);
		size_t countToMove = min(iCount, fChunkSize - currentChunkOffset);
		ZBlockCopy(localSource,
			fVector_Chunks[currentChunkIndex] + currentChunkOffset,
			countToMove);
		iCount -= countToMove;
		localSource += countToMove;
		fPosition += countToMove;
		if (fSize < fPosition)
			fSize = fPosition;
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const char*>(iSource);
	}

uint64 ZStreamRWPos_RAM::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_RAM::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_RAM::Imp_GetSize()
	{ return fSize; }

void ZStreamRWPos_RAM::Imp_SetSize(uint64 iSize)
	{
	if (fSize != iSize)
		{
		uint64 neededChunkCount = (iSize + fChunkSize - 1) / fChunkSize;

		while (neededChunkCount > fVector_Chunks.size())
			fVector_Chunks.push_back(new char[fChunkSize]);

		while (neededChunkCount < fVector_Chunks.size())
			{
			delete fVector_Chunks.back();
			fVector_Chunks.pop_back();
			}

		fSize = iSize;
		}
	}

NAMESPACE_ZOOLIB_END
