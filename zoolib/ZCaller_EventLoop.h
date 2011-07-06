/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZCaller_EventLoop__
#define __ZCaller_EventLoop__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZThread.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCaller_EventLoop

class ZCaller_EventLoop
:	public ZCaller
	{
public:
	ZCaller_EventLoop();
	virtual ~ZCaller_EventLoop();

// From ZCaller
	virtual void Call(ZRef<ZCallable_Void> iCallable);

protected:
// Called by concrete subclass
	void pCall();

// Implemented by concrete subclass
	virtual void pTrigger() = 0;

private:
	ZMtx fMtx;
	bool fTriggered;
	std::vector<ZRef<ZCallable_Void> > fCallables_Pending;
	std::vector<ZRef<ZCallable_Void> > fCallables_Calling;
	};

} // namespace ZooLib

#endif // __ZCaller_EventLoop__
