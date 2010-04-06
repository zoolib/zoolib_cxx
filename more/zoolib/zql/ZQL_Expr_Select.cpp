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

#include "zoolib/zql/ZQL_Expr_Select.h"
#include "zoolib/ZExpr_ValCondition.h" // For sGetRelHead

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Select

ExprRep_Select::ExprRep_Select(
	const ZRef<ZExprRep_Logical>& iExprRep_Logical, const ZRef<ExprRep_Relation>& iExprRep_Relation)
:	fExprRep_Logical(iExprRep_Logical)
,	fExprRep_Relation(iExprRep_Relation)
	{}

ExprRep_Select::~ExprRep_Select()
	{}

bool ExprRep_Select::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Select* theVisitor =
		dynamic_cast<Visitor_ExprRep_Select*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

ZRelHead ExprRep_Select::GetRelHead()
	{ return sGetRelHead(fExprRep_Logical) | fExprRep_Relation->GetRelHead(); }

bool ExprRep_Select::Accept(Visitor_ExprRep_Select& iVisitor)
	{ return iVisitor.Visit_Select(this); }

ZRef<ZExprRep_Logical> ExprRep_Select::GetExprRep_Logical()
	{ return fExprRep_Logical; }

ZRef<ExprRep_Relation> ExprRep_Select::GetExprRep_Relation()
	{ return fExprRep_Relation; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Select

bool Visitor_ExprRep_Select::Visit_Select(ZRef<ExprRep_Select> iRep)
	{
	if (!Visitor_ExprRep_Relation::Visit_ExprRep(iRep))
		return false;

	if (ZRef<ZExprRep_Logical> theExpr_Logical = iRep->GetExprRep_Logical())
		{
		if (!theExpr_Logical->Accept(*this))
			return false;
		}

	if (ZRef<ExprRep_Relation> theExpr_Relation = iRep->GetExprRep_Relation())
		{
		if (!theExpr_Relation->Accept(*this))
			return false;
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Select

Expr_Select::Expr_Select()
	{}

Expr_Select::Expr_Select(const Expr_Select& iOther)
:	inherited(iOther)
	{}

Expr_Select::~Expr_Select()
	{}

Expr_Select& Expr_Select::operator=(const Expr_Select& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Expr_Select::Expr_Select(const ZRef<ExprRep_Select>& iRep)
:	inherited(iRep)
	{}

Expr_Select::operator ZRef<ExprRep_Select>() const
	{ return this->StaticCast<ExprRep_Select>(); }

// =================================================================================================
#pragma mark -
#pragma mark *

Expr_Select sSelect(const ZExpr_Logical& iExpr_Logical, const Expr_Relation& iExpr_Relation)
	{ return Expr_Select(new ExprRep_Select(iExpr_Logical, iExpr_Relation)); }

Expr_Select operator&(const ZExpr_Logical& iExpr_Logical, const Expr_Relation& iExpr_Relation)
	{ return Expr_Select(new ExprRep_Select(iExpr_Logical, iExpr_Relation)); }

Expr_Select operator&(const Expr_Relation& iExpr_Relation, const ZExpr_Logical& iExpr_Logical)
	{ return Expr_Select(new ExprRep_Select(iExpr_Logical, iExpr_Relation)); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
