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

#include "zoolib/zqe/ZQE_Visitor_Expr_DoMakeIterator.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_DoMakeIterator

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Binary_Difference(
	ZRef<Expr_Rel_Binary_Difference> iRep)
	{}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Binary_Intersect(
	ZRef<Expr_Rel_Binary_Intersect> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Intersect(lhs, rhs);
		}
	}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Binary_Join(
	ZRef<Expr_Rel_Binary_Join> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Join(lhs, rhs);
		}
	}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Binary_Union(
	ZRef<Expr_Rel_Binary_Union> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Union(lhs, rhs);
		else
			fIterator = lhs;
		}
	}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Unary_Project(
	ZRef<Expr_Rel_Unary_Project> iRep)
	{}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Unary_Rename(
	ZRef<Expr_Rel_Unary_Rename> iRep)
	{}

void Visitor_Expr_DoMakeIterator::Visit_Expr_Rel_Unary_Select(
	ZRef<Expr_Rel_Unary_Select> iRep)
	{}

ZRef<Iterator> Visitor_Expr_DoMakeIterator::DoMakeIterator(ZRef<ZExpr> iExpr)
	{
	ZRef<Iterator> theIterator;
	if (iExpr)
		{
		iExpr->Accept(*this);
		theIterator.swap(fIterator);
		}
	return theIterator;	
	}

} // namespace ZQE
NAMESPACE_ZOOLIB_END
