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

#include "zoolib/Chan_UTF_CRLF.h"
#include "zoolib/Compat_algorithm.h" // For min
#include "zoolib/ZDebug.h"

using std::min;

namespace ZooLib {

//##static const UTF32 spLF[] = { '\n' };
//##static const UTF32 spCR[] = { '\r' };

static const UTF32 LF = 0x0A; // Probably the same as '\n'
static const UTF32 CR = 0x0D; // Probably the same as '\r'

static const size_t kBufSize = sStackBufferSize;

// =================================================================================================
#pragma mark -
#pragma mark ChanR_UTF_CRLFRemove

ChanR_UTF_CRLFRemove::ChanR_UTF_CRLFRemove(const ChanR_UTF& iChanR)
:	fChanR(iChanR),
	fReplacementCP('\n')
	{}

ChanR_UTF_CRLFRemove::ChanR_UTF_CRLFRemove(UTF32 iReplacementCP, const ChanR_UTF& iChanR)
:	fChanR(iChanR),
	fReplacementCP(iReplacementCP)
	{
	ZAssertStop(2, Unicode::sIsValid(fReplacementCP));
	}

size_t ChanR_UTF_CRLFRemove::QRead(UTF32* oDest, size_t iCount)
	{
	UTF32 buffer[kBufSize];

	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iCount;
	while (localDestEnd > localDest)
		{
		const size_t countRead = sQRead(fChanR, buffer, min(size_t(localDestEnd - localDest), kBufSize));
		if (countRead == 0)
			break;

		const UTF32* readFrom = buffer;
		const UTF32* readFromEnd = buffer + countRead;
		for (;;)
			{
			UTF32 currCP;
			if (not Unicode::sReadInc(readFrom, readFromEnd, currCP))
				break;

			switch (currCP)
				{
				case CR:
					{
					*localDest++ = fReplacementCP;
					fLastWasCR = true;
					break;
					}
				case LF:
					{
					if (!fLastWasCR)
						*localDest++ = fReplacementCP;
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

	return localDest - oDest;
	}

// // =================================================================================================
// #pragma mark -
// #pragma mark ZStreamR_CRLFInsert
// 
// ZStreamR_CRLFInsert::ZStreamR_CRLFInsert(const ZStreamR& iStreamSource)
// :	fStreamSource(iStreamSource),
// 	fLastWasCR(false),
// 	fHasBuffChar(false)
// 	{}
// 
// ZStreamR_CRLFInsert::~ZStreamR_CRLFInsert()
// 	{}
// 
// void ZStreamR_CRLFInsert::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
// 	{
// 	if (oCountRead)
// 		*oCountRead = 0;
// 
// 	if (iCount == 0)
// 		return;
// 
// 	char* localDest = reinterpret_cast<char*>(oDest);
// 	size_t countRemaining = iCount;
// 
// 	while (countRemaining)
// 		{
// 		char* priorDest = localDest;
// 		if (!fHasBuffChar)
// 			{
// 			if (!fStreamSource.ReadChar(fBuffChar))
// 				{
// 				if (fLastWasCR)
// 					{
// 					*localDest++ = LF;
// 					fLastWasCR = false;
// 					if (oCountRead)
// 						++*oCountRead;
// 					--countRemaining;
// 					}
// 				break;
// 				}
// 			fHasBuffChar = true;
// 			}
// 		else
// 			{
// 			if (fLastWasCR)
// 				{
// 				fLastWasCR = false;
// 				*localDest++ = LF;
// 				if (fBuffChar == LF)
// 					fHasBuffChar = false;
// 				}
// 			else
// 				{
// 				if (fBuffChar == CR)
// 					{
// 					*localDest++ = CR;
// 					fLastWasCR = true;
// 					fHasBuffChar = false;
// 					}
// 				else if (fBuffChar == LF)
// 					{
// 					*localDest++ = CR;
// 					fLastWasCR = true;
// 					}
// 				else
// 					{
// 					*localDest++ = fBuffChar;
// 					fLastWasCR = false;
// 					fHasBuffChar = false;
// 					}
// 				}
// 			}
// 		if (oCountRead)
// 			*oCountRead += localDest - priorDest;
// 		countRemaining -= localDest - priorDest;
// 		}
// 	}
// 
// // =================================================================================================
// #pragma mark -
// #pragma mark ZStreamW_CRLFRemove
// 
// ZStreamW_CRLFRemove::ZStreamW_CRLFRemove(const ZStreamW& iStreamSink)
// :	fStreamSink(iStreamSink),
// 	fReplaceChar('n'),
// 	fLastWasCR(false)
// 	{}
// 
// ZStreamW_CRLFRemove::ZStreamW_CRLFRemove(char iReplaceChar, const ZStreamW& iStreamSink)
// :	fStreamSink(iStreamSink),
// 	fReplaceChar(iReplaceChar),
// 	fLastWasCR(false)
// 	{}
// 
// ZStreamW_CRLFRemove::~ZStreamW_CRLFRemove()
// 	{
// 	}
// 
// void ZStreamW_CRLFRemove::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
// 	{
// 	if (oCountWritten)
// 		*oCountWritten = 0;
// 
// 	const char* localSource = reinterpret_cast<const char*>(iSource);
// 	size_t countRemaining = iCount;
// 	while (countRemaining)
// 		{
// 		if (*localSource == CR)
// 			{
// 			size_t countWritten;
// 			fStreamSink.Write(&fReplaceChar, 1, &countWritten);
// 			if (countWritten == 0)
// 				break;
// 			fLastWasCR = true;
// 			if (oCountWritten)
// 				++*oCountWritten;
// 			--countRemaining;
// 			++localSource;
// 			}
// 		else if (*localSource == LF)
// 			{
// 			if (!fLastWasCR)
// 				{
// 				size_t countWritten;
// 				fStreamSink.Write(&fReplaceChar, 1, &countWritten);
// 				if (countWritten == 0)
// 					break;
// 				}
// 			if (oCountWritten)
// 				++*oCountWritten;
// 			--countRemaining;
// 			++localSource;
// 			fLastWasCR = false;
// 			}
// 		else
// 			{
// 			// Find the next CR or LF
// 			const char* innerSource = localSource;
// 			const char* innerEnd = localSource + countRemaining;
// 			char currChar;
// 			while (innerSource < innerEnd)
// 				{
// 				currChar = *innerSource;
// 				if (currChar == CR || currChar == LF)
// 					break;
// 				++innerSource;
// 				}
// 
// 			if (innerSource > localSource)
// 				{
// 				size_t countWritten;
// 				fStreamSink.Write(localSource, innerSource - localSource, &countWritten);
// 				if (countWritten == 0)
// 					break;
// 				if (oCountWritten)
// 					*oCountWritten += countWritten;
// 				countRemaining -= countWritten;
// 				localSource += countWritten;
// 				fLastWasCR = false;
// 				}
// 			}
// 		}
// 	}
// 
// void ZStreamW_CRLFRemove::Imp_Flush()
// 	{
// 	fStreamSink.Flush();
// 	}
// 
// // =================================================================================================
// #pragma mark -
// #pragma mark ZStreamW_CRLFInsert
// 
// ZStreamW_CRLFInsert::ZStreamW_CRLFInsert(const ZStreamW& iStreamSink)
// :	fStreamSink(iStreamSink),
// 	fLastWasCR(false)
// 	{}
// 
// ZStreamW_CRLFInsert::~ZStreamW_CRLFInsert()
// 	{
// 	if (fLastWasCR)
// 		{
// 		fStreamSink.Write(spLF, 1, nullptr);
// 		fLastWasCR = false;
// 		}
// 	}
// 
// void ZStreamW_CRLFInsert::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
// 	{
// 	if (oCountWritten)
// 		*oCountWritten = 0;
// 
// 	const char* localSource = reinterpret_cast<const char*>(iSource);
// 	size_t countRemaining = iCount;
// 	while (countRemaining)
// 		{
// 		if (fLastWasCR)
// 			{
// 			fLastWasCR = false;
// 			size_t countWritten;
// 			fStreamSink.Write(spLF, 1, &countWritten);
// 			if (countWritten == 0)
// 				break;
// 			if (*localSource == LF)
// 				{
// 				if (oCountWritten)
// 					++*oCountWritten;
// 				--countRemaining;
// 				++localSource;
// 				}
// 			}
// 		else if (*localSource == CR)
// 			{
// 			size_t countWritten;
// 			fStreamSink.Write(spCR, 1, &countWritten);
// 			if (countWritten == 0)
// 				break;
// 			fLastWasCR = true;
// 			if (oCountWritten)
// 				++*oCountWritten;
// 			--countRemaining;
// 			++localSource;
// 			}
// 		else if (*localSource == LF)
// 			{
// 			size_t countWritten;
// 			fStreamSink.Write(spCR, 1, &countWritten);
// 			if (countWritten == 0)
// 				break;
// 			// Pretend that the last was a CR, we'll write the LF and consume
// 			// this LF on the next go round
// 			fLastWasCR = true;
// 			}
// 		else
// 			{
// 			// Find the next CR or LF
// 			const char* innerSource = localSource;
// 			const char* innerEnd = localSource + countRemaining;
// 			char currChar;
// 			while (innerSource < innerEnd)
// 				{
// 				currChar = *innerSource;
// 				if (currChar == CR || currChar == LF)
// 					break;
// 				++innerSource;
// 				}
// 
// 			if (innerSource > localSource)
// 				{
// 				size_t countWritten;
// 				fStreamSink.Write(localSource, innerSource - localSource, &countWritten);
// 				if (countWritten == 0)
// 					break;
// 				if (oCountWritten)
// 					*oCountWritten += countWritten;
// 				countRemaining -= countWritten;
// 				localSource += countWritten;
// 				}
// 			}
// 		}
// 	}
// 
// void ZStreamW_CRLFInsert::Imp_Flush()
// 	{
// 	fStreamSink.Flush();
// 	}

} // namespace ZooLib
