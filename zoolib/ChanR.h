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

#include "zoolib/ZQ.h"
#include "zoolib/ZStdInt.h" // For size_t, uint64

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR

template <class Elmt_p>
class ChanR
	{
public:
	typedef Elmt_p Elmt;

	virtual size_t Read(Elmt* iDest, size_t iCount)
		{ return 0; }

	virtual uint64 Skip(uint64 iCount)
		{
		const uint64 kBufSize = 4096 / sizeof(Elmt);
		Elmt scratch[kBufSize];
		return this->Read(&scratch, std::min(iCount, kBufSize));
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
size_t sRead(const ChanR<Elmt_p>& iChanR, Elmt_p* iDest, size_t iCount)
	{ return const_cast<ChanR<Elmt_p>&>(iChanR).Read(iDest, iCount); }

template <class Elmt_p>
uint64 sSkip(const ChanR<Elmt_p>& iChanR, uint64 iCount)
	{ return const_cast<ChanR<Elmt_p>&>(iChanR).Skip(iCount); }

template <class Elmt_p>
size_t sReadable(const ChanR<Elmt_p>& iChanR)
	{ return const_cast<ChanR<Elmt_p>&>(iChanR).Readable(); }

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
size_t sReadFully(const ChanR<Elmt_p>& iChanR, Elmt_p* iDest, size_t iCount)
	{
	Elmt_p* localDest = iDest;
	while (iCount)
		{
		if (const size_t countRead = sRead(iChanR, localDest, iCount))
			{
			iCount -= countRead;
			localDest += countRead;
			}
		else
			{ break; }
		}
	return localDest - iDest;
	}

template <class Elmt_p>
uint64 sSkipFully(const ChanR<Elmt_p>& iChanR, uint64 iCount)
	{
	uint64 countRemaining = iCount;
	while (countRemaining)
		{
		if (const size_t countSkipped = sSkip(iChanR, countRemaining))
			{ countRemaining -= countSkipped; }
		else
			{ break; }
		}
	return iCount - countRemaining;
	}

//template <class Elmt_p>
//class ChannerR_Indirect<Elmt_p>
//:	public ChannerR<Elmt_p
//	{
//
//	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
