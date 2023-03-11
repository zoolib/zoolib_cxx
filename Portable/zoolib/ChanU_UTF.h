// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_ChanU_UTF_h__
#define __ZooLib_ChanU_UTF_h__ 1
#include "zconfig.h"

#include "zoolib/Chan_UTF.h"
#include "zoolib/ChanU.h"
#include "zoolib/Unicode.h"

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class UU>
bool sUnread(const ChanU_UTF& iChanU, const std::basic_string<UU>& iString)
	{
	for (typename std::basic_string<UU>::const_iterator iterBegin = iString.cbegin(),
		iterEnd = iString.cend(),
		iter = iterEnd;
		/*no test*/; /*no inc*/)
		{
		UTF32 theCP;
		if (not Unicode::sDecRead(iterBegin, iter, iterEnd, theCP))
			return true;
		if (not sUnread(iChanU, theCP))
			return false;
		}
	}

} // namespace ZooLib

#endif // __ZooLib_ChanU_UTF_h__
