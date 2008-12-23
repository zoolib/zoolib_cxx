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

#include "zoolib/ZMemoryBlock.h"

#include "zoolib/ZCompat_algorithm.h" // For min()
#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_USING

using std::bad_alloc;
using std::max;
using std::min;

// =================================================================================================
#pragma mark -
#pragma mark * ZMemoryBlock

ZMemoryBlock::ZMemoryBlock(const void* iSource, size_t iSize)
:	fRep(new (iSize) ZMemoryBlock::Rep(iSize))
	{
	this->CopyFrom(iSource, iSize);
	}

void ZMemoryBlock::Touch()
	{
	if (fRep->GetRefCount() == 1)
		return;
	Rep* newRep = new (fRep->fSize) Rep(fRep->fSize);
	ZBlockCopy(fRep->fData, newRep->fData, fRep->fSize);
	fRep = newRep;
	}

void ZMemoryBlock::SetSize(size_t iSize)
	{
	if (iSize != fRep->fSize)
		{
		Rep* newRep = new (iSize) Rep(iSize);
		ZBlockCopy(fRep->fData, newRep->fData, min(fRep->fSize, newRep->fSize));
		fRep = newRep;
		}
	}

void ZMemoryBlock::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->Touch();
	ZBlockCopy(iSource, fRep->fData + iOffset, iCount);
	}

void ZMemoryBlock::CopyTo(size_t iOffset, void* iDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	ZBlockCopy(fRep->fData + iOffset, iDest, iCount);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_MemoryBlock

ZStreamRPos_MemoryBlock::ZStreamRPos_MemoryBlock(const ZMemoryBlock& iMemoryBlock)
:	fMemoryBlock(iMemoryBlock),
	fPosition(0)
	{}

ZStreamRPos_MemoryBlock::~ZStreamRPos_MemoryBlock()
	{}

void ZStreamRPos_MemoryBlock::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToCopy = ZStream::sClampedSize(iCount, fMemoryBlock.GetSize(), fPosition);
//	size_t countToCopy = min(uint64(iCount), sDiffPosR(fMemoryBlock.GetSize(), fPosition));
	fMemoryBlock.CopyTo(fPosition, iDest, countToCopy);
	fPosition += countToCopy;

	if (oCountRead)
		*oCountRead = countToCopy;
	}

void ZStreamRPos_MemoryBlock::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	size_t realSkip = ZStream::sClampedSize(iCount, fMemoryBlock.GetSize(), fPosition);
//	uint64 realSkip = min(iCount, sDiffPosR(fMemoryBlock.GetSize(), fPosition));
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_MemoryBlock::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_MemoryBlock::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_MemoryBlock::Imp_GetSize()
	{ return fMemoryBlock.GetSize(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_MemoryBlock

ZStreamRWPos_MemoryBlock::ZStreamRWPos_MemoryBlock(
	ZMemoryBlock& iMemoryBlock, size_t iGrowIncrement)
:	fMemoryBlock(iMemoryBlock)
	{
	fGrowIncrement = iGrowIncrement;
	fPosition = 0;
	fSizeLogical = fMemoryBlock.GetSize();
	}

ZStreamRWPos_MemoryBlock::ZStreamRWPos_MemoryBlock(ZMemoryBlock& iMemoryBlock)
:	fMemoryBlock(iMemoryBlock)
	{
	fGrowIncrement = 64;
	fPosition = 0;
	fSizeLogical = fMemoryBlock.GetSize();
	}

ZStreamRWPos_MemoryBlock::~ZStreamRWPos_MemoryBlock()
	{
	// Finally, make sure the MemoryBlock is the real size, not the potentially
	// overallocated size we've been using
	fMemoryBlock.SetSize(fSizeLogical);
	}

void ZStreamRWPos_MemoryBlock::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToCopy = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
	fMemoryBlock.CopyTo(fPosition, iDest, countToCopy);
	fPosition += countToCopy;

	if (oCountRead)
		*oCountRead = countToCopy;
	}

void ZStreamRWPos_MemoryBlock::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	size_t realSkip = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

void ZStreamRWPos_MemoryBlock::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	uint64 neededSpace = fPosition + iCount;
	if (fMemoryBlock.GetSize() < neededSpace)
		{
		uint64 realSize = max(neededSpace, uint64(fMemoryBlock.GetSize()) + fGrowIncrement);
		if (realSize == size_t(realSize))
			fMemoryBlock.SetSize(realSize);
		}

	size_t countToCopy = ZStream::sClampedSize(iCount, fMemoryBlock.GetSize(), fPosition);

	fMemoryBlock.CopyFrom(fPosition, iSource, iCount);

	fPosition += countToCopy;

	if (fSizeLogical < fPosition)
		fSizeLogical = fPosition;

	if (oCountWritten)
		*oCountWritten = countToCopy;
	}

void ZStreamRWPos_MemoryBlock::Imp_Flush()
	{ fMemoryBlock.SetSize(fSizeLogical); }

uint64 ZStreamRWPos_MemoryBlock::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_MemoryBlock::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_MemoryBlock::Imp_GetSize()
	{ return fSizeLogical; }

void ZStreamRWPos_MemoryBlock::Imp_SetSize(uint64 iSize)
	{
	size_t realSize = iSize;
	if (realSize != iSize)
		sThrowBadSize();

	fMemoryBlock.SetSize(realSize);
	fSizeLogical = realSize;
	}
