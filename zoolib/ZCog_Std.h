/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#ifndef __ZCog_Std_h__
#define __ZCog_Std_h__ 1

#include "zoolib/ZCog.h"
#include "zoolib/ZTime.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayUntil

template <class Param>
ZCog<Param> spCogFun_DelayUntil(const ZCog<Param>& iSelf, Param iParam,
	ZTime iSystemTime)
	{
	if (ZTime::sSystem() > iSystemTime)
		return sCog_Term<Param>();
	return iSelf;
	}

template <class Param>
ZCog<Param> sCog_DelayUntil(ZTime iSystemTime)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_DelayUntil<Param>));
	return sBindR(spCallable, iSystemTime);
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCog_DelayFor

template <class Param>
ZCog<Param> spCogFun_DelayFor(const ZCog<Param>& iSelf, Param iParam,
	double iDelay)
	{ return sCallCog(sCog_DelayUntil<Param>(ZTime::sSystem() + iDelay), iParam); }

template <class Param>
ZCog<Param> sCog_DelayFor(double iDelay)
	{
	static ZMACRO_auto(spCallable, sCallable(spCogFun_DelayFor<Param>));
	return sBindR(spCallable, iDelay);
	}

// =================================================================================================
#pragma mark -
#pragma mark * sCog_StartAt, sCog_StopAt, sCog_StartAfter, sCog_StopAfter

template <class Param>
ZCog<Param> sCog_StartAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_DelayUntil<Param>(iSystemTime) | iCallable; }

template <class Param>
ZCog<Param> sCog_StopAt(ZTime iSystemTime,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable)
	{ return sCog_DelayUntil<Param>(iSystemTime) * iCallable; }

template <class Param>
ZCog<Param> sCog_StartAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >iCallable)
	{ return sCog_DelayFor<Param>(iDelay) | iCallable; }

template <class Param>
ZCog<Param> sCog_StopAfter(double iDelay,
	const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >iCallable)
	{ return sCog_DelayFor<Param>(iDelay) * iCallable; }

} // namespace ZooLib

#endif // __ZCog_Std_h__
