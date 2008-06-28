/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_PageBuffered.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZMemory.h"

using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_PageBuffered

struct ZStreamRPos_PageBuffered::Buffer
	{
	Buffer* fPrev;
	Buffer* fNext;
	char* fData;
	uint64 fStartPosition;
	};

ZStreamRPos_PageBuffered::ZStreamRPos_PageBuffered(
	size_t iBufferCount, size_t iBufferSize, const ZStreamRPos& iStreamReal)
:	fStreamReal(iStreamReal)
	{
	fBuffer_Head = nil;
	fBuffer_Tail = nil;

	fBufferSize = iBufferSize;

	fPosition = fStreamReal.GetPosition();

	// Allocate all the buffers now, so we don't have to do it dynamically
	try
		{
		while (iBufferCount--)
			{
			Buffer* aBuffer = new Buffer;
			aBuffer->fData = nil;
			// By marking the start position as covering the last fBufferSize
			// bytes of the 64 bit address space we don't need to carry
			// a loaded flag. The weakness is if the user needs to read
			// that part of the address space before all the buffers have
			// been used for other portions. This is unlikely, but possible.
			aBuffer->fStartPosition = 0 - fBufferSize;
			aBuffer->fPrev = nil;
			aBuffer->fNext = fBuffer_Head;
			if (fBuffer_Head)
				{
				fBuffer_Head->fPrev = aBuffer;
				fBuffer_Head = aBuffer;
				}
			else
				{
				fBuffer_Head = aBuffer;
				fBuffer_Tail = aBuffer;
				}
			aBuffer->fData = new char[fBufferSize];
			}
		}
	catch (...)
		{
		Buffer* buffer_Current = fBuffer_Head;
		while (buffer_Current)
			{
			Buffer* nextBuffer = buffer_Current->fNext;
			delete[] buffer_Current->fData;
			delete buffer_Current;
			buffer_Current = nextBuffer;
			}
		throw;
		}
	}

ZStreamRPos_PageBuffered::~ZStreamRPos_PageBuffered()
	{
	Buffer* buffer_Current = fBuffer_Head;
	while (buffer_Current)
		{
		Buffer* nextBuffer = buffer_Current->fNext;
		delete[] buffer_Current->fData;
		delete buffer_Current;
		buffer_Current = nextBuffer;
		}
	}

void ZStreamRPos_PageBuffered::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(iDest);
	iCount = min(uint64(iCount), sDiffPosR(fStreamReal.GetSize(), fPosition));
	while (iCount)
		{
		// Do we have the offset in our buffers?
		Buffer* buffer_Found = fBuffer_Head;
		while (buffer_Found)
			{
			if (buffer_Found->fStartPosition <= fPosition
				&& buffer_Found->fStartPosition + fBufferSize > fPosition)
				{
				break;
				}
			buffer_Found = buffer_Found->fNext;
			}

		if (buffer_Found == nil)
			{
			buffer_Found = fBuffer_Tail;
			buffer_Found->fStartPosition = fPosition - (fPosition % fBufferSize);
			fStreamReal.SetPosition(buffer_Found->fStartPosition);
			fStreamReal.Read(buffer_Found->fData, fBufferSize, nil);
			}

		size_t offsetInBuffer = fPosition % fBufferSize;
		size_t copySize = min(iCount, fBufferSize - offsetInBuffer);
		ZBlockCopy(buffer_Found->fData + offsetInBuffer, localDest, copySize);
		iCount -= copySize;
		fPosition += copySize;
		localDest += copySize;

		if (fBuffer_Head != buffer_Found)
			{
			// Move buffer_Found to the head of the list by removing it
			if (buffer_Found->fPrev)
				buffer_Found->fPrev->fNext = buffer_Found->fNext;
			if (buffer_Found->fNext)
				buffer_Found->fNext->fPrev = buffer_Found->fPrev;
			if (fBuffer_Head == buffer_Found)
				fBuffer_Head = buffer_Found->fNext;
			if (fBuffer_Tail == buffer_Found)
				fBuffer_Tail = buffer_Found->fPrev;

			// and inserting it
			buffer_Found->fPrev = nil;
			buffer_Found->fNext = fBuffer_Head;
			fBuffer_Head->fPrev = buffer_Found;
			fBuffer_Head = buffer_Found;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(iDest);
	}

size_t ZStreamRPos_PageBuffered::Imp_CountReadable()
	{ return fStreamReal.GetSize() - fPosition; }

void ZStreamRPos_PageBuffered::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 realSkip = min(iCount, sDiffPosR(fStreamReal.GetSize(), fPosition));
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_PageBuffered::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_PageBuffered::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_PageBuffered::Imp_GetSize()
	{ return fStreamReal.GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_PageBuffered

ZStreamerRPos_PageBuffered::ZStreamerRPos_PageBuffered(
	size_t iBufferCount, size_t iBufferSize, ZRef<ZStreamerRPos> iStreamerSource)
:	fStreamerSource(iStreamerSource),
	fStream(iBufferCount, iBufferSize, iStreamerSource->GetStreamRPos())
	{}

ZStreamerRPos_PageBuffered::~ZStreamerRPos_PageBuffered()
	{}

const ZStreamRPos& ZStreamerRPos_PageBuffered::GetStreamRPos()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_PageBuffered

struct ZStreamRWPos_PageBuffered::Buffer
	{
	Buffer* fPrev;
	Buffer* fNext;
	char* fData;
	uint64 fStartPosition;
	bool fDirty;
	};

ZStreamRWPos_PageBuffered::ZStreamRWPos_PageBuffered(
	size_t iBufferCount, size_t iBufferSize, const ZStreamRWPos& iStreamReal)
:	fStreamReal(iStreamReal)
	{
	fBuffer_Head = nil;
	fBuffer_Tail = nil;

	fBufferSize = iBufferSize;

	fPosition = fStreamReal.GetPosition();

	// Allocate all the buffers now, so we don't have to do it dynamically
	try
		{
		while (iBufferCount--)
			{
			Buffer* aBuffer = new Buffer;
			aBuffer->fData = nil;
			aBuffer->fDirty = false;
			// By marking the start position as covering the last fBufferSize
			// bytes of the 64 bit address space we don't need to carry
			// a loaded flag. The weakness is if the user needs to read
			// that part of the address space before all the buffers have
			// been used for other portions. This is basically impossible,
			// 2^64 bytes is 16 exabytes.
			aBuffer->fStartPosition = 0 - fBufferSize;
			aBuffer->fPrev = nil;
			aBuffer->fNext = fBuffer_Head;
			if (fBuffer_Head)
				{
				fBuffer_Head->fPrev = aBuffer;
				fBuffer_Head = aBuffer;
				}
			else
				{
				fBuffer_Head = aBuffer;
				fBuffer_Tail = aBuffer;
				}
			aBuffer->fData = new char[fBufferSize];
			}
		}
	catch (...)
		{
		Buffer* buffer_Current = fBuffer_Head;
		while (buffer_Current)
			{
			Buffer* nextBuffer = buffer_Current->fNext;
			delete[] buffer_Current->fData;
			delete buffer_Current;
			buffer_Current = nextBuffer;
			}
		throw;
		}
	}

ZStreamRWPos_PageBuffered::~ZStreamRWPos_PageBuffered()
	{
	// We don't flush our underlying stream -- that might cause performance problems
	// depending on its implementation. Let it decide if it needs to flush. Of course
	// we do copy back any dirty buffers we might have.
	for (Buffer* buffer_Current = fBuffer_Head; buffer_Current; /*no inc*/)
		{
		if (buffer_Current->fDirty)
			{
			fStreamReal.SetPosition(buffer_Current->fStartPosition);

			size_t flushSize = min(uint64(fBufferSize),
				fStreamReal.GetSize() - buffer_Current->fStartPosition);

			fStreamReal.Write(buffer_Current->fData, flushSize, nil);
			}
		Buffer* nextBuffer = buffer_Current->fNext;
		delete[] buffer_Current->fData;
		delete buffer_Current;
		buffer_Current = nextBuffer;
		}
	}

void ZStreamRWPos_PageBuffered::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = reinterpret_cast<char*>(iDest);
	uint64 streamSize = fStreamReal.GetSize();
	iCount = min(uint64(iCount), sDiffPosW(streamSize, fPosition));
	while (iCount)
		{
		// Do we have the offset in our buffers?
		Buffer* buffer_Found = fBuffer_Head;
		while (buffer_Found)
			{
			if (buffer_Found->fStartPosition <= fPosition
				&& buffer_Found->fStartPosition + fBufferSize > fPosition)
				{
				break;
				}
			buffer_Found = buffer_Found->fNext;
			}

		size_t offsetInBuffer = fPosition % fBufferSize;

		if (buffer_Found == nil)
			{
			// We didn't find a buffer encompassing the position
			if (fBuffer_Tail->fDirty)
				{
				fStreamReal.SetPosition(fBuffer_Tail->fStartPosition);
				fStreamReal.Write(fBuffer_Tail->fData,
					min(uint64(fBufferSize), streamSize - fBuffer_Tail->fStartPosition));
				}
			buffer_Found = fBuffer_Tail;
			buffer_Found->fStartPosition = fPosition - offsetInBuffer;
			fStreamReal.SetPosition(buffer_Found->fStartPosition);
			fStreamReal.Read(buffer_Found->fData, fBufferSize, nil);
			buffer_Found->fDirty = false;
			}

		size_t copySize = min(iCount, fBufferSize - offsetInBuffer);
		ZBlockCopy(buffer_Found->fData + offsetInBuffer, localDest, copySize);
		iCount -= copySize;
		fPosition += copySize;
		localDest += copySize;

		if (fBuffer_Head != buffer_Found)
			{
			// Move buffer_Found to the head of the list by removing it
			if (buffer_Found->fPrev)
				buffer_Found->fPrev->fNext = buffer_Found->fNext;
			if (buffer_Found->fNext)
				buffer_Found->fNext->fPrev = buffer_Found->fPrev;
			if (fBuffer_Head == buffer_Found)
				fBuffer_Head = buffer_Found->fNext;
			if (fBuffer_Tail == buffer_Found)
				fBuffer_Tail = buffer_Found->fPrev;

			// and inserting it
			buffer_Found->fPrev = nil;
			buffer_Found->fNext = fBuffer_Head;
			fBuffer_Head->fPrev = buffer_Found;
			fBuffer_Head = buffer_Found;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<char*>(iDest);
	}

void ZStreamRWPos_PageBuffered::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = reinterpret_cast<const char*>(iSource);
	while (iCount)
		{
		// Do we have the offset in our buffers?
		Buffer* buffer_Found = fBuffer_Head;
		while (buffer_Found)
			{
			if (buffer_Found->fStartPosition <= fPosition
				&& buffer_Found->fStartPosition + fBufferSize > fPosition)
				{
				break;
				}
			buffer_Found = buffer_Found->fNext;
			}

		size_t offsetInBuffer = fPosition % fBufferSize;
		if (buffer_Found == nil)
			{
			// We didn't find a buffer encompassing the position
			if (fBuffer_Tail->fDirty)
				{
				fStreamReal.SetPosition(fBuffer_Tail->fStartPosition);
				fStreamReal.Write(fBuffer_Tail->fData,
					min(uint64(fBufferSize), fStreamReal.GetSize() - fBuffer_Tail->fStartPosition));
				}
			buffer_Found = fBuffer_Tail;
			buffer_Found->fStartPosition = fPosition - offsetInBuffer;

			size_t countToFillBuffer = min(uint64(fBufferSize),
				fStreamReal.GetSize() - buffer_Found->fStartPosition);

			if (offsetInBuffer || iCount < countToFillBuffer)
				{
				// We need to read the original data because we're either not going to write
				// starting at the beginning of the buffer, or we're not going to overwrite
				// the entire valid contents of the buffer (or both).
				fStreamReal.SetPosition(buffer_Found->fStartPosition);
				fStreamReal.Read(buffer_Found->fData, countToFillBuffer);
				}
			}

		size_t copySize = min(iCount, fBufferSize - offsetInBuffer);
		ZBlockCopy(localSource, buffer_Found->fData + offsetInBuffer, copySize);
		buffer_Found->fDirty = true;

		iCount -= copySize;
		fPosition += copySize;
		localSource += copySize;


		if (fBuffer_Head != buffer_Found)
			{
			// Move buffer_Found to the head of the list by removing it
			if (buffer_Found->fPrev)
				buffer_Found->fPrev->fNext = buffer_Found->fNext;
			if (buffer_Found->fNext)
				buffer_Found->fNext->fPrev = buffer_Found->fPrev;
			if (fBuffer_Head == buffer_Found)
				fBuffer_Head = buffer_Found->fNext;
			if (fBuffer_Tail == buffer_Found)
				fBuffer_Tail = buffer_Found->fPrev;

			// and inserting it
			buffer_Found->fPrev = nil;
			buffer_Found->fNext = fBuffer_Head;
			fBuffer_Head->fPrev = buffer_Found;
			fBuffer_Head = buffer_Found;
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const char*>(iSource);
	}

void ZStreamRWPos_PageBuffered::Imp_Flush()
	{
	Buffer* buffer_Current = fBuffer_Head;
	while (buffer_Current)
		{
		if (buffer_Current->fDirty)
			{
			fStreamReal.SetPosition(buffer_Current->fStartPosition);
			size_t flushSize = min(uint64(fBufferSize),
				fStreamReal.GetSize() - buffer_Current->fStartPosition);

			size_t countWritten;
			fStreamReal.Write(buffer_Current->fData, flushSize, &countWritten);
			if (countWritten == flushSize)
				{
				buffer_Current->fDirty = false;
				}
			else
				{
				// We're pretty screwed if the underlying stream could not write all the data we
				// asked it to. This should not ever happen, because we resize the real stream
				// whenever we're asked to resize, and insufficient room to grow a stream is
				// likely the only way that Write could fail.
				ZDebugStop(1);
				}
			}
		buffer_Current = buffer_Current->fNext;
		}
	fStreamReal.Flush();
	}

uint64 ZStreamRWPos_PageBuffered::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_PageBuffered::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_PageBuffered::Imp_GetSize()
	{ return fStreamReal.GetSize(); }

void ZStreamRWPos_PageBuffered::Imp_SetSize(uint64 iSize)
	{
	if (iSize < fStreamReal.GetSize())
		{
		// If we're shrinking, then ditch any buffers that cover space beyond
		// the end of the new size
		for (Buffer* buffer_Current = fBuffer_Head;
			buffer_Current; buffer_Current = buffer_Current->fNext)
			{
			if (buffer_Current->fStartPosition >= iSize)
				{
				buffer_Current->fDirty = false;
				buffer_Current->fStartPosition = 0 - fBufferSize - 1;
				}
			}
		}
	fStreamReal.SetSize(iSize);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_PageBuffered

ZStreamerRWPos_PageBuffered::ZStreamerRWPos_PageBuffered(
	size_t iBufferCount, size_t iBufferSize, ZRef<ZStreamerRWPos> iStreamerReal)
:	fStreamerReal(iStreamerReal),
	fStream(iBufferCount, iBufferSize, iStreamerReal->GetStreamRWPos())
	{}

ZStreamerRWPos_PageBuffered::~ZStreamerRWPos_PageBuffered()
	{}

const ZStreamRWPos& ZStreamerRWPos_PageBuffered::GetStreamRWPos()
	{ return fStream; }
