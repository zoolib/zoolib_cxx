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

#ifndef __ZStream_Limited__
#define __ZStream_Limited__ 1

#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZStreamer.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Limited

/// A read filter stream that caps the number of bytes that can be read

class ZStreamR_Limited : public ZStreamR
	{
public:
	ZStreamR_Limited(uint64 iLimit, const ZStreamR& iStreamSource);
	~ZStreamR_Limited();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

protected:
	const ZStreamR& fStreamSource;
	uint64 fCountRemaining;
	};

// typedef ZStreamerR_FT<ZStreamR_Limited> ZStreamerR_Limited;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Limited

/// A write filter stream that caps the number of bytes that can be written

class ZStreamW_Limited : public ZStreamW
	{
public:
	ZStreamW_Limited(uint64 iLimit, const ZStreamW& iStreamSink);
	~ZStreamW_Limited();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;
	uint64 fCountRemaining;
	};

// typedef ZStreamerW_FT<ZStreamW_Limited> ZStreamerW_Limited;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Limited

/** A positionable read filter stream that offsets
the position and caps the number of bytes that can be read. */

class ZStreamRPos_Limited : public ZStreamRPos
	{
public:
	ZStreamRPos_Limited(uint64 iOffset, uint64 iLimit, const ZStreamRPos& iStreamSource);
	~ZStreamRPos_Limited();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

protected:
	const ZStreamRPos& fStreamSource;
	uint64 fOffset;
	uint64 fLimit;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_Limited

/// A positionable read filter streamer encapsulating a ZStreamRPos_Limited.

class ZStreamerRPos_Limited : public ZStreamerRPos
	{
public:
	ZStreamerRPos_Limited(uint64 iOffset, uint64 iLimit, ZRef<ZStreamerRPos> iStreamer);

	virtual ~ZStreamerRPos_Limited();

// From ZStreamerRPos
	virtual const ZStreamRPos& GetStreamRPos();

protected:
	ZRef<ZStreamerRPos> fStreamer;
	ZStreamRPos_Limited fStream;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStream_Limited__
