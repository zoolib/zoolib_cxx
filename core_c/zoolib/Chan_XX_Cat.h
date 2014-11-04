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
// MARK: - ChanR_XX_Cat

template <class XX>
class ChanR_XX_Cat
:	public ChanR<XX>
	{
public:
	typedef XX Elmt_t;

	ChanR_XX_Cat(const ChanR<XX>& iChanR0, const ChanR<XX>& iChanR1)
	:	fChanR0(iChanR0)
	,	fChanR1(iChanR1)
	,	fFirstIsLive(true)
		{}

// From ChanR
	virtual size_t QRead(Elmt_t* oDest, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countRead = sQRead(oDest, iCount, fChanR0))
				return countRead;
			fFirstIsLive = false;
			}
		return sQRead(oDest, iCount, fChanR1);
		}

	virtual uint64 Skip(uint64 iCount)
		{
		if (fFirstIsLive)
			return sSkip(iCount, fChanR0);
		return sSkip(iCount, fChanR1);
		}

	virtual size_t Readable()
		{
		if (fFirstIsLive)
			return sReadable(fChanR0);
		return sReadable(fChanR1);
		}

protected:
	const ChanR<XX>& fChanR0;
	const ChanR<XX>& fChanR1;
	bool fFirstIsLive;
	};

// =================================================================================================
// MARK: - ChanW_XX_Cat

template <class XX>
class ChanW_XX_Cat
:	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;

	ChanW_XX_Cat(const ChanW<XX>& iChanW0, const ChanW<XX>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
	,	fFirstIsLive(true)
		{}

// From ChanW
	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (const size_t countWritten0 = sQWrite(iSource, iCount, fChanW0))
				return countWritten0;
			fFirstIsLive = false;
			}
		return sQWrite(iSource, iCount, fChanW1);
		}

	virtual void Flush()
		{
		if (fFirstIsLive)
			sFlush(fChanW0);
		else
			sFlush(fChanW1);
		}

protected:
	const ChanW<XX>& fChanW0;
	const ChanW<XX>& fChanW1;
	bool fFirstIsLive;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Cat_h__
