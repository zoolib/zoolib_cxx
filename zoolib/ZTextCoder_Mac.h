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

#ifndef __ZTextCoder_Mac__
#define __ZTextCoder_Mac__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TextCoder_Mac
#	define ZCONFIG_API_Avail__TextCoder_Mac \
	(ZCONFIG_SPI_Enabled(Carbon) \
	|| ZCONFIG_SPI_Enabled(MacOSX) \
	|| ZCONFIG_SPI_Enabled(MacClassic))
#endif

#ifndef ZCONFIG_API_Desired__TextCoder_Mac
#	define ZCONFIG_API_Desired__TextCoder_Mac 1
#endif

#include "zoolib/ZTextCoder.h"

#if ZCONFIG_API_Enabled(TextCoder_Mac)

#include ZMACINCLUDE3(CoreServices,CarbonCore,UnicodeConverter.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder_Mac

class ZTextDecoder_Mac : public ZTextDecoder
	{
public:
	ZTextDecoder_Mac(const char* iName);
	ZTextDecoder_Mac(const std::string& iName);
	ZTextDecoder_Mac(TextEncoding iSourceEncoding);
	virtual ~ZTextDecoder_Mac();

	using ZTextDecoder::Decode;

	virtual bool Decode(
		const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
		UTF32* iDest, size_t iDestCU, size_t* oDestCU);

private:
	void Init(TextEncoding iSourceEncoding);

	TextToUnicodeInfo fInfo;
	bool fIsReset;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder_Mac

class ZTextEncoder_Mac : public ZTextEncoder
	{
public:
	ZTextEncoder_Mac(const char* iName);
	ZTextEncoder_Mac(const std::string& iName);
	ZTextEncoder_Mac(TextEncoding iDestEncoding);
	virtual ~ZTextEncoder_Mac();

	using ZTextEncoder::Encode;

	virtual void Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* iDest, size_t iDestBytes, size_t* oDestBytes);
private:
	void Init(TextEncoding iDestEncoding);

	UnicodeToTextInfo fInfo;
	bool fIsReset;
	};

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(TextCoder_Mac)

#endif // __ZTextCoder_Mac__
