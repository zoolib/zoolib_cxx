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

#ifndef __ZStreamerRWFactory_Retry_h__
#define __ZStreamerRWFactory_Retry_h__ 1
#include "zconfig.h"

#include "zoolib/Safe.h"

#include "zoolib/ZStreamer.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark ZStreamerRWFactory_Retry

class ZStreamerRWFactory_Retry : public ZStreamerRWFactory
	{
public:
	ZStreamerRWFactory_Retry(
		ZRef<ZStreamerRWFactory> iStreamerRWFactory, size_t iCount, double iInterval);
	virtual ~ZStreamerRWFactory_Retry();

// From ZStreamerRWFactory
	virtual ZRef<ZStreamerRW> MakeStreamerRW();

// Our protocol
	void Abort();

private:
	Safe<ZRef<ZStreamerRWFactory> > fStreamerRWFactory;
	size_t fCount;
	double fInterval;
	};

} // namespace ZooLib

#endif // __ZStreamerRWFactory_Retry_h__
