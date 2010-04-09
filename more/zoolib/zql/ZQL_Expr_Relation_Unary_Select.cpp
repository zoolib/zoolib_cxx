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

#include "zoolib/zql/ZQL_Expr_Relation_Unary_Select.h"
#include "zoolib/ZExpr_Logic_ValCondition.h" // For sGetRelHead

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation_Unary_Select

Expr_Relation_Unary_Select::Expr_Relation_Unary_Select(
	const ZRef<Expr_Relation>& iExpr_Relation, const ZRef<ZExpr_Logic>& iExpr_Logic)
:	Expr_Relation_Unary(iExpr_Relation)
,	fExpr_Logic(iExpr_Logic)
	{}

Expr_Relation_Unary_Select::~Expr_Relation_Unary_Select()
	{}

ZRelHead Expr_Relation_Unary_Select::GetRelHead()
	{ return this->GetExpr_Relation()->GetRelHead() | sGetRelHead(fExpr_Logic); }

void Expr_Relation_Unary_Select::Accept_Expr_Relation_Unary(
	Visitor_Expr_Relation_Unary& iVisitor)
	{
	if (Visitor_Expr_Relation_Unary_Select* theVisitor =
		dynamic_cast<Visitor_Expr_Relation_Unary_Select*>(&iVisitor))
		{
		this->Accept_Expr_Relation_Unary_Select(*theVisitor);
		}
	else
		{
		Expr_Relation_Unary::Accept_Expr_Relation_Unary(iVisitor);
		}
	}
void Expr_Relation_Unary_Select::Accept_Expr_Relation_Unary_Select(
	Visitor_Expr_Relation_Unary_Select& iVisitor)
	{ iVisitor.Visit_Expr_Relation_Unary_Select(this); }

ZRef<Expr_Relation_Unary> Expr_Relation_Unary_Select::Clone(
	ZRef<Expr_Relation> iExpr_Relation)
	{ return new Expr_Relation_Unary_Select(iExpr_Relation, fExpr_Logic); }

ZRef<ZExpr_Logic> Expr_Relation_Unary_Select::GetExpr_Logic()
	{ return fExpr_Logic; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Unary_Select

void Visitor_Expr_Relation_Unary_Select::Visit_Expr_Relation_Unary_Select(
	ZRef<Expr_Relation_Unary_Select> iRep)
	{
	//??
	Visitor_Expr_Relation_Unary::Visit_Expr_Relation_Unary(iRep);
//	if (ZRef<Expr_Relation> theExpr_Relation = iRep->GetExpr_Relation())
//		theExpr_Relation->Accept(*this);

	if (ZRef<ZExpr_Logic> theExpr_Logic = iRep->GetExpr_Logic())
		theExpr_Logic->Accept(*this);
	}

// =================================================================================================
#pragma mark -
#pragma mark *

ZRef<Expr_Relation_Unary_Select> sSelect(
	const ZRef<ZExpr_Logic>& iExpr_Logic, const ZRef<Expr_Relation>& iExpr_Relation)
	{ return new Expr_Relation_Unary_Select(iExpr_Relation, iExpr_Logic); }

ZRef<Expr_Relation_Unary_Select> operator&(
	const ZRef<ZExpr_Logic>& iExpr_Logic, const ZRef<Expr_Relation>& iExpr_Relation)
	{ return new Expr_Relation_Unary_Select(iExpr_Relation, iExpr_Logic); }

ZRef<Expr_Relation_Unary_Select> operator&(
	const ZRef<Expr_Relation>& iExpr_Relation, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{ return new Expr_Relation_Unary_Select(iExpr_Relation, iExpr_Logic); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
