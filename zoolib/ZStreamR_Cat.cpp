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

#include "zoolib/ZStreamR_Cat.h"

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamR_Cat

/**
\class ZStreamR_Cat
\ingroup stream
*/

ZStreamR_Cat::ZStreamR_Cat(const ZStreamR& iStreamR1, const ZStreamR& iStreamR2)
:	fStreamR1(iStreamR1),
	fStreamR2(iStreamR2),
	fFirstIsLive(true)
	{}

ZStreamR_Cat::~ZStreamR_Cat()
	{}

void ZStreamR_Cat::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	char* localDest = static_cast<char*>(iDest);
	while (iCount)
		{
		size_t countRead;
		if (fFirstIsLive)
			{
			fStreamR1.Read(localDest, iCount, &countRead);
			if (countRead == 0)
				fFirstIsLive = false;
			}
		else
			{
			fStreamR2.Read(localDest, iCount, &countRead);
			if (countRead == 0)
				break;
			}
		localDest += countRead;
		iCount -= countRead;
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<char*>(iDest);
	}

size_t ZStreamR_Cat::Imp_CountReadable()
	{
	if (fFirstIsLive)
		return fStreamR1.CountReadable();
	else
		return fStreamR2.CountReadable();
	}

bool ZStreamR_Cat::Imp_WaitReadable(int iMilliseconds)
	{
	if (fFirstIsLive)
		return fStreamR1.WaitReadable(iMilliseconds);
	else
		return fStreamR2.WaitReadable(iMilliseconds);
	}

void ZStreamR_Cat::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countRemaining = iCount;
	while (countRemaining)
		{
		uint64 countSkipped;
		if (fFirstIsLive)
			{
			fStreamR1.Skip(countRemaining, &countSkipped);
			if (countSkipped == 0)
				fFirstIsLive = false;
			}
		else
			{
			fStreamR2.Skip(countRemaining, &countSkipped);
			if (countSkipped == 0)
				break;
			}
		countRemaining -= countSkipped;
		}
	if (oCountSkipped)
		*oCountSkipped = iCount - countRemaining;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerR_Cat

ZStreamerR_Cat::ZStreamerR_Cat(ZRef<ZStreamerR> iStreamerR1, ZRef<ZStreamerR> iStreamerR2)
:	fStreamerR1(iStreamerR1),
	fStreamerR2(iStreamerR2),
	fStream(iStreamerR1->GetStreamR(), iStreamerR2->GetStreamR())
	{}

ZStreamerR_Cat::~ZStreamerR_Cat()
	{}

const ZStreamR& ZStreamerR_Cat::GetStreamR()
	{ return fStream; }
