/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_Base64_h__
#define __ZStream_Base64_h__ 1
#include "zconfig.h"

#include "zoolib/ZQ.h"
#include "zoolib/ZStream.h"

namespace ZooLib {
namespace Base64 {

struct Decode
	{
	uint8 fTable[256];
	};

struct Encode
	{
	uint8 fTable[64];
	uint8 fPadding;
	};

Decode sDecode_Normal();
Encode sEncode_Normal();

Decode sDecode(uint8 i62, uint8 i63);
Encode sEncode(uint8 i62, uint8 i63, uint8 iPadding);

// =================================================================================================
// MARK: - StreamR_Decode

/** A read filter stream that converts base64 data from
the source stream into binary data on the fly.*/

class StreamR_Decode : public ZStreamR
	{
public:
	StreamR_Decode(const ZStreamR& iStreamSource);
	StreamR_Decode(const Decode& iDecode, const ZStreamR& iStreamSource);
	~StreamR_Decode();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);

protected:
	const Decode fDecode;
	const ZStreamR& fStreamSource;
	uint8 fSinkBuf[3];
	size_t fSinkCount;
	};

// =================================================================================================
// MARK: - StreamW_Encode

/** A write filter stream that writes to the destination stream the base64
equivalent of binary data written to it. */

class StreamW_Encode : public ZStreamW
	{
public:
	StreamW_Encode(const ZStreamW& iStreamSink);
	StreamW_Encode(const Encode& iEncode, const ZStreamW& iStreamSink);
	~StreamW_Encode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const Encode fEncode;
	const ZStreamW& fStreamSink;
	uint8 fSourceBuf[3];
	size_t fSourceCount;
	};

// =================================================================================================

} // namespace Base64

typedef Base64::StreamR_Decode ZStreamR_Base64Decode;
typedef Base64::StreamW_Encode ZStreamW_Base64Encode;

} // namespace ZooLib

#endif // __ZStream_Base64_h__
