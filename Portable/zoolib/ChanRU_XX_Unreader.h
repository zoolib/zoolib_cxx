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

#ifndef __ZooLib_ChanRU_XX_Unreader_h__
#define __ZooLib_ChanRU_XX_Unreader_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRU_XX_Unreader

template <class EE>
class ChanRU_XX_Unreader
:	public ChanRU<EE>
	{
public:
	ChanRU_XX_Unreader(const ChanR<EE>& iChanR)
	:	fChanR(iChanR)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		EE* localDest = oDest;
		EE* localDestEnd = oDest + iCount;
		while (localDest < localDestEnd)
			{
			if (fStack.empty())
				{
				localDest += sRead(fChanR, localDest, localDestEnd - localDest);
				break;
				}
			else
				{
				*localDest++ = fStack.back();
				fStack.pop_back();
				}
			}

		return localDest - oDest;
		}

	virtual size_t Readable()
		{ return fStack.size() + sReadable(fChanR); }

// From ChanU
	virtual void Unread(const EE* iSource, size_t iCount)
		{
		while (iCount--)
			fStack.push_back(*iSource++);
		}

protected:
	const ChanR<EE>& fChanR;
	std::vector<EE> fStack;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRU_XX_Unreader_h__
