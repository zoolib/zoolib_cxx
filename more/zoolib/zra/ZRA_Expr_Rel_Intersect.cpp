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
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCompare_T

template <>
int sCompare_T(const ZRA::Expr_Rel_Intersect& iL, const ZRA::Expr_Rel_Intersect& iR)
	{
	if (int compare = sCompare_T(iL.GetOp0(), iR.GetOp0()))
		return compare;

	return sCompare_T(iL.GetOp1(), iR.GetOp1());
	}

ZMACRO_CompareRegistration_T(ZRA::Expr_Rel_Intersect)

namespace ZRA {

// =================================================================================================
// MARK: - Expr_Rel_Intersect

Expr_Rel_Intersect::Expr_Rel_Intersect(const ZRef<Expr_Rel>& iOp0, const ZRef<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Rel_Intersect::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Intersect* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Intersect*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Intersect(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Intersect::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Intersect::Clone(const ZRef<Expr_Rel>& iOp0, const ZRef<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Intersect(iOp0, iOp1); }

void Expr_Rel_Intersect::Accept_Expr_Rel_Intersect(Visitor_Expr_Rel_Intersect& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Intersect(this); }

// =================================================================================================
// MARK: - Visitor_Expr_Rel_Intersect

void Visitor_Expr_Rel_Intersect::Visit_Expr_Rel_Intersect(const ZRef<Expr_Rel_Intersect>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
// MARK: - Relational operators

ZRef<Expr_Rel_Intersect> sIntersect
	(const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{
	if (iLHS && iRHS)
		return new Expr_Rel_Intersect(iLHS, iRHS);
	sSemanticError("sIntersect, LHS and/or RHS are null");
	return null;
	}

ZRef<Expr_Rel> operator&
	(const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return sIntersect(iLHS, iRHS); }

} // namespace ZRA
} // namespace ZooLib
