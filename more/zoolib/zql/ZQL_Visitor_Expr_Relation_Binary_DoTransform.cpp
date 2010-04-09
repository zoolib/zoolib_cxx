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

#include "zoolib/zql/ZQL_Visitor_Expr_Relation_Binary_DoTransform.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Relation_Binary_DoTransform

void Visitor_Expr_Relation_Binary_DoTransform::Visit_Expr_Relation_Binary(
	ZRef<Expr_Relation_Binary> iRep)
	{
	ZRef<Expr_Relation> oldLHS = iRep->GetLHS();
	ZRef<Expr_Relation> oldRHS = iRep->GetRHS();
	ZRef<Expr_Relation> newLHS = this->DoTransform(oldLHS).DynamicCast<Expr_Relation>();
	ZRef<Expr_Relation> newRHS = this->DoTransform(oldRHS).DynamicCast<Expr_Relation>();
	if (oldLHS == newLHS && oldRHS == newRHS)
		fResult = iRep;
	else
		fResult = iRep->Clone(newLHS, newRHS);
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
