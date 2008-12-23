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

#include "zoolib/ZStreamerRWFactory_Buffered.h"
#include "zoolib/ZStream_Buffered.h"

NAMESPACE_ZOOLIB_USING

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerRWFactory_Buffered

ZStreamerRWFactory_Buffered::ZStreamerRWFactory_Buffered(
	ZRef<ZStreamerRWFactory> iStreamerRWFactory, size_t iReadBufferSize, size_t iWriteBufferSize)
:	fStreamerRWFactory(iStreamerRWFactory),
	fReadBufferSize(iReadBufferSize),
	fWriteBufferSize(iWriteBufferSize)
	{}

ZStreamerRWFactory_Buffered::~ZStreamerRWFactory_Buffered()
	{}

ZRef<ZStreamerRW> ZStreamerRWFactory_Buffered::MakeStreamerRW()
	{
	if (fStreamerRWFactory)
		{
		ZRef<ZStreamerRW> theSRW = fStreamerRWFactory->MakeStreamerRW();

		if (theSRW && (fReadBufferSize || fWriteBufferSize))
			{
			ZRef<ZStreamerR> theSR = theSRW;
			if (fReadBufferSize)
				theSR = new ZStreamerR_Buffered(fReadBufferSize, theSR);

			ZRef<ZStreamerW> theSW = theSRW;
			if (fWriteBufferSize)
				theSW = new ZStreamerW_Buffered(fWriteBufferSize, theSW);

			theSRW = new ZStreamerRW_Wrapper(theSR, theSW);
			}
		return theSRW;
		}

	return ZRef<ZStreamerRW>();
	}
