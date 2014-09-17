/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Chan_XX_Buffered_h__
#define __ZooLib_Chan_XX_Buffered_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

#include "zoolib/ZCompat_algorithm.h" // for sMinMax

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR_XX_Buffered

template <class XX>
class ChanR_XX_Buffered
:	public ChanR<XX>
	{
public:
	typedef XX Elmt_t;

	ChanR_XX_Buffered(size_t iBufferSize, const ChanR<XX>& iChanR)
	:	fChanR(iChanR)
	,	fBuffer(sMinMax<size_t>(128, iBufferSize, 8192), 0),
	,	fBufferSize(iBufferSize)
		{}

// From ChanR
	virtual size_t Read(Elmt_t* oDest, size_t iCount)
		{
		uint8* localDest = reinterpret_cast<uint8*>(oDest);
		while (iCount)
			{
			size_t countInBuffer = fBuffer.size() - fBufferOffset;
			if (countInBuffer)
				{
				// We have some data in our buffer, use it up first.
				size_t countToMove = min(countInBuffer, iCount);
				sMemCopy(localDest, &fBuffer[fBufferOffset], countToMove);
				fBufferOffset += countToMove;
				localDest += countToMove;
				iCount -= countToMove;
				}
			else
				{
				// Our buffer is empty.
				size_t countReadable = fStreamSource.CountReadable();
				if (iCount >= fBuffer.size() || iCount >= countReadable)
					{
					// Either we're asking for more data than would fit in our buffer, or we're asking
					// for more data than the stream will be able to provide without blocking. In
					// either case we bypass the buffer and read straight into our read destination
					size_t countRead;
					fStreamSource.Read(localDest, iCount, &countRead);
					if (countRead == 0)
						break;
					localDest += countRead;
					iCount -= countRead;
					}
				else
					{
					// We're asking for less data than the stream guarantees it could provide without
					// blocking, in which case we fill up as much of our buffer as we can, so some
					// later request will be able to be satisfied straight from our buffer.
					size_t countToRead = min(fBuffer.size(), countReadable);
					size_t countRead;
					fStreamSource.Read(&fBuffer[0] + (fBuffer.size() - countToRead), countToRead, &countRead);
					if (countRead == 0)
						break;
					// If countRead is less than what we asked for, shift the buffer contents so that
					// the last byte read aligns with the last byte of the buffer, otherwise we need
					// to maintain an additional instance variable to track not just where we're
					// feeding out from (fBufferOffset), but just how much more data is actually valid.
					if (countRead < countToRead)
						{
						sMemMove(&fBuffer[0] + (fBuffer.size() - countRead),
							&fBuffer[0] + (fBuffer.size() - countToRead), countRead);
						}
					fBufferOffset = fBuffer.size() - countRead;
					}
				}
			}
		if (oCountRead)
			*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
		}

	virtual size_t Readable()
		{ return fBuffer.size() + sReadable(fChanR); }

protected:
	const ChanR<XX>& fChanR;
	vector<XX> fBuffer;
	};

// =================================================================================================
// MARK: - ChanW_XX_Tee

template <class XX>
class ChanW_XX_Tee
:	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;

	ChanW_XX_Tee(const ChanW<XX>& iChanW0, const ChanW<XX>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
		{}

// From ChanW
	virtual size_t Write(const Elmt_t* iSource, size_t iCount)
		{
		if (const size_t countWritten0 = sWrite(iSource, iCount, fChanW0))
			{
			sWriteFully(iSource, countWritten0, fChanW1);
			return countWritten0;
			}
		else
			{
			return sWrite(iSource, iCount, fChanW1);
			}
		}

	virtual void Flush()
		{
		sFlush(fChanW0);
		sFlush(fChanW1);
		}

protected:
	const ChanW<XX>& fChanW0;
	const ChanW<XX>& fChanW1;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Buffered_h__
