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

#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Calc

Expr_Rel_Calc::Expr_Rel_Calc(const ZRef<Expr_Rel>& iOp0,
	const RelName& iRelName, const ZRef<Callable>& iCallable)
:	inherited(iOp0)
,	fRelName(iRelName)
,	fCallable(iCallable)
	{}

Expr_Rel_Calc::~Expr_Rel_Calc()
	{}

void Expr_Rel_Calc::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Calc*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Calc(*theVisitor);
		}
	else
		{
		inherited::Accept(iVisitor);
		}
	}

void Expr_Rel_Calc::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Calc*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Calc(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Calc::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Calc::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Calc(iOp0, fRelName, fCallable); }

void Expr_Rel_Calc::Accept_Expr_Rel_Calc(Visitor_Expr_Rel_Calc& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Calc(this); }

RelName Expr_Rel_Calc::GetRelName()
	{ return fRelName; }

ZRef<Expr_Rel_Calc::Callable> Expr_Rel_Calc::GetCallable()
	{ return fCallable; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Calc

void Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Calc> sCalc(const ZRef<Expr_Rel>& iParent,
	const RelName& iRelName, const ZRef<Expr_Rel_Calc::Callable>& iCallable)
	{
	if (!iParent)
		sSemanticError("sCalc, iParent is null");
	return new Expr_Rel_Calc(iParent, iRelName, iCallable);
	}

} // namespace ZRA
} // namespace ZooLib
