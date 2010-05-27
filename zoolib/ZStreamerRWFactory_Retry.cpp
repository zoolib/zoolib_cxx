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

#include "zoolib/ZStreamerRWFactory_Retry.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory_Retry

ZStreamerRWFactory_Retry::ZStreamerRWFactory_Retry(
	ZRef<ZStreamerRWFactory> iStreamerRWFactory, size_t iCount, double iInterval)
:	fStreamerRWFactory(iStreamerRWFactory),
	fCount(iCount),
	fInterval(iInterval)
	{}

ZStreamerRWFactory_Retry::~ZStreamerRWFactory_Retry()
	{}

ZRef<ZStreamerRW> ZStreamerRWFactory_Retry::MakeStreamerRW()
	{
	for (size_t attempt = 1; /*no test*/; ++attempt)
		{
		ZRef<ZStreamerRWFactory> theFactory = fStreamerRWFactory;
		if (!theFactory)
			{
			if (ZLOG(s, eInfo, "ZStreamerRWFactory_Retry"))
				s.Writef("MakeStreamerRW, no factory");
			break;
			}

		if (ZLOG(s, eDebug, "ZStreamerRWFactory_Retry"))
			{
			s.Writef("Connect, attempt %d of %d",
				attempt, fCount);
			}

		if (ZRef<ZStreamerRW> theSRW = theFactory->MakeStreamerRW())
			{
			if (ZLOG(s, eInfo, "ZStreamerRWFactory_Retry"))
				s << "Connect succeeded";

			return theSRW;
			}

		if (!fCount || attempt < fCount)
			{
			if (ZLOG(s, eDebug, "ZStreamerRWFactory_Retry"))
				{
				s.Writef("Connect, couldn't connect, sleeping for %g seconds",
					fInterval);
				}

			ZThread::sSleep(fInterval);
			}
		else
			{
			if (ZLOG(s, eInfo, "ZStreamerRWFactory_Retry"))
				s.Writef("Connect failed after %d attempts", fCount);

			break;
			}
		}

	return ZRef<ZStreamerRW>();
	}

void ZStreamerRWFactory_Retry::Abort()
	{
	fStreamerRWFactory.Clear();
	}

} // namespace ZooLib
