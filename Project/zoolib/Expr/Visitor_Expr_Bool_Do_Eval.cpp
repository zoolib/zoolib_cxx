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

#include "zoolib/Expr/Visitor_Expr_Bool_Do_Eval.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Visitor_Expr_Bool_Do_Eval

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_True(const ZRef<Expr_Bool_True>& iRep)
	{ this->pSetResult(true); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_False(const ZRef<Expr_Bool_False>& iRep)
	{ this->pSetResult(false); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_Not(const ZRef<Expr_Bool_Not>& iRep)
	{ this->pSetResult(not this->Do(iRep)); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_And(const ZRef<Expr_Bool_And>& iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) && this->Do(iRep->GetOp1())); }

void Visitor_Expr_Bool_Do_Eval::Visit_Expr_Bool_Or(const ZRef<Expr_Bool_Or>& iRep)
	{ this->pSetResult(this->Do(iRep->GetOp0()) || this->Do(iRep->GetOp1())); }

} // namespace ZooLib
