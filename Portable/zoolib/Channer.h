/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZooLib_Channer_h__
#define __ZooLib_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Chan.h"
#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark Channer

template <class DF> using Channer
	= typename AsDeriveFrom<
		typename TypeListPrefixed<
			ZCounted,
			typename DF::AsTypeList_t
			>::Result_t
		>::Result_t;

// =================================================================================================
#pragma mark -
#pragma mark ChanOfChanner

template <class Channer_p> using ChanOfChanner =
	typename ZooLib::AsDeriveFrom<
		typename TypeListWithoutPrefix<typename Channer_p::AsTypeList_t>::Result_t>::Result_t;

// =================================================================================================
#pragma mark -
#pragma mark Basic channer typedefs.

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
#pragma mark -
#pragma mark Composite channer typedefs.

template <class EE> using ChannerRU = Channer<ChanRU<EE>>;
template <class EE> using ChannerRPos = Channer<ChanRPos<EE>>;
template <class EE> using ChannerWPos = Channer<ChanWPos<EE>>;
template <class EE> using ChannerRWPos = Channer<ChanRWPos<EE>>;
template <class EE> using ChannerRW = Channer<ChanRW<EE>>;
using ChannerClose = Channer<ChanClose>;
template <class EE> using ChannerRCon = Channer<ChanRCon<EE>>;
template <class EE> using ChannerWCon = Channer<ChanWCon<EE>>;
template <class EE> using ChannerConnection = Channer<ChanConnection<EE>>;

// This name is still in use.
template <class EE> using ChannerRWClose = ChannerConnection<EE>;

// =================================================================================================
#pragma mark -
#pragma mark sChanner_Chan.

// Get the channer from a chan

template <class Chan_p>
ZRef<Channer<Chan_p>> sChanner_Chan(const Chan_p& iChan)
	{ return dynamic_cast<Channer<Chan_p>*>(sNonConst(&iChan)); }

template <class Chan_p>
ZRef<Channer<Chan_p>> sAChanner_Chan(const Chan_p& iChan)
	{
	ZRef<Channer<Chan_p>> result = sChanner_Chan<Chan_p>(iChan);
	ZAssert(result);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark Channer_T

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
ZRef<Channer_T<Chan_p>> sChanner_T()
	{ return new Channer_T<Chan_p>(); }

template <class Chan_p, typename... Args_p>
ZRef<Channer_T<Chan_p>> sChanner_T(Args_p&&... args)
	{ return new Channer_T<Chan_p>(std::forward<Args_p>(args)...); }

// =================================================================================================
#pragma mark -
#pragma mark Channer_Channer_T

template <class Chan_p>
class Channer_Channer_T
:	public Channer_T<Chan_p>
	{
	typedef Channer_T<Chan_p> inherited;
public:
	template <class ChannerOther_p, bool Sense_p>
	Channer_Channer_T(const ZRef<ChannerOther_p,Sense_p>& iOther)
	:	inherited(*iOther)
	,	fOther(iOther)
		{}

	template <class ChannerOther_p, bool Sense_p, typename... Args_p>
	Channer_Channer_T(const ZRef<ChannerOther_p,Sense_p>& iOther, Args_p&&... args)
	:	inherited(*iOther, std::forward<Args_p>(args)...)
	,	fOther(iOther)
		{}

protected:
	const ZRef<ZCounted> fOther;
	};

template <class Chan_p, class ChannerOther_p, bool Sense_p>
ZRef<Channer_Channer_T<Chan_p>> sChanner_Channer_T(const ZRef<ChannerOther_p,Sense_p>& iOther)
	{
	if (ZRef<ChannerOther_p,true> theOther = iOther)
		return new Channer_Channer_T<Chan_p>(theOther);
	return null;
	}

template <class Chan_p, class ChannerOther_p, bool Sense_p, typename... Args_p>
ZRef<Channer_Channer_T<Chan_p>>
sChanner_Channer_T(const ZRef<ChannerOther_p,Sense_p>& iOther, Args_p&&... args)
	{
	if (ZRef<ChannerOther_p,true> theOther = iOther)
		return new Channer_Channer_T<Chan_p>(iOther, std::forward<Args_p>(args)...);
	return null;
	}

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
