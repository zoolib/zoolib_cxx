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

#ifndef __ZValCondition_T__
#define __ZValCondition_T__ 1

#include "zconfig.h"

#include "zoolib/ZRef_Counted.h"
#include "zoolib/ZRelHead.h"
#include "zoolib/ZTrail.h"

NAMESPACE_ZOOLIB_BEGIN

class ZValContext
	{};

template <class T> class ZValComparand_T;

template <class T> class ZValCondition_T;

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_T

template <class Val>
class ZValComparatorRep_T : public ZRefCountedWithFinalize
	{
protected:
	ZValComparatorRep_T();

public:
	virtual ~ZValComparatorRep_T();

	virtual bool Matches(const Val& iLHS, const Val& iRHS) = 0;
	};

template <class Val>
ZValComparatorRep_T<Val>::ZValComparatorRep_T()
	{}

template <class Val>
ZValComparatorRep_T<Val>::~ZValComparatorRep_T()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_Simple_T

template <class Val>
class ZValComparatorRep_Simple_T : public ZValComparatorRep_T<Val>
	{
public:
	enum EComparator
		{
		eLT,
		eLE,
		eEQ,
		eGE,
		eGT
		};

	ZValComparatorRep_Simple_T(EComparator iEComparator);

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

// Our protocol
	EComparator GetEComparator();

private:
	const EComparator fEComparator;
	};

template <class Val>
ZValComparatorRep_Simple_T<Val>::ZValComparatorRep_Simple_T(EComparator iEComparator)
:	fEComparator(iEComparator)
	{}

template <class Val>
bool ZValComparatorRep_Simple_T<Val>::Matches(const Val& iLHS, const Val& iRHS)
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
typename ZValComparatorRep_Simple_T<Val>::EComparator
ZValComparatorRep_Simple_T<Val>::GetEComparator()
	{ return fEComparator; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_StringContains_T

template <class Val>
class ZValComparatorRep_StringContains_T : public ZValComparatorRep_T<Val>
	{
public:
	ZValComparatorRep_StringContains_T(int iStrength);

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);

private:
	const int fStrength;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_SeqContains_T

template <class Val>
class ZValComparatorRep_SeqContains_T : public ZValComparatorRep_T<Val>
	{
public:
	ZValComparatorRep_SeqContains_T();

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparatorRep_Regex_T

template <class Val>
class ZValComparatorRep_Regex_T : public ZValComparatorRep_T<Val>
	{
public:
	ZValComparatorRep_Regex_T();

// From ComparatorRep
	virtual bool Matches(const Val& iLHS, const Val& iRHS);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_T

template <class Val>
class ZValComparandRep_T : public ZRefCountedWithFinalize
	{
protected:
	ZValComparandRep_T();

public:
	virtual ~ZValComparandRep_T();

	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal) = 0;
	virtual ZRelHead GetRelHead();
	};

template <class Val>
ZValComparandRep_T<Val>::ZValComparandRep_T()
	{}

template <class Val>
ZValComparandRep_T<Val>::~ZValComparandRep_T()
	{}

template <class Val>
ZRelHead ZValComparandRep_T<Val>::GetRelHead()
	{ return ZRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Const_T

template <class Val>
class ZValComparandRep_Const_T : public ZValComparandRep_T<Val>
	{
public:
	ZValComparandRep_Const_T(const Val& iVal);

// From ZValComparandRep_T
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	Val GetVal();

private:
	const Val fVal;
	};

template <class Val>
ZValComparandRep_Const_T<Val>::ZValComparandRep_Const_T(const Val& iVal)
:	fVal(iVal)
	{}

template <class Val>
Val ZValComparandRep_Const_T<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{ return fVal; }

template <class Val>
Val ZValComparandRep_Const_T<Val>::GetVal()
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Trail_T

template <class Val>
class ZValComparandRep_Trail_T : public ZValComparandRep_T<Val>
	{
public:
	ZValComparandRep_Trail_T(const ZTrail& iTrail);

// From ZValComparandRep_T
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);
	virtual ZRelHead GetRelHead();

// Our protocol
	const ZTrail& GetTrail();

private:
	const ZTrail fTrail;
	};

template <class Val>
ZValComparandRep_Trail_T<Val>::ZValComparandRep_Trail_T(const ZTrail& iTrail)
:	fTrail(iTrail)
	{}

template <class Val>
Val ZValComparandRep_Trail_T<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{
	Val theVal = iVal;
	for (size_t x = 0, theCount = fTrail.Count(); x < theCount; ++x)
		theVal = theVal.GetMap().Get(fTrail.At(x));

	return theVal;
	}

template <class Val>
ZRelHead ZValComparandRep_Trail_T<Val>::GetRelHead()
	{
	ZRelHead theRelHead;
	if (fTrail.Count())
		theRelHead |= fTrail.At(0);
	return theRelHead;
	}

template <class Val>
const ZTrail& ZValComparandRep_Trail_T<Val>::GetTrail()
	{ return fTrail; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparandRep_Var_T

template <class Val>
class ZValComparandRep_Var_T : public ZValComparandRep_T<Val>
	{
public:
	ZValComparandRep_Var_T(const std::string& iVarName);

// From ZValComparandRep_T
	virtual Val Imp_GetVal(ZValContext& iContext, const Val& iVal);

// Our protocol
	std::string GetVarName();

private:
	const std::string fVarName;
	};

template <class Val>
ZValComparandRep_Var_T<Val>::ZValComparandRep_Var_T(const std::string& iVarName)
:	fVarName(iVarName)
	{}

template <class Val>
Val ZValComparandRep_Var_T<Val>::Imp_GetVal(ZValContext& iContext, const Val& iVal)
	{
	ZUnimplemented();
	return Val();
	}

template <class Val>
std::string ZValComparandRep_Var_T<Val>::GetVarName()
	{ return fVarName; }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_T declaration

template <class Val>
class ZValComparand_T : public ZRef<ZValComparandRep_T<Val> >
	{
	typedef ZRef<ZValComparandRep_T<Val> > inherited;
public:
	typedef ZValComparandRep_T<Val> ComparandRep;

	ZValComparand_T();
	ZValComparand_T(const ZValComparand_T& iOther);
	~ZValComparand_T();
	ZValComparand_T& operator=(const ZValComparand_T& iOther);

	ZValComparand_T(const ZRef<ComparandRep>& iRep);
	ZValComparand_T& operator=(const ZRef<ComparandRep>& iRep);

	Val GetVal(ZValContext& iContext, const Val& iVal) const;
	ZRelHead GetRelHead() const;

	ZValCondition_T<Val> LT(const ZRef<ComparandRep>& iRHS) const;
	ZValCondition_T<Val> LE(const ZRef<ComparandRep>& iRHS) const;
	ZValCondition_T<Val> EQ(const ZRef<ComparandRep>& iRHS) const;
	ZValCondition_T<Val> GE(const ZRef<ComparandRep>& iRHS) const;
	ZValCondition_T<Val> GT(const ZRef<ComparandRep>& iRHS) const;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZValCondition_T

template <class Val>
class ZValCondition_T
	{
public:
	typedef ZValComparandRep_T<Val> ComparandRep;
	typedef ZValComparatorRep_T<Val> ComparatorRep;

	ZValCondition_T();
	ZValCondition_T(const ZValCondition_T& iOther);
	~ZValCondition_T();
	ZValCondition_T& operator=(const ZValCondition_T& iOther);

	ZValCondition_T(
		const ZRef<ComparandRep>& iLHS,
		const ZRef<ComparatorRep>& iComparator,
		const ZRef<ComparandRep>& iRHS);

	ZValComparand_T<Val> GetLHS() const;
	ZRef<ComparatorRep> GetComparator() const;
	ZValComparand_T<Val> GetRHS() const;

	bool Matches(ZValContext& iContext, const Val& iVal) const;

	ZRelHead GetRelHead() const;

private:
	ZValComparand_T<Val> fLHS;
	ZRef<ComparatorRep> fComparator;
	ZValComparand_T<Val> fRHS;
	};

template <class Val>
ZValCondition_T<Val>::ZValCondition_T()
	{}

template <class Val>
ZValCondition_T<Val>::ZValCondition_T(const ZValCondition_T& iOther)
:	fLHS(iOther.fLHS)
,	fComparator(iOther.fComparator)
,	fRHS(iOther.fRHS)
	{}

template <class Val>
ZValCondition_T<Val>::~ZValCondition_T()
	{}

template <class Val>
ZValCondition_T<Val>&
ZValCondition_T<Val>::operator=(const ZValCondition_T& iOther)
	{
	fLHS = iOther.fLHS;
	fComparator = iOther.fComparator;
	fRHS = iOther.fRHS;
	return *this;
	}

template <class Val>
ZValCondition_T<Val>::ZValCondition_T(
	const ZRef<ComparandRep>& iLHS,
	const ZRef<ComparatorRep>& iComparator,
	const ZRef<ComparandRep>& iRHS)
:	fLHS(iLHS)
,	fComparator(iComparator)
,	fRHS(iRHS)
	{}

template <class Val>
ZValComparand_T<Val>
ZValCondition_T<Val>::GetLHS() const
	{ return fLHS; }

template <class Val>
ZRef<typename ZValCondition_T<Val>::ComparatorRep>
ZValCondition_T<Val>::GetComparator() const
	{ return fComparator; }

template <class Val>
ZValComparand_T<Val>
ZValCondition_T<Val>::GetRHS() const
	{ return fRHS; }

template <class Val>
bool ZValCondition_T<Val>::Matches(ZValContext& iContext, const Val& iVal) const
	{ return fComparator->Matches(fLHS.GetVal(iContext, iVal), fRHS.GetVal(iContext, iVal)); }

template <class Val>
ZRelHead ZValCondition_T<Val>::GetRelHead() const
	{ return fLHS.GetRelHead() | fRHS.GetRelHead(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValComparand_T definition

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
ZValComparand_T<Val>&
ZValComparand_T<Val>::operator=(const ZValComparand_T<Val>& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

template <class Val>
ZValComparand_T<Val>::ZValComparand_T(const ZRef<ComparandRep>& iRep)
:	inherited(iRep)
	{}

template <class Val>
ZValComparand_T<Val>&
ZValComparand_T<Val>::operator=(const ZRef<ComparandRep>& iRep)
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
ZValCondition_T<Val>
ZValComparand_T<Val>::LT(const ZRef<ComparandRep>& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eLT),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val>
ZValComparand_T<Val>::LE(const ZRef<ComparandRep>& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eLE),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val>
ZValComparand_T<Val>::EQ(const ZRef<ComparandRep>& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eEQ),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val>
ZValComparand_T<Val>::GE(const ZRef<ComparandRep>& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eGE),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val>
ZValComparand_T<Val>::GT(const ZRef<ComparandRep>& iRHS) const
	{
	return ZValCondition_T<Val>
		(*this,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eGT),
		iRHS);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Comparison operators taking comparands and returning a condition

template <class Val>
ZValCondition_T<Val> operator<(
	const ZValComparand_T<Val>& iLHS,
	const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eLT),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator<=(
	const ZValComparand_T<Val>& iLHS,
	const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eLE),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator==(
	const ZValComparand_T<Val>& iLHS,
	const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eEQ),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator>=(
	const ZValComparand_T<Val>& iLHS,
	const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eGE),
		iRHS);
	}

template <class Val>
ZValCondition_T<Val> operator>(
	const ZValComparand_T<Val>& iLHS,
	const ZValComparand_T<Val>& iRHS)
	{
	return ZValCondition_T<Val>
		(iLHS,
		new ZValComparatorRep_Simple_T<Val>(
			ZValComparatorRep_Simple_T<Val>::eGT),
		iRHS);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Comparand constructors

template <class Val>
ZValComparand_T<Val>
CConst_T(const char* iVal)
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Const_T<Val>(std::string(iVal)));
	}

template <class Val>
ZValComparand_T<Val>
CConst_T(const Val& iVal)
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Const_T<Val>(iVal));
	}

template <class Val>
ZValComparand_T<Val>
CName_T(const std::string& iName)
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Trail_T<Val>(ZTrail(&iName, &iName + 1)));
	}

template <class Val>
ZValComparand_T<Val>
CTrail_T(const ZTrail& iTrail)
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Trail_T<Val>(iTrail));
	}

template <class Val>
ZValComparand_T<Val>
CVal_T()
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Trail_T<Val>(ZTrail()));
	}

template <class Val>
ZValComparand_T<Val>
CVar_T(const std::string& iVarName)
	{
	return ZValComparand_T<Val>(
		new ZValComparandRep_Var_T<Val>(iVarName));
	}

NAMESPACE_ZOOLIB_END

#endif // __ZValCondition_T__
