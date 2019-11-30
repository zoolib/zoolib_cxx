// Copyright (c) 2010-2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ValPred/ValPred_DB.h"

#include "zoolib/Compare.h"
#include "zoolib/Compare_Integer.h"

//###include "zoolib/ZTextCollator.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ValComparand_Const_DB

ValComparand_Const_DB::ValComparand_Const_DB(const Val_DB& iVal)
:	fVal(iVal)
	{}

const Val_DB& ValComparand_Const_DB::GetVal() const
	{ return fVal; }

template <>
int sCompare_T(const ValComparand_Const_DB& iL, const ValComparand_Const_DB& iR)
	{ return sCompare_T(iL.GetVal(), iR.GetVal()); }

ZMACRO_CompareRegistration_T(ValComparand_Const_DB)

// =================================================================================================
#pragma mark - ValComparator_Callable_DB

ValComparator_Callable_DB::ValComparator_Callable_DB(ZP<Callable_t> iCallable)
:	fCallable(iCallable)
	{}

const ZP<ValComparator_Callable_DB::Callable_t>& ValComparator_Callable_DB::GetCallable() const
	{ return fCallable; }

template <>
int sCompare_T(const ValComparator_Callable_DB& iL, const ValComparator_Callable_DB& iR)
	{ return sCompare_T((void*)iL.GetCallable().Get(), (void*)iR.GetCallable().Get()); }

ZMACRO_CompareRegistration_T(ValComparator_Callable_DB)

// =================================================================================================
#pragma mark - ValComparator_StringContains

ValComparator_StringContains::ValComparator_StringContains(int iStrength)
:	fStrength(iStrength)
	{}

int ValComparator_StringContains::GetStrength() const
	{ return fStrength; }

template <>
int sCompare_T(const ValComparator_StringContains& iL, const ValComparator_StringContains& iR)
	{ return sCompare_T(iL.GetStrength(), iR.GetStrength()); }

ZMACRO_CompareRegistration_T(ValComparator_StringContains)

// =================================================================================================
#pragma mark - Comparand pseudo constructors

ValComparandPseudo CConst(const Val_DB& iVal)
	{ return new ValComparand_Const_DB(iVal); }

// =================================================================================================
#pragma mark -

namespace { // anonymous

Val_DB spGetVal(const ZP<ValComparand>& iComparand, const Val_DB& iVal)
	{
	if (ZP<ValComparand_Name> asName =
		iComparand.DynamicCast<ValComparand_Name>())
		{
		return iVal.Get<Map_ZZ>().Get(asName->GetName());
		}
	else if (ZP<ValComparand_Const_DB> asConst =
		iComparand.DynamicCast<ValComparand_Const_DB>())
		{
		return asConst->GetVal();
		}
	return null;
	}

bool spDoCompare(const Val_DB& iL, const ZP<ValComparator>& iComparator, const Val_DB& iR)
	{
	if (ZP<ValComparator_Simple> asSimple =
		iComparator.DynamicCast<ValComparator_Simple>())
		{
		const int compare = sCompare_T(iL, iR);
		switch (asSimple->GetEComparator())
			{
			case ValComparator_Simple::eLT: return compare < 0;
			case ValComparator_Simple::eLE: return compare <= 0;
			case ValComparator_Simple::eEQ: return compare == 0;
			case ValComparator_Simple::eNE: return compare != 0;
			case ValComparator_Simple::eGE: return compare >= 0;
			case ValComparator_Simple::eGT: return compare > 0;
			}
		}
	else if (ZP<ValComparator_Callable_DB> asCallable =
		iComparator.DynamicCast<ValComparator_Callable_DB>())
		{
		return asCallable->GetCallable()->Call(iL, iR);
		}
	else if (ZP<ValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ValComparator_StringContains>())
		{
//##		if (const string8* target = iL.PGet<string8>())
//			{
//			if (const string8* pattern = iR.PGet<string8>())
//				return ZTextCollator(asStringContains->GetStrength()).Contains(*pattern, *target);
//			}
		}
	ZUnimplemented();
	}

} // anonymous namespace

bool sMatches(const ValPred& iValPred, const Val_DB& iVal)
	{
	const Val_DB leftVal = spGetVal(iValPred.GetLHS(), iVal);
	const Val_DB rightVal = spGetVal(iValPred.GetRHS(), iVal);
	return spDoCompare(leftVal, iValPred.GetComparator(), rightVal);
	}

} // namespace ZooLib
