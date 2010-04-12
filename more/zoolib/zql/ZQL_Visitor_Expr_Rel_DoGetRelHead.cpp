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

#include "zoolib/zql/ZQL_Visitor_Expr_Rel_DoGetRelHead.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Rel_DoGetRelHead

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Op1(ZRef<ZExpr_Op1_T<Expr_Rel> > iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetOp0())
		result = this->Do(theExpr);

	this->pSetResult(result);
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Op2(ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetOp0())
		result = result | this->Do(theExpr);

	if (ZRef<Expr_Rel> theExpr = iExpr->GetOp1())
		result = result | this->Do(theExpr);

	this->pSetResult(result);
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
	{ this->pSetResult(iExpr->GetRelHead()); }

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetOp0())
		result = this->Do(theExpr) & iExpr->GetRelHead();

	this->pSetResult(result);
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetOp0())
		{
		result = this->Do(theExpr);
		const std::string oldName = iExpr->GetOld();
		if (result.Contains(oldName))
			{
			result -= oldName;
			result |= iExpr->GetNew();
			}
		}

	this->pSetResult(result);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
