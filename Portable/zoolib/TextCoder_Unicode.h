/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_TextCoder_Unicode_h__
#define __ZooLib_TextCoder_Unicode_h__ 1
#include "zconfig.h"

#include "zoolib/TextCoder.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark

ZRef<TextDecoder> sMake_TextDecoder_Unicode(const std::string& iSourceName);

ZRef<TextEncoder> sMake_TextEncoder_Unicode(const std::string& iDestName);

// =================================================================================================
#pragma mark -
#pragma mark TextDecoder_Unicode_AutoDetect

/** \brief A TextDecoder that treats the source data as being UTF-8, UTF-16BE or UTF-16LE based
on the detection of a BOM (Byte Order Mark) at the beginning of the data. */

class TextDecoder_Unicode_AutoDetect : public TextDecoder
	{
public:
	TextDecoder_Unicode_AutoDetect();

	// From TextDecoder (Callable)
	virtual ZQ<bool> QCall(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);

	// Our protocol
	void Reset();

private:
	enum EType { eFresh, eUTF8, eUTF16BE, eUTF16LE };
	EType fType;
	};

} // namespace ZooLib

#endif // __ZooLib_TextCoder_Unicode_h__
