// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_GetRelHead_h__
#define __ZooLib_RelationalAlgebra_GetRelHead_h__ 1
#include "zconfig.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark - sGetRelHead

RelHead sGetRelHead(const ZP<Expr_Rel>& iRel);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_GetRelHead_h__
