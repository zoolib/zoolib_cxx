/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZStream_CRLF.h"

#include "zoolib/ZCompat_algorithm.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy

using std::min;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

static char sCR[] = { '\r' };
static char sLF[] = { '\n' };

static const char CR = '\r';
static const char LF = '\n';

#define kBufSize sStackBufferSize

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CRLFRemove

ZStreamR_CRLFRemove::ZStreamR_CRLFRemove(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fReplaceChar('\n'),
	fLastWasCR(false)
	{}

ZStreamR_CRLFRemove::ZStreamR_CRLFRemove(char iReplaceChar, const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fReplaceChar(iReplaceChar),
	fLastWasCR(false)
	{}

ZStreamR_CRLFRemove::~ZStreamR_CRLFRemove()
	{}

void ZStreamR_CRLFRemove::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char buffer[kBufSize];

	if (oCountRead)
		*oCountRead = 0;
	char* localDest = reinterpret_cast<char*>(iDest);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		size_t countRead;
		fStreamSource.Read(buffer, min(countRemaining, kBufSize), &countRead);
		if (countRead == 0)
			break;
		char* readFrom = buffer;
		char* writeTo = localDest;
		while (countRead--)
			{
			char currChar = *readFrom++;
			switch (currChar)
				{
				case CR:
					*writeTo++ = fReplaceChar;
					fLastWasCR = true;
					break;
				case LF:
					if (!fLastWasCR)
						*writeTo++ = fReplaceChar;
					fLastWasCR = false;
					break;
				default:
					*writeTo++ = currChar;
					fLastWasCR = false;
					break;
				}
			}
		if (oCountRead)
			*oCountRead += writeTo - localDest;
		countRemaining -= writeTo - localDest;
		localDest = writeTo;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_CRLFInsert

ZStreamR_CRLFInsert::ZStreamR_CRLFInsert(const ZStreamR& iStreamSource)
:	fStreamSource(iStreamSource),
	fLastWasCR(false),
	fHasBuffChar(false)
	{}

ZStreamR_CRLFInsert::~ZStreamR_CRLFInsert()
	{}

void ZStreamR_CRLFInsert::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	if (oCountRead)
		*oCountRead = 0;

	if (iCount == 0)
		return;

	char* localDest = reinterpret_cast<char*>(iDest);
	size_t countRemaining = iCount;

	while (countRemaining)
		{
		char* priorDest = localDest;
		if (!fHasBuffChar)
			{
			if (!fStreamSource.ReadChar(fBuffChar))
				{
				if (fLastWasCR)
					{
					*localDest++ = LF;
					fLastWasCR = false;
					if (oCountRead)
						++*oCountRead;
					--countRemaining;
					}
				break;
				}
			fHasBuffChar = true;
			}
		else
			{
			if (fLastWasCR)
				{
				fLastWasCR = false;
				*localDest++ = LF;
				if (fBuffChar == LF)
					fHasBuffChar = false;
				}
			else
				{
				if (fBuffChar == CR)
					{
					*localDest++ = CR;
					fLastWasCR = true;
					fHasBuffChar = false;
					}
				else if (fBuffChar == LF)
					{
					*localDest++ = CR;
					fLastWasCR = true;
					}
				else
					{
					*localDest++ = fBuffChar;
					fLastWasCR = false;
					fHasBuffChar = false;
					}
				}
			}
		if (oCountRead)
			*oCountRead += localDest - priorDest;
		countRemaining -= localDest - priorDest;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_CRLFRemove

ZStreamW_CRLFRemove::ZStreamW_CRLFRemove(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fReplaceChar('n'),
	fLastWasCR(false)
	{}

ZStreamW_CRLFRemove::ZStreamW_CRLFRemove(char iReplaceChar, const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fReplaceChar(iReplaceChar),
	fLastWasCR(false)
	{}

ZStreamW_CRLFRemove::~ZStreamW_CRLFRemove()
	{
	}

void ZStreamW_CRLFRemove::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const char* localSource = reinterpret_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		if (*localSource == CR)
			{
			size_t countWritten;
			fStreamSink.Write(&fReplaceChar, 1, &countWritten);
			if (countWritten == 0)
				break;
			fLastWasCR = true;
			if (oCountWritten)
				++*oCountWritten;
			--countRemaining;
			++localSource;
			}
		else if (*localSource == LF)
			{
			if (!fLastWasCR)
				{
				size_t countWritten;
				fStreamSink.Write(&fReplaceChar, 1, &countWritten);
				if (countWritten == 0)
					break;
				}
			if (oCountWritten)
				++*oCountWritten;
			--countRemaining;
			++localSource;
			fLastWasCR = false;
			}
		else
			{
			// Find the next CR or LF
			const char* innerSource = localSource;
			const char* innerEnd = localSource + countRemaining;
			char currChar;
			while (innerSource < innerEnd)
				{
				currChar = *innerSource;
				if (currChar == CR || currChar == LF)
					break;
				++innerSource;
				}

			if (innerSource > localSource)
				{
				size_t countWritten;
				fStreamSink.Write(localSource, innerSource - localSource, &countWritten);
				if (countWritten == 0)
					{
					countRemaining = 0;
					break;
					}
				if (oCountWritten)
					*oCountWritten += countWritten;
				countRemaining -= countWritten;
				localSource += countWritten;
				fLastWasCR = false;
				}
			}
		}
	}

void ZStreamW_CRLFRemove::Imp_Flush()
	{
	fStreamSink.Flush();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamW_CRLFInsert

ZStreamW_CRLFInsert::ZStreamW_CRLFInsert(const ZStreamW& iStreamSink)
:	fStreamSink(iStreamSink),
	fLastWasCR(false)
	{}

ZStreamW_CRLFInsert::~ZStreamW_CRLFInsert()
	{
	if (fLastWasCR)
		{
		fStreamSink.Write(sLF, 1, nil);
		fLastWasCR = false;
		}
	}

void ZStreamW_CRLFInsert::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	if (oCountWritten)
		*oCountWritten = 0;

	const char* localSource = reinterpret_cast<const char*>(iSource);
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		if (fLastWasCR)
			{
			fLastWasCR = false;
			size_t countWritten;
			fStreamSink.Write(sLF, 1, &countWritten);
			if (countWritten == 0)
				break;
			if (*localSource == LF)
				{
				if (oCountWritten)
					++*oCountWritten;
				--countRemaining;
				++localSource;
				}
			}
		else if (*localSource == CR)
			{
			size_t countWritten;
			fStreamSink.Write(sCR, 1, &countWritten);
			if (countWritten == 0)
				break;
			fLastWasCR = true;
			if (oCountWritten)
				++*oCountWritten;
			--countRemaining;
			++localSource;
			}
		else if (*localSource == LF)
			{
			size_t countWritten;
			fStreamSink.Write(sCR, 1, &countWritten);
			if (countWritten == 0)
				break;
			// Pretend that the last was a CR, we'll write the LF and consume
			// this LF on the next go round
			fLastWasCR = true;
			}
		else
			{
			// Find the next CR or LF
			const char* innerSource = localSource;
			const char* innerEnd = localSource + countRemaining;
			char currChar;
			while (innerSource < innerEnd)
				{
				currChar = *innerSource;
				if (currChar == CR || currChar == LF)
					break;
				++innerSource;
				}

			if (innerSource > localSource)
				{
				size_t countWritten;
				fStreamSink.Write(localSource, innerSource - localSource, &countWritten);
				if (countWritten == 0)
					break;
				if (oCountWritten)
					*oCountWritten += countWritten;
				countRemaining -= countWritten;
				localSource += countWritten;
				}
			}
		}
	}

void ZStreamW_CRLFInsert::Imp_Flush()
	{
	fStreamSink.Flush();
	}

NAMESPACE_ZOOLIB_END
