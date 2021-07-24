// Copyright (c) 2021 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Util_Channer_h__
#define __ZooLib_Util_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"
#include "zoolib/ChanRPos_XX_ChanR.h"
#include "zoolib/ChanRPos_XX_ChanRSize.h"
#include "zoolib/ChanRWPos_XX_RAM.h"

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

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX_ChannerRSize(
	const ZP<ChannerRSize<EE>>& iChannerRSize,
	const ZP<ChannerRWPos<EE>>& iChannerRWPos)
	{
	if (iChannerRSize && iChannerRWPos)
		{
		typedef std::tuple<ZP<Counted>,ZP<Counted>> Holder_t;

		return new Channer_Holder_T<Holder_t,ChanRPos_XX_ChanRSize<EE>>
			(Holder_t(iChannerRSize,iChannerRWPos), *iChannerRSize, *iChannerRWPos);
		}
	return null;
	}

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX(const ZP<ChannerRPos<EE>>& iChannerRPos)
	{ return iChannerRPos; }

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX(const ZP<ChannerRSize<EE>>& iChannerRSize)
	{
	if (not iChannerRSize)
		return null;

	if (ZP<ChannerRPos<EE>> asRPos = iChannerRSize.template DynamicCast<ChannerRPos<EE>>())
		return asRPos;

	return sChannerRPos_XX_ChannerRSize<EE>(iChannerRSize, sChanner_T<ChanRWPos_XX_RAM<EE>>());
	}

template <class EE>
ZP<ChannerRPos<EE>> sChannerRPos_XX(const ZP<ChannerR<EE>>& iChannerR)
	{
	if (not iChannerR)
		return null;

	if (ZP<ChannerRPos<EE>> asRPos = iChannerR.template DynamicCast<ChannerRPos<EE>>())
		return asRPos;

	if (ZP<ChannerRSize<EE>> asRSize = iChannerR.template DynamicCast<ChannerRSize<EE>>())
		return sChannerRPos_XX_ChannerRSize<EE>(asRSize, sChanner_T<ChanRWPos_XX_RAM<EE>>());

	return sChannerRPos_XX_ChannerR<EE>(iChannerR, sChanner_T<ChanRWPos_XX_RAM<EE>>());
	}

} // namespace ZooLib

#endif // __ZooLib_Util_Channer_h__
