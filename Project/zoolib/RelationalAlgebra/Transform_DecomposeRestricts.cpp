// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/RelationalAlgebra/Transform_DecomposeRestricts.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/Expr/Util_Expr_Bool_CNF.h"

using std::set;

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - Transform_DecomposeRestricts

void Transform_DecomposeRestricts::Visit_Expr_Rel_Restrict(const ZP<Expr_Rel_Restrict>& iExpr)
	{
	ZP<Expr_Rel> theRel = this->Do(iExpr->GetOp0());

	foreacha (entry, Util_Expr_Bool::sAsCNF(iExpr->GetExpr_Bool()))
		{
		ZP<Expr_Bool> newBool;
		foreacha (entryj, entry)
			newBool |= entryj.Get();
		theRel &= newBool;
		}

	this->pSetResult(theRel);
	}

} // namespace RelationalAlgebra
} // namespace ZooLib
