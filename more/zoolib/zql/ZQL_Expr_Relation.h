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

#ifndef __ZQL_Expr_Relation__
#define __ZQL_Expr_Relation__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"
#include "zoolib/ZRelHead.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Relation;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Relation

class Expr_Relation : public ZExpr
	{
protected:
	Expr_Relation();

public:
// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor);

// Our protocol
	virtual void Accept_Expr_Relation(Visitor_Expr_Relation& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation

class Visitor_Expr_Relation : public virtual ZVisitor_Expr
	{
public:
	virtual void Visit_Expr_Relation(ZRef<Expr_Relation> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Relation> sIntersect(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> sJoin(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> sProject(const ZRelHead& iRelHead, const ZRef<Expr_Relation>& iExpr);

ZRef<Expr_Relation> sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZRef<Expr_Relation>& iExpr);

ZRef<Expr_Relation> sUnion(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> operator&(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> operator&(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> operator*(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

ZRef<Expr_Relation> operator|(
	const ZRef<Expr_Relation>& iLHS, const ZRef<Expr_Relation>& iRHS);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Relation__
