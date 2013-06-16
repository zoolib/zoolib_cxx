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

#include "zoolib/ZStream_bzip2.h"

#if ZCONFIG_API_Enabled(Stream_bzip2)

#include "zoolib/ZBitStream.h"
#include "zoolib/ZCompat_algorithm.h" // For min/max
#include "zoolib/ZDebug.h"

#include <stdexcept> // For runtime_error

namespace ZooLib {

using std::min;
using std::max;
using std::runtime_error;

#define kDebug_bzip2 2

// =================================================================================================
// MARK: - ZStreamR_bzip2Decode

/**
\class ZStreamR_bzip2Decode
\ingroup stream
*/

/**
\param iStreamSource The stream from which compressed data will be read.
*/
ZStreamR_bzip2Decode::ZStreamR_bzip2Decode(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	this->pInit(1024);
	}

/**
\param iBufferSize The buffer size determines the maximum size of chunks we try to read from
the source stream. The default value is 1024 bytes which should be okay for most purposes. If
your source stream has poor latency then use a larger value or interpose a ZStreamR_Buffered.
\param iStreamSource The stream from which compressed data will be read.
*/
ZStreamR_bzip2Decode::ZStreamR_bzip2Decode(size_t iBufferSize, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	this->pInit(iBufferSize);
	}

ZStreamR_bzip2Decode::~ZStreamR_bzip2Decode()
	{
	::BZ2_bzDecompressEnd(&fState);
	}

void ZStreamR_bzip2Decode::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	ZAssertStop(kDebug_bzip2, fState.avail_in == 0);
	fState.avail_out = iCount;
	fState.next_out = static_cast<char*>(oDest);
	for (;;)
		{
		int result = ::BZ2_bzDecompress(&fState);
		if (fState.avail_out == 0)
			{
			break;
			}
		else if (fState.avail_in == 0)
			{
			// Top up our input buffer
			size_t countReadable = fStreamSource.CountReadable();
			if (countReadable == 0)
				countReadable = 1;

			size_t countToRead = min(countReadable, fBuffer.size());

			size_t countRead;
			fStreamSource.Read(fBuffer, countToRead, &countRead);

			if (countRead == 0)
				break;

			fState.avail_in = countRead;
			fState.next_in = fBuffer;
			}
		}
	if (oCountRead)
		*oCountRead = iCount - fState.avail_out;
	}

size_t ZStreamR_bzip2Decode::Imp_CountReadable()
	{
	return fState.avail_out;
	}

void ZStreamR_bzip2Decode::pInit(size_t iBufferSize)
	{
	fBuffer.resize(max(size_t(1024), iBufferSize));

	fState.bzalloc = nullptr;
	fState.bzfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = &fBuffer[0];
	fState.avail_in = 0;

	fState.next_out = nullptr;
	fState.avail_out = 0;

	if (BZ_OK != ::BZ2_bzDecompressInit(&fState, 0, 0))
		throw runtime_error("ZStreamR_bzip2Decode problem");
	}

// =================================================================================================
// MARK: - ZStreamW_bzip2Encode

/**
\class ZStreamW_bzip2Encode
\ingroup stream
*/

/**
\param iBlockSize100K
\param iStreamSink The stream to which compressed data should be written.
*/
ZStreamW_bzip2Encode::ZStreamW_bzip2Encode(int iBlockSize100K, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	this->pInit(iBlockSize100K, 1024);
	}

/**
\param iBlockSize100K
\param iBufferSize. The buffer size determines how much data we accumulate before passing it on
to the destination stream. The default is 1024, and although almost any value will do we never
use less than 1024. If your destination stream has poor latency then use a (much) larger
value, or interpose a ZStreamW_Buffered or ZStreamW_DynamicBuffered.
\param iStreamSink The stream to which compressed data should be written.
*/
ZStreamW_bzip2Encode::ZStreamW_bzip2Encode(
	int iBlockSize100K, size_t iBufferSize, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	this->pInit(iBlockSize100K, iBufferSize);
	}

ZStreamW_bzip2Encode::~ZStreamW_bzip2Encode()
	{
	if (fBuffer.size())
		{
		try
			{
			this->pFlush();
			}
		catch (...)
			{}
		}

	::BZ2_bzCompressEnd(&fState);
	}

void ZStreamW_bzip2Encode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (fBuffer.empty())
		{
		if (oCountWritten)
			*oCountWritten = 0;
		return;
		}

	ZAssertStop(kDebug_bzip2, fState.avail_in == 0);

	fState.avail_in = iCount;
	fState.next_in = const_cast<char*>(static_cast<const char*>(iSource));
	for (;;)
		{
		int result = ::BZ2_bzCompress(&fState, BZ_RUN);
		if (size_t countToWrite = fBuffer.size() - fState.avail_out)
			{
			size_t countWritten;
			fStreamSink.Write(&fBuffer[0], countToWrite, &countWritten);
			if (countWritten < countToWrite)
				{
				// fStreamSink has closed. Mark ourselves similarly, by zeroing fBufferSize.
				fBufferSize = 0;
				break;
				}
			fState.next_out = &fBuffer[0];
			fState.avail_out = fBuffer.size();
			}
		else if (fState.avail_in == 0)
			{
			break;
			}
		}
	if (oCountWritten)
		*oCountWritten = iCount - fState.avail_in;
	}

/**
Flush closes off any pending zlib block, passes the data to our destination stream and
then flushes the destination stream. Calling Flush too often will degrade compression
performance. However, if the destination stream is being read from 'live' then you must
call Flush to ensure that pending data is passed on, rather than being buffered by zlib
in the hopes that it can be compressed in conjunction with subsequent data.
*/
void ZStreamW_bzip2Encode::Imp_Flush()
	{
	if (fBuffer.empty())
		return;
	this->pFlush();
	fStreamSink.Flush();
	}

void ZStreamW_bzip2Encode::pInit(int iBlockSize100K, size_t iBufferSize)
	{
	fBuffer.resize(max(size_t(1024), iBufferSize));

	fState.bzalloc = nullptr;
	fState.bzfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = nullptr;
	fState.avail_in = 0;

	fState.next_out = &fBuffer[0];
	fState.avail_out = fBuffer.size();

	if (BZ_OK != ::BZ2_bzCompressInit(&fState, iBlockSize100K, 0, 0))
		throw runtime_error("ZStreamW_bzip2Encode problem");
	}

void ZStreamW_bzip2Encode::pFlush()
	{
	ZAssertStop(kDebug_bzip2, fState.avail_in == 0);

	for (;;)
		{
		int result = ::BZ2_bzCompress(&fState, BZ_FLUSH);
		if (size_t countToWrite = fBuffer.size() - fState.avail_out)
			{
			size_t countWritten;
			fStreamSink.Write(&fBuffer[0], countToWrite, &countWritten);
			if (countWritten < countToWrite)
				{
				fBuffer.resize(0);
				ZStreamW::sThrowEndOfStream();
				}
			fState.next_out = &fBuffer[0];
			fState.avail_out = fBuffer.size();
			}
		else
			{
			break;
			}
		}
	}

// =================================================================================================
// MARK: - ZStreamerW_bzip2Encode

ZStreamerW_bzip2Encode::ZStreamerW_bzip2Encode(int iBlockSize100K, ZRef<ZStreamerW> iStreamer)
:	fStreamer(iStreamer),
	fStream(iBlockSize100K, iStreamer->GetStreamW())
	{}

ZStreamerW_bzip2Encode::ZStreamerW_bzip2Encode(
	int iBlockSize100K, size_t iBufferSize, ZRef<ZStreamerW> iStreamer)
:	fStreamer(iStreamer),
	fStream(iBlockSize100K, iBufferSize, iStreamer->GetStreamW())
	{}

ZStreamerW_bzip2Encode::~ZStreamerW_bzip2Encode()
	{}

const ZStreamW& ZStreamerW_bzip2Encode::GetStreamW()
	{ return fStream; }

// =================================================================================================
// MARK: - ZStream_bzip2

static const uint64 BLOCK_HEADER = ZUINT64_C(0x314159265359);
static const uint64 BLOCK_ENDMARK = ZUINT64_C(0x177245385090);
static const uint64 BLOCK_MASK = ZUINT64_C(0xFFFFFFFFFFFF);

void ZStream_bzip2::sAnalyze(const ZStreamR& iStreamR, vector<pair<uint64, uint32> >& oOffsets)
	{
	oOffsets.clear();
	oOffsets.reserve(1024);

	ZBitReaderLE r;

	uint64 bitStartCurrent = 0;
	uint64 bitsRead = 0;
	uint64 buffer = 0;
	bool inBlock = false;
	for (;;)
		{
		uint32 bit;
		if (!r.ReadBits(iStreamR, 1, bit))
			break;

		buffer = ((buffer << 1) | bit) & BLOCK_MASK;
		if (buffer == BLOCK_ENDMARK)
			{
			if (inBlock)
				oOffsets.push_back(pair<uint64, uint32>(bitStartCurrent, bitsRead - bitStartCurrent));
			inBlock = false;
			}
		else if (buffer == BLOCK_HEADER)
			{
			if (inBlock)
				oOffsets.push_back(pair<uint64, uint32>(bitStartCurrent, bitsRead - bitStartCurrent));
			inBlock = true;
			bitStartCurrent = bitsRead;
			}
		++bitsRead;
		}
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Stream_bzip2)
