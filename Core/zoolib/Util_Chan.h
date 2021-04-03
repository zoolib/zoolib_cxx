// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Chan_h__
#define __ZooLib_Util_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

inline void sClear(const DeriveFrom<ChanAspect_Pos,ChanAspect_SizeSet>& iChan)
	{
	sPosSet(iChan, 0);
	sSizeSet(iChan, 0);
	}

// =================================================================================================
#pragma mark -

class SaveSetRestorePos
	{
	const ChanPos& fChanPos;
	const uint64 fPriorPos;
public:
	SaveSetRestorePos(const ChanPos& iChanPos, uint64 iPos)
	:	fChanPos(iChanPos)
	,	fPriorPos(sPos(iChanPos))
		{ sPosSet(iChanPos, iPos); }

	~SaveSetRestorePos()
		{ sPosSet(fChanPos, fPriorPos); }
	};

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
			buf, std::min<size_t>(sClamped(countRemaining), countof(buf))))
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
	{ sQCopyAll(iChanR, iChanW) || sThrow_ExhaustedW(); }

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
bool sTryRead(EE iEE, const ChanRU<EE>& iChanRU)
	{
	if (ZQ<EE> theElement = sQRead(iChanRU))
		{
		if (*theElement == iEE)
			return true;
		sUnread(iChanRU, *theElement);
		}
	return false;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_h__
