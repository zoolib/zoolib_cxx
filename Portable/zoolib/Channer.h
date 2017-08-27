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

// Not super happy with these. Generally better at this point to just use operator* directly.
template <class Thing>
const Thing& sGetChan(const ZRef<Thing>& iChanner)
	{ return *iChanner; }

template <class Result, class Thing>
const Result& sGetChan(const ZRef<Thing>& iChanner)
	{ return *iChanner; }

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
#pragma mark Composite channers typedefs.

template <class EE> using ChannerRU = Channer<ChanRU<EE>>;
template <class EE> using ChannerRPos = Channer<ChanRPos<EE>>;
template <class EE> using ChannerWPos = Channer<ChanWPos<EE>>;
template <class EE> using ChannerRWPos = Channer<ChanRWPos<EE>>;
template <class EE> using ChannerConnection = Channer<ChanConnection<EE>>;

template <class EE> using ChannerRW = Channer<ChanRW<EE>>;

using ChannerClose = Channer<DeriveFrom<Aspect_Abort,Aspect_DisconnectRead,Aspect_DisconnectWrite>>;

template <class EE> using ChannerRW = Channer<ChanRW<EE>>;

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
,	public virtual Channer<typename AsDeriveFrom<typename Chan_p::AsTypeList_t>::Result_t>
	{
public:
	Channer_T() : Chan_p () {}

//	template <class... T>
//	Channer_T(T... args) : Chan_p(args...) {}

	template <class T0>
	Channer_T(const T0& iT0) : Chan_p(iT0) {}

	template <class T0, class T1>
	Channer_T(const T0& iT0, const T1& iT1) : Chan_p(iT0, iT1) {}

	template <class T0, class T1, class T2>
	Channer_T(const T0& iT0, const T1& iT1, const T2& iT2) : Chan_p(iT0, iT1, iT2) {}
	};

template <class Chan_p>
ZRef<Channer_T<Chan_p>> sChanner_T()
	{ return new Channer_T<Chan_p>(); }

template <class Chan_p, class T0>
ZRef<Channer_T<Chan_p>> sChanner_T(const T0& iT0)
	{ return new Channer_T<Chan_p>(iT0); }

template <class Chan_p, class T0, class T1>
ZRef<Channer_T<Chan_p>> sChanner_T(const T0& iT0, const T1& iT1)
	{ return new Channer_T<Chan_p>(iT0, iT1); }

// =================================================================================================
#pragma mark -
#pragma mark Channer_Channer_T

template <class Chan_p>
class Channer_Channer_T
:	public Chan_p
,	public virtual Channer<typename AsDeriveFrom<typename Chan_p::AsTypeList_t>::Result_t>
	{
public:
	template <class ChannerOther_p>
	Channer_Channer_T(const ZRef<ChannerOther_p>& iOther)
	:	Chan_p(*iOther)
	,	fOther(iOther)
		{}

//	template <class ChannerOther_p, class... T>
//	Channer_Channer_T(T... args, const ZRef<ChannerOther_p>& iOther)
//	:	Chan_p(args..., *iOther)
//	,	fOther(iOther)
//		{}

	template <class T0, class ChannerOther_p>
	Channer_Channer_T(const T0& iT0, const ZRef<ChannerOther_p>& iOther)
	:	Chan_p(iT0, *iOther)
	,	fOther(iOther)
		{}

	template <class T0, class T1, class ChannerOther_p>
	Channer_Channer_T(const T0& iT0, const T1& iT1, const ZRef<ChannerOther_p>& iOther)
	:	Chan_p(iT0, iT1, *iOther)
	,	fOther(iOther)
		{}

	template <class T0, class T1, class T2, class ChannerOther_p>
	Channer_Channer_T(const T0& iT0, const T1& iT1, const T2& iT2, const ZRef<ChannerOther_p>& iOther)
	:	Chan_p(iT0, iT1, iT2, *iOther)
	,	fOther(iOther)
		{}

protected:
	const ZRef<ZCounted> fOther;
	};

template <class Chan_p, class ChannerOther_p>
ZRef<Channer_Channer_T<Chan_p>> sChanner_Channer_T(const ZRef<ChannerOther_p>& iOther)
	{ return new Channer_Channer_T<Chan_p>(iOther); }

template <class Chan_p, class T0, class ChannerOther_p>
ZRef<Channer_Channer_T<Chan_p>>
sChanner_Channer_T(const T0& iT0, const ZRef<ChannerOther_p>& iOther)
	{ return new Channer_Channer_T<Chan_p>(iT0, iOther); }

template <class Chan_p, class T0, class T1, class ChannerOther_p>
ZRef<Channer_Channer_T<Chan_p>>
sChanner_Channer_T(const T0& iT0, const T1& iT1, const ZRef<ChannerOther_p>& iOther)
	{ return new Channer_Channer_T<Chan_p>(iT0, iT1, iOther); }

template <class Chan_p, class T0, class T1, class T2, class ChannerOther_p>
ZRef<Channer_Channer_T<Chan_p>>
sChanner_Channer_T(const T0& iT0, const T1& iT1, const T2& iT2, const ZRef<ChannerOther_p>& iOther)
	{ return new Channer_Channer_T<Chan_p>(iT0, iT1, iT2, iOther); }

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
