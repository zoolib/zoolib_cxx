// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_Limited_h__
#define __ZooLib_Chan_XX_Limited_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/Channer.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Limited

template <class Chan_p>
class ChanR_XX_Limited
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanR_XX_Limited(const Chan_p& iChan, uint64 iLimit)
	:	inherited(iChan)
	,	fLimit(iLimit)
		{}

	ChanR_XX_Limited(uint64 iLimit, const Chan_p& iChan)
	:	inherited(iChan)
	,	fLimit(iLimit)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countRead = sRead(inherited::pGetChan(),
			oDest, std::min(sClamped(fLimit), iCount));
		fLimit -= countRead;
		return countRead;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		uint64 theCount = std::min(fLimit, iCount);
		const uint64 countSkipped = sSkip(inherited::pGetChan(), theCount);
		fLimit -= countSkipped;
		return countSkipped;
		}

	virtual size_t Readable()
		{ return std::min(sClamped(fLimit), sReadable(inherited::pGetChan())); }

protected:
	uint64 fLimit;
	};

template <class Channer_p>
ZP<Channer_p> sChannerR_Limited(const ZP<Channer_p>& iChanner, size_t iLimit)
	{
	if (ZP<Counted> asCounted = iChanner)
		{
		typedef ChanOfChanner<Channer_p> Chan_p;
		return sChanner_Holder_T<ZP<Counted>,ChanR_XX_Limited<Chan_p>>
			(asCounted, *iChanner, iLimit);
		}
	return null;
	}

// =================================================================================================
#pragma mark - ChanW_XX_Limited

template <class Chan_p>
class ChanW_XX_Limited
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_Limited(const Chan_p& iChan, uint64 iLimit)
	:	inherited(iChan)
	,	fLimit(iLimit)
		{}

	ChanW_XX_Limited(uint64 iLimit, const Chan_p& iChan)
	:	inherited(iChan)
	,	fLimit(iLimit)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		const size_t countWritten = sWrite(inherited::pGetChan(),
			iSource, std::min<uint64>(fLimit, iCount));
		fLimit -= countWritten;
		return countWritten;
		}

protected:
	uint64 fLimit;
	};

template <class Channer_p>
ZP<Channer_p> sChannerW_Limited(const ZP<Channer_p>& iChanner, size_t iLimit)
	{
	if (ZP<Counted> asCounted = iChanner)
		{
		return sChanner_Holder_T<ZP<Counted>,ChanW_XX_Limited<ChanOfChanner<Channer_p>>>
			(asCounted, *iChanner, iLimit);
		}
	return null;
	}

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Limited_h__
