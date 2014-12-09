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

#ifndef __ZStream_LZW_h__
#define __ZStream_LZW_h__ 1
#include "zconfig.h"

#include "zoolib/ZBitStream.h"
#include "zoolib/ZStream_Filter.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_LZWEncode

/* This one is not done yet.
class ZStreamR_LZWEncode : public ZStreamR_F
	{
public:
	ZStreamR_LZWEncode(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWEncode();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
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
// MARK: - ZStreamR_LZWEncodeNoPatent

class ZStreamR_LZWEncodeNoPatent : public ZStreamR_Filter
	{
public:
	ZStreamR_LZWEncodeNoPatent(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWEncodeNoPatent();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
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
// MARK: - ZStreamR_LZWDecode

/// A read filter stream that LZW-decompresses a source stream.

class ZStreamR_LZWDecode : public ZStreamR_Filter
	{
public:
	ZStreamR_LZWDecode(int iCodeSize_Alphabet, const ZStreamR& iStreamSource);
	~ZStreamR_LZWDecode();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
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

	uint8 fStack[4096];
	uint8* fStackEnd;
	uint8* fStackTop;

	uint16 fPrefix[4096];
	uint8 fSuffix[4096];

	ZBitReaderBE fBitReader;
	};

// =================================================================================================
// MARK: - ZStreamW_LZWEncode

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

	static const int kTableSize = 5003;

	uint8 fCodes_Suffix[kTableSize];
	uint16 fCodes_Prefix[kTableSize];
	uint16 fChild[kTableSize];

	uint16 fPendingPrefix;

	bool fFresh;

	ZBitWriterBE fBitWriter;
	};

// =================================================================================================
// MARK: - ZStreamW_LZWEncodeNoPatent

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
// MARK: - ZStreamW_LZWDecode

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

	uint8 fStack[4096];
	uint8* fStackEnd;
	uint8* fStackTop;

	uint16 fPrefix[4096];
	uint8 fSuffix[4096];

	ZBitReaderBE fBitReader;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZStream_LZW_h__
