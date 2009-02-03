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

#include "zoolib/ZStream_Compressed.h"

#if ZCONFIG_API_Enabled(Stream_ZLib)

#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZUtil_STL.h"

#define kDebug_StreamRWPos_Compressed 2

NAMESPACE_ZOOLIB_BEGIN

using std::min;

static const char sMagicText[] = "ZStream_Compressed 1.0 CRLF\r\nCR\rLF\n";

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Compressed

ZStreamW_Compressed::ZStreamW_Compressed(size_t iChunkSize, const ZStreamWPos& iSink)
:	fSink(iSink),
	fChunkSize(max(size_t(1024), iChunkSize)),
	fChunkPosition(0)
	{
	fBuffer = new char[fChunkSize];
	}

ZStreamW_Compressed::~ZStreamW_Compressed()
	{
	fOffsets.push_back(fSink.GetPosition());

	if (fChunkPosition != 0)
		ZStreamW_ZLibEncode(5, fSink).Write(fBuffer, fChunkPosition);

	delete[] fBuffer;

	for (vector<uint64>::iterator i = fOffsets.begin(); i != fOffsets.end(); ++i)
		fSink.WriteUInt64(*i);

	fSink.Write(sMagicText, sizeof(sMagicText));
	fSink.WriteUInt32(fChunkSize);
	fSink.WriteUInt64((fOffsets.size() - 1) * fChunkSize + fChunkPosition);
	}

void ZStreamW_Compressed::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);

	while (iCount)
		{
		if (fChunkPosition == fChunkSize)
			{
			fOffsets.push_back(fSink.GetPosition());
			ZStreamW_ZLibEncode(5, fSink).Write(fBuffer, fChunkSize);
			fChunkPosition = 0;
			}
		else
			{
			size_t countToCopy = min(fChunkSize - fChunkPosition, iCount);
			ZBlockCopy(localSource, fBuffer + fChunkPosition, countToCopy);
			iCount -= countToCopy;
			localSource += countToCopy;
			fChunkPosition += countToCopy;
			}
		}

	if (oCountWritten)
		*oCountWritten = localSource - static_cast<const char*>(iSource);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Compressed

ZStreamRPos_Compressed::ZStreamRPos_Compressed(const ZStreamRPos& iSource)
:	fSource(iSource),
	fPosition(0)
	{
	uint64 sourceSize = iSource.GetSize();
	if (sourceSize < 8 + sizeof(sMagicText) + 12)
		throw runtime_error("ZStreamRPos_Compressed, source too small");

	iSource.SetPosition(sourceSize - sizeof(sMagicText) - 12);
	char dummy[sizeof(sMagicText)];
	size_t countRead;
	iSource.Read(dummy, sizeof(sMagicText), &countRead);
	if (countRead != sizeof(sMagicText) || 0 != memcmp(dummy, sMagicText, sizeof(sMagicText)))
		throw runtime_error("ZStreamRPos_Compressed, magic text not found");
	
	fChunkSize = iSource.ReadUInt32();
	fSize = iSource.ReadUInt64();
	uint32 offsetCount = (fSize + fChunkSize - 1) / fChunkSize;
	fOffsets.reserve(offsetCount);

	iSource.SetPosition(sourceSize - sizeof(sMagicText) - 12 - (offsetCount * 8));
	for (size_t x = 0; x < offsetCount; ++x)
		fOffsets.push_back(iSource.ReadUInt64());
	}

ZStreamRPos_Compressed::~ZStreamRPos_Compressed()
	{}

void ZStreamRPos_Compressed::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t chunkNumber = fPosition / fChunkSize;
	if (chunkNumber >= fOffsets.size() || fPosition >= fSize)
		{
		if (oCountRead)
			*oCountRead = 0;
		return;
		}

	fSource.SetPosition(fOffsets[chunkNumber]);
	ZStreamR_ZLibDecode decoder(fSource);
	decoder.Skip(fPosition % fChunkSize);

	size_t countToRead = min(iCount, sClampedR(sDiffPosR(fSize, fPosition)));
	size_t countRead;
	decoder.Read(iDest, countToRead, &countRead);
	fPosition += countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

size_t ZStreamRPos_Compressed::Imp_CountReadable()
	{ return sClampedR(sDiffPosR(fOffsets.back(), fPosition)); }

void ZStreamRPos_Compressed::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 realSkip = min(iCount, sDiffPosR(fSize, fPosition));
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_Compressed::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_Compressed::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_Compressed::Imp_GetSize()
	{ return fSize; }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(Stream_ZLib)
