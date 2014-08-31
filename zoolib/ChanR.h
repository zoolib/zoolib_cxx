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
#include "zoolib/ZStdInt.h" // For size_t, uint64
#include "zoolib/ZTypes.h" // For sNonConst

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR

template <class Elmt_p>
class ChanR
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
	typedef Elmt_p Elmt;

	virtual size_t Read(Elmt* oDest, size_t iCount)
		{ return 0; }

	virtual uint64 Skip(uint64 iCount)
		{
		Elmt buf[std::min<size_t>(iCount, sStackBufferSize / sizeof(Elmt))];
		return this->Read(buf, std::min<size_t>(iCount, countof(buf)));
		}

	virtual size_t Readable()
		{ return 0; }

// For a golang-style select mechanism we'll need an API whereby we can efficiently
// wait for readability on multiple channels at once.

//	virtual bool WaitReadable(double iTimeout) // ??
//		{
//		ZThread::sSleep(iTimeout);
//		return false;
//		}
	};

// =================================================================================================
// MARK: -

template <class Elmt_p>
size_t sRead(Elmt_p* oDest, size_t iCount, const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).Read(oDest, iCount); }

template <class Elmt_p>
uint64 sSkip(uint64 iCount, const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).Skip(iCount); }

template <class Elmt_p>
size_t sReadable(const ChanR<Elmt_p>& iChanR)
	{ return sNonConst(iChanR).Readable(); }

// =================================================================================================
// MARK: -

template <class Elmt_p>
ZQ<Elmt_p> sQRead(const ChanR<Elmt_p>& iChanR)
	{
	Elmt_p buf;
	if (not sRead(iChanR, &buf, 1))
		return null;
	return buf;
	}

template <class Elmt_p>
size_t sReadFully(Elmt_p* oDest, size_t iCount, const ChanR<Elmt_p>& iChanR)
	{
	Elmt_p* localDest = oDest;
	while (iCount)
		{
		if (const size_t countRead = sRead(localDest, iCount, iChanR))
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

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
