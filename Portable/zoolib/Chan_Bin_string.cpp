// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

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

uint64 ChanRPos_Bin_string::Pos()
	{ return fPosition; }

void ChanRPos_Bin_string::PosSet(uint64 iPos)
	{ fPosition = sClamped(iPos); }

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

size_t ChanRPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	const size_t countToCopy = std::min(iCount, this->fPosition);
	this->fPosition -= countToCopy;

	// See Chan_XX_Memory for a note regarding bogus unreads.

	return countToCopy;
	}

// =================================================================================================
#pragma mark - ChanRWPos_Bin_string

ChanRWPos_Bin_string::ChanRWPos_Bin_string(std::string* ioStringPtr)
:	fStringPtr(ioStringPtr)
,	fPosition(0)
	{}

uint64 ChanRWPos_Bin_string::Pos()
	{ return fPosition; }

void ChanRWPos_Bin_string::PosSet(uint64 iPos)
	{ fPosition = sClamped(iPos); }

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

void ChanRWPos_Bin_string::SizeSet(uint64 iSize)
	{
	const size_t actualSize = sClamped(iSize);
	if (fPosition > actualSize)
		fPosition = actualSize;
	fStringPtr->resize(actualSize);
	}

size_t ChanRWPos_Bin_string::Unread(const byte* iSource, size_t iCount)
	{
	const size_t countToCopy = std::min<size_t>(iCount, this->fPosition);

	std::copy_n(iSource, countToCopy, (byte*)(&fStringPtr->at(fPosition)));

	this->fPosition -= countToCopy;

	return countToCopy;
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

} // namespace ZooLib
