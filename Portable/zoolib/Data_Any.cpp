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

#include "zoolib/Compare.h"
#include "zoolib/Compare_vector.h"
#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/Data_Any.h"
#include "zoolib/Memory.h"
#include "zoolib/Util_STL_vector.h"

#include <vector>

namespace ZooLib {

using std::vector;

ZMACRO_CompareRegistration_T(Data_Any)

// =================================================================================================
#pragma mark - Data_Any::Rep

class Data_Any::Rep : public CountedWithoutFinalize
	{
public:
	Rep();
	Rep(size_t iSize);
	Rep(const vector<char>& iVector);

	vector<char> fVector;
	};

Data_Any::Rep::Rep()
	{}

Data_Any::Rep::Rep(size_t iSize)
:	fVector(iSize, 0)
	{}

Data_Any::Rep::Rep(const vector<char>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark - Data_Any

Data_Any::Data_Any()
:	fRep(new Rep)
	{}

Data_Any::Data_Any(const Data_Any& iOther)
:	fRep(iOther.fRep)
	{}

Data_Any::~Data_Any()
	{}

Data_Any& Data_Any::operator=(const Data_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Data_Any::Data_Any(size_t iSize)
:	fRep(new Rep(iSize))
	{}

Data_Any::Data_Any(const void* iSource, size_t iSize)
:	fRep(new Rep)
	{
	const char* source = static_cast<const char*>(iSource);
	fRep->fVector.insert(fRep->fVector.begin(), source, source + iSize);
	}

int Data_Any::Compare(const Data_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return 0;

	if (const size_t thisSize = fRep->fVector.size())
		{
		if (const size_t otherSize = iOther.fRep->fVector.size())
			{
			if (thisSize < otherSize)
				return -1;
			else if (otherSize < thisSize)
				return 1;
			else
				return sMemCompare(&fRep->fVector[0], &iOther.fRep->fVector[0], thisSize);
			}
		else
			{
			return 1;
			}
		}
	else if (iOther.fRep->fVector.size())
		{
		return -1;
		}
	else
		{
		return 0;
		}
	}

bool Data_Any::operator<(const Data_Any& iOther) const
	{ return this->Compare(iOther) < 0; }

bool Data_Any::operator==(const Data_Any& iOther) const
	{ return this->Compare(iOther) == 0; }

size_t Data_Any::GetSize() const
	{ return fRep->fVector.size(); }

void Data_Any::SetSize(size_t iSize)
	{
	if (iSize != fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.resize(iSize);
		}
	}

const void* Data_Any::GetPtr() const
	{ return Util_STL::sFirstOrNil(fRep->fVector); }

void* Data_Any::GetPtrMutable()
	{
	this->pTouch();
	return Util_STL::sFirstOrNil(fRep->fVector);
	}

void Data_Any::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->pTouch();
	std::copy_n(static_cast<const char*>(iSource), iCount, fRep->fVector.begin() + iOffset);
	}

void Data_Any::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void Data_Any::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	std::copy_n(fRep->fVector.begin() + iOffset, iCount, static_cast<char*>(oDest));
	}

void Data_Any::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

void Data_Any::pTouch()
	{
	if (fRep->IsShared())
		fRep = new Rep(fRep->fVector);
	}

} // namespace ZooLib
