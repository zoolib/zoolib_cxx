// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Project

Expr_Rel_Project::Expr_Rel_Project(const ZP<Expr_Rel>& iOp0, const RelHead& iRelHead)
:	inherited(iOp0)
,	fRelHead(iRelHead)
	{}

Expr_Rel_Project::~Expr_Rel_Project()
	{}

void Expr_Rel_Project::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Project* theVisitor = sDynNonConst<Visitor_Expr_Rel_Project>(&iVisitor))
		this->Accept_Expr_Rel_Project(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Project::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Project> other = iOther.DynamicCast<Expr_Rel_Project>())
		{
		if (int compare = sCompare_T(this->GetProjectRelHead(), other->GetProjectRelHead()))
			return compare;
		return this->GetOp0()->Compare(other->GetOp0());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Project::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Project* theVisitor = sDynNonConst<Visitor_Expr_Rel_Project>(&iVisitor))
		this->Accept_Expr_Rel_Project(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Project::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Project::Clone(const ZP<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Project(iOp0, fRelHead); }

void Expr_Rel_Project::Accept_Expr_Rel_Project(Visitor_Expr_Rel_Project& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Project(this); }

const RelHead& Expr_Rel_Project::GetProjectRelHead() const
	{ return fRelHead; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Project

void Visitor_Expr_Rel_Project::Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Project> sProject(const ZP<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{
	if (iExpr)
		return new Expr_Rel_Project(iExpr, iRelHead);
	sSemanticError("sProject, rel is null");
	return null;
	}

ZP<Expr_Rel> operator&(const ZP<Expr_Rel>& iExpr, const RelHead& iRelHead)
	{ return sProject(iExpr, iRelHead); }

ZP<Expr_Rel> operator&(const RelHead& iRelHead, const ZP<Expr_Rel>& iExpr)
	{ return sProject(iExpr, iRelHead); }

ZP<Expr_Rel>& operator&=(ZP<Expr_Rel>& ioExpr, const RelHead& iRelHead)
	{ return ioExpr = ioExpr & iRelHead; }

} // namespace RelationalAlgebra
} // namespace ZooLib
