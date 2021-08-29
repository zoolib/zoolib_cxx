// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_h__
#define __ZooLib_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ChanAspect.h"
#include "zoolib/DeriveFrom.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanOfChan

// Gets the DeriveFrom of Chan_p
template <class Chan_p> using ChanOfChan =
	typename ZooLib::AsDeriveFrom<typename Chan_p::AsTypeList_t>::Result_t;

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

template <typename LL, typename EE>
using ChanWriteAt = DeriveFrom<ChanAspect_WriteAt<LL,EE>>;

// ---

using ChanPosAndSet = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_PosSet
	>;

template <typename EE>
using ChanRU = DeriveFrom
	<
	ChanAspect_Read<EE>,
	ChanAspect_Unread<EE>
	>;

template <typename EE>
using ChanRSize = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_Read<EE>,
	ChanAspect_Size
	>;

template <typename EE>
using ChanRPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_PosSet,
	ChanAspect_Read<EE>,
	ChanAspect_Size,
	ChanAspect_Unread<EE>
	>;

template <typename EE>
using ChanWPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_PosSet,
	ChanAspect_Size,
	ChanAspect_SizeSet,
	ChanAspect_Write<EE>
	>;

template <typename EE>
using ChanRWPos = DeriveFrom
	<
	ChanAspect_Pos,
	ChanAspect_PosSet,
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

} // namespace ZooLib

#endif // __ZooLib_Chan_h__
