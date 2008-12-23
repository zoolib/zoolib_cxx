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

#ifndef __ZStream_POSIX__
#define __ZStream_POSIX__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

#include <stdio.h>
#include <unistd.h>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_FILE

/// A read stream that wraps a POSIX FILE*.

class ZStreamR_FILE : public ZStreamR
	{
public:
	ZStreamR_FILE(FILE* iFILE);
	ZStreamR_FILE(FILE* iFILE, bool iAdopt);
	~ZStreamR_FILE();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// Our protocol
	FILE* GetFILE() { return fFILE; }
	FILE* OrphanFILE() { FILE* theFILE = fFILE; fFILE = nil; return theFILE; }

protected:
	FILE* fFILE;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_FILE

/// A positionable read stream that wraps a POSIX FILE*.

class ZStreamRPos_FILE : public ZStreamRPos
	{
public:
	ZStreamRPos_FILE(FILE* iFILE);
	ZStreamRPos_FILE(FILE* iFILE, bool iAdopt);
	~ZStreamRPos_FILE();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// Our protocol
	FILE* GetFILE() { return fFILE; }
	FILE* OrphanFILE() { FILE* theFILE = fFILE; fFILE = nil; return theFILE; }

protected:
	FILE* fFILE;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_FILE

/// A write stream that wraps a POSIX FILE*.

class ZStreamW_FILE : public ZStreamW
	{
public:
	ZStreamW_FILE(FILE* iFILE);
	ZStreamW_FILE(FILE* iFILE, bool iAdopt);
	~ZStreamW_FILE();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// Our protocol
	FILE* GetFILE() { return fFILE; }
	FILE* OrphanFILE() { FILE* theFILE = fFILE; fFILE = nil; return theFILE; }

protected:
	FILE* fFILE;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark -
#pragma mark * FILE backed by a ZStream or ZStreamer


FILE* sStreamOpen(const ZStreamR& iStreamR);
FILE* sStreamOpen(const ZStreamRPos& iStreamRPos);
FILE* sStreamOpen(const ZStreamW& iStreamW);

FILE* sStreamerOpen(ZRef<ZStreamerR> iStreamerR);
FILE* sStreamerOpen(ZRef<ZStreamerRPos> iStreamerRPos);
FILE* sStreamerOpen(ZRef<ZStreamerW> iStreamerW);

NAMESPACE_ZOOLIB_END

#endif // __ZStream_POSIX__
