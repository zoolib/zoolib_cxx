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

#include "zoolib/ZVisitor_ExprRep_Logic_Eval.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_ExprRep_Logic_Eval

bool ZVisitor_ExprRep_Logic_Eval::Visit_Logic_True(ZRef<ZExprRep_Logic_True> iRep)
	{ return true; }

bool ZVisitor_ExprRep_Logic_Eval::Visit_Logic_False(ZRef<ZExprRep_Logic_False> iRep)
	{ return false; }

bool ZVisitor_ExprRep_Logic_Eval::Visit_Logic_Not(ZRef<ZExprRep_Logic_Not> iRep)
	{ return ! iRep->Accept(*this); }

bool ZVisitor_ExprRep_Logic_Eval::Visit_Logic_And(ZRef<ZExprRep_Logic_And> iRep)
	{ return iRep->GetLHS()->Accept(*this) && iRep->GetRHS()->Accept(*this); }

bool ZVisitor_ExprRep_Logic_Eval::Visit_Logic_Or(ZRef<ZExprRep_Logic_Or> iRep)
	{ return iRep->GetLHS()->Accept(*this) || iRep->GetRHS()->Accept(*this); }

NAMESPACE_ZOOLIB_END
