/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTextCoder_Std_h__
#define __ZTextCoder_Std_h__ 1
#include "zconfig.h"

#include "zoolib/ZTextCoder.h"

ZMACRO_MSVCStaticLib_Reference(TextCoder_Std)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZTextDecoder_ASCII

class ZTextDecoder_ASCII : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextEncoder_ASCII

class ZTextEncoder_ASCII : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextDecoder_ISO8859_1

class ZTextDecoder_ISO8859_1 : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextEncoder_ISO8859_1

class ZTextEncoder_ISO8859_1 : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextDecoder_MacRoman

class ZTextDecoder_MacRoman : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextEncoder_MacRoman

class ZTextEncoder_MacRoman : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextDecoder_CP1252

class ZTextDecoder_CP1252 : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextEncoder_CP1252

class ZTextEncoder_CP1252 : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextDecoder_CP850

class ZTextDecoder_CP850 : public ZTextDecoder
	{
public:
	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);
	};

// =================================================================================================
#pragma mark -
#pragma mark ZTextEncoder_CP850

class ZTextEncoder_CP850 : public ZTextEncoder
	{
public:
	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);
	};

} // namespace ZooLib

#endif // __ZTextCoder_Std_h__
