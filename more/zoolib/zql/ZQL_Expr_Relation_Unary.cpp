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

#include "zoolib/zql/ZQL_Expr_Relation_Unary.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation

Expr_Relation_Unary::Expr_Relation_Unary(ZRef<Expr_Relation> iExpr_Relation)
:	fExpr_Relation(iExpr_Relation)
	{}

Expr_Relation_Unary::~Expr_Relation_Unary()
	{}

void Expr_Relation_Unary::Accept_Expr_Relation(Visitor_Expr_Relation& iVisitor)
	{
	if (Visitor_Expr_Relation_Unary* theVisitor =
		dynamic_cast<Visitor_Expr_Relation_Unary*>(&iVisitor))
		{
		this->Accept_Expr_Relation_Unary(*theVisitor);
		}
	else
		{
		Expr_Relation::Accept_Expr_Relation(iVisitor);
		}
	}

void Expr_Relation_Unary::Accept_Expr_Relation_Unary(Visitor_Expr_Relation_Unary& iVisitor)
	{ iVisitor.Visit_Expr_Relation_Unary(this); }

ZRef<Expr_Relation> Expr_Relation_Unary::GetExpr_Relation()
	{ return fExpr_Relation; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Unary

void Visitor_Expr_Relation_Unary::Visit_Expr_Relation_Unary(ZRef<Expr_Relation_Unary> iRep)
	{
	if (ZRef<Expr_Relation> theRelation = iRep->GetExpr_Relation())
		theRelation->Accept(*this);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
