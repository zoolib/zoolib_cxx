// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanU_h__
#define __ZooLib_ChanU_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class EE>
bool sUnread(const ChanU<EE>& iChanU, const EE& iElmt)
	{ return 1 == sUnread(iChanU, &iElmt, 1); }

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
