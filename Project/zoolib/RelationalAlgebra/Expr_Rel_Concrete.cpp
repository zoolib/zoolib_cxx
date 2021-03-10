// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Concrete

Expr_Rel_Concrete::Expr_Rel_Concrete(const ConcreteHead& iConcreteHead)
:	fConcreteHead(iConcreteHead)
	{}

void Expr_Rel_Concrete::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Concrete* theVisitor = sDynNonConst<Visitor_Expr_Rel_Concrete>(&iVisitor))
		this->Accept_Expr_Rel_Concrete(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Concrete::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Concrete> other = iOther.DynamicCast<Expr_Rel_Concrete>())
		return sCompareNew_T(this->GetConcreteHead(), other->GetConcreteHead());

	return Expr::Compare(iOther);
	}

void Expr_Rel_Concrete::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Concrete* theVisitor = sDynNonConst<Visitor_Expr_Rel_Concrete>(&iVisitor))
		this->Accept_Expr_Rel_Concrete(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Concrete::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Concrete::Clone()
	{ return this; }

void Expr_Rel_Concrete::Accept_Expr_Rel_Concrete(Visitor_Expr_Rel_Concrete& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Concrete(this); }

const ConcreteHead& Expr_Rel_Concrete::GetConcreteHead() const
	{ return fConcreteHead; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Concrete

void Visitor_Expr_Rel_Concrete::Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - sConcrete

ZP<Expr_Rel> sConcrete(const RelHead& iRelHead)
	{ return new Expr_Rel_Concrete(sConcreteHead(iRelHead)); }

ZP<Expr_Rel> sConcrete(const RelHead& iRequired, const RelHead& iOptional)
	{ return new Expr_Rel_Concrete(sConcreteHead(iRequired, iOptional)); }

ZP<Expr_Rel> sConcrete(const ConcreteHead& iConcreteHead)
	{ return new Expr_Rel_Concrete(iConcreteHead); }

} // namespace RelationalAlgebra
} // namespace ZooLib
