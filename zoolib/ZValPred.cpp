/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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
#include "zoolib/ZValPred.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator

ZValComparator::ZValComparator()
	{}

ZValComparator::~ZValComparator()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparator_Simple

ZValComparator_Simple::ZValComparator_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

ZValComparator_Simple::EComparator ZValComparator_Simple::GetEComparator()  const
	{ return fEComparator; }

template <>
int sCompare_T(const ZValComparator_Simple& iL, const ZValComparator_Simple& iR)
	{ return sCompare_T(int(iL.GetEComparator()), int(iR.GetEComparator())); }

ZMACRO_CompareRegistration_T(ZValComparator_Simple)

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand

ZValComparand::ZValComparand()
	{}

ZValComparand::~ZValComparand()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_Name

ZValComparand_Name::ZValComparand_Name(const std::string& iName)
:	fName(iName)
	{}

const std::string& ZValComparand_Name::GetName() const
	{ return fName; }

template <>
int sCompare_T(const ZValComparand_Name& iL, const ZValComparand_Name& iR)
	{ return sCompare_T(iL.GetName(), iR.GetName()); }

ZMACRO_CompareRegistration_T(ZValComparand_Name)

// =================================================================================================
#pragma mark -
#pragma mark * ZValPred

ZValPred::ZValPred()
	{}

ZValPred::ZValPred(const ZValPred& iOther)
:	fLHS(iOther.fLHS)
,	fComparator(iOther.fComparator)
,	fRHS(iOther.fRHS)
	{}

ZValPred::~ZValPred()
	{}

ZValPred& ZValPred::operator=(const ZValPred& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

ZValPred::ZValPred(
	const ZRef<ZValComparand>& iLHS,
	const ZRef<ZValComparator>& iComparator,
	const ZRef<ZValComparand>& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

const ZRef<ZValComparand>& ZValPred::GetLHS() const
	{ return fLHS; }

const ZRef<ZValComparator>& ZValPred::GetComparator() const
	{ return fComparator; }

const ZRef<ZValComparand>& ZValPred::GetRHS() const
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValPred, sCompare_T

namespace { // anonymous

int spCompare(
	const ZRef<ZValComparand>& iLHS, const ZRef<ZValComparand>& iRHS)
	{
	const char* typeName = typeid(*iLHS.Get()).name();
	if (int compare = strcmp(typeName, typeid(*iRHS.Get()).name()))
		return compare;
	
	return ZCompare::sCompare(typeName, iLHS.Get(), iRHS.Get());
	}

int spCompare(
	const ZRef<ZValComparator>& iLHS, const ZRef<ZValComparator>& iRHS)
	{
	const char* typeName = typeid(*iLHS.Get()).name();
	if (int compare = strcmp(typeName, typeid(*iRHS.Get()).name()))
		return compare;
	
	return ZCompare::sCompare(typeName, iLHS.Get(), iRHS.Get());
	}

} // anonymous namespace

template<>
int sCompare_T(const ZValPred& iL, const ZValPred& iR)
	{		
	if (int compare = spCompare(iL.GetLHS(), iR.GetLHS()))
		return compare;

	if (int compare = spCompare(iL.GetComparator(), iR.GetComparator()))
		return compare;

	return spCompare(iL.GetRHS(), iR.GetRHS());
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandPseudo

ZValComparandPseudo::ZValComparandPseudo(ZValComparand* iRep)
:	ZRef<ZValComparand>(iRep)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Comparand pseudo constructors

ZValComparandPseudo CName(const std::string& iName)
	{ return new ZValComparand_Name(iName); }

// =================================================================================================
#pragma mark -
#pragma mark * Comparison operators taking comparands and returning a condition

ZValPred operator<(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eLT), iRHS); }

ZValPred operator<=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eLE), iRHS); }

ZValPred operator==(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eEQ), iRHS); }

ZValPred operator!=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eNE), iRHS); }

ZValPred operator>=(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eGE), iRHS); }

ZValPred operator>(const ZValComparandPseudo& iLHS, const ZValComparandPseudo& iRHS)
	{ return ZValPred(iLHS, new ZValComparator_Simple(ZValComparator_Simple::eGT), iRHS); }

} // namespace ZooLib

