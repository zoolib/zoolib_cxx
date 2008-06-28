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

#include "zoolib/ZStreamerRWFactory_Preconnected.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZThread.h"

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory_Preconnected

ZStreamerRWFactory_Preconnected::ZStreamerRWFactory_Preconnected(
	ZRef<ZStreamerRW> iSRW, ZRef<ZStreamerRWFactory> iFactory)
:	fSRW(iSRW),
	fFactory(iFactory)
	{}

ZStreamerRWFactory_Preconnected::~ZStreamerRWFactory_Preconnected()
	{}

ZRef<ZStreamerRW> ZStreamerRWFactory_Preconnected::MakeStreamerRW()
	{
	if (ZRef<ZStreamerRW> theSRW = fSRW)
		{
		fSRW.Clear();
		return theSRW;
		}

	if (fFactory)
		return fFactory->MakeStreamerRW();

	return ZRef<ZStreamerRW>();
	}
