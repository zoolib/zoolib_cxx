// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"

#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"

namespace ZooLib {
namespace RelationalAlgebra {

using std::map;

// =================================================================================================
#pragma mark - Expr_Rel_Calc

Expr_Rel_Calc::Expr_Rel_Calc(const ZP<Expr_Rel>& iOp0,
	const ColName& iColName,
	const ZP<Callable_t>& iCallable)
:	inherited(iOp0)
,	fColName(iColName)
,	fCallable(iCallable)
	{}

Expr_Rel_Calc::~Expr_Rel_Calc()
	{}

void Expr_Rel_Calc::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor = sDynNonConst<Visitor_Expr_Rel_Calc>(&iVisitor))
		this->Accept_Expr_Rel_Calc(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Calc::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Calc> other = iOther.DynamicCast<Expr_Rel_Calc>())
		{
		if (int compare = sCompareNew_T(this->GetColName(), other->GetColName()))
			return compare;

		if (int compare = this->GetOp0()->Compare(other->GetOp0()))
			return compare;

		if (this->GetCallable() < other->GetCallable())
			return -1;

		if (other->GetCallable() < this->GetCallable())
			return 1;

		return 0;
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Calc::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Calc* theVisitor = sDynNonConst<Visitor_Expr_Rel_Calc>(&iVisitor))
		this->Accept_Expr_Rel_Calc(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Calc::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Calc::Clone(const ZP<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Calc(iOp0, fColName, fCallable); }

void Expr_Rel_Calc::Accept_Expr_Rel_Calc(Visitor_Expr_Rel_Calc& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Calc(this); }

const ColName& Expr_Rel_Calc::GetColName() const
	{ return fColName; }

const ZP<Expr_Rel_Calc::Callable_t>& Expr_Rel_Calc::GetCallable() const
	{ return fCallable; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Calc

void Visitor_Expr_Rel_Calc::Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sCalc(const ZP<Expr_Rel>& iOp0,
	const ColName& iColName,
	const ZP<Expr_Rel_Calc::Callable_t>& iCallable)
	{
	return new Expr_Rel_Calc(iOp0, iColName, iCallable);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
