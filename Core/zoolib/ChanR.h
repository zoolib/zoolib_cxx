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

#ifndef __ZooLib_ChanR_h__
#define __ZooLib_ChanR_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZTypes.h" // For sNonConst

#include <stdexcept> // for range_error
#include <string> // because range_error may require it

namespace ZooLib {

inline bool sThrow_ExhaustedR()
	{
	throw std::range_error("ChanR Exhausted");
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR

template <class Elmt_p>
class ChanR
:	public virtual Chan<Elmt_p>
	{
protected:
/** \name Canonical Methods
The canonical methods are protected, thus you cannot create, destroy or assign through a
ChanR reference, you must work with some derived class.
*/	//@{
	ChanR() {}
	virtual ~ChanR() {}
	ChanR(const ChanR&) {}
	ChanR& operator=(const ChanR&) { return *this; }
	//@}

public:
	typedef Elmt_p Elmt_t;
	typedef ChanR<Elmt_p> Chan_Base;

	virtual size_t QRead(Elmt_t* oDest, size_t iCount) = 0;

	virtual uint64 Skip(uint64 iCount)
		{
		// buf will have space for at least one element.
		Elmt_t buf[sStackBufferSize + sizeof(Elmt_t) - 1 / sizeof(Elmt_t)];
		return this->QRead(buf, std::min<size_t>(iCount, countof(buf)));
		}

	virtual size_t Readable()
		{ return 0; }

// For a golang-style select mechanism we'll need something where we can register
// a callable, or something, so that blocked entities can notify when they unblock.
// Something like this perhaps:
//	virtual void WhenReadable(const ZRef<Callable_Void>& iCallable)
//		{}
	};

// =================================================================================================
#pragma mark -
#pragma mark 

template <class Elmt_p>
size_t sQRead(Elmt_p* oDest, size_t iCount, const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).QRead(oDest, iCount); }

template <class Elmt_p>
uint64 sSkip(uint64 iCount, const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).Skip(iCount); }

template <class Elmt_p>
size_t sReadable(const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).Readable(); }

// =================================================================================================
#pragma mark -
#pragma mark 

template <class Elmt_p>
ZQ<Elmt_p> sQRead(const ChanR<Elmt_p>& iChanR)
	{
	Elmt_p buf;
	if (1 != sQRead(&buf, 1, iChanR))
		return null;
	return buf;
	}

template <class Elmt_p>
bool sQRead(Elmt_p& oElmt, const ChanR<Elmt_p>& iChanR)
	{ return 1 == sQRead(&oElmt, 1, iChanR); }

template <class Elmt_p>
Elmt_p sReadMust(const ChanR<Elmt_p>& iChanR)
	{
	Elmt_p buf;
	if (1 != sQRead(&buf, 1, iChanR))
		sThrow_Exhausted(iChanR);
	return buf;
	}

template <class Elmt_p>
size_t sQReadFully(Elmt_p* oDest, size_t iCount, const ChanR<Elmt_p>& iChanR)
	{
	Elmt_p* localDest = oDest;
	while (iCount)
		{
		if (const size_t countRead = sQRead(localDest, iCount, iChanR))
			{
			iCount -= countRead;
			localDest += countRead;
			}
		else
			{ break; }
		}
	return localDest - oDest;
	}

template <class Elmt_p>
uint64 sSkipFully(uint64 iCount, const ChanR<Elmt_p>& iChanR)
	{
	uint64 countRemaining = iCount;
	while (countRemaining)
		{
		if (const size_t countSkipped = sSkip(countRemaining, iChanR))
			{ countRemaining -= countSkipped; }
		else
			{ break; }
		}
	return iCount - countRemaining;
	}

template <class Elmt_p>
uint64 sSkipAll(const ChanR<Elmt_p>& iChanR)
	{
	uint64 result = 0;
	for (;;)
		{
		if (const uint64 countSkipped = sSkip(0x100000, iChanR))
			{ result += countSkipped; }
		else
			{ break; }
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Null

/// A read Chan with no content.

template <class XX>
class ChanR_XX_Null
:	public ChanR<XX>
	{
public:
	typedef XX Elmt_t;
	virtual size_t QRead(Elmt_t* oDest, size_t iCount)
		{ return 0; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
