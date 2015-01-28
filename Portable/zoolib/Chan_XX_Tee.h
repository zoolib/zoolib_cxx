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

#ifndef __ZooLib_Chan_XX_Tee_h__
#define __ZooLib_Chan_XX_Tee_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Tee

template <class XX>
class ChanR_XX_Tee
:	public ChanR<XX>
	{
public:
	typedef XX Elmt_t;

	ChanR_XX_Tee(const ChanR<XX>& iChanR, const ChanW<XX>& iChanW)
	:	fChanR(iChanR)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t QRead(Elmt_t* oDest, size_t iCount)
		{
		// We have to read into a local buffer because we're going to pass
		// what we read to fChanW, and oDest could reference memory that's
		// not safe to read (the garbage buffer, for example).

		Elmt_t buf[std::min<size_t>(iCount, sStackBufferSize / sizeof(Elmt_t))];

		if (const size_t countRead = sQRead(buf, std::min(iCount, countof(buf)), fChanR))
			{
			std::copy(buf, buf + countRead, oDest);

			const size_t countWritten = sQWriteFully(buf, countRead, fChanW);

			if (countWritten != countRead)
				{
				// Hmm, what to do? Report it, ignore it?
				}
			return countRead;
			}

		return 0;
		}

	virtual size_t Readable()
		{ return sReadable(fChanR); }

protected:
	const ChanR<XX>& fChanR;
	const ChanW<XX>& fChanW;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_Tee

template <class XX>
class ChanW_XX_Tee
:	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;

	ChanW_XX_Tee(const ChanW<XX>& iChanW0, const ChanW<XX>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
		{}

// From ChanW
	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount)
		{
		if (const size_t countWritten0 = sQWrite(iSource, iCount, fChanW0))
			{
			sWriteFully(iSource, countWritten0, fChanW1);
			return countWritten0;
			}
		else
			{
			return sQWrite(iSource, iCount, fChanW1);
			}
		}

	virtual void Flush()
		{
		sFlush(fChanW0);
		sFlush(fChanW1);
		}

protected:
	const ChanW<XX>& fChanW0;
	const ChanW<XX>& fChanW1;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Tee_h__
