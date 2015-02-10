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
#include "zoolib/Compat_algorithm.h" // for sMinMax

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Buffered

template <class XX>
class ChanR_XX_Buffered
:	public ChanR<XX>
	{
public:
	typedef XX Elmt_t;

	ChanR_XX_Buffered(size_t iBufferSize, const ChanR<XX>& iChanR)
	:	fChanR(iChanR)
	,	fBuffer(sMinMax<size_t>(128, iBufferSize, 8192), 0)
	,	fBegin(0)
	,	fEnd(0)
		{}

// From ChanR
	virtual size_t QRead(Elmt_t* oDest, size_t iCount)
		{
		Elmt_t* localDest = oDest;
		while (iCount)
			{
			if (fEnd > fBegin)
				{
				// We have some data in our buffer, use it up first.
				const size_t countToMove = std::min(fEnd - fBegin, iCount);
				std::copy(&fBuffer[fBegin], &fBuffer[fBegin + countToMove], localDest);
				fBegin += countToMove;
				localDest += countToMove;
				iCount -= countToMove;
				}
			else
				{
				// Our buffer is empty.
				const size_t countReadable = sReadable(fChanR);
				if (iCount >= fBuffer.size() || iCount >= countReadable)
					{
					// Either we're asking for more data than would fit in our buffer, or we're
					// asking for more data than the stream will be able to provide without
					// blocking. In either case we bypass the buffer and read straight into our
					// read destination
					const size_t countRead = sQRead(localDest, iCount, fChanR);
					if (countRead == 0)
						break;
					localDest += countRead;
					iCount -= countRead;
					}
				else
					{
					// We're asking for less data than the stream guarantees it could provide
					// without blocking, in which case we fill up as much of our buffer as we can,
					// so some later request will be able to be satisfied straight from our buffer.
					const size_t countRead = sQRead(&fBuffer[0],
						std::min(fBuffer.size(), countReadable),
						fChanR);
					if (countRead == 0)
						break;
					fBegin = 0;
					fEnd = countRead;
					}
				}
			}
		return localDest - oDest;
		}

	virtual size_t Readable()
		{ return fEnd - fBegin + sReadable(fChanR); }

protected:
	const ChanR<XX>& fChanR;
	std::vector<XX> fBuffer;
	size_t fBegin;
	size_t fEnd;
	};


// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_Buffered

template <class XX>
class ChanW_XX_Buffered
:	public ChanW<XX>
	{
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Buffered_h__
