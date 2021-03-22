// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/ValPred.h"

#include "zoolib/Compare_string.h"
#include "zoolib/TypeIdName.h"

#include <cstring> // For strcmp

namespace ZooLib {

// =================================================================================================
#pragma mark - ValComparator

ValComparator::ValComparator()
	{}

ValComparator::~ValComparator()
	{}

int ValComparator::Compare(const ZP<ValComparator>& iOther)
	{
	if (int compare = strcmp(sTypeIdName(*this), sTypeIdName(*iOther.Get())))
		return compare;
	return 0;
	}

// =================================================================================================
#pragma mark - ValComparator_Simple

ValComparator_Simple::ValComparator_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

int ValComparator_Simple::Compare(const ZP<ValComparator>& iOther)
	{
	if (ZP<ValComparator_Simple> other = iOther.DynamicCast<ValComparator_Simple>())
		return sCompare_T(int(this->GetEComparator()), int(other->GetEComparator()));

	return ValComparator::Compare(iOther);
	}

ValComparator_Simple::EComparator ValComparator_Simple::GetEComparator() const
	{ return fEComparator; }

// =================================================================================================
#pragma mark - ValComparand

ValComparand::ValComparand()
	{}

ValComparand::~ValComparand()
	{}

int ValComparand::Compare(const ZP<ValComparand>& iOther)
	{
	if (int compare = strcmp(sTypeIdName(*this), sTypeIdName(*iOther.Get())))
		return compare;
	return 0;
	}

// =================================================================================================
#pragma mark - ValComparand_Name

ValComparand_Name::ValComparand_Name(const std::string& iName)
:	fName(iName)
	{}

const std::string& ValComparand_Name::GetName() const
	{ return fName; }

int ValComparand_Name::Compare(const ZP<ValComparand>& iOther)
	{
	if (ZP<ValComparand_Name> other = iOther.DynamicCast<ValComparand_Name>())
		return sCompare_T(this->GetName(), other->GetName());

	return ValComparand::Compare(iOther);
	}

// =================================================================================================
#pragma mark - ValPred

ValPred::ValPred()
	{}

ValPred::ValPred(const ValPred& iOther)
:	fLHS(iOther.fLHS)
,	fComparator(iOther.fComparator)
,	fRHS(iOther.fRHS)
	{}

ValPred::~ValPred()
	{}

ValPred& ValPred::operator=(const ValPred& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

ValPred::ValPred(
	const ZP<ValComparand>& iLHS,
	const ZP<ValComparator>& iComparator,
	const ZP<ValComparand>& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

const ZP<ValComparand>& ValPred::GetLHS() const
	{ return fLHS; }

const ZP<ValComparator>& ValPred::GetComparator() const
	{ return fComparator; }

const ZP<ValComparand>& ValPred::GetRHS() const
	{ return fRHS; }

// =================================================================================================
#pragma mark - ValPred, sCompare_T

template <>
int sCompare_T(const ValPred& iL, const ValPred& iR)
	{
	if (int compare = iL.GetLHS()->Compare(iR.GetLHS()))
		return compare;

	if (int compare = iL.GetComparator()->Compare(iR.GetComparator()))
		return compare;

	return iL.GetRHS()->Compare(iR.GetRHS());
	}

bool operator<(const ValPred& iL, const ValPred& iR)
	{ return sCompare_T(iL, iR) < 0; }

// =================================================================================================
#pragma mark - Comparand pseudo constructors

ValComparandPseudo CName(const std::string& iName)
	{ return new ValComparand_Name(iName); }

} // namespace ZooLib

