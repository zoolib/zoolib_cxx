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

#include "zoolib/zra/ZRA_Expr_Rel_Const.h"

namespace ZooLib {
namespace ZRA {


// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Const

Expr_Rel_Const::Expr_Rel_Const(const ZRef<Expr_Rel>& iOp0,
	const RelName& iRelName, const ZVal_Any& iVal)
:	inherited(iOp0)
,	fRelName(iRelName)
,	fVal(iVal)
	{}

Expr_Rel_Const::~Expr_Rel_Const()
	{}

void Expr_Rel_Const::Accept(ZVisitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = dynamic_cast<Visitor_Expr_Rel_Const*>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Const::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = dynamic_cast<Visitor_Expr_Rel_Const*>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Const::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Const::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Const(iOp0, fRelName, fVal); }

void Expr_Rel_Const::Accept_Expr_Rel_Const(Visitor_Expr_Rel_Const& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Const(this); }

const RelName& Expr_Rel_Const::GetRelName() const
	{ return fRelName; }

const ZVal_Any& Expr_Rel_Const::GetVal() const
	{ return fVal; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Const

void Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Const> sConst(const ZRef<Expr_Rel>& iParent,
	const RelName& iRelName, const ZVal_Any& iVal)
	{
	if (!iParent)
		sSemanticError("sConst, iParent is null");
	return new Expr_Rel_Const(iParent, iRelName, iVal);
	}

} // namespace ZRA
} // namespace ZooLib
