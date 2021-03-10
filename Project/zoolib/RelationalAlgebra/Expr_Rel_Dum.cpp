// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Dum

Expr_Rel_Dum::Expr_Rel_Dum()
	{}

Expr_Rel_Dum::~Expr_Rel_Dum()
	{}

void Expr_Rel_Dum::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Dum* theVisitor = sDynNonConst<Visitor_Expr_Rel_Dum>(&iVisitor))
		this->Accept_Expr_Rel_Dum(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Dum::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Dum> other = iOther.DynamicCast<Expr_Rel_Dum>())
		return 0;

	return Expr::Compare(iOther);
	}

void Expr_Rel_Dum::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Dum* theVisitor = sDynNonConst<Visitor_Expr_Rel_Dum>(&iVisitor))
		this->Accept_Expr_Rel_Dum(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Dum::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Dum::Clone()
	{ return this; }

void Expr_Rel_Dum::Accept_Expr_Rel_Dum(Visitor_Expr_Rel_Dum& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Dum(this); }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Dum

void Visitor_Expr_Rel_Dum::Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sDum()
	{ return new Expr_Rel_Dum; }

} // namespace RelationalAlgebra
} // namespace ZooLib
