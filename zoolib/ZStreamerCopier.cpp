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

#include "zoolib/ZStreamerCopier.h"

#include <vector>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerCopier

static bool sCopy(
	const ZStreamRCon& iStreamRCon,
	void* iBuffer, size_t iBufferSize,
	const ZStreamWCon& iStreamWCon)
	{
	size_t countRead;
	iStreamRCon.Read(iBuffer, iBufferSize, &countRead);

	if (countRead == 0)
		{
		iStreamRCon.ReceiveDisconnect(-1);
		iStreamWCon.SendDisconnect();

		return false;
		}

	size_t countWritten;
	iStreamWCon.Write(iBuffer, countRead, &countWritten);
	if (countWritten != countRead)
		{
		iStreamRCon.Abort();
		iStreamWCon.Abort();
		return false;
		}
	return true;
	}

ZStreamerCopier::ZStreamerCopier(ZRef<ZTaskOwner> iTaskOwner,
	ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon)
:	ZTask(iTaskOwner),
	fStreamerRCon(iStreamerRCon),
	fStreamerWCon(iStreamerWCon),
	fChunkSize(sStackBufferSize)
	{}

ZStreamerCopier::ZStreamerCopier(ZRef<ZTaskOwner> iTaskOwner,
	ZRef<ZStreamerRCon> iStreamerRCon, ZRef<ZStreamerWCon> iStreamerWCon, size_t iChunkSize)
:	ZTask(iTaskOwner),
	fStreamerRCon(iStreamerRCon),
	fStreamerWCon(iStreamerWCon),
	fChunkSize(iChunkSize)
	{}

ZStreamerCopier::~ZStreamerCopier()
	{}

void ZStreamerCopier::RunnerDetached()
	{
	ZTask::pFinished();
	ZWorker::RunnerDetached();
	}

bool ZStreamerCopier::Work()
	{
	ZWorker::Wake();//##
	if (fChunkSize <= sStackBufferSize)
		{
		char buffer[sStackBufferSize];
		return sCopy(fStreamerRCon->GetStreamRCon(),
			buffer, sStackBufferSize, fStreamerWCon->GetStreamWCon());
		}
	else
		{
		std::vector<char> buffer(fChunkSize);
		return sCopy(fStreamerRCon->GetStreamRCon(),
			&buffer[0], fChunkSize, fStreamerWCon->GetStreamWCon());
		}	
	}

void ZStreamerCopier::Kill()
	{
	fStreamerRCon->GetStreamRCon().Abort();
	fStreamerWCon->GetStreamWCon().Abort();
	}

NAMESPACE_ZOOLIB_END