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

#include "zoolib/ZStreamReader.h"
#include "zoolib/ZThreadSimple.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReader

ZStreamReader::ZStreamReader()
:	fRunner(nil)
	{}

ZStreamReader::~ZStreamReader()
	{
	// By the time a streamReader's destructor is called, it must have been
	// detached from its runner. That's the subclass' responsibility.
	ZAssert(!fRunner);
	}

void ZStreamReader::RunnerAttached(ZStreamReaderRunner* iRunner)
	{}

void ZStreamReader::RunnerDetached(ZStreamReaderRunner* iRunner)
	{}

void ZStreamReader::StreamEstablished()
	{}

void ZStreamReader::StreamFailed()
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReaderRunner

void ZStreamReaderRunner::pAttachStreamReader(ZStreamReader* iStreamReader)
	{
	ZAssert(iStreamReader);
	ZAssert(!iStreamReader->fRunner);

	iStreamReader->fRunner = this;

	iStreamReader->RunnerAttached(this);
	}

void ZStreamReaderRunner::pDetachStreamReader(ZStreamReader* iStreamReader)
	{
	ZAssert(iStreamReader);
	ZAssert(iStreamReader->fRunner = this);

	iStreamReader->fRunner = nil;

	iStreamReader->RunnerDetached(this);
	}

void ZStreamReaderRunner::pEstablished(ZStreamReader* iStreamReader)
	{ iStreamReader->StreamEstablished(); }

void ZStreamReaderRunner::pFailed(ZStreamReader* iStreamReader)
	{ iStreamReader->StreamFailed(); }

bool ZStreamReaderRunner::pRead(ZStreamReader* iStreamReader, const ZStreamR& iStreamR)
	{ return iStreamReader->Read(iStreamR); }

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReaderRunner_Threaded

ZStreamReaderRunner_Threaded::ZStreamReaderRunner_Threaded(
	ZStreamReader* iStreamReader, ZRef<ZStreamerR> iSR)
:	fStreamReader(iStreamReader),
	fSR(iSR)
	{}

ZStreamReaderRunner_Threaded::~ZStreamReaderRunner_Threaded()
	{}

void ZStreamReaderRunner_Threaded::Start()
	{
	this->pAttachStreamReader(fStreamReader);
	(new ZThreadSimple<ZStreamReaderRunner_Threaded*>(spRun, this))->Start();
	}

void ZStreamReaderRunner_Threaded::pRun()
	{
	const ZStreamR& theStreamR = fSR->GetStreamR();

	this->pEstablished(fStreamReader);

	for (;;)
		{
		try
			{
			if (!this->pRead(fStreamReader, theStreamR))
				break;
			}
		catch (...)
			{
			break;
			}
		}

	this->pFailed(fStreamReader);

	this->pDetachStreamReader(fStreamReader);

	delete this;
	}

void ZStreamReaderRunner_Threaded::spRun(ZStreamReaderRunner_Threaded* iRunner)
	{ iRunner->pRun(); }

NAMESPACE_ZOOLIB_END
