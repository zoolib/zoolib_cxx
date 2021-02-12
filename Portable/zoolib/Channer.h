// Copyright (c) 2014 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Channer_h__
#define __ZooLib_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/Counted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Channer

template <class DF> using Channer
	= typename AsDeriveFrom<
		typename TypeListPrefixed<
			Counted,
			typename DF::AsTypeList_t
			>::Result_t
		>::Result_t;

// =================================================================================================
#pragma mark - ChanOfChanner

template <class Channer_p> using ChanOfChanner =
	typename ZooLib::AsDeriveFrom<
		typename TypeListWithoutPrefix<typename Channer_p::AsTypeList_t>::Result_t>::Result_t;

// =================================================================================================
#pragma mark - Basic channer typedefs

using ChannerAbort = Channer<ChanAbort>;
using ChannerDisconnectRead = Channer<ChanDisconnectRead>;
using ChannerDisconnectWrite = Channer<ChanDisconnectWrite>;
using ChannerPos = Channer<ChanPos>;
template <class EE> using ChannerR = Channer<ChanR<EE>>;
using ChannerSize = Channer<ChanSize>;
using ChannerSizeSet = Channer<ChanSizeSet>;
template <class EE> using ChannerU = Channer<ChanU<EE>>;
template <class EE> using ChannerW = Channer<ChanW<EE>>;

// =================================================================================================
#pragma mark - Composite channer typedefs

template <class EE> using ChannerRU = Channer<ChanRU<EE>>;
template <class EE> using ChannerRPos = Channer<ChanRPos<EE>>;
template <class EE> using ChannerWPos = Channer<ChanWPos<EE>>;
template <class EE> using ChannerRWPos = Channer<ChanRWPos<EE>>;
template <class EE> using ChannerRW = Channer<ChanRW<EE>>;

using ChannerClose = Channer<ChanClose>;

template <class EE> using ChannerRAbort = Channer<ChanRAbort<EE>>;
template <class EE> using ChannerWAbort = Channer<ChanWAbort<EE>>;
template <class EE> using ChannerRWAbort = Channer<ChanRWAbort<EE>>;

template <class EE> using ChannerRCon = Channer<ChanRCon<EE>>;
template <class EE> using ChannerWCon = Channer<ChanWCon<EE>>;
template <class EE> using ChannerRWCon = Channer<ChanRWCon<EE>>;

template <class EE> using ChannerConnection = ChannerRWCon<EE>;

// This name is still in use
template <class EE> using ChannerRWClose = ChannerRWCon<EE>;

// =================================================================================================
#pragma mark - sChanner_Chan

// If iChan is actually a Channer, return a ZP to that channer.
template <class Chan_p>
ZP<Channer<Chan_p>> sChanner_Chan(const Chan_p& iChan)
	{ return dynamic_cast<Channer<Chan_p>*>(sNonConst(&iChan)); }

// Assert that Chan be a Channer and return it.
template <class Chan_p>
ZP<Channer<Chan_p>> sAChanner_Chan(const Chan_p& iChan)
	{
	ZP<Channer<Chan_p>> result = sChanner_Chan<Chan_p>(iChan);
	ZAssert(result);
	return result;
	}

// =================================================================================================
#pragma mark - Channer_T

template <class Chan_p>
class Channer_T
:	public Chan_p
,	public Channer<typename AsDeriveFrom<typename Chan_p::AsTypeList_t>::Result_t>
	{
public:
	Channer_T() : Chan_p() {}

	template <typename... Args_p>
	Channer_T(Args_p&&... args) : Chan_p(std::forward<Args_p>(args)...) {}
	};

template <class Chan_p>
ZP<Channer_T<Chan_p>> sChanner_T()
	{ return new Channer_T<Chan_p>(); }

template <class Chan_p, typename... Args_p>
ZP<Channer_T<Chan_p>> sChanner_T(Args_p&&... args)
	{ return new Channer_T<Chan_p>(std::forward<Args_p>(args)...); }

// =================================================================================================
#pragma mark - Channer_Holder_T

template <class Holder_p, class Chan_p>
class Channer_Holder_T
:	private Holder_p
,	public Channer_T<Chan_p>
	{
public:
	template <typename... Args_p>
	Channer_Holder_T(Holder_p&& holder, Args_p&&... args)
	:	Holder_p(std::move(holder))
	,	Channer_T<Chan_p>(std::forward<Args_p>(args)...)
		{}

	template <typename... Args_p>
	Channer_Holder_T(const Holder_p& holder, Args_p&&... args)
	:	Holder_p(holder)
	,	Channer_T<Chan_p>(std::forward<Args_p>(args)...)
		{}
	};

template <class Holder_p, class Chan_p, typename... Args_p>
ZP<Channer_T<Chan_p>> sChanner_Holder_T(Holder_p&& holder, Args_p&&... args)
	{
	return new Channer_Holder_T<Holder_p,Chan_p>(
		std::move(holder),
		std::forward<Args_p>(args)...);
	}

template <class Holder_p, class Chan_p, typename... Args_p>
ZP<Channer_T<Chan_p>> sChanner_Holder_T(const Holder_p& holder, Args_p&&... args)
	{
	return new Channer_Holder_T<Holder_p,Chan_p>(
		holder,
		std::forward<Args_p>(args)...);
	}

// =================================================================================================
#pragma mark - sChanner_Channer_T

template <class Chan_p, class ChannerOther_p>
ZP<Channer_T<Chan_p>>
sChanner_Channer_T(const ZP<ChannerOther_p>& iOther)
	{
	if (ZP<Counted> asCounted = iOther)
		return sChanner_Holder_T<ZP<Counted>,Chan_p>(asCounted, *iOther);
	return null;
	}

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
