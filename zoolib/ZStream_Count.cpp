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

#include "zoolib/ZStream_Count.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Count

/**
\class ZStreamR_Count
\ingroup stream
Every read is satisfied by calling through to the real stream, and the number of bytes
succesfully read is accumulated. Call GetCount to find how many bytes have been read.
Call ZeroCount to reset that count.
\note GetCount returns the number of bytes that have been read by calling Read/Skip/CopyTo against
the ZStreamR_Count itself. It obviously has no way of knowing how many bytes have been read from
the referenced real stream.
*/

ZStreamR_Count::ZStreamR_Count(const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fCount(0),
	fCountPtr(&fCount)
	{}

ZStreamR_Count::ZStreamR_Count(uint64& oCount, const ZStreamR& iStreamSource)
:	ZStreamR_Filter(iStreamSource),
	fStreamSource(iStreamSource),
	fCount(0xDEADBEEF),
	fCountPtr(&oCount)
	{
	*fCountPtr = 0;
	}

ZStreamR_Count::~ZStreamR_Count()
	{}

void ZStreamR_Count::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fStreamSource.Read(iDest, iCount, &countRead);
	*fCountPtr += countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_Count::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamSource.CopyTo(iStreamW, iCount, &countRead, oCountWritten);
	*fCountPtr += countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_Count::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamSource.CopyTo(iStreamW, iCount, &countRead, oCountWritten);
	*fCountPtr += countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_Count::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countSkipped;
	fStreamSource.Skip(iCount, &countSkipped);
	*fCountPtr += countSkipped;
	if (oCountSkipped)
		*oCountSkipped = countSkipped;
	}

uint64 ZStreamR_Count::GetCount()
	{ return *fCountPtr; }

void ZStreamR_Count::ZeroCount()
	{ *fCountPtr = 0; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Count

/**
\class ZStreamW_Count
\ingroup stream
Writes are satisfied by calling through to the real stream and the number of bytes
succesfully written is accumulated. Call GetCount to find how many bytes have been written.
Call ZeroCount to reset that count.
\note GetCount returns the number of bytes that have been written by calling
Write/SkipWrite/CopyFrom against the ZStreamW_Count itself. It obviously has no way of knowing
how many bytes have been written to the referenced real stream.
*/

ZStreamW_Count::ZStreamW_Count(const ZStreamW& iStreamSink)
:	fStreamSink(const_cast<ZStreamW*>(&iStreamSink)),
	fCount(0),
	fCountPtr(&fCount)
	{}

ZStreamW_Count::ZStreamW_Count(uint64& oCount, const ZStreamW& iStreamSink)
:	fStreamSink(const_cast<ZStreamW*>(&iStreamSink)),
	fCount(0xDEADBEEF),
	fCountPtr(&oCount)
	{
	*fCountPtr = 0;
	}

ZStreamW_Count::ZStreamW_Count()
:	fStreamSink(nil),
	fCount(0),
	fCountPtr(&fCount)
	{}

ZStreamW_Count::ZStreamW_Count(uint64& oCount)
:	fStreamSink(nil),
	fCount(0xDEADBEEF),
	fCountPtr(&oCount)
	{
	*fCountPtr = 0;
	}

ZStreamW_Count::~ZStreamW_Count()
	{}

void ZStreamW_Count::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	if (fStreamSink)
		fStreamSink->Write(iSource, iCount, &countWritten);
	else
		countWritten = iCount;
	*fCountPtr += countWritten;
	if (oCountWritten)
		*oCountWritten = countWritten;
	}

void ZStreamW_Count::Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (fStreamSink)
		{
		uint64 countWritten;
		fStreamSink->CopyFrom(iStreamR, iCount, oCountRead, &countWritten);
		*fCountPtr += countWritten;
		if (oCountWritten)
			*oCountWritten = countWritten;
		}
	else
		{
		ZStreamW::Imp_CopyFromDispatch(iStreamR, iCount, oCountRead, oCountWritten);
		}
	}

void ZStreamW_Count::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (fStreamSink)
		{
		uint64 countWritten;
		fStreamSink->CopyFrom(iStreamR, iCount, oCountRead, &countWritten);
		*fCountPtr += countWritten;
		if (oCountWritten)
			*oCountWritten = countWritten;
		}
	else
		{
		ZStreamW::Imp_CopyFrom(iStreamR, iCount, oCountRead, oCountWritten);
		}
	}

void ZStreamW_Count::Imp_Flush()
	{
	if (fStreamSink)
		fStreamSink->Flush();
	}

uint64 ZStreamW_Count::GetCount()
	{ return *fCountPtr; }

void ZStreamW_Count::ZeroCount()
	{ *fCountPtr = 0; }

NAMESPACE_ZOOLIB_END
