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

#include "zoolib/ZStreamRWPos_RAM.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_RAM

/**
\class ZStreamRWPos_RAM
\ingroup stream
*/

ZStreamRWPos_RAM::ZStreamRWPos_RAM()
:	fPosition(0)
	{}

ZStreamRWPos_RAM::~ZStreamRWPos_RAM()
	{}

void ZStreamRWPos_RAM::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	const size_t count = ZStream::sClampedSize(iCount, fDeque.size(), fPosition);
	if (count)
		{
		const std::deque<char>::const_iterator i = fDeque.begin() + fPosition;
		copy(i, i + count, static_cast<char*>(oDest));
		fPosition += count;
		}

	if (oCountRead)
		*oCountRead = count;
	}

size_t ZStreamRWPos_RAM::Imp_CountReadable()
	{ return ZStream::sClampedSize(fDeque.size(), fPosition); }

void ZStreamRWPos_RAM::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	const size_t countToSkip = ZStream::sClampedSize(iCount, fDeque.size(), fPosition);
	fPosition += countToSkip;
	if (oCountSkipped)
		*oCountSkipped = countToSkip;
	}

void ZStreamRWPos_RAM::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	const char* localSource = static_cast<const char*>(iSource);
	const size_t countToCopy = ZStream::sClampedSize(iCount, fDeque.size(), fPosition);
	std::copy(localSource, localSource + countToCopy, fDeque.begin() + fPosition);
	fDeque.insert(fDeque.end(), localSource + countToCopy, localSource + iCount);
	fPosition += iCount;
	if (oCountWritten)
		*oCountWritten = iCount;
	}

uint64 ZStreamRWPos_RAM::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_RAM::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_RAM::Imp_GetSize()
	{ return fDeque.size(); }

void ZStreamRWPos_RAM::Imp_SetSize(uint64 iSize)
	{ fDeque.resize(iSize); }

} // namespace ZooLib
