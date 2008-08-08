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

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static bool sRead(
	const ZStreamR& iStreamR, void* iBuffer, size_t iBufferSize, const ZStreamWCon& iStreamWCon)
	{
	size_t countRead;
	iStreamR.Read(iBuffer, iBufferSize, &countRead);

	if (countRead == 0)
		{
		if (const ZStreamRCon* theSRC = dynamic_cast<const ZStreamRCon*>(&iStreamR))
			theSRC->ReceiveDisconnect(-1);

		iStreamWCon.SendDisconnect();

		return false;
		}

	size_t countWritten;
	iStreamWCon.Write(iBuffer, countRead, &countWritten);

	return countWritten == countRead;	
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamCopier

ZStreamCopier::ZStreamCopier(ZRef<ZStreamerWCon> iStreamerWCon)
:	fStreamerWCon(iStreamerWCon),
	fChunkSize(ZooLib::sStackBufferSize)
	{}

ZStreamCopier::ZStreamCopier(ZRef<ZStreamerWCon> iStreamerWCon, size_t iChunkSize)
:	fStreamerWCon(iStreamerWCon),
	fChunkSize(iChunkSize)
	{}

ZStreamCopier::~ZStreamCopier()
	{}

bool ZStreamCopier::Read(const ZStreamR& iStreamR)
	{
	if (fChunkSize <= ZooLib::sStackBufferSize)
		{
		char buffer[ZooLib::sStackBufferSize];
		return sRead(iStreamR, buffer, ZooLib::sStackBufferSize, fStreamerWCon->GetStreamWCon());
		}
	else
		{
		std::vector<char> buffer(fChunkSize);
		return sRead(iStreamR, &buffer[0], fChunkSize, fStreamerWCon->GetStreamWCon());
		}
	}

void ZStreamCopier::RunnerDetached(ZStreamReaderRunner* iRunner)
	{ delete this; }
