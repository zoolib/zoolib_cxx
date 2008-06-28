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

#include "zoolib/ZStreamRPos_StreamR.h"

#include "zoolib/ZDebug.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_StreamR

/**
ZStreamRPos_StreamR takes a source ZStreamR, and a ZStreamRWPos to use as a buffer. Its
size and position are taken from the ZStreamRWPos. A read or call to SetPosition that would
move beyond the end of the ZStreamRWPos is handled by copying more data from the ZStreamR
to the end of the ZStreamRWPos.

The ZStreamRPos_StreamR::Committed() can be called at any time and as often as you like, but
once it has been called it is then an error to use any of the position/size methods. Committed
returns a ZStreamR reference, which will refer to the ZStreamRPos_StreamR if the buffer stream
is not yet exhausted and thus if reads should be satisfied by pulling at least some data from
it. If the ZStreamRWPos is known to be exhausted when Committed is called (i.e. if
GetPosition() == GetSize()) then Committed is returns a reference to the source ZStreamR, so
reads can bypass the indirection that imposed by using the ZStreamRPos_StreamR directly.

This class is useful when you have a ZStreamR to which you need random access for only some
amount of data at the head. That might be the case, for example, when handling a graphic
format that has fiddly meta-data at the beginning, followed by arbitrary data that need not
be accessed randomly.

\sa ZStreamR_DynamicBuffered
*/

ZStreamRPos_StreamR::ZStreamRPos_StreamR(const ZStreamR& iSource, const ZStreamRWPos& iBuffer)
:	fSource(iSource),
	fBuffer(iBuffer),
	fCommitted(false)
	{}

ZStreamRPos_StreamR::~ZStreamRPos_StreamR()
	{}

void ZStreamRPos_StreamR::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(iDest);
	while (iCount)
		{
		size_t countRead;
		fBuffer.Read(localDest, iCount, &countRead);
		if (countRead)
			{
			localDest += countRead;
			iCount -= countRead;
			}
		else
			{
			// Our buffer is empty.
			if (fCommitted)
				{
				// We're committed, so read direct from fSource.
				fSource.Read(localDest, iCount, &countRead);
				if (countRead == 0)
					break;
				localDest += countRead;
				iCount -= countRead;
				}
			else
				{
				// Try topping up our buffer.
				uint64 countCopied;
				fBuffer.CopyFrom(fSource, iCount, nil, &countCopied);
				if (countCopied == 0)
					break;
				fBuffer.SetPosition(fBuffer.GetPosition() - countCopied);
				}
			}
		}	
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(iDest);
	}

size_t ZStreamRPos_StreamR::Imp_CountReadable()
	{
	if (size_t countReadable = fBuffer.CountReadable())
		return countReadable;
	return fSource.CountReadable();
	}

uint64 ZStreamRPos_StreamR::Imp_GetPosition()
	{
	ZAssertStop(2, !fCommitted);

	return fBuffer.GetPosition();
	}

void ZStreamRPos_StreamR::Imp_SetPosition(uint64 iPosition)
	{
	ZAssertStop(2, !fCommitted);

	uint64 curSize = fBuffer.GetSize();
	if (iPosition > curSize)
		{
		// Position the buffer at its end
		fBuffer.SetPosition(curSize);

		// And suck in enough data from fSource so its size (and position) is iPosition bytes
		fBuffer.CopyFrom(fSource, iPosition - curSize, nil, nil);
		}
	fBuffer.SetPosition(iPosition);
	}

uint64 ZStreamRPos_StreamR::Imp_GetSize()
	{
	// In order to know how much data is available we have
	// to suck it all in from fSource. Another reason to avoid
	// using GetSize if at all possible.
	ZAssertStop(2, !fCommitted);

	uint64 curPosition = fBuffer.GetPosition();
	fBuffer.SetPosition(fBuffer.GetSize());
	fBuffer.CopyAllFrom(fSource, nil, nil);
	fBuffer.SetPosition(curPosition);
	return fBuffer.GetSize();
	}

const ZStreamR& ZStreamRPos_StreamR::Committed()
	{
	fCommitted = true;
	if (fBuffer.GetPosition() >= fBuffer.GetSize())
		return fSource;
	else
		return *this;
	}
