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

#ifndef __ZooLib_Chan_XX_Memory_h__
#define __ZooLib_Chan_XX_Memory_h__ 1
#include "zconfig.h"

#include "zoolib/ChanCount.h"
#include "zoolib/ChanCountSet.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanBase_XX_Memory

template <class XX>
class ChanBase_XX_Memory
:	public ChanR<XX>
,	public ChanCount
,	public ChanPos
	{
public:
	typedef XX Elmt_t;

	ChanBase_XX_Memory(const void* iAddress, size_t iCount)
	:	fAddress(static_cast<const byte*>(iAddress))
	,	fCount(iCount)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t QRead(Elmt_t* oDest, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount,
			fCount > fPosition ? fCount - fPosition : 0);
		const Elmt_t* source = static_cast<const Elmt_t*>(fAddress) + fPosition;
		std::copy(source, source + countToCopy, oDest);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{ return fCount >= fPosition ? fCount - fPosition : 0; }

// From ChanGetCount
	virtual uint64 Count()
		{ return fCount; }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void SetPos(uint64 iPos)
		{ fPosition = iPos; }

protected:
	const void* fAddress;
	size_t fCount;
	uint64 fPosition;
	};

// =================================================================================================
// MARK: - ChanRPos_XX_Memory

template <class XX>
class ChanRPos_XX_Memory
:	public ChanBase_XX_Memory<XX>
,	public ChanU<XX>
	{
public:
	typedef XX Elmt_t;

	ChanRPos_XX_Memory(const void* iAddress, size_t iCount)
	:	ChanBase_XX_Memory<XX>(iAddress, iCount)
		{}

// From ChanU
	virtual size_t Unread(const Elmt_t* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min(iCount, this->fPosition);

		if (false)
			{
			// If this code is enabled, then we assert that what's being
			// unread matches what was in here already.

			const Elmt_t* dest = static_cast<const Elmt_t*>(this->fAddress)
				+ this->fPosition - countToCopy;

			for (const Elmt_t* last = iSource + countToCopy; iSource != last; /*no inc*/)
				{
				ZAssert(*iSource == *dest);
				++iSource;
				++dest;
				}
			}

		this->fPosition -= countToCopy;

		return countToCopy;
		}

	virtual size_t UnreadableLimit()
		{ return this->fPosition; }
	};

// =================================================================================================
// MARK: - ChanRWPos_XX_Memory

template <class XX>
class ChanRWPos_XX_Memory
:	public ChanBase_XX_Memory<XX>
,	public ChanU<XX>
,	public ChanW<XX>
,	public ChanCountSet
	{
public:
	typedef XX Elmt_t;

	ChanRWPos_XX_Memory(void* iAddress, size_t iCount, size_t iCapacity)
	:	ChanBase_XX_Memory<XX>(iAddress, iCount)
	,	fCapacity(iCapacity)
		{}

// From ChanU
	virtual size_t Unread(const Elmt_t* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount, this->fPosition);

		Elmt_t* dest = static_cast<Elmt_t*>(sNonConst(this->fAddress))
			+ this->fPosition - countToCopy;

		std::copy(iSource, iSource + countToCopy, dest);

		this->fPosition -= countToCopy;

		return countToCopy;
		}

	virtual size_t UnreadableLimit()
		{ return this->fPosition; }

// From ChanW
	virtual size_t QWrite(const Elmt_t* iSource, size_t iCount)
		{
		Elmt_t* dest = static_cast<Elmt_t*>(sNonConst(this->fAddress)) + this->fPosition;

		this->fCount = std::min(fCapacity, std::max<size_t>(this->fCount, this->fPosition + iCount));

		const size_t countToCopy = std::min<size_t>(iCount,
			this->fCount > this->fPosition ? this->fCount - this->fPosition : 0);

		std::copy(iSource, iSource + countToCopy, dest);

		this->fPosition += countToCopy;

		return countToCopy;
		}

// From ChanCountSet
	virtual void CountSet(uint64 iCount)
		{
		if (fCapacity < iCount)
			sThrowBadCount();
		this->fCount = iCount;
		}

protected:
	const size_t fCapacity;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Memory_h__
