/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_Chan_Bin_FILE_h__
#define __ZooLib_Chan_Bin_FILE_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

#include <stdio.h>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark FILEHolder

class FILEHolder
	{
public:
	FILEHolder(FILE* iFILE, bool iAdopt);
	~FILEHolder();

	FILE* GetFILE();
	FILE* OrphanFILE();

protected:
	FILE* fFILE;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_Bin_FILE

/// A read stream that wraps a POSIX FILE*.

class ChanR_Bin_FILE
:	public ChanR_Bin
,	public FILEHolder
	{
public:
	ChanR_Bin_FILE(FILE* iFILE);
	ChanR_Bin_FILE(FILE* iFILE, bool iAdopt);

// From ChanR_Bin
	virtual size_t Read(byte* oDest, size_t iCount);
	};

//// =================================================================================================
//#pragma mark -
#pragma mark ZStreamRPos_FILE
//
///// A positionable read stream that wraps a POSIX FILE*.
//
//class ZStreamRPos_FILE
//:	public ZStreamRPos
//,	public FILEHolder
//	{
//public:
//	ZStreamRPos_FILE(FILE* iFILE);
//	ZStreamRPos_FILE(FILE* iFILE, bool iAdopt);
//
//// From ZStreamR
//	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
//
//// From ZStreamRPos
//	virtual uint64 Imp_GetPosition();
//	virtual void Imp_SetPosition(uint64 iPosition);
//
//	virtual uint64 Imp_GetSize();
//	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_Bin_FILE

/// A write stream that wraps a POSIX FILE*.

class ChanW_Bin_FILE
:	public ChanW_Bin
,	public FILEHolder
	{
public:
	ChanW_Bin_FILE(FILE* iFILE);
	ChanW_Bin_FILE(FILE* iFILE, bool iAdopt);

// From ChanW_Bin
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();
	};

//// =================================================================================================
//#pragma mark -
#pragma mark FILE backed by a ZStream or ZStreamer
//
//FILE* sStreamOpen(const ZStreamR& iStreamR);
//FILE* sStreamOpen(const ZStreamRPos& iStreamRPos);
//FILE* sStreamOpen(const ZStreamW& iStreamW);
//
//FILE* sStreamerOpen(ZRef<ZStreamerR> iStreamerR);
//FILE* sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos);
//FILE* sStreamerOpen(ZRef<ZStreamerW> iStreamerW);

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_FILE_h__
