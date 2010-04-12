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

#ifndef __ZQL_Expr_Rel_Intersect__
#define __ZQL_Expr_Rel_Intersect__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Op_T.h"
#include "zoolib/zql/ZQL_Expr_Rel.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel_Intersect;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Intersect

class Expr_Rel_Intersect
:	public virtual Expr_Rel
,	public virtual ZExpr_Op2_T<Expr_Rel>
	{
	typedef ZExpr_Op2_T<Expr_Rel> inherited;
public:
	Expr_Rel_Intersect(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1);

// From ZExpr_Op2_T
	virtual void Accept_Expr_Op2(ZVisitor_Expr_Op2_T<Expr_Rel>& iVisitor);

	virtual ZRef<Expr_Rel> Self();
	virtual ZRef<Expr_Rel> Clone(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1);

// Our protocol
	virtual void Accept_Expr_Rel_Intersect(Visitor_Expr_Rel_Intersect& iVisitor);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Intersect

class Visitor_Expr_Rel_Intersect : public virtual ZVisitor_Expr_Op2_T<Expr_Rel>
	{
	typedef ZVisitor_Expr_Op2_T<Expr_Rel> inherited;
public:
	virtual void Visit_Expr_Rel_Intersect(ZRef<Expr_Rel_Intersect> iExpr);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Intersect> sIntersect(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

ZRef<Expr_Rel_Intersect> operator&(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel__
