/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZTextCoder_ICU.h"

#if ZCONFIG_API_Enabled(TextCoder_ICU)

#include "zoolib/ZDebug.h"
#include "zoolib/ZFactoryChain.h"

#include <stdexcept>
#include "unicode/ucnv.h"

using std::min;
using std::runtime_error;
using std::string;

NAMESPACE_ZOOLIB_USING

ZAssertCompile(sizeof(UChar) == sizeof(UTF16));

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

namespace ZANONYMOUS {

bool sMake_Decoder(ZTextDecoder*& oResult, const string& iParam)
	{
	try
		{
		oResult = new ZTextDecoder_ICU(iParam);
		return true;		
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZTextDecoder*, const string&>
	sMaker_Decoder(sMake_Decoder);

bool sMake_Encoder(ZTextEncoder*& oResult, const string& iParam)
	{
	try
		{
		oResult = new ZTextEncoder_ICU(iParam);
		return true;
		}
	catch (...)
		{}
	return false;
	}

ZFactoryChain_Maker_T<ZTextEncoder*, const string&>
	sMaker_Encoder(sMake_Encoder);

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder_ICU

static void sCallbackCount(const void* iContext, UConverterToUnicodeArgs* toUArgs,
	const char* codePoints, int32_t length, UConverterCallbackReason reason, UErrorCode* err)
	{
	if (reason == UCNV_UNASSIGNED)
		{
		*err = U_ZERO_ERROR;
		*static_cast<size_t*>(const_cast<void*>(iContext)) += length;
		}
	}

ZTextDecoder_ICU::ZTextDecoder_ICU(const string& iSourceName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iSourceName.c_str(), &status);
	if (!fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setToUCallBack(fConverter, sCallbackCount, &fCountSkipped, nil, nil, &status);
	}

ZTextDecoder_ICU::ZTextDecoder_ICU(const char* iSourceName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iSourceName, &status);
	if (!fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setToUCallBack(fConverter, sCallbackCount, &fCountSkipped, nil, nil, &status);
	}

ZTextDecoder_ICU::~ZTextDecoder_ICU()
	{
	::ucnv_close(fConverter);
	}

bool ZTextDecoder_ICU::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* iDest, size_t iDestCU, size_t* oDestCU)
	{
	UChar utf16Buffer[kBufSize];

	const char* localSource = static_cast<const char*>(iSource);

	UTF32* localDest = iDest;
	size_t localDestCU = iDestCU;

	fCountSkipped = 0;

	UErrorCode status = U_ZERO_ERROR;
	while (iSourceBytes && localDestCU)
		{
		const char* tempLocalSource = localSource;
		UChar* tempDest = utf16Buffer;

		::ucnv_toUnicode(fConverter,
						&tempDest, tempDest + min(kBufSize, iDestCU),
						&tempLocalSource, tempLocalSource + iSourceBytes,
						nil, false, &status);

		size_t sourceConsumed = tempLocalSource - localSource;
		size_t utf16Generated = tempDest - utf16Buffer;

		size_t utf16Consumed;
		size_t utf32Generated;
		ZUnicode::sUTF16ToUTF32(
			reinterpret_cast<const UTF16*>(utf16Buffer), utf16Generated, &utf16Consumed,
			localDest, iDestCU, &utf32Generated);

		localSource += sourceConsumed;
		iSourceBytes -= sourceConsumed;
		localDest += utf32Generated;
		iDestCU -= utf32Generated;
		}

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const char*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = fCountSkipped;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	return true;
	}

void ZTextDecoder_ICU::Reset()
	{
	::ucnv_resetToUnicode(fConverter);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder_ICU

ZTextEncoder_ICU::ZTextEncoder_ICU(const string& iDestName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iDestName.c_str(), &status);
	if (!fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setFromUCallBack(fConverter, UCNV_FROM_U_CALLBACK_SKIP, nil, nil, nil, &status);
	}

ZTextEncoder_ICU::ZTextEncoder_ICU(const char* iDestName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iDestName, &status);
	if (!fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setFromUCallBack(fConverter, UCNV_FROM_U_CALLBACK_SKIP, nil, nil, nil, &status);
	}

ZTextEncoder_ICU::~ZTextEncoder_ICU()
	{
	::ucnv_close(fConverter);
	}

void ZTextEncoder_ICU::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* iDest, size_t iDestBytes, size_t* oDestBytes)
	{
	UChar utf16Buffer[kBufSize];

	const UTF32* localSource = iSource;
	char* localDest = static_cast<char*>(iDest);

	UErrorCode status = U_ZERO_ERROR;

	while (iSourceCU && iDestBytes)
		{
		size_t utf32Consumed;
		size_t utf16Generated;
		ZUnicode::sUTF32ToUTF16( localSource, iSourceCU, &utf32Consumed,
			reinterpret_cast<UTF16*>(utf16Buffer), min(kBufSize, iDestBytes), &utf16Generated,
			nil);

		char* tempDest = localDest;
		const UChar* tempLocalSource = utf16Buffer;
		::ucnv_fromUnicode(fConverter,
						&tempDest, tempDest + iDestBytes,
						&tempLocalSource, tempLocalSource + utf16Generated,
						nil, false, &status);

		size_t utf16Consumed = tempLocalSource - utf16Buffer;
		size_t destGenerated = tempDest - localDest;

		if (utf16Generated > utf16Consumed)
			{
			// We were not able to consume all the intermediary UTF-16 that was generated.
			// Turn the number of complete code points consumed back into the number of
			// corresponding UTF-32 code units.
			size_t codePoints = ZUnicode::sCUToCP(
				reinterpret_cast<const UTF16*>(utf16Buffer), utf16Consumed);

			utf32Consumed = ZUnicode::sCPToCU(localSource, codePoints);
			}
		localSource += utf32Consumed;
		iSourceCU -= utf32Consumed;
		localDest += destGenerated;
		iDestBytes -= destGenerated;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<char*>(iDest);
	}

void ZTextEncoder_ICU::Reset()
	{
	::ucnv_resetFromUnicode(fConverter);
	}

#endif // ZCONFIG_API_Enabled(TextCoder_ICU)
