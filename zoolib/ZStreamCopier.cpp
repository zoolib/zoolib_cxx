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

#include "zoolib/ZStreamCopier.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static bool sCopy(
	const ZStreamR& iStreamR, void* iBuffer, size_t iBufferSize, const ZStreamW& iStreamW)
	{
	size_t countRead;
	iStreamR.Read(iBuffer, iBufferSize, &countRead);

	if (countRead == 0)
		{
		if (const ZStreamRCon* theSRC = dynamic_cast<const ZStreamRCon*>(&iStreamR))
			theSRC->ReceiveDisconnect(-1);

		if (const ZStreamWCon* theSWC = dynamic_cast<const ZStreamWCon*>(&iStreamW))
			theSWC->SendDisconnect();

		return false;
		}

	size_t countWritten;
	iStreamW.Write(iBuffer, countRead, &countWritten);

	return countWritten == countRead;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamCopier

ZStreamCopier::ZStreamCopier(ZRef<ZStreamerW> iStreamerW)
:	fStreamerW(iStreamerW),
	fChunkSize(sStackBufferSize)
	{}

ZStreamCopier::ZStreamCopier(ZRef<ZStreamerW> iStreamerW, size_t iChunkSize)
:	fStreamerW(iStreamerW),
	fChunkSize(iChunkSize)
	{}

ZStreamCopier::~ZStreamCopier()
	{}

bool ZStreamCopier::Read(const ZStreamR& iStreamR)
	{
	if (fChunkSize <= sStackBufferSize)
		{
		char buffer[sStackBufferSize];
		return sCopy(iStreamR, buffer, sStackBufferSize, fStreamerW->GetStreamW());
		}
	else
		{
		std::vector<char> buffer(fChunkSize);
		return sCopy(iStreamR, &buffer[0], fChunkSize, fStreamerW->GetStreamW());
		}
	}

void ZStreamCopier::RunnerDetached(ZStreamReaderRunner* iRunner)
	{ delete this; }

NAMESPACE_ZOOLIB_END