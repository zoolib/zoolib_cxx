// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Channer_h__
#define __ZooLib_Util_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/ChanRPos_XX_ChanR.h"

// =================================================================================================
#pragma mark -

namespace ZooLib {

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX_ChannerR(
	const ZP<ChannerR<EE>>& iChannerR,
	const ZP<ChannerRWPos<EE>>& iChannerRWPos)
	{
	if (iChannerR && iChannerRWPos)
		{
		typedef std::tuple<ZP<Counted>,ZP<Counted>> Holder_t;

		return new Channer_Holder_T<Holder_t,ChanRPos_XX_ChanR<EE>>
			(Holder_t(iChannerR,iChannerRWPos), *iChannerR, *iChannerRWPos);
		}
	return null;
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Channer_h__
