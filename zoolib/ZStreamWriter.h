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

#ifndef __ZStreamWriter__
#define __ZStreamWriter__
#include "zconfig.h"

#include "ZStreamer.h"
#include "ZSleeper.h"
#include "ZThread.h"

class ZStreamWriterRunner;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriter

class ZStreamWriter
	{
	friend class ZStreamWriterRunner;
protected:
	ZStreamWriter();
	~ZStreamWriter();

public:
// Our protocol
	virtual bool Write(const ZStreamW& iStreamW) = 0;

	virtual void RunnerAttached(ZStreamWriterRunner* iRunner);
	virtual void RunnerDetached(ZStreamWriterRunner* iRunner);

	virtual void StreamEstablished();
	virtual void StreamFailed();

	void Wake();
	void WakeAt(ZTime iSystemTime);

private:
	ZStreamWriterRunner* fRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriterRunner

class ZStreamWriterRunner
	{
	friend class ZStreamWriter;

protected:
// Called by subclasses
	void pAttachStreamWriter(ZStreamWriter* iStreamWriter);
	void pDetachStreamWriter(ZStreamWriter* iStreamWriter);

	void pEstablished(ZStreamWriter* iStreamWriter);
	void pFailed(ZStreamWriter* iStreamWriter);

	bool pWrite(ZStreamWriter* iStreamWriter, const ZStreamW& iStreamW);

// Called by ZStreamWriter instances.
	virtual void WakeAt(ZStreamWriter* iStreamWriter, ZTime iSystemTime) = 0;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamWriterRunner_Threaded

class ZStreamWriterRunner_Threaded : public ZStreamWriterRunner
	{
public:
	ZStreamWriterRunner_Threaded(ZStreamWriter* iStreamWriter, ZRef<ZStreamerW> iSW);
	~ZStreamWriterRunner_Threaded();

	void Start();

// From ZStreamWriterRunner
	virtual void WakeAt(ZStreamWriter* iStreamWriter, ZTime iSystemTime);

private:
	void pRun();
	static void spRun(ZStreamWriterRunner_Threaded* iRunner);

	ZMutex fMutex;
	ZCondition fCondition;
	ZStreamWriter* fStreamWriter;
	ZRef<ZStreamerW> fSW;
	ZTime fNextWake;
	};

#endif // __ZStreamWriter__
