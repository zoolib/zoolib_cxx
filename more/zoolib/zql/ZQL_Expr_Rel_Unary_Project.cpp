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

#include "zoolib/zql/ZQL_Expr_Rel_Unary_Project.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Unary_Project

Expr_Rel_Unary_Project::Expr_Rel_Unary_Project(
		const ZRef<Expr_Rel>& iExpr_Rel, const ZRelHead& iRelHead)
:	Expr_Rel_Unary(iExpr_Rel)
,	fRelHead(iRelHead)
	{}

Expr_Rel_Unary_Project::~Expr_Rel_Unary_Project()
	{}

ZRelHead Expr_Rel_Unary_Project::GetRelHead()
	{ return this->GetExpr_Rel()->GetRelHead() & fRelHead; }

void Expr_Rel_Unary_Project::Accept_Expr_Rel_Unary(
	Visitor_Expr_Rel_Unary& iVisitor)
	{
	if (Visitor_Expr_Rel_Unary_Project* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Unary_Project*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Unary_Project(*theVisitor);
		}
	else
		{
		Expr_Rel_Unary::Accept_Expr_Rel_Unary(iVisitor);
		}
	}

ZRef<Expr_Rel_Unary> Expr_Rel_Unary_Project::Clone(
	ZRef<Expr_Rel> iExpr_Rel)
	{ return new Expr_Rel_Unary_Project(iExpr_Rel, fRelHead); }

void Expr_Rel_Unary_Project::Accept_Expr_Rel_Unary_Project(
	Visitor_Expr_Rel_Unary_Project& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Unary_Project(this); }

ZRelHead Expr_Rel_Unary_Project::GetProjectRelHead()
	{ return fRelHead; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_Project

void Visitor_Expr_Rel_Unary_Project::Visit_Expr_Rel_Unary_Project(
	ZRef<Expr_Rel_Unary_Project> iRep)
	{ Visitor_Expr_Rel_Unary::Visit_Expr_Rel_Unary(iRep); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
