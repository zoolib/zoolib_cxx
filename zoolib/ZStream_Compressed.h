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

#ifndef __ZStream_Compressed_h__
#define __ZStream_Compressed_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream_ZLib.h"

#if ZCONFIG_API_Enabled(Stream_ZLib)

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamW_Compressed

class ZStreamW_Compressed : public ZStreamW
	{
public:
	ZStreamW_Compressed(size_t iChunkSize, const ZStreamWPos& iSink);
	~ZStreamW_Compressed();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	const ZStreamWPos& fSink;
	size_t fChunkSize;
	size_t fChunkPosition;
	char* fBuffer;
	std::vector<uint64> fOffsets;
	};

// =================================================================================================
// MARK: - ZStreamRPos_Compressed

class ZStreamRPos_Compressed : public ZStreamRPos
	{
public:
	ZStreamRPos_Compressed(const ZStreamRPos& iSource);
	~ZStreamRPos_Compressed();

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// From ZStreamRPos
	virtual uint64 Imp_GetPosition();
	virtual void Imp_SetPosition(uint64 iPosition);

	virtual uint64 Imp_GetSize();

private:
	const ZStreamRPos& fSource;
	size_t fChunkSize;
	uint64 fSize;
	uint64 fPosition;
	std::vector<uint64> fOffsets;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Stream_ZLib)

#endif // __ZStream_Compressed_h__
