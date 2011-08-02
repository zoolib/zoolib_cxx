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

ZQ<void> ZCaller_EventLoop::QCall(ZRef<ZCallable_Void> iCallable)
	{
	if (iCallable)
		{
		ZAcqMtx acq(fMtx);
		if (fTriggered++ || this->pTrigger())
			{
			fCallables.push_back(iCallable);
			return notnull;
			}
		}
	return null;
	}

void ZCaller_EventLoop::pCall()
	{
	vector<ZRef<ZCallable_Void> > calling;

	{
	ZAcqMtx acq(fMtx);
	fTriggered = false;
	fCallables.swap(calling);
	}

	for (vector<ZRef<ZCallable_Void> >::iterator iter = calling.begin();
		iter != calling.end(); ++iter)
		{ (*iter)->Call(); }
	}

void ZCaller_EventLoop::pDiscardPending()
	{
	ZAcqMtx acq(fMtx);
	fCallables.clear();
	}

} // namespace ZooLib
