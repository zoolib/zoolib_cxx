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

#include "zoolib/zql/ZQL_ExprRep_Relation.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Binary_Intersect.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Binary_Join.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Binary_Union.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Project.h"
#include "zoolib/zql/ZQL_ExprRep_Relation_Unary_Rename.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Relation

ExprRep_Relation::ExprRep_Relation()
	{}

void ExprRep_Relation::Accept_ExprRep(ZVisitor_ExprRep& iVisitor)
	{
	if (Visitor_ExprRep_Relation* theVisitor =
		dynamic_cast<Visitor_ExprRep_Relation*>(&iVisitor))
		{
		this->Accept_ExprRep_Relation(*theVisitor);
		}
	else
		{
		ZExprRep::Accept_ExprRep(iVisitor);
		}
	}

void ExprRep_Relation::Accept_ExprRep_Relation(Visitor_ExprRep_Relation& iVisitor)
	{ iVisitor.Visit_ExprRep_Relation(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation

void Visitor_ExprRep_Relation::Visit_ExprRep_Relation(ZRef<ExprRep_Relation> iRep)
	{ ZVisitor_ExprRep::Visit_ExprRep(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<ExprRep_Relation> sIntersect(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Intersect(iLHS, iRHS); }

ZRef<ExprRep_Relation> sJoin(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Join(iLHS, iRHS); }

ZRef<ExprRep_Relation> sProject(const ZRelHead& iRelHead, const ZRef<ExprRep_Relation>& iExpr)
	{ return new ExprRep_Relation_Unary_Project(iExpr, iRelHead); }

ZRef<ExprRep_Relation> sRename(const string& iOldPropName, const string& iNewPropName,
	const ZRef<ExprRep_Relation>& iExpr)
	{ return new ExprRep_Relation_Unary_Rename(iExpr, iOldPropName, iNewPropName); }

ZRef<ExprRep_Relation> sUnion(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Union(iLHS, iRHS); }

ZRef<ExprRep_Relation> operator&(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Intersect(iLHS, iRHS); }

ZRef<ExprRep_Relation> operator*(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Join(iLHS, iRHS); }

ZRef<ExprRep_Relation> operator|(
	const ZRef<ExprRep_Relation>& iLHS, const ZRef<ExprRep_Relation>& iRHS)
	{ return new ExprRep_Relation_Binary_Union(iLHS, iRHS); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
