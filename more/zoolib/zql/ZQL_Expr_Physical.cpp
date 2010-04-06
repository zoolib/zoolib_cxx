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

#include "zoolib/zql/ZQL_Expr_Physical.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * ExprRep_Physical

ExprRep_Physical::ExprRep_Physical()
	{}

ExprRep_Physical::~ExprRep_Physical()
	{}

bool ExprRep_Physical::Accept(Visitor_ExprRep_Relation& iVisitor)
	{
	if (Visitor_ExprRep_Physical* theVisitor =
		dynamic_cast<Visitor_ExprRep_Physical*>(&iVisitor))
		{
		return this->Accept(*theVisitor);
		}
	else
		{
		return ExprRep_Relation::Accept(iVisitor);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Physical

bool Visitor_ExprRep_Physical::Visit_Physical(ZRef<ExprRep_Physical> iRep)
	{
	return Visitor_ExprRep_Relation::Visit_ExprRep(iRep);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
