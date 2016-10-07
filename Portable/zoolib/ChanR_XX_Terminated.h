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

#ifndef __ZooLib_Chan_XX_Terminated_h__
#define __ZooLib_Chan_XX_Terminated_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Terminated

template <class EE>
class ChanR_XX_Terminated
:	public ChanR<EE>
	{
public:
	ChanR_XX_Terminated(EE iTerminator, const ChanR<EE>& iChanR)
	:	fTerminator(iTerminator)
	,	fChanR(iChanR)
	,	fHitTerminator(false)
		{}

// From ChanR
	virtual size_t QRead(EE* oDest, size_t iCount)
		{
		size_t countRemaining = iCount;
		while (not fHitTerminator && countRemaining)
			{
			if (NotQ<EE> theQ = sQRead(fChanR))
				{
				break;
				}
			else
				{
				if (*theQ == fTerminator)
					{
					fHitTerminator = true;
					break;
					}
				*oDest++ = *theQ;
				--countRemaining;
				}
			}
		return iCount - countRemaining;
		}

	virtual size_t Readable()
		{ return fHitTerminator ? 0 : sReadable(fChanR); }

// Our protocol
	bool HitTerminator()
		{ return fHitTerminator; }

	void Reset()
		{ fHitTerminator = false; }

protected:
	const EE fTerminator;
	const ChanR<EE>& fChanR;
	bool fHitTerminator;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Terminated_h__
