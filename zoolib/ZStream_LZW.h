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

#ifndef __ZStream_LZW__
#define __ZStream_LZW__ 1
#include "zconfig.h"

#include "ZBitStream.h"
#include "ZStream.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWEncode

/* This one is not done yet.
class ZStreamR_LZWEncode : public ZStreamR
	{
public:
	ZStreamR_LZWEncode(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWEncode();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

protected:
	const ZStreamR& fStreamSource;

	int fCodeSize_Alphabet;
	int fCodeSize_Emitted;

	int fCode_Clear;
	int fCode_FIN;

	size_t fCountSinceClear;

	bool fSentFIN;
	bool fFinished;

	ZBitWriterBE fBitWriter;
	};
*/

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWEncodeNoPatent

class ZStreamR_LZWEncodeNoPatent : public ZStreamR
	{
public:
	ZStreamR_LZWEncodeNoPatent(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWEncodeNoPatent();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

protected:
	const ZStreamR& fStreamSource;

	int fCodeSize_Alphabet;
	int fCodeSize_Emitted;

	int fCode_Clear;
	int fCode_FIN;

	size_t fCountSinceClear;

	bool fSentFIN;
	bool fFinished;

	ZBitWriterBE fBitWriter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWDecode

/// A read filter stream that LZW-decompresses a source stream.

class ZStreamR_LZWDecode : public ZStreamR
	{
public:
	ZStreamR_LZWDecode(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWDecode();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();

protected:
	const ZStreamR& fStreamSource;

	bool fSeenFIN;

	int fCodeSize_Alphabet;
	int fCodeSize_Read;

	int fCode_Clear;
	int fCode_FIN;
	int fCode_FirstAvailable;
	int fCode_Last;
	int fCode_ABCABCA;

	uint8* fStack;
	uint8* fStackEnd;
	uint8* fStackTop;

	uint16* fPrefix;
	uint8* fSuffix;

	ZBitReaderBE fBitReader;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWEncode

/// A write filter stream that LZW-compresses to a destination stream.

class ZStreamW_LZWEncode : public ZStreamW
	{
public:
	ZStreamW_LZWEncode(int iCodeSize_Alphabet, const ZStreamW& iStreamSink);
	~ZStreamW_LZWEncode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;

	int fCodeSize_Alphabet;
	int fCodeSize_Emitted;

	int fCode_Clear;
	int fCode_FIN;
	int fCode_FirstAvailable;

	uint8* fCodes_Suffix;
	uint16* fCodes_Prefix;
	uint16* fChild;

	uint16 fPendingPrefix;

	bool  fFresh;

	ZBitWriterBE fBitWriter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWEncodeNoPatent

class ZStreamW_LZWEncodeNoPatent : public ZStreamW
	{
public:
	ZStreamW_LZWEncodeNoPatent(int iCodeSize_Alphabet, const ZStreamW& iStreamSink);
	~ZStreamW_LZWEncodeNoPatent();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;

	int fCodeSize_Alphabet;
	int fCodeSize_Emitted;

	int fCode_Clear;
	int fCode_FIN;

	size_t fCountSinceClear;

	ZBitWriterBE fBitWriter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWDecode

/// An output filter stream that LZW-decompresses to destination stream.

class ZStreamW_LZWDecode : public ZStreamW
	{
public:
	ZStreamW_LZWDecode(int iCodeSize_Alphabet, const ZStreamW& iStreamSink);
	~ZStreamW_LZWDecode();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);
	virtual void Imp_Flush();

protected:
	const ZStreamW& fStreamSink;

	bool fSeenFIN;

	int fCodeSize_Alphabet;
	int fCodeSize_Read;

	int fCode_Clear;
	int fCode_FIN;
	int fCode_FirstAvailable;
	int fCode_Last;
	int fCode_ABCABCA;

	uint8* fStack;
	uint8* fStackEnd;
	uint8* fStackTop;

	uint16* fPrefix;
	uint8* fSuffix;

	ZBitReaderBE fBitReader;
	};

// =================================================================================================

#endif // __ZStream_LZW__
