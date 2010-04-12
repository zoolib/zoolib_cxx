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

#include "zoolib/zql/ZQL_Expr_Rel_Join.h"

using std::string;

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Expr_Rel_Join

Expr_Rel_Join::Expr_Rel_Join(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1)
:	inherited(iOp0, iOp1)
	{}

void Expr_Rel_Join::Accept_Expr_Op2(ZVisitor_Expr_Op2_T<Expr_Rel>& iVisitor)
	{
	if (Visitor_Expr_Rel_Join* theVisitor =
		dynamic_cast<Visitor_Expr_Rel_Join*>(&iVisitor))
		{
		this->Accept_Expr_Rel_Join(*theVisitor);
		}
	else
		{
		inherited::Accept_Expr_Op2(iVisitor);
		}
	}

ZRef<Expr_Rel> Expr_Rel_Join::Self()
	{ return this; }

ZRef<Expr_Rel> Expr_Rel_Join::Clone(ZRef<Expr_Rel> iOp0, ZRef<Expr_Rel> iOp1)
	{ return new Expr_Rel_Join(iOp0, iOp1); }

void Expr_Rel_Join::Accept_Expr_Rel_Join(Visitor_Expr_Rel_Join& iVisitor)
	{ iVisitor.Visit_Expr_Rel_Join(this); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Join

void Visitor_Expr_Rel_Join::Visit_Expr_Rel_Join(ZRef<Expr_Rel_Join> iExpr)
	{ inherited::Visit_Expr_Op2(iExpr); }

// =================================================================================================
#pragma mark -
#pragma mark * Relational operators

ZRef<Expr_Rel_Join> sJoin(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Join(iLHS, iRHS); }

ZRef<Expr_Rel_Join> operator*(
	const ZRef<Expr_Rel>& iLHS, const ZRef<Expr_Rel>& iRHS)
	{ return new Expr_Rel_Join(iLHS, iRHS); }

} // namespace ZQL
NAMESPACE_ZOOLIB_END
