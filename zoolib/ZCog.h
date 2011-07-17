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

#ifndef __ZCog__
#define __ZCog__ 1

#include "zoolib/ZCallable.h"
#include "zoolib/ZCallable_Bind.h"
#include "zoolib/ZCallable_Function.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCog

template <class Param_p>
class ZCog
:	public ZRef<ZCallable<ZCog<Param_p>(const ZCog<Param_p>&,Param_p)> >
	{
public:
	typedef Param_p Param;
	typedef ZCog(Signature)(const ZCog& iSelf, Param iParam);
	typedef ZCallable<Signature> Callable;
	typedef ZRef<Callable> inherited;

//--

	ZCog()
		{}

	ZCog(const ZCog& iOther)
	:	inherited(iOther)
		{}

	~ZCog()
		{}

	ZCog& operator=(const ZCog& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

//--

	ZCog(const inherited& iOther)
	:	inherited(iOther)
		{}
	
	ZCog& operator=(const inherited& iOther)
		{
		inherited::operator=(iOther);
		return *this;
		}

//--

	ZCog(Callable* iCallable)
	:	inherited(iCallable)
		{}

	ZCog& operator=(Callable* iCallable)
		{
		inherited::operator=(iCallable);
		return *this;
		}

//--

	ZCog(const null_t&)
		{}

	ZCog& operator=(const null_t& iNull)
		{
		inherited::operator=(iNull);
		return *this;
		}
	};

// =================================================================================================
#pragma mark -
#pragma mark * sCog

const struct
	{
	template <class Param>
	ZCog<Param> operator()
		(ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>* iCallable) const
		{ return ZCog<Param>(iCallable); }

	template <class Param>
	ZCog<Param> operator()
		(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable) const
		{ return ZCog<Param>(iCallable); }

	template <class Param>
	ZCog<Param> operator()
		(const ZCog<Param>& iCog) const
		{ return iCog; }

	template <class Param>
	ZCog<Param> operator&
		(ZCallable<ZCog<Param>(const ZCog<Param>&,Param)>* iCallable) const
		{ return ZCog<Param>(iCallable); }

	template <class Param>
	ZCog<Param> operator&
		(const ZRef<ZCallable<ZCog<Param>(const ZCog<Param>&,Param)> >& iCallable) const
		{ return ZCog<Param>(iCallable); }

	template <class Param>
	ZCog<Param> operator&
		(const ZCog<Param>& iCog) const
		{ return iCog; }

	} sCog = {};

// =================================================================================================
#pragma mark -
#pragma mark * sAny

template <class Param>
ZCog<Param> sAny_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sAny(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0)
		{
		if (iCog1)
			return sBindR(sCallable(sAny_Fun<Param>), iCog0, iCog1);
		return iCog0;
		}
	return iCog1;
	}

template <class Param>
ZCog<Param> sAny_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		{
		if (iCog1 && not sCompareAndSet(iCog1, iCog1->Call(iCog1, iParam)))
			return iSelf;
		}
	else if (iCog1)
		{
		iCog1 = iCog1->Call(iCog1, iParam);
		}

	return sAny(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator&(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return sAny(iCog0, iCog1); }

template <class Param>
ZCog<Param>& operator&=(ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return iCog0 = sAny(iCog0, iCog1); }

// =================================================================================================
#pragma mark -
#pragma mark * sAll

template <class Param>
ZCog<Param> sAll_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sAll(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0 && iCog1)
		return sBindR(sCallable(sAll_Fun<Param>), iCog0, iCog1);
	return null;
	}

template <class Param>
ZCog<Param> sAll_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		{
		if (iCog1 && not sCompareAndSet(iCog1, iCog1->Call(iCog1, iParam)))
			return iSelf;
		}
	else if (iCog0 && iCog1)
		{
		iCog1 = iCog1->Call(iCog1, iParam);
		}

	return sAll(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator*(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return sAll(iCog0, iCog1); }

template <class Param>
ZCog<Param>& operator*=(ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return iCog0 = sAll(iCog0, iCog1); }

// =================================================================================================
#pragma mark -
#pragma mark * sSeq

template <class Param>
ZCog<Param> sSeq_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1);

template <class Param>
ZCog<Param> sSeq(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{
	if (iCog0)
		{
		if (iCog1)
			return sBindR(sCallable(sSeq_Fun<Param>), iCog0, iCog1);
		return iCog0;
		}
	return iCog1;
	}

template <class Param>
ZCog<Param> sSeq_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog0, ZCog<Param> iCog1)
	{
	if (iCog0 && not sCompareAndSet(iCog0, iCog0->Call(iCog0, iParam)))
		return iSelf;

	if (!iCog0 && iCog1)
		iCog1 = iCog1->Call(iCog1, iParam);

	return sSeq(iCog0, iCog1);
	}

template <class Param>
ZCog<Param> operator|(const ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return sSeq(iCog0, iCog1); }

template <class Param>
ZCog<Param>& operator|=(ZCog<Param>& iCog0, const ZCog<Param>& iCog1)
	{ return iCog0 = sSeq(iCog0, iCog1); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat

template <class Param>
ZCog<Param> sRepeat_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iCog_Init, ZCog<Param> iCog)
	{
	if (not iCog || not sCompareAndSet(iCog, iCog->Call(iCog, iParam)))
		return iSelf;

	if (iCog)
		return sBindR(sCallable(sRepeat_Fun<Param>), iCog_Init, iCog);

	return sBindR(sCallable(sRepeat_Fun<Param>), iCog_Init, iCog_Init);
	}

template <class Param>
ZCog<Param> sRepeat(ZCog<Param> iCog)
	{ return sBindR(sCallable(sRepeat_Fun<Param>), iCog, iCog); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat (count)

template <class Param>
ZCog<Param> sRepeat_Count_Fun(const ZCog<Param>& iSelf, Param iParam,
	size_t iCount, ZCog<Param> iCog_Init, ZCog<Param> iCog)
	{
	if (not iCount)
		return null;

	if (iCog && not sCompareAndSet(iCog, iCog->Call(iCog, iParam)))
		return iSelf;

	if (iCog)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCog_Init, iCog);

	if (--iCount)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCog_Init, iCog_Init);

	return null;
	}

template <class Param>
ZCog<Param> sRepeat(size_t iCount, ZCog<Param> iCog)
	{ return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iCog, iCog); }

// =================================================================================================
#pragma mark -
#pragma mark * sDelayUntil

template <class Param>
ZCog<Param> sDelayUntil_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZTime iSystemTime)
	{
	if (ZTime::sSystem() > iSystemTime)
		return null;
	return iSelf;
	}

template <class Param>
ZCog<Param> sDelayUntil(ZTime iSystemTime)
	{ return sBindR(sCallable(sDelayUntil_Fun<Param>), iSystemTime); }

// =================================================================================================
#pragma mark -
#pragma mark * sDelayFor

template <class Param>
ZCog<Param> sDelayFor_Fun(const ZCog<Param>& iSelf, Param iParam,
	double iDelay)
	{
	if (ZCog<Param> theCog = sDelayUntil<Param>(ZTime::sSystem() + iDelay))
		return theCog->Call(theCog, iParam);
	return null;
	}

template <class Param>
ZCog<Param> sDelayFor(double iDelay)
	{ return sBindR(sCallable(sDelayFor_Fun<Param>), iDelay); }

// =================================================================================================
#pragma mark -
#pragma mark * sStartAt, sStopAt, sStartAfter, sStopAfter

template <class Param>
ZCog<Param> sStartAt(ZTime iSystemTime, ZCog<Param> iCog)
	{ return sDelayUntil<Param>(iSystemTime) | iCog; }

template <class Param>
ZCog<Param> sStopAt(ZTime iSystemTime, ZCog<Param> iCog)
	{ return sDelayUntil<Param>(iSystemTime) * iCog; }

template <class Param>
ZCog<Param> sStartAfter(double iDelay, ZCog<Param> iCog)
	{ return sDelayFor<Param>(iDelay) | iCog; }

template <class Param>
ZCog<Param> sStopAfter(double iDelay, ZCog<Param> iCog)
	{ return sDelayFor<Param>(iDelay) * iCog; }

} // namespace ZooLib

#endif // __ZCog__
