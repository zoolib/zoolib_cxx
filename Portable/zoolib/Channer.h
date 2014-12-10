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

#include "zoolib/ChanClose.h"
#include "zoolib/ChanCount.h"
#include "zoolib/ChanCountSet.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW.h"
#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
// MARK: -

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
// MARK: -

typedef Channer<ChanClose> ChannerClose;
typedef Channer<ChanPos> ChannerPos;
typedef Channer<ChanCount> ChannerCount;
typedef Channer<ChanCountSet> ChannerCountSet;

// =================================================================================================
// MARK: - Channer_T

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


// See Channer_Channer for a (I think) more general version of Channer_FT
//// =================================================================================================
//// MARK: - Channer_FT
//
//template <class Chan_p, class Chan_Self_p = typename Chan_p::Chan_Base>
//class Channer_FT
//:	public virtual Channer<Chan_Self_p>
//	{
//protected:
//	Channer_FT() {}
//
//public:
//	virtual ~Channer_FT() {}
//
//	Channer_FT(const ZRef<Channer<Chan_Self_p> >& iChanner)
//	:	fChannerReal(iChanner)
//	,	fChan(sGetChan(fChannerReal))
//		{}
//
//	template <class Param_p>
//	Channer_FT(Param_p& iParam, const ZRef<Channer<Chan_Self_p> >& iChanner)
//	:	fChannerReal(iChanner)
//	,	fChan(iParam, sGetChan(fChannerReal))
//		{}
//
//	template <class Param_p>
//	Channer_FT(const Param_p& iParam, const ZRef<Channer<Chan_Self_p> >& iChanner)
//	:	fChannerReal(iChanner)
//	,	fChan(iParam, sGetChan(fChannerReal))
//		{}
//
//// From Channer
//	virtual void GetChan(const Chan_Self_p*& oChanPtr) { oChanPtr = &fChan; }
//
//// Our protocol
//	Chan_p& GetChanActual() { return fChan; }
//
//protected:
//	ZRef<Channer<Chan_Self_p> > fChannerReal;
//	Chan_p fChan;
//	};
//
//template <class Chan_p>
//ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_FT(
//	const ZRef<Channer<typename Chan_p::Chan_Base> >& iChanner)
//	{ return new Channer_FT<Chan_p>; }
//
//template <class Chan_p, class Param_p>
//ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_FT(
//	Param_p& iParam, const ZRef<Channer<typename Chan_p::Chan_Base> >& iChanner)
//	{ return new Channer_FT<Chan_p>(iParam, iChanner); }
//
//template <class Chan_p, class Param_p>
//ZRef<Channer<typename Chan_p::Chan_Base> > sChanner_FT(
//	const Param_p& iParam, const ZRef<Channer<typename Chan_p::Chan_Base> >& iChanner)
//	{ return new Channer_FT<Chan_p>(iParam, iChanner); }

// =================================================================================================
// MARK: - ChannerComboRW

template <class Elmt_p>
class ChannerComboRW
	{
public:
	ChannerComboRW(const ZRef<Channer<ChanR<Elmt_p> > >& iChannerR,
		const ZRef<Channer<ChanW<Elmt_p> > >& iChannerW)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
		{}

	const ZRef<Channer<ChanR<Elmt_p> > >& GetR() const
		{ return fChannerR; }

	void SetR(const ZRef<Channer<ChanR<Elmt_p> > >& iChannerR)
		{ fChannerR = iChannerR; }

	const ZRef<Channer<ChanW<Elmt_p> > >& GetW() const
		{ return fChannerW; }

	void SetW(const ZRef<Channer<ChanW<Elmt_p> > >& iChannerW)
		{ fChannerW = iChannerW; }

protected:
	ZRef<Channer<ChanR<Elmt_p> > > fChannerR;
	ZRef<Channer<ChanW<Elmt_p> > > fChannerW;
	};

// =================================================================================================
// MARK: - ChannerComboRWClose

template <class Elmt_p>
class ChannerComboRWClose
	{
public:
	ChannerComboRWClose(
		const ZRef<Channer<ChanR<Elmt_p> > >& iChannerR,
		const ZRef<Channer<ChanW<Elmt_p> > >& iChannerW,
		const ZRef<Channer<ChanClose> >& iChannerClose)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
	,	fChannerClose(iChannerClose)
		{}

	const ZRef<Channer<ChanR<Elmt_p> > >& GetR() const
		{ return fChannerR; }

	void SetR(const ZRef<Channer<ChanR<Elmt_p> > >& iChannerR)
		{ fChannerR = iChannerR; }

	const ZRef<Channer<ChanW<Elmt_p> > >& GetW() const
		{ return fChannerW; }

	void SetW(const ZRef<Channer<ChanW<Elmt_p> > >& iChannerW)
		{ fChannerW = iChannerW; }

	const ZRef<Channer<ChanClose> >& GetClose() const
		{ return fChannerClose; }

	void SetClose(const ZRef<Channer<ChanClose> >& iChannerClose)
		{ fChannerClose = iChannerClose; }

protected:
	ZRef<Channer<ChanR<Elmt_p> > > fChannerR;
	ZRef<Channer<ChanW<Elmt_p> > > fChannerW;
	ZRef<Channer<ChanClose> > fChannerClose;
	};

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
