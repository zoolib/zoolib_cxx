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
:	public ChanRPos<EE>
	{
public:
	ChanRPos_XX_Memory(const PaC<const EE>& iPaC)
	:	fAddress(iPaC.first)
	,	fSize(iPaC.second)
	,	fPosition(0)
		{}

	ChanRPos_XX_Memory(const void* iAddress, size_t iSize)
	:	fAddress(static_cast<const EE*>(iAddress))
	,	fSize(iSize)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount,
			fSize > fPosition ? fSize - fPosition : 0);
		std::copy_n(fAddress + fPosition, countToCopy, oDest);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual uint64 Skip(uint64 iCount)
		{
		const size_t countToCopy = std::min<size_t>(sClamped(iCount),
			fSize > fPosition ? fSize - fPosition : 0);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{ return fSize >= fPosition ? fSize - fPosition : 0; }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanSize
	virtual uint64 Size()
		{ return fSize; }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount, fPosition);

		if (false)
			{
			// If this code is enabled, then we assert that what's being
			// unread matches what was in here already.

			const EE* dest = fAddress + fPosition - countToCopy;

			for (const EE* last = iSource + countToCopy; iSource != last; /*no inc*/)
				{
				ZAssert(*iSource == *dest);
				++iSource;
				++dest;
				}
			}

		fPosition -= countToCopy;

		return countToCopy;
		}

protected:
	const EE* fAddress;
	const size_t fSize;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark - ChanRWPos_XX_Memory

template <class EE>
class ChanRWPos_XX_Memory
:	public ChanRWPos<EE>
	{
public:
	ChanRWPos_XX_Memory(const PaC<EE>& iPaC)
	:	fAddress(iPaC.first)
	,	fSize(iPaC.second)
	,	fCapacity(iPaC.second)
	,	fPosition(0)
		{}

	ChanRWPos_XX_Memory(void* iAddress, size_t iSize, size_t iCapacity)
	:	fAddress(static_cast<EE*>(iAddress))
	,	fSize(iSize)
	,	fCapacity(iCapacity)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(sClamped(iCount),
			fSize > fPosition ? fSize - fPosition : 0);
		std::copy_n(fAddress + fPosition, countToCopy, oDest);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Skip(uint64 iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount,
			fSize > fPosition ? fSize - fPosition : 0);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{ return fSize >= fPosition ? fSize - fPosition : 0; }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanU
	virtual size_t Unread(const EE* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount, fPosition);

		EE* dest = fAddress + fPosition - countToCopy;

		std::copy_n(iSource, countToCopy, dest);

		fPosition -= countToCopy;

		return countToCopy;
		}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		EE* dest = fAddress + fPosition;

		fSize = std::min(fCapacity, std::max<size_t>(fSize, fPosition + iCount));

		const size_t countToCopy = std::min<size_t>(iCount,
			fSize > fPosition ? fSize - fPosition : 0);

		std::copy_n(iSource, countToCopy, dest);

		fPosition += countToCopy;

		return countToCopy;
		}

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

protected:
	EE* fAddress;
	size_t fSize;
	const size_t fCapacity;
	uint64 fPosition;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Memory_h__
