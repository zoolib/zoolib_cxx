static const char rcsid[] =
"@(#) $Id: ZStreamWriter.cpp,v 1.2 2008/02/29 10:48:24 agreen Exp $";

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

#include "ZStreamWriter.h"
#include "ZThreadSimple.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriter

ZStreamWriter::ZStreamWriter()
:	fRunner(nil)
	{}

ZStreamWriter::~ZStreamWriter()
	{
	// By the time a streamWriter's destructor is called, it must have been
	// detached from its runner. That's the subclass' responsibility.
	ZAssert(!fRunner);
	}

void ZStreamWriter::RunnerAttached(ZStreamWriterRunner* iRunner)
	{}

void ZStreamWriter::RunnerDetached(ZStreamWriterRunner* iRunner)
	{}

void ZStreamWriter::StreamEstablished()
	{}

void ZStreamWriter::StreamFailed()
	{}

void ZStreamWriter::Wake()
	{
	if (fRunner)
		fRunner->WakeAt(this, ZTime::sSystem());
	}

void ZStreamWriter::WakeAt(ZTime iSystemTime)
	{
	if (fRunner)
		fRunner->WakeAt(this, iSystemTime);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriterRunner

void ZStreamWriterRunner::pAttachStreamWriter(ZStreamWriter* iStreamWriter)
	{
	ZAssert(iStreamWriter);
	ZAssert(!iStreamWriter->fRunner);

	iStreamWriter->fRunner = this;

	iStreamWriter->RunnerAttached(this);
	}

void ZStreamWriterRunner::pDetachStreamWriter(ZStreamWriter* iStreamWriter)
	{
	ZAssert(iStreamWriter);
	ZAssert(iStreamWriter->fRunner = this);

	iStreamWriter->fRunner = nil;

	iStreamWriter->RunnerDetached(this);
	}

void ZStreamWriterRunner::pEstablished(ZStreamWriter* iStreamWriter)
	{ iStreamWriter->StreamEstablished(); }

void ZStreamWriterRunner::pFailed(ZStreamWriter* iStreamWriter)
	{ iStreamWriter->StreamFailed(); }

bool ZStreamWriterRunner::pWrite(ZStreamWriter* iStreamWriter, const ZStreamW& iStreamW)
	{ return iStreamWriter->Write(iStreamW); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriterRunner_Threaded

ZStreamWriterRunner_Threaded::ZStreamWriterRunner_Threaded(
	ZStreamWriter* iStreamWriter, ZRef<ZStreamerW> iSW)
:	fStreamWriter(iStreamWriter),
	fSW(iSW),
	fNextWake(ZTime::sSystem())
	{}

ZStreamWriterRunner_Threaded::~ZStreamWriterRunner_Threaded()
	{
	ZAssert(!fStreamWriter);
	}

void ZStreamWriterRunner_Threaded::Start()
	{
	this->pAttachStreamWriter(fStreamWriter);
	(new ZThreadSimple<ZStreamWriterRunner_Threaded*>(spRun, this))->Start();
	}

void ZStreamWriterRunner_Threaded::WakeAt(ZStreamWriter* iStreamWriter, ZTime iSystemTime)
	{
	ZMutexLocker locker(fMutex);
	if (fNextWake > iSystemTime)
		{
		fNextWake = iSystemTime;
		fCondition.Broadcast();
		}
	}

void ZStreamWriterRunner_Threaded::pRun()
	{
	const ZStreamW& theStreamW = fSW->GetStreamW();

	this->pEstablished(fStreamWriter);

	for (;;)
		{
		ZMutexLocker locker(fMutex);
		for (;;)
			{
			const ZTime now = ZTime::sSystem();
			if (fNextWake <= now)
				{
				fNextWake = now + 3600;
				break;
				}

			fCondition.Wait(fMutex, bigtime_t(1000000 * (fNextWake - now)));
			}
		locker.Release();

		try
			{
			if (!this->pWrite(fStreamWriter, theStreamW))
				break;
			}
		catch (...)
			{
			break;
			}
		}

	this->pFailed(fStreamWriter);

	this->pDetachStreamWriter(fStreamWriter);
	fStreamWriter = nil;

	delete this;
	}

void ZStreamWriterRunner_Threaded::spRun(ZStreamWriterRunner_Threaded* iRunner)
	{ iRunner->pRun(); }
