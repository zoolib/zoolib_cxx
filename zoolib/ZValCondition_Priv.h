/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZValCondition_Priv__
#define __ZValCondition_Priv__ 1

#include "zoolib/ZValCondition_T.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep

template <class Val>
ZValComparatorRep<Val>::ZValComparatorRep()
	{}

template <class Val>
ZValComparatorRep<Val>::~ZValComparatorRep()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_Simple

template <class Val>
ZValComparatorRep_Simple<Val>::ZValComparatorRep_Simple(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

template <class Val>
bool ZValComparatorRep_Simple<Val>::Matches(const Val& iLHS, const Val& iRHS)
	{
	int compare = sCompare_T(iLHS, iRHS);
	switch (fEComparator)
		{
		case eLT: return compare < 0;
		case eLE: return compare <= 0;
		case eEQ: return compare == 0;
		case eGE: return compare >= 0;
		case eGT: return compare > 0;
		}
	ZUnimplemented();
	return false;
	}

template <class Val>
typename ZValComparatorRep_Simple<Val>::EComparator ZValComparatorRep_Simple<Val>::GetEComparator()
	{ return fEComparator; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep

template <class Val>
ZValComparandRep<Val>::ZValComparandRep()
	{}

template <class Val>
ZValComparandRep<Val>::~ZValComparandRep()
	{}

template <class Val>
ZRelHead ZValComparandRep<Val>::GetRelHead()
	{ return ZRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Const

template <class Val>
ZValComparandRep_Const<Val>::ZValComparandRep_Const(const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
Val ZValComparandRep_Const<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{ return fVal; }

template <class Val>
Val ZValComparandRep_Const<Val>::GetVal()
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Trail

template <class Val>
ZValComparandRep_Trail<Val>::ZValComparandRep_Trail(const ZTrail& iTrail)
:	fTrail(iTrail)
	{}

template <class Val>
Val ZValComparandRep_Trail<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{
	Val theVal = iVal;
	for (size_t x = 0, theCount = fTrail.Count(); x < theCount; ++x)
		theVal = theVal.GetMap().Get(fTrail.At(x));

	return theVal;
	}

template <class Val>
ZRelHead ZValComparandRep_Trail<Val>::GetRelHead()
	{
	ZRelHead theRelHead;
	if (fTrail.Count())
		theRelHead |= fTrail.At(0);
	return theRelHead;
	}

template <class Val>
const ZTrail& ZValComparandRep_Trail<Val>::GetTrail()
	{ return fTrail; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Var

template <class Val>
ZValComparandRep_Var<Val>::ZValComparandRep_Var(const std::string& iVarName)
:	fVarName(iVarName)
	{}

template <class Val>
Val ZValComparandRep_Var<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{
	ZUnimplemented();
	return Val();
	}

template <class Val>
std::string ZValComparandRep_Var<Val>::GetVarName()
	{ return fVarName; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_T

template <class Val>
ZValComparand_T<Val>::ZValComparand_T()
	{}

template <class Val>
ZValComparand_T<Val>::ZValComparand_T(const ZValComparand_T& iOther)
:	inherited(iOther)
	{}

template <class Val>
ZValComparand_T<Val>::~ZValComparand_T()
	{}

template <class Val>
ZValComparand_T<Val>& ZValComparand_T<Val>::operator=(const ZValComparand_T<Val>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

template <class Val>
ZValComparand_T<Val>::ZValComparand_T(const ZRef<ZValComparandRep<Val> >& iRep)
:	inherited(iRep)
	{}

template <class Val>
ZValComparand_T<Val>& ZValComparand_T<Val>::operator=(const ZRef<ZValComparandRep<Val> >& iRep)
	{
	inherited::operator=(iRep);
	return *this;
	}

template <class Val>
Val ZValComparand_T<Val>::GetVal(ZValContext& iContext, const Val& iVal) const
	{
	if (*this)
		return (*this)->Imp_GetVal(iContext, iVal);
	return Val();
	}

template <class Val>
ZRelHead ZValComparand_T<Val>::GetRelHead() const
	{
	if (*this)
		return (*this)->GetRelHead();
	return ZRelHead();
	}

template <class Val>
ZValCondition_T<Val> ZValComparand_T<Val>::LT(const ZRef<ZValComparandRep<Val> >& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eLT), iRHS);
	}

template <class Val>
ZValCondition_T<Val> ZValComparand_T<Val>::LE(const ZRef<ZValComparandRep<Val> >& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eLE), iRHS);
	}

template <class Val>
ZValCondition_T<Val> ZValComparand_T<Val>::EQ(const ZRef<ZValComparandRep<Val> >& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eEQ), iRHS);
	}

template <class Val>
ZValCondition_T<Val> ZValComparand_T<Val>::GT(const ZRef<ZValComparandRep<Val> >& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eGT), iRHS);
	}

template <class Val>
ZValCondition_T<Val> ZValComparand_T<Val>::GE(const ZRef<ZValComparandRep<Val> >& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eGE), iRHS);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition_T

template <class Val>
ZValCondition_T<Val>::ZValCondition_T()
	{}

template <class Val>
ZValCondition_T<Val>::ZValCondition_T(const ZValCondition_T<Val>& iOther)
:	fLHS(iOther.fLHS)
,	fComparator(iOther.fComparator)
,	fRHS(iOther.fRHS)
	{}

template <class Val>
ZValCondition_T<Val>::~ZValCondition_T()
	{}

template <class Val>
ZValCondition_T<Val>& ZValCondition_T<Val>::operator=(const ZValCondition_T<Val>& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

template <class Val>
ZValCondition_T<Val>::ZValCondition_T(
	const ZRef<ZValComparandRep<Val> >& iLHS,
	const ZRef<ZValComparatorRep<Val> >& iComparator,
	const ZRef<ZValComparandRep<Val> >& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

template <class Val>
ZValComparand_T<Val> ZValCondition_T<Val>::GetLHS() const
	{ return fLHS; }

template <class Val>
ZRef<ZValComparatorRep<Val> > ZValCondition_T<Val>::GetComparator() const
	{ return fComparator; }

template <class Val>
ZValComparand_T<Val> ZValCondition_T<Val>::GetRHS() const
	{ return fRHS; }

template <class Val>
bool ZValCondition_T<Val>::Matches(ZValContext& iContext, const Val& iVal) const
	{ return fComparator->Matches(fLHS.GetVal(iContext, iVal), fRHS.GetVal(iContext, iVal)); }

template <class Val>
ZRelHead ZValCondition_T<Val>::GetRelHead() const
	{ return fLHS.GetRelHead() | fRHS.GetRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition_T and ZValComparand_T 

template <class Val>
ZValCondition_T<Val> operator<(const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eLT), iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator<=(const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eLE), iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator==(const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eEQ), iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator>=(const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eGE), iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator>(const ZValComparand_T<Val>& iLHS, const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS, new ZValComparatorRep_Simple<Val>(ZValComparatorRep_Simple<Val>::eGT), iRHS);
	}

template <class Val>
ZValComparand_T<Val> CConst_T(const char* iVal)
	{ return ZValComparand_T<Val>(new ZValComparandRep_Const<Val>(std::string(iVal))); }

template <class Val>
ZValComparand_T<Val> CConst_T(const Val& iVal)
	{ return ZValComparand_T<Val>(new ZValComparandRep_Const<Val>(iVal)); }

template <class Val>
ZValComparand_T<Val> CName_T(const std::string& iName)
	{ return ZValComparand_T<Val>(new ZValComparandRep_Trail<Val>(ZTrail(&iName, &iName + 1))); }

template <class Val>
ZValComparand_T<Val> CTrail_T(const ZTrail& iTrail)
	{ return ZValComparand_T<Val>(new ZValComparandRep_Trail<Val>(iTrail)); }

template <class Val>
ZValComparand_T<Val> CVal_T()
	{ return ZValComparand_T<Val>(new ZValComparandRep_Trail<Val>(ZTrail())); }

template <class Val>
ZValComparand_T<Val> CVar_T(const std::string& iVarName)
	{ return ZValComparand_T<Val>(new ZValComparandRep_Var<Val>(iVarName)); }

NAMESPACE_ZOOLIB_END

#endif // __ZValCondition_Priv__
