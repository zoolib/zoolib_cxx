/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZCaller_EventLoop.h"

namespace ZooLib {

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZCaller_EventLoop

ZCaller_EventLoop::ZCaller_EventLoop()
:	fTriggered(false)
	{}

ZCaller_EventLoop::~ZCaller_EventLoop()
	{}

void ZCaller_EventLoop::Call(ZRef<ZCallable_Void> iCallable)
	{
	if (iCallable)
		{
		ZAcqMtx acq(fMtx);
		fCallables_Pending.push_back(iCallable);
		if (not fTriggered++)
			this->pTrigger();
		}
	}

void ZCaller_EventLoop::pCall()
	{
	ZGuardRMtx guard(fMtx);
	fTriggered = false;
	fCallables_Pending.swap(fCallables_Calling);
	guard.Release();

	for (vector<ZRef<ZCallable_Void> >::iterator iter = fCallables_Calling.begin();
		iter != fCallables_Calling.end(); ++iter)
		{ (*iter)->Call(); }

	fCallables_Calling.resize(0);
	}

} // namespace ZooLib
