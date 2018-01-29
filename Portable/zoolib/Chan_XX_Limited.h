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

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Limited

template <class EE>
class ChanR_XX_Limited
:	public ChanR<EE>
	{
public:
	ChanR_XX_Limited(uint64 iLimit, const ChanR<EE>& iChanR)
	:	fChanR(iChanR)
	,	fLimit(iLimit)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countRead = sRead(fChanR, oDest, std::min<uint64>(fLimit, iCount));
		fLimit -= countRead;
		return countRead;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const size_t countSkipped = sSkip(fChanR, std::min<uint64>(fLimit, iCount));
		fLimit -= countSkipped;
		return countSkipped;
		}

	virtual size_t Readable()
		{ return std::min<uint64>(fLimit, sReadable(fChanR)); }

protected:
	const ChanR<EE>& fChanR;
	uint64 fLimit;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_Limited

template <class EE>
class ChanW_XX_Limited
:	public ChanW<EE>
	{
public:
	ChanW_XX_Limited(uint64 iLimit, const ChanW<EE>& iChanW)
	:	fChanW(iChanW)
	,	fLimit(iLimit)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		const size_t countWritten = sWrite(iSource, std::min<uint64>(fLimit, iCount), fChanW);
		fLimit -= countWritten;
		return countWritten;
		}

	virtual void Flush()
		{ sFlush(fChanW); }

protected:
	const ChanW<EE>& fChanW;
	uint64 fLimit;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Limited_h__
