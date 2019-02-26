/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/Chan_Bin_string.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanW_Bin_string

ChanW_Bin_string::ChanW_Bin_string(std::string* ioString)
:	fStringPtr(ioString)
	{}

size_t ChanW_Bin_string::Write(const byte* iSource, size_t iCountCU)
	{
	fStringPtr->append((char*)iSource, iCountCU);
	return iCountCU;
	}

// =================================================================================================
#pragma mark - ChanRPos_Bin_string

ChanRPos_Bin_string::ChanRPos_Bin_string(const std::string& iString)
:	fString(iString)
,	fPosition(0)
	{}

size_t ChanRPos_Bin_string::Read(byte* oDest, size_t iCount)
	{
	const size_t theSize = fString.size();
	if (const size_t countToCopy =
		std::min<size_t>(iCount, theSize > fPosition ? theSize - fPosition : 0))
		{
		std::copy_n(&fString.at(fPosition), countToCopy, (char*)oDest);
		fPosition += countToCopy;
		return countToCopy;
		}
	return 0;
	}

size_t ChanRPos_Bin_string::Readable()
	{
	const size_t theSize = fString.size();
	return theSize >= fPosition ? theSize - fPosition : 0;
	}

uint64 ChanRPos_Bin_string::Size()
	{ return fString.size(); }

uint64 ChanRPos_Bin_string::Pos()
	{ return fPosition; }

void ChanRPos_Bin_string::PosSet(uint64 iPos)
	{ fPosition = iPos; }

void ChanRPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	ZAssert(fPosition >= iCount);
	fPosition -= iCount;
	// See Chan_XX_Memory for a note regarding bogus unreads.
	}

// =================================================================================================
#pragma mark - ChanRWPos_Bin_string

ChanRWPos_Bin_string::ChanRWPos_Bin_string(std::string* ioStringPtr)
:	fStringPtr(ioStringPtr)
,	fPosition(0)
	{}

size_t ChanRWPos_Bin_string::Read(byte* oDest, size_t iCount)
	{
	const size_t theSize = fStringPtr->size();
	if (const size_t countToCopy =
		std::min<size_t>(iCount, theSize > fPosition ? theSize - fPosition : 0))
		{
		std::copy_n(&fStringPtr->at(fPosition), countToCopy, (char*)oDest);
		fPosition += countToCopy;
		return countToCopy;
		}
	return 0;
	}

size_t ChanRWPos_Bin_string::Readable()
	{
	const size_t theSize = fStringPtr->size();
	return theSize >= fPosition ? theSize - fPosition : 0;
	}

uint64 ChanRWPos_Bin_string::Size()
	{ return fStringPtr->size(); }

uint64 ChanRWPos_Bin_string::Pos()
	{ return fPosition; }

void ChanRWPos_Bin_string::PosSet(uint64 iPos)
	{ fPosition = iPos; }

void ChanRWPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	ZAssert(fPosition >= iCount);
	fPosition -= iCount;
	}

size_t ChanRWPos_Bin_string::Write(const byte* iSource, size_t iCount)
	{
	const size_t newPosition = fPosition + iCount;

	if (fStringPtr->size() < newPosition)
		fStringPtr->resize(newPosition);

	std::copy_n(iSource, iCount, (byte*)(&fStringPtr->at(fPosition)));

	fPosition = newPosition;

	return iCount;
	}

void ChanRWPos_Bin_string::SizeSet(uint64 iSize)
	{
	if (fPosition > iSize)
		fPosition = iSize;
	fStringPtr->resize(iSize);
	}

} // namespace ZooLib
