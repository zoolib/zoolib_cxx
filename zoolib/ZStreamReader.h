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

#ifndef __ZStreamReader__
#define __ZStreamReader__
#include "zconfig.h"

#include "ZStreamer.h"
#include "ZThread.h"

class ZStreamReaderRunner;

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReader

class ZStreamReader
	{
	friend class ZStreamReaderRunner;
protected:
	ZStreamReader();
	~ZStreamReader();

public:
// Our protocol
	virtual bool Read(const ZStreamR& iStreamR) = 0;

	virtual void RunnerAttached(ZStreamReaderRunner* iRunner);
	virtual void RunnerDetached(ZStreamReaderRunner* iRunner);

	virtual void StreamEstablished();
	virtual void StreamFailed();

private:
	ZStreamReaderRunner* fRunner;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReaderRunner

class ZStreamReaderRunner
	{
protected:
// Called by subclasses
	void pAttachStreamReader(ZStreamReader* iStreamReader);
	void pDetachStreamReader(ZStreamReader* iStreamReader);

	void pEstablished(ZStreamReader* iStreamReader);
	void pFailed(ZStreamReader* iStreamReader);

	bool pRead(ZStreamReader* iStreamReader, const ZStreamR& iStreamR);
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamReaderRunner_Threaded

class ZStreamReaderRunner_Threaded : public ZStreamReaderRunner
	{
public:
	ZStreamReaderRunner_Threaded(ZStreamReader* iStreamReader, ZRef<ZStreamerR> iSR);
	~ZStreamReaderRunner_Threaded();

	void Start();

private:
	void pRun();
	static void spRun(ZStreamReaderRunner_Threaded* iRunner);

	ZStreamReader* fStreamReader;
	ZRef<ZStreamerR> fSR;
	};

#endif // __ZStreamReader__
