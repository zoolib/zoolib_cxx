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

#ifndef __ZStreamR_Source_h__
#define __ZStreamR_Source_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Source

/// A read stream providing an endless repeating sequence of bytes.

class ZStreamR_Source : public ZStreamR
	{
public:
	/// A sequence of zeroes.
	ZStreamR_Source();

	/// A sequence of bytes, each with the value \a iData.
	ZStreamR_Source(uint8 iData);

	/// An arbitrary sequence of length \a iDataSize.
	ZStreamR_Source(const void* iData, size_t iDataSize);

	ZStreamR_Source(const std::pair<const void*, size_t>& iParam);
	~ZStreamR_Source();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

protected:
	void pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	std::vector<uint8> fData;
	size_t fOffset;
	};

// =================================================================================================
// MARK: - ZStreamerR_Source

/// A read streamer encapsulating a ZStreamR_Source.

class ZStreamerR_Source : public ZStreamerR
	{
public:
	ZStreamerR_Source();
	ZStreamerR_Source(uint8 iData);
	ZStreamerR_Source(const void* iData, size_t iDataSize);
	virtual ~ZStreamerR_Source();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZStreamR_Source fStream;
	};

// Alternate template-based definition:
// typedef ZStreamerR_T<ZStreamR_Source> ZStreamerR_Source;

} // namespace ZooLib

#endif // __ZStreamR_Source_h__
