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

#include "zoolib/ZTextCoder_Mac.h"

#if ZCONFIG_API_Enabled(TextCoder_Mac)

#include "zoolib/ZDebug.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUnicode.h"

#include <stdexcept>

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacErrors.h)
#include ZMACINCLUDE3(CoreServices,CarbonCore,TextEncodingConverter.h)

ZAssertCompile(sizeof(UniChar) == sizeof(UTF16));

static const size_t kBufSize = ZooLib::sStackBufferSize;

using std::min;
using std::runtime_error;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Factory functions

#include "zoolib/ZFactoryChain.h"

namespace ZANONYMOUS {

bool sMake_Decoder(ZTextDecoder*& oResult, const string& iParam)
	{
	try
		{
		oResult = new ZTextDecoder_Mac(iParam);
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
		oResult = new ZTextEncoder_Mac(iParam);
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
#pragma mark * ZTextCoder_Mac

static TextEncoding sLookupName(const string& iName)
	{
	TextEncoding theTE;
	Str255 theNameStr255;
	ZString::sToPString(iName, theNameStr255, 255);
	if (noErr == TECGetTextEncodingFromInternetName(&theTE, theNameStr255))
		return theTE;

	// Try again, forcing lower case
	ZString::sToPString(ZUnicode::sToLower(iName), theNameStr255, 255);
	if (noErr == TECGetTextEncodingFromInternetName(&theTE, theNameStr255))
		return theTE;

	// Work our way through any aliases ZTextCoder may know about
	vector<string> aliases;
	ZTextCoder::sGetAliases(iName, aliases);
	for (vector<string>::iterator i = aliases.begin(); i != aliases.end(); ++i)
		{
		ZString::sToPString(ZUnicode::sToLower(*i), theNameStr255, 255);
		if (noErr == TECGetTextEncodingFromInternetName(&theTE, theNameStr255))
			return theTE;
		}

	return 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextDecoder_Mac

ZTextDecoder_Mac::ZTextDecoder_Mac(const char* iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextDecoder_Mac::ZTextDecoder_Mac(const string& iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextDecoder_Mac::ZTextDecoder_Mac(TextEncoding iSourceEncoding)
	{
	this->Init(iSourceEncoding);
	}

ZTextDecoder_Mac::~ZTextDecoder_Mac()
	{
	::DisposeTextToUnicodeInfo(&fInfo);
	}

static ByteOffset sSourceOffsets[kBufSize];
static bool sInitedSourceOffsets = false;

bool ZTextDecoder_Mac::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* iDest, size_t iDestCU, size_t* oDestCU)
	{
	// When we're working with a destination buffer that can't hold all the source material
	// we have ConvertFromTextToUnicode write into the local array 'offsets' the byte
	// offsets that correspond to each UTF-16 intermediate code unit generated.
	// sSourceOffsets is used to tell ConvertFromTextToUnicode which offsets are
	// 'significant', in our case that means all of them. We populate sSourceOffsets
	// the first time we need to use it, it's then available for use in all
	// subsequent invocations.
	ByteOffset offsets[kBufSize];
	ByteCount countOffsets;

	// utf16Buffer is the target for calls to ConvertFromTextToUnicode,
	// we use ZUnicode::sUTF16ToUTF32 to further transcribe the generated UTF-16
	// into the UTF-32 we need.
	UniChar utf16Buffer[kBufSize];

	const uint8* localSource = static_cast<const uint8*>(iSource);
	UTF32* localDest = iDest;
	bool sourceComplete = true;

	size_t sourceBytesSkipped = 0;

	OptionBits flags = 0;
	if (fIsReset)
		fIsReset = false;
	else
		flags |= kUnicodeKeepInfoMask;

	while (iSourceBytes && iDestCU)
		{
		ByteCount sourceConsumed;
		ByteCount utf16Generated;
		OSStatus err;
		if (iDestCU < iSourceBytes && iDestCU < kBufSize)
			{
			// We have space for fewer UTF-32 code units than we have source code units,
			// so it's at least feasible that we'd overflow the destination. We also
			// have space for fewer code units than we have for intermediate UTF-16
			// code units and so this iteration may be the one where we have to
			// bail out early. So we pass the offset array to ConvertFromTextToUnicode in
			// order to discover where any truncation in the source occurred.
			if (!sInitedSourceOffsets)
				{
				// This is thread safe, even if two threads execute this at the same time
				// the end result will be the same -- an array of kBufSize elements containing
				// values from 0 to kBufSize - 1, and sInitedSourceOffsets being true.
				for (size_t x = 0; x < kBufSize; ++x)
					sSourceOffsets[x] = x;
				sInitedSourceOffsets = true;
				}
			size_t countToConvert = min(iSourceBytes, kBufSize);
			err = ::ConvertFromTextToUnicode(fInfo, countToConvert, localSource, 
				flags,
				countToConvert, sSourceOffsets, &countOffsets, offsets,
				kBufSize * sizeof(UniChar), &sourceConsumed, &utf16Generated, utf16Buffer);
			}
		else
			{
			err = ::ConvertFromTextToUnicode(fInfo, iSourceBytes, localSource, 
				flags,
				0, nil, nil, nil, // Offset array stuff.
				kBufSize * sizeof(UniChar), &sourceConsumed, &utf16Generated, utf16Buffer);
			}

		if (sourceConsumed == 0)
			{
			// ConvertFromTextToUnicode was unable to consume any source data.
			if (err == noErr)
				{
				// This is actually an unlikely return value. Let's flag it for now and see.
				ZDebugStopf(1, ("ConvertFromTextToUnicode returned noErr"));
				break;
				}
			else if (err == kTECPartialCharErr)
				{
				// An incomplete sequence was all that was present in the source. 
				sourceComplete = false;
				break;
				}
			else if (err == kTextMalformedInputErr)
				{
				// Bad source data. Skip a byte and try again.
				++localSource;
				--iSourceBytes;
				++sourceBytesSkipped;
				}
			else if (err == kTECUsedFallbacksStatus)
				{
				// Ignore.
				}
			else if (err == kTECBufferBelowMinimumSizeErr)
				{
				// This shouldn't ever happen as we have a fair sized
				// intermediate buffer, and we're not decomposing source characters
				// into multiple Unicode characters.
				sourceComplete = false;
				break;
				}
			else
				{
				ZDebugStopf(1, ("ConvertFromTextToUnicode returned err %d", err));
				}
			}
		else
			{
			ZAssertStopf(1, (utf16Generated & 1) == 0, ("utf16Generated should be even")); 
			utf16Generated /= 2;

			size_t utf16Consumed;
			size_t utf32Generated;
			ZUnicode::sUTF16ToUTF32(
				reinterpret_cast<const UTF16*>(utf16Buffer), utf16Generated, &utf16Consumed,
				localDest, iDestCU, &utf32Generated);

			if (utf16Generated > utf16Consumed)
				{
				// We were not able to consume all the utf16 data generated by
				// ConvertFromTextToUnicode. So the number of source code units
				// consumed is *not* sourceConsumed, but some lesser number.
				ZAssertStop(1, iDestCU < iSourceBytes && iDestCU < kBufSize);
				sourceConsumed = offsets[utf16Consumed - 1];
				}
			localSource += sourceConsumed;
			iSourceBytes -= sourceConsumed;
			localDest += utf32Generated;
			iDestCU -= utf32Generated;
			}
		}
	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = sourceBytesSkipped;
	if (oDestCU)
		*oDestCU = localDest - iDest;
	return true;
	}

void ZTextDecoder_Mac::Init(TextEncoding iSourceEncoding)
	{
	fIsReset = true;

	// Note: We can't use kUnicode32BitFormat, the Unicode converter does not support it.
	UnicodeMapping theMapping;
	theMapping.unicodeEncoding = ::CreateTextEncoding(
		kTextEncodingUnicodeV3_2, kTextEncodingDefaultVariant, kUnicode16BitFormat);

	theMapping.otherEncoding = iSourceEncoding;
	theMapping.mappingVersion = kUnicodeUseLatestMapping;
	if (noErr != ::CreateTextToUnicodeInfo(&theMapping, &fInfo))
		throw runtime_error("ZTextDecoder_Mac, couldn't create converter");
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTextEncoder_Mac

ZTextEncoder_Mac::ZTextEncoder_Mac(const char* iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextEncoder_Mac::ZTextEncoder_Mac(const string& iName)
	{
	this->Init(sLookupName(iName));
	}

ZTextEncoder_Mac::ZTextEncoder_Mac(TextEncoding iDestEncoding)
	{
	this->Init(iDestEncoding);
	}

ZTextEncoder_Mac::~ZTextEncoder_Mac()
	{
	::DisposeUnicodeToTextInfo(&fInfo);
	}

void ZTextEncoder_Mac::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
					void* iDest, size_t iDestBytes, size_t* oDestBytes)
	{
	// utf16Buffer is the source for calls to ConvertFromUnicodeToText, we use
	// ZUnicode::sUTF32ToUTF16 to populate it from the UTF-32 we're passed.
	UniChar utf16Buffer[kBufSize];

	const UTF32* localSource = iSource;
	uint8* localDest = static_cast<uint8*>(iDest);

	// We allow the use of fallbacks, but have installed a custom fallback handler
	// that emits no code units. So Unicode code points which the encoder can't
	// handle are simply skipped, in line with our normal practice.
	OptionBits flags = kUnicodeUseFallbacksMask;
	if (fIsReset)
		fIsReset = false;
	else
		flags |= kUnicodeKeepInfoMask;

	while (iSourceCU && iDestBytes)
		{
		size_t utf32Consumed;
		size_t utf16Generated;
		ZUnicode::sUTF32ToUTF16(localSource, iSourceCU, &utf32Consumed,
						reinterpret_cast<UTF16*>(utf16Buffer), kBufSize, &utf16Generated,
						nil);

		ByteCount utf16Consumed;
		ByteCount destGenerated;
		OSStatus err = ::ConvertFromUnicodeToText(fInfo, utf16Generated * 2, utf16Buffer, 
			flags,
			0, nil, nil, nil, // Offset array stuff.
			iDestBytes, &utf16Consumed, &destGenerated, localDest);

		ZAssertStopf(1, (utf16Consumed & 1) == 0, ("utf16Consumed should be even")); 
		utf16Consumed /= 2;
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
		*oDestBytes = localDest - static_cast<uint8*>(iDest);
	}

static pascal OSStatus sUnicodeToTextFallback_Null(UniChar *iSrcUniStr, ByteCount iSrcUniStrLen,
	ByteCount *oSrcConvLen,
	TextPtr oDestStr, ByteCount iDestStrLen, ByteCount *oDestConvLen,
	LogicalAddress iInfoPtr, ConstUnicodeMappingPtr iUnicodeMappingPtr)
	{
	*oSrcConvLen = iSrcUniStrLen;
	*oDestConvLen = 0;
	return noErr;
	}

static UnicodeToTextFallbackUPP sUnicodeToTextFallback_NullUPP
	= NewUnicodeToTextFallbackUPP(sUnicodeToTextFallback_Null);

void ZTextEncoder_Mac::Init(TextEncoding iDestEncoding)
	{
	fIsReset = true;

	UnicodeMapping theMapping;
	theMapping.unicodeEncoding = ::CreateTextEncoding(
		kTextEncodingUnicodeV3_2, kTextEncodingDefaultVariant, kUnicode16BitFormat);

	theMapping.otherEncoding = iDestEncoding;
	theMapping.mappingVersion = kUnicodeUseLatestMapping;
	if (noErr != ::CreateUnicodeToTextInfo(&theMapping, &fInfo))
		throw runtime_error("ZTextDecoder_Mac, couldn't create converter");

	::SetFallbackUnicodeToText(fInfo, sUnicodeToTextFallback_NullUPP,
		kUnicodeFallbackCustomOnly | kUnicodeFallbackInterruptSafeMask, nil);
	}

#endif // ZCONFIG_API_Enabled(TextCoder_Mac)
