/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_ChanRWPos_XX_RAM_h__
#define __ZooLib_ChanRWPos_XX_RAM_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include <deque>

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRWPos_XX_RAM

/// A RWPos stream that maintains its data in memory.

template <class EE>
class ChanRWPos_XX_RAM
:	public ChanRWPos<EE>
	{
public:
	ChanRWPos_XX_RAM()
	:	fPosition(0)
		{}

	~ChanRWPos_XX_RAM()
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		if (const size_t count = sMin<size_t>(iCount, pReadable()))
			{
			const typename std::deque<EE>::const_iterator ii = fDeque.begin() + fPosition;
			copy(ii, ii + count, oDest);
			fPosition += count;
			return count;
			}
		return 0;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const size_t count = sMin<size_t>(iCount, pReadable());
		fPosition += count;
		return count;
		}

	virtual size_t Readable()
		{ return pReadable(); }

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		if (iCount)
			{
			if (fPosition > fDeque.size())
				{
				// fPosition is past the end of the deque, so grow it with unassigned values
				fDeque.resize(fPosition);
				fDeque.insert(fDeque.end(), iSource, iSource + iCount);
				}
			else
				{
				const size_t countToCopy = sMin<size_t>(iCount, pReadable());
				std::copy(iSource, iSource + countToCopy, fDeque.begin() + fPosition);
				fDeque.insert(fDeque.end(), iSource + countToCopy, iSource + iCount);
				}
			fPosition += iCount;
			}
		return iCount;
		}

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanSize
	virtual uint64 Size()
		{ return fDeque.size(); }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t count = sMin(fPosition, iCount);
		fPosition -= count;
		return count;
		}

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{ fDeque.resize(iSize); }

protected:
	size_t pReadable()
		{ return fDeque.size() > fPosition ? fDeque.size() - fPosition : 0; }

	std::deque<EE> fDeque;
	size_t fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRWPos_XX_RAM_h__
