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

#include "zoolib/ZCaller_Thread.h"
#include "zoolib/ZCommer.h"
#include "zoolib/ZGetSet.h"

#include "zoolib/ZLog.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer

ZCommer::ZCommer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	ZStreamerReader(iStreamerR),
	ZStreamerWriter(iStreamerW),
	fReadStarted(false),
	fWriteStarted(false)
	{}

ZCommer::~ZCommer()
	{ ZAssert(!fReadStarted && !fWriteStarted); }

void ZCommer::ReadStarted()
	{
	ZStreamerReader::ReadStarted();

	ZGuardRMtx guard(fMtx);
	ZAssert(!fReadStarted);
	fReadStarted = true;
	fCnd.Broadcast();
	if (fReadStarted && fWriteStarted)
		{
		guard.Release();
		this->Started();
		}
	}

bool ZStreamerReader::Read(const ZStreamR& iStreamR)
	{ return false; }

bool ZStreamerReader::Write(const ZStreamW& iStreamW)
	{ return false; }

void ZCommer::Started()
	{
	ZGuardRMtx guard(fMtx);
	if (ZRef<Callable_t> theCallable = fCallable_Started)
		{
		guard.Release();
		theCallable->Call(this);
		}
	}

void ZCommer::WaitTillFinished()
	{
	ZAcqMtx locker(fMtx);
	while (fReadStarted || fWriteStarted)
		fCnd.Wait(fMtx);
	}

void ZCommer::SetCallable_Finished(ZRef<Callable_t> iCallable)
	{
	ZAcqMtx locker(fMtx);
	return sGetSet(fCallable_Finished, iCallable);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

void sStartCommerRunners(ZRef<ZCommer> iCommer)
	{
	ZRef<ZCaller> theCaller = new ZCaller_Thread;
	iCommer.StaticCast<ZStreamerReader>()->Attach(theCaller);
	iCommer.StaticCast<ZStreamerWriter>()->Attach(theCaller);
	}

} // namespace ZooLib
