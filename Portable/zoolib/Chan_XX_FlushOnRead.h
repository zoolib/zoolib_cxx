// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_FlushOnRead_h__
#define __ZooLib_Chan_XX_FlushOnRead_h__ 1
#include "zconfig.h"

#include "zoolib/Atomic.h"
#include "zoolib/Channer.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Chan_XX_FlushOnRead

template <class EE>
class Chan_XX_FlushOnRead
:	public ChanRW<EE>
	{
public:
	Chan_XX_FlushOnRead(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	:	fChanR(iChanR)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		if (sAtomic_GetSet(&fLastWasWrite, 0))
			{
			ZAcqMtx acq(fMutex);
			sFlush(fChanW);
			}
		return sRead(fChanR, oDest, iCount);
		}

	virtual size_t Readable()
		{
		if (sAtomic_GetSet(&fLastWasWrite, 0))
			{
			ZAcqMtx acq(fMutex);
			sFlush(fChanW);
			}
		return sReadable(fChanR);
		}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		sAtomic_Set(&fLastWasWrite, 1);
		ZAcqMtx acq(fMutex);
		return sWrite(fChanW, iSource, iCount);
		}

protected:
	ZAtomic_t fLastWasWrite;
	ZMtx fMutex;
	const ChanR<EE>& fChanR;
	const ChanW<EE>& fChanW;
	};

// =================================================================================================
#pragma mark - Channer_XX_FlushOnRead

template <class EE>
class Channer_XX_FlushOnRead
:	public Channer<ChanRW<EE>>
,	public Chan_XX_FlushOnRead<EE>
	{
public:
	Channer_XX_FlushOnRead(const ZP<Channer<ChanR<EE>>>& iChannerR,
		const ZP<Channer<ChanW<EE>>>& iChannerW)
	:	Chan_XX_FlushOnRead<EE>(*iChannerR, *iChannerW)
	,	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
		{}

private:
	const ZP<Channer<ChanR<EE>>> fChannerR;
	const ZP<Channer<ChanW<EE>>> fChannerW;
	};

template <class EE>
ZP<Channer_XX_FlushOnRead<EE>> sChanner_XX_FlushOnRead(
	const ZP<Channer<ChanR<EE>>>& iChannerR,
	const ZP<Channer<ChanW<EE>>>& iChannerW)
	{ return new Channer_XX_FlushOnRead<EE>(iChannerR, iChannerW); }

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_FlushOnRead_h__
