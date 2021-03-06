// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
#define __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"

#include "zoolib/ValPred/Visitor_Expr_Bool_ValPred_DB_ToStrim.h"

#include "zoolib/QueryEngine/Expr_Rel_Search.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Comment.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dee.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Difference.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Dum.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Intersect.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Project.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"

namespace ZooLib {
namespace RelationalAlgebra {
namespace Util_Strim_Rel {

// =================================================================================================
#pragma mark - RelationalAlgebra::Util_Strim_Rel::Visitor

class Visitor
:	public virtual Visitor_Expr_Bool_ValPred_DB_ToStrim
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Comment
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Dee
,	public virtual Visitor_Expr_Rel_Difference
,	public virtual Visitor_Expr_Rel_Dum
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Intersect
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Project
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Union
,	public virtual QueryEngine::Visitor_Expr_Rel_Search
	{
public:
	virtual void Visit_Expr(const ZP<Expr>& iExpr);
	virtual void Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Comment(const ZP<Expr_Rel_Comment>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Dee(const ZP<Expr_Rel_Dee>& iExpr);
	virtual void Visit_Expr_Rel_Difference(const ZP<Expr_Rel_Difference>& iExpr);
	virtual void Visit_Expr_Rel_Dum(const ZP<Expr_Rel_Dum>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Intersect(const ZP<Expr_Rel_Intersect>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Project(const ZP<Expr_Rel_Project>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr);

	virtual void Visit_Expr_Rel_Search(const ZP<QueryEngine::Expr_Rel_Search>& iExpr);

private:
	void pWriteBinary(const std::string& iFunctionName, const ZP<Expr_Op2_T<Expr_Rel>>& iExpr);
	};

typedef Visitor::Options Options;

void sToStrim(const ChanW_UTF& iStrimW, const ZP<Expr_Rel>& iRel);

void sToStrim_Parseable(const ChanW_UTF& iStrimW, const ZP<Expr_Rel>& iRel);

void sToStrim(const ChanW_UTF& iStrimW, const ZP<Expr_Rel>& iRel, const Options& iOptions);

ZP<Expr_Rel> sFromStrim(const ChanRU_UTF& iChanRU);

ZP<Expr_Rel> sQFromStrim(const ChanRU_UTF& iChanRU);

} // namespace Util_Strim_Rel
} // namespace RelationalAlgebra

const ChanW_UTF& operator<<(const ChanW_UTF& ww, const ZP<RelationalAlgebra::Expr_Rel>& iRel);

} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Util_Strim_Rel_h__
