// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Transform_PushDownRestricts_h__
#define __ZooLib_RelationalAlgebra_Transform_PushDownRestricts_h__
#include "zconfig.h"

#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"

#include "zoolib/RelationalAlgebra/Expr_Rel_Calc.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Const.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Embed.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Product.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Rename.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Union.h"

#include <vector>

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - RelationalAlgebra::Transform_PushDownRestricts

class Transform_PushDownRestricts
:	public virtual Visitor_Expr_Op_Do_Transform_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Calc
,	public virtual Visitor_Expr_Rel_Concrete
,	public virtual Visitor_Expr_Rel_Const
,	public virtual Visitor_Expr_Rel_Embed
,	public virtual Visitor_Expr_Rel_Product
,	public virtual Visitor_Expr_Rel_Rename
,	public virtual Visitor_Expr_Rel_Restrict
,	public virtual Visitor_Expr_Rel_Union
	{
public:
// From Visitor_Expr_Rel_XXX
	virtual void Visit_Expr_Rel_Calc(const ZP<Expr_Rel_Calc>& iExpr);
	virtual void Visit_Expr_Rel_Concrete(const ZP<Expr_Rel_Concrete>& iExpr);
	virtual void Visit_Expr_Rel_Const(const ZP<Expr_Rel_Const>& iExpr);
	virtual void Visit_Expr_Rel_Embed(const ZP<Expr_Rel_Embed>& iExpr);
	virtual void Visit_Expr_Rel_Product(const ZP<Expr_Rel_Product>& iExpr);
	virtual void Visit_Expr_Rel_Rename(const ZP<Expr_Rel_Rename>& iExpr);
	virtual void Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr);
	virtual void Visit_Expr_Rel_Union(const ZP<Expr_Rel_Union>& iExpr);

protected:
	void pHandleIt(const RelHead& iRH, const ZP<Expr_Rel>& iRel);

	struct Restrict
		{
		ZP<Expr_Bool> fExpr_Bool;
		RelHead fNames;
		size_t fCountTouching;
		size_t fCountSubsuming;
		};
	std::vector<Restrict*> fRestricts;
	RelHead fRelHead;
	};

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Transform_PushDownRestricts_h__
