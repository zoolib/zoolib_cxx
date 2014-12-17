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

#include "zoolib/Compare.h"
#include "zoolib/Compare_Integer.h"

#include "zoolib/ZTextCollator.h"

#include "zoolib/ValPred/ValPred_Any.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ValComparand_Const_Any

ValComparand_Const_Any::ValComparand_Const_Any(const Val_Any& iVal)
:	fVal(iVal)
	{}

const Val_Any& ValComparand_Const_Any::GetVal() const
	{ return fVal; }

template <>
int sCompare_T(const ValComparand_Const_Any& iL, const ValComparand_Const_Any& iR)
	{ return sCompare_T(iL.GetVal(), iR.GetVal()); }

ZMACRO_CompareRegistration_T(ValComparand_Const_Any)

// =================================================================================================
// MARK: - ValComparator_Callable_Any

ValComparator_Callable_Any::ValComparator_Callable_Any(ZRef<Callable_t> iCallable)
:	fCallable(iCallable)
	{}

const ZRef<ValComparator_Callable_Any::Callable_t>& ValComparator_Callable_Any::GetCallable() const
	{ return fCallable; }

template <>
int sCompare_T(const ValComparator_Callable_Any& iL, const ValComparator_Callable_Any& iR)
	{ return sCompare_T((void*)iL.GetCallable().Get(), (void*)iR.GetCallable().Get()); }

ZMACRO_CompareRegistration_T(ValComparator_Callable_Any)

// =================================================================================================
// MARK: - ValComparator_StringContains

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
// MARK: - Comparand pseudo constructors

ValComparandPseudo CConst(const Val_Any& iVal)
	{ return new ValComparand_Const_Any(iVal); }

// =================================================================================================
// MARK: -

namespace { // anonymous

Val_Any spGetVal(const ZRef<ValComparand>& iComparand, const Val_Any& iVal)
	{
	if (ZRef<ValComparand_Name> asName =
		iComparand.DynamicCast<ValComparand_Name>())
		{
		return iVal.Get<ZMap_Any>().Get(asName->GetName());
		}
	else if (ZRef<ValComparand_Const_Any> asConst =
		iComparand.DynamicCast<ValComparand_Const_Any>())
		{
		return asConst->GetVal();
		}
	return null;
	}

bool spDoCompare(const Val_Any& iL, const ZRef<ValComparator>& iComparator, const Val_Any& iR)
	{
	if (ZRef<ValComparator_Simple> asSimple =
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
		ZUnimplemented();
		}
	else if (ZRef<ValComparator_Callable_Any> asCallable =
		iComparator.DynamicCast<ValComparator_Callable_Any>())
		{
		return asCallable->GetCallable()->Call(iL, iR);
		}
	else if (ZRef<ValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ValComparator_StringContains>())
		{
		if (const string8* target = iL.PGet<string8>())
			{
			if (const string8* pattern = iR.PGet<string8>())
				return ZTextCollator(asStringContains->GetStrength()).Contains(*pattern, *target);
			}
		return false;
		}
	ZUnimplemented();
	return true;
	}

} // anonymous namespace

bool sMatches(const ValPred& iValPred, const Val_Any& iVal)
	{
	const Val_Any leftVal = spGetVal(iValPred.GetLHS(), iVal);
	const Val_Any rightVal = spGetVal(iValPred.GetRHS(), iVal);
	return spDoCompare(leftVal, iValPred.GetComparator(), rightVal);
	}

} // namespace ZooLib
