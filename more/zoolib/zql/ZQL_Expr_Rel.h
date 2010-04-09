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

#ifndef __ZQL_Expr_Rel__
#define __ZQL_Expr_Rel__ 1
#include "zconfig.h"

#include "zoolib/ZExpr.h"
#include "zoolib/ZRelHead.h"

#include <string>

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel

class Expr_Rel : public ZExpr
	{
protected:
	Expr_Rel();

public:
// From ZExpr
	virtual void Accept_Expr(ZVisitor_Expr& iVisitor);

// Our protocol
	virtual void Accept_Expr_Rel(Visitor_Expr_Rel& iVisitor);

	virtual ZRelHead GetRelHead() = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel

class Visitor_Expr_Rel : public virtual ZVisitor_Expr
	{
public:
	virtual void Visit_Expr_Rel(ZRef<Expr_Rel> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel> sIntersect(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> sJoin(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> sProject(const ZRelHead& iRelHead, const ZRef<Expr_Rel>& iExpr);

ZRef<Expr_Rel> sRename(const std::string& iOldPropName, const std::string& iNewPropName,
	const ZRef<Expr_Rel>& iExpr);

ZRef<Expr_Rel> sUnion(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> operator&(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> operator*(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel> operator|(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel__
