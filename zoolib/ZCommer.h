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

#include "zoolib/ZStreamer.h"

#include "zoolib/ZStreamReader.h"
#include "zoolib/ZStreamWriter.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer

class ZCommer : public ZStreamReader, public ZStreamWriter
	{
protected:
	ZCommer();
	~ZCommer();

public:
// From ZStreamReader
	virtual void RunnerAttached(ZStreamReaderRunner* iRunner);
	virtual void RunnerDetached(ZStreamReaderRunner* iRunner);
	virtual bool Read(const ZStreamR& iStreamR) = 0;

// From ZStreamWriter
	virtual void RunnerAttached(ZStreamWriterRunner* iRunner);
	virtual void RunnerDetached(ZStreamWriterRunner* iRunner);
	virtual bool Write(const ZStreamW& iStreamW) = 0;

// Our protocol
	virtual void Attached();
	virtual void Detached();

	void Wake();
	void WaitTillDetached();

private:
	ZMutex fMutex;
	ZCondition fCondition;
	bool fAttachedReader;
	bool fAttachedWriter;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

// Here for now till I find a better home.
void sStartReaderRunner(ZStreamReader* iStreamReader, ZRef<ZStreamerR> iSR);

void sStartWriterRunner(ZStreamWriter* iStreamWriter, ZRef<ZStreamerW> iSW);

void sStartRunners(ZCommer* iCommer, ZRef<ZStreamerR> iSR, ZRef<ZStreamerW> iSW);

#endif // __ZCommer__
