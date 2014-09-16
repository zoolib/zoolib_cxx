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
#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"
#include "zoolib/ThreadSafe.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Chan_XX_FlushOnRead

template <class XX>
class Chan_XX_FlushOnRead
:	public ChanR<XX>
,	public ChanW<XX>
	{
public:
	typedef XX Elmt_t;

	Chan_XX_FlushOnRead(const ChanR<Elmt_t>& iChanR, const ChanW<Elmt_t>& iChanW)
	:	fChanR(iChanR)
	,	fChanW(iChanW)
		{}

// From ChanR
	virtual size_t Read(Elmt_t* oDest, size_t iCount)
		{
		if (ZMACRO_ThreadSafe_Swap(fLastWasWrite, 0))
			{
			ZAcqMtx acq(fMutex);
			sFlush(fChanW);
			}
		return sRead(oDest, iCount, fChanR);
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
	virtual size_t Write(const Elmt_t* iSource, size_t iCount)
		{
		ZMACRO_ThreadSafe_Set(fLastWasWrite, 1);
		ZAcqMtx acq(fMutex);
		return sWrite(iSource, iCount, fChanW);
		}

protected:
	ThreadSafe_t fLastWasWrite;
	ZMtx fMutex;
	const ChanR<Elmt_t>& fChanR;
	const ChanW<Elmt_t>& fChanW;
	};

// =================================================================================================
// MARK: - Channer_XX_FlushOnRead

template <class XX>
class Channer_XX_FlushOnRead
:	public virtual Channer<ChanR<XX> >
,	public virtual Channer<ChanW<XX> >
	{
public:
	typedef XX Elmt_t;

	Channer_XX_FlushOnRead(const ZRef<Channer<ChanR<Elmt_t> > >& iChannerR,
		const ZRef<Channer<ChanW<Elmt_t> > >& iChannerW)
	:	fChannerR(iChannerR)
	,	fChannerW(iChannerW)
	,	fChan(sGetChan(fChannerR), sGetChan(fChannerW))
		{}

	virtual void GetChan(const ChanR<Elmt_t>*& oChanPtr)
		{ oChanPtr = &fChan; }

	virtual void GetChan(const ChanW<Elmt_t>*& oChanPtr)
		{ oChanPtr = &fChan; }

private:
	const ZRef<Channer<ChanR<Elmt_t> > > fChannerR;
	const ZRef<Channer<ChanW<Elmt_t> > > fChannerW;
	const Chan_XX_FlushOnRead<Elmt_t> fChan;
	};

template <class XX>
ZRef<Channer_XX_FlushOnRead<XX> > sChanner_XX_FlushOnRead(
	const ZRef<Channer<ChanR<XX> > >& iChannerR,
	const ZRef<Channer<ChanW<XX> > >& iChannerW)
	{ return new Channer_XX_FlushOnRead<XX>(iChannerR, iChannerW); }

} // namespace ZooLib

#endif // __ZooLib_Chan_XX_FlushOnRead_h__
