static const char rcsid[] = "@(#) $Id: ZStream_Limited.cpp,v 1.9 2006/04/27 03:19:32 agreen Exp $";
/* ------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZStream_Limited.h"

#include "ZCompat_algorithm.h"
using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Limited

ZStreamR_Limited::ZStreamR_Limited(uint64 iLimit, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fCountRemaining(iLimit)
	{}

ZStreamR_Limited::~ZStreamR_Limited()
	{}

void ZStreamR_Limited::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countRead;
	fStreamSource.Read(iDest, min(uint64(iCount), fCountRemaining), &countRead);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

size_t ZStreamR_Limited::Imp_CountReadable()
	{ return sClampedR(min(fCountRemaining, uint64(fStreamSource.CountReadable()))); }

void ZStreamR_Limited::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamSource.CopyTo(iStreamW, min(iCount, fCountRemaining), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_Limited::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamSource.CopyTo(iStreamW, min(iCount, fCountRemaining), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void ZStreamR_Limited::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countSkipped;
	fStreamSource.Skip(min(iCount, fCountRemaining), &countSkipped);
	fCountRemaining -= countSkipped;
	if (oCountSkipped)
		*oCountSkipped = countSkipped;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Limited

/// A write filter stream that caps the number of bytes that can be written

ZStreamW_Limited::ZStreamW_Limited(uint64 iLimit, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fCountRemaining(iLimit)
	{}

ZStreamW_Limited::~ZStreamW_Limited()
	{}

void ZStreamW_Limited::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t countWritten;
	fStreamSink.Write(iSource, min(uint64(iCount), fCountRemaining), &countWritten);
	fCountRemaining -= countWritten;
	if (oCountWritten)
		*oCountWritten = countWritten;
	}

void ZStreamW_Limited::Imp_Flush()
	{ fStreamSink.Flush(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Limited

ZStreamRPos_Limited::ZStreamRPos_Limited(
	uint64 iOffset, uint64 iLimit, const ZStreamRPos& iStreamSource)
:	fStreamSource(iStreamSource),
	fOffset(iOffset),
	fLimit(iLimit)
	{}
	
ZStreamRPos_Limited::~ZStreamRPos_Limited()
	{}

void ZStreamRPos_Limited::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint64 curPos = fStreamSource.GetPosition();
	if (fOffset + fLimit > curPos)
		fStreamSource.Read(iDest, min(uint64(iCount), fOffset + fLimit - curPos), oCountRead);
	else if (oCountRead)
		*oCountRead = 0;
	}

uint64 ZStreamRPos_Limited::Imp_GetPosition()
	{ return fStreamSource.GetPosition() - fOffset; }

void ZStreamRPos_Limited::Imp_SetPosition(uint64 iPosition)
	{ fStreamSource.SetPosition(fOffset + iPosition); }

uint64 ZStreamRPos_Limited::Imp_GetSize()
	{ return min(fStreamSource.GetSize(), fLimit); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRPos_Limited

ZStreamerRPos_Limited::ZStreamerRPos_Limited(
	uint64 iOffset, uint64 iLimit, ZRef<ZStreamerRPos> iStreamer)
:	fStreamer(iStreamer),
	fStream(iOffset, iLimit, iStreamer->GetStreamRPos())
	{}

ZStreamerRPos_Limited::~ZStreamerRPos_Limited()
	{}

const ZStreamRPos& ZStreamerRPos_Limited::GetStreamRPos()
	{ return fStream; }
