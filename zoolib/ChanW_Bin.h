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

#ifndef __ZooLib_ChanW_Bin_h__
#define __ZooLib_ChanW_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
// MARK: -

typedef ChanW<byte> ChanW_Bin;

// =================================================================================================
// MARK: -

inline
size_t sWrite(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sWrite(iChan, static_cast<const uint8*>(iSource), iCount); }

template <class T>
bool sQWriteSwapped(const ChanW_Bin& iChanW, const T& iT)
	{
	T buf = iT;
	sByteSwap<T>(&buf);
	if (sizeof (T) != sWriteFully(iChanW, &buf, sizeof (T)))
		return false;
	return true;
	}

template <class T>
bool sQWrite(const ChanW_Bin& iChanW, const T& iT)
	{
	if (sizeof (T) != sWriteFully(iChanW, &iT, sizeof (T)))
		return false;
	return true;
	}

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_h__
