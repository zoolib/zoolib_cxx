// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__
#define __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__
#include "zconfig.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - RelationalAlgebra::sTransform_ConsolidateRenames
ZP<Expr_Rel> sTransform_ConsolidateRenames(const ZP<Expr_Rel>& iRel);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_Transform_ConsolidateRenames_h__
