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

#ifndef __ZTextCoder_iconv_h__
#define __ZTextCoder_iconv_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TextCoder_iconv
// We can't simply do this:
// #define ZCONFIG_API_Avail__TextCoder_iconv ZCONFIG_SPI_Enabled(iconv)
// because <iconv.h> may #define 'iconv' as 'libiconv', and any use of
// ZCONFIG_SPI_Enabled(iconv) subsequent to the include of iconv.h
// becomes ZCONFIG_SPI_Enabled(libiconv) which of course evaluates to false.
	#if ZCONFIG_SPI_Enabled(iconv)
		#define ZCONFIG_API_Avail__TextCoder_iconv 1
	#else
		#define ZCONFIG_API_Avail__TextCoder_iconv 0
	#endif
#endif

#ifndef ZCONFIG_API_Desired__TextCoder_iconv
	#define ZCONFIG_API_Desired__TextCoder_iconv 1
#endif

#include "zoolib/ZTextCoder.h"

#if ZCONFIG_API_Enabled(TextCoder_iconv)

#include <iconv.h>

namespace ZooLib {

// =================================================================================================
// MARK: - ZTextDecoder_iconv

/// Puts a ZTextDecoder interface over the \a iconv library.

class ZTextDecoder_iconv : public ZTextDecoder
	{
public:
	ZTextDecoder_iconv(const std::string& iSourceName);
	ZTextDecoder_iconv(const char* iSourceName);
	virtual ~ZTextDecoder_iconv();

	using ZTextDecoder::Decode;

	virtual bool Decode
		(const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* oDest, size_t iDestCU, size_t* oDestCU);

	virtual void Reset();

private:
	iconv_t fConverter;
	};

// =================================================================================================
// MARK: - ZTextEncoder_iconv

/// Puts a ZTextEncoder interface over the \a iconv library.

class ZTextEncoder_iconv : public ZTextEncoder
	{
public:
	ZTextEncoder_iconv(const std::string& iDestName);
	ZTextEncoder_iconv(const char* iDestName);
	virtual ~ZTextEncoder_iconv();

	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
		void* oDest, size_t iDestBytes, size_t* oDestBytes);

	virtual void Reset();

private:
	iconv_t fConverter;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCoder_iconv)

#endif // __ZTextCoder_iconv_h__
