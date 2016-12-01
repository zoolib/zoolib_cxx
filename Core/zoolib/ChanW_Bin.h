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
#pragma mark -

typedef ChanW<byte> ChanW_Bin;

// =================================================================================================
#pragma mark -

inline
size_t sQWrite(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sQWrite<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

inline
size_t sQWriteFully(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sQWriteFully<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

inline
void sEWrite(const ChanW_Bin& iChan, const void* iSource, size_t iCount)
	{ return sEWrite<byte>(iChan, static_cast<const byte*>(iSource), iCount); }

template <class T>
bool sQWriteNative(const ChanW_Bin& iChanW, const T& iT)
	{
	if (sizeof(T) != sWriteFully(iChanW, &iT, sizeof(T)))
		return false;
	return true;
	}

template <class T>
bool sQWriteSwapped(const ChanW_Bin& iChanW, const T& iT)
	{
	const T buf = sByteSwapped(iT);
	if (sizeof(T) != sQWriteFully(iChanW, &buf, sizeof(T)))
		return false;
	return true;
	}

#if ZCONFIG_Endian == ZCONFIG_Endian_Big

	template <class T>
	bool sQWriteBE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteNative<T>(iChanW, iT); }

	template <class T>
	bool sQWriteLE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteSwapped<T>(iChanW, iT); }

#else

	template <class T>
	bool sQWriteBE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteSwapped<T>(iChanW, iT); }

	template <class T>
	bool sQWriteLE(const ChanW_Bin& iChanW, const T& iT)
		{ return sQWriteNative<T>(iChanW, iT); }

#endif

template <class T>
void sEWriteBE(const ChanW_Bin& iChanW, const T& iT)
	{
	if (not sQWriteBE<T>(iChanW, iT))
		sThrow_ExhaustedW();
	}

template <class T>
void sEWriteLE(const ChanW_Bin& iChanW, const T& iT)
	{
	if (not sQWriteLE<T>(iChanW, iT))
		sThrow_ExhaustedW();
	}

} // namespace ZooLib

#endif // __ZooLib_ChanW_Bin_h__
