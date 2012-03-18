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

#ifndef __ZStream_bzip2_h__
#define __ZStream_bzip2_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__Stream_bzip2
	#define ZCONFIG_API_Avail__Stream_bzip2 ZCONFIG_SPI_Enabled(bzip2)
#endif

#ifndef ZCONFIG_API_Desired__Stream_bzip2
	#define ZCONFIG_API_Desired__Stream_bzip2 1
#endif

#include "zoolib/ZStreamer.h"

#if ZCONFIG_API_Enabled(Stream_bzip2)

#include <bzlib.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_bzip2Decode

/// A read filter stream that bzip2-decompresses a source stream.

class ZStreamR_bzip2Decode : public ZStreamR
	{
public:
	ZStreamR_bzip2Decode(const ZStreamR& iStreamSource);
	ZStreamR_bzip2Decode(size_t iBufferSize, const ZStreamR& iStreamSource);
	~ZStreamR_bzip2Decode();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

protected:
	void pInit(size_t iBufferSize);

	const ZStreamR& fStreamSource;
	bz_stream fState;
	std::vector<char> fBuffer
	size_t fBufferSize;
	};

typedef ZStreamerR_FT<ZStreamR_bzip2Decode> ZStreamerR_bzip2Decode;

// =================================================================================================
// MARK: - ZStreamW_bzip2Encode

/// A write filter stream that bzip2-compresses to a destination stream.

class ZStreamW_bzip2Encode : public ZStreamW
	{
public:
	ZStreamW_bzip2Encode(int iBlockSize100K, const ZStreamW& iStreamSink);
	ZStreamW_bzip2Encode(int iBlockSize100K, size_t iBufferSize, const ZStreamW& iStreamSink);
	~ZStreamW_bzip2Encode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	void pInit(int iBlockSize100K, size_t iBufferSize);
	void pFlush();

	const ZStreamW& fStreamSink;
	bz_stream fState;
	std::vector<char> fBuffer
	size_t fBufferSize;
	};

// =================================================================================================
// MARK: - ZStreamerW_bzip2Encode

/// A write filter streamer that zlib-compresses (deflates) to a destination stream.

class ZStreamerW_bzip2Encode : public ZStreamerW
	{
public:
	ZStreamerW_bzip2Encode(int iBlockSize100K, ZRef<ZStreamerW> iStreamer);
	ZStreamerW_bzip2Encode(int iBlockSize100K, size_t iBufferSize, ZRef<ZStreamerW> iStreamer);

	virtual ~ZStreamerW_bzip2Encode();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamer;
	ZStreamW_bzip2Encode fStream;
	};

// =================================================================================================
// MARK: - ZStream_bzip2

namespace ZStream_bzip2 {

void sAnalyze(const ZStreamR& iStreamR, vector<pair<uint64, uint32> >& oOffsets);

} // namespace ZStream_bzip2

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Stream_bzip2)

#endif // __ZStream_bzip2_h__
