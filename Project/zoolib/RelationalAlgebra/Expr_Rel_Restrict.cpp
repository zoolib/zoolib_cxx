// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompareNew_T

template <>
int sCompareNew_T(const RelationalAlgebra::Expr_Rel_Restrict& iL,
	const RelationalAlgebra::Expr_Rel_Restrict& iR)
	{
	if (int compare = sCompareNew_T(iL.GetExpr_Bool(), iR.GetExpr_Bool()))
		return compare;

	return sCompareNew_T(iL.GetOp0(), iR.GetOp0());
	}

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Restrict

Expr_Rel_Restrict::Expr_Rel_Restrict(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Bool>& iExpr_Bool)
:	inherited(iOp0)
,	fExpr_Bool(iExpr_Bool)
	{}

Expr_Rel_Restrict::~Expr_Rel_Restrict()
	{}

void Expr_Rel_Restrict::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Restrict* theVisitor = sDynNonConst<Visitor_Expr_Rel_Restrict>(&iVisitor))
		this->Accept_Expr_Rel_Restrict(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Restrict::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Restrict* theVisitor = sDynNonConst<Visitor_Expr_Rel_Restrict>(&iVisitor))
		this->Accept_Expr_Rel_Restrict(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Restrict::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Restrict::Clone(const ZP<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Restrict(iOp0, fExpr_Bool); }

void Expr_Rel_Restrict::Accept_Expr_Rel_Restrict(Visitor_Expr_Rel_Restrict& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Restrict(this); }

const ZP<Expr_Bool>& Expr_Rel_Restrict::GetExpr_Bool() const
	{ return fExpr_Bool; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Restrict

void Visitor_Expr_Rel_Restrict::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	this->Visit_Expr_Op1(iExpr);

	if (const ZP<Expr_Bool>& theExpr_Bool = iExpr->GetExpr_Bool())
		theExpr_Bool->Accept(*this);
	}

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Restrict> sRestrict(
	const ZP<Expr_Rel>& iExpr_Rel, const ZP<Expr_Bool>& iExpr_Bool)
	{
	if (iExpr_Rel && iExpr_Bool)
		return new Expr_Rel_Restrict(iExpr_Rel, iExpr_Bool);
	sSemanticError("sRestrict, rel and/or bool are null");
	return null;
	}

ZP<Expr_Rel> operator&(
	const ZP<Expr_Rel>& iExpr_Rel, const ZP<Expr_Bool>& iExpr_Bool)
	{ return sRestrict(iExpr_Rel, iExpr_Bool); }

ZP<Expr_Rel> operator&(
	const ZP<Expr_Bool>& iExpr_Bool, const ZP<Expr_Rel>& iExpr_Rel)
	{ return sRestrict(iExpr_Rel, iExpr_Bool); }

ZP<Expr_Rel>& operator&=(ZP<Expr_Rel>& ioExpr_Rel, const ZP<Expr_Bool>& iExpr_Bool)
	{ return ioExpr_Rel = ioExpr_Rel & iExpr_Bool; }

} // namespace RelationalAlgebra
} // namespace ZooLib
