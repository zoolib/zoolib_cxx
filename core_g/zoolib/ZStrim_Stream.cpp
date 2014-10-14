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

#include "zoolib/Unicode.h"

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZStream.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZTextCoder.h"

using std::min;
using std::pair;

namespace ZooLib {

static const size_t kBufSize = sStackBufferSize;

#define kDebug_Strim 2

// =================================================================================================
// MARK: - Static helpers

static void spReadUTF32_UTF32(const ZStreamR& iStreamR, UTF32* oDest, size_t iCount, size_t* oCount)
	{ iStreamR.Read(oDest, iCount, oCount); }

static void spReadUTF32_UTF32Swap(const ZStreamR& iStreamR,
	UTF32* oDest, size_t iCount, size_t* oCount)
	{
	size_t countRead;
	iStreamR.Read(oDest, iCount, &countRead);
	if (oCount)
		*oCount = countRead;

	while (countRead--)
		{
		*oDest = ZByteSwap_Read32(oDest);
		++oDest;
		}
	}

static void spReadUTF32_UTF16(const ZStreamR& iStreamR,
	UTF32* oDest, const size_t iCount, size_t* oCount)
	{
	ZUnimplemented(); // Actually, just untested
	UTF16 utf16Buffer[kBufSize];
	size_t localCount = iCount;
	while (localCount)
		{
		size_t bytesRead;
		iStreamR.Read(utf16Buffer, sizeof(UTF16) * min(localCount , kBufSize), &bytesRead);
		const size_t utf16Read = bytesRead / 2;

		size_t utf16Consumed;
		size_t utf32Generated;
		Unicode::sUTF16ToUTF32(
			utf16Buffer, utf16Read,
			&utf16Consumed, nullptr,
			oDest, localCount,
			&utf32Generated);

		localCount -= utf32Generated;
		oDest += utf32Generated;
		}
	if (oCount)
		*oCount = iCount - localCount;
	}

static void spReadUTF32_UTF16Swap(const ZStreamR& iStreamR,
	UTF32* oDest, const size_t iCount, size_t* oCount)
	{
	ZUnimplemented(); // Actually, just untested
	UTF16 utf16Buffer[kBufSize];
	size_t localCount = iCount;
	while (localCount)
		{
		size_t bytesRead;
		iStreamR.Read(utf16Buffer, sizeof(UTF16) * min(localCount , kBufSize), &bytesRead);
		const size_t utf16Read = bytesRead / 2;

		UTF16* swapPtr = utf16Buffer;
		for (size_t count = utf16Read; count; --count)
			{
			*swapPtr = ZByteSwap_Read16(swapPtr);
			++swapPtr;
			}

		size_t utf16Consumed;
		size_t utf32Generated;
		Unicode::sUTF16ToUTF32(
			utf16Buffer, utf16Read,
			&utf16Consumed, nullptr,
			oDest, localCount,
			&utf32Generated);

		localCount -= utf32Generated;
		oDest += utf32Generated;
		}
	if (oCount)
		*oCount = iCount - localCount;
	}

static void spReadUTF32_UTF8(const ZStreamR& iStreamR,
	UTF32* oDest, const size_t iCount, size_t* oCount)
	{
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;

	uint8 curByte;
	bool gotByte = false;
	while (localDest < localDestEnd)
		{
		if (not gotByte)
			{
			if (not iStreamR.ReadByte(curByte))
				break;
			}

		gotByte = false;

		size_t sequenceLength = Unicode::sUTF8SequenceLength[curByte];
		if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else if (sequenceLength == 1)
			{
			*localDest++ = curByte;
			}
		else
			{
			UTF32 theCP = curByte & Unicode::sUTF8StartByteMask[sequenceLength];
			gotByte = false;
			while (--sequenceLength)
				{
				if (not iStreamR.ReadByte(curByte))
					break;
				if ((curByte & 0xC0) != 0x80)
					{
					// It's not a legal continuation byte.
					gotByte = true;
					break;
					}
				theCP <<= 6;
				theCP += curByte & 0x3F;
				}

			if (not gotByte)
				*localDest++ = theCP;
			}
		}
	if (oCount)
		*oCount = localDest - oDest;
	}

static void spReadUTF8_UTF8(const ZStreamR& iStreamR, UTF8* oDest,
	const size_t iCountCU, size_t* oCountCU,
	const size_t iCountCP, size_t* oCountCP)
	{
	size_t localCountCP = iCountCP;
	UTF8* localDest = oDest;
	UTF8* localDestEnd = oDest + iCountCU;

	uint8 curByte;
	bool gotByte = false;
	while (localDest < localDestEnd && localCountCP)
		{
		if (not gotByte)
			{
			if (not iStreamR.ReadByte(curByte))
				break;
			}

		gotByte = false;

		size_t sequenceLength = Unicode::sUTF8SequenceLength[curByte];
		if (sequenceLength == 0)
			{
			// It's a continuation or illegal, ignore it.
			}
		else if (sequenceLength == 1)
			{
			--localCountCP;
			*localDest++ = curByte;
			}
		else
			{
			// Remember where we started.
			UTF8* priorLocalDest = localDest;
			// Assume we'll succeed.
			--localCountCP;
			// Copy over the start byte, without interpretation.
			*localDest++ = curByte;
			gotByte = false;
			while (--sequenceLength)
				{
				if (not iStreamR.ReadByte(curByte))
					break;
				if ((curByte & 0xC0) != 0x80)
					{
					// It's not a legal continuation byte. Undo the changes we made.
					localDest = priorLocalDest;
					++localCountCP;
					gotByte = true;
					break;
					}
				else
					{
					// It's a valid continuation byte, copy it over.
					*localDest++ = curByte;
					}
				}
			}
		}
	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - oDest;
	}

// =================================================================================================
// MARK: - ZStrimR_StreamUTF32BE

ZStrimR_StreamUTF32BE::ZStrimR_StreamUTF32BE(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZStrimR_StreamUTF32BE::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (ZCONFIG(Endian, Big))
		spReadUTF32_UTF32(fStreamR, oDest, iCount, oCount);
	else
		spReadUTF32_UTF32Swap(fStreamR, oDest, iCount, oCount);
	}

// =================================================================================================
// MARK: - ZStrimR_StreamUTF32LE

ZStrimR_StreamUTF32LE::ZStrimR_StreamUTF32LE(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZStrimR_StreamUTF32LE::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (ZCONFIG(Endian, Big))
		spReadUTF32_UTF32Swap(fStreamR, oDest, iCount, oCount);
	else
		spReadUTF32_UTF32(fStreamR, oDest, iCount, oCount);
	}

// =================================================================================================
// MARK: - ZStrimR_StreamUTF16BE

ZStrimR_StreamUTF16BE::ZStrimR_StreamUTF16BE(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZStrimR_StreamUTF16BE::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (ZCONFIG(Endian, Big))
		spReadUTF32_UTF16(fStreamR, oDest, iCount, oCount);
	else
		spReadUTF32_UTF16Swap(fStreamR, oDest, iCount, oCount);
	}

// =================================================================================================
// MARK: - ZStrimR_StreamUTF16LE

ZStrimR_StreamUTF16LE::ZStrimR_StreamUTF16LE(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZStrimR_StreamUTF16LE::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{
	if (ZCONFIG(Endian, Big))
		spReadUTF32_UTF16Swap(fStreamR, oDest, iCount, oCount);
	else
		spReadUTF32_UTF16(fStreamR, oDest, iCount, oCount);
	}

// =================================================================================================
// MARK: - ZStrimR_StreamUTF8

ZStrimR_StreamUTF8::ZStrimR_StreamUTF8(const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{}

void ZStrimR_StreamUTF8::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{ spReadUTF32_UTF8(fStreamR, oDest, iCount, oCount); }

void ZStrimR_StreamUTF8::Imp_ReadUTF8(UTF8* oDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{ spReadUTF8_UTF8(fStreamR, oDest, iCountCU, oCountCU, iCountCP, oCountCP); }

// =================================================================================================
// MARK: - ZStrimW_StreamUTF32BE

ZStrimW_StreamUTF32BE::ZStrimW_StreamUTF32BE(const ZStreamW& iStreamW)
:	fStreamW(iStreamW)
	{}

void ZStrimW_StreamUTF32BE::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		size_t countWritten;
		fStreamW.Write(iSource, iCountCU * 4, &countWritten);
		if (oCountCU)
			*oCountCU = countWritten / 4;
		}
	else
		{
		const UTF32* localSource = iSource;
		UTF32 buffer[256];
		while (iCountCU)
			{
			const UTF32* src = localSource;
			UTF32* dest = buffer;
			size_t countToMove = min(iCountCU, size_t(256));
			size_t counter = countToMove;
			while (counter--)
				*dest++ = ZByteSwap_Read32(src++);

			size_t countWritten;
			fStreamW.Write(buffer, countToMove * 4, &countWritten);
			if (countWritten == 0)
				break;
			countWritten /= 4;
			iSource += countWritten;
			iCountCU -= countWritten;
			}
		if (oCountCU)
			*oCountCU = localSource - iSource;
		}
	}

void ZStrimW_StreamUTF32BE::Imp_Flush()
	{ fStreamW.Flush(); }

// =================================================================================================
// MARK: - ZStrimW_StreamUTF32LE

ZStrimW_StreamUTF32LE::ZStrimW_StreamUTF32LE(const ZStreamW& iStreamW)
:	fStreamW(iStreamW)
	{}

void ZStrimW_StreamUTF32LE::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (ZCONFIG(Endian, Little))
		{
		size_t countWritten;
		fStreamW.Write(iSource, iCountCU * 4, &countWritten);
		if (oCountCU)
			*oCountCU = countWritten / 4;
		}
	else
		{
		const UTF32* localSource = iSource;
		UTF32 buffer[256];
		while (iCountCU)
			{
			const UTF32* src = localSource;
			UTF32* dest = buffer;
			size_t countToMove = min(iCountCU, size_t(256));
			size_t counter = countToMove;
			while (counter--)
				*dest++ = ZByteSwap_Read32(src++);

			size_t countWritten;
			fStreamW.Write(buffer, countToMove * 4, &countWritten);
			if (countWritten == 0)
				break;
			countWritten /= 4;
			iSource += countWritten;
			iCountCU -= countWritten;
			}
		if (oCountCU)
			*oCountCU = localSource - iSource;
		}
	}

void ZStrimW_StreamUTF32LE::Imp_Flush()
	{ fStreamW.Flush(); }

// =================================================================================================
// MARK: - ZStrimW_StreamUTF16BE

ZStrimW_StreamUTF16BE::ZStrimW_StreamUTF16BE(const ZStreamW& iStreamW)
:	fStreamW(iStreamW)
	{}

void ZStrimW_StreamUTF16BE::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (ZCONFIG(Endian, Big))
		{
		size_t countWritten;
		fStreamW.Write(iSource, iCountCU * 2, &countWritten);
		if (oCountCU)
			*oCountCU = countWritten / 2;
		}
	else
		{
		const UTF16* localSource = iSource;
		UTF16 buffer[256];
		while (iCountCU)
			{
			const UTF16* src = localSource;
			UTF16* dest = buffer;
			size_t countToMove = min(iCountCU, size_t(256));
			size_t counter = countToMove;
			while (counter--)
				*dest++ = ZByteSwap_Read16(src++);

			size_t countWritten;
			fStreamW.Write(buffer, countToMove * 2, &countWritten);
			if (countWritten == 0)
				break;
			countWritten /= 2;
			iSource += countWritten;
			iCountCU -= countWritten;
			}
		if (oCountCU)
			*oCountCU = localSource - iSource;
		}
	}

void ZStrimW_StreamUTF16BE::Imp_Flush()
	{ fStreamW.Flush(); }

// =================================================================================================
// MARK: - ZStrimW_StreamUTF16LE

ZStrimW_StreamUTF16LE::ZStrimW_StreamUTF16LE(const ZStreamW& iStreamW)
:	fStreamW(iStreamW)
	{}

void ZStrimW_StreamUTF16LE::Imp_WriteUTF16(const UTF16* iSource, size_t iCountCU, size_t* oCountCU)
	{
	if (ZCONFIG(Endian, Little))
		{
		size_t countWritten;
		fStreamW.Write(iSource, iCountCU * 2, &countWritten);
		if (oCountCU)
			*oCountCU = countWritten / 2;
		}
	else
		{
		const UTF16* localSource = iSource;
		UTF16 buffer[256];
		while (iCountCU)
			{
			const UTF16* src = localSource;
			UTF16* dest = buffer;
			size_t countToMove = min(iCountCU, size_t(256));
			size_t counter = countToMove;
			while (counter--)
				*dest++ = ZByteSwap_Read16(src++);

			size_t countWritten;
			fStreamW.Write(buffer, countToMove * 2, &countWritten);
			if (countWritten == 0)
				break;
			countWritten /= 2;
			iSource += countWritten;
			iCountCU -= countWritten;
			}
		if (oCountCU)
			*oCountCU = localSource - iSource;
		}
	}

void ZStrimW_StreamUTF16LE::Imp_Flush()
	{ fStreamW.Flush(); }

// =================================================================================================
// MARK: - ZStrimW_StreamUTF8

ZStrimW_StreamUTF8::ZStrimW_StreamUTF8(const ChanW_Bin& iStreamW)
:	fStreamW(iStreamW)
	{}

void ZStrimW_StreamUTF8::Imp_WriteUTF8(const UTF8* iSource, size_t iCountCU, size_t* oCountCU)
	{
	size_t countCU = sQWrite(iSource, iCountCU, fStreamW);
	if (oCountCU)
		*oCountCU = countCU;
	}

void ZStrimW_StreamUTF8::Imp_Flush()
	{ sFlush(fStreamW); }

// =================================================================================================
// MARK: - ZStrimR_StreamDecoder

ZStrimR_StreamDecoder::ZStrimR_StreamDecoder(ZTextDecoder* iDecoder, const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fDecoder(iDecoder)
	{
	ZAssertStop(2, fDecoder);
	}

ZStrimR_StreamDecoder::ZStrimR_StreamDecoder(const pair<ZTextDecoder*, const ZStreamR*>& iParam)
:	fStreamR(*iParam.second),
	fDecoder(iParam.first)
	{
	ZAssertStop(2, iParam.second);
	ZAssertStop(2, fDecoder);
	}

ZStrimR_StreamDecoder::~ZStrimR_StreamDecoder()
	{ delete fDecoder; }

void ZStrimR_StreamDecoder::Imp_ReadUTF32(UTF32* oDest, size_t iCount, size_t* oCount)
	{ fDecoder->Decode(fStreamR, oDest, iCount, oCount); }

void ZStrimR_StreamDecoder::SetDecoder(ZTextDecoder* iDecoder)
	{
	ZAssertStop(2, iDecoder && iDecoder != fDecoder);
	delete sGetSet(fDecoder, iDecoder);
	}

ZTextDecoder* ZStrimR_StreamDecoder::GetSetDecoder(ZTextDecoder* iDecoder)
	{
	ZAssertStop(2, iDecoder);
	return sGetSet(fDecoder, iDecoder);
	}

// =================================================================================================
// MARK: - ZStrimW_StreamEncoder

ZStrimW_StreamEncoder::ZStrimW_StreamEncoder(ZTextEncoder* iEncoder, const ZStreamW& iStreamW)
:	fStreamW(iStreamW),
	fEncoder(iEncoder)
	{
	ZAssertStop(2, fEncoder);
	}

ZStrimW_StreamEncoder::ZStrimW_StreamEncoder(const pair<ZTextEncoder*, const ZStreamW*>& iParam)
:	fStreamW(*iParam.second),
	fEncoder(iParam.first)
	{
	ZAssertStop(2, iParam.second);
	ZAssertStop(2, fEncoder);
	}

ZStrimW_StreamEncoder::~ZStrimW_StreamEncoder()
	{ delete fEncoder; }

void ZStrimW_StreamEncoder::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{ fEncoder->Encode(fStreamW, iSource, iCountCU, oCountCU); }

void ZStrimW_StreamEncoder::Imp_Flush()
	{ fStreamW.Flush(); }

void ZStrimW_StreamEncoder::SetEncoder(ZTextEncoder* iEncoder)
	{
	ZAssertStop(2, iEncoder && iEncoder != fEncoder);
	delete sGetSet(fEncoder, iEncoder);
	}

ZTextEncoder* ZStrimW_StreamEncoder::GetSetEncoder(ZTextEncoder* iEncoder)
	{
	ZAssertStop(2, iEncoder);
	return sGetSet(fEncoder, iEncoder);
	}

} // namespace ZooLib
