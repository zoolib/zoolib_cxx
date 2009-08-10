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

#include "zoolib/ZStreamerReader.h"

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZStreamerReader

ZStreamerReader::ZStreamerReader(ZRef<ZStreamerR> iStreamerR)
:	fStreamerR(iStreamerR)
	{}

void ZStreamerReader::RunnerAttached()
	{
	ZActor::RunnerAttached();
	this->ReadStarted();
	}

void ZStreamerReader::RunnerDetached()
	{
	this->ReadFinished();
	ZActor::RunnerDetached();
	}

bool ZStreamerReader::Act()
	{
	if (!this->Read(fStreamerR->GetStreamR()))
		return false;

	// ##
	// This is the point at which we'd want to register with a waker that
	// will call our Wake() when fStreamerR is readable. For now we'll just
	// unconditionally call Wake(), so this method will be called again
	// immediately, and we'll just block in Read().
	this->Wake();
	return true;
	}

void ZStreamerReader::ReadStarted()
	{}

void ZStreamerReader::ReadFinished()
	{}

NAMESPACE_ZOOLIB_END
