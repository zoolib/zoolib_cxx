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

#ifndef __ZCaller_EventLoop_h__
#define __ZCaller_EventLoop_h__ 1
#include "zconfig.h"

#include "zoolib/ZCaller.h"
#include "zoolib/ZThread.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
// MARK: - ZCaller_EventLoop

class ZCaller_EventLoop
:	public ZCaller
	{
public:
	ZCaller_EventLoop();
	virtual ~ZCaller_EventLoop();

// From ZCaller
	virtual bool Enqueue(const ZRef<ZCallable_Void>& iCallable);

protected:
// Called by concrete subclass
	void pInvokeClearQueue();

	void pDiscardPending();

// Implemented by concrete subclass
	virtual bool pTrigger() = 0;

private:
	ZMtx fMtx;
	bool fTriggered;
	std::vector<ZRef<ZCallable_Void> > fCallables;
	};

// =================================================================================================
// MARK: - ZCaller_EventLoop_CallableTrigger

class ZCaller_EventLoop_CallableTrigger
:	public ZCaller_EventLoop
	{
public:
	typedef ZCallable<bool()> Callable_Trigger;

	ZCaller_EventLoop_CallableTrigger(const ZRef<Callable_Trigger>& iCallable_Trigger);

// From ZCaller_EventLoop
	virtual bool pTrigger();

// Our protocol
	void InvokeClearQueue();

private:
	ZRef<Callable_Trigger> fCallable_Trigger;
	};

} // namespace ZooLib

#endif // __ZCaller_EventLoop_h__
