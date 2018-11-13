/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/POSIX/Chan_Bin_POSIXFD.h"

#if ZCONFIG_SPI_Enabled(POSIX)

#include "zoolib/POSIX/Util_POSIXFD.h"

#include <unistd.h> // for close

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark FDHolder

FDHolder::FDHolder()
	{}

FDHolder::~FDHolder()
	{}

// =================================================================================================
#pragma mark -
#pragma mark FDHolder_Std

FDHolder_Std::FDHolder_Std(int iFD)
:	fFD(iFD)
	{}

FDHolder_Std::~FDHolder_Std()
	{}

int FDHolder_Std::GetFD()
	{ return fFD; }

// =================================================================================================
#pragma mark -
#pragma mark FDHolder_CloseWhenDtor

FDHolder_CloseWhenDestroyed::FDHolder_CloseWhenDestroyed(int iFD)
:	fFD(iFD)
	{}

FDHolder_CloseWhenDestroyed::~FDHolder_CloseWhenDestroyed()
	{ ::close(fFD); }

int FDHolder_CloseWhenDestroyed::GetFD()
	{ return fFD; }

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_POSIXFD

ChanR_Bin_POSIXFD::ChanR_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
:	fFDHolder(iFDHolder)
	{}

ChanR_Bin_POSIXFD::~ChanR_Bin_POSIXFD()
	{}

size_t ChanR_Bin_POSIXFD::Read(byte* oDest, size_t iCount)
	{ return Util_POSIXFD::sRead(fFDHolder->GetFD(), oDest, iCount); }

size_t ChanR_Bin_POSIXFD::Readable()
	{ return Util_POSIXFD::sReadable(fFDHolder->GetFD()); }

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_POSIXFD

ChanW_Bin_POSIXFD::ChanW_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
:	fFDHolder(iFDHolder)
	{}

ChanW_Bin_POSIXFD::~ChanW_Bin_POSIXFD()
	{}

size_t ChanW_Bin_POSIXFD::Write(const byte* iSource, size_t iCount)
	{ return Util_POSIXFD::sWrite(fFDHolder->GetFD(), iSource, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark ChanRPos_Bin_POSIXFD

ChanRPos_Bin_POSIXFD::ChanRPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
:	fFDHolder(iFDHolder)
	{}

ChanRPos_Bin_POSIXFD::~ChanRPos_Bin_POSIXFD()
	{}

uint64 ChanRPos_Bin_POSIXFD::Pos()
	{ return Util_POSIXFD::sPos(fFDHolder->GetFD()); }

void ChanRPos_Bin_POSIXFD::PosSet(uint64 iPos)
	{ Util_POSIXFD::sPosSet(fFDHolder->GetFD(), iPos); }

size_t ChanRPos_Bin_POSIXFD::Read(byte* oDest, size_t iCount)
	{ return Util_POSIXFD::sRead(fFDHolder->GetFD(), oDest, iCount); }

size_t ChanRPos_Bin_POSIXFD::Readable()
	{ return Util_POSIXFD::sReadable(fFDHolder->GetFD()); }

uint64 ChanRPos_Bin_POSIXFD::Size()
	{ return Util_POSIXFD::sSize(fFDHolder->GetFD()); }

size_t ChanRPos_Bin_POSIXFD::Unread(const byte* iSource, size_t iCount)
	{ return Util_POSIXFD::sUnread(fFDHolder->GetFD(), iSource, iCount); }

size_t ChanRPos_Bin_POSIXFD::UnreadableLimit()
	{ return Util_POSIXFD::sUnreadableLimit(fFDHolder->GetFD()); }

// =================================================================================================
#pragma mark -
#pragma mark ChanWPos_Bin_POSIXFD

ChanWPos_Bin_POSIXFD::ChanWPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
:	fFDHolder(iFDHolder)
	{}

ChanWPos_Bin_POSIXFD::~ChanWPos_Bin_POSIXFD()
	{}

uint64 ChanWPos_Bin_POSIXFD::Pos()
	{ return Util_POSIXFD::sPos(fFDHolder->GetFD()); }

void ChanWPos_Bin_POSIXFD::PosSet(uint64 iPos)
	{ Util_POSIXFD::sPosSet(fFDHolder->GetFD(), iPos); }

uint64 ChanWPos_Bin_POSIXFD::Size()
	{ return Util_POSIXFD::sSize(fFDHolder->GetFD()); }

void ChanWPos_Bin_POSIXFD::SizeSet(uint64 iSize)
	{ Util_POSIXFD::sSizeSet(fFDHolder->GetFD(), iSize); }

size_t ChanWPos_Bin_POSIXFD::Write(const byte* iSource, size_t iCount)
	{ return Util_POSIXFD::sWrite(fFDHolder->GetFD(), iSource, iCount); }

// =================================================================================================
#pragma mark -
#pragma mark ChanRWPos_Bin_POSIXFD

ChanRWPos_Bin_POSIXFD::ChanRWPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
:	fFDHolder(iFDHolder)
	{}

ChanRWPos_Bin_POSIXFD::~ChanRWPos_Bin_POSIXFD()
	{}

uint64 ChanRWPos_Bin_POSIXFD::Pos()
	{ return Util_POSIXFD::sPos(fFDHolder->GetFD()); }

void ChanRWPos_Bin_POSIXFD::PosSet(uint64 iPos)
	{ Util_POSIXFD::sPosSet(fFDHolder->GetFD(), iPos); }

size_t ChanRWPos_Bin_POSIXFD::Read(byte* oDest, size_t iCount)
	{ return Util_POSIXFD::sRead(fFDHolder->GetFD(), oDest, iCount); }

size_t ChanRWPos_Bin_POSIXFD::Readable()
	{ return Util_POSIXFD::sReadable(fFDHolder->GetFD()); }

uint64 ChanRWPos_Bin_POSIXFD::Size()
	{ return Util_POSIXFD::sSize(fFDHolder->GetFD()); }

void ChanRWPos_Bin_POSIXFD::SizeSet(uint64 iSize)
	{ Util_POSIXFD::sSizeSet(fFDHolder->GetFD(), iSize); }

size_t ChanRWPos_Bin_POSIXFD::Write(const byte* iSource, size_t iCount)
	{ return Util_POSIXFD::sWrite(fFDHolder->GetFD(), iSource, iCount); }

size_t ChanRWPos_Bin_POSIXFD::Unread(const byte* iSource, size_t iCount)
	{ return Util_POSIXFD::sUnread(fFDHolder->GetFD(), iSource, iCount); }

size_t ChanRWPos_Bin_POSIXFD::UnreadableLimit()
	{ return Util_POSIXFD::sUnreadableLimit(fFDHolder->GetFD()); }

//// =================================================================================================
//#pragma mark -
//#pragma mark ChanRCon_Bin_POSIXFD
//
//ChanRCon_Bin_POSIXFD::ChanRCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
//:	fFDHolder(iFDHolder)
//	{}
//
//ChanRCon_Bin_POSIXFD::~ChanRCon_Bin_POSIXFD()
//	{}
//
//void ChanRCon_Bin_POSIXFD::Abort()
//	{ return Util_POSIXFD::sAbort(fFDHolder->GetFD()); }
//
////bool ChanRCon_Bin_POSIXFD::DisconnectRead(double iTimeout)
////	{ return Util_POSIXFD::sDisconnectRead(fFDHolder->GetFD(), iTimeout); }
//
//size_t ChanRCon_Bin_POSIXFD::Read(byte* oDest, size_t iCount)
//	{ return Util_POSIXFD::sReadCon(fFDHolder->GetFD(), oDest, iCount); }
//
//size_t ChanRCon_Bin_POSIXFD::Readable()
//	{ return Util_POSIXFD::sReadable(fFDHolder->GetFD()); }
//
//bool ChanRCon_Bin_POSIXFD::WaitReadable(double iTimeout)
//	{ return Util_POSIXFD::sWaitReadable(fFDHolder->GetFD(), iTimeout); }
//
//// =================================================================================================
//#pragma mark -
//#pragma mark ChanWCon_Bin_POSIXFD
//
//ChanWCon_Bin_POSIXFD::ChanWCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
//:	fFDHolder(iFDHolder)
//	{}
//
//ChanWCon_Bin_POSIXFD::~ChanWCon_Bin_POSIXFD()
//	{}
//
//void ChanWCon_Bin_POSIXFD::Abort()
//	{ return Util_POSIXFD::sAbort(fFDHolder->GetFD()); }
//
////void ChanWCon_Bin_POSIXFD::DisconnectWrite()
////	{ Util_POSIXFD::sDisconnectWrite(fFDHolder->GetFD()); }
//
//size_t ChanWCon_Bin_POSIXFD::Write(const byte* iSource, size_t iCount)
//	{ return Util_POSIXFD::sWriteCon(fFDHolder->GetFD(), iSource, iCount); }
//
//// =================================================================================================
//#pragma mark -
//#pragma mark ChanRWCon_Bin_POSIXFD
//
//ChanRWCon_Bin_POSIXFD::ChanRWCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder)
//:	fFDHolder(iFDHolder)
//	{}
//
//ChanRWCon_Bin_POSIXFD::~ChanRWCon_Bin_POSIXFD()
//	{}
//
//void ChanRWCon_Bin_POSIXFD::Abort()
//	{ return Util_POSIXFD::sAbort(fFDHolder->GetFD()); }
//
////bool ChanRWCon_Bin_POSIXFD::DisconnectRead(double iTimeout)
////	{ return Util_POSIXFD::sDisconnectRead(fFDHolder->GetFD(), iTimeout); }
//
////void ChanRWCon_Bin_POSIXFD::DisconnectWrite()
////	{ Util_POSIXFD::sDisconnectWrite(fFDHolder->GetFD()); }
//
//size_t ChanRWCon_Bin_POSIXFD::Read(byte* oDest, size_t iCount)
//	{ return Util_POSIXFD::sReadCon(fFDHolder->GetFD(), oDest, iCount); }
//
//size_t ChanRWCon_Bin_POSIXFD::Readable()
//	{ return Util_POSIXFD::sReadable(fFDHolder->GetFD()); }
//
//bool ChanRWCon_Bin_POSIXFD::WaitReadable(double iTimeout)
//	{ return Util_POSIXFD::sWaitReadable(fFDHolder->GetFD(), iTimeout); }
//
//size_t ChanRWCon_Bin_POSIXFD::Write(const byte* iSource, size_t iCount)
//	{ return Util_POSIXFD::sWriteCon(fFDHolder->GetFD(), iSource, iCount); }

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)
