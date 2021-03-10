// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"

#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompareNew_T

template <>
int sCompareNew_T(const RelationalAlgebra::Expr_Rel_Embed& iL,
	const RelationalAlgebra::Expr_Rel_Embed& iR)
	{
	if (int compare = sCompareNew_T(iL.GetBoundNames(), iR.GetBoundNames()))
		return compare;

	if (int compare = sCompareNew_T(iL.GetColName(), iR.GetColName()))
		return compare;

	if (int compare = sCompareNew_T(iL.GetOp0(), iR.GetOp0()))
		return compare;

	return sCompareNew_T(iL.GetOp1(), iR.GetOp1());
	}

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Embed

Expr_Rel_Embed::Expr_Rel_Embed(const ZP<Expr_Rel>& iOp0, const RelHead& iBoundNames,
	const ColName& iColName, const ZP<Expr_Rel>& iEmbedee)
:	inherited(iOp0, iEmbedee)
,	fBoundNames(iBoundNames)
,	fColName(iColName)
	{}

void Expr_Rel_Embed::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Embed* theVisitor = sDynNonConst<Visitor_Expr_Rel_Embed>(&iVisitor))
		this->Accept_Expr_Rel_Embed(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Embed::Accept_Expr_Op2(Visitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Embed* theVisitor = sDynNonConst<Visitor_Expr_Rel_Embed>(&iVisitor))
		this->Accept_Expr_Rel_Embed(*theVisitor);
	else
		inherited::Accept_Expr_Op2(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Embed::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Embed::Clone(const ZP<Expr_Rel>& iOp0, const ZP<Expr_Rel>& iOp1)
	{ return new Expr_Rel_Embed(iOp0, fBoundNames, fColName, iOp1); }

void Expr_Rel_Embed::Accept_Expr_Rel_Embed(Visitor_Expr_Rel_Embed& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Embed(this); }

const RelHead& Expr_Rel_Embed::GetBoundNames() const
	{ return fBoundNames; }

const ColName& Expr_Rel_Embed::GetColName() const
	{ return fColName; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Embed

void Visitor_Expr_Rel_Embed::Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr)
	{ this->Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sEmbed(const ZP<Expr_Rel>& iOp0, const RelHead& iBoundNames,
	const ColName& iColName, const ZP<Expr_Rel>& iEmbedee)
	{ return new Expr_Rel_Embed(iOp0, iBoundNames, iColName, iEmbedee); }

} // namespace RelationalAlgebra
} // namespace ZooLib
