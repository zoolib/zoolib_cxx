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

#ifndef __ZooLib_Chan_Bin_Base64_h__
#define __ZooLib_Chan_Bin_Base64_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Base64

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

} // namespace Base64

// =================================================================================================
// MARK: - ChanR_Bin_Base64Decode

/** A read filter stream that converts base64 data from
the source stream into binary data on the fly.*/

class ChanR_Bin_Base64Decode
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Base64Decode(const ChanR_Bin& iChanR);
	ChanR_Bin_Base64Decode(const Base64::Decode& iDecode, const ChanR_Bin& iChanR);
	~ChanR_Bin_Base64Decode();

// From ChanR_Bin
	virtual size_t QRead(byte* oDest, size_t iCount);

protected:
	const Base64::Decode fDecode;
	const ChanR_Bin& fChanR;
	uint8 fSinkBuf[3];
	size_t fSinkCount;
	};

// =================================================================================================
// MARK: - ChanW_Bin_Base64Encode

/** A write filter stream that writes to the destination stream the base64
equivalent of binary data written to it. */

class ChanW_Bin_Base64Encode
:	public ChanW_Bin
	{
public:
	ChanW_Bin_Base64Encode(const ChanW_Bin& iChanW);
	ChanW_Bin_Base64Encode(const Base64::Encode& iEncode, const ChanW_Bin& iChanW);
	~ChanW_Bin_Base64Encode();

// From ZStreamW
	virtual size_t QWrite(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	const Base64::Encode fEncode;
	const ChanW_Bin& fChanW;
	uint8 fSourceBuf[3];
	size_t fSourceCount;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_Base64_h__
