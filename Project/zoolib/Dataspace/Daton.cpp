// Copyright (c) 2016 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Daton.h"

// =================================================================================================
#pragma mark - sCompareNew_T

namespace ZooLib {

template <>
int sCompareNew_T<Dataspace::Daton>(const Dataspace::Daton& iL, const Dataspace::Daton& iR)
	{ return sCompareNew_T(iL.GetData(), iR.GetData()); }

} // namespace ZooLib

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Daton

Daton::Daton()
	{}

Daton::Daton(const Daton& iOther)
:	fData(iOther.fData)
	{}

Daton::~Daton()
	{}

Daton& Daton::operator=(const Daton& iOther)
	{
	fData = iOther.fData;
	return *this;
	}

Daton::Daton(Data_ZZ iData)
:	fData(iData)
	{}

bool Daton::operator==(const Daton& iOther) const
	{ return fData == iOther.fData; }

bool Daton::operator<(const Daton& iOther) const
	{ return fData < iOther.fData; }

Data_ZZ Daton::GetData() const
	{ return fData; }

} // namespace Dataspace
} // namespace ZooLib
