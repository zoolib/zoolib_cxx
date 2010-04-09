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

#ifndef __ZQL_Expr_Rel_Unary_Select__
#define __ZQL_Expr_Rel_Unary_Select__ 1
#include "zconfig.h"

#include "zoolib/ZExpr_Logic.h"
#include "zoolib/zql/ZQL_Expr_Rel_Unary.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

class Visitor_Expr_Rel_Unary_Select;

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Unary_Select

class Expr_Rel_Unary_Select : public Expr_Rel_Unary
	{
public:
	Expr_Rel_Unary_Select(
		const ZRef<Expr_Rel>& iExpr_Rel,
		const ZRef<ZExpr_Logic>& iExpr_Logic);

	virtual ~Expr_Rel_Unary_Select();

// From Expr_Rel via Expr_Rel_Unary
	virtual ZRelHead GetRelHead();

// From Expr_Rel_Unary
	virtual void Accept_Expr_Rel_Unary(Visitor_Expr_Rel_Unary& iVisitor);

	virtual ZRef<Expr_Rel_Unary> Clone(ZRef<Expr_Rel> iExpr_Rel);

// Our protocol
	virtual void Accept_Expr_Rel_Unary_Select(
		Visitor_Expr_Rel_Unary_Select& iVisitor);

	ZRef<ZExpr_Logic> GetExpr_Logic();

private:
	const ZRef<ZExpr_Logic> fExpr_Logic;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_Select

class Visitor_Expr_Rel_Unary_Select : public virtual Visitor_Expr_Rel_Unary
	{
public:
	virtual void Visit_Expr_Rel_Unary_Select(ZRef<Expr_Rel_Unary_Select> iRep);
	};

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Unary_Select> sSelect(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Logic>& iExpr_Logic);

ZRef<Expr_Rel_Unary_Select> operator&(
	const ZRef<Expr_Rel>& iExpr_Rel, const ZRef<ZExpr_Logic>& iExpr_Logic);

ZRef<Expr_Rel_Unary_Select> operator&(
	const ZRef<ZExpr_Logic>& iExpr_Logic, const ZRef<Expr_Rel>& iExpr_Rel);

} // namespace ZQL
NAMESPACE_ZOOLIB_END

#endif // __ZQL_Expr_Rel_Unary_Select__
