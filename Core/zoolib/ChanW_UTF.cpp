/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ChanW_UTF.h"
#include "zoolib/Unicode.h"

#include "zoolib/Stringf.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark Helpers

static bool spQWrite(const ChanW_UTF& iChanW, const UTF32* iSource, size_t iCountCU)
	{
	size_t countWritten;
	sWrite(iSource, iCountCU, &countWritten, iCountCU, nullptr, iChanW);
	return countWritten == iCountCU;
	}

static bool spQWrite(const ChanW_UTF& iChanW, const UTF16* iSource, size_t iCountCU)
	{
	size_t countWritten;
	sWrite(iSource, iCountCU, &countWritten, iCountCU, nullptr, iChanW);
	return countWritten == iCountCU;
	}

static bool spQWrite(const ChanW_UTF& iChanW, const UTF8* iSource, size_t iCountCU)
	{
	size_t countWritten;
	sWrite(iSource, iCountCU, &countWritten, iCountCU, nullptr, iChanW);
	return countWritten == iCountCU;
	}

// =================================================================================================
#pragma mark -
#pragma mark String buffers, limiting and reporting both CU and CP

void sWrite(const UTF32* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	const size_t countWritten = sQWrite(iSource, iCountCU, iChanW);
	if (oCountCU)
		*oCountCU = countWritten;
	if (oCountCP)
		*oCountCP = countWritten;
	}

void sWrite(const UTF16* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	const UTF16* localSource = iSource;
	size_t localCountCP = iCountCP;
	while (iCountCU)
		{
		UTF32 buffer[kBufSize];
		size_t utf16Consumed;
		size_t utf32Generated;
		if (not Unicode::sUTF16ToUTF32(
			localSource, iCountCU,
			&utf16Consumed, nullptr,
			buffer, std::min(localCountCP, kBufSize),
			&utf32Generated))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF16 sequence.
			if (utf16Consumed == 0)
				{
				// In fact there was no valid UTF16 in localSource at all, so we'll just bail.
				break;
				}
			}

		const size_t utf32Consumed = sQWriteFully(iChanW, buffer, utf32Generated);
		localCountCP -= utf32Consumed;
		if (utf32Consumed < utf32Generated)
			{
			// It's a truncated write. We need to convert from utf32Consumed back into
			// the number of utf16 code units.
			localSource += Unicode::sCPToCU(localSource, utf32Consumed);
			break;
			}
		localSource += utf16Consumed;
		iCountCU -= utf16Consumed;
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	}

void sWrite(const UTF8* iSource,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	const ChanW_UTF& iChanW)
	{
	const UTF8* localSource = iSource;
	size_t localCountCP = iCountCP;
	while (iCountCU)
		{
		UTF32 buffer[kBufSize];
		size_t utf8Consumed;
		size_t utf32Generated;
		if (not Unicode::sUTF8ToUTF32(
			localSource, iCountCU,
			&utf8Consumed, nullptr,
			buffer, std::min(localCountCP, kBufSize),
			&utf32Generated))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF8 sequence.
			if (utf8Consumed == 0)
				{
				// In fact there was no valid UTF8 in localSource at all, so we'll just bail.
				break;
				}
			}

		const size_t utf32Consumed = sQWriteFully(iChanW, buffer, utf32Generated);
		localCountCP -= utf32Consumed;
		if (utf32Consumed < utf32Generated)
			{
			// It's a truncated write. We need to convert from utf32Consumed back into
			// the number of utf8 code units.
			localSource += Unicode::sCPToCU(localSource, utf32Consumed);
			break;
			}
		localSource += utf8Consumed;
		iCountCU -= utf8Consumed;
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	}

// =================================================================================================
#pragma mark -
#pragma mark Zero-terminated strings

bool sQWrite(const ChanW_UTF& iChanW, const UTF32* iString)
	{ return spQWrite(iChanW, iString, Unicode::sCountCU(iString)); }

bool sQWrite(const ChanW_UTF& iChanW, const UTF16* iString)
	{ return spQWrite(iChanW, iString, Unicode::sCountCU(iString)); }

bool sQWrite(const ChanW_UTF& iChanW, const UTF8* iString)
	{ return spQWrite(iChanW, iString, Unicode::sCountCU(iString)); }

void sEWrite(const ChanW_UTF& iChanW, const UTF32* iString)
	{ spQWrite(iChanW, iString, Unicode::sCountCU(iString)) || sThrow_ExhaustedW(); }

void sEWrite(const ChanW_UTF& iChanW, const UTF16* iString)
	{ spQWrite(iChanW, iString, Unicode::sCountCU(iString)) || sThrow_ExhaustedW(); }

void sEWrite(const ChanW_UTF& iChanW, const UTF8* iString)
	{ spQWrite(iChanW, iString, Unicode::sCountCU(iString)) || sThrow_ExhaustedW(); }

// =================================================================================================
#pragma mark -
#pragma mark Standard library strings

bool sQWrite(const ChanW_UTF& iChanW, const string32& iString)
	{
	// Some non-conformant string implementations (MS) hate it if you access the buffer underlying
	// an empty string. They don't even return null, they crash or throw an exception.
	const size_t countCU = iString.size();
	return countCU == 0 || spQWrite(iChanW, iString.data(), countCU);
	}

bool sQWrite(const ChanW_UTF& iChanW, const string16& iString)
	{
	const size_t countCU = iString.size();
	return countCU == 0 || spQWrite(iChanW, iString.data(), countCU);
	}

bool sQWrite(const ChanW_UTF& iChanW, const string8& iString)
	{
	const size_t countCU = iString.size();
	return countCU == 0 || spQWrite(iChanW, iString.data(), countCU);
	}

void sEWrite(const ChanW_UTF& iChanW, const string32& iString)
	{
	const size_t countCU = iString.size();
	countCU == 0 || spQWrite(iChanW, iString.data(), countCU) || sThrow_ExhaustedW();
	}

void sEWrite(const ChanW_UTF& iChanW, const string16& iString)
	{
	const size_t countCU = iString.size();
	countCU == 0 || spQWrite(iChanW, iString.data(), countCU) || sThrow_ExhaustedW();
	}

void sEWrite(const ChanW_UTF& iChanW, const string8& iString)
	{
	const size_t countCU = iString.size();
	countCU == 0 || spQWrite(iChanW, iString.data(), countCU) || sThrow_ExhaustedW();
	}

// =================================================================================================
#pragma mark -
#pragma mark Formatted strings

/** Write the zero-terminated UTF-8 string starting at \a iString. Standard printf-style parameter
substitution is applied to the string before writing.
*/
void sEWritef(const ChanW_UTF& iChanW,
	const UTF8* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	size_t countCU_Produced, count_CUWritten;
	sWritev(iChanW, &countCU_Produced, &count_CUWritten, iString, args);
	va_end(args);
	if (count_CUWritten != countCU_Produced)
		sThrow_ExhaustedW();
	}

/** Write the zero-terminated UTF-8 string starting at \a iString. Standard printf-style parameter
substitution is applied to the string before writing. The number of UTF-8 code units produced
is returned in \a oCount_CUProduced. The number of UTF-8 code units successfully written is
returned in \a oCount_CUWritten.
*/
void sWritef(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, ...)
	{
	va_list args;
	va_start(args, iString);
	sWritev(iChanW, oCount_CUProduced, oCount_CUWritten, iString, args);
	va_end(args);
	}

void sEWritev(const ChanW_UTF& iChanW,
	const UTF8* iString, va_list iArgs)
	{
	size_t countCU_Produced, count_CUWritten;
	sWritev(iChanW, &countCU_Produced, &count_CUWritten, iString, iArgs);
	if (count_CUWritten != countCU_Produced)
		sThrow_ExhaustedW();
	}

void sWritev(const ChanW_UTF& iChanW, size_t* oCount_CUProduced, size_t* oCount_CUWritten,
	const UTF8* iString, va_list iArgs)
	{
	const std::string theString = sStringv(iString, iArgs);
	const size_t theCount = theString.size();
	if (oCount_CUProduced)
		*oCount_CUProduced = theCount;

	if (theCount)
		{
		size_t countWritten;
		sWrite(theString.data(), theCount, &countWritten, theCount, nullptr, iChanW);
		if (oCount_CUWritten)
			*oCount_CUWritten = countWritten;
		}
	else
		{
		if (oCount_CUWritten)
			*oCount_CUWritten = 0;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Native16

size_t ChanW_UTF_Native16::QWrite(const UTF32* iSource, size_t iCountCU)
	{
	const UTF32* localSource = iSource;
	while (iCountCU)
		{
		UTF16 buffer[kBufSize];

		size_t utf32Consumed;
		size_t utf16Generated;
		Unicode::sUTF32ToUTF16(
			localSource, iCountCU,
			&utf32Consumed, nullptr,
			buffer, kBufSize,
			&utf16Generated,
			kBufSize, nullptr);

		ZAssert(utf32Consumed <= iCountCU);
		ZAssert(utf16Generated <= kBufSize);

		size_t utf16Consumed = this->QWriteUTF16(buffer, utf16Generated);
		if (utf16Consumed < utf16Generated)
			{
			// It's a truncated write. We need to convert from utf16Consumed back into
			// the number of code points, which is also the number of UTF32 code units.
			localSource += Unicode::sCUToCP(buffer, utf16Consumed);
			break;
			}

		localSource += utf32Consumed;
		iCountCU -= utf32Consumed;
		}
	return localSource - iSource;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanW_UTF_Native8

size_t ChanW_UTF_Native8::QWrite(const UTF32* iSource, size_t iCountCU)
	{
	const UTF32* localSource = iSource;
	while (iCountCU)
		{
		UTF8 buffer[kBufSize];

		size_t utf32Consumed;
		size_t utf8Generated;
		Unicode::sUTF32ToUTF8(
			localSource, iCountCU,
			&utf32Consumed, nullptr,
			buffer, kBufSize,
			&utf8Generated,
			kBufSize, nullptr);

		ZAssert(utf32Consumed <= iCountCU);
		ZAssert(utf8Generated <= kBufSize);

		size_t utf8Consumed = this->QWriteUTF8(buffer, utf8Generated);
		if (utf8Consumed < utf8Generated)
			{
			// It's a truncated write. We need to convert from utf8Consumed back into
			// the number UTF32 code units..
			const size_t codePoints = Unicode::sCUToCP(buffer, utf8Consumed);
			localSource += Unicode::sCPToCU(localSource, codePoints);
			break;
			}

		localSource += utf32Consumed;
		iCountCU -= utf32Consumed;
		}
	return localSource - iSource;
	}

} // namespace ZooLib
