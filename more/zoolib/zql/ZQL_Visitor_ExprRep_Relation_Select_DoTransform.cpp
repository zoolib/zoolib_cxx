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

#include "zoolib/zql/ZQL_Visitor_ExprRep_Relation_Select_DoTransform.h"

NAMESPACE_ZOOLIB_BEGIN
namespace ZQL {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_ExprRep_Relation_Select_DoTransform

bool Visitor_ExprRep_Relation_Select_DoTransform::Visit_ExprRep_Relation_Select(
	ZRef<ExprRep_Relation_Select> iRep)
	{
	ZRef<ZExprRep_Logic> oldLogical = iRep->GetExprRep_Logic();
	ZRef<ExprRep_Relation> oldRelation = iRep->GetExprRep_Relation();
	ZRef<ZExprRep_Logic> newLogical = this->DoTransform(oldLogical).DynamicCast<ZExprRep_Logic>();
	ZRef<ExprRep_Relation> newRelation =
		this->DoTransform(oldRelation).DynamicCast<ExprRep_Relation>();

	if (oldLogical == newLogical && oldRelation == newRelation)
		fResult = iRep;
	else
		fResult = new ExprRep_Relation_Select(newLogical, newRelation);

	return true;
	}

} // namespace ZQL
NAMESPACE_ZOOLIB_END
