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

#include "zoolib/ZFileFormat_IFF.h"
#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"

#include <string.h> // For strlen

#ifndef kDebug_IFF
#	define kDebug_IFF 1
#endif

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::pair;

// =================================================================================================
#pragma mark -
#pragma mark * Static helper functions

static const uint8 spZeroes[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

static void spWriteZeroes(const ZStreamW& iStream, size_t iCount)
	{
	while (iCount)
		{
		size_t countWritten;
		iStream.Write(spZeroes, min(iCount, countof(spZeroes)), &countWritten);
		iCount -= countWritten;
		}
	}

static size_t spAligned(size_t iCount, size_t iMultiple)
	{ return (iMultiple - (iCount % iMultiple)) % iMultiple; }

// =================================================================================================
#pragma mark -
#pragma mark * ZFileFormat_IFF::Writer

ZFileFormat_IFF::Writer::Writer(bool iBigEndianSizes, size_t iPadMultiple)
:	fBigEndianSizes(iBigEndianSizes),
	fPadMultiple(iPadMultiple)
	{}

ZFileFormat_IFF::Writer::~Writer()
	{
	ZAssertStop(1, fChunks.empty());
	}

void ZFileFormat_IFF::Writer::Begin(const ZStreamWPos& iStream, uint32 iChunkType)
	{
	iStream.WriteUInt32(iChunkType);
	iStream.WriteUInt32(0); // Dummy for non-header-including size to be written in End
	fChunks.push_back(pair<uint64, size_t>(iStream.GetPosition(), iChunkType));
	}

void ZFileFormat_IFF::Writer::Begin(const ZStreamWPos& iStream, const char* iChunkType)
	{
	ZAssertStop(kDebug_IFF, ::strlen(iChunkType) == 4);
	uint32 chunkType = ZByteSwap_ReadBig32(iChunkType);
	this->Begin(iStream, chunkType);
	}

void ZFileFormat_IFF::Writer::End(const ZStreamWPos& iStream, uint32 iChunkType)
	{
	// Verify we've got a chunk open.
	ZAssertStop(kDebug_IFF, fChunks.size());

	// Grab info about the currently open chunk, and remove it from the stack.
	uint64 start = fChunks.back().first;
	uint32 chunkType = fChunks.back().second;
	fChunks.pop_back();

	// Verify that the chunk we're closing is the last one we opened.
	ZAssertStop(kDebug_IFF, chunkType == iChunkType);

	// Write the chunk's size.
	uint64 position = iStream.GetPosition();

	iStream.SetPosition(start - 4);

	// Potential for overflow here
	size_t chunkSize = position - start;

	if (fBigEndianSizes)
		iStream.WriteUInt32(chunkSize);
	else
		iStream.WriteUInt32LE(chunkSize);

	iStream.SetPosition(position);

	// Write out any padding necessary.
	spWriteZeroes(iStream, spAligned(chunkSize, fPadMultiple));
	}

void ZFileFormat_IFF::Writer::End(const ZStreamWPos& iStream, const char* iChunkType)
	{
	ZAssertStop(kDebug_IFF, ::strlen(iChunkType) == 4);
	uint32 chunkType = ZByteSwap_ReadBig32(iChunkType);
	this->End(iStream, chunkType);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileFormat_IFF::StreamR_Chunk

ZFileFormat_IFF::StreamR_Chunk::StreamR_Chunk(uint32& oChunkType, const ZStreamR& iStream)
:	fStream(iStream)
	{ this->pInit(oChunkType, true, true, 2); }

ZFileFormat_IFF::StreamR_Chunk::StreamR_Chunk(
	uint32& oChunkType, bool iSkipOnDestroy, const ZStreamR& iStream)
:	fStream(iStream)
	{ this->pInit(oChunkType, iSkipOnDestroy, true, 2); }

ZFileFormat_IFF::StreamR_Chunk::StreamR_Chunk(uint32& oChunkType, bool iSkipOnDestroy,
	bool iBigEndianSizes, size_t iPadMultiple, const ZStreamR& iStream)
:	fStream(iStream)
	{ this->pInit(oChunkType, iSkipOnDestroy, iBigEndianSizes, iPadMultiple); }

ZFileFormat_IFF::StreamR_Chunk::~StreamR_Chunk()
	{
	if (fSkipOnDestroy)
		{
		try
			{
			fStream.Skip(fCountRemaining + fPadCount);
			}
		catch (...)
			{}
		}
	}

void ZFileFormat_IFF::StreamR_Chunk::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);

	while (iCount && fCountRemaining)
		{
		size_t countRead;
		fStream.Read(localDest, min(iCount, fCountRemaining), &countRead);
		if (countRead == 0)
			break;
		localDest += countRead;
		fCountRemaining -= countRead;
		iCount -= countRead;
		}

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t ZFileFormat_IFF::StreamR_Chunk::Imp_CountReadable()
	{ return min(fCountRemaining, fStream.CountReadable()); }

bool ZFileFormat_IFF::StreamR_Chunk::Imp_WaitReadable(double iTimeout)
	{ return fStream.WaitReadable(iTimeout); }

void ZFileFormat_IFF::StreamR_Chunk::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	uint64 countRead;
	fStream.CopyTo(iStreamW, min(iCount, uint64(fCountRemaining)), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;	
	}

void ZFileFormat_IFF::StreamR_Chunk::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStream.CopyTo(iStreamW, min(iCount, uint64(fCountRemaining)), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;	
	}

void ZFileFormat_IFF::StreamR_Chunk::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countSkipped;
	fStream.Skip(min(iCount, uint64(fCountRemaining)), &countSkipped);
	fCountRemaining -= countSkipped;
	if (oCountSkipped)
		*oCountSkipped = countSkipped;	
	}

void ZFileFormat_IFF::StreamR_Chunk::pInit(uint32& oChunkType, bool iSkipOnDestroy,
	bool iBigEndianSizes, size_t iPadMultiple)
	{
	try
		{
		fSkipOnDestroy = iSkipOnDestroy;
		oChunkType = fStream.ReadUInt32();
		if (iBigEndianSizes)
			fCountRemaining = fStream.ReadUInt32();
		else
			fCountRemaining = fStream.ReadUInt32LE();

		fPadCount = spAligned(fCountRemaining, iPadMultiple);
		}
	catch (...)
		{
		fSkipOnDestroy = false;
		fCountRemaining = 0;
		oChunkType = 0;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileFormat_IFF::StreamRPos_Chunk

ZFileFormat_IFF::StreamRPos_Chunk::StreamRPos_Chunk(uint32& oChunkType, const ZStreamRPos& iStream)
:	fStream(iStream)
	{
	this->pInit(oChunkType, true, true, 2);
	}

ZFileFormat_IFF::StreamRPos_Chunk::StreamRPos_Chunk(
	uint32& oChunkType, bool iSkipOnDestroy, const ZStreamRPos& iStream)
:	fStream(iStream)
	{
	this->pInit(oChunkType, iSkipOnDestroy, true, 2);
	}

ZFileFormat_IFF::StreamRPos_Chunk::StreamRPos_Chunk(uint32& oChunkType, bool iSkipOnDestroy,
	bool iBigEndianSizes, size_t iPadMultiple, const ZStreamRPos& iStream)
:	fStream(iStream)
	{
	this->pInit(oChunkType, iSkipOnDestroy, iBigEndianSizes, iPadMultiple);
	}

ZFileFormat_IFF::StreamRPos_Chunk::~StreamRPos_Chunk()
	{
	if (fSkipOnDestroy)
		{
		try
			{
			fStream.SetPosition(fStart + spAligned(fSize, fPadMultiple));
			}
		catch (...)
			{}
		}
	}

void ZFileFormat_IFF::StreamRPos_Chunk::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	size_t countRemaining = ZStream::sClampedSize(iCount, fStart + fSize, fStream.GetPosition());
	while (countRemaining)
		{
		size_t countRead;
		fStream.Read(localDest, countRemaining, &countRead);
		if (countRead == 0)
			break;
		localDest += countRead;
		countRemaining -= countRead;
		}

	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

void ZFileFormat_IFF::StreamRPos_Chunk::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	fStream.CopyTo(iStreamW,
		min(iCount, fStart + fSize - fStream.GetPosition()), oCountRead, oCountWritten);
	}

void ZFileFormat_IFF::StreamRPos_Chunk::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	fStream.CopyTo(iStreamW,
		min(iCount, fStart + fSize - fStream.GetPosition()), oCountRead, oCountWritten);
	}

void ZFileFormat_IFF::StreamRPos_Chunk::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	if (uint64 countToSkip = ZStream::sClampedCount(iCount, fStart + fSize, fStream.GetPosition()))
		fStream.Skip(countToSkip, oCountSkipped);
	else if (oCountSkipped)
		*oCountSkipped = 0;
	}

uint64 ZFileFormat_IFF::StreamRPos_Chunk::Imp_GetPosition()
	{ return fStream.GetPosition() - fStart; }

void ZFileFormat_IFF::StreamRPos_Chunk::Imp_SetPosition(uint64 iPosition)
	{ fStream.SetPosition(fStart + iPosition); }

uint64 ZFileFormat_IFF::StreamRPos_Chunk::Imp_GetSize()
	{ return min(uint64(fSize), fStream.GetSize() - fStart); }

void ZFileFormat_IFF::StreamRPos_Chunk::pInit(uint32& oChunkType, bool iSkipOnDestroy,
	bool iBigEndianSizes, size_t iPadMultiple)
	{
	try
		{
		fSkipOnDestroy = iSkipOnDestroy;
		fPadMultiple = iPadMultiple;
		oChunkType = fStream.ReadUInt32();
		if (iBigEndianSizes)
			fSize = fStream.ReadUInt32();
		else
			fSize = fStream.ReadUInt32LE();
		}
	catch (...)
		{
		fSkipOnDestroy = false;
		fSize = 0;
		oChunkType = 0;
		}
	fStart = fStream.GetPosition();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZFileFormat_IFF::StreamWPos_Chunk

ZFileFormat_IFF::StreamWPos_Chunk::StreamWPos_Chunk(uint32 iChunkType, const ZStreamWPos& iStream)
:	fStream(iStream),
	fBigEndianSizes(true),
	fPadMultiple(2)
	{
	this->pInit(iChunkType);
	}

ZFileFormat_IFF::StreamWPos_Chunk::StreamWPos_Chunk(uint32 iChunkType, bool iBigEndianSizes,
	size_t iPadMultiple, const ZStreamWPos& iStream)
:	fStream(iStream),
	fBigEndianSizes(iBigEndianSizes),
	fPadMultiple(iPadMultiple)
	{
	this->pInit(iChunkType);
	}

ZFileFormat_IFF::StreamWPos_Chunk::~StreamWPos_Chunk()
	{
	uint64 position = fStream.GetPosition();
	size_t size = position - fStart;
	fStream.SetPosition(fStart - 4);
	if (fBigEndianSizes)
		fStream.WriteUInt32(size);
	else
		fStream.WriteUInt32LE(size);
	fStream.SetPosition(position + spAligned(size, fPadMultiple));
	}

void ZFileFormat_IFF::StreamWPos_Chunk::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{ fStream.Write(iSource, iCount, oCountWritten); }

void ZFileFormat_IFF::StreamWPos_Chunk::Imp_CopyFromDispatch(
	const ZStreamR& iStreamR, uint64 iCount, uint64* oCountRead, uint64* oCountWritten)
	{ fStream.CopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

void ZFileFormat_IFF::StreamWPos_Chunk::Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{ fStream.CopyFrom(iStreamR, iCount, oCountRead, oCountWritten); }

uint64 ZFileFormat_IFF::StreamWPos_Chunk::Imp_GetPosition()
	{ return fStream.GetPosition() - fStart; }

void ZFileFormat_IFF::StreamWPos_Chunk::Imp_SetPosition(uint64 iPosition)
	{ fStream.SetPosition(iPosition + fStart); }

uint64 ZFileFormat_IFF::StreamWPos_Chunk::Imp_GetSize()
	{ return fStream.GetSize() - fStart; }

void ZFileFormat_IFF::StreamWPos_Chunk::Imp_SetSize(uint64 iSize)
	{ fStream.SetSize(iSize + fStart); }

void ZFileFormat_IFF::StreamWPos_Chunk::pInit(uint32 iChunkType)
	{
	fStream.WriteUInt32(iChunkType);
	fStream.WriteUInt32(0); // Dummy for non-header-including size to be written in End

	fStart = fStream.GetPosition();
	}

NAMESPACE_ZOOLIB_END
