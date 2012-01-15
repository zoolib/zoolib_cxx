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

#include "zoolib/ZStreamR_Source.h"
#include "zoolib/ZMemory.h" // For ZMemSet & ZMemCopy

using std::min;
using std::pair;

namespace ZooLib {

// =================================================================================================
// MARK: - ZStreamR_Source

/**
\class ZStreamR_Source
\ingroup stream
ZStreamR_Source provides an endless repeating sequence of bytes. The sequence is defined
by calling the appropriate constructor.
*/

ZStreamR_Source::ZStreamR_Source()
	{
	fData = new uint8[1024];
	fDataSize = 1024;
	ZMemZero(fData, 1024);
	fOffset = 0;
	}

ZStreamR_Source::ZStreamR_Source(uint8 iData)
	{
	fData = new uint8[1024];
	fDataSize = 1024;
	ZMemSet(fData, iData, 1024);
	fOffset = 0;
	}

ZStreamR_Source::ZStreamR_Source(const void* iData, size_t iDataSize)
	{
	fData = new uint8[iDataSize];
	fDataSize = iDataSize;
	ZMemCopy(fData, iData, fDataSize);
	fOffset = 0;
	}

ZStreamR_Source::ZStreamR_Source(const pair<const void*, size_t>& iParam)
	{
	fData = new uint8[iParam.second];
	fDataSize = iParam.second;
	ZMemCopy(fData, iParam.first, fDataSize);
	fOffset = 0;
	}

ZStreamR_Source::~ZStreamR_Source()
	{
	delete[] fData;
	}

void ZStreamR_Source::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount)
		{
		size_t countToMove = min(iCount, fDataSize - fOffset);
		ZMemCopy(localDest, fData + fOffset, countToMove);
		fOffset = (fOffset + countToMove) % fDataSize;
		localDest += countToMove;
		iCount -= countToMove;
		}
	if (oCountRead)
		*oCountRead += localDest - reinterpret_cast<uint8*>(oDest);
	}

void ZStreamR_Source::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten);
	}

void ZStreamR_Source::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	this->pCopyTo(iStreamW, iCount, oCountRead, oCountWritten);
	}

void ZStreamR_Source::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	fOffset = (fOffset + iCount) % fDataSize;
	if (oCountSkipped)
		*oCountSkipped = iCount;
	}

void ZStreamR_Source::pCopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	if (oCountRead)
		*oCountRead = 0;
	if (oCountWritten)
		*oCountWritten = 0;
	while (iCount)
		{
		uint64 countToWrite = min(iCount, uint64(fDataSize - fOffset));
		size_t countWritten;
		iStreamW.Write(fData + fOffset, countToWrite, &countWritten);
		if (countWritten == 0)
			break;
		fOffset = (fOffset + countWritten) % fDataSize;
		iCount -= countWritten;
		if (oCountRead)
			*oCountRead += countWritten;
		if (oCountWritten)
			*oCountWritten += countWritten;
		}
	}

// =================================================================================================
// MARK: - ZStreamerR_Source

ZStreamerR_Source::ZStreamerR_Source()
	{}

ZStreamerR_Source::ZStreamerR_Source(uint8 iData)
:	fStream(iData)
	{}

ZStreamerR_Source::ZStreamerR_Source(const void* iData, size_t iDataSize)
:	fStream(iData, iDataSize)
	{}

ZStreamerR_Source::~ZStreamerR_Source()
	{}

const ZStreamR& ZStreamerR_Source::GetStreamR()
	{ return fStream; }

} // namespace ZooLib
