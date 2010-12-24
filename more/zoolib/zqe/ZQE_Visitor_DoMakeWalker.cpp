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

#include "zoolib/ZExpr_Bool_ValPred_Any.h"
#include "zoolib/zqe/ZQE_Visitor_DoMakeWalker.h"
#include "zoolib/zqe/ZQE_Walker_Calc.h"
#include "zoolib/zqe/ZQE_Walker_Const.h"
#include "zoolib/zqe/ZQE_Walker_Embed.h"
#include "zoolib/zqe/ZQE_Walker_Product.h"
#include "zoolib/zqe/ZQE_Walker_Project.h"
#include "zoolib/zqe/ZQE_Walker_Rename.h"
#include "zoolib/zqe/ZQE_Walker_Select.h"

namespace ZooLib {
namespace ZQE {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_DoMakeWalker

void Visitor_DoMakeWalker::Visit_Expr_Rel_Difference(ZRef<ZRA::Expr_Rel_Difference> iExpr)
	{ ZUnimplemented(); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Intersect(ZRef<ZRA::Expr_Rel_Intersect> iExpr)
	{ ZUnimplemented(); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Embed(ZRef<ZRA::Expr_Rel_Embed> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZRef<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Embed(op0, iExpr->GetRelName(), op1));
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Product(ZRef<ZRA::Expr_Rel_Product> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		{
		if (ZRef<Walker> op1 = this->Do(iExpr->GetOp1()))
			this->pSetResult(new Walker_Product(op0, op1));
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Union(ZRef<ZRA::Expr_Rel_Union> iExpr)
	{ ZUnimplemented(); }

void Visitor_DoMakeWalker::Visit_Expr_Rel_Calc(ZRef<ZRA::Expr_Rel_Calc> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Calc(op0, iExpr->GetRelName(), iExpr->GetCallable()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Const(ZRef<ZRA::Expr_Rel_Const> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Const(op0, iExpr->GetRelName(), iExpr->GetVal()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Project(ZRef<ZRA::Expr_Rel_Project> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Project(op0, iExpr->GetProjectRelHead()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Rename(ZRef<ZRA::Expr_Rel_Rename> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Rename(op0, iExpr->GetNew(), iExpr->GetOld()));
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Restrict(ZRef<ZRA::Expr_Rel_Restrict_Any> iExpr)
	{
	if (ZRef<Walker> theWalker = this->Do(iExpr->GetOp0()))
		{
		this->pSetResult(
			new Walker_Select(theWalker, new ZExpr_Bool_ValPred_Any(iExpr->GetValPred())));	
		}
	}

void Visitor_DoMakeWalker::Visit_Expr_Rel_Select(ZRef<ZRA::Expr_Rel_Select> iExpr)
	{
	if (ZRef<Walker> op0 = this->Do(iExpr->GetOp0()))
		this->pSetResult(new Walker_Select(op0, iExpr->GetExpr_Bool()));
	}

} // namespace ZQE
} // namespace ZooLib
