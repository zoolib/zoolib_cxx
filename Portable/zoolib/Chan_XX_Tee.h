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

template <class EE>
class ChanR_XX_Tee
:	public ChanR<EE>
	{
public:
	ChanR_XX_Tee(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	:	fChanR(iChanR)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		// We have to read into a local buffer because we're going to pass
		// what we read to fChanW, and oDest could reference memory that's
		// not safe to read (the garbage buffer, for example).

		EE buf[std::min<size_t>(iCount, sStackBufferSize / sizeof(EE))];

		if (const size_t countRead = sRead(fChanR, buf, std::min(iCount, countof(buf))))
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

	virtual size_t Readable()
		{ return sReadable(fChanR); }

protected:
	const ChanR<EE>& fChanR;
	const ChanW<EE>& fChanW;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_XX_Tee

template <class EE>
class ChanW_XX_Tee
:	public ChanW<EE>
	{
public:
	ChanW_XX_Tee(const ChanW<EE>& iChanW0, const ChanW<EE>& iChanW1)
	:	fChanW0(iChanW0)
	,	fChanW1(iChanW1)
		{}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		if (const size_t countWritten0 = sWrite(iSource, iCount, fChanW0))
			{
			sWriteFully(iSource, countWritten0, fChanW1);
			return countWritten0;
			}
		else
			{
			return sWrite(iSource, iCount, fChanW1);
			}
		}

	virtual void Flush()
		{
		sFlush(fChanW0);
		sFlush(fChanW1);
		}

protected:
	const ChanW<EE>& fChanW0;
	const ChanW<EE>& fChanW1;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Tee_h__
