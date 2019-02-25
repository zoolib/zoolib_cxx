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
#include "zoolib/DeriveFrom.h"

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

using ChanAbort = DeriveFrom<ChanAspect_Abort>;

using ChanDisconnectRead = DeriveFrom<ChanAspect_DisconnectRead>;

using ChanDisconnectWrite = DeriveFrom<ChanAspect_DisconnectWrite>;

using ChanPos = DeriveFrom<ChanAspect_Pos>;

template <typename EE>
using ChanR = DeriveFrom<ChanAspect_Read<EE>>;

template <typename LL, typename EE>
using ChanReadAt = DeriveFrom<ChanAspect_ReadAt<LL,EE>>;

using ChanSize = DeriveFrom<ChanAspect_Size>;

using ChanSizeSet = DeriveFrom<ChanAspect_SizeSet>;

template <typename EE>
using ChanU = DeriveFrom<ChanAspect_Unread<EE>>;

using ChanWaitReadable = DeriveFrom<ChanAspect_WaitReadable>;

template <typename EE>
using ChanW = DeriveFrom<ChanAspect_Write<EE>>;

// ---

template <typename EE>
using ChanRU = DeriveFrom
	<
	ChanAspect_Read<EE>,
	ChanAspect_Unread<EE>
	>;

template <typename EE>
using ChanRPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_Read<EE>,
	ChanAspect_Size,
	ChanAspect_Unread<EE>
	>;

template <typename EE>
using ChanWPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_Size,
	ChanAspect_SizeSet,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRWPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_Read<EE>,
	ChanAspect_Size,
	ChanAspect_SizeSet,
	ChanAspect_Unread<EE>,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRW = DeriveFrom
	<
	ChanAspect_Read<EE>,
	ChanAspect_WaitReadable,
	ChanAspect_Write<EE>
	>;

using ChanClose = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_DisconnectRead,
	ChanAspect_DisconnectWrite
	>;

template <typename EE>
using ChanRAbort = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_Read<EE>,
	ChanAspect_WaitReadable
	>;

template <typename EE>
using ChanWAbort = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRWAbort = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_Read<EE>,
	ChanAspect_WaitReadable,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRCon = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_DisconnectRead,
	ChanAspect_Read<EE>,
	ChanAspect_WaitReadable
	>;

template <typename EE>
using ChanWCon = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_DisconnectWrite,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRWCon = DeriveFrom
	<
	ChanAspect_Abort,
	ChanAspect_DisconnectRead,
	ChanAspect_DisconnectWrite,
	ChanAspect_Read<EE>,
	ChanAspect_WaitReadable,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanConnection = ChanRWCon<EE>;

} // namespace ZooLib

#endif // __ZooLib_Chan_h__
