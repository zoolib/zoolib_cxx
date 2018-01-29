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

#ifndef __ZooLib_Channer_XX_Wrapper_h__
#define __ZooLib_Channer_XX_Wrapper_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChannerRW_Wrapper

template <class EE>
class ChannerRW_Wrapper
:	public ChannerRW<EE>
	{
public:
	ChannerRW_Wrapper(
		const ZRef<Channer<ChanR<EE> > >& iChannerR,
		const ZRef<Channer<ChanW<EE> > >& iChannerW)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
		{}

	virtual size_t QRead(byte* oDest, size_t iCount)
		{ return sQRead(*fChannerR, oDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return sSkip(*fChannerR, iCount); }

	virtual size_t QWrite(const byte* iSource, size_t iCount)
		{ return sQWrite(*fChannerW, iSource, iCount); }

	virtual void Flush()
		{ sFlush(*fChannerW); }

private:
	const ZRef<Channer<ChanR<EE> > > fChannerR;
	const ZRef<Channer<ChanW<EE> > > fChannerW;
	};

template <class EE>
ZRef<ChannerRW<EE> > sChanner_Wrapper(
	const ZRef<Channer<ChanR<EE> > >& iChannerR,
	const ZRef<Channer<ChanW<EE> > >& iChannerW)
	{ return new ChannerRW_Wrapper<EE>(iChannerR, iChannerW); }

// =================================================================================================
#pragma mark -
#pragma mark ChannerRWClose_Wrapper

template <class EE>
class ChannerRWClose_Wrapper
:	public ChannerConnection<EE>
	{
public:
	ChannerRWClose_Wrapper(
		const ZRef<Channer<ChanR<EE> > >& iChannerR,
		const ZRef<Channer<ChanW<EE> > >& iChannerW,
		const ZRef<ChannerClose>& iChannerClose)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
	,	fChannerClose(iChannerClose)
		{}

	virtual size_t QRead(byte* oDest, size_t iCount)
		{ return sRead(*fChannerR, oDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return sSkip(*fChannerR, iCount); }

	virtual size_t QWrite(const byte* iSource, size_t iCount)
		{ return sWrite(*fChannerW, iSource, iCount); }

	virtual void Flush()
		{ sFlush(*fChannerW); }

	virtual void Abort()
		{ return sAbort(*fChannerClose); }

	virtual bool DisconnectRead(double iTimeout)
		{ return sDisconnectRead(*fChannerClose, iTimeout); }

	virtual void DisconnectWrite()
		{ return sDisconnectWrite(*fChannerClose); }

private:
	const ZRef<Channer<ChanR<EE> > > fChannerR;
	const ZRef<Channer<ChanW<EE> > > fChannerW;
	const ZRef<ChannerClose> fChannerClose;
	};

template <class EE>
ZRef<ChannerRWClose<EE> > sChanner_Wrapper(
	const ZRef<Channer<ChanR<EE> > >& iChannerR,
	const ZRef<Channer<ChanW<EE> > >& iChannerW,
	const ZRef<ChannerClose>& iChannerClose)
	{ return new ChannerRWClose_Wrapper<EE>(iChannerR, iChannerW, iChannerClose); }

} // namespace ZooLib

#endif // __ZooLib_Channer_XX_Wrapper_h__
