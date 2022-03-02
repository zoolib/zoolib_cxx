// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Chan_UTF_TextCoder_h__
#define __ZooLib_Chan_UTF_TextCoder_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/ChanW_UTF.h"
#include "zoolib/TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_UTF_TextDecoder

class ChanR_UTF_TextDecoder
:	public virtual ChanR_UTF
	{
public:
	ChanR_UTF_TextDecoder(const ZP<TextDecoder>& iTextDecoder, const ChanR_Bin& iChan);
	~ChanR_UTF_TextDecoder();

// From ChanR_UTF
	virtual size_t Read(UTF32* oDest, size_t iCountCU);

protected:
	const ZP<TextDecoder> fTextDecoder;
	const ChanR_Bin& fChan;
	};

// =================================================================================================
#pragma mark - ChanW_UTF_TextEncoder

class ChanW_UTF_TextEncoder
:	public virtual ChanW_UTF_Native32
	{
public:
	ChanW_UTF_TextEncoder(const ZP<TextEncoder>& iTextEncoder, const ChanW_Bin& iChan);
	~ChanW_UTF_TextEncoder();

// From ChanW_UTF_Native32
	virtual size_t Write(const UTF32* iSource, size_t iCountCU);

private:
	const ZP<TextEncoder> fTextEncoder;
	const ChanW_Bin& fChan;
	};

// =================================================================================================

} // namespace ZooLib

#endif // __ZooLib_Chan_UTF_TextCoder_h__
