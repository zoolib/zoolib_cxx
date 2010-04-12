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

#include "zoolib/ZVisitor_Expr_Logic_DoToStrim.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVisitor_Expr_Logic_DoToStrim

void ZVisitor_Expr_Logic_DoToStrim::Visit_Expr_Logic_True(ZRef<ZExpr_Logic_True> iRep)
	{ pStrimW() << "any"; }

void ZVisitor_Expr_Logic_DoToStrim::Visit_Expr_Logic_False(ZRef<ZExpr_Logic_False> iRep)
	{ pStrimW() << "none"; }

void ZVisitor_Expr_Logic_DoToStrim::Visit_Expr_Logic_Not(ZRef<ZExpr_Logic_Not> iRep)
	{
	pStrimW() << "~(";
	this->DoToStrim(iRep->GetOp0());
	pStrimW() << ")";
	}

void ZVisitor_Expr_Logic_DoToStrim::Visit_Expr_Logic_And(ZRef<ZExpr_Logic_And> iRep)
	{
	pStrimW() << "(";
	this->DoToStrim(iRep->GetOp0());
	pStrimW() << " & ";
	this->DoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

void ZVisitor_Expr_Logic_DoToStrim::Visit_Expr_Logic_Or(ZRef<ZExpr_Logic_Or> iRep)
	{
	pStrimW() << "(";
	this->DoToStrim(iRep->GetOp0());
	pStrimW() << " | ";
	this->DoToStrim(iRep->GetOp1());
	pStrimW() << ")";
	}

NAMESPACE_ZOOLIB_END
