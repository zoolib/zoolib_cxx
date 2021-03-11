// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"

#include "zoolib/Compare_string.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Comment

Expr_Rel_Comment::Expr_Rel_Comment(const ZP<Expr_Rel>& iOp0,
	const std::string& iComment,
	const ZP<Callable_Void>& iCallable)
:	inherited(iOp0)
,	fComment(iComment)
,	fCallable(iCallable)
	{}

Expr_Rel_Comment::~Expr_Rel_Comment()
	{}

void Expr_Rel_Comment::Accept(const Visitor& iVisitor)
	{
	if (Visitor_Expr_Rel_Comment* theVisitor = sDynNonConst<Visitor_Expr_Rel_Comment>(&iVisitor))
		this->Accept_Expr_Rel_Comment(*theVisitor);
	else
		inherited::Accept(iVisitor);
	}

int Expr_Rel_Comment::Compare(const ZP<Expr>& iOther)
	{
	if (ZP<Expr_Rel_Comment> other = iOther.DynamicCast<Expr_Rel_Comment>())
		{
		if (int compare = sCompare_T(this->GetComment(), other->GetComment()))
			return compare;

		return this->GetOp0()->Compare(other->GetOp0());
		}

	return Expr::Compare(iOther);
	}

void Expr_Rel_Comment::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Comment* theVisitor = sDynNonConst<Visitor_Expr_Rel_Comment>(&iVisitor))
		this->Accept_Expr_Rel_Comment(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZP<Expr_Rel> Expr_Rel_Comment::Self()
	{ return this; }

ZP<Expr_Rel> Expr_Rel_Comment::Clone(const ZP<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Comment(iOp0, fComment, fCallable); }

void Expr_Rel_Comment::Accept_Expr_Rel_Comment(Visitor_Expr_Rel_Comment& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Comment(this); }

const std::string& Expr_Rel_Comment::GetComment() const
	{ return fComment; }

ZP<Callable_Void> Expr_Rel_Comment::GetCallable() const
	{ return fCallable; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Comment

void Visitor_Expr_Rel_Comment::Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZP<Expr_Rel_Comment> sComment(const ZP<Expr_Rel>& iExpr_Rel, const std::string& iComment)
	{
	if (iExpr_Rel)
		return new Expr_Rel_Comment(iExpr_Rel, iComment, null);
	sSemanticError("sComment, rel is null");
	return null;
	}

ZP<Expr_Rel_Comment> sComment(const ZP<Expr_Rel>& iExpr_Rel,
	const std::string& iComment,
	const ZP<Callable_Void>& iCallable)
	{
	if (iExpr_Rel)
		return new Expr_Rel_Comment(iExpr_Rel, iComment, iCallable);
	sSemanticError("sComment, rel is null");
	return null;
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
