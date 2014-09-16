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
// MARK: - ChanBase_Bin_string

ChanBase_Bin_string::ChanBase_Bin_string(std::string* ioStringPtr)
:	fStringPtr(ioStringPtr)
,	fPosition(0)
	{}

size_t ChanBase_Bin_string::Read(byte* oDest, size_t iCount)
	{
	const size_t theSize = fStringPtr->size();
	const size_t countToCopy = std::min<size_t>(iCount,
		theSize > fPosition ? theSize - fPosition : 0);
	std::copy(&fStringPtr->at(fPosition), &fStringPtr->at(fPosition + iCount), (char*)oDest);
	fPosition += countToCopy;
	return countToCopy;
	}

size_t ChanBase_Bin_string::Readable()
	{
	const size_t theSize = fStringPtr->size();
	return theSize >= fPosition ? theSize - fPosition : 0;
	}

uint64 ChanBase_Bin_string::Count()
	{ return fStringPtr->size(); }

uint64 ChanBase_Bin_string::Pos()
	{ return fPosition; }

void ChanBase_Bin_string::SetPos(uint64 iPos)
	{ fPosition = iPos; }

// =================================================================================================
// MARK: - ChanRPos_Bin_string

ChanRPos_Bin_string::ChanRPos_Bin_string(const std::string& iString)
:	ChanBase_Bin_string(const_cast<std::string*>(&fString))
,	fString(iString)
	{}

size_t ChanRPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	const size_t countToCopy = std::min(iCount, this->fPosition);
	this->fPosition -= countToCopy;

	// See Chan_XX_Memory for a note regarding bogus unreads.

	return countToCopy;
	}

size_t ChanRPos_Bin_string::UnreadableLimit()
	{ return this->fPosition; }

// =================================================================================================
// MARK: - ChanRWPos_Bin_string

ChanRWPos_Bin_string::ChanRWPos_Bin_string(std::string* ioStringPtr)
:	ChanBase_Bin_string(ioStringPtr)
	{}

size_t ChanRWPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	const size_t countToCopy = std::min<size_t>(iCount, this->fPosition);

	std::copy(iSource, iSource + iCount, (byte*)(&fStringPtr->at(fPosition)));

	this->fPosition -= countToCopy;

	return countToCopy;
	}

size_t ChanRWPos_Bin_string::UnreadableLimit()
	{ return this->fPosition; }

size_t ChanRWPos_Bin_string::Write(const byte* iSource, size_t iCount)
	{
	const size_t newPosition = this->fPosition + iCount;
	if (this->fStringPtr->size() < newPosition)
		this->fStringPtr->resize(newPosition);

	std::copy(iSource, iSource + iCount, (byte*)(&fStringPtr->at(fPosition)));

	this->fPosition = newPosition;

	return iCount;
	}

void ChanRWPos_Bin_string::CountSet(uint64 iCount)
	{
	if (this->fPosition > iCount)
		this->fPosition = iCount;
	this->fStringPtr->resize(iCount);
	}

} // namespace ZooLib
