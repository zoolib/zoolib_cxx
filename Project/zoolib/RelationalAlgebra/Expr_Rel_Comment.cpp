/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#include "zoolib/Compare_Ref.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T(const RelationalAlgebra::Expr_Rel_Comment& iL,
	const RelationalAlgebra::Expr_Rel_Comment& iR)
	{
	if (int compare = sCompare_T(iL.GetComment(), iR.GetComment()))
		return compare;

	return sCompare_T(iL.GetOp0(), iR.GetOp0());
	}

ZMACRO_CompareRegistration_T(RelationalAlgebra::Expr_Rel_Comment)

namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Expr_Rel_Comment

Expr_Rel_Comment::Expr_Rel_Comment(const ZRef<Expr_Rel>& iOp0, const std::string& iComment)
:	inherited(iOp0)
,	fComment(iComment)
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

void Expr_Rel_Comment::Accept_Expr_Op1(Visitor_Expr_Op1_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Comment* theVisitor = sDynNonConst<Visitor_Expr_Rel_Comment>(&iVisitor))
		this->Accept_Expr_Rel_Comment(*theVisitor);
	else
		inherited::Accept_Expr_Op1(iVisitor);
	}

ZRef<Expr_Rel> Expr_Rel_Comment::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Comment::Clone(const ZRef<Expr_Rel>& iOp0)
	{ return new Expr_Rel_Comment(iOp0, fComment); }

void Expr_Rel_Comment::Accept_Expr_Rel_Comment(Visitor_Expr_Rel_Comment& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Comment(this); }

const std::string& Expr_Rel_Comment::GetComment() const
	{ return fComment; }

// =================================================================================================
#pragma mark - Visitor_Expr_Rel_Comment

void Visitor_Expr_Rel_Comment::Visit_Expr_Rel_Comment(const ZRef<Expr_Rel_Comment>& iExpr)
	{ this->Visit_Expr_Op1(iExpr); }

// =================================================================================================
#pragma mark - Relational operators

ZRef<Expr_Rel_Comment> sComment(const ZRef<Expr_Rel>& iExpr_Rel, const std::string& iComment)
	{
	if (iExpr_Rel)
		return new Expr_Rel_Comment(iExpr_Rel, iComment);
	sSemanticError("sComment, rel is null");
	return null;
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
