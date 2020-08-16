// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"

#include "zoolib/Compare_Ref.h"
#include "zoolib/Compare_string.h"
#include "zoolib/Log.h"

using std::string;

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Rename& iL,
	const RelationalAlgebra::Expr_Rel_Rename& iR)
	{
	if (int compare = sCompare_T(iL.GetOld(), iR.GetOld()))
		return compare;

	if (int compare = sCompare_T(iL.GetNew(), iR.GetNew()))
		return compare;

	return sCompare_T(iL.GetOp0(), iR.GetOp0());
	}

ZMACRO_CompareRegistration_T(RelationalAlgebra::Expr_Rel_Rename)

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Rename

Expr_Rel_Rename::Expr_Rel_Rename(const ZP<Expr_Rel>& iOp0,
	const ColName& iNew, const ColName& iOld)
:	inherited(iOp0)
,	fNew(iNew)
,	fOld(iOld)
	{}

Expr_Rel_Rename::~Expr_Rel_Rename()
	{}

void Expr_Rel_Rename::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Rename* theVisitor = sDynNonConst<Visitor_Expr_Rel_Rename>(&iVisitor))
		this->Accept_Expr_Rel_Rename(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

void Expr_Rel_Rename::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Rename* theVisitor = sDynNonConst<Visitor_Expr_Rel_Rename>(&iVisitor))
		this->Accept_Expr_Rel_Rename(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Rename::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Rename::Clone(const ZP<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Rename(iOp0, fNew, fOld); }

void Expr_Rel_Rename::Accept_Expr_Rel_Rename(Visitor_Expr_Rel_Rename& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Rename(this); }

const ColName& Expr_Rel_Rename::GetNew() const
	{ return fNew; }

const ColName& Expr_Rel_Rename::GetOld() const
	{ return fOld; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Rename

void Visitor_Expr_Rel_Rename::Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel> sRename(const ZP<Expr_Rel>& iExpr,
	const ColName& iNewPropName, const ColName& iOldPropName)
	{
	if (iExpr)
		return new Expr_Rel_Rename(iExpr, iNewPropName, iOldPropName);
	sSemanticError("sRename, rel is null");
	return null;
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
