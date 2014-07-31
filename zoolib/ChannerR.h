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

#ifndef __ZooLib_ChannerR_h__
#define __ZooLib_ChannerR_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ZCounted.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChannerR

template <class ChanR_p>
class ChannerR
:	public ZCounted
	{
public:
	typedef ChanR_p ChanR_t;

// Our protocol
	virtual const ChanR& GetChanR() = 0;
	};

// =================================================================================================
// MARK: - ChanR_Channer

template <class ChannerR_p>
class ChanR_Channer
:	public ZRef<ChannerR_p>
,	public ChanR<typedef ChannerR_p::ChanR_t::Elmt>
	{
public:
	typedef ChannerR_p ChannerR_t;

	typedef ChannerR_t::Elmt Elmt;

	ChanR_Channer(const ZRef<ChannerR_p>& iChanner)
	:	fChanner(iChanner)
		{}

	// From ChanR<ChanR_p::Elmt>
	virtual size_t Read(Elmt* iDest, size_t iCount)
		{ return fChanner->GetChanR().Read(iDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return fChanner->GetChanR().Skip(iCount); }

	virtual size_t Readable()
		{ return fChanner->GetChanR().Readable(); }

private:
	const ZRef<ChannerR_p> fChanner;
	};

// =================================================================================================
// MARK: - ChannerR_T

template <class ChanR_p>
class ChannerR_T : public ChannerR
	{
public:
	ChannerR_T() {}

	virtual ~ChannerR_T() {}

	template <class Param_p>
	ChannerR_T(Param_p& iParam) : fChan(iParam) {}

	template <class Param_p>
	ChannerR_T(const Param_p& iParam) : fChan(iParam) {}

// From ChannerR
	virtual const ChanR& GetChanR() { return fChan; }

// Our protocol
	ChanR_p& GetChan() { return fChan; }

protected:
	ChanR_p fChan;
	};

template <class Chan_p>
ZRef<ChannerR> sChannerR_T()
	{ return new ChannerR_T<Chan_p>; }

template <class Chan_p, class Param_p>
ZRef<ChannerR> sChannerR_T(Param_p& iParam)
	{ return new ChannerR_T<Chan_p>(iParam); }

// =================================================================================================
// MARK: - ChannerR_FT

template <class Chan_p>
class ChannerR_FT : public ChannerR
	{
protected:
	ChannerR_FT() {}

public:
	virtual ~ChannerR_FT() {}

	template <class P>
	ChannerR_FT(Param_p& iParam, const ZRef<ChannerR>& iChanner)
	:	fChannerReal(iChanner),
		fChan(iParam, iChanner->GetChanR())
		{}

	template <class Param_p>
	ChannerR_FT(const Param_p& iParam, const ZRef<ChannerR>& iChanner)
	:	fChannerReal(iChanner),
		fChan(iParam, iChanner->GetChanR())
		{}

	ChannerR_FT(const ZRef<ChannerR>& iChanner)
	:	fChannerReal(iChanner),
		fChan(iChanner->GetChanR())
		{}

// From ChannerR
	virtual const ChanR& GetChanR() { return fChan; }

// Our protocol
	Chan_p& GetChan() { return fChan; }

protected:
	ZRef<ChannerR> fChannerReal;
	Chan_p fChan;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_h__
