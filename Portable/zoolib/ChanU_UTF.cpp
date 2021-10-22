// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/ChanU_UTF.h"
#include "zoolib/ChanU.h"
#include "zoolib/Unicode.h"

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

template
bool sUnread<UTF32>(const ChanU_UTF& iChanU, const std::basic_string<UTF32>& iString);

template
bool sUnread<UTF16>(const ChanU_UTF& iChanU, const std::basic_string<UTF16>& iString);

template
bool sUnread<UTF8>(const ChanU_UTF& iChanU, const std::basic_string<UTF8>& iString);

} // namespace ZooLib
