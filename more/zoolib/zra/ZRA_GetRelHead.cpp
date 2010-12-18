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
#include "zoolib/zra/ZRA_Expr_Rel_Difference.h"
#include "zoolib/zra/ZRA_Expr_Rel_Extend.h"
#include "zoolib/zra/ZRA_Expr_Rel_Intersect.h"
#include "zoolib/zra/ZRA_Expr_Rel_Product.h"
#include "zoolib/zra/ZRA_Expr_Rel_Union.h"
#include "zoolib/zra/ZRA_Expr_Rel_Project.h"
#include "zoolib/zra/ZRA_Expr_Rel_Rename.h"
#include "zoolib/zra/ZRA_Expr_Rel_Restrict_Any.h"
#include "zoolib/zra/ZRA_Expr_Rel_Select.h"
#include "zoolib/zra/ZRA_Expr_Rel_Concrete.h"
#include "zoolib/zra/ZRA_GetRelHead.h"

namespace ZooLib {
namespace ZRA {

namespace { // anonymous
class Visitor_GetRelHead
:	public virtual ZVisitor_Do_T<RelHead>
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Union
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict_Any
,	public virtual Visitor_Expr_Rel_Select
,	public virtual Visitor_Expr_Rel_Extend
,	public virtual Visitor_Expr_Rel_Concrete
	{
	virtual void Visit_Expr_Op1(ZRef<ZExpr_Op1_T<Expr_Rel> > iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

	virtual void Visit_Expr_Op2(ZRef<ZExpr_Op2_T<Expr_Rel> > iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

	virtual void Visit_Expr_Rel_Difference(ZRef<Expr_Rel_Difference> iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

//	virtual void Visit_Expr_Rel_Intersect(ZRef<Expr_Rel_Intersect> iExpr);
	virtual void Visit_Expr_Rel_Product(ZRef<Expr_Rel_Product> iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) & this->Do(iExpr->GetOp1())); }
//	virtual void Visit_Expr_Rel_Union(ZRef<Expr_Rel_Union> iExpr);

	virtual void Visit_Expr_Rel_Extend(ZRef<Expr_Rel_Extend> iExpr)
		{ this->pSetResult(iExpr->GetRelName()); }

	virtual void Visit_Expr_Rel_Project(ZRef<Expr_Rel_Project> iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) & iExpr->GetProjectRelHead()); }

	virtual void Visit_Expr_Rel_Rename(ZRef<Expr_Rel_Rename> iExpr)
		{
		RelHead result = this->Do(iExpr->GetOp0());
		if (result.Contains(iExpr->GetOld()))
			{
			result -= iExpr->GetOld();
			result |= iExpr->GetNew();
			}
		this->pSetResult(result);
		}
//	virtual void Visit_Expr_Rel_Restrict(ZRef<Expr_Rel_Restrict> iExpr);
//	virtual void Visit_Expr_Rel_Select(ZRef<Expr_Rel_Select> iExpr);

	virtual void Visit_Expr_Rel_Concrete(ZRef<Expr_Rel_Concrete> iExpr)
		{ this->pSetResult(iExpr->GetConcreteRelHead()); }
	};

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * sGetRelHead

RelHead sGetRelHead(const ZRef<Expr_Rel>& iRel)
	{ return Visitor_GetRelHead().Do(iRel); }

} // namespace ZRA
} // namespace ZooLib
