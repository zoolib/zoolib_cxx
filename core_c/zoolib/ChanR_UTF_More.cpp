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

#include "zoolib/ChanR_UTF_More.h"

#include "zoolib/ZMemory.h"
#include "zoolib/ZUnicode.h"

namespace ZooLib {

using std::max;
using std::min;

static const size_t kBufSize = sStackBufferSize;
#define kDebug_Strim 2

#if 0
// =================================================================================================
// MARK: - ChanR_UTF_Native32

void ChanR_UTF_Native32::ReadUTF16(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	UTF32 utf32Buffer[kBufSize];
	UTF16* localDest = oDest;
	size_t localCountCP = iCountCP;
	// If we've got space for two code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 2 && localCountCP)
		{
		const size_t utf32Read = this->Read(utf32Buffer, min(kBufSize, min(localCountCP, iCountCU)));
		if (utf32Read == 0)
			break;

		size_t utf32Consumed;
		size_t utf16Generated;
		size_t cpGenerated;
		ZUnicode::sUTF32ToUTF16(
			utf32Buffer, utf32Read,
			&utf32Consumed, nullptr,
			localDest, iCountCU,
			&utf16Generated,
			localCountCP, &cpGenerated);

		ZAssertStop(kDebug_Strim, utf32Consumed == utf32Read);

		localCountCP -= cpGenerated;
		iCountCU -= utf16Generated;
		localDest += utf16Generated;
		}
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}

void ChanR_UTF_Native32::ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	UTF32 utf32Buffer[kBufSize];
	UTF8* localDest = oDest;
	size_t localCountCP = iCountCP;
	// If we've got space for six code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 6 && localCountCP)
		{
		const size_t utf32Read = this->Read(utf32Buffer, min(kBufSize, min(localCountCP, iCountCU)));
		if (utf32Read == 0)
			break;

		size_t utf32Consumed;
		size_t utf8Generated;
		size_t cpGenerated;
		ZUnicode::sUTF32ToUTF8(
			utf32Buffer, utf32Read,
			&utf32Consumed, nullptr,
			localDest, iCountCU,
			&utf8Generated,
			localCountCP, &cpGenerated);

		ZAssertStop(kDebug_Strim, utf32Consumed == utf32Read);

		localCountCP -= cpGenerated;
		iCountCU -= utf8Generated;
		localDest += utf8Generated;
		}
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}

// =================================================================================================
// MARK: - ChanR_UTF_Native16

size_t ChanR_UTF_Native16::Read(UTF32* oDest, size_t iCountCU)
	{
	ZUnimplemented();

	// This is a problem one -- we may have to nibble-Read and gradually fill the buffer.
	#if 0
	UTF16 utf16Buffer[kBufSize];
	UTF32* localDest = oDest;
	while (iCountCU)
		{
		const size_t utf16Read = this->Read(utf16Buffer, min(kBufSize, max<size_t>(2, iCountCU)));
		if (utf16Read == 0)
			break;

		size_t utf16Consumed;
		size_t utf32Generated;
		ZUnicode::sUTF16ToUTF32(
			utf16Buffer, utf16Read,
			&utf16Consumed, nullptr,
			localDest, iCountCU,
			&utf32Generated);

		ZAssertStop(kDebug_Strim, utf16Consumed == utf16Read);

		iCountCU -= utf32Generated;
		localDest += utf32Generated;
		}
	return localDest - oDest;
	#endif
	}

void ChanR_UTF_Native16::ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	ZUnimplemented();

	UTF16 utf16Buffer[kBufSize];
	UTF8* localDest = oDest;
	size_t localCountCP = iCountCP;
	// If we've got space for six code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 6 && localCountCP)
		{
		const size_t utf16Read = this->Read(utf16Buffer, min(kBufSize, min(localCountCP, iCountCU)));
		if (utf16Read == 0)
			break;

		size_t utf16Consumed;
		size_t utf8Generated;
		size_t cpGenerated;
		ZUnicode::sUTF16ToUTF8(
			utf16Buffer, utf16Read,
			&utf16Consumed, nullptr,
			localDest, iCountCU,
			&utf8Generated,
			localCountCP, &cpGenerated);

		ZAssertStop(kDebug_Strim, utf16Consumed == utf16Read);

		localCountCP -= cpGenerated;
		iCountCU -= utf8Generated;
		localDest += utf8Generated;
		}
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}
#endif

// =================================================================================================
// MARK: - ChanR_UTF_Native8

size_t ChanR_UTF_Native8::Read(UTF32* oDest, size_t iCountCU)
	{
	UTF8 utf8Buffer[kBufSize];
	size_t utf8Buffered = 0;

	UTF32* localDest = oDest;

	while (iCountCU)
		{
		const size_t utf8Read = this->Read(utf8Buffer, min(kBufSize - utf8Buffered, iCountCU));
		if (utf8Read == 0)
			break;

		utf8Buffered += utf8Read;

		size_t utf8Consumed;
		size_t utf32Generated;
		ZUnicode::sUTF8ToUTF32(
			utf8Buffer, utf8Read,
			&utf8Consumed, nullptr,
			localDest, iCountCU,
			&utf32Generated);

		iCountCU -= utf32Generated;
		localDest += utf32Generated;

		if (utf8Consumed && utf8Buffered > utf8Consumed)
			{
			sMemMove(utf8Buffer, utf8Buffer + utf8Consumed, utf8Buffered - utf8Consumed);
			utf8Buffered -= utf8Consumed;
			}
		}
	return localDest - oDest;
	}

void ChanR_UTF_Native8::ReadUTF16(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	UTF8 utf8Buffer[kBufSize];
	size_t utf8Buffered = 0;

	UTF16* localDest = oDest;
	size_t localCountCP = iCountCP;

	// If we've got space for two code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 2 && localCountCP)
		{
		// Top up the buffer as much as we can.
		const size_t utf8Read = this->Read(utf8Buffer + utf8Buffered,
			min(kBufSize - utf8Buffered, min(localCountCP, iCountCU)));

		if (utf8Read == 0)
			break;

		utf8Buffered += utf8Read;

		size_t utf8Consumed;
		size_t utf16Generated;
		size_t cpGenerated;
		ZUnicode::sUTF8ToUTF16(
			utf8Buffer, utf8Buffered,
			&utf8Consumed, nullptr,
			localDest, iCountCU,
			&utf16Generated,
			localCountCP, &cpGenerated);

		localCountCP -= cpGenerated;
		iCountCU -= utf16Generated;
		localDest += utf16Generated;

		if (utf8Consumed && utf8Buffered > utf8Consumed)
			{
			sMemMove(utf8Buffer, utf8Buffer + utf8Consumed, utf8Buffered - utf8Consumed);
			utf8Buffered -= utf8Consumed;
			}
		}

	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}

} // namespace ZooLib
