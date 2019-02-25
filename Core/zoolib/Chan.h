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

#ifndef __ZooLib_Chan_h__
#define __ZooLib_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ChanAspects.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sClamped

inline size_t sClamped(uint64 iVal)
	{
	if (sizeof(size_t) < sizeof(uint64))
		return size_t(std::min(iVal, uint64(size_t(-1))));
	else
		return size_t(iVal);
	}

inline uint64 sSubtractSaturated(uint64 iLHS, uint64 iRHS)
	{ return (iLHS - iRHS) & -(iRHS <= iLHS); }

inline uint64 sClampedAvailable(uint64 iSize, uint64 iPosition)
	{ return sSubtractSaturated(iSize, iPosition); }

inline uint64 sClamped(uint64 iCount, uint64 iSize, uint64 iPosition)
	{ return std::min(iCount, sClampedAvailable(iSize, iPosition)); }

// =================================================================================================
#pragma mark - Composites

using ChanAbort = DeriveFrom<Aspect_Abort>;

using ChanDisconnectRead = DeriveFrom<Aspect_DisconnectRead>;

using ChanDisconnectWrite = DeriveFrom<Aspect_DisconnectWrite>;

using ChanPos = DeriveFrom<Aspect_Pos>;

template <typename EE>
using ChanR = DeriveFrom<Aspect_Read<EE>>;

template <typename LL, typename EE>
using ChanReadAt = DeriveFrom<Aspect_ReadAt<LL,EE>>;

using ChanSize = DeriveFrom<Aspect_Size>;

using ChanSizeSet = DeriveFrom<Aspect_SizeSet>;

template <typename EE>
using ChanU = DeriveFrom<Aspect_Unread<EE>>;

using ChanWaitReadable = DeriveFrom<Aspect_WaitReadable>;

template <typename EE>
using ChanW = DeriveFrom<Aspect_Write<EE>>;

// ---

template <typename EE>
using ChanRU = DeriveFrom
	<
	Aspect_Read<EE>,
	Aspect_Unread<EE>
	>;

template <typename EE>
using ChanRPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Read<EE>,
	Aspect_Size,
	Aspect_Unread<EE>
	>;

template <typename EE>
using ChanWPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Size,
	Aspect_SizeSet,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanRWPos = DeriveFrom
	<
	Aspect_Pos,
	Aspect_Read<EE>,
	Aspect_Size,
	Aspect_SizeSet,
	Aspect_Unread<EE>,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanRW = DeriveFrom
	<
	Aspect_Read<EE>,
	Aspect_WaitReadable,
	Aspect_Write<EE>
	>;

using ChanClose = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectRead,
	Aspect_DisconnectWrite
	>;

template <typename EE>
using ChanRAbort = DeriveFrom
	<
	Aspect_Abort,
	Aspect_Read<EE>,
	Aspect_WaitReadable
	>;

template <typename EE>
using ChanWAbort = DeriveFrom
	<
	Aspect_Abort,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanRWAbort = DeriveFrom
	<
	Aspect_Abort,
	Aspect_Read<EE>,
	Aspect_WaitReadable,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanRCon = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectRead,
	Aspect_Read<EE>,
	Aspect_WaitReadable
	>;

template <typename EE>
using ChanWCon = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectWrite,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanRWCon = DeriveFrom
	<
	Aspect_Abort,
	Aspect_DisconnectRead,
	Aspect_DisconnectWrite,
	Aspect_Read<EE>,
	Aspect_WaitReadable,
	Aspect_Write<EE>
	>;

template <typename EE>
using ChanConnection = ChanRWCon<EE>;

} // namespace ZooLib

#endif // __ZooLib_Chan_h__
