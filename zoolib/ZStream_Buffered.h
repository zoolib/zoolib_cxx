/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_Buffered__
#define __ZStream_Buffered__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Buffered

/// A read filter stream that tries to pre-read data in chunks as large as its buffer size.

class ZStreamR_Buffered : public ZStreamR
	{
public:
	ZStreamR_Buffered(size_t iBufferSize, const ZStreamR& iStreamSource);
	~ZStreamR_Buffered();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// Our protocol
	/// Discard any data residing in the buffer.
	void Abandon();

protected:
	const ZStreamR& fStreamSource;
	uint8* fBuffer;
	size_t fBufferSize;
	size_t fBufferOffset;
	};

typedef ZStreamerR_FT<ZStreamR_Buffered> ZStreamerR_Buffered;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Buffered

/// A write filter stream that buffers data written to it.

class ZStreamW_Buffered : public ZStreamW
	{
public:
	ZStreamW_Buffered(size_t iBufferSize, const ZStreamW& iStreamSink);
	~ZStreamW_Buffered();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// Our protocol
	void Abandon();

protected:
	void pFlush();

	const ZStreamW& fStreamSink;
	uint8* fBuffer;
	size_t fBufferSize;
	size_t fBufferOffset;
	};

typedef ZStreamerW_FT<ZStreamW_Buffered> ZStreamerW_Buffered;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_DynamicBuffered

/// A read filter stream that can rewind a read stream by copying read data into a ZStreamRWPos.

class ZStreamR_DynamicBuffered : public ZStreamR
	{
public:
	ZStreamR_DynamicBuffered(const ZStreamR& iStreamSource, const ZStreamRWPos& iStreamBuffer);
	~ZStreamR_DynamicBuffered();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

// Our protocol
	void Rewind();
	void Commit();

protected:
	const ZStreamR& fStreamSource;
	const ZStreamRWPos& fStreamBuffer;
	enum { eModeReadBufferThenAppend, eModeReadBufferThenStream, eModeAppend, eModeStream } fMode;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_DynamicBuffered

/// A read filter streamer encapsulating a ZStreamR_DynamicBuffered.

class ZStreamerR_DynamicBuffered : public ZStreamerR
	{
public:
	ZStreamerR_DynamicBuffered(ZRef<ZStreamerR> iStreamerSource,
		ZRef<ZStreamerRWPos> iStreamerBuffer);

	virtual ~ZStreamerR_DynamicBuffered();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

// Our protocol
	ZStreamR_DynamicBuffered& GetStream();

protected:
	ZRef<ZStreamerR> fStreamerSource;
	ZRef<ZStreamerR> fStreamerBuffer;
	ZStreamR_DynamicBuffered fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_DynamicBuffered

/// A write filter stream that buffers all data written to it until it's flushed.

class ZStreamW_DynamicBuffered : public ZStreamW
	{
public:
	ZStreamW_DynamicBuffered(const ZStreamW& iStreamSink, const ZStreamRWPos& iStreamBuffer);
	~ZStreamW_DynamicBuffered();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();

protected:
	void pFlush();

	const ZStreamW& fStreamSink;
	const ZStreamRWPos& fStreamBuffer;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_DynamicBuffered

/// A write filter streamer encapsulating a ZStreamW_DynamicBuffered.

class ZStreamerW_DynamicBuffered : public ZStreamerW
	{
public:
	ZStreamerW_DynamicBuffered
		(ZRef<ZStreamerW> iStreamerReal, ZRef<ZStreamerRWPos> iStreamerBuffer);

	virtual ~ZStreamerW_DynamicBuffered();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamerSink;
	ZRef<ZStreamerRWPos> fStreamerBuffer;
	ZStreamW_DynamicBuffered fStream;
	};

} // namespace ZooLib

#endif // __ZStream_Buffered__
