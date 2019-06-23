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

#include "zoolib/ChanFilter.h"
#include "zoolib/Channer.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Buffered

template <class Chan_p>
class ChanR_XX_Buffered
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanR_XX_Buffered(const Chan_p& iChan, size_t iBufferSize)
	:	inherited(iChan)
	,	fBuffer(iBufferSize, EE())
	,	fBegin(0)
	,	fEnd(0)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		while (iCount)
			{
			if (fEnd > fBegin)
				{
				// We have some data in our buffer, consume it first.
				const size_t countToMove = std::min(fEnd - fBegin, iCount);
				std::copy_n(&fBuffer[fBegin], countToMove, localDest);
				fBegin += countToMove;
				localDest += countToMove;
				iCount -= countToMove;
				}
			else
				{
				// Our buffer is empty.
				const size_t countReadable = sReadable(inherited::pGetChan());
				if (iCount >= fBuffer.size() || iCount >= countReadable)
					{
					// Either we're asking for more data than would fit in our buffer, or we're
					// asking for more data than the stream will be able to provide without
					// blocking. In either case we bypass the buffer and read straight into our
					// read destination
					const size_t countRead = sRead(inherited::pGetChan(), localDest, iCount);
					localDest += countRead;
					iCount -= countRead;
					break;
					}
				else
					{
					// We're asking for less data than the stream guarantees it could provide
					// without blocking, in which case we fill up as much of our buffer as we can,
					// so some later request will be able to be satisfied straight from our buffer.
					const size_t countRead = sRead(inherited::pGetChan(),
						&fBuffer[0],
						std::min(fBuffer.size(), countReadable));
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
		{ return fEnd - fBegin + sReadable(inherited::pGetChan()); }

// From ChanAspect_WaitReadable
	virtual bool WaitReadable(double iTimeout)
		{
		if (fEnd > fBegin)
			return true;
		return WaitReadableIf<Chan_p>::sCall(inherited::pGetChan(), iTimeout);
		}

protected:
	std::vector<EE> fBuffer;
	size_t fBegin;
	size_t fEnd;
	};

template <class Channer_p>
ZP<Channer_p> sChannerR_Buffered(const ZP<Channer_p>& iChanner, size_t iBufferSize)
	{ return sChanner_Channer_T<ChanR_XX_Buffered<ChanOfChanner<Channer_p>>>(iChanner, iBufferSize); }

// =================================================================================================
#pragma mark - ChanW_XX_Buffered

template <class Chan_p>
class ChanW_XX_Buffered
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_Buffered(const Chan_p& iChan, size_t iBufferSize)
	:	inherited(iChan)
	,	fBuffer(iBufferSize, EE())
	,	fOffset(0)
		{}

	~ChanW_XX_Buffered()
		{
		try
			{
			this->pFlush();
			}
		catch (...)
			{
			ZDebugLogf(1, ("~ChanW_XX_Buffered, unable to flush entire buffer"));
			}
		}

	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		const EE* localSource = iSource;
		while (iCount)
			{
			if (fOffset == 0 && fBuffer.size() <= iCount)
				{
				// We have an empty buffer *and* we have more data to send than would fit in the buffer.
				const size_t countWritten = sWrite(inherited::pGetChan(), localSource, iCount);
				if (countWritten == 0)
					break;
				localSource += countWritten;
				iCount -= countWritten;
				}
			else
				{
				// Either we already have data in the buffer, or we have an empty buffer
				// and less than a buffer's worth to send.
				const size_t countToCopy = std::min(iCount, fBuffer.size() - fOffset);
				std::copy_n(localSource, countToCopy, &fBuffer[fOffset]);
				fOffset += countToCopy;
				localSource += countToCopy;
				iCount -= countToCopy;
				if (fOffset == fBuffer.size())
					{
					// The buffer's completely full.
					this->pFlush();
					}
				}
			}

		return localSource - iSource;
		}

	virtual void Flush()
		{
		this->pFlush();
		sFlush(inherited::pGetChan());
		}

protected:
	void pFlush()
		{
		if (size_t used = sGetSet(fOffset, 0))
			{
			if (used != sWriteFully(inherited::pGetChan(), &fBuffer[0], used))
				sThrow_ExhaustedW();
			}
		}

	std::vector<EE> fBuffer;
	size_t fOffset;
	};

template <class Channer_p>
ZP<Channer_p> sChannerW_Buffered(const ZP<Channer_p>& iChanner, size_t iBufferSize)
	{ return sChanner_Channer_T<ChanW_XX_Buffered<ChanOfChanner<Channer_p>>>(iChanner, iBufferSize); }

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Buffered_h__
