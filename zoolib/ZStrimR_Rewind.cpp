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

#include "ZStrimR_Rewind.h"

#include "ZDebug.h"
#include "ZStream.h"
#include "ZStrim_Stream.h"

#define kDebug_StrimR_Rewind 1

// =================================================================================================
#pragma mark -
#pragma mark * ZStrimR_Rewind

/**
\class ZStrimR_Rewind
\ingroup strim
*/

ZStrimR_Rewind::ZStrimR_Rewind(const ZStrimR& iStrimSource, const ZStreamRWPos& iBuffer)
:	fStrimSource(iStrimSource),
	fBuffer(iBuffer)
	{}

ZStrimR_Rewind::~ZStrimR_Rewind()
	{}

void ZStrimR_Rewind::Imp_ReadUTF32(UTF32* iDest, size_t iCount, size_t* oCount)
	{
	UTF32* localDest = iDest;
	size_t countRemaining = iCount;
	while (countRemaining)
		{
		size_t countRead;
		ZStrimR_StreamUTF8(fBuffer).Read(localDest, countRemaining, &countRead);
		if (countRead == 0)
			{
			size_t priorPosition = fBuffer.GetPosition();
			// Our buffer's reached the end. Try topping it up.
			size_t countCopied;
			ZStrimW_StreamUTF8(fBuffer).CopyFrom(fStrimSource, countRemaining, &countCopied, nil);
			if (countCopied == 0)
				break;
			fBuffer.SetPosition(priorPosition);
			}
		else
			{
			localDest += countRead;
			countRemaining -= countRead;
			}
		}
	if (oCount)
		*oCount = localDest - iDest;
	}

void ZStrimR_Rewind::Rewind()
	{ fBuffer.SetPosition(0); }

uint64 ZStrimR_Rewind::GetMark()
	{ return fBuffer.GetPosition(); }

void ZStrimR_Rewind::SetMark(uint64 iMark)
	{
	ZAssertStop(kDebug_StrimR_Rewind, iMark <= fBuffer.GetSize());
	fBuffer.SetPosition(iMark);
	}
