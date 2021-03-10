// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Difference.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompareNew_T

template <>
int sCompareNew_T(const RelationalAlgebra::Expr_Rel_Difference& iL,
	const RelationalAlgebra::Expr_Rel_Difference& iR)
	{
	if (int compare = sCompareNew_T(iL.GetOp0(), iR.GetOp0()))
		return compare;

	return sCompareNew_T(iL.GetOp1(), iR.GetOp1());
	}

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Difference

Expr_Rel_Difference::Expr_Rel_Difference(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Rel_Difference::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Difference* theVisitor =
		sDynNonConst<Visitor_Expr_Rel_Difference>(&iVisitor))
		{ this->Accept_Expr_Rel_Difference(*theVisitor); }
	else
		{ inherited::Accept_Expr_Op2(iVisitor); }
	}

ZP<Expr_Rel> Expr_Rel_Difference::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Difference::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Difference(iOp0, iOp1); }

void Expr_Rel_Difference::Accept_Expr_Rel_Difference(Visitor_Expr_Rel_Difference& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Difference(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Difference

void Visitor_Expr_Rel_Difference::Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

} // namespace RelationalAlgebra
} // namespace ZooLib
