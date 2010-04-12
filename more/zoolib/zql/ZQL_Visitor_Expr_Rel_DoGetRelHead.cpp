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

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Binary(
	ZRef<Expr_Rel_Binary> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theLHS = iExpr->GetLHS())
		result = result | this->DoGetRelHead(theLHS);

	if (ZRef<Expr_Rel> theRHS = iExpr->GetRHS())
		result = result | this->DoGetRelHead(theRHS);

	fResult = result;
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Concrete(
	ZRef<Expr_Rel_Concrete> iExpr)
	{ fResult = iExpr->GetRelHead(); }

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Unary(
	ZRef<Expr_Rel_Unary> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetExpr_Rel())
		result = this->DoGetRelHead(theExpr);

	fResult = result;
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Unary_Project(
	ZRef<Expr_Rel_Unary_Project> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetExpr_Rel())
		result = this->DoGetRelHead(theExpr) & iExpr->GetRelHead();

	fResult = result;
	}

void Visitor_Expr_Rel_DoGetRelHead::Visit_Expr_Rel_Unary_Rename(
	ZRef<Expr_Rel_Unary_Rename> iExpr)
	{
	RelHead result;

	if (ZRef<Expr_Rel> theExpr = iExpr->GetExpr_Rel())
		{
		result = this->DoGetRelHead(theExpr);
		const std::string oldName = iExpr->GetOld();
		if (result.Contains(oldName))
			{
			result -= oldName;
			result |= iExpr->GetNew();
			}
		}

	fResult = result;
	}

RelHead Visitor_Expr_Rel_DoGetRelHead::DoGetRelHead(ZRef<Expr_Rel> iExpr)
	{
	RelHead theResult;
	if (iExpr)
		{
		iExpr->Accept(*this);
		std::swap(theResult, fResult);
		}
	return theResult;	
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
