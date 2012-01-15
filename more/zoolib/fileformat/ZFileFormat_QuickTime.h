/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZFileFormat_QuickTime_h__
#define __ZFileFormat_QuickTime_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZTypes.h"

#include <vector>

namespace ZooLib {
namespace FileFormat {
namespace QuickTime {

// =================================================================================================
// MARK: - FileFormat::QuickTime::Writer

class Writer
	{
public:
	Writer();
	~Writer();

// Our protocol
	void Begin(const ZStreamWPos& iStream, uint32 iChunkType);
	void Begin(const ZStreamWPos& iStream, const char* iChunkType);

	void End(const ZStreamWPos& iStream, uint32 iChunkType);
	void End(const ZStreamWPos& iStream, const char* iChunkType);

private:
	std::vector<std::pair<uint64, uint32> > fChunks;
	};

// =================================================================================================
// MARK: - ZStreamR_IFF

class StreamR_Chunk : public ZStreamR
	{
public:
	StreamR_Chunk(uint32& oChunkType, const ZStreamR& iStream);
	StreamR_Chunk(uint32& oChunkType, bool iSkipOnDestroy, const ZStreamR& iStream);
	~StreamR_Chunk();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	void pInit(uint32& oChunkType, bool iSkipOnDestroy);

	const ZStreamR& fStream;
	bool fSkipOnDestroy;
	size_t fCountRemaining;
	};

// =================================================================================================
// MARK: - StreamRPos_Chunk

class StreamRPos_Chunk : public ZStreamRPos
	{
public:
	StreamRPos_Chunk(uint32& oChunkType, const ZStreamRPos& iStream);
	StreamRPos_Chunk(uint32& oChunkType, bool iSkipOnDestroy, const ZStreamRPos& iStream);
	~StreamRPos_Chunk();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamR via ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	void pInit(uint32& oChunkType, bool iSkipOnDestroy);

	const ZStreamRPos& fStream;
	bool fSkipOnDestroy;
	uint64 fStart;
	size_t fSize;
	};

// =================================================================================================
// MARK: - StreamWPos_Chunk

class StreamWPos_Chunk : public ZStreamWPos
	{
public:
	StreamWPos_Chunk(uint32 iChunkType, const ZStreamWPos& iStream);
	~StreamWPos_Chunk();

// From ZStreamW via ZStreamWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();

// From ZStreamWPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();
	virtual void Imp_SetSize(uint64 iSize);

private:
	const ZStreamWPos& fStream;
	uint64 fStart;
	};

} // namespace QuickTime
} // namespace FileFormat
} // namespace ZooLib

#endif // __ZFileFormat_QuickTime_h__
