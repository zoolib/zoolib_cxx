/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZStream_CFData.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZMemory.h" // For ZBlockMove & ZBlockCopy

NAMESPACE_ZOOLIB_BEGIN

using std::min;
using std::max;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_CFData

ZStreamRPos_CFData::ZStreamRPos_CFData(CFDataRef iDataRef)
:	fDataRef(iDataRef),
	fPosition(0)
	{}

ZStreamRPos_CFData::~ZStreamRPos_CFData()
	{}

void ZStreamRPos_CFData::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToCopy = ZStream::sClampedSize(iCount, ::CFDataGetLength(fDataRef), fPosition);
	ZBlockCopy(::CFDataGetBytePtr(fDataRef) + fPosition, iDest, countToCopy);
	fPosition += countToCopy;

	if (oCountRead)
		*oCountRead = countToCopy;
	}

void ZStreamRPos_CFData::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	size_t realSkip = ZStream::sClampedSize(iCount, ::CFDataGetLength(fDataRef), fPosition);
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

uint64 ZStreamRPos_CFData::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRPos_CFData::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRPos_CFData::Imp_GetSize()
	{ return ::CFDataGetLength(fDataRef); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_CFData

ZStreamRWPos_CFData::ZStreamRWPos_CFData(CFMutableDataRef iDataRef, size_t iGrowIncrement)
:	fDataRef(iDataRef),
	fGrowIncrement(iGrowIncrement),
	fPosition(0),
	fSizeLogical(::CFDataGetLength(fDataRef))
	{}

ZStreamRWPos_CFData::ZStreamRWPos_CFData(CFMutableDataRef iDataRef)
:	fDataRef(iDataRef),
	fGrowIncrement(64),
	fPosition(0),
	fSizeLogical(::CFDataGetLength(iDataRef))
	{}

ZStreamRWPos_CFData::~ZStreamRWPos_CFData()
	{
	// Finally, make sure fDataRef is the real size, not the potentially
	// overallocated size we've been using
	::CFDataSetLength(fDataRef, fSizeLogical);
	}

void ZStreamRWPos_CFData::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	size_t countToCopy = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
	ZBlockCopy(::CFDataGetBytePtr(fDataRef) + fPosition, iDest, countToCopy);
	fPosition += countToCopy;

	if (oCountRead)
		*oCountRead = countToCopy;
	}

void ZStreamRWPos_CFData::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	size_t realSkip = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
	fPosition += realSkip;
	if (oCountSkipped)
		*oCountSkipped = realSkip;
	}

void ZStreamRWPos_CFData::Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
	{
	size_t availableSize = ::CFDataGetLength(fDataRef);

	uint64 neededSpace = fPosition + iCount;
	if (availableSize < neededSpace)
		{
		uint64 realSize = max(neededSpace, uint64(availableSize) + fGrowIncrement);
		if (realSize == size_t(realSize))
			::CFDataSetLength(fDataRef, realSize);
		}

	size_t countToCopy = ZStream::sClampedSize(iCount, ::CFDataGetLength(fDataRef), fPosition);

	ZBlockCopy(iSource, ::CFDataGetMutableBytePtr(fDataRef) + fPosition, iCount);

	fPosition += countToCopy;

	if (fSizeLogical < fPosition)
		fSizeLogical = fPosition;

	if (oCountWritten)
		*oCountWritten = countToCopy;
	}

void ZStreamRWPos_CFData::Imp_Flush()
	{ ::CFDataSetLength(fDataRef, fSizeLogical); }

uint64 ZStreamRWPos_CFData::Imp_GetPosition()
	{ return fPosition; }

void ZStreamRWPos_CFData::Imp_SetPosition(uint64 iPosition)
	{ fPosition = iPosition; }

uint64 ZStreamRWPos_CFData::Imp_GetSize()
	{ return fSizeLogical; }

void ZStreamRWPos_CFData::Imp_SetSize(uint64 iSize)
	{
	size_t realSize = iSize;
	if (realSize != iSize)
		sThrowBadSize();

	::CFDataSetLength(fDataRef, realSize);
	fSizeLogical = realSize;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
