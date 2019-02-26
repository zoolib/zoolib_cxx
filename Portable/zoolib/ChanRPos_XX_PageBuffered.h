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

#ifndef __ZooLib_ChanRPos_XX_PageBuffered_h__
#define __ZooLib_ChanRPos_XX_PageBuffered_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/Memory.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRPos_XX_PageBuffered

/** A positionable read filter stream that buffers a fixed number of fixed size chunks
which are read in preference to accessing the source stream. Chunks are recycled in
least recently used order.
*/
template <class EE>
class ChanRPos_XX_PageBuffered
:	public ChanRPos<EE>
	{
public:
	ChanRPos_XX_PageBuffered(const ChanRPos<EE>& iChanRPos, size_t iBufferCount, size_t iBufferSize)
	:	fChanReal(iChanRPos)
		{
		fBuffer_Head = nullptr;
		fBuffer_Tail = nullptr;

		fBufferSize = iBufferSize;

		fPosition = sPos(fChanReal);

		// Allocate all the buffers now, so we don't have to do it dynamically
		while (iBufferCount--)
			{
			Buffer* aBuffer = new Buffer;
			aBuffer->fData = aBuffer->fData = new EE[fBufferSize];
			// By marking the start position as covering the last fBufferSize
			// bytes of the 64 bit address space we don't need to carry
			// a loaded flag. The weakness is if the user needs to read
			// that part of the address space before all the buffers have
			// been used for other portions. This is unlikely, but possible.
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

	~ChanRPos_XX_PageBuffered()
		{
		for (Buffer* buffer_Current = fBuffer_Head; buffer_Current; /*no inc*/)
			{
			Buffer* nextBuffer = buffer_Current->fNext;
			delete[] buffer_Current->fData;
			delete buffer_Current;
			buffer_Current = nextBuffer;
			}
		}

// From ChanAspect_Pos,
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanAspect_Read<EE>,
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		iCount = sClamped(sClamped(iCount, sSize(fChanReal), fPosition));
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

			if (buffer_Found == nullptr)
				{
				buffer_Found = fBuffer_Tail;
				buffer_Found->fStartPosition = fPosition - (fPosition % fBufferSize);
				sPosSet(fChanReal, buffer_Found->fStartPosition);
				sRead(fChanReal, buffer_Found->fData, fBufferSize);
				}

			const size_t offsetInBuffer = fPosition % fBufferSize;
			const size_t copyCount = sClamped(sClamped(iCount, fBufferSize, offsetInBuffer));
			std::copy_n(buffer_Found->fData + offsetInBuffer, copyCount, localDest);
			iCount -= copyCount;
			fPosition += copyCount;
			localDest += copyCount;

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

// From ChanAspect_Size,
	virtual uint64 Size()
		{ return sSize(fChanReal); }

// From ChanAspect_Unread<EE>
	virtual void Unread(const EE* iSource, size_t iCount)
		{
		ZAssert(fPosition >= iCount);
		fPosition -= iCount;
		// See Chan_XX_Memory for a note regarding bogus unreads.
		}

protected:
	const ChanRPos<EE>& fChanReal;

	struct Buffer
		{
		Buffer* fPrev;
		Buffer* fNext;
		EE* fData;
		uint64 fStartPosition;
		};

	Buffer* fBuffer_Head;
	Buffer* fBuffer_Tail;
	size_t fBufferSize;

	uint64 fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRPos_XX_PageBuffered_h__
