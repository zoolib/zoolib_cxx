/* ------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZCommer.h"

#include "ZLog.h"
#include "ZThreadSimple.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer

ZCommer::ZCommer()
:	fMutex("ZCommer::fMutex"),
	fAttachedReader(false),
	fAttachedWriter(false)
	{}

ZCommer::~ZCommer()
	{}

void ZCommer::RunnerAttached(ZStreamReaderRunner* iRunner)
	{
	ZStreamReader::RunnerAttached(iRunner);

	ZMutexLocker locker(fMutex);
	ZAssert(!fAttachedReader);
	fAttachedReader = true;
	fCondition.Broadcast();

	if (fAttachedWriter && fAttachedReader)
		this->Attached();
	}

void ZCommer::RunnerDetached(ZStreamReaderRunner* iRunner)
	{
	ZStreamReader::RunnerDetached(iRunner);

	ZMutexLocker locker(fMutex);
	ZAssert(fAttachedReader);
	fAttachedReader = false;
	fCondition.Broadcast();

	if (!fAttachedWriter && !fAttachedReader)
		{
		locker.Release();
		this->Detached();
		}
	}

void ZCommer::RunnerAttached(ZStreamWriterRunner* iRunner)
	{
	ZStreamWriter::RunnerAttached(iRunner);

	ZMutexLocker locker(fMutex);
	ZAssert(!fAttachedWriter);
	fAttachedWriter = true;
	fCondition.Broadcast();

	if (fAttachedWriter && fAttachedReader)
		this->Attached();
	}

void ZCommer::RunnerDetached(ZStreamWriterRunner* iRunner)
	{
	ZStreamWriter::RunnerDetached(iRunner);

	ZMutexLocker locker(fMutex);
	ZAssert(fAttachedWriter);
	fAttachedWriter = false;
	fCondition.Broadcast();

	if (!fAttachedWriter && !fAttachedReader)
		{
		locker.Release();
		this->Detached();
		}
	}

void ZCommer::Attached()
	{}

void ZCommer::Detached()
	{}

void ZCommer::Wake()
	{
	ZStreamWriter::Wake();
	}

void ZCommer::WaitTillDetached()
	{
	ZMutexLocker locker(fMutex);
	while (fAttachedWriter || fAttachedReader)
		fCondition.Wait(fMutex);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

void sStartReaderRunner(ZStreamReader* iStreamReader, ZRef<ZStreamerR> iSR)
	{
	ZStreamReaderRunner_Threaded* theRR = new ZStreamReaderRunner_Threaded(iStreamReader, iSR);
	theRR->Start();
	}

void sStartWriterRunner(ZStreamWriter* iStreamWriter, ZRef<ZStreamerW> iSW)
	{
	ZStreamWriterRunner_Threaded* theWR = new ZStreamWriterRunner_Threaded(iStreamWriter, iSW);
	theWR->Start();
	}

void sStartRunners(ZCommer* iCommer,
	ZRef<ZStreamerR> iSR, ZRef<ZStreamerW> iSW)
	{
	sStartReaderRunner(iCommer, iSR);
	sStartWriterRunner(iCommer, iSW);
	}
