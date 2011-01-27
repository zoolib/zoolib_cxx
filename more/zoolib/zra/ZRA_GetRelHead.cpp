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

#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/zra/ZRA_Expr_Rel_Calc.h"
#include "zoolib/zra/ZRA_Expr_Rel_Const.h"
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Embed.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_GetRelHead.h"

namespace ZooLib {
namespace ZRA {

// =================================================================================================
#pragma mark -
#pragma mark * Visitor_GetRelHead (anonymous)

namespace { // anonymous

class Visitor_GetRelHead
:	public virtual ZVisitor_Do_T<RelHead>
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
	{
	virtual void Visit_Expr_Op2(const ZRef<ZExpr_Op2_T<Expr_Rel> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

	virtual void Visit_Expr_Op1(const ZRef<ZExpr_Op1_T<Expr_Rel> >& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

	virtual void Visit_Expr_Rel_Difference(const ZRef<Expr_Rel_Difference>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

	virtual void Visit_Expr_Rel_Embed(const ZRef<Expr_Rel_Embed>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | iExpr->GetRelName()); }

	virtual void Visit_Expr_Rel_Project(const ZRef<Expr_Rel_Project>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) & iExpr->GetProjectRelHead()); }

	virtual void Visit_Expr_Rel_Rename(const ZRef<Expr_Rel_Rename>& iExpr)
		{
		RelHead result = this->Do(iExpr->GetOp0());
		if (ZUtil_STL::sEraseIfContains(result, iExpr->GetOld()))
			result |= iExpr->GetNew();
		this->pSetResult(result);
		}

	virtual void Visit_Expr_Rel_Calc(const ZRef<Expr_Rel_Calc>& iExpr)
		{ this->pSetResult(iExpr->GetRelName()); }

	virtual void Visit_Expr_Rel_Concrete(const ZRef<Expr_Rel_Concrete>& iExpr)
		{ this->pSetResult(iExpr->GetConcreteRelHead()); }

	virtual void Visit_Expr_Rel_Const(const ZRef<Expr_Rel_Const>& iExpr)
		{ this->pSetResult(iExpr->GetRelName()); }
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * sGetRelHead

RelHead sGetRelHead(const ZRef<Expr_Rel>& iRel)
	{ return Visitor_GetRelHead().Do(iRel); }

} // namespace ZRA
} // namespace ZooLib
