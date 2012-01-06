/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZBlockStore_h__
#define __ZBlockStore_h__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h" // For ZStreamerRWPos

namespace ZooLib {

class ZBlockStore
	{
private:
	ZBlockStore(const ZBlockStore& other); // Not implemented
	ZBlockStore& operator=(const ZBlockStore& ZBlockStore); // Not implemented

protected:
	ZBlockStore();

public:
	typedef uint32 BlockID;

	virtual ~ZBlockStore();

	virtual void Flush();

	virtual ZRef<ZStreamerRWPos> Create(BlockID& oBlockID) = 0;

	virtual ZRef<ZStreamerR> OpenR(BlockID iBlockID);
	virtual ZRef<ZStreamerRPos> OpenRPos(BlockID iBlockID);

	virtual ZRef<ZStreamerW> OpenW(BlockID iBlockID);
	virtual ZRef<ZStreamerWPos> OpenWPos(BlockID iBlockID);

	virtual ZRef<ZStreamerRWPos> OpenRWPos(BlockID iBlockID) = 0;

	virtual bool Delete(BlockID iBlockID) = 0;
	virtual size_t EfficientSize() = 0;
	};

} // namespace ZooLib

#endif // __ZBlockStore_h__
