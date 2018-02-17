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

#ifndef __ZooLib_Chan_XX_Limited_h__
#define __ZooLib_Chan_XX_Limited_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/Channer.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Limited

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
		const size_t countRead = sRead(inherited::pGetChan(), oDest, std::min<uint64>(fLimit, iCount));
		fLimit -= countRead;
		return countRead;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const size_t countSkipped = sSkip(inherited::pGetChan(), std::min<uint64>(fLimit, iCount));
		fLimit -= countSkipped;
		return countSkipped;
		}

	virtual size_t Readable()
		{ return std::min<uint64>(fLimit, sReadable(inherited::pGetChan())); }

protected:
	uint64 fLimit;
	};

template <class Channer_p>
ZRef<Channer_p> sChannerR_Limited(const ZRef<Channer_p>& iChanner, size_t iLimit)
	{ return sChanner_Channer_T<ChanR_XX_Limited<ChanOfChanner<Channer_p>>>(iChanner, iLimit); }

// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_Limited

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
		const size_t countWritten = sWrite(inherited::pGetChan(), iSource, std::min<uint64>(fLimit, iCount));
		fLimit -= countWritten;
		return countWritten;
		}

protected:
	uint64 fLimit;
	};

template <class Channer_p>
ZRef<Channer_p> sChannerW_Limited(const ZRef<Channer_p>& iChanner, size_t iLimit)
	{ return sChanner_Channer_T<ChanW_XX_Limited<ChanOfChanner<Channer_p>>>(iChanner, iLimit); }

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Limited_h__
