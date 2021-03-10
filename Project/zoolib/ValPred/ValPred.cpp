// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/ValPred.h"

#include "zoolib/Compare_Integer.h"
#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ValComparator

ValComparator::ValComparator()
	{}

ValComparator::~ValComparator()
	{}

// =================================================================================================
#pragma mark - ValComparator_Simple

ValComparator_Simple::ValComparator_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

ValComparator_Simple::EComparator ValComparator_Simple::GetEComparator() const
	{ return fEComparator; }

template <>
int sCompareNew_T(const ValComparator_Simple& iL, const ValComparator_Simple& iR)
	{ return sCompareNew_T(int(iL.GetEComparator()), int(iR.GetEComparator())); }

// =================================================================================================
#pragma mark - ValComparand

ValComparand::ValComparand()
	{}

ValComparand::~ValComparand()
	{}

// =================================================================================================
#pragma mark - ValComparand_Name

ValComparand_Name::ValComparand_Name(const std::string& iName)
:	fName(iName)
	{}

const std::string& ValComparand_Name::GetName() const
	{ return fName; }

template <>
int sCompareNew_T(const ValComparand_Name& iL, const ValComparand_Name& iR)
	{ return sCompareNew_T(iL.GetName(), iR.GetName()); }

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
#pragma mark - ValPred, sCompareNew_T

template <>
int sCompareNew_T(const ValPred& iL, const ValPred& iR)
	{
	if (int compare = sCompareNew_T(iL.GetLHS(), iR.GetLHS()))
		return compare;

	if (int compare = sCompareNew_T(iL.GetComparator(), iR.GetComparator()))
		return compare;

	return sCompareNew_T(iL.GetRHS(), iR.GetRHS());
	}

bool operator<(const ValPred& iL, const ValPred& iR)
	{ return sCompareNew_T(iL, iR) < 0; }

// =================================================================================================
#pragma mark - Comparand pseudo constructors

ValComparandPseudo CName(const std::string& iName)
	{ return new ValComparand_Name(iName); }

} // namespace ZooLib

