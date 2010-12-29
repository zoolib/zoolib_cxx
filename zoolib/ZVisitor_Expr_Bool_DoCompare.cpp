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

#include "zoolib/ZSetRestore_T.h"
#include "zoolib/ZVisitor_Expr_Bool_DoCompare.h"

namespace ZooLib {
namespace Visitor_Expr_Bool_DoCompare {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_DoCompare::Comparer_Bootstrap

Comparer_Bootstrap::Comparer_Bootstrap()
	{}

int Comparer_Bootstrap::Compare(ZRef<ZExpr_Bool> iLHS, ZRef<ZExpr_Bool> iRHS)
	{
	ZSetRestore_T<ZRef<ZExpr_Bool> > sr(fExpr, iLHS);
	return this->Do(iRHS);
	}

void Comparer_Bootstrap::Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>&)
	{ pSetResult(Comparer_True(this).Do(fExpr)); } 
	
void Comparer_Bootstrap::Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>&)
	{ pSetResult(Comparer_False(this).Do(fExpr)); }

void Comparer_Bootstrap::Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>& iExpr)
	{ pSetResult(Comparer_Not(this, iExpr).Do(fExpr)); } 
	
void Comparer_Bootstrap::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iExpr)
	{ pSetResult(Comparer_And(this, iExpr).Do(fExpr)); } 
	
void Comparer_Bootstrap::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iExpr)
	{ pSetResult(Comparer_Or(this, iExpr).Do(fExpr)); } 

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_DoCompare::Comparer_Bootstrap

Comparer::Comparer()
:	fBootstrap(nullptr)
	{ ZUnimplemented(); }

Comparer::Comparer(Comparer_Bootstrap* iBootstrap)
:	fBootstrap(iBootstrap)
	{}

void Comparer::Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>&)
	{ pSetResult(-1); }

void Comparer::Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>&)
	{ pSetResult(-1); }

void Comparer::Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>&)
	{ pSetResult(-1); }

void Comparer::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>&)
	{ pSetResult(-1); }

void Comparer::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>&)
	{ pSetResult(-1); }

int Comparer::CompareUnary(const ZRef<ZExpr_Op1_T<ZExpr_Bool> >& iLHS,
	const ZRef<ZExpr_Op1_T<ZExpr_Bool> >& iRHS)
	{ return fBootstrap->Compare(iLHS->GetOp0(), iRHS->GetOp0()); }

int Comparer::CompareBinary(const ZRef<ZExpr_Op2_T<ZExpr_Bool> >& iLHS,
	const ZRef<ZExpr_Op2_T<ZExpr_Bool> >& iRHS)
	{
	if (int compare = fBootstrap->Compare(iLHS->GetOp0(), iRHS->GetOp0()))
		return compare;
	return fBootstrap->Compare(iLHS->GetOp1(), iRHS->GetOp1());
	}

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_DoCompare::Comparer_GT_XXX

void Comparer_GT_True::Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>&)
	{ pSetResult(1); }

void Comparer_GT_False::Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>&)
	{ pSetResult(1); }

void Comparer_GT_Not::Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>&)
	{ pSetResult(1); }

void Comparer_GT_And::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>&)
	{ pSetResult(1); }

void Comparer_GT_Or::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>&)
	{ pSetResult(1); }

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_Expr_Bool_DoCompare::Comparer_XXX

Comparer_True::Comparer_True(Comparer_Bootstrap* iBootstrap)
:	Comparer(iBootstrap)
	{}

void Comparer_True::Visit_Expr_Bool_True(const ZRef<ZExpr_Bool_True>&)
	{ pSetResult(0); }

Comparer_False::Comparer_False(Comparer_Bootstrap* iBootstrap)
:	Comparer(iBootstrap)
	{}

void Comparer_False::Visit_Expr_Bool_False(const ZRef<ZExpr_Bool_False>&)
	{ pSetResult(0); }

Comparer_Not::Comparer_Not(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_Not> iExpr)
:	Comparer(iBootstrap)
,	fExpr(iExpr)
	{}

void Comparer_Not::Visit_Expr_Bool_Not(const ZRef<ZExpr_Bool_Not>& iExpr)
	{ pSetResult(CompareUnary(fExpr, iExpr)); } 

Comparer_And::Comparer_And(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_And> iExpr)
:	Comparer(iBootstrap)
,	fExpr(iExpr)
	{}

void Comparer_And::Visit_Expr_Bool_And(const ZRef<ZExpr_Bool_And>& iExpr)
	{ pSetResult(CompareBinary(fExpr, iExpr)); } 

Comparer_Or::Comparer_Or(Comparer_Bootstrap* iBootstrap, ZRef<ZExpr_Bool_Or> iExpr)
:	Comparer(iBootstrap)
,	fExpr(iExpr)
	{}

void Comparer_Or::Visit_Expr_Bool_Or(const ZRef<ZExpr_Bool_Or>& iExpr)
	{ pSetResult(CompareBinary(fExpr, iExpr)); } 

} // namespace Visitor_Expr_Bool_DoCompare
} // namespace ZooLib
