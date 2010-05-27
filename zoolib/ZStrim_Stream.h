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

#ifndef __ZStrim_Stream__
#define __ZStrim_Stream__ 1

#include "zoolib/ZStrim.h"

namespace ZooLib {

class ZStreamR;
class ZStreamW;
class ZTextDecoder;
class ZTextEncoder;

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamUTF32BE

/// A read strim that sources text by reading big endian UTF-32 code units from a ZStreamR.

class ZStrimR_StreamUTF32BE : public ZStrimR
	{
public:
	ZStrimR_StreamUTF32BE(const ZStreamR& iStreamR);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

private:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamUTF32LE

/// A read strim that sources text by reading little endian UTF-32 code units from a ZStreamR.

class ZStrimR_StreamUTF32LE : public ZStrimR
	{
public:
	ZStrimR_StreamUTF32LE(const ZStreamR& iStreamR);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

private:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamUTF16BE

/// A read strim that sources text by reading big endian UTF-16 code units from a ZStreamR.

class ZStrimR_StreamUTF16BE : public ZStrimR
	{
public:
	ZStrimR_StreamUTF16BE(const ZStreamR& iStreamR);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

private:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamUTF16LE

/// A read strim that sources text by reading little endian UTF-16 code units from a ZStreamR.

class ZStrimR_StreamUTF16LE : public ZStrimR
	{
public:
	ZStrimR_StreamUTF16LE(const ZStreamR& iStreamR);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

private:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamUTF8

/// A read strim that sources text by reading UTF-8 code units from a ZStreamR.

class ZStrimR_StreamUTF8 : public ZStrimR
	{
public:
	ZStrimR_StreamUTF8(const ZStreamR& iStreamR);

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

	virtual void Imp_ReadUTF8(UTF8*
		oDest, size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP);

private:
	const ZStreamR& fStreamR;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamUTF32BE

/// A write strim that writes text to a ZStreamW in big endian UTF-32.

class ZStrimW_StreamUTF32BE : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_StreamUTF32BE(const ZStreamW& iStreamW);

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamUTF32LE

/// A write strim that writes text to a ZStreamW in little endian UTF-32.

class ZStrimW_StreamUTF32LE : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_StreamUTF32LE(const ZStreamW& iStreamW);

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamUTF16BE

/// A write strim that writes text to a ZStreamW in big endian UTF-16.

class ZStrimW_StreamUTF16BE : public ZStrimW_NativeUTF16
	{
public:
	ZStrimW_StreamUTF16BE(const ZStreamW& iStreamW);

// From ZStrimW via ZStrimW_NativeUTF16
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamUTF16LE

/// A write strim that writes text to a ZStreamW in little endian UTF-16.

class ZStrimW_StreamUTF16LE : public ZStrimW_NativeUTF16
	{
public:
	ZStrimW_StreamUTF16LE(const ZStreamW& iStreamW);

// From ZStrimW via ZStrimW_NativeUTF16
	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamUTF8

/// A write strim that writes text to a ZStreamW in UTF-8.

class ZStrimW_StreamUTF8 : public ZStrimW_NativeUTF8
	{
public:
	ZStrimW_StreamUTF8(const ZStreamW& iStreamW);

// From ZStrimW via ZStrimW_NativeUTF8
	virtual void Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

private:
	const ZStreamW& fStreamW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_StreamDecoder

/** A read strim that provides text by reading it from a ZStreamR in some encoding, using
a ZTextDecoder instance to convert it to Unicode. */

class ZStrimR_StreamDecoder : public ZStrimR
	{
public:
	ZStrimR_StreamDecoder(ZTextDecoder* iDecoder, const ZStreamR& iStreamR);
	ZStrimR_StreamDecoder(const std::pair<ZTextDecoder*, const ZStreamR*>& iParam);
	~ZStrimR_StreamDecoder();

// From ZStrimR
	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);

// Our protocol
	void SetDecoder(ZTextDecoder* iDecoder);
	ZTextDecoder* SetDecoderReturnOld(ZTextDecoder* iDecoder);

private:
	const ZStreamR& fStreamR;
	ZTextDecoder* fDecoder;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_StreamEncoder

/** A write strim that disposes of text written to it by
writing it to a ZStreamW using a ZTextEncoder. */

class ZStrimW_StreamEncoder : public ZStrimW_NativeUTF32
	{
public:
	ZStrimW_StreamEncoder(ZTextEncoder* iEncoder, const ZStreamW& iStreamW);
	ZStrimW_StreamEncoder(const std::pair<ZTextEncoder*, const ZStreamW*>& iParam);
	~ZStrimW_StreamEncoder();

// From ZStrimW via ZStrimW_NativeUTF32
	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
	virtual void Imp_Flush();

// Our protocol
	void SetEncoder(ZTextEncoder* iEncoder);
	ZTextEncoder* SetEncoderReturnOld(ZTextEncoder* iEncoder);

private:
	const ZStreamW& fStreamW;
	ZTextEncoder* fEncoder;
	};

} // namespace ZooLib

#endif // __ZStrim_Stream__
