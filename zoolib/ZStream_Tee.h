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

#ifndef __ZStream_Tee_h__
#define __ZStream_Tee_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream_Filter.h"
#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Tee

/// A read filter stream that echoes any data read from it to a write stream.

class ZStreamR_Tee : public ZStreamR_Filter
	{
public:
	ZStreamR_Tee(const ZStreamR& iStreamR, const ZStreamW& iStreamW);
	~ZStreamR_Tee();

	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

protected:
	const ZStreamR& fStreamR;
	const ZStreamW& fStreamW;
	};

// =================================================================================================
// MARK: - ZStreamerR_Tee

/// A read filter streamer encapsulating a ZStreamR_Tee.

class ZStreamerR_Tee : public ZStreamerR
	{
public:
	ZStreamerR_Tee(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZStreamerR_Tee();

// From ZStreamerR
	virtual const ZStreamR& GetStreamR();

protected:
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	ZStreamR_Tee fStream;
	};

// =================================================================================================
// MARK: - ZStreamW_Tee

/// A write filter stream that replicates writes made against it.

class ZStreamW_Tee : public ZStreamW
	{
public:
	ZStreamW_Tee(const ZStreamW& iStreamSink1, const ZStreamW& iStreamSink2);
	~ZStreamW_Tee();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink1;
	const ZStreamW& fStreamSink2;
	};

// =================================================================================================
// MARK: - ZStreamerW_Tee

/// A write filter streamer encapsulating a ZStreamW_Tee.

class ZStreamerW_Tee : public ZStreamerW
	{
public:
	ZStreamerW_Tee(ZRef<ZStreamerW> iStreamerSink1, ZRef<ZStreamerW> iStreamerSink2);
	virtual ~ZStreamerW_Tee();

// From ZStreamerW
	virtual const ZStreamW& GetStreamW();

protected:
	ZRef<ZStreamerW> fStreamerSink1;
	ZRef<ZStreamerW> fStreamerSink2;
	ZStreamW_Tee fStream;
	};

} // namespace ZooLib

#endif // __ZStream_Tee_h__
