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

#include "zoolib/zra/ZRA_Expr_Rel_Select.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Select

Expr_Rel_Select::Expr_Rel_Select(ZRef<Expr_Rel> iOp0, ZRef<ZExpr_Bool> iExpr_Bool)
:	inherited(iOp0)
,	fExpr_Bool(iExpr_Bool)
	{}

Expr_Rel_Select::~Expr_Rel_Select()
	{}

void Expr_Rel_Select::Accept_Expr_Op1(ZVisitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Select* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Select*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Select(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op1(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Select::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Select::Clone(ZRef<Expr_Rel> iOp0)
	{ return new Expr_Rel_Select(iOp0, fExpr_Bool); }

void Expr_Rel_Select::Accept_Expr_Rel_Select( Visitor_Expr_Rel_Select& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Select(this); }

ZRef<ZExpr_Bool> Expr_Rel_Select::GetExpr_Bool()
	{ return fExpr_Bool; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Select

void Visitor_Expr_Rel_Select::Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr)
	{
	this->Visit_Expr_Op1(iExpr);

	if (ZRef<ZExpr_Bool> theExpr_Bool = iExpr->GetExpr_Bool())
		theExpr_Bool->Accept(*this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Select> sSelect(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool)
	{
	if (iExpr_Rel && iExpr_Bool)
		return new Expr_Rel_Select(iExpr_Rel, iExpr_Bool);
	sSemanticError("sSelect, rel and/or bool are null");
	return null;
	}

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool)
	{ return sSelect(iExpr_Rel, iExpr_Bool); }

ZRef<Expr_Rel> operator&(
	const ZRef<ZExpr_Bool>& iExpr_Bool, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return sSelect(iExpr_Rel, iExpr_Bool); }

ZRef<Expr_Rel>& operator&=(ZRef<Expr_Rel>& ioExpr_Rel, const ZRef<ZExpr_Bool>& iExpr_Bool)
	{ return ioExpr_Rel = ioExpr_Rel & iExpr_Bool; }

} // namespace ZRA
} // namespace ZooLib
