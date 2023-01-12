// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_XX_Memory_h__
#define __ZooLib_Chan_XX_Memory_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/ChanW.h" // For sThrow_ExhaustedW

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanBase_XX_Memory

template <class EE>
class ChanRPos_XX_Memory
:	public virtual ChanRPos<EE>
	{
public:
	ChanRPos_XX_Memory(const PaC<const EE>& iPaC)
	:	fAddress(iPaC.first)
	,	fSize(iPaC.second)
	,	fPos(0)
		{}

	ChanRPos_XX_Memory(const void* iAddress, size_t iSize)
	:	fAddress(static_cast<const EE*>(iAddress))
	,	fSize(iSize)
	,	fPos(0)
		{}

// From ChanPos
	virtual uint64 Pos()
		{ return fPos; }

// From ChanPosSet
	virtual void PosSet(uint64 iPos)
		{ fPos = iPos; }

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countToCopy =
			sClamped(std::min<uint64>(iCount, fSize > fPos ? fSize - fPos : 0));
		std::copy_n(fAddress + fPos, countToCopy, oDest);
		fPos += countToCopy;
		return countToCopy;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		uint64 countToSkip = std::min<uint64>(iCount, fSize > fPos ? fSize - fPos : 0);
		fPos += countToSkip;
		return countToSkip;
		}

	virtual size_t Readable()
		{ return sClamped(fSize >= fPos ? fSize - fPos : 0); }

// From ChanSize
	virtual uint64 Size()
		{ return fSize; }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t countToCopy = sClamped(std::min<uint64>(iCount, fPos));

		if (false)
			{
			// If this code is enabled, then we assert that what's being
			// unread matches what was in here already.

			const EE* dest = fAddress + fPos - countToCopy;

			for (const EE* last = iSource + countToCopy; iSource != last; /*no inc*/)
				{
				ZAssert(*iSource == *dest);
				++iSource;
				++dest;
				}
			}

		fPos -= countToCopy;

		return countToCopy;
		}

protected:
	const EE* fAddress;
	const size_t fSize;
	uint64 fPos;
	};

// =================================================================================================
#pragma mark - ChanRWPos_XX_Memory

template <class EE>
class ChanRWPos_XX_Memory
:	public virtual ChanRWPos<EE>
	{
public:
	ChanRWPos_XX_Memory(const PaC<EE>& iPaC)
	:	fAddress(iPaC.first)
	,	fSize(iPaC.second)
	,	fCapacity(iPaC.second)
	,	fPos(0)
		{}

	ChanRWPos_XX_Memory(void* iAddress, size_t iSize, size_t iCapacity)
	:	fAddress(static_cast<EE*>(iAddress))
	,	fSize(iSize)
	,	fCapacity(iCapacity)
	,	fPos(0)
		{}

// From ChanPos
	virtual uint64 Pos()
		{ return fPos; }

// From ChanPosSet
	virtual void PosSet(uint64 iPos)
		{ fPos = iPos; }

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countToCopy =
			sClamped(std::min<uint64>(iCount, fSize > fPos ? fSize - fPos : 0));
		std::copy_n(fAddress + fPos, countToCopy, oDest);
		fPos += countToCopy;
		return countToCopy;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		uint64 countToSkip = std::min<uint64>(iCount, fSize > fPos ? fSize - fPos : 0);
		fPos += countToSkip;
		return countToSkip;
		}

	virtual size_t Readable()
		{ return sClamped(fSize >= fPos ? fSize - fPos : 0); }

// From ChanSize
	virtual uint64 Size()
		{ return fSize; }

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{
		if (fCapacity < iSize)
			sThrow_ExhaustedW();
		fSize = iSize;
		}

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t countToCopy = sClamped(std::min<uint64>(iCount, fPos));

		EE* dest = fAddress + fPos - countToCopy;

		std::copy_n(iSource, countToCopy, dest);

		fPos -= countToCopy;

		return countToCopy;
		}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		fSize = std::min(fCapacity, std::max<size_t>(fSize, sClamped(fPos + iCount)));

		const size_t countToCopy =
			sClamped(std::min<uint64>(iCount, fSize > fPos ? fSize - fPos : 0));

		std::copy_n(iSource, countToCopy, fAddress + fPos);

		fPos += countToCopy;

		return countToCopy;
		}

protected:
	EE* fAddress;
	size_t fSize;
	const size_t fCapacity;
	uint64 fPos;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Memory_h__
