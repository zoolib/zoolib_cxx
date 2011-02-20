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

#include "zoolib/ZExpr_Bool_ValPred.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"
#include "zoolib/zqe/ZQE_Walker_Calc.h"
#include "zoolib/zqe/ZQE_Walker_Const.h"
#include "zoolib/zqe/ZQE_Walker_Dee.h"
#include "zoolib/zqe/ZQE_Walker_Embed.h"
#include "zoolib/zqe/ZQE_Walker_Product.h"
#include "zoolib/zqe/ZQE_Walker_Project.h"
#include "zoolib/zqe/ZQE_Walker_Rename.h"
#include "zoolib/zqe/ZQE_Walker_Restrict.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

void Visitor_DoMakeWalker::Visit(const ZRef<ZVisitee>& iRep)
	{ ZUnimplemented(); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Calc(const ZRef<ZRA::Expr_Rel_Calc>& iExpr)
	{
	this->pSetResult(
		new Walker_Calc(iExpr->GetRelName(), iExpr->GetBindings(), iExpr->GetCallable()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Const(const ZRef<ZRA::Expr_Rel_Const>& iExpr)
	{ this->pSetResult(new Walker_Const(iExpr->GetRelName(), iExpr->GetVal())); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Dee(const ZRef<ZRA::Expr_Rel_Dee>& iExpr)
	{ this->pSetResult(new Walker_Dee); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Embed(const ZRef<ZRA::Expr_Rel_Embed>& iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Embed(iExpr->GetRelName(), iExpr->GetBindings(), op0));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Product(const ZRef<ZRA::Expr_Rel_Product>& iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZRef<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Product(op0, op1));
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Project(const ZRef<ZRA::Expr_Rel_Project>& iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Project(op0, iExpr->GetProjectRelHead()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Rename(const ZRef<ZRA::Expr_Rel_Rename>& iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Rename(op0, iExpr->GetNew(), iExpr->GetOld()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Restrict(const ZRef<ZRA::Expr_Rel_Restrict>& iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Restrict(op0, iExpr->GetExpr_Bool()));
	}

} // namespace ZQE
} // namespace ZooLib
