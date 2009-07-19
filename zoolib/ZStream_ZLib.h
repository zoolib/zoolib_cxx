/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_ZLib__
#define __ZStream_ZLib__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Stream_ZLib
#	define ZCONFIG_API_Avail__Stream_ZLib ZCONFIG_SPI_Enabled(zlib)
#endif

#ifndef ZCONFIG_API_Desired__Stream_ZLib
#	define ZCONFIG_API_Desired__Stream_ZLib 1
#endif

#include "zoolib/ZStreamer.h"

#if ZCONFIG_API_Enabled(Stream_ZLib)

#include <zlib.h>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_ZLibDecode

/// A read filter stream that zlib-decompresses (inflates) a source stream.

class ZStreamR_ZLibDecode : public ZStreamR
	{
public:
	ZStreamR_ZLibDecode(const ZStreamR& iStreamSource);
	ZStreamR_ZLibDecode(size_t iBufferSize, const ZStreamR& iStreamSource);
	~ZStreamR_ZLibDecode();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

protected:
	void pInit(size_t iBufferSize);

	const ZStreamR& fStreamSource;
	z_stream fState;
	Bytef* fBuffer;
	size_t fBufferSize;
	};

typedef ZStreamerR_FT<ZStreamR_ZLibDecode> ZStreamerR_ZLibDecode;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_ZLibEncode

/// A write filter stream that zlib-compresses (deflates) to a destination stream.

class ZStreamW_ZLibEncode : public ZStreamW
	{
public:
	ZStreamW_ZLibEncode(int iCompressionLevel, const ZStreamW& iStreamSink);
	ZStreamW_ZLibEncode(int iCompressionLevel, size_t iBufferSize, const ZStreamW& iStreamSink);
	~ZStreamW_ZLibEncode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	void pInit(int iCompressionLevel, size_t iBufferSize);
	void pFlush();

	const ZStreamW& fStreamSink;
	z_stream fState;
	Bytef* fBuffer;
	size_t fBufferSize;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_ZLibEncode

/// A write filter streamer that zlib-compresses (deflates) to a destination stream.

class ZStreamerW_ZLibEncode : public ZStreamerW
	{
public:
	ZStreamerW_ZLibEncode(int iCompressionLevel, ZRef<ZStreamerW> iStreamer);
	ZStreamerW_ZLibEncode(int iCompressionLevel, size_t iBufferSize, ZRef<ZStreamerW> iStreamer);

	virtual ~ZStreamerW_ZLibEncode();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamer;
	ZStreamW_ZLibEncode fStream;
	};

// =================================================================================================

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Stream_ZLib)

#endif // __ZStream_ZLib__
