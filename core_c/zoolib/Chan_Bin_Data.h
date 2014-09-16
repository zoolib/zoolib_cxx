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

#include "zoolib/ChanCount.h"
#include "zoolib/ChanCountSet.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanR_Bin.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW_Bin.h"
#include "zoolib/Util_Chan.h" // For sCopyAll

namespace ZooLib {

// =================================================================================================
// MARK: - ChanBase_Bin_Data

template <class Data_p>
class ChanBase_Bin_Data
:	public ChanR_Bin
,	public ChanCount
,	public ChanPos
	{
public:
	typedef Data_p Data;

	ChanBase_Bin_Data(Data* ioDataPtr)
	:	fDataPtr(ioDataPtr)
	,	fPosition(0)
		{}

// From ChanR
	virtual size_t Read(Elmt_t* oDest, size_t iCount)
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

// From ChanGetCount
	virtual uint64 Count()
		{ return fDataPtr->GetSize(); }

// From ChanPos
	virtual uint64 Pos()
		{ return fPosition; }

	virtual void SetPos(uint64 iPos)
		{ fPosition = iPos; }

protected:
	Data* fDataPtr;
	size_t fPosition;
	};

// =================================================================================================
// MARK: - ChanRPos_Bin_Data

template <class Data_p>
class ChanRPos_Bin_Data
:	public ChanBase_Bin_Data<Data_p>
,	public ChanU<byte>
	{
public:
	typedef Data_p Data;

	ChanRPos_Bin_Data(Data iData)
	:	ChanBase_Bin_Data<Data_p>(&fData)
	,	fData(iData)
		{}

// From ChanU
	virtual size_t Unread(const Elmt_t* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min(iCount, this->fPosition);
		this->fPosition -= countToCopy;

		// See Chan_XX_Memory for a note regarding bogus unreads.

		return countToCopy;
		}

	virtual size_t UnreadableLimit()
		{ return this->fPosition; }

	Data fData;
	};

// =================================================================================================
// MARK: - ChanRWPos_Bin_Data

template <class Data_p>
class ChanRWPos_Bin_Data
:	public ChanBase_Bin_Data<Data_p>
,	public ChanU<byte>
,	public ChanW_Bin
,	public ChanCountSet
	{
public:
	typedef Data_p Data;

	ChanRWPos_Bin_Data(Data* ioData)
	:	ChanBase_Bin_Data<Data_p>(ioData)
		{}

// From ChanU
	virtual size_t Unread(const byte* iSource, size_t iCount)
		{
		const size_t countToCopy = std::min<size_t>(iCount, this->fPosition);

		this->fDataPtr->CopyFrom(this->fPosition - countToCopy, iSource, countToCopy);

		this->fPosition -= countToCopy;

		return countToCopy;
		}

	virtual size_t UnreadableLimit()
		{ return this->fPosition; }

// From ChanW
	virtual size_t Write(const byte* iSource, size_t iCount)
		{
		const size_t newPosition = this->fPosition + iCount;
		if (this->fDataPtr->GetSize() < newPosition)
			this->fDataPtr->SetSize(newPosition);

		this->fDataPtr->CopyFrom(this->fPosition, iSource, iCount);

		this->fPosition = newPosition;

		return iCount;
		}

// From ChanCountSet
	virtual void CountSet(uint64 iCount)
		{
		if (this->fPosition > iCount)
			this->fPosition = iCount;
		this->fDataPtr->SetSize(iCount);
		}
	};

// =================================================================================================
// MARK: - Data stream reading functions

template <class Data_p>
Data_p sReadAll_T(const ChanR_Bin& iChanR)
	{
	Data_p theData;
	sCopyAll(iChanR, ChanRWPos_Bin_Data<Data_p>(&theData));
	return theData;
	}

template <class Data_p>
Data_p sRead_T(const ChanR_Bin& iChanR, size_t iSize)
	{
	Data_p theData;
	sCopyFully(iChanR, ChanRWPos_Bin_Data<Data_p>(&theData), iSize);
	return theData;
	}

} // namespace ZooLib

#endif // __ZooLib_Chan_Bin_Data_h__
