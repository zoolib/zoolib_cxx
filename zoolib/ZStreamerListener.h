/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#ifndef __ZStreamerListener__
#define __ZStreamerListener__
#include "zconfig.h"

#include "zoolib/ZSafe.h"
#include "zoolib/ZStreamer.h"
#include "zoolib/ZWorker.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerListener

class ZStreamerListener : public ZWorker
	{
public:
	ZStreamerListener(ZRef<ZStreamerRWFactory> iFactory);
	virtual ~ZStreamerListener();

// From ZWorker
	virtual void RunnerAttached();
	virtual void RunnerDetached();

	virtual bool Work();

// Our protocol
	virtual void ListenStarted();
	virtual void ListenFinished();

	virtual bool Connected(ZRef<ZStreamerRW> iStreamer) = 0;

	ZRef<ZStreamerRWFactory> GetFactory();

	void Stop();

protected:
	ZSafe<ZRef<ZStreamerRWFactory> > fFactory;
	};

} // namespace ZooLib

#endif // __ZStreamerListener__
