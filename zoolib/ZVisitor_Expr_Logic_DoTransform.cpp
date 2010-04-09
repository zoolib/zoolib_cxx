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

#include "zoolib/ZVisitor_Expr_Logic_DoTransform.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_DoTransform

void ZVisitor_Expr_Logic_DoTransform::Visit_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{
	ZRef<ZExpr_Logic> oldRep = iRep->GetOperand();
	ZRef<ZExpr_Logic> newRep = this->DoTransform(oldRep).DynamicCast<ZExpr_Logic>();
	if (oldRep == newRep)
		fResult = iRep;
	else
		fResult = new ZExpr_Logic_Not(newRep);
	}

void ZVisitor_Expr_Logic_DoTransform::Visit_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{
	ZRef<ZExpr_Logic> oldLHS = iRep->GetLHS();
	ZRef<ZExpr_Logic> oldRHS = iRep->GetRHS();
	ZRef<ZExpr_Logic> newLHS = this->DoTransform(oldLHS).DynamicCast<ZExpr_Logic>();
	ZRef<ZExpr_Logic> newRHS = this->DoTransform(oldRHS).DynamicCast<ZExpr_Logic>();
	if (oldLHS == newLHS && oldRHS == newRHS)
		fResult = iRep;
	else
		fResult = new ZExpr_Logic_And(newLHS, newRHS);
	}

void ZVisitor_Expr_Logic_DoTransform::Visit_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{
	ZRef<ZExpr_Logic> oldLHS = iRep->GetLHS();
	ZRef<ZExpr_Logic> oldRHS = iRep->GetRHS();
	ZRef<ZExpr_Logic> newLHS = this->DoTransform(oldLHS).DynamicCast<ZExpr_Logic>();
	ZRef<ZExpr_Logic> newRHS = this->DoTransform(oldRHS).DynamicCast<ZExpr_Logic>();
	if (oldLHS == newLHS && oldRHS == newRHS)
		fResult = iRep;
	else
		fResult = new ZExpr_Logic_Or(newLHS, newRHS);
	}

NAMESPACE_ZOOLIB_END
