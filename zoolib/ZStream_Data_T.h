/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZStream_Data_T__
#define __ZStream_Data_T__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_Data_T

template <class T>
class ZStreamRPos_Data_T : public ZStreamRPos
	{
public:
	ZStreamRPos_Data_T(const T& iData)
	:	fData(iData),
		fPosition(0)
		{}

	~ZStreamRPos_Data_T()
		{}

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
		{
		size_t countToCopy = ZStream::sClampedSize(iCount, fData.GetSize(), fPosition);
		fData.CopyTo(fPosition, oDest, countToCopy);
		fPosition += countToCopy;

		if (oCountRead)
			*oCountRead = countToCopy;
		}

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped)
		{
		size_t realSkip = ZStream::sClampedSize(iCount, fData.GetSize(), fPosition);
		fPosition += realSkip;
		if (oCountSkipped)
			*oCountSkipped = realSkip;
		}

// From ZStreamRPos
	virtual uint64 Imp_GetPosition()
		{ return fPosition; }

	virtual void Imp_SetPosition(uint64 iPosition)
		{ fPosition = iPosition; }

	virtual uint64 Imp_GetSize()
		{ return fData.GetSize(); }

// Our protocol
	const T& GetData() const
		{ return fData; }

private:
	T fData;
	uint64 fPosition;
	};

template <class T>
ZStreamRPos_Data_T<T> MakeStreamRPos_Data_T(const T& iData)
	{ return ZStreamRPos_Data_T<T>(iData); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_Data_T

template <class T>
class ZStreamRWPos_Data_T : public ZStreamRWPos
	{
public:
	ZStreamRWPos_Data_T(T& iData, size_t iGrowIncrement)
	:	fData(iData)
		{
		fGrowIncrement = iGrowIncrement;
		fPosition = 0;
		fSizeLogical = fData.GetSize();
		}

	ZStreamRWPos_Data_T(T& iData)
	:	fData(iData)
		{
		fGrowIncrement = 64;
		fPosition = 0;
		fSizeLogical = fData.GetSize();
		}

	~ZStreamRWPos_Data_T()
		{
		// Finally, make sure the Data is the real size, not the potentially
		// overallocated size we've been using
		fData.SetSize(fSizeLogical);
		}

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
		{
		const size_t countToCopy = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
		fData.CopyTo(fPosition, oDest, countToCopy);
		fPosition += countToCopy;

		if (oCountRead)
			*oCountRead = countToCopy;
		}

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped)
		{
		const size_t realSkip = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
		fPosition += realSkip;
		if (oCountSkipped)
			*oCountSkipped = realSkip;
		}

// From ZStreamW via ZStreamRWPos
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten)
		{
		const uint64 neededSpace = fPosition + iCount;
		if (fData.GetSize() < neededSpace)
			{
			const uint64 realSize = std::max(neededSpace, uint64(fData.GetSize()) + fGrowIncrement);
			fData.SetSize(ZStream::sClampedSize(realSize));
			}

		size_t countToCopy = ZStream::sClampedSize(iCount, fData.GetSize(), fPosition);

		fData.CopyFrom(fPosition, iSource, iCount);

		fPosition += countToCopy;

		if (fSizeLogical < fPosition)
			fSizeLogical = fPosition;

		if (oCountWritten)
			*oCountWritten = countToCopy;
		}

	virtual void Imp_Flush()
		{ fData.SetSize(fSizeLogical); }

// From ZStreamRPos/ZStreamWPos via ZStreamRWPos
	virtual uint64 Imp_GetPosition()
		{ return fPosition; }

	virtual void Imp_SetPosition(uint64 iPosition)
		{ fPosition = iPosition; }

	virtual uint64 Imp_GetSize()
		{ return fSizeLogical; }

// From ZStreamWPos via ZStreamRWPos
	virtual void Imp_SetSize(uint64 iSize)
		{
		const size_t realSize = iSize;
		if (realSize != iSize)
			sThrowBadSize();

		fData.SetSize(realSize);
		fSizeLogical = realSize;
		}

// Our protocol
	const T& GetData() const
		{
		const_cast<T&>(fData).SetSize(fSizeLogical);
		return fData;
		}

	T& GetData()
		{
		fData.SetSize(fSizeLogical);
		return fData;
		}

private:
	T& fData;
	size_t fGrowIncrement;
	uint64 fPosition;
	size_t fSizeLogical;
	};

template <class T>
ZStreamRWPos_Data_T<T> MakeStreamRWPos_Data_T(T& iData)
	{ return ZStreamRWPos_Data_T<T>(iData); }

// =================================================================================================
#pragma mark -
#pragma mark * Data stream reading functions

template <class T>
void sReadAll_T(T& ioData, const ZStreamR& iStreamR)
	{ ZStreamRWPos_Data_T<T>(ioData).CopyAllFrom(iStreamR); }

template <class T>
void sRead_T(T& ioData, const ZStreamR& iStreamR, size_t iSize)
	{ ZStreamRWPos_Data_T<T>(ioData).CopyFrom(iStreamR, iSize); }

template <class T>
T sReadAll_T(const ZStreamR& iStreamR)
	{
	T theData;
	sReadAll_T<T>(theData, iStreamR);
	return theData;
	}

template <class T>
T sRead_T(const ZStreamR& iStreamR, size_t iSize)
	{
	T theData;
	sRead_T<T>(theData, iStreamR, iSize);
	return theData;
	}

} // namespace ZooLib

#endif // __ZStream_Data_T__
