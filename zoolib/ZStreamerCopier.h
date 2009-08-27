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

#ifndef __ZStreamerCopier__
#define __ZStreamerCopier__
#include "zconfig.h"

#include "zoolib/ZStreamer.h"
#include "zoolib/ZTask.h"
#include "zoolib/ZWorker.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerCopier

class ZStreamerCopier
:	public ZWorker,
	public ZTask
	{
public:
	ZStreamerCopier(ZRef<ZTaskOwner> iTaskOwner, ZRef<ZStreamerRCon> iStreamerRCon,
		ZRef<ZStreamerWCon> iStreamerWCon);

	ZStreamerCopier(ZRef<ZTaskOwner> iTaskOwner, ZRef<ZStreamerRCon> iStreamerRCon,
		ZRef<ZStreamerWCon> iStreamerWCon, size_t iChunkSize);

	virtual ~ZStreamerCopier();

// From ZWorker
	virtual void RunnerDetached();
	virtual bool Work();

// From ZTask
	virtual void Kill();

private:
	ZRef<ZStreamerRCon> fStreamerRCon;
	ZRef<ZStreamerWCon> fStreamerWCon;
	size_t fChunkSize;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamerCopier__
