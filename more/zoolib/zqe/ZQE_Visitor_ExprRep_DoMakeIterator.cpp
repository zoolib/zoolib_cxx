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

#include "zoolib/zqe/ZQE_Visitor_ExprRep_DoMakeIterator.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_DoMakeIterator

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Difference(ZRef<ExprRep_Relation_Difference> iRep)
	{
	return true;
	}

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Intersect(ZRef<ExprRep_Relation_Intersect> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Intersect(lhs, rhs);
		}
	return true;
	}

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Join(ZRef<ExprRep_Relation_Join> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Join(lhs, rhs);
		}
	return true;
	}

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Project(ZRef<ExprRep_Relation_Project> iRep)
	{
	return true;
	}

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Rename(ZRef<ExprRep_Relation_Rename> iRep)
	{
	return true;
	}

bool Visitor_ExprRep_DoMakeIterator::Visit_ExprRep_Relation_Union(ZRef<ExprRep_Relation_Union> iRep)
	{
	if (ZRef<Iterator> lhs = this->DoMakeIterator(iRep->GetLHS()))
		{
		if (ZRef<Iterator> rhs = this->DoMakeIterator(iRep->GetRHS()))
			fIterator = new Iterator_Union(lhs, rhs);
		else
			fIterator = lhs;
		}
	return true;
	}

ZRef<Iterator> Visitor_ExprRep_DoMakeIterator::DoMakeIterator(ZRef<ZExprRep> iExprRep)
	{
	ZRef<Iterator> theIterator;
	if (iExprRep)
		{
		iExprRep->Accept(*this);
		theIterator.swap(fIterator);
		}
	return theIterator;	
	}

} // namespace ZQE
NAMESPACE_ZOOLIB_END
