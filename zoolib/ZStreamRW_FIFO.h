/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#ifndef __ZStreamRW_FIFO__
#define __ZStreamRW_FIFO__ 1

#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZThreadImp.h"

#include <deque>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRW_FIFO

/** A RW stream that queues written data and returns it in FIFO order when read. */

class ZStreamRW_FIFO
:	public ZStreamR,
	public ZStreamW
	{
public:
	ZStreamRW_FIFO();
	ZStreamRW_FIFO(size_t iMaxSize);
	~ZStreamRW_FIFO();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

// Our protocol
	void Close();
	bool IsClosed();
	void Reset();

private:
	ZMtx fMutex;
	ZCnd fCondition_UserCount;
	ZCnd fCondition_Read;
	ZCnd fCondition_Write;
	bool fClosed;
	size_t fMaxSize;
	std::deque<uint8> fBuffer;
	size_t fUserCount;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamRW_FIFO__
