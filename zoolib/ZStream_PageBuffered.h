/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_PageBuffered__
#define __ZStream_PageBuffered__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_PageBuffered

/** A positionable read filter stream that buffers a fixed number of fixed size chunks
which are read in preference to accessing the source stream. Chunks are recycled in
least recently used order.
*/
class ZStreamRPos_PageBuffered : public ZStreamRPos
	{
public:
	ZStreamRPos_PageBuffered(
		size_t iBufferCount, size_t iBufferSize, const ZStreamRPos& iStreamReal);

	~ZStreamRPos_PageBuffered();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

protected:
	void Internal_Init(size_t iBufferCount, size_t iBufferSize);

	const ZStreamRPos& fStreamReal;
	struct Buffer;

	Buffer* fBuffer_Head;
	Buffer* fBuffer_Tail;
	size_t fBufferSize;

	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_PageBuffered

/// A positionable read filter streamer wrapping a ZStreamRPos_PageBuffered.

class ZStreamerRPos_PageBuffered : public ZStreamerRPos
	{
public:
	ZStreamerRPos_PageBuffered(
		size_t iBufferCount, size_t iBufferSize, ZRef<ZStreamerRPos> iStreamerSource);

	virtual ~ZStreamerRPos_PageBuffered();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

protected:
	ZRef<ZStreamerRPos> fStreamerSource;
	ZStreamRPos_PageBuffered fStream;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_PageBuffered

/** A positionable read/write filter stream that buffers a fixed number of fixed size chunks
which are read and written to in preference to accessing the real stream. Chunks are recycled in
least recently used order, being written back to the real stream when necessary.
*/
class ZStreamRWPos_PageBuffered : public ZStreamRWPos
	{
public:
	ZStreamRWPos_PageBuffered(
		size_t iBufferCount, size_t iBufferSize, const ZStreamRWPos& iStreamReal);

	~ZStreamRWPos_PageBuffered();

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize);

protected:
	void Internal_Init(size_t iBufferCount, size_t iBufferSize);

	const ZStreamRWPos& fStreamReal;
	struct Buffer;

	Buffer* fBuffer_Head;
	Buffer* fBuffer_Tail;
	size_t fBufferSize;

	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWPos_PageBuffered

/// A positionable read/write filter streamer wrapping a ZStreamerRWPos_PageBuffered.

class ZStreamerRWPos_PageBuffered : public ZStreamerRWPos
	{
public:
	ZStreamerRWPos_PageBuffered(
		size_t iBufferCount, size_t iBufferSize, ZRef<ZStreamerRWPos> iStreamerReal);

	virtual ~ZStreamerRWPos_PageBuffered();

// From ZStreamerRWPos
	virtual const ZStreamRWPos& GetStreamRWPos();

protected:
	ZRef<ZStreamerRWPos> fStreamerReal;
	ZStreamRWPos_PageBuffered fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStream_PageBuffered__
