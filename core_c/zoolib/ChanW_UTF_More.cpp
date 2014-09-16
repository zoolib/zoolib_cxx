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

#include "zoolib/ChanW_UTF_More.h"
#include "zoolib/Unicode.h"

#include "zoolib/ZDebug.h"

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;
#define kDebug_Strim 2

// =================================================================================================
// MARK: - ChanW_UTF_Native32

size_t ChanW_UTF_Native32::Write(const UTF16* iSource, size_t iCountCU)
	{
	const UTF16* localSource = iSource;
	while (iCountCU)
		{
		UTF32 buffer[kBufSize];
		size_t utf16Consumed;
		size_t utf32Generated;
		if (not Unicode::sUTF16ToUTF32(
			localSource, iCountCU,
			&utf16Consumed, nullptr,
			buffer, kBufSize,
			&utf32Generated))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF16 sequence.
			if (utf16Consumed == 0)
				{
				// In fact there was no valid UTF16 in localSource at all, so we'll just bail.
				break;
				}
			}

		size_t utf32Consumed = this->Write(buffer, utf32Generated);
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
	return localSource - iSource;
	}

size_t ChanW_UTF_Native32::Write(const UTF8* iSource, size_t iCountCU)
	{
	const UTF8* localSource = iSource;
	while (iCountCU)
		{
		UTF32 buffer[kBufSize];
		size_t utf8Consumed;
		size_t utf32Generated;
		if (not Unicode::sUTF8ToUTF32(
			localSource, iCountCU,
			&utf8Consumed, nullptr,
			buffer, kBufSize,
			&utf32Generated))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF8 sequence.
			if (utf8Consumed == 0)
				{
				// In fact there was no valid UTF8 in localSource at all, so it's time to bail.
				break;
				}
			}

		size_t utf32Consumed = this->Write(buffer, utf32Generated);
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
	return localSource - iSource;
	}

// =================================================================================================
// MARK: - ChanW_UTF_Native16

size_t ChanW_UTF_Native16::Write(const UTF32* iSource, size_t iCountCU)
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

		ZAssertStop(kDebug_Strim, utf32Consumed <= iCountCU);
		ZAssertStop(kDebug_Strim, utf16Generated <= kBufSize);

		size_t utf16Consumed = this->Write(buffer, utf16Generated);
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

size_t ChanW_UTF_Native16::Write(const UTF8* iSource, size_t iCountCU)
	{
	const UTF8* localSource = iSource;
	while (iCountCU)
		{
		UTF16 buffer[kBufSize];
		size_t utf8Consumed;
		size_t utf16Generated;
		if (not Unicode::sUTF8ToUTF16(
			localSource, iCountCU,
			&utf8Consumed, nullptr,
			buffer, kBufSize,
			&utf16Generated,
			iCountCU, nullptr))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF8 sequence.
			if (utf8Consumed == 0)
				{
				// In fact there was no valid UTF8 in localSource at all, so it's time to bail.
				break;
				}
			}

		size_t utf16Consumed = this->Write(buffer, utf16Generated);
		if (utf16Consumed < utf16Generated)
			{
			// It's a truncated write. We need to convert from utf16Consumed back into
			// the number of utf8 code units.
			const size_t codePoints = Unicode::sCUToCP(buffer, utf16Consumed);
			localSource += Unicode::sCPToCU(localSource, codePoints);
			break;
			}

		localSource += utf8Consumed;
		iCountCU -= utf8Consumed;
		}
	return localSource - iSource;
	}

// =================================================================================================
// MARK: - ChanW_UTF_Native8

size_t ChanW_UTF_Native8::Write(const UTF32* iSource, size_t iCountCU)
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

		ZAssertStop(kDebug_Strim, utf32Consumed <= iCountCU);
		ZAssertStop(kDebug_Strim, utf8Generated <= kBufSize);

		size_t utf8Consumed = this->Write(buffer, utf8Generated);
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

size_t ChanW_UTF_Native8::Write(const UTF16* iSource, size_t iCountCU)
	{
	const UTF16* localSource = iSource;
	while (iCountCU)
		{
		UTF8 buffer[kBufSize];
		size_t utf16Consumed;
		size_t utf8Generated;
		if (not Unicode::sUTF16ToUTF8(
			localSource, iCountCU,
			&utf16Consumed, nullptr,
			buffer, kBufSize,
			&utf8Generated,
			iCountCU, nullptr))
			{
			// localSource[0] to localSource[iCountCU] ends with an incomplete UTF16 sequence.
			if (utf16Consumed == 0)
				{
				// In fact there was no valid UTF16 in localSource at all, so it's time to bail.
				break;
				}
			}

		size_t utf8Consumed = this->Write(buffer, utf8Generated);
		if (utf8Consumed < utf8Generated)
			{
			// It's a truncated write. We need to convert from utf8Consumed back into
			// the number of utf16 code units.
			const size_t codePoints = Unicode::sCUToCP(buffer, utf8Consumed);
			localSource += Unicode::sCPToCU(localSource, codePoints);
			break;
			}

		localSource += utf16Consumed;
		iCountCU -= utf16Consumed;
		}
	return localSource - iSource;
	}

} // namespace ZooLib
