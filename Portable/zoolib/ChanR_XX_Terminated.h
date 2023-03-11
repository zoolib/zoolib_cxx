// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_XX_Terminated_h__
#define __ZooLib_ChanR_XX_Terminated_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Terminated

template <class EE>
class ChanR_XX_Terminated
:	public virtual ChanR<EE>
	{
public:
	ChanR_XX_Terminated(EE iTerminator, const ChanR<EE>& iChanR)
	:	fTerminator(iTerminator)
	,	fChanR(iChanR)
	,	fHitTerminator(false)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		size_t countRemaining = iCount;
		while (not fHitTerminator && countRemaining)
			{
			ZQ<EE> theQ = sQRead(fChanR);
			if (not theQ)
				break;

			if (*theQ == fTerminator)
				{
				fHitTerminator = true;
				break;
				}
			*oDest++ = *theQ;
			--countRemaining;
			}
		return iCount - countRemaining;
		}

	virtual size_t Readable()
		{ return fHitTerminator ? 0 : sReadable(fChanR); }

// Our protocol
	bool HitTerminator()
		{ return fHitTerminator; }

	void Reset()
		{ fHitTerminator = false; }

protected:
	const EE fTerminator;
	const ChanR<EE>& fChanR;
	bool fHitTerminator;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Terminated_h__
