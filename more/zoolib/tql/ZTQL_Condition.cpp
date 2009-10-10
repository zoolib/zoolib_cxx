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

#include "zoolib/tql/ZTQL_Condition.h"

using std::set;
using std::string;

NAMESPACE_ZOOLIB_BEGIN

namespace ZTQL {

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep

ComparatorRep::ComparatorRep()
	{}

ComparatorRep::~ComparatorRep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ComparatorRep_Simple

ComparatorRep_Simple::ComparatorRep_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

bool ComparatorRep_Simple::Matches(const Value& iLHS, const Value& iRHS)
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

ComparatorRep_Simple::EComparator ComparatorRep_Simple::GetEComparator()
	{ return fEComparator; }

// =================================================================================================
#pragma mark -
#pragma mark * Comparator

Comparator::Comparator()
	{}

Comparator::Comparator(ZRef<ComparatorRep> iRep)
:	fRep(iRep)
	{}

ZRef<ComparatorRep> Comparator::GetRep() const
	{ return fRep; }

bool Comparator::Matches(const Value& iLHS, const Value& iRHS) const
	{
	if (fRep)
		return fRep->Matches(iLHS, iRHS);
	// Assume LT, for the sake of having defined behavior.
	return iLHS < iRHS;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep

ComparandRep::ComparandRep()
	{}

ComparandRep::~ComparandRep()
	{}

void ComparandRep::GatherPropNames(set<ZTName>& ioNames)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep_Name

ComparandRep_Name::ComparandRep_Name(const ZTName& iName)
:	fName(iName)
	{}

const Value& ComparandRep_Name::Imp_GetValue(const Tuple& iTuple)
	{ return *iTuple.PGet(fName); }

void ComparandRep_Name::GatherPropNames(set<ZTName>& ioNames)
	{ ioNames.insert(fName); }

const ZTName& ComparandRep_Name::GetName()
	{ return fName; }

// =================================================================================================
#pragma mark -
#pragma mark * ComparandRep_Value

ComparandRep_Value::ComparandRep_Value(const Value& iValue)
:	fValue(iValue)
	{}

const Value& ComparandRep_Value::Imp_GetValue(const Tuple& iTuple)
	{ return fValue; }

const Value& ComparandRep_Value::GetValue()
	{ return fValue; }

// =================================================================================================
#pragma mark -
#pragma mark * Comparand

Comparand::Comparand()
	{}

Comparand::Comparand(const Comparand& iOther)
:	fRep(iOther.fRep)
	{}

Comparand::~Comparand()
	{}

Comparand& Comparand::operator=(const Comparand& iOther)
	{
	fRep = iOther.fRep;
	return *this;
	}

Comparand::Comparand(ZRef<ComparandRep> iRep)
:	fRep(iRep)
	{}

ZRef<ComparandRep> Comparand::GetRep() const
	{ return fRep; }

static Value sEmptyValue;

const Value& Comparand::GetValue(const Tuple& iTuple) const
	{
	if (fRep)
		return fRep->Imp_GetValue(iTuple);

	return sEmptyValue;
	}

void Comparand::GatherPropNames(set<ZTName>& ioNames) const
	{
	if (fRep)
		fRep->GatherPropNames(ioNames);
	}

Condition Comparand::LT(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eLT),
		iRHS.fRep);
	}

Condition Comparand::LE(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eLE),
		iRHS.fRep);
	}

Condition Comparand::EQ(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eEQ),
		iRHS.fRep);
	}

Condition Comparand::GE(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eGE),
		iRHS.fRep);
	}

Condition Comparand::GT(const Comparand& iRHS) const
	{
	return Condition(
		fRep,
		new ComparatorRep_Simple(ComparatorRep_Simple::eGT),
		iRHS.fRep);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Condition

Condition::Condition()
	{}

Condition::Condition(const Condition& iOther)
:	fLHS(iOther.fLHS),
	fComparator(iOther.fComparator),
	fRHS(iOther.fRHS)
	{}

Condition::~Condition()
	{}

Condition& Condition::operator=(const Condition& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

Condition::Condition(
	ZRef<ComparandRep> iLHS, ZRef<ComparatorRep> iComparator, ZRef<ComparandRep> iRHS)
:	fLHS(iLHS),
	fComparator(iComparator),
	fRHS(iRHS)
	{}

Condition::Condition(const Comparand& iLHS, const Comparator& iComparator, const Comparand& iRHS)
:	fLHS(iLHS),
	fComparator(iComparator),
	fRHS(iRHS)
	{}

const Comparand& Condition::GetLHS() const
	{ return fLHS; }

const Comparator& Condition::GetComparator() const
	{ return fComparator; }

const Comparand& Condition::GetRHS() const
	{ return fRHS; }

bool Condition::Matches(const Tuple& iTuple) const
	{ return fComparator.Matches(fLHS.GetValue(iTuple), fRHS.GetValue(iTuple)); }

void Condition::GatherPropNames(set<ZTName>& ioNames) const
	{
	fLHS.GatherPropNames(ioNames);
	fRHS.GatherPropNames(ioNames);
	}

} // namespace ZTQL

NAMESPACE_ZOOLIB_END
