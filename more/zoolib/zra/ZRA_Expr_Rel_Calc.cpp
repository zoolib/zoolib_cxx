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

#include "zoolib/ZCompare_Ref.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Calc& iL, const ZRA::Expr_Rel_Calc& iR)
	{
	if (int compare = sCompare_T(iL.GetRelName(), iR.GetRelName()))
		return compare;

	if (int compare = sCompare_T(iL.GetOp0(), iR.GetOp0()))
		return compare;

	if (iL.GetCallable() < iR.GetCallable())
		return -1;

	if (iR.GetCallable() < iL.GetCallable())
		return 1;

	return 0;
	}

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Calc)

namespace ZRA {

using std::map;

// =================================================================================================
// MARK: - Expr_Rel_Calc

Expr_Rel_Calc::Expr_Rel_Calc(const ZRef<Expr_Rel>& iOp0,
	const RelName& iRelName,
	const ZRef<Callable>& iCallable)
:	inherited(iOp0)
,	fRelName(iRelName)
,	fCallable(iCallable)
	{}

Expr_Rel_Calc::~Expr_Rel_Calc()
	{}

void Expr_Rel_Calc::Accept(const ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor = DynNonConst<Visitor_Expr_Rel_Calc>(&iVisitor))
		this->Accept_Expr_Rel_Calc(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Calc::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor = DynNonConst<Visitor_Expr_Rel_Calc>(&iVisitor))
		this->Accept_Expr_Rel_Calc(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Calc::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Calc::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Calc(iOp0, fRelName, fCallable); }

void Expr_Rel_Calc::Accept_Expr_Rel_Calc(Visitor_Expr_Rel_Calc& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Calc(this); }

const RelName& Expr_Rel_Calc::GetRelName() const
	{ return fRelName; }

const ZRef<Expr_Rel_Calc::Callable>& Expr_Rel_Calc::GetCallable() const
	{ return fCallable; }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Calc

void Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel> sCalc(const ZRef<Expr_Rel>& iOp0,
	const RelName& iRelName,
	const ZRef<Expr_Rel_Calc::Callable>& iCallable)
	{
	return new Expr_Rel_Calc(iOp0, iRelName, iCallable);
	}

} // namespace ZRA
} // namespace ZooLib
