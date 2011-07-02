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

#ifndef __ZStreamerFactory_Precon__
#define __ZStreamerFactory_Precon__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory_Precon

class ZStreamerRWFactory_Precon : public ZStreamerRWFactory
	{
public:
	ZStreamerRWFactory_Precon(ZRef<ZStreamerRW> iSRW, ZRef<ZStreamerRWFactory> iFactory);
	virtual ~ZStreamerRWFactory_Precon();

// From ZStreamerRWFactory
	virtual ZRef<ZStreamerRW> MakeStreamerRW();

private:
	ZRef<ZStreamerRW> fSRW;
	ZRef<ZStreamerRWFactory> fFactory;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWConFactory_Precon

class ZStreamerRWConFactory_Precon : public ZStreamerRWConFactory
	{
public:
	ZStreamerRWConFactory_Precon
		(ZRef<ZStreamerRWCon> iSRWCon, ZRef<ZStreamerRWConFactory> iFactory);
	virtual ~ZStreamerRWConFactory_Precon();

// From ZStreamerRWConFactory
	virtual ZRef<ZStreamerRWCon> MakeStreamerRWCon();

private:
	ZRef<ZStreamerRWCon> fSRWCon;
	ZRef<ZStreamerRWConFactory> fFactory;
	};

} // namespace ZooLib

#endif // __ZStreamerFactory_Precon__
