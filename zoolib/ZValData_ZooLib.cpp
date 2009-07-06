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

#include "zoolib/ZValData_ZooLib.h"
#include "zoolib/ZMemory.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_ZooLib::Rep

class ZValData_ZooLib::Rep : public ZRefCounted
	{
public:
	void* operator new(size_t iObjectSize, size_t iDataSize);
	void operator delete(void* iPtr, size_t iObjectSize);

	Rep();
	Rep(size_t inSize);

	size_t fSize;
	char fData[1];
	};

void* ZValData_ZooLib::Rep::operator new(size_t iObjectSize, size_t iDataSize)
	{ return new char[iObjectSize + iDataSize]; }

void ZValData_ZooLib::Rep::operator delete(void* iPtr, size_t iDataSize)
	{ delete[] static_cast<char*>(iPtr); }

ZValData_ZooLib::Rep::Rep()
:	fSize(0)
	{}

ZValData_ZooLib::Rep::Rep(size_t iSize)
:	fSize(iSize)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_ZooLib

ZValData_ZooLib::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep->fSize); }

void ZValData_ZooLib::swap(ZValData_ZooLib& iOther)
	{ std::swap(fRep, iOther.fRep); }

ZValData_ZooLib::ZValData_ZooLib()
:	fRep(new (0) Rep)
	{}

ZValData_ZooLib::ZValData_ZooLib(const ZValData_ZooLib& iOther)
:	fRep(iOther.fRep)
	{}

ZValData_ZooLib::~ZValData_ZooLib()
	{}

ZValData_ZooLib& ZValData_ZooLib::operator=(const ZValData_ZooLib& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZValData_ZooLib::ZValData_ZooLib(size_t iSize)
:	fRep(new (iSize) Rep(iSize))
	{}

ZValData_ZooLib::ZValData_ZooLib(const void* iSource, size_t iSize)
:	fRep(new (iSize) ZValData_ZooLib::Rep(iSize))
	{ ZBlockCopy(iSource, fRep->fData, iSize); }

int ZValData_ZooLib::Compare(const ZValData_ZooLib& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return 0;

	if (int result = memcmp(myRep->fData, otherRep->fData, std::min(myRep->fSize, otherRep->fSize)))
		return result;

	return int(myRep->fSize) - int(otherRep->fSize);
	}

bool ZValData_ZooLib::operator<(const ZValData_ZooLib& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return false;

	if (int result = memcmp(myRep->fData, otherRep->fData, std::min(myRep->fSize, otherRep->fSize)))
		return result < 0;

	return fRep->fSize < otherRep->fSize;
	}

bool ZValData_ZooLib::operator==(const ZValData_ZooLib& iOther) const
	{
	Rep* myRep = fRep.GetObject();
	Rep* otherRep = iOther.fRep.GetObject();

	if (myRep == otherRep)
		return true;

	if (myRep->fSize != otherRep->fSize)
		return false;

	return 0 == memcmp(myRep->fData, otherRep->fData, myRep->fSize);
	}

size_t ZValData_ZooLib::GetSize() const
	{ return fRep->fSize; }

void ZValData_ZooLib::SetSize(size_t iSize)
	{
	if (iSize != fRep->fSize)
		{
		Rep* newRep = new (iSize) Rep(iSize);
		ZBlockCopy(fRep->fData, newRep->fData, std::min(fRep->fSize, newRep->fSize));
		fRep = newRep;
		}
	}

const void* ZValData_ZooLib::GetData() const
	{ return fRep->fData; }

void* ZValData_ZooLib::GetData()
	{
	this->pTouch();
	return fRep->fData;
	}

void ZValData_ZooLib::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->pTouch();
	ZBlockCopy(iSource, fRep->fData + iOffset, iCount);
	}

void ZValData_ZooLib::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZValData_ZooLib::CopyTo(size_t iOffset, void* iDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	ZBlockCopy(fRep->fData + iOffset, iDest, iCount);
	}

void ZValData_ZooLib::CopyTo(void* iDest, size_t iCount) const
	{ this->CopyTo(0, iDest, iCount); }

void ZValData_ZooLib::pTouch()
	{
	if (fRep->GetRefCount() == 1)
		return;
	Rep* newRep = new (fRep->fSize) Rep(fRep->fSize);
	ZBlockCopy(fRep->fData, newRep->fData, fRep->fSize);
	fRep = newRep;
	}

NAMESPACE_ZOOLIB_END
