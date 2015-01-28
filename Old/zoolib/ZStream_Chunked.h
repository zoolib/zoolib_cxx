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

#ifndef __ZStream_Chunked_h__
#define __ZStream_Chunked_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStreamR_Chunked

class ZStreamR_Chunked : public ZStreamR
	{
public:
	ZStreamR_Chunked(const ZStreamR& iStreamSource);
	~ZStreamR_Chunked();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	const ZStreamR& fStreamSource;
	size_t fChunkSize;
	bool fHitEnd;
	};

// =================================================================================================
#pragma mark -
#pragma mark ZStreamW_Chunked

class ZStreamW_Chunked : public ZStreamW
	{
public:
	ZStreamW_Chunked(const ZStreamW& iStreamSink);
	~ZStreamW_Chunked();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

private:
	void pFlush();

	const ZStreamW& fStreamSink;
	uint8 fBuffer[256];
	size_t fBufferUsed;
	};

} // namespace ZooLib

#endif // __ZStream_Chunked_h__
