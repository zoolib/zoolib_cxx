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

#include "zoolib/ZVisitor_Expr_Logic_DoEval.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_DoEval

ZVisitor_Expr_Logic_DoEval::ZVisitor_Expr_Logic_DoEval()
	{}

void ZVisitor_Expr_Logic_DoEval::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep)
	{ this->pSetResult(true); }

void ZVisitor_Expr_Logic_DoEval::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep)
	{ this->pSetResult(false); }

void ZVisitor_Expr_Logic_DoEval::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{ this->pSetResult(! this->Do(iRep)); }

void ZVisitor_Expr_Logic_DoEval::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{ this->pSetResult(this->Do(iRep->GetLHS()) && this->Do(iRep->GetRHS())); }

void ZVisitor_Expr_Logic_DoEval::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{ this->pSetResult(this->Do(iRep->GetLHS()) || this->Do(iRep->GetRHS())); }

NAMESPACE_ZOOLIB_END
