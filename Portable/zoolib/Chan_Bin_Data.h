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

#ifndef __ZooLib_Chan_Bin_Data_h__
#define __ZooLib_Chan_Bin_Data_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/Util_Chan.h" // For sCopyAll

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanRPos_Bin_Data

template <class Data_p>
class ChanRPos_Bin_Data
:	public ChanRPos<byte>
	{
public:
	typedef Data_p Data;

	ChanRPos_Bin_Data(const Data& iData)
	:	fData(iData)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount)
		{
		const size_t theSize = fData.GetSize();
		const size_t countToCopy = std::min<size_t>(iCount,
			theSize > fPosition ? theSize - fPosition : 0);
		fData.CopyTo(fPosition, oDest, countToCopy);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{
		const size_t theSize = fData.GetSize();
		return theSize >= fPosition ? theSize - fPosition : 0;
		}

// From ChanSize
	virtual uint64 Size()
		{ return fData.GetSize(); }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanU
	virtual void Unread(const byte* iSource, size_t iCount)
		{
		ZAssert(fPosition >= iCount);
		fPosition -= iCount;
		// See Chan_XX_Memory for a note regarding bogus unreads.
		}

private:
	Data fData;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanRWPos_Bin_Data

template <class Data_p>
class ChanRWPos_Bin_Data
:	public ChanRWPos<byte>
	{
public:
	typedef Data_p Data;

	ChanRWPos_Bin_Data(Data* ioData)
	:	fDataPtr(ioData)
	,	fPosition(0)
		{}

	virtual size_t Read(byte* oDest, size_t iCount)
		{
		const size_t theSize = fDataPtr->GetSize();
		const size_t countToCopy = std::min<size_t>(iCount,
			theSize > fPosition ? theSize - fPosition : 0);
		fDataPtr->CopyTo(fPosition, oDest, countToCopy);
		fPosition += countToCopy;
		return countToCopy;
		}

	virtual size_t Readable()
		{
		const size_t theSize = fDataPtr->GetSize();
		return theSize >= fPosition ? theSize - fPosition : 0;
		}

// From ChanSize
	virtual uint64 Size()
		{ return fDataPtr->GetSize(); }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void PosSet(uint64 iPos)
		{ fPosition = iPos; }

// From ChanU
	virtual void Unread(const byte* iSource, size_t iCount)
		{
		ZAssert(fPosition >= iCount);
		fDataPtr->CopyFrom(fPosition - iCount, iSource, iCount);
		fPosition -= iCount;
		}

// From ChanW
	virtual size_t Write(const byte* iSource, size_t iCount)
		{
		const size_t newPosition = fPosition + iCount;
		if (fDataPtr->GetSize() < newPosition)
			fDataPtr->SetSize(newPosition);

		fDataPtr->CopyFrom(fPosition, iSource, iCount);

		fPosition = newPosition;

		return iCount;
		}

// From ChanSizeSet
	virtual void SizeSet(uint64 iSize)
		{
		if (fPosition > iSize)
			fPosition = iSize;
		fDataPtr->SetSize(iSize);
		}

private:
	Data* fDataPtr;
	size_t fPosition;
	};

// =================================================================================================
#pragma mark - ChanW_Bin_Data

template <class Data_p>
class ChanW_Bin_Data
:	public ChanW<byte>
	{
public:
	typedef Data_p Data;

	ChanW_Bin_Data(Data* ioData)
	:	fDataPtr(ioData)
		{}

	~ChanW_Bin_Data()
		{}

// From ChanW
	virtual size_t Write(const byte* iSource, size_t iCount)
		{
		const size_t thePos = fDataPtr->GetSize();
		fDataPtr->SetSize(thePos + iCount);

		fDataPtr->CopyFrom(thePos, iSource, iCount);

		return iCount;
		}
private:
	Data* fDataPtr;
	};

// =================================================================================================
#pragma mark - Data stream reading functions

template <class Data_p>
Data_p sReadAll_T(const ChanR_Bin& iChanR)
	{
	Data_p theData;
	sECopyAll(iChanR, ChanW_Bin_Data<Data_p>(&theData));
	return theData;
	}

template <class Data_p>
Data_p sRead_T(const ChanR_Bin& iChanR, size_t iSize)
	{
	Data_p theData(iSize);
	sEReadMem(iChanR, theData.GetPtrMutable(), iSize);
	return theData;
	}

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_Data_h__
