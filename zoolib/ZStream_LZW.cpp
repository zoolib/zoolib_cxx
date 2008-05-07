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

#include "ZStream_LZW.h"
#include "ZDebug.h"
#include "ZMemory.h"

using std::min;

#define kDebug_LZW 2

static const int kTableSize = 5003;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWEncode

// ZStreamR_LZWEncode is not done yet.

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWEncodeNoPatent

/**
\class ZStreamR_LZWEncodeNoPatent
\ingroup stream
A read filter stream that returns a LZW-compatible codestream but without actually
compressing, thus avoiding infringement of the Unisys patent.
*/

ZStreamR_LZWEncodeNoPatent::ZStreamR_LZWEncodeNoPatent(
	int iCodeSize_Alphabet, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	ZAssertStop(kDebug_LZW, iCodeSize_Alphabet > 0 && iCodeSize_Alphabet <= 8);

	if (iCodeSize_Alphabet == 1)
		iCodeSize_Alphabet = 2;

	fCodeSize_Alphabet = iCodeSize_Alphabet;
	fCodeSize_Emitted = iCodeSize_Alphabet + 1;

	fCode_Clear = (1 << fCodeSize_Alphabet);
	fCode_FIN = (1 << fCodeSize_Alphabet) + 1;

	fCountSinceClear = (1 << fCodeSize_Alphabet) - 2;

	fSentFIN = false;
	fFinished = false;
	}

ZStreamR_LZWEncodeNoPatent::~ZStreamR_LZWEncodeNoPatent()
	{}

void ZStreamR_LZWEncodeNoPatent::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (!fFinished && iCount)
		{
		size_t countRead;
		if (fCountSinceClear == (1 << fCodeSize_Alphabet) - 2)
			{
			fBitWriter.WriteBits(localDest, fCodeSize_Emitted, fCode_Clear, &countRead);
			fCountSinceClear = 0;
			}
		else
			{
			if (fSentFIN)
				{
				fBitWriter.Finish(localDest, &countRead);
				fFinished = true;
				}
			else
				{
				uint8 theDatum;
				if (fStreamSource.ReadByte(theDatum))
					{
					fBitWriter.WriteBits(localDest, fCodeSize_Emitted, theDatum, &countRead);
					}
				else
					{
					fBitWriter.WriteBits(localDest, fCodeSize_Emitted, fCode_FIN, &countRead);
					fSentFIN = true;
					}
				}
			}
		localDest += countRead;
		iCount -= countRead;
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

size_t ZStreamR_LZWEncodeNoPatent::Imp_CountReadable()
	{
	// As we're actually expanding fCodeSize_Alphabet bits, each cluster in a single
	// source byte to (fCodeSize_Alphabet + 1) bits returned contiguously, we can approximate
	// the count readable thus:
	return (fStreamSource.CountReadable() * (fCodeSize_Alphabet + 1)) / 8;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_LZWDecode

/**
\class ZStreamR_LZWDecode
\ingroup stream
*/

ZStreamR_LZWDecode::ZStreamR_LZWDecode(int iCodeSize_Alphabet, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource)
	{
	ZAssertStop(kDebug_LZW, iCodeSize_Alphabet > 0 && iCodeSize_Alphabet <= 8);

	if (iCodeSize_Alphabet == 1)
		iCodeSize_Alphabet = 2;

	fSeenFIN = false;

	fCodeSize_Alphabet = iCodeSize_Alphabet;
	fCodeSize_Read = iCodeSize_Alphabet + 1;

	fCode_Clear = (1 << fCodeSize_Alphabet);
	fCode_FIN = (1 << fCodeSize_Alphabet) + 1;
	fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;
	fCode_Last = -1;
	fCode_ABCABCA = -1;

	fStack = nil;
	fPrefix = nil;
	fSuffix = nil;
	try
		{
		fStack = new uint8[4096];
		fPrefix = new uint16[4096];
		fSuffix = new uint8[4096];
		}
	catch (...)
		{
		delete[] fStack;
		delete[] fPrefix;
		delete[] fSuffix;
		throw;
		}

	fStackEnd = fStack + 4096;
	fStackTop = fStackEnd;
	}

ZStreamR_LZWDecode::~ZStreamR_LZWDecode()
	{
	delete[] fStack;
	delete[] fPrefix;
	delete[] fSuffix;
	}

void ZStreamR_LZWDecode::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (iCount)
		{
		if (fStackTop == fStackEnd)
			{
			if (fSeenFIN)
				break;

			for (;;)
				{
				uint32 readCode;
				if (!fBitReader.ReadBits(fStreamSource, fCodeSize_Read, readCode))
					return;

				uint32 theCode = readCode;

				if (theCode == fCode_FIN)
					{
					fSeenFIN = true;
					break;
					}

				if (theCode == fCode_Clear)
					{
					fCodeSize_Read = fCodeSize_Alphabet + 1;
					fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;
					fCode_Last = -1;
					fCode_ABCABCA = -1;
					continue;
					}

				if (theCode >= fCode_FirstAvailable)
					{
					theCode = fCode_Last;
					ZAssertStop(kDebug_LZW, fCode_ABCABCA != -1);
					*--fStackTop = fCode_ABCABCA;
					}

				while (theCode > fCode_Clear)
					{
					*--fStackTop = fSuffix[theCode];
					theCode = fPrefix[theCode];
					}

				*--fStackTop = theCode;
				fCode_ABCABCA = theCode;

				// Make a new entry in the alphabet, if not just cleared.
				if (fCode_Last != -1)
					{
					fPrefix[fCode_FirstAvailable] = fCode_Last;
					fSuffix[fCode_FirstAvailable] = theCode;
					++fCode_FirstAvailable;
					if (fCode_FirstAvailable >= (1 << fCodeSize_Read) && fCodeSize_Read < 12)
						++fCodeSize_Read;
					}

				fCode_Last = readCode;
				break;
				}
			}

		size_t countToCopy = min(iCount, size_t(fStackEnd - fStackTop));
		ZBlockCopy(fStackTop, localDest, countToCopy);
		fStackTop += countToCopy;
		localDest += countToCopy;
		iCount -= countToCopy;
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

size_t ZStreamR_LZWDecode::Imp_CountReadable()
	{ return fStackEnd - fStackTop; }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWEncode

/**
\class ZStreamW_LZWEncode
\ingroup stream
*/

ZStreamW_LZWEncode::ZStreamW_LZWEncode(int iCodeSize_Alphabet, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	ZAssertStop(kDebug_LZW, iCodeSize_Alphabet > 0 && iCodeSize_Alphabet <= 8);

	if (iCodeSize_Alphabet == 1)
		iCodeSize_Alphabet = 2;

	fCodeSize_Alphabet = iCodeSize_Alphabet;
	fCodeSize_Emitted = iCodeSize_Alphabet + 1;

	fCode_Clear = (1 << fCodeSize_Alphabet);
	fCode_FIN = (1 << fCodeSize_Alphabet) + 1;
	fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;

	fCodes_Suffix = nil;
	fCodes_Prefix = nil;
	fChild = nil;

	try
		{
		fCodes_Suffix = new uint8[kTableSize];
		fCodes_Prefix = new uint16[kTableSize];
		fChild = new uint16[kTableSize];
		}
	catch (...)
		{
		delete[] fCodes_Suffix;
		delete[] fCodes_Prefix;
		delete[] fChild;
		throw;
		}

	ZBlockZero(fChild, sizeof(uint16) * kTableSize);

	fFresh = true;
	}

ZStreamW_LZWEncode::~ZStreamW_LZWEncode()
	{
	try
		{
		if (fFresh)
			fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_Clear);
		else
			fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fPendingPrefix);

		fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_FIN);
		fBitWriter.Finish(fStreamSink);
		}
	catch (...)
		{}

	delete[] fCodes_Suffix;
	delete[] fCodes_Prefix;
	delete[] fChild;
	}

void ZStreamW_LZWEncode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		uint8 currentSuffix = *localSource++;
		--iCount;

		if (fFresh)
			{
			fFresh = false;
			fPendingPrefix = currentSuffix;
			fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_Clear);
			}
		else
			{
			size_t hash = (uint32(fPendingPrefix ^ currentSuffix) << 5) % kTableSize;
			size_t hashDelta = 1;

			for (;;)
				{
				if (fChild[hash] == 0)
					{
					fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fPendingPrefix);

					size_t temp = fCode_FirstAvailable;

					if (fCode_FirstAvailable < 4096)
						{
						fCodes_Prefix[hash] = fPendingPrefix;
						fCodes_Suffix[hash] = currentSuffix;
						fChild[hash] = fCode_FirstAvailable;
						++fCode_FirstAvailable;
						}

					if (temp == (1 << fCodeSize_Emitted))
						{
						if (fCodeSize_Emitted < 12)
							{
							++fCodeSize_Emitted;
							}
						else
							{
							fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_Clear);
							fCodeSize_Emitted = fCodeSize_Alphabet + 1;
							fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;
							ZBlockZero(fChild, sizeof(uint16) * kTableSize);
							}
						}

					fPendingPrefix = currentSuffix;
					break;
					}

				if (fCodes_Prefix[hash] == fPendingPrefix && fCodes_Suffix[hash] == currentSuffix)
					{
					fPendingPrefix = fChild[hash];
					break;
					}

				hash += hashDelta;
				hashDelta += 2;
				if (hash >= kTableSize)
					hash -= kTableSize;
				}
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_LZWEncode::Imp_Flush()
	{ fStreamSink.Flush(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWEncodeNoPatent

/**
\class ZStreamW_LZWEncodeNoPatent
\ingroup stream
A write filter stream that writes an LZW-compatible codestream but without actually
compressing, thus avoiding infringement of the Unisys patent.
*/

ZStreamW_LZWEncodeNoPatent::ZStreamW_LZWEncodeNoPatent(
	int iCodeSize_Alphabet, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	ZAssertStop(kDebug_LZW, iCodeSize_Alphabet > 0 && iCodeSize_Alphabet <= 8);

	if (iCodeSize_Alphabet == 1)
		iCodeSize_Alphabet = 2;

	fCodeSize_Alphabet = iCodeSize_Alphabet;
	fCodeSize_Emitted = iCodeSize_Alphabet + 1;

	fCode_Clear = (1 << fCodeSize_Alphabet);
	fCode_FIN = (1 << fCodeSize_Alphabet) + 1;

	fCountSinceClear = (1 << fCodeSize_Alphabet) - 2;
	}

ZStreamW_LZWEncodeNoPatent::~ZStreamW_LZWEncodeNoPatent()
	{
	try
		{
		if (fCountSinceClear == (1 << fCodeSize_Alphabet) - 2)
			fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_Clear);
		fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_FIN);
		fBitWriter.Finish(fStreamSink);
		}
	catch (...)
		{}
	}

void ZStreamW_LZWEncodeNoPatent::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fCountSinceClear == (1 << fCodeSize_Alphabet) - 2)
			{
			fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, fCode_Clear);
			fCountSinceClear = 0;
			}
		fBitWriter.WriteBits(fStreamSink, fCodeSize_Emitted, *localSource);
		++localSource;
		--iCount;
		++fCountSinceClear;
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_LZWEncodeNoPatent::Imp_Flush()
	{ fStreamSink.Flush(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_LZWDecode

/**
\class ZStreamW_LZWDecode
\ingroup stream
*/

ZStreamW_LZWDecode::ZStreamW_LZWDecode(int iCodeSize_Alphabet, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink)
	{
	ZAssertStop(kDebug_LZW, iCodeSize_Alphabet > 0 && iCodeSize_Alphabet <= 8);

	if (iCodeSize_Alphabet == 1)
		iCodeSize_Alphabet = 2;

	fSeenFIN = false;

	fCodeSize_Alphabet = iCodeSize_Alphabet;
	fCodeSize_Read = iCodeSize_Alphabet + 1;

	fCode_Clear = (1 << fCodeSize_Alphabet);
	fCode_FIN = (1 << fCodeSize_Alphabet) + 1;
	fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;
	fCode_Last = -1;
	fCode_ABCABCA = -1;

	fStack = nil;
	fPrefix = nil;
	fSuffix = nil;
	try
		{
		fStack = new uint8[4096];
		fPrefix = new uint16[4096];
		fSuffix = new uint8[4096];
		}
	catch (...)
		{
		delete[] fStack;
		delete[] fPrefix;
		delete[] fSuffix;
		throw;
		}

	fStackEnd = fStack + 4096;
	fStackTop = fStackEnd;
	}

ZStreamW_LZWDecode::~ZStreamW_LZWDecode()
	{
	delete[] fStack;
	delete[] fPrefix;
	delete[] fSuffix;
	}

void ZStreamW_LZWDecode::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount && !fSeenFIN)
		{
		while (iCount)
			{
			uint32 readCode;
			size_t countConsumed;
			fBitReader.ReadBits(localSource, fCodeSize_Read, readCode, &countConsumed);
			iCount -= countConsumed;
			localSource += countConsumed;

			uint32 theCode = readCode;

			if (theCode == fCode_FIN)
				{
				fSeenFIN = true;
				break;
				}

			if (theCode == fCode_Clear)
				{
				fCodeSize_Read = fCodeSize_Alphabet + 1;
				fCode_FirstAvailable = (1 << fCodeSize_Alphabet) + 2;
				fCode_Last = -1;
				fCode_ABCABCA = -1;
				continue;
				}

			if (theCode >= fCode_FirstAvailable)
				{
				theCode = fCode_Last;
				ZAssertStop(kDebug_LZW, fCode_ABCABCA != -1);
				*--fStackTop = fCode_ABCABCA;
				}

			while (theCode > fCode_Clear)
				{
				*--fStackTop = fSuffix[theCode];
				theCode = fPrefix[theCode];
				}

			*--fStackTop = theCode;
			fCode_ABCABCA = theCode;

			// Make a new entry in the alphabet, if not just cleared.
			if (fCode_Last != -1)
				{
				fPrefix[fCode_FirstAvailable] = fCode_Last;
				fSuffix[fCode_FirstAvailable] = theCode;
				++fCode_FirstAvailable;
				if (fCode_FirstAvailable >= (1 << fCodeSize_Read) && fCodeSize_Read < 12)
					++fCodeSize_Read;
				}

			fCode_Last = readCode;
			break;
			}

		size_t countWritten;
		fStreamSink.Write(fStackTop, fStackEnd - fStackTop, &countWritten);
		fStackTop += countWritten;

		if (fStackTop != fStackEnd)
			{
			// We couldn't purge the stack. So pretend that we've seen a FIN
			// to prevent any future calls doing anything, and further pretend
			// that we couldn't suck up all the source data, so the caller knows
			// something went wrong.
			fSeenFIN = true;
			--localSource;
			}
		}
	if (oCountWritten)
		*oCountWritten = localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ZStreamW_LZWDecode::Imp_Flush()
	{
	fStreamSink.Flush();
	}
