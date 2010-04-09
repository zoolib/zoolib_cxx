/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/zql/ZQL_Expr_Relation_Unary_Project.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Unary_Project

Expr_Relation_Unary_Project::Expr_Relation_Unary_Project(
		const ZRef<Expr_Relation>& iExpr_Relation, const ZRelHead& iRelHead)
:	Expr_Relation_Unary(iExpr_Relation)
,	fRelHead(iRelHead)
	{}

Expr_Relation_Unary_Project::~Expr_Relation_Unary_Project()
	{}

ZRelHead Expr_Relation_Unary_Project::GetRelHead()
	{ return this->GetExpr_Relation()->GetRelHead() & fRelHead; }

void Expr_Relation_Unary_Project::Accept_Expr_Relation_Unary(
	Visitor_Expr_Relation_Unary& iVisitor)
	{
	if (Visitor_Expr_Relation_Unary_Project* theVisitor =
		dynamic_cast<Visitor_Expr_Relation_Unary_Project*>(&iVisitor))
		{
		this->Accept_Expr_Relation_Unary_Project(*theVisitor);
		}
	else
		{
		Expr_Relation_Unary::Accept_Expr_Relation_Unary(iVisitor);
		}
	}

ZRef<Expr_Relation_Unary> Expr_Relation_Unary_Project::Clone(
	ZRef<Expr_Relation> iExpr_Relation)
	{ return new Expr_Relation_Unary_Project(iExpr_Relation, fRelHead); }

void Expr_Relation_Unary_Project::Accept_Expr_Relation_Unary_Project(
	Visitor_Expr_Relation_Unary_Project& iVisitor)
	{ iVisitor.Visit_Expr_Relation_Unary_Project(this); }

ZRelHead Expr_Relation_Unary_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Unary_Project

void Visitor_Expr_Relation_Unary_Project::Visit_Expr_Relation_Unary_Project(
	ZRef<Expr_Relation_Unary_Project> iRep)
	{ Visitor_Expr_Relation_Unary::Visit_Expr_Relation_Unary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
