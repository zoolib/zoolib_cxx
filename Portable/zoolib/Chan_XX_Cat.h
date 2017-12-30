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

#ifndef __ZooLib_Chan_XX_Cat_h__
#define __ZooLib_Chan_XX_Cat_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Cat

template <class EE>
class ChanR_XX_Cat
:	public ChanR<EE>
	{
public:
	ChanR_XX_Cat(const ChanR<EE>& iChanR0, const ChanR<EE>& iChanR1)
	:	fChanR0(iChanR0)
	,	fChanR1(iChanR1)
	,	fFirstIsLive(true)
		{}

// From ChanR
	virtual size_t QRead(EE* oDest, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countRead = sQRead(fChanR0, oDest, iCount))
				return countRead;
			fFirstIsLive = false;
			}
		return sQRead(fChanR1, oDest, iCount);
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
#pragma mark -
#pragma mark ChanW_XX_Cat

template <class EE>
class ChanW_XX_Cat
:	public ChanW<EE>
	{
public:
	ChanW_XX_Cat(const ChanW<EE>& iChanW0, const ChanW<EE>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
	,	fFirstIsLive(true)
		{}

// From ChanW
	virtual size_t QWrite(const EE* iSource, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countWritten0 = sQWrite(fChanW0, iSource, iCount))
				return countWritten0;
			fFirstIsLive = false;
			}
		return sQWrite(fChanW1, iSource, iCount);
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
