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

#include <stdexcept> // For std::range_error

namespace ZooLib {

// =================================================================================================
// MARK: -

typedef ChanW<byte> ChanW_Bin;

inline void sThrowEndOfChanW()
	{ throw std::range_error("sThrowEndOfChanW"); }

// =================================================================================================
// MARK: -

inline
size_t sWrite(const void* iSource, size_t iCount, const ChanW_Bin& iChan)
	{ return sWrite(static_cast<const byte*>(iSource), iCount, iChan); }

inline
size_t sWriteFully(const void* iSource, size_t iCount, const ChanW_Bin& iChan)
	{ return sWriteFully(static_cast<const byte*>(iSource), iCount, iChan); }

template <class T>
bool sQWriteNative(const T& iT, const ChanW_Bin& iChanW)
	{
	if (sizeof(T) != sWriteFully(&iT, sizeof(T), iChanW))
		return false;
	return true;
	}

template <class T>
bool sQWriteSwapped(const T& iT, const ChanW_Bin& iChanW)
	{
	const T buf = sByteSwapped(iT);
	if (sizeof(T) != sWriteFully(&buf, sizeof(T), iChanW))
		return false;
	return true;
	}

#if ZCONFIG_Endian == ZCONFIG_Endian_Big

	template <class T>
	bool sQWriteBE(const T& iT, const ChanW_Bin& iChanW)
		{ return sQWriteNative<T>(iT, iChanW); }

	template <class T>
	bool sQWriteLE(const T& iT, const ChanW_Bin& iChanW)
		{ return sQWriteSwapped<T>(iT, iChanW); }

#else

	template <class T>
	bool sQWriteBE(const T& iT, const ChanW_Bin& iChanW)
		{ return sQWriteSwapped<T>(iT, iChanW); }

	template <class T>
	bool sQWriteLE(const T& iT, const ChanW_Bin& iChanW)
		{ return sQWriteNative<T>(iT, iChanW); }

#endif

template <class T>
bool sQWrite(const T& iT, const ChanW_Bin& iChanW)
	{ return sQWriteBE<T>(iT, iChanW); }

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_h__
