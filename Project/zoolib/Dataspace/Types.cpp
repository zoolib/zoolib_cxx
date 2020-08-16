// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Types.h"

#include "zoolib/Compare.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - sCompare_T

template <>
int sCompare_T(const Dataspace::AbsentOptional_t& iL,
	const Dataspace::AbsentOptional_t& iR)
	{ return 0; }

ZMACRO_CompareRegistration_T(Dataspace::AbsentOptional_t)

namespace Dataspace {

} // namespace Dataspace
} // namespace ZooLib
