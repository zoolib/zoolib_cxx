/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZStreamerRWFactory_Preconnected__
#define __ZStreamerRWFactory_Preconnected__ 1
#include "zconfig.h"

#include "zoolib/ZStreamer.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory_Preconnected

class ZStreamerRWFactory_Preconnected : public ZStreamerRWFactory
	{
public:
	ZStreamerRWFactory_Preconnected(ZRef<ZStreamerRW> iSRW, ZRef<ZStreamerRWFactory> iFactory);
	virtual ~ZStreamerRWFactory_Preconnected();

// From ZStreamerRWFactory
	virtual ZRef<ZStreamerRW> MakeStreamerRW();

private:
	ZRef<ZStreamerRW> fSRW;
	ZRef<ZStreamerRWFactory> fFactory;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZStreamerRWFactory_Preconnected__
