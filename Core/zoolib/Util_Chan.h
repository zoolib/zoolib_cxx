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

#ifndef __ZooLib_Util_Chan_h__
#define __ZooLib_Util_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

inline void sTruncate(const ChanSizeSet& iChanSizeSet, const ChanPos& iChanPos)
	{
	sSizeSet(iChanSizeSet, 0);
	sPosSet(iChanPos, 0);
	}

// =================================================================================================
#pragma mark -

template <class EE>
std::pair<uint64,uint64> sCopyFully(
	const ChanR<EE>& iChanR, const ChanW<EE>& iChanW, uint64 iCount)
	{
	EE buf[(sStackBufferSize + sizeof(EE) - 1) / sizeof(EE)];

	for (uint64 countRemaining = iCount; /*no test*/; /*no inc*/)
		{
		if (const size_t countRead = sRead(iChanR,
			buf, std::min<size_t>(countRemaining, countof(buf))))
			{
			const size_t countWritten = sWriteFully(iChanW, buf, countRead);

			if (countWritten == countRead)
				{
				countRemaining -= countRead;
				// Here's where we return to the beginning of the loop.
				// In all other cases we exit.
				continue;
				}

			return std::pair<uint64,uint64>(
				iCount - countRemaining + countRead,
				iCount - countRemaining + countWritten);
			}

		return std::pair<uint64,uint64>(
			iCount - countRemaining,
			iCount - countRemaining);
		}
	}

template <class EE>
bool sQCopyFully(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW, uint64 iCount)
	{
	std::pair<uint64,uint64> result = sCopyFully(iChanR, iChanW, iCount);
	return iCount == result.first && result.first == result.second;
	}

template <class EE>
void sECopyFully(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW, uint64 iCount)
	{ sQCopyFully(iChanR, iChanW, iCount) || sThrow_ExhaustedW(); }

template <class EE>
std::pair<uint64,uint64> sCopyAll(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	{
	EE buf[(sStackBufferSize + sizeof(EE) - 1) / sizeof(EE)];

	uint64 totalCopied = 0;
	for (;;)
		{
		if (const size_t countRead = sRead(iChanR, buf, countof(buf)))
			{
			const size_t countWritten = sWriteFully(iChanW, buf, countRead);

			if (countWritten == countRead)
				{
				totalCopied += countRead;
				continue;
				}

			return std::pair<uint64,uint64>(totalCopied + countRead, totalCopied + countWritten);
			}

		return std::pair<uint64,uint64>(totalCopied, totalCopied);
		}
	}

template <class EE>
bool sQCopyAll(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	{
	std::pair<uint64,uint64> result = sCopyAll(iChanR, iChanW);
	return result.first == result.second;
	}

template <class EE>
void sECopyAll(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	{ sQCopyAll(iChanR, iChanW) ||sThrow_ExhaustedW(); }

// Not sure about the XX_Until name -- it made sense in ZUtil_Strim, maybe not so much elsewhere.
template <class EE>
bool sCopy_Until(const ChanR<EE>& iChanR, const EE& iTerminator, const ChanW<EE>& iChanW)
	{
	while (ZQ<EE> theQ = sQRead(iChanR))
		{
		if (iTerminator == *theQ)
			return true;
		else
			sEWrite(iChanW, *theQ);
		}
	return false;
	}

template <class EE>
bool sSkip_Until(const ChanR<EE>& iChanR, const EE& iTerminator)
	{ return sCopy_Until(iChanR, iTerminator, ChanW_XX_Discard<EE>()); }

// =================================================================================================
#pragma mark -

template <class EE>
bool sTryRead(EE iEE, const ChanR<EE>& iChanR, const ChanU<EE>& iChanU)
	{
	if (NotQ<EE> theElement = sQRead(iChanR))
		{ return false; }
	else if (*theElement == iEE)
		{ return true; }
	else
		{
		sUnread(iChanU, *theElement);
		return false;
		}	
	}

template <class EE>
bool sTryRead(EE iEE, const ChanRU<EE>& iChanRU)
	{ return sTryRead(iEE, iChanRU, iChanRU); }

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_h__
