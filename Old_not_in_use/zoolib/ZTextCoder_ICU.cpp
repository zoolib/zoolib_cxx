// Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#include "zoolib/ZTextCoder_ICU.h"

#if ZCONFIG_API_Enabled(TextCoder_ICU)
ZMACRO_MSVCStaticLib_cpp(TextCoder_ICU)

#include "zoolib/ZDebug.h"
#include "zoolib/ZUnicode.h"

#include <stdexcept>
#include "unicode/ucnv.h"

using std::min;
using std::runtime_error;
using std::string;

namespace ZooLib {

ZAssertCompile(sizeof(UChar) == sizeof(UTF16));

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark - Factory functions

namespace { // anonymous

class Make_Decoder
:	public FunctionChain<ZTextDecoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZTextDecoder_ICU(iParam);
		return true;
		}
	} sMaker0;

class Make_Encoder
:	public FunctionChain<ZTextEncoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		oResult = new ZTextEncoder_ICU(iParam);
		return true;
		}
	} sMaker1;

} // anonymous namespace

// =================================================================================================
#pragma mark - ZTextDecoder_ICU

static void spCallbackCount(const void* iContext, UConverterToUnicodeArgs* toUArgs,
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
	if (not fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setToUCallBack(fConverter, spCallbackCount, &fCountSkipped, nullptr, nullptr, &status);
	}

ZTextDecoder_ICU::ZTextDecoder_ICU(const char* iSourceName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iSourceName, &status);
	if (not fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setToUCallBack(fConverter, spCallbackCount, &fCountSkipped, nullptr, nullptr, &status);
	}

ZTextDecoder_ICU::~ZTextDecoder_ICU()
	{
	::ucnv_close(fConverter);
	}

bool ZTextDecoder_ICU::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	UChar utf16Buffer[kBufSize];

	const char* localSource = static_cast<const char*>(iSource);

	UTF32* localDest = oDest;
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
			nullptr, false, &status);

		size_t sourceConsumed = tempLocalSource - localSource;
		size_t utf16Generated = tempDest - utf16Buffer;

		size_t utf16Consumed;
		size_t utf32Generated;
		ZUnicode::sUTF16ToUTF32(
			reinterpret_cast<const UTF16*>(utf16Buffer), utf16Generated,
			&utf16Consumed, nullptr,
			localDest, iDestCU,
			&utf32Generated);

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
		*oDestCU = localDest - oDest;
	return true;
	}

void ZTextDecoder_ICU::Reset()
	{
	::ucnv_resetToUnicode(fConverter);
	}

// =================================================================================================
#pragma mark - ZTextEncoder_ICU

ZTextEncoder_ICU::ZTextEncoder_ICU(const string& iDestName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iDestName.c_str(), &status);
	if (not fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setFromUCallBack(fConverter, UCNV_FROM_U_CALLBACK_SKIP, nullptr, nullptr, nullptr, &status);
	}

ZTextEncoder_ICU::ZTextEncoder_ICU(const char* iDestName)
	{
	UErrorCode status = U_ZERO_ERROR;
	fConverter = ::ucnv_open(iDestName, &status);
	if (not fConverter)
		throw runtime_error("Couldn't open converter");
	::ucnv_setFromUCallBack(fConverter, UCNV_FROM_U_CALLBACK_SKIP, nullptr, nullptr, nullptr, &status);
	}

ZTextEncoder_ICU::~ZTextEncoder_ICU()
	{
	::ucnv_close(fConverter);
	}

void ZTextEncoder_ICU::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	UChar utf16Buffer[kBufSize];

	const UTF32* localSource = iSource;
	char* localDest = static_cast<char*>(oDest);

	UErrorCode status = U_ZERO_ERROR;

	while (iSourceCU && iDestBytes)
		{
		size_t utf32Consumed;
		size_t utf16Generated;
		ZUnicode::sUTF32ToUTF16(
			localSource, iSourceCU,
			&utf32Consumed, nullptr,
			reinterpret_cast<UTF16*>(utf16Buffer), min(kBufSize, iDestBytes),
			&utf16Generated,
			iSourceCU, nullptr);

		char* tempDest = localDest;
		const UChar* tempLocalSource = utf16Buffer;
		::ucnv_fromUnicode(fConverter,
			&tempDest, tempDest + iDestBytes,
			&tempLocalSource, tempLocalSource + utf16Generated,
			nullptr, false, &status);

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
		*oDestBytes = localDest - static_cast<char*>(oDest);
	}

void ZTextEncoder_ICU::Reset()
	{
	::ucnv_resetFromUnicode(fConverter);
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCoder_ICU)
