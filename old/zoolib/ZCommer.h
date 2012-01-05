/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZCommer__
#define __ZCommer__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer

class ZCommer
:	public ZCounted
	{
public:
	ZCommer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZCommer();

// Our protocol
	virtual bool Read(const ZStreamR& iStreamR);
	virtual bool Write(const ZStreamW& iStreamW);

	void WaitTillFinished();

	void WakeWrite();

	typedef ZRef<ZCommer> ZRef_ZCommer;
	typedef ZCallable<void(ZRef_ZCommer)> Callable_t;
	void SetCallable_Finished(ZRef<Callable_t> iCallable);

private:
	ZMtx fMtx;
	ZCnd fCnd;
	bool fReadStarted;
	bool fWriteStarted;
	ZRef<Callable_t> fCallable_Finished;
	ZRef<ZStreamerR> fStreamerR;
	ZRef<ZStreamerW> fStreamerW;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

void sStartCommerRunners(ZRef<ZCommer> iCommer);

} // namespace ZooLib

#endif // __ZCommer__
