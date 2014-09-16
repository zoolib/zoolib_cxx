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

#ifndef __ZooLib_Chan_XX_Unreader_h__
#define __ZooLib_Chan_XX_Unreader_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanU.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - Chan_XX_Unreader

template <class XX>
class Chan_XX_Unreader
:	public ChanR<XX>
,	public ChanU<XX>
	{
public:
	typedef XX Elmt_t;

	Chan_XX_Unreader(const ChanR<Elmt_t>& iChanR)
	:	fChanR(iChanR)
		{}

// From ChanR
	virtual size_t Read(Elmt_t* oDest, size_t iCount)
		{
		Elmt_t* localDest = oDest;
		Elmt_t* localDestEnd = oDest + iCount;
		while (localDest < localDestEnd)
			{
			if (fStack.empty())
				{
				size_t countRead = sRead(localDest, localDestEnd - localDest, fChanR);
				if (countRead == 0)
					break;
				localDest += countRead;
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

	virtual size_t Unread(const Elmt_t* iSource, size_t iCount)
		{
		const size_t theCount = iCount;
		while (--iCount)
			fStack.push_back(*iSource++);
		return theCount;
		}

	virtual size_t UnreadableLimit()
		{ return size_t(-1); }

protected:
	const ChanR<Elmt_t>& fChanR;
	std::vector<Elmt_t> fStack;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Memory_h__
