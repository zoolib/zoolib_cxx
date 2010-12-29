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

#include "zoolib/ZVisitor_Expr_Bool_ValPred_Any_DoCompare.h"

namespace ZooLib {
namespace Visitor_Expr_Bool_ValPred_Any_DoCompare {

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Bool_ValPred_Any_DoCompare

void Comparer_Bootstrap::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred_Any>& iExpr)
	{ pSetResult(Comparer_ValPred(this, iExpr).Do(fExpr)); } 

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_ValPred_Any_DoCompare::Comparer

void Comparer::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred_Any>& iRep)
	{ pSetResult(-1); } 

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_ValPred_Any_DoCompare::Comparer_GT_ValPred

void Comparer_GT_ValPred::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred_Any>&)
	{ pSetResult(1); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_ValPred_Any_DoCompare::Comparer_ValPred

Comparer_ValPred::Comparer_ValPred(
	Visitor_Expr_Bool_DoCompare::Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_ValPred_Any> iExpr)
:	Comparer(iBootstrap)
,	fExpr(iExpr)
	{}

void Comparer_ValPred::Visit_Expr_Bool_ValPred(const ZRef<ZExpr_Bool_ValPred_Any>& iExpr)
	{ pSetResult(sCompare_T(fExpr->GetValPred(), iExpr->GetValPred())); } 

} // namespace Visitor_Expr_Bool_ValPred_Any_DoCompare
} // namespace ZooLib
