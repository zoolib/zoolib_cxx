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

#include "zoolib/ZStreamerReader.h"
#include "zoolib/ZStreamerWriter.h"
#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer

class ZCommer
:	public ZStreamerReader,
	public ZStreamerWriter
	{
public:
	ZCommer(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW);
	virtual ~ZCommer();

// From ZStreamerReader
	virtual void ReadStarted();
	virtual void ReadFinished();

// From ZStreamerWriter
	virtual void WriteStarted();
	virtual void WriteFinished();

// Our protocol
	void Wake() { ZStreamerWriter::Wake(); }
	void WakeAt(ZTime iSystemTime) { ZStreamerWriter::WakeAt(iSystemTime); }

	virtual void Started();
	virtual void Finished();

	void WaitTillFinished();

private:
	ZMtx fMtx;
	ZCnd fCnd;
	bool fReadStarted;
	bool fWriteStarted;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZCommer utility methods

void sStartCommerRunners(ZRef<ZCommer> iCommer);

NAMESPACE_ZOOLIB_END

#endif // __ZCommer__
