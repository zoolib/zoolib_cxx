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

#ifndef __ZooLib_ChannerCombo_h__
#define __ZooLib_ChannerCombo_h__ 1
#include "zconfig.h"

#include "zoolib/ChannerXX.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ChannerComboRW

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
#pragma mark -
#pragma mark ChannerComboRWClose

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

#endif // __ZooLib_ChannerCombo_h__
