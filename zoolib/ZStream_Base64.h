/* ------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStream_Base64__
#define __ZStream_Base64__ 1
#include "zconfig.h"

#include "ZStream.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Base64Encode

/** A read filter stream that converts data from the source stream into base64 data on the fly. */

class ZStreamR_Base64Encode : public ZStreamR
	{
public:
	ZStreamR_Base64Encode(const ZStreamR& iStreamSource);
	~ZStreamR_Base64Encode();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

protected:
	const ZStreamR& fStreamSource;
	uint8 fSinkBuf[4];
	size_t fSinkCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Base64Decode

/** A read filter stream that converts base64 data from
the source stream into binary data on the fly.*/

class ZStreamR_Base64Decode : public ZStreamR
	{
public:
	ZStreamR_Base64Decode(const ZStreamR& iStreamSource);
	~ZStreamR_Base64Decode();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);

protected:
	const ZStreamR& fStreamSource;
	uint8 fSinkBuf[3];
	size_t fSinkCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Base64Encode

/** A write filter stream that writes to the destination stream the base64
equivalent of binary data written to it. */

class ZStreamW_Base64Encode : public ZStreamW
	{
public:
	ZStreamW_Base64Encode(const ZStreamW& iStreamSink);
	~ZStreamW_Base64Encode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;
	uint8 fSourceBuf[3];
	size_t fSourceCount;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_Base64Decode

/*** A write filter stream that writes to the destination stream the binary
equivalent of base64 data written to it. */

class ZStreamW_Base64Decode : public ZStreamW
	{
public:
	ZStreamW_Base64Decode(const ZStreamW& iStreamSink);
	~ZStreamW_Base64Decode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;
	uint32 fSource;
	size_t fSourceCount;
	};

// =================================================================================================

#endif // __ZStream_Base64__
