/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "ZStrimU_StreamUTF8Buffered.h"
#include "ZMemory.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_StreamUTF8Buffered

ZStrimU_StreamUTF8Buffered::ZStrimU_StreamUTF8Buffered(size_t iBufferSize, const ZStreamR& iStreamR)
:	fStreamR(iStreamR),
	fBuffer(iBufferSize, 0),
	fFeedIn(0),
	fFeedOut(0)
	{}

ZStrimU_StreamUTF8Buffered::~ZStrimU_StreamUTF8Buffered()
	{}


// Do a more optimal version of this?
static void sUTF8ToUTF32(const UTF8* iStart, size_t iCountCU,
	UTF32* iDest, size_t iDestCount, size_t& oDestCount)
	{
	ZUnicode::sUTF8ToUTF32(iStart, iCountCU,
		nil, nil,
		iDest, iDestCount,
		&oDestCount);
	}

void ZStrimU_StreamUTF8Buffered::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	// Optimize the common case -- single CU, with data in the buffer.
	if (iCount == 1 && fFeedOut < fFeedIn)
		{
		*iDest = fBuffer[fFeedOut++];
		if (oCount)
			*oCount = 1;
		return;
		}

	const size_t bufSize = fBuffer.size();
	vector<UTF8> utf8Buffer(bufSize, 0);

	UTF32* localDest = iDest;
	while (iCount)
		{
		if (fFeedOut < fFeedIn)
			{
			const size_t countWritten = min(iCount, fFeedIn - fFeedOut);
			ZBlockCopy(&fBuffer[fFeedOut], localDest, countWritten * sizeof(UTF32));
			fFeedOut += countWritten;
			localDest += countWritten;
			iCount -= countWritten;
			}
		else
			{
			size_t countToRead;
			UTF32* utf32Buffer;
			if (fFeedOut)
				{
				// Remember the last CP in fBuffer[0]
				fBuffer[0] = fBuffer[fFeedOut - 1];

				// Start writing to &fBuffer[1]
				utf32Buffer = &fBuffer[1];
				countToRead = bufSize - 1;
				fFeedIn = 1;
				fFeedOut = 1;
				}
			else
				{
				utf32Buffer = &fBuffer[0];
				countToRead = bufSize;
				fFeedIn = 0;
				fFeedOut = 0;
				}

			size_t utf8Read;
			fStreamR.Read(&utf8Buffer[0], countToRead, &utf8Read);
			if (!utf8Read)
				break;

			size_t destCU;
			sUTF8ToUTF32(&utf8Buffer[0], utf8Read, utf32Buffer, countToRead, destCU);
			fFeedIn += destCU;
			}
		}

	if (oCount)
		*oCount = localDest - iDest;
	}

void ZStrimU_StreamUTF8Buffered::Imp_Unread()
	{
	ZAssert(fFeedOut);
	--fFeedOut;
	}
