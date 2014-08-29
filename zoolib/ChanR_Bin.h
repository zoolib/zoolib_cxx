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
// MARK: -

typedef ChanR<byte> ChanR_Bin;

// =================================================================================================
// MARK: -

// Overloads of sRead that take void*, so a binary chan can read into any pointer.

inline
size_t sRead(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ return sRead(iChan, static_cast<uint8*>(oDest), iCount); }

inline
size_t sReadFully(const ChanR_Bin& iChan, void* oDest, size_t iCount)
	{ return sReadFully(iChan, static_cast<uint8*>(oDest), iCount); }

// =================================================================================================
// MARK: -

template <class T>
ZQ<T> sQReadSwapped(const ChanR_Bin& iChanR)
	{
	T buf;
	if (sizeof (T) != sReadFully(iChanR, &buf, sizeof (T)))
		return null;
	return buf;
	}

// Overload of sQRead, which returns an arbitrary type from a binary channel, byteswapped
// if requested.  This replaces all the ReadInt32(), ReadInt64LE() stuff in ZStream.

template <class T, bool BigEndian=false>
ZQ<T> sQRead(const ChanR_Bin& iChanR)
	{
	if (BigEndian != (ZCONFIG_Endian == ZCONFIG_Endian_Big))
		return sQReadSwapped<T>(iChanR);

	// This is an inline copy of source from sQRead<T> (version with no endian param), because
	// otherwise
	uint8 buf;
	if (not sRead(iChanR, &buf, 1))
		return null;
	return buf;
	}

} // namespace ZooLib

#endif // __ZooLib_ChanR_Bin_h__
