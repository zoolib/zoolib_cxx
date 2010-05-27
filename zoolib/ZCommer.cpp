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

#include "zoolib/ZCommer.h"

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

void ZCommer::ReadFinished()
	{
	ZGuardRMtx guard(fMtx);
	ZAssert(fReadStarted);
	fReadStarted = false;
	fCnd.Broadcast();
	if (!fReadStarted && !fWriteStarted)
		{
		guard.Release();
		this->Finished();
		}
	else
		{
		guard.Release();
		}

	ZStreamerReader::ReadFinished();
	}

void ZCommer::WriteStarted()
	{
	ZStreamerWriter::WriteStarted();

	ZGuardRMtx guard(fMtx);
	ZAssert(!fWriteStarted);
	fWriteStarted = true;
	fCnd.Broadcast();
	if (fReadStarted && fWriteStarted)
		{
		guard.Release();
		this->Started();
		}
	}

void ZCommer::WriteFinished()
	{
	ZGuardRMtx guard(fMtx);
	ZAssert(fWriteStarted);
	fWriteStarted = false;
	fCnd.Broadcast();
	if (!fReadStarted && !fWriteStarted)
		{
		guard.Release();
		this->Finished();
		}
	else
		{
		guard.Release();
		}

	ZStreamerWriter::WriteFinished();
	}

void ZCommer::Started()
	{}

void ZCommer::Finished()
	{}

void ZCommer::WaitTillFinished()
	{
	ZAcqMtx locker(fMtx);
	while (fReadStarted || fWriteStarted)
		fCnd.Wait(fMtx);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

void sStartCommerRunners(ZRef<ZCommer> iCommer)
	{
	sStartWorkerRunner(iCommer.StaticCast<ZStreamerReader>());
	sStartWorkerRunner(iCommer.StaticCast<ZStreamerWriter>());
	}

} // namespace ZooLib
