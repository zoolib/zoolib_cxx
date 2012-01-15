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

#ifndef __ZTextCoder_Unicode_h__
#define __ZTextCoder_Unicode_h__ 1
#include "zconfig.h"

#include "zoolib/ZTextCoder.h"

#include "zoolib/ZCompat_MSVCStaticLib.h"
ZMACRO_MSVCStaticLib_Reference(TextCoder_Unicode)

namespace ZooLib {

// =================================================================================================
// MARK: - ZTextDecoder_UTF32BE

class ZTextDecoder_UTF32BE : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
// MARK: - ZTextEncoder_UTF32BE

class ZTextEncoder_UTF32BE : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
// MARK: - ZTextDecoder_UTF32LE

class ZTextDecoder_UTF32LE : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
// MARK: - ZTextEncoder_UTF32LE

class ZTextEncoder_UTF32LE : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
// MARK: - ZTextDecoder_UTF16BE

class ZTextDecoder_UTF16BE : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
// MARK: - ZTextEncoder_UTF16BE

class ZTextEncoder_UTF16BE : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
// MARK: - ZTextDecoder_UTF16LE

class ZTextDecoder_UTF16LE : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
// MARK: - ZTextEncoder_UTF16LE

class ZTextEncoder_UTF16LE : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
// MARK: - ZTextDecoder_UTF8

class ZTextDecoder_UTF8 : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
// MARK: - ZTextEncoder_UTF8

class ZTextEncoder_UTF8 : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
// MARK: - ZTextDecoder_Unicode_AutoDetect

/** \brief A ZTextDecoder that treats the source data as being UTF-8, UTF-16BE or UTF-16LE based
on the detection of a BOM (Byte Order Mark) at the beginning of the data. */

class ZTextDecoder_Unicode_AutoDetect : public ZTextDecoder
	{
public:
	ZTextDecoder_Unicode_AutoDetect();

	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);

	virtual void Reset();

private:
	enum EType { eFresh, eUTF8, eUTF16BE, eUTF16LE };
	EType fType;
	};

} // namespace ZooLib

#endif // __ZTextCoder_Unicode_h__
