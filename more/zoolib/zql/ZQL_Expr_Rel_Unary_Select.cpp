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

#include "zoolib/zql/ZQL_Expr_Rel_Unary_Select.h"
#include "zoolib/ZExpr_Logic_ValCondition.h" // For sGetRelHead

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Unary_Select

Expr_Rel_Unary_Select::Expr_Rel_Unary_Select(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Logic>& iExpr_Logic)
:	Expr_Rel_Unary(iExpr_Rel)
,	fExpr_Logic(iExpr_Logic)
	{}

Expr_Rel_Unary_Select::~Expr_Rel_Unary_Select()
	{}

ZRelHead Expr_Rel_Unary_Select::GetRelHead()
	{ return this->GetExpr_Rel()->GetRelHead() | sGetRelHead(fExpr_Logic); }

void Expr_Rel_Unary_Select::Accept_Expr_Rel_Unary(
	Visitor_Expr_Rel_Unary& iVisitor)
	{
	if (Visitor_Expr_Rel_Unary_Select* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Unary_Select*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Unary_Select(*theVisitor);
		}
	else
		{
		Expr_Rel_Unary::Accept_Expr_Rel_Unary(iVisitor);
		}
	}
void Expr_Rel_Unary_Select::Accept_Expr_Rel_Unary_Select(
	Visitor_Expr_Rel_Unary_Select& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Unary_Select(this); }

ZRef<Expr_Rel_Unary> Expr_Rel_Unary_Select::Clone(
	ZRef<Expr_Rel> iExpr_Rel)
	{ return new Expr_Rel_Unary_Select(iExpr_Rel, fExpr_Logic); }

ZRef<ZExpr_Logic> Expr_Rel_Unary_Select::GetExpr_Logic()
	{ return fExpr_Logic; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_Select

void Visitor_Expr_Rel_Unary_Select::Visit_Expr_Rel_Unary_Select(
	ZRef<Expr_Rel_Unary_Select> iRep)
	{
	//??
	Visitor_Expr_Rel_Unary::Visit_Expr_Rel_Unary(iRep);
//	if (ZRef<Expr_Rel> theExpr_Rel = iRep->GetExpr_Rel())
//		theExpr_Rel->Accept(*this);

	if (ZRef<ZExpr_Logic> theExpr_Logic = iRep->GetExpr_Logic())
		theExpr_Logic->Accept(*this);
	}

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Unary_Select> sSelect(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{ return new Expr_Rel_Unary_Select(iExpr_Rel, iExpr_Logic); }

ZRef<Expr_Rel_Unary_Select> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Logic>& iExpr_Logic)
	{ return new Expr_Rel_Unary_Select(iExpr_Rel, iExpr_Logic); }

ZRef<Expr_Rel_Unary_Select> operator&(
	const ZRef<ZExpr_Logic>& iExpr_Logic, const ZRef<Expr_Rel>& iExpr_Rel)
	{ return new Expr_Rel_Unary_Select(iExpr_Rel, iExpr_Logic); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
