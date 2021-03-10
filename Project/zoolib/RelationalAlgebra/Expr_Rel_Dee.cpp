// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Dee

Expr_Rel_Dee::Expr_Rel_Dee()
	{}

Expr_Rel_Dee::~Expr_Rel_Dee()
	{}

void Expr_Rel_Dee::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Dee* theVisitor = sDynNonConst<Visitor_Expr_Rel_Dee>(&iVisitor))
		this->Accept_Expr_Rel_Dee(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Dee::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Dee> other = iOther.DynamicCast<Expr_Rel_Dee>())
		return 0;

	return Expr::Compare(iOther);
	}

void Expr_Rel_Dee::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Dee* theVisitor = sDynNonConst<Visitor_Expr_Rel_Dee>(&iVisitor))
		this->Accept_Expr_Rel_Dee(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Dee::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Dee::Clone()
	{ return this; }

void Expr_Rel_Dee::Accept_Expr_Rel_Dee(Visitor_Expr_Rel_Dee& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Dee(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Dee

void Visitor_Expr_Rel_Dee::Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sDee()
	{ return new Expr_Rel_Dee; }

} // namespace RelationalAlgebra
} // namespace ZooLib
