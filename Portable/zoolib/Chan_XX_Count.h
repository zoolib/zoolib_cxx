// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_Count_h__
#define __ZooLib_Chan_XX_Count_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Count

template <class Chan_p>
class ChanR_XX_Count
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanR_XX_Count(const Chan_p& iChan)
	:	inherited(iChan)
	,	fCount(0)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countRead = sRead(inherited::pGetChan(), oDest, iCount);
		fCount += countRead;
		return countRead;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const uint64 countSkipped = sSkip(inherited::pGetChan(), iCount);
		fCount += countSkipped;
		return countSkipped;
		}

// Our protocol
	uint64 GetCount()
		{ return fCount; }

protected:
	uint64 fCount;
	};

// =================================================================================================
#pragma mark - ChanW_XX_Count

template <class Chan_p>
class ChanW_XX_Count
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
public:
	ChanW_XX_Count(const Chan_p& iChan)
	:	inherited(iChan)
	,	fCount(0)
		{}

// From ChanW
	virtual size_t Write(const typename Chan_p::Element_t* iSource, size_t iCount)
		{
		const size_t countWritten = sWrite(inherited::pGetChan(), iSource, iCount);
		fCount += countWritten;
		return countWritten;
		}

// Our protocol
	uint64 GetCount()
		{ return fCount; }

protected:
	uint64 fCount;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Count_h__
