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

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZValData_Any.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_Any::Rep

class ZValData_Any::Rep : public ZRefCounted
	{
public:
	Rep();
	Rep(size_t inSize);
	Rep(const vector<char>& iVector);

	vector<char> fVector;
	};

ZValData_Any::Rep::Rep()
	{}

ZValData_Any::Rep::Rep(size_t iSize)
:	fVector(0, iSize)
	{}

ZValData_Any::Rep::Rep(const vector<char>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValData_Any

ZAny ZValData_Any::AsAny() const
	{ return fRep->fVector; }

ZValData_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep->fVector.size()); }

ZValData_Any::ZValData_Any()
:	fRep(new Rep)
	{}

ZValData_Any::ZValData_Any(const ZValData_Any& iOther)
:	fRep(iOther.fRep)
	{}

ZValData_Any::~ZValData_Any()
	{}

ZValData_Any& ZValData_Any::operator=(const ZValData_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZValData_Any::ZValData_Any(size_t iSize)
:	fRep(new Rep(iSize))
	{}

ZValData_Any::ZValData_Any(const void* iSource, size_t iSize)
:	fRep(new Rep)
	{
	const char* source = static_cast<const char*>(iSource);
	fRep->fVector.insert(fRep->fVector.begin(), source, source + iSize);
	}

int ZValData_Any::Compare(const ZValData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return 0;

	return sCompare_T(fRep->fVector.begin(), fRep->fVector.end(),
		iOther.fRep->fVector.begin(), iOther.fRep->fVector.end());
	}

bool ZValData_Any::operator<(const ZValData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return false;
	return fRep->fVector < iOther.fRep->fVector;
	}

bool ZValData_Any::operator==(const ZValData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return false;
	return fRep->fVector == iOther.fRep->fVector;
	}

size_t ZValData_Any::GetSize() const
	{ return fRep->fVector.size(); }

void ZValData_Any::SetSize(size_t iSize)
	{
	if (iSize != fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.resize(iSize);
		}
	}

const void* ZValData_Any::GetData() const
	{ return &*fRep->fVector.begin(); }

void* ZValData_Any::GetData()
	{
	this->pTouch();
	return &*fRep->fVector.begin();
	}

void ZValData_Any::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->pTouch();
	const char* source = static_cast<const char*>(iSource);
	std::copy(source, source + iCount, fRep->fVector.begin() + iOffset);
	}

void ZValData_Any::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZValData_Any::CopyTo(size_t iOffset, void* iDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	vector<char>::const_iterator begin = fRep->fVector.begin() + iOffset;
	std::copy(begin + iOffset, begin + iCount, static_cast<char*>(iDest));
	}

void ZValData_Any::CopyTo(void* iDest, size_t iCount) const
	{ this->CopyTo(0, iDest, iCount); }

void ZValData_Any::pTouch()
	{
	if (fRep->GetRefCount() == 1)
		return;
	fRep = new Rep(fRep->fVector);
	}

NAMESPACE_ZOOLIB_END
