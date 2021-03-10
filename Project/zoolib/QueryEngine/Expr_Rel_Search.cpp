// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Expr_Rel_Search.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Expr_Rel_Search

Expr_Rel_Search::Expr_Rel_Search(const RelationalAlgebra::RelHead& iRelHead_Bound,
	const RelationalAlgebra::Rename& iRename,
	const RelationalAlgebra::RelHead& iRelHead_Optional,
	const ZP<Expr_Bool>& iExpr_Bool)
:	fRelHead_Bound(iRelHead_Bound)
,	fRename(iRename)
,	fRelHead_Optional(iRelHead_Optional)
,	fExpr_Bool(iExpr_Bool)
	{}

void Expr_Rel_Search::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Search* theVisitor = sDynNonConst<Visitor_Expr_Rel_Search>(&iVisitor))
		this->Accept_Expr_Rel_Search(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Search::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Search> other = iOther.DynamicCast<Expr_Rel_Search>())
		{
		if (int compare = sCompareNew_T(this->GetRelHead_Bound(), other->GetRelHead_Bound()))
			return compare;

		if (int compare = sCompareNew_T(this->GetRename(), other->GetRename()))
			return compare;

		if (int compare = sCompareNew_T(this->GetRelHead_Optional(), other->GetRelHead_Optional()))
			return compare;

		return this->GetExpr_Bool()->Compare(other->GetExpr_Bool());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Search::Accept_Expr_Op0(Visitor_Expr_Op0_T<RelationalAlgebra::Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Search* theVisitor = sDynNonConst<Visitor_Expr_Rel_Search>(&iVisitor))
		this->Accept_Expr_Rel_Search(*theVisitor);
	else
		inherited::Accept_Expr_Op0(iVisitor);
	}

ZP<RelationalAlgebra::Expr_Rel> Expr_Rel_Search::Self()
	{ return this; }

ZP<RelationalAlgebra::Expr_Rel> Expr_Rel_Search::Clone()
	{ return this; }

void Expr_Rel_Search::Accept_Expr_Rel_Search(Visitor_Expr_Rel_Search& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Search(this); }

const RelationalAlgebra::RelHead& Expr_Rel_Search::GetRelHead_Bound() const
	{ return fRelHead_Bound; }

const RelationalAlgebra::Rename& Expr_Rel_Search::GetRename() const
	{ return fRename; }

const RelationalAlgebra::RelHead& Expr_Rel_Search::GetRelHead_Optional() const
	{ return fRelHead_Optional; }

const ZP<Expr_Bool>& Expr_Rel_Search::GetExpr_Bool() const
	{ return fExpr_Bool; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Search

void Visitor_Expr_Rel_Search::Visit_Expr_Rel_Search(
	const ZP<Expr_Rel_Search>& iExpr)
	{ this->Visit_Expr(iExpr); }

} // namespace QueryEngine
} // namespace ZooLib
