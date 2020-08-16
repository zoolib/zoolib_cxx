// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_Tee_h__
#define __ZooLib_Chan_XX_Tee_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_Tee

template <class Chan_p>
class ChanR_XX_Tee
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanR_XX_Tee(const Chan_p& iChan, const ChanW<EE>& iChanW)
	:	inherited(iChan)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		// We have to read into a local buffer because we're going to pass
		// what we read to fChanW, and oDest could reference memory that's
		// not safe to read (the garbage buffer, for example).

		EE buf[std::min<size_t>(iCount, sStackBufferSize / sizeof(EE))];

		if (const size_t countRead = sRead(inherited::pGetChan(), buf, std::min(iCount, countof(buf))))
			{
			std::copy_n(buf, countRead, oDest);

			const size_t countWritten = sWriteFully(fChanW, buf, countRead);

			if (countWritten != countRead)
				{
				// Hmm, what to do? Report it, ignore it?
				}
			return countRead;
			}

		return 0;
		}

protected:
	const ChanW<EE>& fChanW;
	};

// =================================================================================================
#pragma mark - ChanW_XX_Tee

template <class Chan_p>
class ChanW_XX_Tee
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
	typedef typename Chan_p::Element_t EE;
public:
	ChanW_XX_Tee(const Chan_p& iChan, const ChanW<EE>& iChanW)
	:	inherited(iChan)
	,	fChanW(iChanW)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		if (const size_t countWritten0 = sWrite(inherited::pGetChan(), iSource, iCount))
			{
			sWriteFully(fChanW, iSource, countWritten0);
			return countWritten0;
			}
		else
			{
			return sWrite(fChanW, iSource, iCount);
			}
		}

	virtual void Flush()
		{
		inherited::Flush();
		sFlush(fChanW);
		}

protected:
	const ChanW<EE>& fChanW;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Tee_h__
