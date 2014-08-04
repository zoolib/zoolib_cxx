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

#ifndef __ZooLib_ChanRBin_h__
#define __ZooLib_ChanRBin_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

typedef uint8 byte;

typedef ChanR<byte> ChanRBin;

// =================================================================================================
// MARK: -

size_t sReadVoid(const ChanRBin& iChan, void* oDest, size_t iCount)
	{ return sRead(iChan, static_cast<uint8*>(oDest), iCount; }

size_t sReadVoidFully(const ChanRBin& iChan, void* oDest, size_t iCount)
	{ return sReadFully(iChan, static_cast<uint8*>(oDest), iCount; }

template <class T, bool Swapped>
ZQ<T> sQReadSwapped(const ChanRBin& iChanR);

template <class T>
ZQ<T> sQReadSwapped<T,true>(const ChanRBin& iChanR)
	{
	T buf;
	if (sizeof T != sReadAll(iChanR, &buf, sizeof T))
		return null;

	sByteSwap<sizeof T>(&buf);

	return buf;
	}

template <class T>
ZQ<T> sQReadSwapped<T,false>(const ChanRBin& iChanR)
	{
	T buf;
	if (sizeof T != sReadAll(iChanR, &buf, sizeof T))
		return null;
	return buf;
	}

template <class T, bool BigEndian=true>
ZQ<T> sQRead(const ChanRBin& iChanR)
	{ return sQReadSwapped<T,BigEndian != (ZCONFIG_Endian == ZCONFIG_Endian_Big)>(iChanR); }

} // namespace ZooLib

#endif // __ZooLib_ChanRBin_h__
