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

#ifndef __ZooLib_ChannerXX_h__
#define __ZooLib_ChannerXX_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

#include "zoolib/ChanClose.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanSize.h"
#include "zoolib/ChanSizeSet.h"
#include "zoolib/ChanR.h"
#include "zoolib/ChanU.h"
#include "zoolib/ChanW.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark 

typedef Channer<ChanClose> ChannerClose;
typedef Channer<ChanPos> ChannerPos;
typedef Channer<ChanSize> ChannerSize;
typedef Channer<ChanSizeSet> ChannerSizeSet;

// =================================================================================================
#pragma mark -
#pragma mark ChannerR, ChannerU, ChannerW

#if ZCONFIG_CPP >= 2011

template <class Elmt_p> using ChannerR = Channer<ChanR<Elmt_p> >;
template <class Elmt_p> using ChannerU = Channer<ChanU<Elmt_p> >;
template <class Elmt_p> using ChannerW = Channer<ChanW<Elmt_p> >;

#else // ZCONFIG_CPP >= 2011

template <class Elmt_p> class ChannerR : public Channer<ChanR<Elmt_p> > {};
template <class Elmt_p> class ChannerU : public Channer<ChanU<Elmt_p> > {};
template <class Elmt_p> class ChannerW : public Channer<ChanW<Elmt_p> > {};

#endif // ZCONFIG_CPP >= 2011

// =================================================================================================
#pragma mark -
#pragma mark ChannerRW

template <class Elmt_p>
class ChannerRW
:	public virtual ChannerR<Elmt_p>
,	public virtual ChannerW<Elmt_p>
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRWClose

template <class Elmt_p>
class ChannerRWClose
:	public virtual ChannerRW<Elmt_p>
,	public virtual ChannerClose
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRU

template <class Elmt_p>
class ChannerRU
:	public virtual ChannerR<Elmt_p>
,	public virtual ChannerU<Elmt_p>
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRPos

template <class Elmt_p>
class ChannerRPos
:	public virtual ChannerRU<Elmt_p>
,	public virtual ChannerPos
,	public virtual ChannerSize
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerWPos

template <class Elmt_p>
class ChannerWPos
:	public virtual ChannerW<Elmt_p>
,	public virtual ChannerPos
,	public virtual ChannerSize
,	public virtual ChannerSizeSet
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRWPos

template <class Elmt_p>
class ChannerRWPos
:	public virtual ChannerRPos<Elmt_p>
,	public virtual ChannerWPos<Elmt_p>
	{};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRU_T

template <class Chan_p>
class ChannerRU_T
:	public virtual ChannerRU<typename Chan_p::CommonElmt_t>
	{
public:
	typedef typename Chan_p::CommonElmt_t CommonElmt_t;

	ChannerRU_T()
		{}

	template <class P>
	ChannerRU_T(P& iParam)
	:	fChan(iParam)
		{}

	template <class P>
	ChannerRU_T(const P& iParam)
	:	fChan(iParam)
		{}

// From ChannerR
	virtual void GetChan(const ChanR<CommonElmt_t>*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerU
	virtual void GetChan(const ChanU<CommonElmt_t>*& oChanPtr) { oChanPtr = &fChan; }

protected:
	Chan_p fChan;
	};

// =================================================================================================
#pragma mark -
#pragma mark ChannerRWPos_T

template <class Chan_p>
class ChannerRWPos_T
:	public virtual ChannerRWPos<typename Chan_p::CommonElmt_t>
	{
public:
	typedef typename Chan_p::CommonElmt_t CommonElmt_t;

	ChannerRWPos_T() {}
	virtual ~ChannerRWPos_T() {}

	template <class Param_p>
	ChannerRWPos_T(Param_p& iParam) : fChan(iParam) {}

	template <class Param_p>
	ChannerRWPos_T(const Param_p& iParam) : fChan(iParam) {}

// From ChannerR
	virtual void GetChan(const ChanR<CommonElmt_t>*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerU
	virtual void GetChan(const ChanU<CommonElmt_t>*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerW
	virtual void GetChan(const ChanW<CommonElmt_t>*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerPos
	virtual void GetChan(const ChanPos*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerSize
	virtual void GetChan(const ChanSize*& oChanPtr) { oChanPtr = &fChan; }

// From ChannerSizeSet
	virtual void GetChan(const ChanSizeSet*& oChanPtr) { oChanPtr = &fChan; }

// Our protocol
	Chan_p& GetChanActual() { return fChan; }

protected:
	Chan_p fChan;
	};

} // namespace ZooLib

#endif // __ZooLib_ChannerXX_h__
