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

#include "zoolib/zql/ZQL_Expr_Rel.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Intersect.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Join.h"
#include "zoolib/zql/ZQL_Expr_Rel_Binary_Union.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Project.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary_Rename.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

Expr_Rel::Expr_Rel()
	{}

void Expr_Rel::Accept_Expr(ZVisitor_Expr& iVisitor)
	{
	if (Visitor_Expr_Rel* theVisitor =
		dynamic_cast<Visitor_Expr_Rel*>(&iVisitor))
		{
		this->Accept_Expr_Rel(*theVisitor);
		}
	else
		{
		ZExpr::Accept_Expr(iVisitor);
		}
	}

void Expr_Rel::Accept_Expr_Rel(Visitor_Expr_Rel& iVisitor)
	{ iVisitor.Visit_Expr_Rel(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel

void Visitor_Expr_Rel::Visit_Expr_Rel(ZRef<Expr_Rel> iRep)
	{ ZVisitor_Expr::Visit_Expr(iRep); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sIntersect(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Intersect(iLHS, iRHS); }

ZRef<Expr_Rel> sJoin(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Join(iLHS, iRHS); }

ZRef<Expr_Rel> sProject(const ZRelHead& iRelHead, const ZRef<Expr_Rel>& iExpr)
	{ return new Expr_Rel_Unary_Project(iExpr, iRelHead); }

ZRef<Expr_Rel> sRename(const string& iOldPropName, const string& iNewPropName,
	const ZRef<Expr_Rel>& iExpr)
	{ return new Expr_Rel_Unary_Rename(iExpr, iOldPropName, iNewPropName); }

ZRef<Expr_Rel> sUnion(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Union(iLHS, iRHS); }

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Intersect(iLHS, iRHS); }

ZRef<Expr_Rel> operator*(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Join(iLHS, iRHS); }

ZRef<Expr_Rel> operator|(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Binary_Union(iLHS, iRHS); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
