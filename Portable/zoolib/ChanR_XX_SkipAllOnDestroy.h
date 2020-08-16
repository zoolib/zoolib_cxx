// Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
// MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanR_XX_SkipAllOnDestroy_h__
#define __ZooLib_ChanR_XX_SkipAllOnDestroy_h__ 1
#include "zconfig.h"

#include "zoolib/ChanFilter.h"
#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - ChanR_XX_SkipAllOnDestroy

/// A read filter stream that when destroyed invokes SkipAll on its real stream.

template <class Chan_p>
class ChanR_XX_SkipAllOnDestroy
:	public ChanFilter<Chan_p>
	{
	typedef ChanFilter<Chan_p> inherited;
public:
	ChanR_XX_SkipAllOnDestroy(const Chan_p& iChan)
	:	inherited(iChan)
		{}

	~ChanR_XX_SkipAllOnDestroy()
		{ sSkipAll(inherited::pGetChan()); }
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_SkipAllOnDestroy_h__
