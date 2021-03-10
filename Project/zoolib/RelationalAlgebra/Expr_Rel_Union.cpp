// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Union

Expr_Rel_Union::Expr_Rel_Union(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
:	inherited(iOp0, iOp1)
	{
	ZAssert(iOp0 and iOp1);
	}

int Expr_Rel_Union::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Union> other = iOther.DynamicCast<Expr_Rel_Union>())
		{
		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;

		return this->GetOp1()->Compare(other->GetOp1());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Union::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Union* theVisitor = sDynNonConst<Visitor_Expr_Rel_Union>(&iVisitor))
		this->Accept_Expr_Rel_Union(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Union::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Union::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Union(iOp0, iOp1); }

void Expr_Rel_Union::Accept_Expr_Rel_Union(Visitor_Expr_Rel_Union& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Union(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Union

void Visitor_Expr_Rel_Union::Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Union> sUnion(const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Union(iLHS, iRHS); }

ZP<Expr_Rel> operator|(const ZP<Expr_Rel>& iLHS, const ZP<Expr_Rel>& iRHS)
	{ return sUnion(iLHS, iRHS); }

ZP<Expr_Rel>& operator|=(ZP<Expr_Rel>& ioLHS, const ZP<Expr_Rel>& iRHS)
	{ return ioLHS = ioLHS | iRHS; }

} // namespace RelationalAlgebra
} // namespace ZooLib
