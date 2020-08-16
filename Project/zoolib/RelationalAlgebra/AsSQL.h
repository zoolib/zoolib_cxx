// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_RelationalAlgebra_AsSQL_h__
#define __ZooLib_RelationalAlgebra_AsSQL_h__
#include "zconfig.h"

#include "zoolib/ChanW_UTF.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

#include <map>

namespace ZooLib {
namespace RelationalAlgebra {

// =================================================================================================
#pragma mark -

bool sWriteAsSQL(const std::map<string8,RelHead>& iTables, ZP<Expr_Rel> iRel, const ChanW_UTF& w);

} // namespace RelationalAlgebra
} // namespace ZooLib

#endif // __ZooLib_RelationalAlgebra_AsSQL_h__
