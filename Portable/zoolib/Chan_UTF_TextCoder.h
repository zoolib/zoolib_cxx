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
:	public ChanR_UTF
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
:	public ChanW_UTF_Native32
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
