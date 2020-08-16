// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_QueryEngine_Transform_Search_h__
#define __ZooLib_QueryEngine_Transform_Search_h__ 1
#include "zconfig.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace QueryEngine {

// =================================================================================================
#pragma mark - sTransform_Search

ZP<RelationalAlgebra::Expr_Rel> sTransform_Search(const ZP<RelationalAlgebra::Expr_Rel>& iExpr);

} // namespace QueryEngine
} // namespace ZooLib

#endif // __ZooLib_QueryEngine_Transform_Search_h__
