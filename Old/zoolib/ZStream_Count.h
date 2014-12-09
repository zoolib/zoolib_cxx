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

#ifndef __ZStream_Count_h__
#define __ZStream_Count_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream_Filter.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Count

/// A read filter stream that counts the number of bytes read from it.

class ZStreamR_Count : public ZStreamR_Filter
	{
public:
	/// \param iStreamSource The stream from which data is to be read.
	ZStreamR_Count(const ZStreamR& iStreamSource);
	ZStreamR_Count(uint64& oCount, const ZStreamR& iStreamSource);

	~ZStreamR_Count();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

// Our protocol

	/// Returns the number of bytes read since construction or the last call to ZeroCount.
	uint64 GetCount();

	/// Resets our counter back to zero.
	void ZeroCount();

protected:
	const ZStreamR& fStreamSource;
	uint64 fCount;
	uint64* fCountPtr;
	};

//typedef ZStreamerR_FT<ZStreamR_Count> ZStreamerR_Count;

// =================================================================================================
// MARK: - ZStreamW_Count

/// A write filter stream that counts the number of bytes written to it.

class ZStreamW_Count : public ZStreamW
	{
public:
	/// \param iStreamSink The stream to which data is to be written.
	ZStreamW_Count(const ZStreamW& iStreamSink);

	ZStreamW_Count(uint64& oCount, const ZStreamW& iStreamSink);

	/** The stream will count data written to it, but as it
	has no write stream the data will just be discarded. */
	ZStreamW_Count();
	ZStreamW_Count(uint64& oCount);

	~ZStreamW_Count();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

	virtual void Imp_CopyFromDispatch(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyFrom(const ZStreamR& iStreamR, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Flush();

// Our protocol

	/// Returns the number of bytes written since construction or the last call to ZeroCount.
	uint64 GetCount();

	/// Resets our counter back to zero.
	void ZeroCount();

protected:
	const ZStreamW* fStreamSink;
	uint64 fCount;
	uint64* fCountPtr;
	};

//typedef ZStreamerW_FT<ZStreamW_Count> ZStreamerW_Count;

} // namespace ZooLib

#endif // __ZStream_Count_h__
