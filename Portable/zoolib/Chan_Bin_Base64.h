// Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_Bin_Base64_h__
#define __ZooLib_Chan_Bin_Base64_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Base64

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
#pragma mark - ChanR_Bin_Base64Decode

/** A read filter stream that converts base64 data from
the source stream into binary data on the fly.*/

class ChanR_Bin_Base64Decode
:	public ChanR_Bin
	{
public:
	ChanR_Bin_Base64Decode(const ChanR_Bin& iChanR);
	ChanR_Bin_Base64Decode(const Base64::Decode& iDecode, const ChanR_Bin& iChanR);
	~ChanR_Bin_Base64Decode();

// From ChanAspect_Read
	virtual size_t Read(byte* oDest, size_t iCount);

protected:
	const Base64::Decode fDecode;
	const ChanR_Bin& fChanR;
	uint8 fSinkBuf[3];
	size_t fSinkCount;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_Base64Encode

/** A write filter stream that writes to the destination stream the base64
equivalent of binary data written to it. */

class ChanW_Bin_Base64Encode
:	public ChanW_Bin
	{
public:
	ChanW_Bin_Base64Encode(const ChanW_Bin& iChanW);
	ChanW_Bin_Base64Encode(const Base64::Encode& iEncode, const ChanW_Bin& iChanW);
	~ChanW_Bin_Base64Encode();

// From ChanAspect_Write
	virtual size_t Write(const byte* iSource, size_t iCount);
	virtual void Flush();

protected:
	const Base64::Encode fEncode;
	const ChanW_Bin& fChanW;
	uint8 fSourceBuf[3];
	size_t fSourceCount;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_Base64_h__
