// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UUID_h__
#define __ZooLib_UUID_h__ 1
#include "zconfig.h"

#include "zoolib/StdInt.h" // For byte
#include "zoolib/TagVal.h"

#include <array>

namespace ZooLib {

// =================================================================================================
#pragma mark - UUID

typedef TagVal<std::array<byte,16>, struct Tag_UUID> UUID;

} // namespace ZooLib

#endif // __ZooLib_UUID_h__
