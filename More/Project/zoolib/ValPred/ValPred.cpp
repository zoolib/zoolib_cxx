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

#include "zoolib/Compare_Integer.h"
#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"

#include "zoolib/ValPred/ValPred.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ValComparator

ValComparator::ValComparator()
	{}

ValComparator::~ValComparator()
	{}

// =================================================================================================
#pragma mark -
#pragma mark ValComparator_Simple

ValComparator_Simple::ValComparator_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

ValComparator_Simple::EComparator ValComparator_Simple::GetEComparator() const
	{ return fEComparator; }

template <>
int sCompare_T(const ValComparator_Simple& iL, const ValComparator_Simple& iR)
	{ return sCompare_T(int(iL.GetEComparator()), int(iR.GetEComparator())); }

ZMACRO_CompareRegistration_T(ValComparator_Simple)

// =================================================================================================
#pragma mark -
#pragma mark ValComparand

ValComparand::ValComparand()
	{}

ValComparand::~ValComparand()
	{}

// =================================================================================================
#pragma mark -
#pragma mark ValComparand_Name

ValComparand_Name::ValComparand_Name(const std::string& iName)
:	fName(iName)
	{}

const std::string& ValComparand_Name::GetName() const
	{ return fName; }

template <>
int sCompare_T(const ValComparand_Name& iL, const ValComparand_Name& iR)
	{ return sCompare_T(iL.GetName(), iR.GetName()); }

ZMACRO_CompareRegistration_T(ValComparand_Name)

// =================================================================================================
#pragma mark -
#pragma mark ValPred

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
	const ZRef<ValComparand>& iLHS,
	const ZRef<ValComparator>& iComparator,
	const ZRef<ValComparand>& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

const ZRef<ValComparand>& ValPred::GetLHS() const
	{ return fLHS; }

const ZRef<ValComparator>& ValPred::GetComparator() const
	{ return fComparator; }

const ZRef<ValComparand>& ValPred::GetRHS() const
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark ValPred, sCompare_T

template <>
int sCompare_T(const ValPred& iL, const ValPred& iR)
	{
	if (int compare = sCompare_T(iL.GetLHS(), iR.GetLHS()))
		return compare;

	if (int compare = sCompare_T(iL.GetComparator(), iR.GetComparator()))
		return compare;

	return sCompare_T(iL.GetRHS(), iR.GetRHS());
	}

bool operator<(const ValPred& iL, const ValPred& iR)
	{ return sCompare_T(iL, iR) < 0; }

// =================================================================================================
#pragma mark -
#pragma mark Comparand pseudo constructors

ValComparandPseudo CName(const std::string& iName)
	{ return new ValComparand_Name(iName); }

} // namespace ZooLib

