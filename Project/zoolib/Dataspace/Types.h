// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Dataspace_Types_h__
#define __ZooLib_Dataspace_Types_h__ 1
#include "zconfig.h"
#include "zoolib/Compare_T.h"

#include "zoolib/QueryEngine/Result.h"

#include "zoolib/RelationalAlgebra/Expr_Rel.h"
#include "zoolib/RelationalAlgebra/RelHead.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {

namespace DataspaceTypes {

using QueryEngine::Result;
using QueryEngine::ResultDeltas;

using RelationalAlgebra::Expr_Rel;

using RelationalAlgebra::ColName;

using RelationalAlgebra::ConcreteHead;
using RelationalAlgebra::sConcreteHead;

using RelationalAlgebra::RelHead;
using RelationalAlgebra::sRelHead;

struct AbsentOptional_t {};

} // namespace DataspaceTypes

namespace Dataspace {

using namespace DataspaceTypes;

} // namespace Dataspace

template <>
inline int sCompare_T(const DataspaceTypes::AbsentOptional_t&,
	const DataspaceTypes::AbsentOptional_t&)
	{ return 0; }

} // namespace ZooLib

#endif // __ZooLib_Dataspace_Types_h__
