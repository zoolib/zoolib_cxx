// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/QueryEngine/Expr_Rel_Search.h"

#include "zoolib/Compare_Ref.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompareNew_T

template <>
int sCompareNew_T(const QueryEngine::Expr_Rel_Search& iL, const QueryEngine::Expr_Rel_Search& iR)
	{
	if (int compare = sCompareNew_T(iL.GetRelHead_Bound(), iR.GetRelHead_Bound()))
		return compare;

	if (int compare = sCompareNew_T(iL.GetRename(), iR.GetRename()))
		return compare;

	if (int compare = sCompareNew_T(iL.GetRelHead_Optional(), iR.GetRelHead_Optional()))
		return compare;

	return sCompareNew_T(iL.GetExpr_Bool(), iR.GetExpr_Bool());
	}

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
