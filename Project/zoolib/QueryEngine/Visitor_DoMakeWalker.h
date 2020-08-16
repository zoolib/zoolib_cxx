// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__
#define __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__ 1
#include "zconfig.h"

#include "zoolib/Visitor_Do_T.h"

#include "zoolib/QueryEngine/Walker.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - Visitor_DoMakeWalker

class Visitor_DoMakeWalker
:	public virtual Visitor_Do_T<ZP<Walker>>
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Calc
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Const
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Comment
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Dee
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Dum
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Embed
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Product
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Project
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Rename
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Restrict
,	public virtual RelationalAlgebra::Visitor_Expr_Rel_Union
	{
public:
// From Visitor
	virtual void Visit(const ZP<Visitee>& iRep);

// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Calc(const ZP<RelationalAlgebra::Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Comment(const ZP<RelationalAlgebra::Expr_Rel_Comment>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<RelationalAlgebra::Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZP<RelationalAlgebra::Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Dum(const ZP<RelationalAlgebra::Expr_Rel_Dum>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZP<RelationalAlgebra::Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<RelationalAlgebra::Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZP<RelationalAlgebra::Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<RelationalAlgebra::Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<RelationalAlgebra::Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZP<RelationalAlgebra::Expr_Rel_Union>& iExpr);
	};

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Visitor_DoMakeWalker_h__
