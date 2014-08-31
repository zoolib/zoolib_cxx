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

#include "zoolib/ChanGetLength.h"
#include "zoolib/ChanSetLength.h"
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
,	public ChanGetLength
,	public ChanPos
	{
public:
	typedef XX Elmt;

	ChanBase_XX_Memory(const void* iAddress, size_t iSize)
	:	fAddress(static_cast<const byte*>(iAddress))
	,	fSize(iSize)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t Read(Elmt* oDest, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount,
			fSize > fPosition ? fSize - fPosition : 0);
		const Elmt* source = static_cast<const Elmt*>(fAddress) + fPosition;
		std::copy(source, source + countToCopy, oDest);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{ return fSize >= fPosition ? fSize - fPosition : 0; }

// From ChanGetLength
	virtual uint64 GetLength()
		{ return fSize; }

// From ChanPos
	virtual uint64 GetPos()
		{ return fPosition; }

	virtual void SetPos(uint64 iPos)
		{ fPosition = iPos; }

protected:
	const void* fAddress;
	size_t fSize;
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
	typedef XX Elmt;

	ChanRPos_XX_Memory(const void* iAddress, size_t iSize)
	:	ChanBase_XX_Memory<XX>(iAddress, iSize)
		{}

// From ChanU
	virtual size_t Unread(const Elmt* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min(iCount, this->fPosition);

		if (false)
			{
			// If this code is enabled, then we assert that what's being
			// unread matches what was in here already.

			const Elmt* dest = static_cast<const Elmt*>(this->fAddress)
				+ this->fPosition - countToCopy;

			for (const Elmt* last = iSource + countToCopy; iSource != last; /*no inc*/)
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
,	public ChanSetLength
	{
public:
	typedef XX Elmt;

	ChanRWPos_XX_Memory(void* iAddress, size_t iSize, size_t iCapacity)
	:	ChanBase_XX_Memory<XX>(iAddress, iSize)
	,	fCapacity(iCapacity)
		{}

// From ChanU
	virtual size_t Unread(const Elmt* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount, this->fPosition);

		Elmt* dest = static_cast<Elmt*>(sNonConst(this->fAddress))
			+ this->fPosition - countToCopy;

		std::copy(iSource, iSource + countToCopy, dest);

		this->fPosition -= countToCopy;

		return countToCopy;
		}

	virtual size_t UnreadableLimit()
		{ return this->fPosition; }

// From ChanW
	virtual size_t Write(const Elmt* iSource, size_t iCount)
		{
		Elmt* dest = static_cast<Elmt*>(sNonConst(this->fAddress)) + this->fPosition;

		this->fSize = std::min(fCapacity, std::max<size_t>(this->fSize, this->fPosition + iCount));

		const size_t countToCopy = std::min<size_t>(iCount,
			this->fSize > this->fPosition ? this->fSize - this->fPosition : 0);

		std::copy(iSource, iSource + countToCopy, dest);

		this->fPosition += countToCopy;

		return countToCopy;
		}

// From ChanSetLength
	virtual void SetLength(uint64 iLength)
		{
		if (fCapacity < iLength)
			sThrowBadLength();
		this->fSize = iLength;
		}

protected:
	const size_t fCapacity;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_Memory_h__
