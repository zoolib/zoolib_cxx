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

#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZMemory.h" // For ZMemCopy

using std::max;
using std::min;

NAMESPACE_ZOOLIB_BEGIN

#define kDebug_Stream_Tee 2

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Tee

/**
\class ZStreamR_Tee
\ingroup stream
*/

ZStreamR_Tee::ZStreamR_Tee(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
:	ZStreamR_Filter(iStreamR),
	fStreamR(iStreamR),
	fStreamW(iStreamW)
	{}

ZStreamR_Tee::~ZStreamR_Tee()
	{}

void ZStreamR_Tee::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = static_cast<char*>(oDest);
	while (iCount)
		{
		// We have to read into a local buffer because we're going to pass
		// what we read to fStreamW, and oDest could reference memory that's
		// not safe to read (the garbage buffer, for example).
		char buffer[sStackBufferSize];

		size_t countRead;
		fStreamR.Read(buffer, min(iCount, sizeof(buffer)), &countRead);
		if (countRead == 0)
			break;

		ZMemCopy(localDest, buffer, countRead);

		size_t countWritten;
		fStreamW.Write(buffer, countRead, &countWritten);

		// This is another case where we're basically screwed if the
		// write stream goes dead on us, as far as preserving ZStreamR
		// semantics and not reading more data than we can dispose of.
		ZAssertLog(kDebug_Stream_Tee, countRead == countWritten);

		localDest += countRead;
		iCount -= countRead;
		}
	if (oCountRead)
		*oCountRead = localDest - static_cast<char*>(oDest);
	}

void ZStreamR_Tee::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	// Bypass ZStreamR_Filter's Imp_Skip
	ZStreamR::Imp_Skip(iCount, oCountSkipped);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Tee

ZStreamerR_Tee::ZStreamerR_Tee(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR),
	fStreamerW(iStreamerW),
	fStream(iStreamerR->GetStreamR(), iStreamerW->GetStreamW())
	{}

ZStreamerR_Tee::~ZStreamerR_Tee()
	{}

const ZStreamR& ZStreamerR_Tee::GetStreamR()
	{ return fStream; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Tee

/**
\class ZStreamW_Tee
\ingroup stream
ZStreamW_Tee takes a pair of write streams and echoes whatever is written to it to both of them.
This makes it easy to, say, log all output to a network to the screen or a file as well. The
stream will be writable so long as at least one of the write streams is writeable.
*/

ZStreamW_Tee::ZStreamW_Tee(const ZStreamW& iStreamSink1, const ZStreamW& iStreamSink2)
:	fStreamSink1(iStreamSink1),
	fStreamSink2(iStreamSink2)
	{
	ZAssert(&iStreamSink1 != this);
	ZAssert(&iStreamSink2 != this);
	}

ZStreamW_Tee::~ZStreamW_Tee()
	{}

void ZStreamW_Tee::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		size_t countWritten1;
		fStreamSink1.Write(localSource, iCount, &countWritten1);

		size_t countWritten2;
		fStreamSink2.Write(localSource, iCount, &countWritten2);

		// Our streams will each write either iCount, or as much data as they are able to
		// fit, or they'll return zero when they've hit their bounds. We treat the maximum of
		// what was written to the two streams as being the amount we actually wrote, so that
		// we'll keep writing data so long as one of the streams is able to absorb it. This
		// does rely on streams not doing short writes unless they've really hit their limit.
		// This might be an unreasonable assumption, I'll have to see.
		size_t realCountWritten = max(countWritten1, countWritten2);
		localSource += realCountWritten;
		iCount -= realCountWritten;
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_Tee::Imp_Flush()
	{
	fStreamSink1.Flush();
	fStreamSink2.Flush();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerW_Tee

ZStreamerW_Tee::ZStreamerW_Tee(ZRef<ZStreamerW> iStreamerSink1, ZRef<ZStreamerW> iStreamerSink2)
:	fStreamerSink1(iStreamerSink1),
	fStreamerSink2(iStreamerSink2),
	fStream(iStreamerSink1->GetStreamW(), iStreamerSink2->GetStreamW())
	{}

ZStreamerW_Tee::~ZStreamerW_Tee()
	{}

const ZStreamW& ZStreamerW_Tee::GetStreamW()
	{ return fStream; }

NAMESPACE_ZOOLIB_END
