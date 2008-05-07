/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "ZTQL_Condition.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep

ZTQL::ComparatorRep::ComparatorRep()
	{}

ZTQL::ComparatorRep::~ComparatorRep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparatorRep_Simple

ZTQL::ComparatorRep_Simple::ComparatorRep_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

bool ZTQL::ComparatorRep_Simple::Matches(const ZTValue& iLHS, const ZTValue& iRHS)
	{	
	switch (fEComparator)
		{
		case eLT: return iLHS < iRHS;
		case eLE: return iLHS <= iRHS;
		case eEQ: return iLHS == iRHS;
		case eGE: return iLHS >= iRHS;
		case eGT: return iLHS > iRHS;
		}
	ZUnimplemented();
	return iLHS < iRHS;
	}

ZTQL::ComparatorRep_Simple::EComparator ZTQL::ComparatorRep_Simple::GetEComparator()
	{ return fEComparator; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Comparator

ZTQL::Comparator::Comparator()
	{}

ZTQL::Comparator::Comparator(ZRef<ComparatorRep> iRep)
:	fRep(iRep)
	{}

ZRef<ZTQL::ComparatorRep> ZTQL::Comparator::GetRep() const
	{ return fRep; }

bool ZTQL::Comparator::Matches(const ZTValue& iLHS, const ZTValue& iRHS) const
	{
	if (fRep)
		return fRep->Matches(iLHS, iRHS);
	// Assume LT, for the sake of having defined behavior.
	return iLHS < iRHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep

ZTQL::ComparandRep::ComparandRep()
	{}

ZTQL::ComparandRep::~ComparandRep()
	{}

void ZTQL::ComparandRep::GatherPropNames(set<ZTName>& ioNames)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep_Name

ZTQL::ComparandRep_Name::ComparandRep_Name(const ZTName& iName)
:	fName(iName)
	{}

const ZTValue& ZTQL::ComparandRep_Name::Imp_GetValue(const ZTuple& iTuple)
	{ return iTuple.GetValue(fName); }

void ZTQL::ComparandRep_Name::GatherPropNames(set<ZTName>& ioNames)
	{ ioNames.insert(fName); }

const ZTName& ZTQL::ComparandRep_Name::GetName()
	{ return fName; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::ComparandRep_Value

ZTQL::ComparandRep_Value::ComparandRep_Value(const ZTValue& iValue)
:	fValue(iValue)
	{}

const ZTValue& ZTQL::ComparandRep_Value::Imp_GetValue(const ZTuple& iTuple)
	{ return fValue; }

const ZTValue& ZTQL::ComparandRep_Value::GetValue()
	{ return fValue; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Comparand

ZTQL::Comparand::Comparand()
	{}

ZTQL::Comparand::Comparand(const Comparand& iOther)
:	fRep(iOther.fRep)
	{}

ZTQL::Comparand::Comparand(ZRef<ComparandRep> iRep)
:	fRep(iRep)
	{}

ZTQL::Comparand::~Comparand()
	{}

ZTQL::Comparand& ZTQL::Comparand::operator=(const Comparand& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

ZRef<ZTQL::ComparandRep> ZTQL::Comparand::GetRep() const
	{ return fRep; }

const ZTValue& ZTQL::Comparand::GetValue(const ZTuple& iTuple) const
	{
	if (fRep)
		return fRep->Imp_GetValue(iTuple);

	return ZTValue();
	}

void ZTQL::Comparand::GatherPropNames(set<ZTName>& ioNames) const
	{
	if (fRep)
		fRep->GatherPropNames(ioNames);
	}

ZTQL::Condition ZTQL::Comparand::LT(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eLT),
		iRHS.fRep);
	}

ZTQL::Condition ZTQL::Comparand::LE(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eLE),
		iRHS.fRep);
	}

ZTQL::Condition ZTQL::Comparand::EQ(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eEQ),
		iRHS.fRep);
	}

ZTQL::Condition ZTQL::Comparand::GE(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eGE),
		iRHS.fRep);
	}

ZTQL::Condition ZTQL::Comparand::GT(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eGT),
		iRHS.fRep);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL::Condition

ZTQL::Condition::Condition(
	ZRef<ComparandRep> iLHS, ZRef<ComparatorRep> iComparator, ZRef<ComparandRep> iRHS)
:	fLHS(iLHS),
	fComparator(iComparator),
	fRHS(iRHS)
	{}

ZTQL::Condition::Condition(
	const Comparand& iLHS, const Comparator& iComparator, const Comparand& iRHS)
:	fLHS(iLHS),
	fComparator(iComparator),
	fRHS(iRHS)
	{}

const ZTQL::Comparand& ZTQL::Condition::GetLHS() const
	{ return fLHS; }

const ZTQL::Comparator& ZTQL::Condition::GetComparator() const
	{ return fComparator; }

const ZTQL::Comparand& ZTQL::Condition::GetRHS() const
	{ return fRHS; }

bool ZTQL::Condition::Matches(const ZTuple& iTuple) const
	{ return fComparator.Matches(fLHS.GetValue(iTuple), fRHS.GetValue(iTuple)); }

void ZTQL::Condition::GatherPropNames(set<ZTName>& ioNames) const
	{
	fLHS.GatherPropNames(ioNames);
	fRHS.GatherPropNames(ioNames);
	}
