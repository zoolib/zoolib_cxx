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

#include "zoolib/zql/ZQL_ExprRep_Relation_Binary.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation_Binary

ExprRep_Relation_Binary::ExprRep_Relation_Binary(
	ZRef<ExprRep_Relation> iLHS, ZRef<ExprRep_Relation> iRHS)
:	fLHS(iLHS)
,	fRHS(iRHS)
	{}

ExprRep_Relation_Binary::~ExprRep_Relation_Binary()
	{}

void ExprRep_Relation_Binary::Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Relation_Binary* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation_Binary*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation_Binary(*theVisitor);
		}
	else
		{
		ExprRep_Relation::Accept_ExprRep_Relation(iVisitor);
		}
	}

void ExprRep_Relation_Binary::Accept_ExprRep_Relation_Binary(
	Visitor_ExprRep_Relation_Binary& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation_Binary(this); }

ZRef<ExprRep_Relation> ExprRep_Relation_Binary::GetLHS()
	{ return fLHS; }

ZRef<ExprRep_Relation> ExprRep_Relation_Binary::GetRHS()
	{ return fRHS; }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Binary

void Visitor_ExprRep_Relation_Binary::Visit_ExprRep_Relation_Binary(
	ZRef<ExprRep_Relation_Binary> iRep)
	{
	if (ZRef<ExprRep_Relation> theRelation = iRep->GetLHS())
		theRelation->Accept(*this);

	if (ZRef<ExprRep_Relation> theRelation = iRep->GetRHS())
		theRelation->Accept(*this);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
