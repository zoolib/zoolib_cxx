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

#ifndef __ZooLib_Chan_Channer_h__
#define __ZooLib_Chan_Channer_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"

namespace ZooLib {

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
	virtual size_t Read(Elmt_p* oDest, size_t iCount)
		{ return sRead(fChanner->GetChanR(), oDest, iCount); }

	virtual uint64 Skip(uint64 iCount)
		{ return sSkip(fChanner->GetChanR(), iCount); }

	virtual size_t Readable()
		{ return sReadable(fChanner->GetChanR()); }

private:
	const ZRef<ChannerR<Elmt_p> > fChanner;
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
		{ return sWrite(fChanner->GetChanW(), iSource, iCount); }

	virtual void Flush()
		{ sFlush(fChanner->GetChanW()); }

private:
	const ZRef<ChannerW<Elmt_p> > fChanner;
	};

} // namespace ZooLib

#endif // __ZooLib_Chan_Channer_h__
