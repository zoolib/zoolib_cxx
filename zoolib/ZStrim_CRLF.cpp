/* ------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZStrim_CRLF.h"
#include "ZCompat_algorithm.h" // for min
#include "ZDebug.h"

using std::min;

static const UTF32 sLF[] = { '\n' };
static const UTF32 sCR[] = { '\r' };
static const UTF32 LF = '\n';
static const UTF32 CR = '\r';

static const size_t kBufSize = ZooLib::sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_CRLFRemove

ZStrimR_CRLFRemove::ZStrimR_CRLFRemove(const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fReplaceCP('\n')
	{}

ZStrimR_CRLFRemove::ZStrimR_CRLFRemove(UTF32 iReplaceCP, const ZStrimR& iStrimSource)
:	fStrimSource(iStrimSource),
	fReplaceCP(iReplaceCP)
	{
	ZAssertStop(2, ZUnicode::sIsValid(fReplaceCP));
	}

void ZStrimR_CRLFRemove::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32 buffer[kBufSize];

	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iCount;
	while (localDestEnd > localDest)
		{
		size_t countRead;
		fStrimSource.Read(buffer, min(size_t(localDestEnd - localDest), kBufSize), &countRead);
		if (countRead == 0)
			break;

		const UTF32* readFrom = buffer;
		const UTF32* readFromEnd = buffer + countRead;
		for (;;)
			{
			UTF32 currCP;
			if (!ZUnicode::sReadInc(readFrom, readFromEnd, currCP))
				break;

			switch (currCP)
				{
				case CR:
					{
					*localDest++ = fReplaceCP;
					fLastWasCR = true;
					break;
					}
				case LF:
					{
					if (!fLastWasCR)
						*localDest++ = fReplaceCP;
					fLastWasCR = false;
					break;
					}
				default:
					{
					*localDest++ = currCP;
					fLastWasCR = false;
					break;
					}
				}
			}
		}

	if (oCount)
		*oCount = localDest - iDest;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_CRLFRemove

ZStrimW_CRLFRemove::ZStrimW_CRLFRemove(const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fReplaceCP('\n')
	{}

ZStrimW_CRLFRemove::ZStrimW_CRLFRemove(UTF32 iReplaceCP, const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink),
	fReplaceCP(iReplaceCP)
	{
	ZAssertStop(2, ZUnicode::sIsValid(fReplaceCP));
	}

void ZStrimW_CRLFRemove::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	const UTF32* localSource = iSource;
	while (iCountCU)
		{
		if (*localSource == CR)
			{
			size_t countWritten;
			fStrimSink.Write(&fReplaceCP, 1, &countWritten);
			if (countWritten == 0)
				break;
			--iCountCU;
			++localSource;
			fLastWasCR = true;
			}
		else if (*localSource == LF)
			{
			if (!fLastWasCR)
				{
				size_t countWritten;
				fStrimSink.Write(&fReplaceCP, 1, &countWritten);
				if (countWritten == 0)
					break;
				}
			--iCountCU;
			++localSource;
			fLastWasCR = false;
			}
		else
			{
			// Find the next CR or LF
			const UTF32* innerSource = localSource;
			const UTF32* innerEnd = localSource + iCountCU;
			UTF32 currCP;
			while (innerSource < innerEnd)
				{
				currCP = *innerSource;
				if (currCP == CR || currCP == LF)
					break;
				++innerSource;
				}

			if (innerSource > localSource)
				{
				size_t countWritten;
				fStrimSink.Write(localSource, innerSource - localSource, &countWritten);
				if (countWritten == 0)
					{
					iCountCU = 0;
					break;
					}
				iCountCU -= countWritten;
				localSource += countWritten;
				fLastWasCR = false;
				}
			}
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimW_CRLFInsert

ZStrimW_CRLFInsert::ZStrimW_CRLFInsert(const ZStrimW& iStrimSink)
:	fStrimSink(iStrimSink)
	{}

void ZStrimW_CRLFInsert::Imp_WriteUTF32(const UTF32* iSource, size_t iCountCU, size_t* oCountCU)
	{
	const UTF32* localSource = iSource;
	while (iCountCU)
		{
		if (fLastWasCR)
			{
			fLastWasCR = false;
			size_t countWritten;
			fStrimSink.Write(sLF, 1, &countWritten);
			if (countWritten == 0)
				break;
			if (*localSource == LF)
				{
				--iCountCU;
				++localSource;
				}
			}
		else if (*localSource == CR)
			{
			size_t countWritten;
			fStrimSink.Write(sCR, 1, &countWritten);
			if (countWritten == 0)
				break;
			--iCountCU;
			++localSource;
			fLastWasCR = true;
			}
		else if (*localSource == LF)
			{
			size_t countWritten;
			fStrimSink.Write(sCR, 1, &countWritten);
			if (countWritten == 0)
				break;
			// Pretend that the last was a CR, we'll write the LF and consume
			// this LF on the next go round.
			fLastWasCR = true;
			}
		else
			{
			// Find the next CR or LF
			const UTF32* innerSource = localSource;
			const UTF32* innerEnd = localSource + iCountCU;
			UTF32 currCU;
			while (innerSource < innerEnd)
				{
				currCU = *innerSource;
				if (currCU == CR || currCU == LF)
					break;
				++innerSource;
				}

			if (innerSource > localSource)
				{
				size_t countWritten;
				fStrimSink.Write(localSource, innerSource - localSource, &countWritten);
				if (countWritten == 0)
					break;
				iCountCU -= countWritten;
				localSource += countWritten;
				}
			}
		}

	if (oCountCU)
		*oCountCU = localSource - iSource;
	}
