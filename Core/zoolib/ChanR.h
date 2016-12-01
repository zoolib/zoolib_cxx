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

template <class EE>
ZQ<EE> sQRead(const ChanR<EE>& iChanR)
	{
	EE buf;
	if (1 != sQRead(iChanR, &buf, 1))
		return null;
	return buf;
	}

// This is still used in HTTP and Util_Chan_UTF
template <class EE>
bool sQRead(const ChanR<EE>& iChanR, EE& oElmt)
	{ return 1 == sQRead(iChanR, &oElmt, 1); }

template <class EE>
EE sERead(const ChanR<EE>& iChanR)
	{
	EE buf;
	if (1 != sQRead(&buf, 1, iChanR))
		sThrow_Exhausted(iChanR);
	return buf;
	}

template <class EE>
size_t sQReadFully(const ChanR<EE>& iChanR, EE* oDest, size_t iCount)
	{
	EE* localDest = oDest;
	while (iCount)
		{
		if (const size_t countRead = sQRead(iChanR, localDest, iCount))
			{
			iCount -= countRead;
			localDest += countRead;
			}
		else
			{
			break;
			}
		}
	return localDest - oDest;
	}

template <class EE>
uint64 sSkipFully(const ChanR<EE>& iChanR, uint64 iCount)
	{
	uint64 countRemaining = iCount;

	// We need the 64->32 bit clamping stuff here
	while (countRemaining)
		{
		if (const size_t countSkipped = sSkip(iChanR, countRemaining))
			countRemaining -= countSkipped;
		else
			break;
		}

	return iCount - countRemaining;
	}

template <class EE>
uint64 sSkipAll(const ChanR<EE>& iChanR)
	{
	uint64 result = 0;
	while (const uint64 countSkipped = sSkip(iChanR, 0x100000))
		result += countSkipped;
	return result;
	}

template <class EE>
void sERead(const ChanR<EE>& iChanR, EE* oDest, size_t iCount)
	{
	if (iCount != sQReadFully<EE>(iChanR, oDest, iCount))
		sThrow_ExhaustedR();
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_XX_Null

/// A read Chan with no content.

template <class EE>
class ChanR_XX_Null
:	public ChanR<EE>
	{
public:
	virtual size_t QRead(EE* oDest, size_t iCount)
		{ return 0; }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
