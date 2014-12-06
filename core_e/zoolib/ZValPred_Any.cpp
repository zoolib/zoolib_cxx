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
#include "zoolib/ZValPred_Any.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZValComparand_Const_Any

ZValComparand_Const_Any::ZValComparand_Const_Any(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

const ZVal_Any& ZValComparand_Const_Any::GetVal() const
	{ return fVal; }

template <>
int sCompare_T(const ZValComparand_Const_Any& iL, const ZValComparand_Const_Any& iR)
	{ return sCompare_T(iL.GetVal(), iR.GetVal()); }

ZMACRO_CompareRegistration_T(ZValComparand_Const_Any)

// =================================================================================================
// MARK: - ZValComparator_Callable_Any

ZValComparator_Callable_Any::ZValComparator_Callable_Any(ZRef<Callable_t> iCallable)
:	fCallable(iCallable)
	{}

const ZRef<ZValComparator_Callable_Any::Callable_t>& ZValComparator_Callable_Any::GetCallable() const
	{ return fCallable; }

template <>
int sCompare_T(const ZValComparator_Callable_Any& iL, const ZValComparator_Callable_Any& iR)
	{ return sCompare_T((void*)iL.GetCallable().Get(), (void*)iR.GetCallable().Get()); }

ZMACRO_CompareRegistration_T(ZValComparator_Callable_Any)

// =================================================================================================
// MARK: - ZValComparator_StringContains

ZValComparator_StringContains::ZValComparator_StringContains(int iStrength)
:	fStrength(iStrength)
	{}

int ZValComparator_StringContains::GetStrength() const
	{ return fStrength; }

template <>
int sCompare_T(const ZValComparator_StringContains& iL, const ZValComparator_StringContains& iR)
	{ return sCompare_T(iL.GetStrength(), iR.GetStrength()); }

ZMACRO_CompareRegistration_T(ZValComparator_StringContains)

// =================================================================================================
// MARK: - Comparand pseudo constructors

ZValComparandPseudo CConst(const ZVal_Any& iVal)
	{ return new ZValComparand_Const_Any(iVal); }

// =================================================================================================
// MARK: -

namespace { // anonymous

ZVal_Any spGetVal(const ZRef<ZValComparand>& iComparand, const ZVal_Any& iVal)
	{
	if (ZRef<ZValComparand_Name> asName =
		iComparand.DynamicCast<ZValComparand_Name>())
		{
		return iVal.Get<ZMap_Any>().Get(asName->GetName());
		}
	else if (ZRef<ZValComparand_Const_Any> asConst =
		iComparand.DynamicCast<ZValComparand_Const_Any>())
		{
		return asConst->GetVal();
		}
	return null;
	}

bool spDoCompare(const ZVal_Any& iL, const ZRef<ZValComparator>& iComparator, const ZVal_Any& iR)
	{
	if (ZRef<ZValComparator_Simple> asSimple =
		iComparator.DynamicCast<ZValComparator_Simple>())
		{
		const int compare = sCompare_T(iL, iR);
		switch (asSimple->GetEComparator())
			{
			case ZValComparator_Simple::eLT: return compare < 0;
			case ZValComparator_Simple::eLE: return compare <= 0;
			case ZValComparator_Simple::eEQ: return compare == 0;
			case ZValComparator_Simple::eNE: return compare != 0;
			case ZValComparator_Simple::eGE: return compare >= 0;
			case ZValComparator_Simple::eGT: return compare > 0;
			}
		ZUnimplemented();
		}
	else if (ZRef<ZValComparator_Callable_Any> asCallable =
		iComparator.DynamicCast<ZValComparator_Callable_Any>())
		{
		return asCallable->GetCallable()->Call(iL, iR);
		}
	else if (ZRef<ZValComparator_StringContains> asStringContains =
		iComparator.DynamicCast<ZValComparator_StringContains>())
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

bool sMatches(const ZValPred& iValPred, const ZVal_Any& iVal)
	{
	const ZVal_Any leftVal = spGetVal(iValPred.GetLHS(), iVal);
	const ZVal_Any rightVal = spGetVal(iValPred.GetRHS(), iVal);
	return spDoCompare(leftVal, iValPred.GetComparator(), rightVal);
	}

} // namespace ZooLib
