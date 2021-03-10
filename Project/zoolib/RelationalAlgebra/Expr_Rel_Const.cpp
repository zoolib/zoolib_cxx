// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"

#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Const

Expr_Rel_Const::Expr_Rel_Const(const ColName& iColName, const Val_DB& iVal)
:	fColName(iColName)
,	fVal(iVal)
	{}

Expr_Rel_Const::~Expr_Rel_Const()
	{}

void Expr_Rel_Const::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = sDynNonConst<Visitor_Expr_Rel_Const>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Const::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Const> other = iOther.DynamicCast<Expr_Rel_Const>())
		{
		if (int compare = sCompareNew_T(this->GetColName(), other->GetColName()))
			return compare;

		return sCompareNew_T(this->GetVal(), other->GetVal());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Const::Accept_Expr_Op0(Visitor_Expr_Op0_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Const* theVisitor = sDynNonConst<Visitor_Expr_Rel_Const>(&iVisitor))
		this->Accept_Expr_Rel_Const(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Const::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Const::Clone()
	{ return this; }

void Expr_Rel_Const::Accept_Expr_Rel_Const(Visitor_Expr_Rel_Const& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Const(this); }

const ColName& Expr_Rel_Const::GetColName() const
	{ return fColName; }

const Val_DB& Expr_Rel_Const::GetVal() const
	{ return fVal; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Const

void Visitor_Expr_Rel_Const::Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
	{ this->Visit_Expr_Op0(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sConst(const ColName& iColName, const Val_DB& iVal)
	{ return new Expr_Rel_Const(iColName, iVal); }

} // namespace RelationalAlgebra
} // namespace ZooLib
