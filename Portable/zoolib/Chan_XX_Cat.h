// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_Cat_h__
#define __ZooLib_Chan_XX_Cat_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Cat

template <class EE>
class ChanR_XX_Cat
:	public virtual ChanR<EE>
	{
public:
	ChanR_XX_Cat(const ChanR<EE>& iChanR0, const ChanR<EE>& iChanR1)
	:	fChanR0(iChanR0)
	,	fChanR1(iChanR1)
	,	fFirstIsLive(true)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countRead = sRead(fChanR0, oDest, iCount))
				return countRead;
			fFirstIsLive = false;
			}
		return sRead(fChanR1, oDest, iCount);
		}

	virtual uint64 Skip(uint64 iCount)
		{
		if (fFirstIsLive)
			return sSkip(fChanR0, iCount);
		return sSkip(fChanR1, iCount);
		}

	virtual size_t Readable()
		{
		if (fFirstIsLive)
			return sReadable(fChanR0);
		return sReadable(fChanR1);
		}

protected:
	const ChanR<EE>& fChanR0;
	const ChanR<EE>& fChanR1;
	bool fFirstIsLive;
	};

// =================================================================================================
#pragma mark - ChanW_XX_Cat

template <class EE>
class ChanW_XX_Cat
:	public virtual ChanW<EE>
	{
public:
	ChanW_XX_Cat(const ChanW<EE>& iChanW0, const ChanW<EE>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
	,	fFirstIsLive(true)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countWritten0 = sWrite(fChanW0, iSource, iCount))
				return countWritten0;
			fFirstIsLive = false;
			}
		return sWrite(fChanW1, iSource, iCount);
		}

	virtual void Flush()
		{
		if (fFirstIsLive)
			sFlush(fChanW0);
		else
			sFlush(fChanW1);
		}

protected:
	const ChanW<EE>& fChanW0;
	const ChanW<EE>& fChanW1;
	bool fFirstIsLive;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Cat_h__
