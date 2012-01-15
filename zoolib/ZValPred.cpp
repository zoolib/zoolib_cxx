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

#include "zoolib/ZCompare_Integer.h"
#include "zoolib/ZCompare_Ref.h"
#include "zoolib/ZCompare_String.h"
#include "zoolib/ZValPred.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZValComparator

ZValComparator::ZValComparator()
	{}

ZValComparator::~ZValComparator()
	{}

// =================================================================================================
// MARK: - ZValComparator_Simple

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
// MARK: - ZValComparand

ZValComparand::ZValComparand()
	{}

ZValComparand::~ZValComparand()
	{}

// =================================================================================================
// MARK: - ZValComparand_Name

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
// MARK: - ZValPred

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

ZValPred::ZValPred
	(const ZRef<ZValComparand>& iLHS,
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
// MARK: - ZValPred, sCompare_T

template<>
int sCompare_T(const ZValPred& iL, const ZValPred& iR)
	{
	if (int compare = sCompare_T(iL.GetLHS(), iR.GetLHS()))
		return compare;

	if (int compare = sCompare_T(iL.GetComparator(), iR.GetComparator()))
		return compare;

	return sCompare_T(iL.GetRHS(), iR.GetRHS());
	}

bool operator<(const ZValPred& iL, const ZValPred& iR)
	{ return sCompare_T(iL, iR) < 0; }

// =================================================================================================
// MARK: - ZValComparandPseudo

ZValComparandPseudo::ZValComparandPseudo(ZValComparand* iRep)
:	ZRef<ZValComparand>(iRep)
	{}

// =================================================================================================
// MARK: - Comparand pseudo constructors

ZValComparandPseudo CName(const std::string& iName)
	{ return new ZValComparand_Name(iName); }

// =================================================================================================
// MARK: - Comparison operators taking comparands and returning a condition

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

