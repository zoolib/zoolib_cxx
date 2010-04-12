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

#include "zoolib/zql/ZQL_Visitor_Expr_Rel_Unary_DoTransform.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_Unary_DoTransform

void Visitor_Expr_Rel_Unary_DoTransform::Visit_Expr_Rel_Unary(
	ZRef<Expr_Rel_Unary> iExpr)
	{
	ZRef<Expr_Rel> oldExpr = iExpr->GetExpr_Rel();
	ZRef<Expr_Rel> newExpr = this->DoTransform(oldExpr).DynamicCast<Expr_Rel>();
	if (oldExpr == newExpr)
		fResult = iExpr;
	else
		fResult = iExpr->Clone(newExpr);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
