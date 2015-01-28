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

#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -

template <class Chan_p>
class Channer
:	public ZCounted
	{
public:
	typedef Chan_p Chan_t;

	virtual void GetChan(const Chan_t*& oChanPtr) = 0;
	};

template <class Chan_p>
const Chan_p& sGetChan(const ZRef<Channer<Chan_p> >& iChanner)
	{
	const Chan_p* theChan_p;
	iChanner->GetChan(theChan_p);
	return *theChan_p;
	}

// =================================================================================================
#pragma mark -
#pragma mark Channer_T

template <class Chan_p, class Chan_Self_p = typename Chan_p::Chan_Base>
class Channer_T
:	public virtual Channer<Chan_Self_p>
	{
public:
	Channer_T() {}
	virtual ~Channer_T() {}

	template <class Param_p>
	Channer_T(Param_p& iParam) : fChan(iParam) {}

	template <class Param_p>
	Channer_T(const Param_p& iParam) : fChan(iParam) {}

// From Channer
	virtual void GetChan(const Chan_Self_p*& oChanPtr) { oChanPtr = &fChan; }

// Our protocol
	Chan_p& GetChanActual() { return fChan; }

protected:
	Chan_p fChan;
	};

template <class Chan_p>
ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_T()
	{ return new Channer_T<Chan_p>; }

template <class Chan_p, class Param_p>
ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_T(Param_p& iParam)
	{ return new Channer_T<Chan_p>(iParam); }

template <class Chan_p, class Param_p>
ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_T(const Param_p& iParam)
	{ return new Channer_T<Chan_p>(iParam); }

// =================================================================================================
#pragma mark -
#pragma mark Channer_Channer_T

template <class Chan_p,
	class Chan_Other_p = typename Chan_p::Chan_Base,
	class Chan_Self_p = typename Chan_p::Chan_Base>
class Channer_Channer_T
:	public virtual Channer<Chan_Self_p>
	{
protected:
	Channer_Channer_T() {}

public:
	virtual ~Channer_Channer_T() {}

	template <class P>
	Channer_Channer_T(P& iParam, const ZRef<Channer<Chan_Other_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(iParam, sGetChan(iChannerOther))
		{}

	template <class P>
	Channer_Channer_T(const P& iParam, const ZRef<Channer<Chan_Other_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(iParam, sGetChan(iChannerOther))
		{}

	Channer_Channer_T(const ZRef<Channer<Chan_Other_p> >& iChannerOther)
	:	fChannerOther(iChannerOther),
		fChan(sGetChan(iChannerOther))
		{}

// From Channer
	virtual void GetChan(const Chan_Self_p*& oChanPtr)
		{ oChanPtr = &fChan; }

// Our protocol
	Chan_p& GetChanActual() { return fChan; }

protected:
	const ZRef<Channer<Chan_Other_p> > fChannerOther;
	Chan_p fChan;
	};

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
