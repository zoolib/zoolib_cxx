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

#include "zoolib/zql/ZQL_ExprRep_Relation_Unary.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation

ExprRep_Relation_Unary::ExprRep_Relation_Unary(ZRef<ExprRep_Relation> iExprRep_Relation)
:	fExprRep_Relation(iExprRep_Relation)
	{}

ExprRep_Relation_Unary::~ExprRep_Relation_Unary()
	{}

bool ExprRep_Relation_Unary::Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Unary* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Unary*>(&iVisitor))
		{
		return this->Accept_ExprRep_Relation_Unary(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept_ExprRep_Relation(iVisitor);
		}
	}

bool ExprRep_Relation_Unary::Accept_ExprRep_Relation_Unary(Visitor_ExprRep_Relation_Unary& iVisitor)
	{ return ExprRep_Relation::Accept_ExprRep_Relation(iVisitor); }

ZRef<ExprRep_Relation> ExprRep_Relation_Unary::GetExprRep_Relation()
	{ return fExprRep_Relation; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Unary

bool Visitor_ExprRep_Relation_Unary::Visit_ExprRep_Relation_Unary(ZRef<ExprRep_Relation_Unary> iRep)
	{
	if (ZRef<ExprRep_Relation> theRelation = iRep->GetExprRep_Relation())
		theRelation->Accept(*this);

	return true;
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
