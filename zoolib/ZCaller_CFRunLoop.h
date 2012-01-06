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

#ifndef __ZCaller_CFRunLoop_h__
#define __ZCaller_CFRunLoop_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZCaller_EventLoop.h"

// CFRunLoop is not available in CW's headers
#ifndef ZCONFIG_API_Avail__Caller_CFRunLoop
#	if ZCONFIG_SPI_Enabled(CoreFoundation) && !ZCONFIG(Compiler,CodeWarrior)
#		define ZCONFIG_API_Avail__Caller_CFRunLoop 1
#	endif
#endif

#ifndef ZCONFIG_API_Avail__Caller_CFRunLoop
#	define ZCONFIG_API_Avail__Caller_CFRunLoop 0
#endif

#ifndef ZCONFIG_API_Desired__Caller_CFRunLoop
#	define ZCONFIG_API_Desired__Caller_CFRunLoop 1
#endif

#if ZCONFIG_API_Enabled(Caller_CFRunLoop)

#include ZMACINCLUDE2(CoreFoundation,CFRunLoop.h)

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCaller_CFRunLoop

class ZCaller_CFRunLoop
:	public ZCaller_EventLoop
	{
public:
	static ZRef<ZCaller_CFRunLoop> sMain();

	ZCaller_CFRunLoop(CFRunLoopRef iRunLoopRef);
	virtual ~ZCaller_CFRunLoop();

// From ZCounted via ZCaller
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void AddMode(CFStringRef iMode);

protected:
// From ZCaller_EventLoop
	virtual bool pTrigger();

private:
	static void spCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void* info);

	ZRef<CFRunLoopRef> fRunLoop;
	ZRef<CFRunLoopObserverRef> fObserver;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Caller_CFRunLoop)

#endif // __ZCaller_CFRunLoop_h__
