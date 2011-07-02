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

#include "zoolib/ZStream_ZLib.h"

#if ZCONFIG_API_Enabled(Stream_ZLib)

#include "zoolib/ZCompat_algorithm.h" // For min/max
#include "zoolib/ZDebug.h"

#include <stdexcept> // For runtime_error

using std::min;
using std::max;
using std::runtime_error;

namespace ZooLib {

#define kDebug_ZLib 2

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_ZLibDecode

/**
\class ZStreamR_ZLibDecode
\ingroup stream
*/

/**
\param iStreamSource The stream from which compressed data will be read.
*/
ZStreamR_ZLibDecode::ZStreamR_ZLibDecode(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	this->pInit(ZStream_ZLib::eFormatR_Auto, 1024);
	}

/**
\param iFormatR
\param iBufferSize The buffer size determines the maximum size of chunks we try to read from
the source stream. The default value is 1024 bytes which should be okay for most purposes. If
your source stream has poor latency then use a larger value or interpose a ZStreamR_Buffered.
\param iStreamSource The stream from which compressed data will be read.
*/
ZStreamR_ZLibDecode::ZStreamR_ZLibDecode
	(ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize,
	const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	this->pInit(iFormatR, iBufferSize);
	}

ZStreamR_ZLibDecode::~ZStreamR_ZLibDecode()
	{
	::inflateEnd(&fState);
	delete[] fBuffer;
	}

void ZStreamR_ZLibDecode::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	ZAssertStop(kDebug_ZLib, fState.avail_in == 0);
	fState.avail_out = iCount;
	fState.next_out = reinterpret_cast<Bytef*>(oDest);
	for (;;)
		{
		/*int result = */::inflate(&fState, Z_NO_FLUSH);
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

			size_t countToRead = min(countReadable, fBufferSize);

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

size_t ZStreamR_ZLibDecode::Imp_CountReadable()
	{
	return fState.avail_out;
	}

bool ZStreamR_ZLibDecode::Imp_WaitReadable(double iTimeout)
	{
	if (fState.avail_out)
		return true;
	return fStreamSource.WaitReadable(iTimeout);
	}

void ZStreamR_ZLibDecode::pInit(ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize)
	{
	fBufferSize = max(size_t(1024), iBufferSize);
	fBuffer = new Bytef[fBufferSize];

	fState.zalloc = nullptr;
	fState.zfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = fBuffer;
	fState.avail_in = 0;

	fState.next_out = nullptr;
	fState.avail_out = 0;

	int windowBits;
	switch (iFormatR)
		{
		case ZStream_ZLib::eFormatR_Auto: windowBits = 32 | 15; break;
		case ZStream_ZLib::eFormatR_GZip: windowBits = 16 | 15; break;
		case ZStream_ZLib::eFormatR_ZLib: windowBits = 15; break;
		case ZStream_ZLib::eFormatR_Raw: windowBits = -15; break;
		}

	int error = inflateInit2(&fState, windowBits);
	if (Z_OK != error)
		throw runtime_error("ZStreamR_ZLibDecode problem");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_ZLibDecode

/// A read filter streamer that zlib-decompresses (inflates) a source stream.

ZStreamerR_ZLibDecode::ZStreamerR_ZLibDecode(ZRef<ZStreamerR> iStreamer)
:	fStreamer(iStreamer),
	fStream(iStreamer->GetStreamR())
	{}

ZStreamerR_ZLibDecode::ZStreamerR_ZLibDecode
	(ZStream_ZLib::EFormatR iFormatR, size_t iBufferSize,
	ZRef<ZStreamerR> iStreamer)
:	fStreamer(iStreamer),
	fStream(iFormatR, iBufferSize, iStreamer->GetStreamR())
	{}

ZStreamerR_ZLibDecode::~ZStreamerR_ZLibDecode()
	{}

const ZStreamR& ZStreamerR_ZLibDecode::GetStreamR()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_ZLibEncode

/**
\class ZStreamW_ZLibEncode
\ingroup stream
*/

/**
\param iCompressionLevel Indicates the amount of compression that should be applied, in the
range from 1 to 9. 3 is a good tradeoff between CPU time and compression ratio.
\param iStreamSink The stream to which compressed data should be written.
*/
ZStreamW_ZLibEncode::ZStreamW_ZLibEncode(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	this->pInit(ZStream_ZLib::eFormatW_ZLib, 3, 1024);
	}

/**
\param iFormatW
\param iCompressionLevel Indicates the amount of compression that should be applied, in the
range from 1 to 9. 3 is a good tradeoff between CPU time and compression ratio.
\param iBufferSize. The buffer size determines how much data we accumulate before passing it on
to the destination stream. The default is 1024, and although almost any value will do we never
use less than 1024. If your destination stream has poor latency then use a (much) larger
value, or interpose a ZStreamW_Buffered or ZStreamW_DynamicBuffered.
\param iStreamSink The stream to which compressed data should be written.
*/
ZStreamW_ZLibEncode::ZStreamW_ZLibEncode
	(ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
	const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	this->pInit(iFormatW, iCompressionLevel, iBufferSize);
	}

ZStreamW_ZLibEncode::~ZStreamW_ZLibEncode()
	{
	if (fBufferSize)
		{
		try
			{
			this->pFlush();
			}
		catch (...)
			{}
		}

	::deflateEnd(&fState);

	delete[] fBuffer;
	}

void ZStreamW_ZLibEncode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (!fBufferSize)
		{
		if (oCountWritten)
			*oCountWritten = 0;
		return;
		}

	ZAssertStop(kDebug_ZLib, fState.avail_in == 0);

	fState.avail_in = iCount;
	fState.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(iSource));
	for (;;)
		{
		/*int result = */::deflate(&fState, Z_NO_FLUSH);
		if (size_t countToWrite = fBufferSize - fState.avail_out)
			{
			size_t countWritten;
			fStreamSink.Write(fBuffer, countToWrite, &countWritten);
			if (countWritten < countToWrite)
				{
				// fStreamSink has closed. Mark ourselves similarly, by zeroing fBufferSize.
				fBufferSize = 0;
				break;
				}
			fState.next_out = fBuffer;
			fState.avail_out = fBufferSize;
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
void ZStreamW_ZLibEncode::Imp_Flush()
	{
	if (!fBufferSize)
		return;
	this->pFlush();
	fStreamSink.Flush();
	}

void ZStreamW_ZLibEncode::pInit
	(ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize)
	{
	fBufferSize = max(size_t(1024), iBufferSize);
	fBuffer = new Bytef[fBufferSize];

	fState.zalloc = nullptr;
	fState.zfree = nullptr;
	fState.opaque = nullptr;

	fState.next_in = nullptr;
	fState.avail_in = 0;

	fState.next_out = fBuffer;
	fState.avail_out = fBufferSize;

	int windowBits;
	switch (iFormatW)
		{
		case ZStream_ZLib::eFormatW_GZip: windowBits = 16 | 15; break;
		case ZStream_ZLib::eFormatW_ZLib: windowBits = 15; break;
		case ZStream_ZLib::eFormatW_Raw: windowBits = -15; break;
		}

	if (Z_OK != ::deflateInit2(&fState,
		iCompressionLevel, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY))
		{
		throw runtime_error("ZStreamW_ZLibEncode problem");
		}
	}

void ZStreamW_ZLibEncode::pFlush()
	{
	ZAssertStop(kDebug_ZLib, fState.avail_in == 0);

	for (;;)
		{
		/*int result = */::deflate(&fState, Z_SYNC_FLUSH);
		if (size_t countToWrite = fBufferSize - fState.avail_out)
			{
			size_t countWritten;
			fStreamSink.Write(fBuffer, countToWrite, &countWritten);
			if (countWritten < countToWrite)
				{
				fBufferSize = 0;
				ZStreamW::sThrowEndOfStream();
				}
			fState.next_out = fBuffer;
			fState.avail_out = fBufferSize;
			}
		else
			{
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_ZLibEncode

ZStreamerW_ZLibEncode::ZStreamerW_ZLibEncode(ZRef<ZStreamerW> iStreamer)
:	fStreamer(iStreamer),
	fStream(iStreamer->GetStreamW())
	{}

ZStreamerW_ZLibEncode::ZStreamerW_ZLibEncode
	(ZStream_ZLib::EFormatW iFormatW, int iCompressionLevel, size_t iBufferSize,
	ZRef<ZStreamerW> iStreamer)
:	fStreamer(iStreamer),
	fStream(iFormatW, iCompressionLevel, iBufferSize, iStreamer->GetStreamW())
	{}

ZStreamerW_ZLibEncode::~ZStreamerW_ZLibEncode()
	{}

const ZStreamW& ZStreamerW_ZLibEncode::GetStreamW()
	{ return fStream; }

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Stream_ZLib)
