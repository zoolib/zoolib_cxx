// Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_TextCoder_h__
#define __ZooLib_TextCoder_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - TextDecoder

typedef Callable<bool(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)>
	TextDecoder;

bool sDecode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	const ZP<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU);

void sDecode(
	const ChanR_Bin& iChanR,
	const ZP<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU);

ZQ<UTF32> sQDecode(const ChanR_Bin& iChanR, const ZP<TextDecoder>& iTextDecoder);

// =================================================================================================
#pragma mark - TextEncoder

typedef Callable<void(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)>
	TextEncoder;

void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZP<TextEncoder>& iTextEncoder,
	void* oDest, size_t iDestBytes, size_t* oDestBytes);

void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZP<TextEncoder>& iTextEncoder,
	const ChanW_Bin& iChanW);

bool sEncode(UTF32 iCP, const ZP<TextEncoder>& iTextEncoder, const ChanW_Bin& iChanW);

} // namespace ZooLib

#endif // __ZooLib_TextCoder_h__
