// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/GetRelHead.h"

#include "zoolib/Visitor_Do_T.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Difference.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"

namespace ZooLib {
namespace RelationalAlgebra {

using namespace Util_STL;

// =================================================================================================
#pragma mark - Visitor_GetRelHead (anonymous)

namespace { // anonymous

class Visitor_GetRelHead
:	public virtual Visitor_Do_T<RelHead>
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
	{
	virtual void Visit_Expr_Op1(const ZP<Expr_Op1_T<Expr_Rel>>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

	virtual void Visit_Expr_Op2(const ZP<Expr_Op2_T<Expr_Rel>>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | this->Do(iExpr->GetOp1())); }

	virtual void Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | iExpr->GetColName()); }

	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr)
		{ this->pSetResult(sRelHead(iExpr->GetConcreteHead())); }

	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr)
		{ this->pSetResult(sRelHead(iExpr->GetColName())); }

	virtual void Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0())); }

	virtual void Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) | iExpr->GetColName()); }

	virtual void Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr)
		{ this->pSetResult(this->Do(iExpr->GetOp0()) & iExpr->GetProjectRelHead()); }

	virtual void Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr)
		{
		RelHead result = this->Do(iExpr->GetOp0());
		if (sQErase(result, iExpr->GetOld()))
			result |= iExpr->GetNew();
		this->pSetResult(result);
		}
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - sGetRelHead

RelHead sGetRelHead(const ZP<Expr_Rel>& iRel)
	{ return Visitor_GetRelHead().Do(iRel); }

} // namespace RelationalAlgebra
} // namespace ZooLib
