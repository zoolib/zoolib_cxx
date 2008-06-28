/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZStream_Chunked.h"

#include "zoolib/ZMemory.h" // For ZBlockCopy

using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Chunked

ZStreamR_Chunked::ZStreamR_Chunked(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	fChunkSize = fStreamSource.ReadUInt8();
	fHitEnd = fChunkSize == 0;
	}

ZStreamR_Chunked::~ZStreamR_Chunked()
	{}

void ZStreamR_Chunked::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = fStreamSource.ReadUInt8();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			size_t countRead;
			fStreamSource.Read(localDest, min(iCount, fChunkSize), &countRead);
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

size_t ZStreamR_Chunked::Imp_CountReadable()
	{ return min(fChunkSize, fStreamSource.CountReadable()); }

void ZStreamR_Chunked::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;
	while (countRemaining && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			fChunkSize = fStreamSource.ReadUInt8();
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			uint64 countSkipped;
			fStreamSource.Skip(min(countRemaining, uint64(fChunkSize)), &countSkipped);
			countRemaining -= countSkipped;
			fChunkSize -= countSkipped;
			}
		}

	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Chunked

ZStreamW_Chunked::ZStreamW_Chunked(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	fBuffer = new uint8[256];
	fBufferUsed = 0;
	}

ZStreamW_Chunked::~ZStreamW_Chunked()
	{
	try
		{
		this->Internal_Flush();
		fStreamSink.WriteUInt8(0); // Terminating zero-length chunk.
		}
	catch (...)
		{}

	delete[] fBuffer;
	}

void ZStreamW_Chunked::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fBufferUsed == 255)
			this->Internal_Flush();
		size_t countToCopy = min(iCount, size_t(255 - fBufferUsed));
		ZBlockCopy(localSource, fBuffer + fBufferUsed, countToCopy);
		fBufferUsed += countToCopy;
		iCount -= countToCopy;
		localSource += countToCopy;
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_Chunked::Imp_Flush()
	{
	this->Internal_Flush();
	fStreamSink.Flush();
	}

void ZStreamW_Chunked::Internal_Flush()
	{
	if (const size_t bufferUsed = fBufferUsed)
		{
		fBufferUsed = 0;
		fStreamSink.WriteUInt8(bufferUsed);
		fStreamSink.Write(fBuffer, bufferUsed);
		}
	}

