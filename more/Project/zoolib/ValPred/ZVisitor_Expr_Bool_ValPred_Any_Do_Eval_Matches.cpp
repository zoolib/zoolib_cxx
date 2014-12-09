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

#include "zoolib/ValPred/ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches.h"
#include "zoolib/ValPred/ZValPred_Any.h"

using std::set;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark *

ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches::
ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches(const ZVal_Any& iVal)
:	fVal(iVal)
	{}

void ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches::Visit_Expr_Bool_ValPred(
	const ZRef<ZExpr_Bool_ValPred>& iExpr)
	{ this->pSetResult(sMatches(iExpr->GetValPred(), fVal)); }

bool sMatches(const ZRef<ZExpr_Bool>& iExpr, const ZVal_Any& iVal)
	{ return ZVisitor_Expr_Bool_ValPred_Any_Do_Eval_Matches(iVal).Do(iExpr); }

} // namespace ZooLib
