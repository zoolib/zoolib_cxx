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

#include "zoolib/ZExpr_ValCondition.h"
#include "zoolib/ZExpr_ValCondition_Priv.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Explicit instantiations

template class ZExprRep_ValCondition_T<ZVal_Expr>;

template class ZVisitor_ExprRep_ValCondition_T<ZVal_Expr>;

template
ZExpr_Logical operator<(
	const ZValComparand_T<ZVal_Expr>& iLHS, const ZValComparand_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator<=(
	const ZValComparand_T<ZVal_Expr>& iLHS, const ZValComparand_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator==(
	const ZValComparand_T<ZVal_Expr>& iLHS, const ZValComparand_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator>=(
	const ZValComparand_T<ZVal_Expr>& iLHS, const ZValComparand_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator>(
	const ZValComparand_T<ZVal_Expr>& iLHS, const ZValComparand_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator&(
	const ZValCondition_T<ZVal_Expr>& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator&(const ZValCondition_T<ZVal_Expr>& iLHS, const ZExpr_Logical& iRHS);

template
ZExpr_Logical operator&(const ZExpr_Logical& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical& operator&=(ZExpr_Logical& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator|(
	const ZValCondition_T<ZVal_Expr>& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical operator|(const ZValCondition_T<ZVal_Expr>& iLHS, const ZExpr_Logical& iRHS);

template
ZExpr_Logical operator|(const ZExpr_Logical& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

template
ZExpr_Logical& operator|=(ZExpr_Logical& iLHS, const ZValCondition_T<ZVal_Expr>& iRHS);

ZRelHead sGetRelHead(const ZRef<ZExprRep_Logical>& iRep)
	{ return sGetRelHead_T<ZVal_Expr>(iRep); }

bool sMatches(const ZRef<ZExprRep_Logical>& iRep, const ZVal_Expr& iVal)
	{ return sMatches_T<ZVal_Expr>(iRep, iVal); }

NAMESPACE_ZOOLIB_END
