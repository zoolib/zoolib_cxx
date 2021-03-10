// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Intersect.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Intersect

Expr_Rel_Intersect::Expr_Rel_Intersect(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{}

int Expr_Rel_Intersect::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Intersect> other = iOther.DynamicCast<Expr_Rel_Intersect>())
		{
		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;

		return this->GetOp1()->Compare(other->GetOp1());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Intersect::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Intersect* theVisitor = sDynNonConst<Visitor_Expr_Rel_Intersect>(&iVisitor))
		this->Accept_Expr_Rel_Intersect(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Intersect::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Intersect::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Intersect(iOp0, iOp1); }

void Expr_Rel_Intersect::Accept_Expr_Rel_Intersect(Visitor_Expr_Rel_Intersect& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Intersect(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Intersect

void Visitor_Expr_Rel_Intersect::Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Intersect> sIntersect(
	const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{
	if (iLHS && iRHS)
		return new Expr_Rel_Intersect(iLHS, iRHS);
	sSemanticError("sIntersect, LHS and/or RHS are null");
	return null;
	}

ZP<Expr_Rel> operator&(
	const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{ return sIntersect(iLHS, iRHS); }

} // namespace RelationalAlgebra
} // namespace ZooLib
