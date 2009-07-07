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

#ifndef __ZStream_ValData_T__
#define __ZStream_ValData_T__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRPos_ValData_T

template <class T>
class ZStreamRPos_ValData_T : public ZStreamRPos
	{
public:
	ZStreamRPos_ValData_T(const T& iValData)
	:	fValData(iValData),
		fPosition(0)
		{}
	
	~ZStreamRPos_ValData_T()
		{}

// From ZStreamR via ZStreamRPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
		{
		size_t countToCopy = ZStream::sClampedSize(iCount, fValData.GetSize(), fPosition);
		fValData.CopyTo(fPosition, iDest, countToCopy);
		fPosition += countToCopy;

		if (oCountRead)
			*oCountRead = countToCopy;
		}

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped)
		{
		size_t realSkip = ZStream::sClampedSize(iCount, fValData.GetSize(), fPosition);
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
		{ return fValData.GetSize(); }

private:
	T fValData;
	uint64 fPosition;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamRWPos_ValData_T

template <class T>
class ZStreamRWPos_ValData_T : public ZStreamRWPos
	{
public:
	ZStreamRWPos_ValData_T(T& iValData, size_t iGrowIncrement)
	:	fValData(iValData)
		{
		fGrowIncrement = iGrowIncrement;
		fPosition = 0;
		fSizeLogical = fValData.GetSize();
		}
	
	ZStreamRWPos_ValData_T(T& iValData)
	:	fValData(iValData)
		{
		fGrowIncrement = 64;
		fPosition = 0;
		fSizeLogical = fValData.GetSize();
		}

	~ZStreamRWPos_ValData_T()
		{
		// Finally, make sure the Data is the real size, not the potentially
		// overallocated size we've been using
		fValData.SetSize(fSizeLogical);
		}

// From ZStreamR via ZStreamRWPos
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
		{
		const size_t countToCopy = ZStream::sClampedSize(iCount, fSizeLogical, fPosition);
		fValData.CopyTo(fPosition, iDest, countToCopy);
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
		if (fValData.GetSize() < neededSpace)
			{
			const uint64 realSize = std::max(neededSpace, uint64(fValData.GetSize()) + fGrowIncrement);
			fValData.SetSize(ZStream::sClampedSize(realSize));
			}

		size_t countToCopy = ZStream::sClampedSize(iCount, fValData.GetSize(), fPosition);

		fValData.CopyFrom(fPosition, iSource, iCount);

		fPosition += countToCopy;

		if (fSizeLogical < fPosition)
			fSizeLogical = fPosition;

		if (oCountWritten)
			*oCountWritten = countToCopy;
		}

	virtual void Imp_Flush()
		{ fValData.SetSize(fSizeLogical); }

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

		fValData.SetSize(realSize);
		fSizeLogical = realSize;
		}

private:
	T& fValData;
	size_t fGrowIncrement;
	uint64 fPosition;
	size_t fSizeLogical;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Data stream reading functions

template <class T>
void sReadAll_T(T& ioValData, const ZStreamR& iStreamR)
	{ ZStreamRWPos_ValData_T<T>(ioValData).CopyAllFrom(iStreamR); }

template <class T>
void sRead_T(T& ioValData, const ZStreamR& iStreamR, size_t iSize)
	{ ZStreamRWPos_ValData_T<T>(ioValData).CopyFrom(iStreamR, iSize); }

template <class T>
T sReadAll_T(const ZStreamR& iStreamR)
	{
	T theData;
	sReadAll_T<T>(theData, iStreamR);
	return theData;
	}

template <class T>
T sReadAll(const ZStreamR& iStreamR, size_t iSize)
	{
	T theData;
	sRead_T<T>(theData, iStreamR, iSize);
	return theData;
	}

NAMESPACE_ZOOLIB_END

#endif // __ZStream_ValData_T__
