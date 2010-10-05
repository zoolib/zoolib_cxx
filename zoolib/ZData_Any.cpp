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

#include "zoolib/ZCompare.h"
#include "zoolib/ZCompare_Vector.h"
#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZData_Any.h"
#include "zoolib/ZUtil_STL.h"

#include <vector>

namespace ZooLib {

using std::vector;

ZMACRO_CompareRegistration_T(ZData_Any)

// =================================================================================================
#pragma mark -
#pragma mark * ZData_Any::Rep

class ZData_Any::Rep : public ZCountedWithoutFinalize
	{
public:
	Rep();
	Rep(size_t iSize);
	Rep(const vector<char>& iVector);

	vector<char> fVector;
	};

ZData_Any::Rep::Rep()
	{}

ZData_Any::Rep::Rep(size_t iSize)
:	fVector(iSize, 0)
	{}

ZData_Any::Rep::Rep(const vector<char>& iVector)
:	fVector(iVector)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZData_Any

ZAny ZData_Any::AsAny() const
	{ return ZAny(*this); }

ZData_Any::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fRep->fVector.size()); }

ZData_Any::ZData_Any()
:	fRep(new Rep)
	{}

ZData_Any::ZData_Any(const ZData_Any& iOther)
:	fRep(iOther.fRep)
	{}

ZData_Any::~ZData_Any()
	{}

ZData_Any& ZData_Any::operator=(const ZData_Any& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZData_Any::ZData_Any(size_t iSize)
:	fRep(new Rep(iSize))
	{}

ZData_Any::ZData_Any(const void* iSource, size_t iSize)
:	fRep(new Rep)
	{
	const char* source = static_cast<const char*>(iSource);
	fRep->fVector.insert(fRep->fVector.begin(), source, source + iSize);
	}

int ZData_Any::Compare(const ZData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return 0;

	return sCompare_T(fRep->fVector, iOther.fRep->fVector);
	}

bool ZData_Any::operator<(const ZData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return false;
	return fRep->fVector < iOther.fRep->fVector;
	}

bool ZData_Any::operator==(const ZData_Any& iOther) const
	{
	if (fRep == iOther.fRep)
		return false;
	return fRep->fVector == iOther.fRep->fVector;
	}

size_t ZData_Any::GetSize() const
	{ return fRep->fVector.size(); }

void ZData_Any::SetSize(size_t iSize)
	{
	if (iSize != fRep->fVector.size())
		{
		this->pTouch();
		fRep->fVector.resize(iSize);
		}
	}

const void* ZData_Any::GetData() const
	{ return ZUtil_STL::sFirstOrNil(fRep->fVector); }

void* ZData_Any::GetData()
	{
	this->pTouch();
	return ZUtil_STL::sFirstOrNil(fRep->fVector);
	}

void ZData_Any::CopyFrom(size_t iOffset, const void* iSource, size_t iCount)
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	this->pTouch();
	const char* source = static_cast<const char*>(iSource);
	std::copy(source, source + iCount, fRep->fVector.begin() + iOffset);
	}

void ZData_Any::CopyFrom(const void* iSource, size_t iCount)
	{ this->CopyFrom(0, iSource, iCount); }

void ZData_Any::CopyTo(size_t iOffset, void* oDest, size_t iCount) const
	{
	ZAssertStop(2, iCount + iOffset <= this->GetSize());
	if (iCount == 0)
		return;
	vector<char>::const_iterator begin = fRep->fVector.begin() + iOffset;
	std::copy(begin, begin + iCount, static_cast<char*>(oDest));
	}

void ZData_Any::CopyTo(void* oDest, size_t iCount) const
	{ this->CopyTo(0, oDest, iCount); }

void ZData_Any::pTouch()
	{
	if (fRep->IsShared())
		fRep = new Rep(fRep->fVector);
	}

} // namespace ZooLib
