// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Product

Expr_Rel_Product::Expr_Rel_Product(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Rel_Product::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Product* theVisitor = sDynNonConst<Visitor_Expr_Rel_Product>(&iVisitor))
		this->Accept_Expr_Rel_Product(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Product::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Product> other = iOther.DynamicCast<Expr_Rel_Product>())
		{
		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;

		return this->GetOp1()->Compare(other->GetOp1());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Product::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Product* theVisitor = sDynNonConst<Visitor_Expr_Rel_Product>(&iVisitor))
		this->Accept_Expr_Rel_Product(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Product::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Product::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Product(iOp0, iOp1); }

void Expr_Rel_Product::Accept_Expr_Rel_Product(Visitor_Expr_Rel_Product& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Product(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Product

void Visitor_Expr_Rel_Product::Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sProduct(const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{
	if (iLHS && iRHS)
		return new Expr_Rel_Product(iLHS, iRHS);
	sSemanticError("sProduct, LHS and/or RHS are null");
	return null;
	}

ZP<Expr_Rel> operator*(const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{ return sProduct(iLHS, iRHS); }

ZP<Expr_Rel>& operator*=(ZP<Expr_Rel>& ioLHS, const ZP<Expr_Rel>& iRHS)
	{ return ioLHS = ioLHS * iRHS; }

} // namespace RelationalAlgebra
} // namespace ZooLib
