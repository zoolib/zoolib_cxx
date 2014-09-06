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

#include "zoolib/Caller_CFRunLoop.h"

#if ZCONFIG_API_Enabled(Caller_CFRunLoop)

#include "zoolib/ZUtil_CF_Context.h"

namespace ZooLib {

// =================================================================================================
// MARK: - Caller_CFRunLoop

// Usable pre 10.5
#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
	static CFRunLoopRef spRunLoopMain = ::CFRunLoopGetCurrent();
#endif

ZRef<Caller_CFRunLoop> Caller_CFRunLoop::sMain()
	{
	static ZRef<Caller_CFRunLoop> spCaller;
	if (not spCaller)
		{
		#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
			ZRef<Caller_CFRunLoop> theCaller = new Caller_CFRunLoop(spRunLoopMain);
		#else
			ZRef<Caller_CFRunLoop> theCaller = new Caller_CFRunLoop(::CFRunLoopGetMain());
		#endif
		spCaller.AtomicCAS(nullptr, theCaller.Get());
		}
	return spCaller;
	}

Caller_CFRunLoop::Caller_CFRunLoop(CFRunLoopRef iRunLoop)
:	fRunLoop(iRunLoop)
	{}

Caller_CFRunLoop::~Caller_CFRunLoop()
	{}

void Caller_CFRunLoop::Initialize()
	{
	Caller::Initialize();

	fObserver = sAdopt& ::CFRunLoopObserverCreate(
		nullptr, // allocator
		kCFRunLoopAllActivities,// kCFRunLoopBeforeTimers, // activities
		true, // repeats
		0, // order
		spCallback,
		ZUtil_CF::Context<CFRunLoopObserverContext>(this->GetWeakRefProxy()));

	::CFRunLoopAddObserver(fRunLoop, fObserver, kCFRunLoopCommonModes);
	}

void Caller_CFRunLoop::Finalize()
	{
	::CFRunLoopObserverInvalidate(fObserver);

	Caller::Finalize();
	}

void Caller_CFRunLoop::AddMode(CFStringRef iMode)
	{
	::CFRunLoopAddObserver(fRunLoop, fObserver, iMode);
	}

bool Caller_CFRunLoop::pTrigger()
	{
	::CFRunLoopWakeUp(fRunLoop);
	return true;
	}

void Caller_CFRunLoop::spCallback(
	CFRunLoopObserverRef observer, CFRunLoopActivity activity, void* info)
	{
	if (ZRef<Caller_CFRunLoop> theCaller =
		ZWeakRef<Caller_CFRunLoop>(static_cast<WeakRefProxy*>(info)))
		{ theCaller->pInvokeClearQueue(); }
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Caller_CFRunLoop)
