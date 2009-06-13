/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZStreamRWCon_MemoryPipe__
#define __ZStreamRWCon_MemoryPipe__ 1

#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWCon_MemoryPipe

/** A connected RW stream that transfers data directly from a writer's source
buffer to a reader's destination buffer. Note that read and write must therefore
be called from different threads. */

class ZStreamRWCon_MemoryPipe
:	public ZStreamRCon,
	public ZStreamWCon
	{
public:
	ZStreamRWCon_MemoryPipe();
	~ZStreamRWCon_MemoryPipe();

// From ZStreamR via ZStreamRCon
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRCon
	virtual bool Imp_ReceiveDisconnect(int iMilliseconds);

// From ZStreamW via ZStreamWCon
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

// From ZStreamWCon
	virtual void Imp_SendDisconnect();

// From ZStreamRCon and ZStreamWCon
	virtual void Imp_Abort();

private:
	void pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	void pCopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	ZMtx fMutex;
	ZCnd fCondition_Read;
	ZCnd fCondition_Write;

	bool fWriteClosed;

	const uint8* fSource;
	const uint8* fSourceEnd;

	uint8* fDest;
	size_t fDestCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWCon_MemoryPipe

class ZStreamerRWCon_MemoryPipe : public ZStreamerRWCon
	{
public:
	ZStreamerRWCon_MemoryPipe();
	virtual ~ZStreamerRWCon_MemoryPipe();

// From ZStreamerRCon via ZStreamerRWCon
	virtual const ZStreamRCon& GetStreamRCon();

// From ZStreamerWCon via ZStreamerRWCon
	virtual const ZStreamWCon& GetStreamWCon();

private:
	ZStreamRWCon_MemoryPipe fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamRWCon_MemoryPipe__
