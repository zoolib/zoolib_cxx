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

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"
#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChannerR

template <class Elmt_p>
class ChannerR
:	public ZCounted
	{
public:
	typedef ChanR<Elmt_p> ChanR_t;

// Our protocol
	virtual const ChanR_t& GetChanR() = 0;
	};

// =================================================================================================
// MARK: - ChanR_Channer

template <class Elmt_p>
class ChanR_Channer
:	public ZRef<ChannerR<Elmt_p> >
,	public ChanR<Elmt_p>
	{
public:
	ChanR_Channer(const ZRef<ChannerR<Elmt_p> >& iChanner)
	:	fChanner(iChanner)
		{}

// From ChanR<Elmt_p>
	virtual size_t Read(Elmt_p* iDest, size_t iCount)
		{ return fChanner->GetChanR().Read(iDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return fChanner->GetChanR().Skip(iCount); }

	virtual size_t Readable()
		{ return fChanner->GetChanR().Readable(); }

private:
	const ZRef<ChannerR<Elmt_p> > fChanner;
	};

// =================================================================================================
// MARK: - ChannerR_T

template <class ChanR_p>
class ChannerR_T
:	public virtual ChannerR<typename ChanR_p::Elmt>
	{
protected:
	typedef ChanR_p ChanR_Actual;

	typedef typename ChanR_p::Elmt_t Elmt_t;

	typedef ChanR<Elmt_t> ChanR_t;
	typedef ChannerR<Elmt_t> ChannerR_t;

public:
	ChannerR_T() {}

	virtual ~ChannerR_T() {}

	template <class Param_p>
	ChannerR_T(Param_p& iParam) : fChan(iParam) {}

	template <class Param_p>
	ChannerR_T(const Param_p& iParam) : fChan(iParam) {}

// From ChannerR
	virtual const ChanR_t& GetChanR() { return fChan; }

// Our protocol
	ChanR_Actual& GetChan() { return fChan; }

protected:
	ChanR_Actual fChan;
	};

template <class Chan_p>
ZRef<ChannerR<typename Chan_p::Elem_t> > sChannerR_T()
	{ return new ChannerR_T<Chan_p>; }

template <class Chan_p, class Param_p>
ZRef<ChannerR<typename Chan_p::Elem_t> > sChannerR_T(Param_p& iParam)
	{ return new ChannerR_T<Chan_p>(iParam); }

template <class Chan_p, class Param_p>
ZRef<ChannerR<typename Chan_p::Elem_t> > sChannerR_T(const Param_p& iParam)
	{ return new ChannerR_T<Chan_p>(iParam); }

// =================================================================================================
// MARK: - ChannerR_FT

template <class ChanR_p>
class ChannerR_FT
:	public virtual ChannerR<typename ChanR_p::Elmt>
	{
protected:
	typedef ChanR_p ChanR_Actual;

	typedef typename ChanR_p::Elmt_t Elmt_t;

	typedef ChanR<Elmt_t> ChanR_t;
	typedef ChannerR<Elmt_t> ChannerR_t;

	ChannerR_FT() {}

public:
	virtual ~ChannerR_FT() {}

	template <class Param_p>
	ChannerR_FT(Param_p& iParam, const ZRef<ChannerR_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iParam, iChanner->GetChanR())
		{}

	template <class Param_p>
	ChannerR_FT(const Param_p& iParam, const ZRef<ChannerR_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iParam, iChanner->GetChanR())
		{}

	ChannerR_FT(const ZRef<ChannerR_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iChanner->GetChanR())
		{}

// From ChannerR
	virtual const ChanR_t& GetChanR() { return fChan; }

// Our protocol
	ChanR_Actual& GetChan() { return fChan; }

protected:
	ZRef<ChannerR_t> fChannerReal;
	ChanR_Actual fChan;
	};

// =================================================================================================
// MARK: - ChannerW

template <class Elmt_p>
class ChannerW
:	public ZCounted
	{
public:
	typedef ChanW<Elmt_p> ChanW_t;

// Our protocol
	virtual const ChanW_t& GetChanW() = 0;
	};

// =================================================================================================
// MARK: - ChanW_Channer

template <class Elmt_p>
class ChanW_Channer
:	public ZRef<ChannerR<Elmt_p> >
,	public ChanW<Elmt_p>
	{
public:
	ChanW_Channer(const ZRef<ChannerW<Elmt_p> >& iChanner)
	:	fChanner(iChanner)
		{}

// From ChanW<Elmt_p>
	virtual size_t Write(const Elmt_p* iSource, size_t iCount)
		{ return fChanner->GetChanW().Write(iSource, iCount); }

	virtual void Flush()
		{ fChanner->GetChanW().Flush(); }

private:
	const ZRef<ChannerW<Elmt_p> > fChanner;
	};

// =================================================================================================
// MARK: - ChannerW_T

template <class ChanW_p>
class ChannerW_T
:	public virtual ChannerW<typename ChanW_p::Elmt>
	{
protected:
	typedef ChanW_p ChanW_Actual;

	typedef typename ChanW_p::Elmt_t Elmt_t;

	typedef ChanW<Elmt_t> ChanW_t;
	typedef ChannerW<Elmt_t> ChannerW_t;

public:
	ChannerW_T() {}

	virtual ~ChannerW_T() {}

	template <class Param_p>
	ChannerW_T(Param_p& iParam) : fChan(iParam) {}

	template <class Param_p>
	ChannerW_T(const Param_p& iParam) : fChan(iParam) {}

// From ChannerW
	virtual const ChanW_t& GetChanW() { return fChan; }

// Our protocol
	ChanW_Actual& GetChan() { return fChan; }

protected:
	ChanW_Actual fChan;
	};

template <class Chan_p>
ZRef<ChannerW<typename Chan_p::Elem_t> > sChannerW_T()
	{ return new ChannerW_T<Chan_p>; }

template <class Chan_p, class Param_p>
ZRef<ChannerW<typename Chan_p::Elem_t> > sChannerW_T(Param_p& iParam)
	{ return new ChannerW_T<Chan_p>(iParam); }

template <class Chan_p, class Param_p>
ZRef<ChannerW<typename Chan_p::Elem_t> > sChannerW_T(const Param_p& iParam)
	{ return new ChannerW_T<Chan_p>(iParam); }

// =================================================================================================
// MARK: - ChannerW_FT

template <class ChanW_p>
class ChannerW_FT
:	public virtual ChannerW<typename ChanW_p::Elmt>
	{
protected:
	typedef ChanW_p ChanW_Actual;

	typedef typename ChanW_p::Elmt_t Elmt_t;

	typedef ChanW<Elmt_t> ChanW_t;
	typedef ChannerW<Elmt_t> ChannerW_t;

	ChannerW_FT() {}

public:
	virtual ~ChannerW_FT() {}

	template <class Param_p>
	ChannerW_FT(Param_p& iParam, const ZRef<ChannerW_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iParam, iChanner->GetChanW())
		{}

	template <class Param_p>
	ChannerW_FT(const Param_p& iParam, const ZRef<ChannerW_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iParam, iChanner->GetChanW())
		{}

	ChannerW_FT(const ZRef<ChannerW_t>& iChanner)
	:	fChannerReal(iChanner)
	,	fChan(iChanner->GetChanW())
		{}

// From ChannerW
	virtual const ChanW_t& GetChanW() { return fChan; }

// Our protocol
	ChanW_Actual& GetChan() { return fChan; }

protected:
	ZRef<ChannerW_t> fChannerReal;
	ChanW_Actual fChan;
	};

} // namespace ZooLib

#endif // __ZooLib_Channer_h__
