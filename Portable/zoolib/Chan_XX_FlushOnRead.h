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

#ifndef __ZooLib_Chan_XX_FlushOnRead_h__
#define __ZooLib_Chan_XX_FlushOnRead_h__ 1
#include "zconfig.h"

#include "zoolib/Channer.h"
//#include "zoolib/ChanR.h"
//#include "zoolib/ChanW.h"
#include "zoolib/ThreadSafe.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Chan_XX_FlushOnRead

template <class EE>
class Chan_XX_FlushOnRead
:	public ChanRW<EE>
	{
public:
	Chan_XX_FlushOnRead(const ChanR<EE>& iChanR, const ChanW<EE>& iChanW)
	:	fChanR(iChanR)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t Read(EE* oDest, size_t iCount)
		{
		if (ZMACRO_ThreadSafe_Swap(fLastWasWrite, 0))
			{
			ZAcqMtx acq(fMutex);
			sFlush(fChanW);
			}
		return sRead(fChanR, oDest, iCount);
		}

	virtual size_t Readable()
		{
		if (ZMACRO_ThreadSafe_Swap(fLastWasWrite, 0))
			{
			ZAcqMtx acq(fMutex);
			sFlush(fChanW);
			}
		return sReadable(fChanR);
		}

// From ChanW
	virtual size_t Write(const EE* iSource, size_t iCount)
		{
		ZMACRO_ThreadSafe_Set(fLastWasWrite, 1);
		ZAcqMtx acq(fMutex);
		return sWrite(fChanW, iSource, iCount);
		}

protected:
	ThreadSafe_t fLastWasWrite;
	ZMtx fMutex;
	const ChanR<EE>& fChanR;
	const ChanW<EE>& fChanW;
	};

// =================================================================================================
#pragma mark - Channer_XX_FlushOnRead

template <class EE>
class Channer_XX_FlushOnRead
:	public Channer<ChanRW<EE> >
,	public Chan_XX_FlushOnRead<EE>
	{
public:
	Channer_XX_FlushOnRead(const ZRef<Channer<ChanR<EE> > >& iChannerR,
		const ZRef<Channer<ChanW<EE> > >& iChannerW)
	:	Chan_XX_FlushOnRead<EE>(*iChannerR, *iChannerW)
	,	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
		{}

private:
	const ZRef<Channer<ChanR<EE> > > fChannerR;
	const ZRef<Channer<ChanW<EE> > > fChannerW;
	};

template <class EE>
ZRef<Channer_XX_FlushOnRead<EE> > sChanner_XX_FlushOnRead(
	const ZRef<Channer<ChanR<EE> > >& iChannerR,
	const ZRef<Channer<ChanW<EE> > >& iChannerW)
	{ return new Channer_XX_FlushOnRead<EE>(iChannerR, iChannerW); }

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_FlushOnRead_h__
