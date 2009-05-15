/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZDebug.h"
#include "zoolib/ZStrimU_Std.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimU_Std

ZStrimU_Std::ZStrimU_Std(ZTextDecoder* iDecoder, const ZStreamR& iStreamR)
:	fStrimR_StreamDecoder(iDecoder, iStreamR),
	fStrimR_CRLFRemove(fStrimR_StreamDecoder),
	fState(eStateFresh),
	fLineCount(0)
	{}

// The implementations of Imp_ReadUTF32, Imp_ReadUTF16 and Imp_ReadUTF8 are derived from those in
// ZStrimU_Unreader, with modifications to manage the line count.
void ZStrimU_Std::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = iDest;
	UTF32* localDestEnd = iDest + iCount;
	if  (fState == eStateUnread && localDest < localDestEnd)
		{
		if (ZUnicode::sWriteInc(localDest, localDestEnd, fCP))
			fState = eStateNormal;
		}

	while (localDest < localDestEnd)
		{
		size_t countRead;
		fStrimR_CRLFRemove.Read(localDest, localDestEnd - localDest, &countRead);
		if (countRead == 0)
			break;
		localDest += countRead;
		}

	if (oCount)
		*oCount = localDest - iDest;

	if (iCount)
		{
		if (localDest == iDest)
			{
			fState = eStateHitEnd;
			}
		else
			{
			fState = eStateNormal;
			const UTF32* temp = localDest;
			fCP = ZUnicode::sDecRead(temp);

			// Count the number of carriage returns.
			temp = iDest;
			while (temp < localDest)
				{
				if (*temp++ == '\n')
					++fLineCount;
				}			
			}
		}
	}

void ZStrimU_Std::Imp_ReadUTF16(UTF16* iDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	UTF16* localDest = iDest;
	UTF16* localDestEnd = iDest + iCountCU;
	size_t localCountCP = iCountCP;

	if (fState == eStateUnread && localDest + 1 < localDestEnd && localCountCP)
		{
		if (ZUnicode::sWriteInc(localDest, localDestEnd, fCP))
			{
			--localCountCP;
			fState = eStateNormal;
			}
		}

	while (localDest + 1 < localDestEnd && localCountCP)
		{
		size_t countCURead;
		size_t countCPRead;
		fStrimR_CRLFRemove.Read(localDest,
			localDestEnd - localDest, &countCURead, localCountCP, &countCPRead);

		if (countCURead == 0)
			break;
		localDest += countCURead;
		localCountCP -= countCPRead;
		}

	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - iDest;

	if (iCountCP)
		{
		if (localDest == iDest)
			{
			fState = eStateHitEnd;
			}
		else
			{
			fState = eStateNormal;
			const UTF16* temp = localDest;
			fCP = ZUnicode::sDecRead(temp);

			// Count the number of carriage returns.
			temp = iDest;
			while (temp < localDest)
				{
				if (*temp++ == '\n')
					++fLineCount;
				}
			}
		}
	}

void ZStrimU_Std::Imp_ReadUTF8(UTF8* iDest,
	size_t iCountCU, size_t* oCountCU, size_t iCountCP, size_t* oCountCP)
	{
	UTF8* localDest = iDest;
	UTF8* localDestEnd = iDest + iCountCU;
	size_t localCountCP = iCountCP;

	while (fState == eStateUnread && localDest + 5 < localDestEnd && localCountCP)
		{
		if (ZUnicode::sWriteInc(localDest, localDestEnd, fCP))
			{
			--localCountCP;
			fState = eStateNormal;
			}
		}

	while (localDest + 5 < localDestEnd && localCountCP)
		{
		size_t countCURead;
		size_t countCPRead;
		fStrimR_CRLFRemove.Read(localDest,
			localDestEnd - localDest, &countCURead, localCountCP, &countCPRead);

		if (countCURead == 0)
			break;
		fCP = localDest[countCURead - 1];
		localDest += countCURead;
		localCountCP -= countCPRead;
		}

	if (oCountCP)
		*oCountCP = iCountCP - localCountCP;
	if (oCountCU)
		*oCountCU = localDest - iDest;

	if (iCountCP)
		{
		if (localDest == iDest)
			{
			fState = eStateHitEnd;
			}
		else
			{
			fState = eStateNormal;
			// FIXME. This might be a problem, in that we're reading back from the buffer we're
			// writing to, and if that's a shared scratch buffer then its contents might get
			// changed between our write into it and this call to get the last code point.
			const UTF8* temp = localDest;
			fCP = ZUnicode::sDecRead(temp);

			// Count the number of carriage returns.
			temp = iDest;
			while (temp < localDest)
				{
				if (*temp++ == '\n')
					++fLineCount;
				}
			}
		}
	}

void ZStrimU_Std::Imp_Unread()
	{
	switch (fState)
		{
		case eStateFresh:
			{
			ZDebugStopf(2, ("StrimU_Std::Imp_Unread called without having read anything."));
			break;
			}
		case eStateNormal:
			{
			fState = eStateUnread;
			if (fCP == '\n')
				{
				if (fLineCount)
					--fLineCount;
				}
			break;
			}
		case eStateUnread:
			{
			ZDebugStopf(2, ("StrimU_Std::Imp_Unread called twice consecutively."));
			break;
			}
		case eStateHitEnd:
			{
			ZDebugStopf(2, ("StrimU_Std::Imp_Unread called when end of stream has been seen."));
			break;
			}
		}	
	}

void ZStrimU_Std::SetDecoder(ZTextDecoder* iDecoder)
	{ fStrimR_StreamDecoder.SetDecoder(iDecoder); }

ZTextDecoder* ZStrimU_Std::SetDecoderReturnOld(ZTextDecoder* iDecoder)
	{ return fStrimR_StreamDecoder.SetDecoderReturnOld(iDecoder); }

size_t ZStrimU_Std::GetLineCount()
	{ return fLineCount; }

void ZStrimU_Std::ResetLineCount()
	{ fLineCount = 0; }

NAMESPACE_ZOOLIB_END
