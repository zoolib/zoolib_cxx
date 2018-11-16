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

#ifndef __ZooLib_ChanRWPos_XX_PageBuffered_h__
#define __ZooLib_ChanRWPos_XX_PageBuffered_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRWPos_XX_PageBuffered

/** A positionable read filter stream that buffers a fixed number of fixed size chunks
which are read in preference to accessing the source stream. Chunks are recycled in
least recently used order.
*/
template <class EE>
class ChanRWPos_XX_PageBuffered
:	public ChanRWPos<EE>
	{
public:
	ChanRWPos_XX_PageBuffered(const ChanRWPos<EE>& iChanRWPos, size_t iBufferCount, size_t iBufferSize)
	:	fChanReal(iChanRWPos)
		{
		fBuffer_Head = nullptr;
		fBuffer_Tail = nullptr;

		fBufferSize = iBufferSize;

		fPosition = sPos(fChanReal);

		// Allocate all the buffers now, so we don't have to do it dynamically
		while (iBufferCount--)
			{
			Buffer* aBuffer = new Buffer;
			aBuffer->fData = new EE[fBufferSize];
			aBuffer->fDirty = false;
			// By marking the start position as covering the last fBufferSize
			// bytes of the 64 bit address space we don't need to carry
			// a loaded flag. The weakness is if the user needs to read
			// that part of the address space before all the buffers have
			// been used for other portions. This is basically impossible,
			// 2^64 bytes is 16 exabytes.
			aBuffer->fStartPosition = 0 - fBufferSize;
			aBuffer->fPrev = nullptr;
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
			}
		}

	~ChanRWPos_XX_PageBuffered()
		{
		// Copy back any dirty buffers we might have ...
		this->pFlush();

		// ... but not fChanReal.

		// Dispose buffers
		for (Buffer* buffer_Current = fBuffer_Head; buffer_Current; /*no inc*/)
			{
			Buffer* nextBuffer = buffer_Current->fNext;
			delete[] buffer_Current->fData;
			delete buffer_Current;
			buffer_Current = nextBuffer;
			}
		}

// From Aspect_Pos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From Aspect_Read<EE>
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		uint64 streamSize = sSize(fChanReal);
		iCount = sClamped(sClamped(iCount, streamSize, fPosition));
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

			if (buffer_Found == nullptr)
				{
				// We didn't find a buffer encompassing the position
				if (fBuffer_Tail->fDirty)
					{
					sPosSet(fChanReal, fBuffer_Tail->fStartPosition);
					sWrite(fChanReal, fBuffer_Tail->fData,
						sClamped(sClamped(fBufferSize), streamSize, fBuffer_Tail->fStartPosition));
					}
				buffer_Found = fBuffer_Tail;
				buffer_Found->fStartPosition = fPosition - offsetInBuffer;
				sPosSet(fChanReal, buffer_Found->fStartPosition);
				sRead(fChanReal, buffer_Found->fData, fBufferSize);
				buffer_Found->fDirty = false;
				}

			size_t copySize = std::min(iCount, fBufferSize - offsetInBuffer);
			std::copy_n(buffer_Found->fData + offsetInBuffer, copySize, localDest);
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
				buffer_Found->fPrev = nullptr;
				buffer_Found->fNext = fBuffer_Head;
				fBuffer_Head->fPrev = buffer_Found;
				fBuffer_Head = buffer_Found;
				}
			}
		return localDest - oDest;
		}

// From Aspect_Size
	virtual uint64 Size()
		{ return sSize(fChanReal); }

// From Aspect_SizeSet
	virtual void SizeSet(uint64 iSize)
		{
		if (iSize < sSize(fChanReal))
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
		sSizeSet(fChanReal, iSize);
		}

// From Aspect_Unread<EE>
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t countToCopy = sClamped(std::min(uint64(iCount), this->fPosition));

		fPosition -= countToCopy;

		// See Chan_XX_Memory for a note regarding bogus unreads.

		return countToCopy;
		}

// From Aspect_Write
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		const EE* localSource = iSource;
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
			if (buffer_Found == nullptr)
				{
				// We didn't find a buffer encompassing the position
				if (fBuffer_Tail->fDirty)
					{
					sPosSet(fChanReal, fBuffer_Tail->fStartPosition);
					sEWrite(fChanReal, fBuffer_Tail->fData,
						sClamped(sClamped(fBufferSize), sSize(fChanReal), fBuffer_Tail->fStartPosition));
					}
				buffer_Found = fBuffer_Tail;
				buffer_Found->fStartPosition = fPosition - offsetInBuffer;

				size_t countToFillBuffer =
					sClamped(sClamped(fBufferSize, sSize(fChanReal), buffer_Found->fStartPosition));

				if (offsetInBuffer || iCount < countToFillBuffer)
					{
					// We need to read the original data because we're either not going to write
					// starting at the beginning of the buffer, or we're not going to overwrite
					// the entire valid contents of the buffer (or both).
					sPosSet(fChanReal, buffer_Found->fStartPosition);
					sRead(fChanReal, buffer_Found->fData, countToFillBuffer);
					}
				}

			size_t copySize = std::min(iCount, fBufferSize - offsetInBuffer);
			std::copy_n(localSource, copySize, buffer_Found->fData + offsetInBuffer);
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
				buffer_Found->fPrev = nullptr;
				buffer_Found->fNext = fBuffer_Head;
				fBuffer_Head->fPrev = buffer_Found;
				fBuffer_Head = buffer_Found;
				}
			}
		return localSource - iSource;
		}

	virtual void Flush()
		{
		this->pFlush();
		sFlush(fChanReal);
		}


protected:
	void pFlush()
		{
		Buffer* buffer_Current = fBuffer_Head;
		while (buffer_Current)
			{
			if (buffer_Current->fDirty)
				{
				sPosSet(fChanReal, buffer_Current->fStartPosition);
				size_t flushSize =
					sClamped(sClamped(fBufferSize, sSize(fChanReal), buffer_Current->fStartPosition));

				size_t countWritten = sWrite(fChanReal, buffer_Current->fData, flushSize);
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
		}

	const ChanRWPos<EE>& fChanReal;

	struct Buffer
		{
		Buffer* fPrev;
		Buffer* fNext;
		EE* fData;
		uint64 fStartPosition;
		bool fDirty;
		};

	Buffer* fBuffer_Head;
	Buffer* fBuffer_Tail;
	size_t fBufferSize;

	uint64 fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRWPos_XX_PageBuffered_h__
