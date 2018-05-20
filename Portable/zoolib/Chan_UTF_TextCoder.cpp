/* -------------------------------------------------------------------------------------------------
Copyright (c) 2018 Andrew Green
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

#include "zoolib/Chan_UTF_TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_TextDecoder

ChanR_UTF_TextDecoder::ChanR_UTF_TextDecoder(
	const ZRef<TextDecoder>& iTextDecoder, const ChanR_Bin& iChan)
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
#pragma mark -
#pragma mark ChanW_UTF_TextEncoder

ChanW_UTF_TextEncoder::ChanW_UTF_TextEncoder(
	const ZRef<TextEncoder>& iTextEncoder, const ChanW_Bin& iChan)
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
