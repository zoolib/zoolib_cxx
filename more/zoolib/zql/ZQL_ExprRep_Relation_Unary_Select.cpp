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

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Select.h"
#include "zoolib/ZExprRep_Logic_ValCondition.h" // For sGetRelHead

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Unary_Select

ExprRep_Relation_Unary_Select::ExprRep_Relation_Unary_Select(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZRef<ZExprRep_Logic>& iExprRep_Logic)
:	ExprRep_Relation_Unary(iExprRep_Relation)
,	fExprRep_Logic(iExprRep_Logic)
	{}

ExprRep_Relation_Unary_Select::~ExprRep_Relation_Unary_Select()
	{}

ZRelHead ExprRep_Relation_Unary_Select::GetRelHead()
	{ return this->GetExprRep_Relation()->GetRelHead() | sGetRelHead(fExprRep_Logic); }

void ExprRep_Relation_Unary_Select::Accept_ExprRep_Relation_Unary(
	Visitor_ExprRep_Relation_Unary& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Unary_Select* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Unary_Select*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation_Unary_Select(*theVisitor);
		}
	else
		{
		ExprRep_Relation_Unary::Accept_ExprRep_Relation_Unary(iVisitor);
		}
	}
void ExprRep_Relation_Unary_Select::Accept_ExprRep_Relation_Unary_Select(
	Visitor_ExprRep_Relation_Unary_Select& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation_Unary_Select(this); }

ZRef<ExprRep_Relation_Unary> ExprRep_Relation_Unary_Select::Clone(
	ZRef<ExprRep_Relation> iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Select(iExprRep_Relation, fExprRep_Logic); }

ZRef<ZExprRep_Logic> ExprRep_Relation_Unary_Select::GetExprRep_Logic()
	{ return fExprRep_Logic; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary_Select

void Visitor_ExprRep_Relation_Unary_Select::Visit_ExprRep_Relation_Unary_Select(
	ZRef<ExprRep_Relation_Unary_Select> iRep)
	{
	//??
	Visitor_ExprRep_Relation_Unary::Visit_ExprRep_Relation_Unary(iRep);
//	if (ZRef<ExprRep_Relation> theExprRep_Relation = iRep->GetExprRep_Relation())
//		theExprRep_Relation->Accept(*this);

	if (ZRef<ZExprRep_Logic> theExprRep_Logic = iRep->GetExprRep_Logic())
		theExprRep_Logic->Accept(*this);
	}

// =================================================================================================
#pragma mark -
#pragma mark *

ZRef<ExprRep_Relation_Unary_Select> sSelect(
	const ZRef<ZExprRep_Logic>& iExprRep_Logic, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Select(iExprRep_Relation, iExprRep_Logic); }

ZRef<ExprRep_Relation_Unary_Select> operator&(
	const ZRef<ZExprRep_Logic>& iExprRep_Logic, const ZRef<ExprRep_Relation>& iExprRep_Relation)
	{ return new ExprRep_Relation_Unary_Select(iExprRep_Relation, iExprRep_Logic); }

ZRef<ExprRep_Relation_Unary_Select> operator&(
	const ZRef<ExprRep_Relation>& iExprRep_Relation, const ZRef<ZExprRep_Logic>& iExprRep_Logic)
	{ return new ExprRep_Relation_Unary_Select(iExprRep_Relation, iExprRep_Logic); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
