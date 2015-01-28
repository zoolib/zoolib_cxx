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

#include "zoolib/ChanR_UTF.h"
#include "zoolib/Memory.h"
#include "zoolib/Unicode.h"

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

using std::min;

// =================================================================================================
#pragma mark -

void sRead(UTF32* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	 const ChanR_UTF& iChanR)
	 {
	 const size_t countRead = sQRead(oDest, std::min(iCountCU, iCountCP), iChanR);

	 if (oCountCU)
	 	*oCountCU = countRead;
	 if (oCountCP)
	 	*oCountCP = countRead;
	 }

void sRead(UTF16* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	 const ChanR_UTF& iChanR)
	{
	UTF32 utf32Buffer[kBufSize];
	UTF16* localDest = oDest;
	size_t localCountCP = iCountCP;
	// If we've got space for two code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 2 && localCountCP)
		{
		const size_t utf32Read =
			sQRead(utf32Buffer, min(kBufSize, min(localCountCP, iCountCU)), iChanR);

		if (utf32Read == 0)
			break;

		size_t utf32Consumed;
		size_t utf16Generated;
		size_t cpGenerated;
		Unicode::sUTF32ToUTF16(
			utf32Buffer, utf32Read,
			&utf32Consumed, nullptr,
			localDest, iCountCU,
			&utf16Generated,
			localCountCP, &cpGenerated);

		ZAssert(utf32Consumed == utf32Read);

		localCountCP -= cpGenerated;
		iCountCU -= utf16Generated;
		localDest += utf16Generated;
		}
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}

void sRead(UTF8* oDest,
	 size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP,
	 const ChanR_UTF& iChanR)
	{
	UTF32 utf32Buffer[kBufSize];
	UTF8* localDest = oDest;
	size_t localCountCP = iCountCP;
	// If we've got space for six code units we are assured of
	// being able to read at least one code point.
	while (iCountCU >= 6 && localCountCP)
		{
		const size_t utf32Read =
			sQRead(utf32Buffer, min(kBufSize, min(localCountCP, iCountCU)), iChanR);

		if (utf32Read == 0)
			break;

		size_t utf32Consumed;
		size_t utf8Generated;
		size_t cpGenerated;
		Unicode::sUTF32ToUTF8(
			utf32Buffer, utf32Read,
			&utf32Consumed, nullptr,
			localDest, iCountCU,
			&utf8Generated,
			localCountCP, &cpGenerated);

		ZAssert(utf32Consumed == utf32Read);

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
#pragma mark -

ZQ<string32> sQReadUTF32(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string32 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP);
		UTF32* dest = sNonConst(result.data()) + destGenerated;
		const size_t cuRead = sQRead(dest, iCountCP, iChanR);
		const size_t cpRead = Unicode::sCUToCP(dest, cuRead);

		if (cuRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

ZQ<string16> sQReadUTF16(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string16 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP + 1);
		size_t cuRead, cpRead;
		sRead(sNonConst(result.data()) + destGenerated,
			iCountCP + 1, &cuRead, iCountCP, &cpRead, iChanR);

		if (cuRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

ZQ<string8> sQReadUTF8(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	string8 result;
	size_t destGenerated = 0;
	while (iCountCP)
		{
		result.resize(destGenerated + iCountCP + 5);
		size_t cuRead, cpRead;
		sRead(sNonConst(result.data()) + destGenerated,
			iCountCP + 5, &cuRead, iCountCP, &cpRead, iChanR);

		if (cuRead == 0)
			return null;
		iCountCP -= cpRead;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

// -----

string32 sReadMustUTF32(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string32> theQ = sQReadUTF32(iCountCP, iChanR);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

string16 sReadMustUTF16(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string16> theQ = sQReadUTF16(iCountCP, iChanR);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

string8 sReadMustUTF8(size_t iCountCP, const ChanR_UTF& iChanR)
	{
	const ZQ<string8> theQ = sQReadUTF8(iCountCP, iChanR);
	if (not theQ)
		sThrow_ExhaustedR();
	return *theQ;
	}

// -----

string8 sReadAllUTF8(const ChanR_UTF& iChanR)
	{
	string8 result;
	size_t destGenerated = 0;
	for (;;)
		{
		result.resize(destGenerated + 1024);
		size_t cuRead;
		sRead(sNonConst(result.data()) + destGenerated,
			1024, &cuRead, 1024, nullptr, iChanR);

		if (cuRead == 0)
			break;
		destGenerated += cuRead;
		}
	result.resize(destGenerated);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Native16

size_t ChanR_UTF_Native16::QRead(UTF32* oDest, size_t iCountCU)
	{
	UTF16 utf16Buffer[kBufSize];
	size_t utf16Buffered = 0;

	UTF32* localDest = oDest;

	while (iCountCU)
		{
		size_t countCURead;
		this->ReadUTF16(utf16Buffer,
			kBufSize - utf16Buffered, &countCURead,
			iCountCU, nullptr);

		if (countCURead == 0)
			break;

		utf16Buffered += countCURead;

		size_t utf16Consumed;
		size_t utf32Generated;
		Unicode::sUTF16ToUTF32(
			utf16Buffer, countCURead,
			&utf16Consumed, nullptr,
			localDest, iCountCU,
			&utf32Generated);

		iCountCU -= utf32Generated;
		localDest += utf32Generated;

		if (utf16Consumed && utf16Buffered > utf16Consumed)
			{
			sMemMove(utf16Buffer, utf16Buffer + utf16Consumed, utf16Buffered - utf16Consumed);
			utf16Buffered -= utf16Consumed;
			}
		}
	return localDest - oDest;
	}

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_Native8

size_t ChanR_UTF_Native8::QRead(UTF32* oDest, size_t iCountCU)
	{
	UTF8 utf8Buffer[kBufSize];
	size_t utf8Buffered = 0;

	UTF32* localDest = oDest;

	while (iCountCU)
		{
		size_t countCURead;
		this->ReadUTF8(utf8Buffer,
			kBufSize - utf8Buffered, &countCURead,
			iCountCU, nullptr);

		if (countCURead == 0)
			break;

		utf8Buffered += countCURead;

		size_t utf8Consumed;
		size_t utf32Generated;
		Unicode::sUTF8ToUTF32(
			utf8Buffer, countCURead,
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

} // namespace ZooLib
