// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanU_UTF_h__
#define __ZooLib_ChanU_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class UU>
bool sUnread(const ChanU_UTF& iChanU, const std::basic_string<UU>& iString);

} // namespace ZooLib

#endif // __ZooLib_ChanU_UTF_h__
