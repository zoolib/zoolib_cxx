// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_h__
#define __ZooLib_ChanR_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include "zoolib/ZQ.h"

#include <stdexcept> // For range_error
#include <string> // because range_error may require it

namespace ZooLib {

// =================================================================================================
#pragma mark -

ZMACRO_NoReturn_Pre
inline bool sThrow_ExhaustedR();
ZMACRO_NoReturn_Post

inline bool sThrow_ExhaustedR()
	{ throw std::range_error("ChanR Exhausted"); }

// =================================================================================================
#pragma mark -

template <class EE>
ZQ<EE> sQRead(const ChanR<EE>& iChanR)
	{
	EE buf;
	if (1 != sRead(iChanR, &buf, 1))
		return null;
	return buf;
	}

template <class EE>
EE sERead(const ChanR<EE>& iChanR)
	{
	EE buf;
	if (1 != sRead(iChanR, &buf, 1))
		sThrow_ExhaustedR();
	return buf;
	}

template <class EE>
size_t sReadFully(const ChanR<EE>& iChanR, EE* oDest, size_t iCount)
	{
	EE* localDest = oDest;
	while (iCount)
		{
		if (const size_t countRead = sRead(iChanR, localDest, iCount))
			{
			iCount -= countRead;
			localDest += countRead;
			}
		else
			{
			break;
			}
		}
	return localDest - oDest;
	}

template <class EE>
size_t sReadFully(const ChanR<EE>& iChanR, const PaC<EE>& iDest)
	{ return sReadFully(iChanR, sPtr(iDest), sCount(iDest)); }

template <class EE>
uint64 sSkipFully(const ChanR<EE>& iChanR, uint64 iCount)
	{
	uint64 countRemaining = iCount;

	// We need the 64->32 bit clamping stuff here
	while (countRemaining)
		{
		if (const uint64 countSkipped = sSkip(iChanR, countRemaining))
			countRemaining -= countSkipped;
		else
			break;
		}

	return iCount - countRemaining;
	}

template <class EE>
uint64 sSkipAll(const ChanR<EE>& iChanR)
	{
	uint64 result = 0;
	while (const uint64 countSkipped = sSkip(iChanR, 0x100000))
		result += countSkipped;
	return result;
	}

template <class EE>
void sERead(const ChanR<EE>& iChanR, EE* oDest, size_t iCount)
	{ iCount == sReadFully<EE>(iChanR, oDest, iCount) || sThrow_ExhaustedR(); }

template <class EE>
void sERead(const ChanR<EE>& iChanR, const PaC<EE>& iDest)
	{ sERead(iChanR, sPtr(iDest), sCount(iDest)); }

template <class EE>
void sESkip(const ChanR<EE>& iChanR, uint64 iCount)
	{ iCount == sSkipFully<EE>(iChanR, iCount) || sThrow_ExhaustedR(); }

// =================================================================================================
#pragma mark - ChanR_XX_Null

/// A read Chan with no content.

template <class EE>
class ChanR_XX_Null
:	public ChanR<EE>
	{
public:
	virtual size_t Read(EE* oDest, size_t iCount)
		{ return 0; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
