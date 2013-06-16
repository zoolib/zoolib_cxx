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

#include "zoolib/ZTextCoder_Unicode.h"
#include "zoolib/ZByteSwap.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"

#include <string>

ZMACRO_MSVCStaticLib_cpp(TextCoder_Unicode)

using std::min;
using std::string;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_Decoder
:	public ZFunctionChain_T<ZTextDecoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (false)
			{}
		else if (iParam == "utf-8" || iParam == "utf8")
			{
			oResult = new ZTextDecoder_UTF8;
			}
		else if (iParam == "ucs-4" || iParam == "ucs4"
			|| iParam == "utf-32" || iParam == "utf32")
			{
			if (ZCONFIG(Endian, Big))
				oResult = new ZTextDecoder_UTF32BE;
			else
				oResult = new ZTextDecoder_UTF32LE;
			}
		else if (iParam == "utf-16" || iParam == "utf16")
			{
			if (ZCONFIG(Endian, Big))
				oResult = new ZTextDecoder_UTF16BE;
			else
				oResult = new ZTextDecoder_UTF16LE;
			}
		else if (iParam == "ucs-4be" || iParam == "ucs4be"
			|| iParam == "utf-32be" || iParam == "utf32be")
			{
			oResult = new ZTextDecoder_UTF32BE;
			}
		else if (iParam == "ucs-4le" || iParam == "ucs4le"
			|| iParam == "utf-32le" || iParam == "utf32le")
			{
			oResult = new ZTextDecoder_UTF32LE;
			}
		else if (iParam == "utf-16be" || iParam == "utf16be")
			{
			oResult = new ZTextDecoder_UTF16BE;
			}
		else if (iParam == "utf-16le" || iParam == "utf16le")
			{
			oResult = new ZTextDecoder_UTF16LE;
			}
		else
			{
			return false;
			}
		return true;
		}
	} sMaker0;

class Make_Encoder
:	public ZFunctionChain_T<ZTextEncoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (false)
			{}
		else if (iParam == "utf-8" || iParam == "utf8")
			{
			oResult = new ZTextEncoder_UTF8;
			}
		else if (iParam == "ucs-4" || iParam == "ucs4" || iParam == "utf-32" || iParam == "utf32")
			{
			if (ZCONFIG(Endian, Big))
				oResult = new ZTextEncoder_UTF32BE;
			else
				oResult = new ZTextEncoder_UTF32LE;
			}
		else if (iParam == "utf-16" || iParam == "utf16")
			{
			if (ZCONFIG(Endian, Big))
				oResult = new ZTextEncoder_UTF16BE;
			else
				oResult = new ZTextEncoder_UTF16LE;
			}
		else if (iParam == "ucs-4be" || iParam == "ucs4be" || iParam == "utf-32be" || iParam == "utf32be")
			{
			oResult = new ZTextEncoder_UTF32BE;
			}
		else if (iParam == "ucs-4le" || iParam == "ucs4le" || iParam == "utf-32le" || iParam == "utf32le")
			{
			oResult = new ZTextEncoder_UTF32LE;
			}
		else if (iParam == "utf-16be" || iParam == "utf16be")
			{
			oResult = new ZTextEncoder_UTF16BE;
			}
		else if (iParam == "utf-16le" || iParam == "utf16le")
			{
			oResult = new ZTextEncoder_UTF16LE;
			}
		else
			{
			return false;
			}
		return true;
		}
	} sMaker1;

} // anonymous namespace

// =================================================================================================
// MARK: - Utility functions

static void spCopyUTF32(const void* iSource, void* oDest, size_t iCount)
	{ sMemCopy(oDest, iSource, iCount * sizeof(UTF32)); }

static void spCopyUTF32Swapped(const void* iSource, void* oDest, size_t iCount)
	{
	const UTF32* localSource = static_cast<const UTF32*>(iSource);
	UTF32* localDest = static_cast<UTF32*>(oDest);
	while (iCount--)
		*localDest++ = ZByteSwap_Read32(localSource++);
	}

static bool spDecodeUTF32(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;

	if (iSourceBytes < 4)
		{
		if (oSourceBytes)
			*oSourceBytes = 0;
		if (oDestCU)
			*oDestCU = 0;
		if (iSourceBytes && iDestCU)
			{
			// We had destination space, but insufficient source data.
			return false;
			}
		return true;
		}
	else
		{
		size_t countToCopy = min(iSourceBytes / 4, iDestCU);

		const UTF32* localSource = static_cast<const UTF32*>(iSource);
		UTF32* localDest = static_cast<UTF32*>(oDest);
		if (oSourceBytesSkipped)
			{
			size_t skipped = 0;
			while (countToCopy--)
				{
				UTF32 theCP = *localSource++;
				if (ZUnicode::sIsValid(theCP))
					*localDest++ = theCP;
				else
					++skipped;
				}
			*oSourceBytesSkipped = 4 * skipped;
			}
		else
			{
			while (countToCopy--)
				{
				UTF32 theCP = *localSource++;
				if (ZUnicode::sIsValid(theCP))
					*localDest++ = theCP;
				}
			}

		if (oSourceBytes)
			*oSourceBytes = 4 * (localSource - static_cast<const UTF32*>(iSource));
		if (oDestCU)
			*oDestCU = 4 * (localDest - static_cast<UTF32*>(oDest));
		return true;
		}
	}

static bool spDecodeUTF32Swapped(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;

	if (iSourceBytes < 4)
		{
		if (oSourceBytes)
			*oSourceBytes = 0;
		if (oDestCU)
			*oDestCU = 0;
		if (iSourceBytes && iDestCU)
			{
			// We had destination space, but insufficient source data.
			return false;
			}
		return true;
		}
	else
		{
		size_t countToCopy = min(iSourceBytes / 4, iDestCU);

		const UTF32* localSource = static_cast<const UTF32*>(iSource);
		UTF32* localDest = static_cast<UTF32*>(oDest);
		if (oSourceBytesSkipped)
			{
			size_t skipped = 0;
			while (countToCopy--)
				{
				UTF32 theCP = ZByteSwap_Read32(localSource++);
				if (ZUnicode::sIsValid(theCP))
					*localDest++ = theCP;
				else
					++skipped;
				}
			*oSourceBytesSkipped = 4 * skipped;
			}
		else
			{
			while (countToCopy--)
				{
				UTF32 theCP = ZByteSwap_Read32(localSource++);
				if (ZUnicode::sIsValid(theCP))
					*localDest++ = theCP;
				}
			}

		if (oSourceBytes)
			*oSourceBytes = 4 * (localSource - static_cast<const UTF32*>(iSource));
		if (oDestCU)
			*oDestCU = 4 * (localDest - static_cast<UTF32*>(oDest));
		return true;
		}
	}

static bool spDecodeUTF16(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (iSourceBytes < 2)
		{
		if (oSourceBytes)
			*oSourceBytes = 0;
		if (oSourceBytesSkipped)
			*oSourceBytesSkipped = 0;
		if (oDestCU)
			*oDestCU = 0;
		if (iSourceBytes && iDestCU)
			{
			// We had destination space, but insufficient source data.
			return false;
			}
		return true;
		}
	else
		{
		bool sourceComplete = ZUnicode::sUTF16ToUTF32(
			static_cast<const UTF16*>(iSource), iSourceBytes / 2,
			oSourceBytes, oSourceBytesSkipped,
			oDest, iDestCU,
			oDestCU);

		if (oSourceBytes)
			{
			// oSourceBytes has had the number of UTF16 code units placed in it.
			// The caller works in terms of bytes, so we convert it.
			*oSourceBytes *= 2;
			}

		if (oSourceBytesSkipped)
			*oSourceBytesSkipped *= 2;

		return sourceComplete;
		}
	}

static bool spDecodeUTF16Swapped(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (iSourceBytes < 2)
		{
		if (oSourceBytes)
			*oSourceBytes = 0;
		if (oSourceBytesSkipped)
			*oSourceBytesSkipped = 0;
		if (oDestCU)
			*oDestCU = 0;
		if (iSourceBytes && iDestCU)
			{
			// We had destination space, but insufficient source data.
			return false;
			}
		return true;
		}
	else
		{
		UTF16 buffer[kBufSize];
		const UTF16* localSource = static_cast<const UTF16*>(iSource);
		size_t localSourceCU = iSourceBytes / 2;

		UTF32* localDest = oDest;

		bool sourceComplete = true;
		if (oSourceBytesSkipped)
			{
			*oSourceBytesSkipped = 0;
			while (localSourceCU && iDestCU)
				{
				// Copy some of iSource into buffer, swapping as we go.
				const UTF16* copySource = localSource;
				UTF16* copyDest = buffer;
				size_t countToMove = min(localSourceCU, kBufSize);
				while (countToMove--)
					*copyDest++ = ZByteSwap_Read16(copySource++);

				size_t countConsumed;
				size_t countSkipped;
				size_t countGenerated;
				sourceComplete = ZUnicode::sUTF16ToUTF32(
					buffer, countToMove,
					&countConsumed, &countSkipped,
					localDest, iDestCU,
					&countGenerated);

				ZAssertStop(2, countConsumed == countToMove);

				localSource += countToMove;
				localSource += countToMove;
				localSourceCU -= countToMove;

				*oSourceBytesSkipped += countSkipped * 2;

				localDest += countGenerated;
				iDestCU -= countGenerated;
				}
			}
		else
			{
			while (localSourceCU && iDestCU)
				{
				// Copy some of iSource into buffer, swapping as we go.
				const UTF16* copySource = localSource;
				UTF16* copyDest = buffer;
				size_t countToMove = min(localSourceCU, kBufSize);
				while (countToMove--)
					*copyDest++ = ZByteSwap_Read16(copySource++);

				size_t countConsumed;
				size_t countGenerated;
				sourceComplete = ZUnicode::sUTF16ToUTF32(
					buffer, countToMove,
					&countConsumed, nullptr,
					localDest, iDestCU,
					&countGenerated);

				ZAssertStop(2, countConsumed == countToMove);

				localSource += countToMove;
				localSource += countToMove;
				localSourceCU -= countToMove;

				localDest += countGenerated;
				iDestCU -= countGenerated;
				}
			}

		if (oSourceBytes)
			*oSourceBytes = (localSource - static_cast<const UTF16*>(iSource)) * 2;
		if (oDestCU)
			*oDestCU = localDest - oDest;

		return sourceComplete;
		}
	}

static void spEncodeUTF16(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	ZUnicode::sUTF32ToUTF16(
		iSource, iSourceCU,
		oSourceCU, nullptr,
		static_cast<UTF16*>(oDest), iDestBytes / 2,
		oDestBytes,
		iSourceCU, nullptr);

	if (oDestBytes)
		{
		// oDestBytes has had the number of UTF16 code units placed in it.
		// The caller works in terms of bytes, so we convert it.
		*oDestBytes *= 2;
		}
	}

static void spEncodeUTF16Swapped(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	size_t countGenerated;
	ZUnicode::sUTF32ToUTF16(
		iSource, iSourceCU,
		oSourceCU, nullptr,
		static_cast<UTF16*>(oDest), iDestBytes / 2,
		&countGenerated,
		iSourceCU, nullptr);

	if (oDestBytes)
		*oDestBytes = countGenerated * 2;

	UTF16* swapDest = static_cast<UTF16*>(oDest);
	while (countGenerated--)
		{
		*swapDest = ZByteSwap_Read16(swapDest);
		++swapDest;
		}
	}

static bool spDecodeUTF8(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	return ZUnicode::sUTF8ToUTF32(
		static_cast<const UTF8*>(iSource), iSourceBytes,
		oSourceBytes, oSourceBytesSkipped,
		oDest, iDestCU,
		oDestCU);
	}

static void spEncodeUTF8(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	ZUnicode::sUTF32ToUTF8(
		iSource, iSourceCU,
		oSourceCU, nullptr,
		static_cast<UTF8*>(oDest), iDestBytes,
		oDestBytes,
		iSourceCU, nullptr);
	}

// =================================================================================================
// MARK: - ZTextDecoder_UTF32BE

bool ZTextDecoder_UTF32BE::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		return spDecodeUTF32(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	else
		{
		return spDecodeUTF32Swapped(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	}

// =================================================================================================
// MARK: - ZTextEncoder_UTF32BE

void ZTextEncoder_UTF32BE::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	size_t countToCopy = min(iSourceCU, iDestBytes / 4);

	if (ZCONFIG(Endian, Big))
		spCopyUTF32(iSource, oDest, countToCopy);
	else
		spCopyUTF32Swapped(iSource, oDest, countToCopy);

	if (oSourceCU)
		*oSourceCU = countToCopy;
	if (oDestBytes)
		*oDestBytes = countToCopy * 4;
	}

// =================================================================================================
// MARK: - ZTextDecoder_UTF32LE

bool ZTextDecoder_UTF32LE::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		return spDecodeUTF32Swapped(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	else
		{
		return spDecodeUTF32(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	}

// =================================================================================================
// MARK: - ZTextEncoder_UTF32LE

void ZTextEncoder_UTF32LE::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	size_t countToCopy = min(iSourceCU, iDestBytes / 4);

	if (ZCONFIG(Endian, Big))
		spCopyUTF32Swapped(iSource, oDest, countToCopy);
	else
		spCopyUTF32(iSource, oDest, countToCopy);

	if (oSourceCU)
		*oSourceCU = countToCopy;
	if (oDestBytes)
		*oDestBytes = countToCopy * 4;
	}

// =================================================================================================
// MARK: - ZTextDecoder_UTF16BE

bool ZTextDecoder_UTF16BE::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		return spDecodeUTF16(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	else
		{
		return spDecodeUTF16Swapped(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	}

// =================================================================================================
// MARK: - ZTextEncoder_UTF16BE

void ZTextEncoder_UTF16BE::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	if (ZCONFIG(Endian, Big))
		spEncodeUTF16(iSource, iSourceCU, oSourceCU, oDest, iDestBytes, oDestBytes);
	else
		spEncodeUTF16Swapped(iSource, iSourceCU, oSourceCU, oDest, iDestBytes, oDestBytes);
	}

// =================================================================================================
// MARK: - ZTextDecoder_UTF16LE

bool ZTextDecoder_UTF16LE::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		return spDecodeUTF16Swapped(
			iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	else
		{
		return spDecodeUTF16(
		iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
		}
	}

// =================================================================================================
// MARK: - ZTextEncoder_UTF16LE

void ZTextEncoder_UTF16LE::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	if (ZCONFIG(Endian, Big))
		spEncodeUTF16Swapped(iSource, iSourceCU, oSourceCU, oDest, iDestBytes, oDestBytes);
	else
		spEncodeUTF16(iSource, iSourceCU, oSourceCU, oDest, iDestBytes, oDestBytes);
	}

// =================================================================================================
// MARK: - ZTextDecoder_UTF8

bool ZTextDecoder_UTF8::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	return spDecodeUTF8(
		iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped, oDest, iDestCU, oDestCU);
	}

// =================================================================================================
// MARK: - ZTextEncoder_UTF8

void ZTextEncoder_UTF8::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	spEncodeUTF8(iSource, iSourceCU, oSourceCU, oDest, iDestBytes, oDestBytes);
	}

// =================================================================================================
// MARK: - ZTextDecoder_Unicode_AutoDetect

ZTextDecoder_Unicode_AutoDetect::ZTextDecoder_Unicode_AutoDetect()
:	fType(eFresh)
	{}

// Byte Order Mark
// UTF-8    --> EF BB BF
// UTF-16LE --> FF FE
// UTF-16BE --> FE FF

bool ZTextDecoder_Unicode_AutoDetect::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	if (fType == eFresh)
		{
		if (oSourceBytes)
			*oSourceBytes = 0;
		if (oSourceBytesSkipped)
			*oSourceBytesSkipped = 0;
		if (oDestCU)
			*oDestCU = 0;

		if (iSourceBytes == 0)
			return true;

		const uint8* localSource = static_cast<const uint8*>(iSource);
		uint8 firstByte = *localSource++;
		if (firstByte == 0xFF)
			{
			if (iSourceBytes < 2)
				return false;
			if (*localSource++ == 0xFE)
				fType = eUTF16LE;
			}
		else if (firstByte == 0xFE)
			{
			if (iSourceBytes < 2)
				return false;
			if (*localSource++ == 0xFF)
				fType = eUTF16BE;
			}
		else if (firstByte == 0xEF)
			{
			if (iSourceBytes < 2)
				return false;
			if (*localSource++ == 0xBB)
				{
				if (iSourceBytes < 3)
					return false;
				if (*localSource++ == 0xBF)
					fType = eUTF8;
				}
			}

		if (fType == eFresh)
			{
			// Failed to find a BOM of any sort, so we'll want to
			// read everything as UTF8 from iSource onwards.
			fType = eUTF8;
			}
		else
			{
			// We found a BOM. Record that we've consumed those bytes in oSourceBytes
			// and return. Of course we haven't generated any output, but that's okay.
			// We'll be called again, this time with the BOM skipped over and we
			// can then call the appropriate spDecode method.
			if (oSourceBytes)
				*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
			return true;
			}
		}

	switch (fType)
		{
		case eUTF8:
			{
			return spDecodeUTF8(iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
				oDest, iDestCU, oDestCU);
			}
		case eUTF16BE:
			{
			if (ZCONFIG(Endian, Big))
				{
				return spDecodeUTF16(iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
					oDest, iDestCU, oDestCU);
				}
			else
				{
				return spDecodeUTF16Swapped(iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
					oDest, iDestCU, oDestCU);
				}
			}
		case eUTF16LE:
			{
			if (ZCONFIG(Endian, Big))
				{
				return spDecodeUTF16Swapped(iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
					oDest, iDestCU, oDestCU);
				}
			else
				{
				return spDecodeUTF16(iSource, iSourceBytes, oSourceBytes, oSourceBytesSkipped,
					oDest, iDestCU, oDestCU);
				}
			}
		default:
			break;
		}
	// Can't get here.
	ZUnimplemented();
	return true;
	}

void ZTextDecoder_Unicode_AutoDetect::Reset()
	{ fType = eFresh; }

} // namespace ZooLib
