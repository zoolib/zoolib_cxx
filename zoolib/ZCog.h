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

//--

	ZCog(const null_t&)
		{}
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
	ZCog<Param> i0, ZCog<Param> i1);

template <class Param>
ZCog<Param> sAny(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{
	if (i0)
		{
		if (i1)
			return sBindR(sCallable(sAny_Fun<Param>), i0, i1);
		return i0;
		}
	return i1;
	}

template <class Param>
ZCog<Param> sAny_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> i0, ZCog<Param> i1)
	{
	if (i0 && not sAssignIfUnequal(i0, i0->Call(i0, iParam)))
		{
		if (i1 && not sAssignIfUnequal(i1, i1->Call(i1, iParam)))
			return iSelf;
		}
	else if (i1)
		{
		i1 = i1->Call(i1, iParam);
		}

	return sAny(i0, i1);
	}

template <class Param>
ZCog<Param> operator&(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return sAny(i0, i1); }

template <class Param>
ZCog<Param>& operator&=(ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return i0 = sAny(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sAll

template <class Param>
ZCog<Param> sAll_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> i0, ZCog<Param> i1);

template <class Param>
ZCog<Param> sAll(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{
	if (i0 && i1)
		return sBindR(sCallable(sAll_Fun<Param>), i0, i1);
	return null;
	}

template <class Param>
ZCog<Param> sAll_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> i0, ZCog<Param> i1)
	{
	if (i0 && not sAssignIfUnequal(i0, i0->Call(i0, iParam)))
		{
		if (i1 && not sAssignIfUnequal(i1, i1->Call(i1, iParam)))
			return iSelf;
		}
	else if (i0 && i1)
		{
		i1 = i1->Call(i1, iParam);
		}

	return sAll(i0, i1);
	}

template <class Param>
ZCog<Param> operator*(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return sAll(i0, i1); }

template <class Param>
ZCog<Param>& operator*=(ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return i0 = sAll(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sSeq

template <class Param>
ZCog<Param> sSeq_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> i0, ZCog<Param> i1);

template <class Param>
ZCog<Param> sSeq(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{
	if (i0)
		{
		if (i1)
			return sBindR(sCallable(sSeq_Fun<Param>), i0, i1);
		return i0;
		}
	return i1;
	}

template <class Param>
ZCog<Param> sSeq_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> i0, ZCog<Param> i1)
	{
	if (i0 && not sAssignIfUnequal(i0, i0->Call(i0, iParam)))
		return iSelf;

	if (!i0 && i1)
		i1 = i1->Call(i1, iParam);

	return sSeq(i0, i1);
	}

template <class Param>
ZCog<Param> operator|(const ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return sSeq(i0, i1); }

template <class Param>
ZCog<Param>& operator|=(ZCog<Param>& i0, const ZCog<Param>& i1)
	{ return i0 = sSeq(i0, i1); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat

template <class Param>
ZCog<Param> sRepeat_Fun(const ZCog<Param>& iSelf, Param iParam,
	ZCog<Param> iChild_Start, ZCog<Param> iChild)
	{
	if (not iChild || not sAssignIfUnequal(iChild, iChild->Call(iChild, iParam)))
		return iSelf;

	if (iChild)
		return sBindR(sCallable(sRepeat_Fun<Param>), iChild_Start, iChild);

	return sBindR(sCallable(sRepeat_Fun<Param>), iChild_Start, iChild_Start);
	}

template <class Param>
ZCog<Param> sRepeat(ZCog<Param> iChild)
	{ return sBindR(sCallable(sRepeat_Fun<Param>), iChild, iChild); }

// =================================================================================================
#pragma mark -
#pragma mark * sRepeat (count)

template <class Param>
ZCog<Param> sRepeat_Count_Fun(const ZCog<Param>& iSelf, Param iParam,
	size_t iCount, ZCog<Param> iChild_Start, ZCog<Param> iChild)
	{
	if (not iCount)
		return null;

	if (iChild && not sAssignIfUnequal(iChild, iChild->Call(iChild, iParam)))
		return iSelf;

	if (iChild)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iChild_Start, iChild);

	if (--iCount)
		return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iChild_Start, iChild_Start);

	return null;
	}

template <class Param>
ZCog<Param> sRepeat(size_t iCount, ZCog<Param> iChild)
	{ return sBindR(sCallable(sRepeat_Count_Fun<Param>), iCount, iChild, iChild); }

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
	if (ZCog<Param> theZCog = sDelayUntil<Param>(ZTime::sSystem() + iDelay))
		return theZCog->Call(theZCog, iParam);
	return null;
	}

template <class Param>
ZCog<Param> sDelayFor(double iDelay)
	{ return sBindR(sCallable(sDelayFor_Fun<Param>), iDelay); }

// =================================================================================================
#pragma mark -
#pragma mark * sStartAt, sStopAt, sStartAfter, sStopAfter

template <class Param>
ZCog<Param> sStartAt(ZTime iSystemTime, ZCog<Param> iChild)
	{ return sDelayUntil<Param>(iSystemTime) | iChild; }

template <class Param>
ZCog<Param> sStopAt(ZTime iSystemTime, ZCog<Param> iChild)
	{ return sDelayUntil<Param>(iSystemTime) * iChild; }

template <class Param>
ZCog<Param> sStartAfter(double iDelay, ZCog<Param> iChild)
	{ return sDelayFor<Param>(iDelay) | iChild; }

template <class Param>
ZCog<Param> sStopAfter(double iDelay, ZCog<Param> iChild)
	{ return sDelayFor<Param>(iDelay) * iChild; }

} // namespace ZooLib

#endif // __ZCog__
