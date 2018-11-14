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

#ifndef __ZooLib_POSIX_Chan_Bin_POSIXFD_h__
#define __ZooLib_POSIX_Chan_Bin_POSIXFD_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ZCounted.h"

#if ZCONFIG_SPI_Enabled(POSIX)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark FDHolder

class FDHolder
:	public ZCounted
	{
public:
	FDHolder();
	virtual ~FDHolder();

// Our protocol
	virtual int GetFD() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark FDHolder_Std

class FDHolder_Std
:	public FDHolder
	{
public:
	FDHolder_Std(int iFD);
	virtual ~FDHolder_Std();

// Our protocol
	virtual int GetFD();

protected:
	int fFD;
	};

// =================================================================================================
#pragma mark -
#pragma mark FDHolder_CloseOnDestroy

class FDHolder_CloseOnDestroy
:	public FDHolder
	{
public:
	FDHolder_CloseOnDestroy(int iFD);
	virtual ~FDHolder_CloseOnDestroy();

// Our protocol
	virtual int GetFD();

protected:
	int fFD;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_POSIXFD

class ChanR_Bin_POSIXFD
:	public FDHolder
,	public ChanR<byte>
	{
	ChanR_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanR_Bin_POSIXFD();

// From Aspect_Read<byte>
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

protected:
	const ZRef<FDHolder> fFDHolder;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_POSIXFD

class ChanW_Bin_POSIXFD
:	public ChanW_Bin
	{
public:
	ChanW_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanW_Bin_POSIXFD();

// From Aspect_Write<byte>
	virtual size_t Write(const byte* iSource, size_t iCount);

protected:
	const ZRef<FDHolder> fFDHolder;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanRPos_Bin_POSIXFD

class ChanRPos_Bin_POSIXFD
:	public ChanRPos<byte>
	{
public:
	ChanRPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanRPos_Bin_POSIXFD();

// From Aspect Pos
	virtual uint64 Pos();
	virtual void PosSet(uint64 iPos);

// From Aspect_Read<byte>
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From Aspect_Size
	virtual uint64 Size();

// From Aspect_Unread<byte>
	virtual size_t Unread(const byte* iSource, size_t iCount);
	virtual size_t UnreadableLimit();

protected:
	const ZRef<FDHolder> fFDHolder;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanWPos_Bin_POSIXFD

class ChanWPos_Bin_POSIXFD
:	public ChanWPos<byte>
	{
public:
	ChanWPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanWPos_Bin_POSIXFD();

// From Aspect Pos
	virtual uint64 Pos();
	virtual void PosSet(uint64 iPos);

// From Aspect_Size
	virtual uint64 Size();

// From Aspect_SizeSet
	virtual void SizeSet(uint64 iSize);

// From Aspect_Write<byte>
	virtual size_t Write(const byte* iSource, size_t iCount);

protected:
	const ZRef<FDHolder> fFDHolder;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanRWPos_Bin_POSIXFD

class ChanRWPos_Bin_POSIXFD
:	public ChanRWPos<byte>
	{
public:
	ChanRWPos_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanRWPos_Bin_POSIXFD();

// From Aspect Pos
	virtual uint64 Pos();
	virtual void PosSet(uint64 iPos);

// From Aspect_Read<byte>
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From Aspect_Size
	virtual uint64 Size();

// From Aspect_SizeSet
	virtual void SizeSet(uint64 iSize);

// From Aspect_Write<byte>
	virtual size_t Write(const byte* iSource, size_t iCount);

// From Aspect_Unread<byte>
	virtual size_t Unread(const byte* iSource, size_t iCount);
	virtual size_t UnreadableLimit();

protected:
	const ZRef<FDHolder> fFDHolder;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanRCon_Bin_POSIXFD

class ChanRCon_Bin_POSIXFD
:	public ChanRCon<byte>
	{
public:
	ChanRCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
	~ChanRCon_Bin_POSIXFD();

// From Aspect_Abort
	virtual void Abort();

// From Aspect_DisconnectRead
	virtual bool DisconnectRead(double iTimeout);

// From Aspect_Read<byte>
	virtual size_t Read(byte* oDest, size_t iCount);
	virtual size_t Readable();

// From Aspect_WaitReadable
	virtual bool WaitReadable(double iTimeout);

protected:
	const ZRef<FDHolder> fFDHolder;
	};

//// =================================================================================================
//#pragma mark -
//#pragma mark ChanWCon_Bin_POSIXFD
//
//class ChanWCon_Bin_POSIXFD
//:	public ChanWCon<byte>
//	{
//public:
//	ChanWCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
//	~ChanWCon_Bin_POSIXFD();
//
//// From Aspect_Abort
//	virtual void Abort();
//
//// From Aspect_DisconnectWrite
//	virtual void DisconnectWrite();
//
//// From Aspect_Write<byte>
//	virtual size_t Write(const byte* iSource, size_t iCount);
//
//protected:
//	const ZRef<FDHolder> fFDHolder;
//	};
//
//// =================================================================================================
//#pragma mark -
//#pragma mark ChanRWCon_Bin_POSIXFD
//
//class ChanRWCon_Bin_POSIXFD
//:	public ChanRWCon<byte>
//	{
//public:
//	ChanRWCon_Bin_POSIXFD(const ZRef<FDHolder>& iFDHolder);
//	~ChanRWCon_Bin_POSIXFD();
//
//// From Aspect_Abort
//	virtual void Abort();
//
//// From Aspect_DisconnectRead
//	virtual bool DisconnectRead(double iTimeout);
//
//// From Aspect_DisconnectWrite
//	virtual void DisconnectWrite();
//
//// From Aspect_Read<byte>
//	virtual size_t Read(byte* oDest, size_t iCount);
//	virtual size_t Readable();
//
//// From Aspect_WaitReadable
//	virtual bool WaitReadable(double iTimeout);
//
//// From Aspect_Write<byte>
//	virtual size_t Write(const byte* iSource, size_t iCount);
//
//protected:
//	const ZRef<FDHolder> fFDHolder;
//	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(POSIX)

#endif // __ZooLib_POSIX_Chan_Bin_POSIXFD_h__
