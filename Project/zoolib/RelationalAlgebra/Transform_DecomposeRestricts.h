// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Transform_DecomposeRestricts_h__
#define __ZooLib_RelationalAlgebra_Transform_DecomposeRestricts_h__
#include "zconfig.h"

#include "zoolib/Expr/Visitor_Expr_Op_Do_Transform_T.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Restrict.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - RelationalAlgebra::Transform_DecomposeRestricts

class Transform_DecomposeRestricts
:	public virtual Visitor_Expr_Op_Do_Transform_T<Expr_Rel>
,	public virtual Visitor_Expr_Rel_Restrict
	{
public:
	virtual void Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr);
	};

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Transform_DecomposeRestricts_h__
