/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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
	const ZRef<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU);

void sDecode(
	const ChanR_Bin& iChanR,
	const ZRef<TextDecoder>& iTextDecoder,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU);

ZQ<UTF32> sQDecode(const ChanR_Bin& iChanR, const ZRef<TextDecoder>& iTextDecoder);

// =================================================================================================
#pragma mark - TextEncoder

typedef Callable<void(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)>
	TextEncoder;

void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZRef<TextEncoder>& iTextEncoder,
	void* oDest, size_t iDestBytes, size_t* oDestBytes);

void sEncode(
	const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	const ZRef<TextEncoder>& iTextEncoder,
	const ChanW_Bin& iChanW);

bool sEncode(UTF32 iCP, const ZRef<TextEncoder>& iTextEncoder, const ChanW_Bin& iChanW);

} // namespace ZooLib

#endif // __ZooLib_TextCoder_h__
