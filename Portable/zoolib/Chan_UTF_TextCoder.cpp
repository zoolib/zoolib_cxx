// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Chan_UTF_TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_TextDecoder

ChanR_UTF_TextDecoder::ChanR_UTF_TextDecoder(
	const ZP<TextDecoder>& iTextDecoder, const ChanR_Bin& iChan)
:	fTextDecoder(iTextDecoder)
,	fChan(iChan)
	{}

ChanR_UTF_TextDecoder::~ChanR_UTF_TextDecoder()
	{}

size_t ChanR_UTF_TextDecoder::Read(UTF32* oDest, size_t iCountCU)
	{
	size_t countProduced;
	sDecode(fChan, fTextDecoder, oDest, iCountCU, &countProduced);
	return countProduced;
	}

// =================================================================================================
#pragma mark - ChanW_UTF_TextEncoder

ChanW_UTF_TextEncoder::ChanW_UTF_TextEncoder(
	const ZP<TextEncoder>& iTextEncoder, const ChanW_Bin& iChan)
:	fTextEncoder(iTextEncoder)
,	fChan(iChan)
	{}

ChanW_UTF_TextEncoder::~ChanW_UTF_TextEncoder()
	{}

size_t ChanW_UTF_TextEncoder::Write(const UTF32* iSource, size_t iCountCU)
	{
	size_t countConsumed;
	sEncode(iSource, iCountCU, &countConsumed, fTextEncoder, fChan);
	return countConsumed;
	}

// =================================================================================================

} // namespace ZooLib
