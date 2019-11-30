// Copyright (c) 2009-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Data_ZZ.h"

#include "zoolib/Compare.h"
#include "zoolib/Compare_vector.h"
#include "zoolib/CountedWithoutFinalize.h"
#include "zoolib/Memory.h"
#include "zoolib/Util_STL_vector.h"

#include <vector>

namespace ZooLib {

using std::vector;

ZMACRO_CompareRegistration_T(Data_ZZ)

// =================================================================================================
#pragma mark - Data_ZZ::Rep

class Data_ZZ::Rep : public CountedWithoutFinalize
	{
public:
	Rep();
	Rep(size_t iSize);
	Rep(const vector<char>& iVector);

	vector<char> fVector;
	};

Data_ZZ::Rep::Rep()
	{}

Data_ZZ::Rep::Rep(size_t iSize)
:	fVector(iSize, 0)
	{}

Data_ZZ::Rep::Rep(const vector<char>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark - Data_ZZ

Data_ZZ::Data_ZZ()
:	fRep(new Rep)
	{}

Data_ZZ::Data_ZZ(const Data_ZZ& iOther)
:	fRep(iOther.fRep)
	{}

Data_ZZ::~Data_ZZ()
	{}

Data_ZZ& Data_ZZ::operator=(const Data_ZZ& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Data_ZZ::Data_ZZ(size_t iSize)
:	fRep(new Rep(iSize))
	{}

Data_ZZ::Data_ZZ(const void* iSource, size_t iSize)
:	fRep(new Rep)
	{
	const char* source = static_cast<const char*>(iSource);
	fRep->fVector.insert(fRep->fVector.begin(), source, source + iSize);
	}

int Data_ZZ::Compare(const Data_ZZ& iOther) const
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

bool Data_ZZ::operator<(const Data_ZZ& iOther) const
	{ return this->Compare(iOther) < 0; }

bool Data_ZZ::operator==(const Data_ZZ& iOther) const
	{ return this->Compare(iOther) == 0; }

size_t Data_ZZ::GetSize() const
	{ return fRep->fVector.size(); }

void Data_ZZ::SetSize(size_t iSize)
	{
	if (iSize != fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.resize(iSize);
		}
	}

const void* Data_ZZ::GetPtr() const
	{ return Util_STL::sFirstOrNil(fRep->fVector); }

void* Data_ZZ::GetPtrMutable()
	{
	this->pTouch();
	return Util_STL::sFirstOrNil(fRep->fVector);
	}

void Data_ZZ::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->pTouch();
	std::copy_n(static_cast<const char*>(iSource), iCount, fRep->fVector.begin() + iOffset);
	}

void Data_ZZ::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void Data_ZZ::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	std::copy_n(fRep->fVector.begin() + iOffset, iCount, static_cast<char*>(oDest));
	}

void Data_ZZ::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

void Data_ZZ::pTouch()
	{
	if (fRep->IsShared())
		fRep = new Rep(fRep->fVector);
	}

} // namespace ZooLib
