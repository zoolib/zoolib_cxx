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

#include "zoolib/zql/ZQL_Expr_Rel_Unary.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

Expr_Rel_Unary::Expr_Rel_Unary(ZRef<Expr_Rel> iExpr_Rel)
:	fExpr_Rel(iExpr_Rel)
	{}

Expr_Rel_Unary::~Expr_Rel_Unary()
	{}

void Expr_Rel_Unary::Accept_Expr_Rel(Visitor_Expr_Rel& iVisitor)
	{
	if (Visitor_Expr_Rel_Unary* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Unary*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Unary(*theVisitor);
		}
	else
		{
		Expr_Rel::Accept_Expr_Rel(iVisitor);
		}
	}

void Expr_Rel_Unary::Accept_Expr_Rel_Unary(Visitor_Expr_Rel_Unary& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Unary(this); }

ZRef<Expr_Rel> Expr_Rel_Unary::GetExpr_Rel()
	{ return fExpr_Rel; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary

void Visitor_Expr_Rel_Unary::Visit_Expr_Rel_Unary(ZRef<Expr_Rel_Unary> iRep)
	{
	if (ZRef<Expr_Rel> theRelation = iRep->GetExpr_Rel())
		theRelation->Accept(*this);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
