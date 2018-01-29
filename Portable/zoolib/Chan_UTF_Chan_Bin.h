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

#ifndef __ZooLib_Chan_UTF_Chan_Bin_h__
#define __ZooLib_Chan_UTF_Chan_Bin_h__ 1

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF.h"

namespace ZooLib {

//class ZTextDecoder;
//class ZTextEncoder;

//// =================================================================================================
//#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF32BE
//
///// A read strim that sources text by reading big endian UTF-32 code units from a ZStreamR.
//
//class ChanR_UTF_Chan_Bin_UTF32BE : public ZStrimR
//	{
//public:
//	ChanR_UTF_Chan_Bin_UTF32BE(const ZStreamR& iStreamR);
//
//// From ZStrimR
//	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);
//
//private:
//	const ZStreamR& fStreamR;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF32LE
//
///// A read strim that sources text by reading little endian UTF-32 code units from a ZStreamR.
//
//class ChanR_UTF_Chan_Bin_UTF32LE : public ZStrimR
//	{
//public:
//	ChanR_UTF_Chan_Bin_UTF32LE(const ZStreamR& iStreamR);
//
//// From ZStrimR
//	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);
//
//private:
//	const ZStreamR& fStreamR;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF16BE
//
///// A read strim that sources text by reading big endian UTF-16 code units from a ZStreamR.
//
//class ChanR_UTF_Chan_Bin_UTF16BE : public ZStrimR
//	{
//public:
//	ChanR_UTF_Chan_Bin_UTF16BE(const ZStreamR& iStreamR);
//
//// From ZStrimR
//	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);
//
//private:
//	const ZStreamR& fStreamR;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF16LE
//
///// A read strim that sources text by reading little endian UTF-16 code units from a ZStreamR.
//
//class ChanR_UTF_Chan_Bin_UTF16LE : public ZStrimR
//	{
//public:
//	ChanR_UTF_Chan_Bin_UTF16LE(const ZStreamR& iStreamR);
//
//// From ZStrimR
//	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);
//
//private:
//	const ZStreamR& fStreamR;
//	};

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_UTF8

/// A read strim that sources text by reading UTF-8 code units from a ZStreamR.

class ChanR_UTF_Chan_Bin_UTF8
:	public ChanR_UTF
	{
public:
	ChanR_UTF_Chan_Bin_UTF8(const ChanR_Bin& iChanR_Bin);

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCount);

private:
	const ChanR_Bin& fChanR_Bin;
	};

//// =================================================================================================
//#pragma mark -
#pragma mark ZStrimW_StreamUTF32BE
//
///// A write strim that writes text to a ZStreamW in big endian UTF-32.
//
//class ZStrimW_StreamUTF32BE : public ZStrimW_NativeUTF32
//	{
//public:
//	ZStrimW_StreamUTF32BE(const ZStreamW& iStreamW);
//
//// From ZStrimW via ZStrimW_NativeUTF32
//	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
//	virtual void Imp_Flush();
//
//private:
//	const ZStreamW& fStreamW;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ZStrimW_StreamUTF32LE
//
///// A write strim that writes text to a ZStreamW in little endian UTF-32.
//
//class ZStrimW_StreamUTF32LE : public ZStrimW_NativeUTF32
//	{
//public:
//	ZStrimW_StreamUTF32LE(const ZStreamW& iStreamW);
//
//// From ZStrimW via ZStrimW_NativeUTF32
//	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
//	virtual void Imp_Flush();
//
//private:
//	const ZStreamW& fStreamW;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ZStrimW_StreamUTF16BE
//
///// A write strim that writes text to a ZStreamW in big endian UTF-16.
//
//class ZStrimW_StreamUTF16BE : public ZStrimW_NativeUTF16
//	{
//public:
//	ZStrimW_StreamUTF16BE(const ZStreamW& iStreamW);
//
//// From ZStrimW via ZStrimW_NativeUTF16
//	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
//	virtual void Imp_Flush();
//
//private:
//	const ZStreamW& fStreamW;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ZStrimW_StreamUTF16LE
//
///// A write strim that writes text to a ZStreamW in little endian UTF-16.
//
//class ZStrimW_StreamUTF16LE : public ZStrimW_NativeUTF16
//	{
//public:
//	ZStrimW_StreamUTF16LE(const ZStreamW& iStreamW);
//
//// From ZStrimW via ZStrimW_NativeUTF16
//	virtual void Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU);
//	virtual void Imp_Flush();
//
//private:
//	const ZStreamW& fStreamW;
//	};

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Chan_Bin_UTF8

/// A write strim that writes text to a ZStreamW in UTF-8.

class ChanW_UTF_Chan_Bin_UTF8
:	public ChanW_UTF_Native8
	{
public:
	ChanW_UTF_Chan_Bin_UTF8(const ChanW_Bin& iChanW_Bin);

// From ChanW_UTF
	virtual void Flush();

// From ChanW_UTF_Native8
	virtual size_t WriteUTF8(const UTF8* iSource, size_t iCountCU);

private:
	const ChanW_Bin& fChanW_Bin;
	};

//// =================================================================================================
//#pragma mark -
#pragma mark ChanR_UTF_Chan_Bin_Decoder
//
///** A read strim that provides text by reading it from a ZStreamR in some encoding, using
//a ZTextDecoder instance to convert it to Unicode. */
//
//class ChanR_UTF_Chan_Bin_Decoder : public ZStrimR
//	{
//public:
//	ChanR_UTF_Chan_Bin_Decoder(ZTextDecoder* iDecoder, const ZStreamR& iStreamR);
//	ChanR_UTF_Chan_Bin_Decoder(const std::pair<ZTextDecoder*, const ZStreamR*>& iParam);
//	~ChanR_UTF_Chan_Bin_Decoder();
//
//// From ZStrimR
//	virtual void Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount);
//
//// Our protocol
//	void SetDecoder(ZTextDecoder* iDecoder);
//	ZTextDecoder* GetSetDecoder(ZTextDecoder* iDecoder);
//
//private:
//	const ZStreamR& fStreamR;
//	ZTextDecoder* fDecoder;
//	};
//
//// =================================================================================================
//#pragma mark -
#pragma mark ZStrimW_StreamEncoder
//
///** A write strim that disposes of text written to it by
//writing it to a ZStreamW using a ZTextEncoder. */
//
//class ZStrimW_StreamEncoder : public ZStrimW_NativeUTF32
//	{
//public:
//	ZStrimW_StreamEncoder(ZTextEncoder* iEncoder, const ZStreamW& iStreamW);
//	ZStrimW_StreamEncoder(const std::pair<ZTextEncoder*, const ZStreamW*>& iParam);
//	~ZStrimW_StreamEncoder();
//
//// From ZStrimW via ZStrimW_NativeUTF32
//	virtual void Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU);
//	virtual void Imp_Flush();
//
//// Our protocol
//	void SetEncoder(ZTextEncoder* iEncoder);
//	ZTextEncoder* GetSetEncoder(ZTextEncoder* iEncoder);
//
//private:
//	const ZStreamW& fStreamW;
//	ZTextEncoder* fEncoder;
//	};

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_Chan_Bin_h__
