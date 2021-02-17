// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

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

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

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

// From ChanSize
	virtual uint64 Size()
		{ return fDeque.size(); }

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{ fDeque.resize(iSize); }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t count = sMin(fPosition, iCount);
		fPosition -= count;
		return count;
		}

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

protected:
	size_t pReadable()
		{ return fDeque.size() > fPosition ? fDeque.size() - fPosition : 0; }

	std::deque<EE> fDeque;
	size_t fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanRWPos_XX_RAM_h__
