/* -------------------------------------------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZProgressWatcher.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - ZProgressPusher

ZProgressPusher::ZProgressPusher(
	ZProgressWatcher* iProgressWatcher, const string& iDescription, int32 iMax)
	{
	fWatcher = iProgressWatcher;
	fPushes = 1;
	if (fWatcher)
		fWatcher->Push(iDescription, iMax);
	}

ZProgressPusher::ZProgressPusher(ZProgressWatcher* iProgressWatcher, int32 iMax)
	{
	fWatcher = iProgressWatcher;
	fPushes = 1;
	if (fWatcher)
		fWatcher->Push(string(), iMax);
	}

ZProgressPusher::ZProgressPusher(
	ZProgressPusher& iOtherPusher, const string& iDescription, int32 iMax)
	{
	fWatcher = iOtherPusher.fWatcher;
	fPushes = 1;
	if (fWatcher)
		fWatcher->Push(iDescription, iMax);
	}

ZProgressPusher::ZProgressPusher(ZProgressPusher& iOtherPusher, int32 iMax)
	{
	fWatcher = iOtherPusher.fWatcher;
	fPushes = 1;
	if (fWatcher)
		fWatcher->Push(string(), iMax);
	}	

ZProgressPusher::~ZProgressPusher()
	{
	if (fWatcher)
		{
		while (fPushes--)
			fWatcher->Pop();
		}
	}

void ZProgressPusher::Push(const string& iDescription, int32 iMax)
	{
	++fPushes;
	if (fWatcher)
		fWatcher->Push(iDescription, iMax);	
	}

void ZProgressPusher::Push(int32 iMax)
	{
	++fPushes;
	if (fWatcher)
		fWatcher->Push(string(), iMax);	
	}

void ZProgressPusher::Process(int32 iDelta)
	{
	if (fWatcher)
		fWatcher->Process(iDelta);
	}

void ZProgressPusher::Pop()
	{
	ZAssertStop(2, fPushes > 0);
	--fPushes;

	if (fWatcher)
		fWatcher->Pop();
	}

} // namespace ZooLib
