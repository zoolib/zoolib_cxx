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

#include "zoolib/Log.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/Visitor_Do_T.h"

//###include "zoolib/ZTextCollator.h"

#include "zoolib/Expr/Expr_Bool.h"

#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_Any.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark -
#pragma mark Walker_Restrict::Exec

class Walker_Restrict::Exec
	{
public:
	Exec() {};
	virtual ~Exec() {}

	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark Exec variants (anonymous)

namespace { // anonymous

class Exec_False : public Walker_Restrict::Exec
	{
public:
	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{ return false; }
	};

class Exec_True : public Walker_Restrict::Exec
	{
public:
	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{ return true; }
	};

class Exec_Not : public Walker_Restrict::Exec
	{
public:
	Exec_Not(Exec* iExec)
	:	fExec(iExec)
		{}

	virtual ~Exec_Not()
		{ delete fExec; }

	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{ return !fExec->Call(iVars, iConsts); }

	Exec* fExec;
	};

class Exec_And : public Walker_Restrict::Exec
	{
public:
	Exec_And(Exec* iLeft, Exec* iRight)
	:	fLeft(iLeft)
	,	fRight(iRight)
		{}

	virtual ~Exec_And()
		{
		delete fLeft;
		delete fRight;
		}

	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{ return fLeft->Call(iVars, iConsts) && fRight->Call(iVars, iConsts); }

	Exec* fLeft;
	Exec* fRight;
	};

class Exec_Or : public Walker_Restrict::Exec
	{
public:
	Exec_Or(Exec* iLeft, Exec* iRight)
	:	fLeft(iLeft)
	,	fRight(iRight)
		{}

	virtual ~Exec_Or()
		{
		delete fLeft;
		delete fRight;
		}

	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{ return fLeft->Call(iVars, iConsts) || fRight->Call(iVars, iConsts); }

	Exec* fLeft;
	Exec* fRight;
	};

template <class Functor_p, bool UseLeftVar_p, bool UseRightVar_p>
class Exec_Functor : public Walker_Restrict::Exec
	{
public:
	Exec_Functor(const Functor_p& iFunctor, size_t iOffsetLeft, size_t iOffsetRight)
	:	fFunctor(iFunctor)
	,	fOffsetLeft(iOffsetLeft)
	,	fOffsetRight(iOffsetRight)
		{}

	virtual bool Call(const Val_Any* iVars, const Val_Any* iConsts)
		{
		if (UseLeftVar_p)
			{
			if (UseRightVar_p)
				return fFunctor(iVars[fOffsetLeft], iVars[fOffsetRight]);
			else
				return fFunctor(iVars[fOffsetLeft], iConsts[fOffsetRight]);
			}
		else
			{
			if (UseRightVar_p)
				return fFunctor(iConsts[fOffsetLeft], iVars[fOffsetRight]);
			else
				return fFunctor(iConsts[fOffsetLeft], iConsts[fOffsetRight]);
			}
		}

	const Functor_p fFunctor;
	const size_t fOffsetLeft;
	const size_t fOffsetRight;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Functors (anonymous)

namespace { // anonymous

struct Functor_LT
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) < 0; }
	};

struct Functor_LE
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) <= 0; }
	};

struct Functor_EQ
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) == 0; }
	};

struct Functor_NE
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) != 0; }
	};

struct Functor_GE
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) >= 0; }
	};

struct Functor_GT
	{
	bool operator()(const Val_Any& l, const Val_Any& r) const { return sCompare_T(l, r) > 0; }
	};

struct Functor_Callable
	{
	typedef ValComparator_Callable_Any::Callable_t Callable_t;

	Functor_Callable(ZRef<Callable_t> iCallable)
	:	fCallable(iCallable)
		{}

	bool operator()(const Val_Any& l, const Val_Any& r) const
		{ return fCallable->Call(l, r); }

	ZRef<Callable_t> fCallable;
	};

struct Functor_StringContains
	{
	Functor_StringContains(int iStrength)
	:	fStrength(iStrength)
		{
		ZUnimplemented();
		}

	bool operator()(const Val_Any& l, const Val_Any& r) const
		{
		if (const string8* target = l.PGet<string8>())
			{
//##			if (const string8* pattern = r.PGet<string8>())
//##				return ZTextCollator(fStrength).Contains(*pattern, *target);
			}
		return false;
		}

	int fStrength;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark AsExec (anonymous)

namespace { // anonymous

class AsExec
:	public virtual Visitor_Do_T<Walker_Restrict::Exec*>
,	public virtual Visitor_Expr_Bool_True
,	public virtual Visitor_Expr_Bool_False
,	public virtual Visitor_Expr_Bool_Not
,	public virtual Visitor_Expr_Bool_And
,	public virtual Visitor_Expr_Bool_Or
,	public virtual Visitor_Expr_Bool_ValPred
	{
public:
	AsExec(const map<string8,size_t>& iVars, vector<Val_Any>& ioConsts)
	:	fVars(iVars)
	,	fConsts(ioConsts)
		{}

// From Visitor_Expr_Bool_XXX
	virtual void Visit_Expr_Bool_True(const ZRef<Expr_Bool_True>& iRep)
		{ this->pSetResult(new Exec_True); }

	virtual void Visit_Expr_Bool_False(const ZRef<Expr_Bool_False>& iRep)
		{ this->pSetResult(new Exec_False); }

	virtual void Visit_Expr_Bool_Not(const ZRef<Expr_Bool_Not>& iRep)
		{ this->pSetResult(new Exec_Not(this->Do(iRep->GetOp0()))); }

	virtual void Visit_Expr_Bool_And(const ZRef<Expr_Bool_And>& iRep)
		{ this->pSetResult(new Exec_And(this->Do(iRep->GetOp0()), this->Do(iRep->GetOp1()))); }

	virtual void Visit_Expr_Bool_Or(const ZRef<Expr_Bool_Or>& iRep)
		{ this->pSetResult(new Exec_Or(this->Do(iRep->GetOp0()), this->Do(iRep->GetOp1()))); }

// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred>& iExpr);

// Our protocol
	Walker_Restrict::Exec* pMakeExec(const ValPred& iValPred);

	template <class Functor_p>
	Walker_Restrict::Exec* pMakeExec_T(
		const Functor_p& iFunctor,
		const ZRef<ValComparand>& iLHS, const ZRef<ValComparand>& iRHS);

	const map<string8,size_t>& fVars;
	vector<Val_Any>& fConsts;
	};

void AsExec::Visit_Expr_Bool_ValPred(const ZRef<Expr_Bool_ValPred>& iExpr)
	{ this->pSetResult(this->pMakeExec(iExpr->GetValPred())); }

template <class Functor_p>
Walker_Restrict::Exec* AsExec::pMakeExec_T(
	const Functor_p& iFunctor,
	const ZRef<ValComparand>& iLHS, const ZRef<ValComparand>& iRHS)
	{
	if (ZRef<ValComparand_Name> asNameL =
		iLHS.DynamicCast<ValComparand_Name>())
		{
		const size_t theOffsetL = sGetMust(fVars, asNameL->GetName());
		if (ZRef<ValComparand_Name> asNameR =
			iRHS.DynamicCast<ValComparand_Name>())
			{
			const size_t theOffsetR = sGetMust(fVars, asNameR->GetName());
			return new Exec_Functor<Functor_p, true, true>(iFunctor, theOffsetL, theOffsetR);
			}
		else if (ZRef<ValComparand_Const_Any> asConstR =
			iRHS.DynamicCast<ValComparand_Const_Any>())
			{
			const size_t theOffsetR = fConsts.size();
			fConsts.push_back(asConstR->GetVal());
			return new Exec_Functor<Functor_p, true, false>(iFunctor, theOffsetL, theOffsetR);
			}
		else
			{
			ZUnimplemented();
			}
		}
	else if (ZRef<ValComparand_Const_Any> asConstL =
		iLHS.DynamicCast<ValComparand_Const_Any>())
		{
		const size_t theOffsetL = fConsts.size();
		fConsts.push_back(asConstL->GetVal());
		if (ZRef<ValComparand_Name> asNameR =
			iRHS.DynamicCast<ValComparand_Name>())
			{
			const size_t theOffsetR = sGetMust(fVars, asNameR->GetName());
			return new Exec_Functor<Functor_p, false, true>(iFunctor, theOffsetL, theOffsetR);
			}
		else if (ZRef<ValComparand_Const_Any> asConstR =
			iRHS.DynamicCast<ValComparand_Const_Any>())
			{
			const size_t theOffsetR = fConsts.size();
			fConsts.push_back(asConstR->GetVal());
			return new Exec_Functor<Functor_p, false, false>(iFunctor, theOffsetL, theOffsetR);
			}
		}
	else
		{
		ZUnimplemented();
		}
	return nullptr;
	}

Walker_Restrict::Exec* AsExec::pMakeExec(const ValPred& iValPred)
	{
	const ZRef<ValComparator>& theComparator = iValPred.GetComparator();

	if (ZRef<ValComparator_Simple> asSimple =
		theComparator.DynamicCast<ValComparator_Simple>())
		{
		switch (asSimple->GetEComparator())
			{
			case ValComparator_Simple::eLT:
				{
				return pMakeExec_T<>(
					Functor_LT(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			case ValComparator_Simple::eLE:
				{
				return pMakeExec_T<>(
					Functor_LE(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			case ValComparator_Simple::eEQ:
				{
				return pMakeExec_T<>(
					Functor_EQ(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			case ValComparator_Simple::eNE:
				{
				return pMakeExec_T<>(
					Functor_NE(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			case ValComparator_Simple::eGE:
				{
				return pMakeExec_T<>(
					Functor_GE(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			case ValComparator_Simple::eGT:
				{
				return pMakeExec_T<>(
					Functor_GT(),
					iValPred.GetLHS(), iValPred.GetRHS());
				}
			}
		}
	else if (ZRef<ValComparator_Callable_Any> asCallable =
		theComparator.DynamicCast<ValComparator_Callable_Any>())
		{
		return pMakeExec_T<>(
			Functor_Callable(asCallable->GetCallable()),
			iValPred.GetLHS(), iValPred.GetRHS());
		}
	else if (ZRef<ValComparator_StringContains> asStringContains =
		theComparator.DynamicCast<ValComparator_StringContains>())
		{
		return pMakeExec_T<>(
			Functor_StringContains(asStringContains->GetStrength()),
			iValPred.GetLHS(), iValPred.GetRHS());
		}

	ZUnimplemented();
	return nullptr;
	}

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark Walker_Restrict

Walker_Restrict::Walker_Restrict(ZRef<Walker> iWalker, ZRef<Expr_Bool> iExpr_Bool)
:	Walker_Unary(iWalker)
,	fExpr_Bool(iExpr_Bool)
,	fExec(nullptr)
	{}

Walker_Restrict::~Walker_Restrict()
	{ delete fExec; }

ZRef<Walker> Walker_Restrict::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	fWalker = fWalker->Prime(iOffsets, fChildOffsets, ioBaseOffset);
	if (not fWalker)
		return null;

	oOffsets.insert(fChildOffsets.begin(), fChildOffsets.end());
	fChildOffsets.insert(iOffsets.begin(), iOffsets.end());

	fExec = AsExec(fChildOffsets, fConsts).Do(fExpr_Bool);

	return this;
	}

bool Walker_Restrict::QReadInc(Val_Any* ioResults)
	{
	const Val_Any* theConsts = sFirstOrNil(fConsts);

	for (;;)
		{
		if (not fWalker->QReadInc(ioResults))
			return false;

		if (fExec->Call(ioResults, theConsts))
			return true;
		}
	}

} // namespace QueryEngine
} // namespace ZooLib
