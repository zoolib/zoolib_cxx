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

#ifndef __ZRA_Expr_Rel_Calc__
#define __ZRA_Expr_Rel_Calc__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZVal_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"

namespace ZooLib {
namespace ZRA {

class Visitor_Expr_Rel_Calc;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Calc

class Expr_Rel_Calc
:	public virtual Expr_Rel
,	public virtual ZExpr_Op1_T<Expr_Rel>
	{
	typedef ZExpr_Op1_T<Expr_Rel> inherited;
public:
	class PseudoMap;
	typedef ZCallable<ZVal_Any(const PseudoMap&)> Callable;

	Expr_Rel_Calc(const ZRef<Expr_Rel>& iOp0,
		const RelName& iRelName,
		const ZRef<Callable>& iCallable);

	virtual ~Expr_Rel_Calc();

// From ZVisitee
	virtual void Accept(ZVisitor& iVisitor);

// From ZExpr_Op1_T<Expr_Rel>
	virtual void Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(const ZRef<Expr_Rel>& iOp0);

// Our protocol
	virtual void Accept_Expr_Rel_Calc(Visitor_Expr_Rel_Calc& iVisitor);

	const RelName& GetRelName() const;
	const ZRef<Callable>& GetCallable() const;

private:
	const RelName fRelName;
	const ZRef<Callable> fCallable;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Calc::PseudoMap

class Expr_Rel_Calc::PseudoMap
	{
public:
	PseudoMap(const std::map<string8,size_t>& iBindings, const ZVal_Any* iVals)
	:	fBindings(iBindings)
	,	fVals(iVals)
		{}

	const ZVal_Any* PGet(const string8& iName) const
		{
		if (ZQ<size_t> theOffsetQ = ZUtil_STL::sQGet(fBindings, iName))
			return fVals + theOffsetQ.Get();
		return nullptr;
		}

	template <class S>
	const S* PGet(const string8& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->PGet<S>();
		return nullptr;
		}

	template <class S>
	ZQ<S> QGet(const string8& iName) const
		{
		if (const ZVal_Any* theVal = this->PGet(iName))
			return theVal->QGet<S>();
		return null;
		}

	template <class S>
	S DGet(const S& iDefault, const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return iDefault;
		}

	template <class S>
	S Get(const string8& iName) const
		{
		if (const S* theVal = this->PGet<S>(iName))
			return *theVal;
		return S();
		}

private:
	const std::map<string8,size_t>& fBindings;
	const ZVal_Any* fVals;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Calc

class Visitor_Expr_Rel_Calc
:	public virtual ZVisitor_Expr_Op1_T<Expr_Rel>
	{
public:
	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sCalc(const ZRef<Expr_Rel>& iOp0,
	const RelName& iRelName,
	const ZRef<Expr_Rel_Calc::Callable>& iCallable);

} // namespace ZRA

template<>
int sCompare_T(const ZRA::Expr_Rel_Calc& iL, const ZRA::Expr_Rel_Calc& iR);

} // namespace ZooLib

#endif // __ZRA_Expr_Rel_Calc__
