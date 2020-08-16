// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_UTCDateTime_h__
#define __ZooLib_UTCDateTime_h__ 1
#include "zconfig.h"

#include "zoolib/TagVal.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - UTCDateTime

typedef TagVal<double, struct Tag_UTCDateTime> UTCDateTime;

} // namespace ZooLib

#endif // __ZooLib_UTCDateTime_h__
