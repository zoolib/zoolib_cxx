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

#ifndef __ZooLib_ChanR_Bin_h__
#define __ZooLib_ChanR_Bin_h__ 1
#include "zconfig.h"

#include "zoolib/ByteSwap.h"
#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin

typedef ChanR<byte> ChanR_Bin;

// =================================================================================================
#pragma mark -

// Overloads of sRead that take void*, so a binary chan can read into any pointer.

inline
size_t sQRead(void* oDest, size_t iCount, const ChanR_Bin& iChan)
	{ return sQRead(static_cast<byte*>(oDest), iCount, iChan); }

inline
size_t sQReadFully(void* oDest, size_t iCount, const ChanR_Bin& iChan)
	{ return sQReadFully(static_cast<byte*>(oDest), iCount, iChan); }

inline
void sERead(void* oDest, size_t iCount, const ChanR_Bin& iChan)
	{ return sERead(static_cast<byte*>(oDest), iCount, iChan); }

// =================================================================================================
#pragma mark -

template <class T>
ZQ<T> sQReadNative(const ChanR_Bin& iChanR)
	{
	T buf;
	if (sizeof(T) != sReadFully(iChanR, &buf, sizeof(T)))
		return null;
	return buf;
	}

template <class T>
ZQ<T> sQReadSwapped(const ChanR_Bin& iChanR)
	{
	T buf;
	if (sizeof(T) != sQReadFully(&buf, sizeof(T), iChanR))
		return null;
	return sByteSwapped(buf);
	}

#if ZCONFIG_Endian == ZCONFIG_Endian_Big

	template <class T>
	ZQ<T> sQReadBE(const ChanR_Bin& iChanR)
		{ return sQReadNative<T>(iChanR); }

	template <class T>
	ZQ<T> sQReadLE(const ChanR_Bin& iChanR)
		{ return sQReadSwapped<T>(iChanR); }

#else

	template <class T>
	ZQ<T> sQReadBE(const ChanR_Bin& iChanR)
		{ return sQReadSwapped<T>(iChanR); }

	template <class T>
	ZQ<T> sQReadLE(const ChanR_Bin& iChanR)
		{ return sQReadNative<T>(iChanR); }

#endif

template <class T>
T sEReadBE(const ChanR_Bin& iChanR)
	{
	ZQ<T> theQ = sQReadBE<T>(iChanR);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

template <class T>
T sEReadLE(const ChanR_Bin& iChanR)
	{
	ZQ<T> theQ = sQReadLE<T>(iChanR);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

} // namespace ZooLib

#endif // __ZooLib_ChanR_Bin_h__
