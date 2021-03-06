// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/Log.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/Visitor_Do_T.h"

//###include "zoolib/ZTextCollator.h"

#include "zoolib/Expr/Expr_Bool.h"

#include "zoolib/ValPred/Expr_Bool_ValPred.h"
#include "zoolib/ValPred/ValPred_DB.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Walker_Restrict::Exec

class Walker_Restrict::Exec
	{
public:
	Exec() {};
	virtual ~Exec() {}

	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts) = 0;
	};

// =================================================================================================
#pragma mark - Exec variants (anonymous)

namespace { // anonymous

class Exec_False : public Walker_Restrict::Exec
	{
public:
	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
		{ return false; }
	};

class Exec_True : public Walker_Restrict::Exec
	{
public:
	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
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

	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
		{ return not fExec->Call(iVars, iConsts); }

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

	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
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

	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
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

	virtual bool Call(const Val_DB* iVars, const Val_DB* iConsts)
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
#pragma mark - Functors (anonymous)

namespace { // anonymous

struct Functor_LT
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) < 0; }
	};

struct Functor_LE
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) <= 0; }
	};

struct Functor_EQ
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) == 0; }
	};

struct Functor_NE
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) != 0; }
	};

struct Functor_GE
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) >= 0; }
	};

struct Functor_GT
	{
	bool operator()(const Val_DB& l, const Val_DB& r) const { return sCompare_T(l, r) > 0; }
	};

struct Functor_Callable
	{
	typedef ValComparator_Callable_DB::Callable_t Callable_t;

	Functor_Callable(ZP<Callable_t> iCallable)
	:	fCallable(iCallable)
		{}

	bool operator()(const Val_DB& l, const Val_DB& r) const
		{ return fCallable->Call(l, r); }

	ZP<Callable_t> fCallable;
	};

struct Functor_StringContains
	{
	Functor_StringContains(int iStrength)
	:	fStrength(iStrength)
		{
		ZUnimplemented();
		}

	bool operator()(const Val_DB& l, const Val_DB& r) const
		{
//##		if (const string8* target = l.PGet<string8>())
//##			{
//##			if (const string8* pattern = r.PGet<string8>())
//##				return ZTextCollator(fStrength).Contains(*pattern, *target);
//##			}
		return false;
		}

	int fStrength;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - AsExec (anonymous)

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
	AsExec(const map<string8,size_t>& iVars, vector<Val_DB>& ioConsts)
	:	fVars(iVars)
	,	fConsts(ioConsts)
		{}

// From Visitor_Expr_Bool_XXX
	virtual void Visit_Expr_Bool_True(const ZP<Expr_Bool_True>& iRep)
		{ this->pSetResult(new Exec_True); }

	virtual void Visit_Expr_Bool_False(const ZP<Expr_Bool_False>& iRep)
		{ this->pSetResult(new Exec_False); }

	virtual void Visit_Expr_Bool_Not(const ZP<Expr_Bool_Not>& iRep)
		{ this->pSetResult(new Exec_Not(this->Do(iRep->GetOp0()))); }

	virtual void Visit_Expr_Bool_And(const ZP<Expr_Bool_And>& iRep)
		{ this->pSetResult(new Exec_And(this->Do(iRep->GetOp0()), this->Do(iRep->GetOp1()))); }

	virtual void Visit_Expr_Bool_Or(const ZP<Expr_Bool_Or>& iRep)
		{ this->pSetResult(new Exec_Or(this->Do(iRep->GetOp0()), this->Do(iRep->GetOp1()))); }

// From Visitor_Expr_Bool_ValPred
	virtual void Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr);

// Our protocol
	Walker_Restrict::Exec* pMakeExec(const ValPred& iValPred);

	template <class Functor_p>
	Walker_Restrict::Exec* pMakeExec_T(
		const Functor_p& iFunctor,
		const ZP<ValComparand>& iLHS, const ZP<ValComparand>& iRHS);

	const map<string8,size_t>& fVars;
	vector<Val_DB>& fConsts;
	};

void AsExec::Visit_Expr_Bool_ValPred(const ZP<Expr_Bool_ValPred>& iExpr)
	{ this->pSetResult(this->pMakeExec(iExpr->GetValPred())); }

template <class Functor_p>
Walker_Restrict::Exec* AsExec::pMakeExec_T(
	const Functor_p& iFunctor,
	const ZP<ValComparand>& iLHS, const ZP<ValComparand>& iRHS)
	{
	if (ZP<ValComparand_Name> asNameL =
		iLHS.DynamicCast<ValComparand_Name>())
		{
		const size_t theOffsetL = sGetMust(fVars, asNameL->GetName());
		if (ZP<ValComparand_Name> asNameR =
			iRHS.DynamicCast<ValComparand_Name>())
			{
			const size_t theOffsetR = sGetMust(fVars, asNameR->GetName());
			return new Exec_Functor<Functor_p, true, true>(iFunctor, theOffsetL, theOffsetR);
			}
		else if (ZP<ValComparand_Const_DB> asConstR =
			iRHS.DynamicCast<ValComparand_Const_DB>())
			{
			const size_t theOffsetR = fConsts.size();
			fConsts.push_back(asConstR->GetVal());
			return new Exec_Functor<Functor_p, true, false>(iFunctor, theOffsetL, theOffsetR);
			}
		}
	else if (ZP<ValComparand_Const_DB> asConstL =
		iLHS.DynamicCast<ValComparand_Const_DB>())
		{
		const size_t theOffsetL = fConsts.size();
		fConsts.push_back(asConstL->GetVal());
		if (ZP<ValComparand_Name> asNameR =
			iRHS.DynamicCast<ValComparand_Name>())
			{
			const size_t theOffsetR = sGetMust(fVars, asNameR->GetName());
			return new Exec_Functor<Functor_p, false, true>(iFunctor, theOffsetL, theOffsetR);
			}
		else if (ZP<ValComparand_Const_DB> asConstR =
			iRHS.DynamicCast<ValComparand_Const_DB>())
			{
			const size_t theOffsetR = fConsts.size();
			fConsts.push_back(asConstR->GetVal());
			return new Exec_Functor<Functor_p, false, false>(iFunctor, theOffsetL, theOffsetR);
			}
		}
	ZUnimplemented();
	}

Walker_Restrict::Exec* AsExec::pMakeExec(const ValPred& iValPred)
	{
	const ZP<ValComparator>& theComparator = iValPred.GetComparator();

	if (ZP<ValComparator_Simple> asSimple =
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
	else if (ZP<ValComparator_Callable_DB> asCallable =
		theComparator.DynamicCast<ValComparator_Callable_DB>())
		{
		return pMakeExec_T<>(
			Functor_Callable(asCallable->GetCallable()),
			iValPred.GetLHS(), iValPred.GetRHS());
		}
	else if (ZP<ValComparator_StringContains> asStringContains =
		theComparator.DynamicCast<ValComparator_StringContains>())
		{
		return pMakeExec_T<>(
			Functor_StringContains(asStringContains->GetStrength()),
			iValPred.GetLHS(), iValPred.GetRHS());
		}

	ZUnimplemented();
	}

} // anonymous namespace

// =================================================================================================
#pragma mark - Walker_Restrict

Walker_Restrict::Walker_Restrict(ZP<Walker> iWalker, ZP<Expr_Bool> iExpr_Bool)
:	Walker_Unary(iWalker)
,	fExpr_Bool(iExpr_Bool)
,	fExec(nullptr)
	{}

Walker_Restrict::~Walker_Restrict()
	{ delete fExec; }

ZP<Walker> Walker_Restrict::Prime(
	const map<string8,size_t>& iOffsets,
	map<string8,size_t>& oOffsets,
	size_t& ioBaseOffset)
	{
	delete fExec;
	fExec = nullptr;

	// fCombinedOffsets is our instance variable so that elements in the Exec tree can
	// reference it without incurring any ownership.
	fCombinedOffsets.clear();
	fWalker = fWalker->Prime(iOffsets, fCombinedOffsets, ioBaseOffset);
	if (not fWalker)
		return null;

	oOffsets.insert(fCombinedOffsets.begin(), fCombinedOffsets.end());
	fCombinedOffsets.insert(iOffsets.begin(), iOffsets.end());

	fExec = AsExec(fCombinedOffsets, fConsts).Do(fExpr_Bool);

	return this;
	}

bool Walker_Restrict::QReadInc(Val_DB* ioResults)
	{
	this->Called_QReadInc();

	const Val_DB* theConsts = sFirstOrNil(fConsts);

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
