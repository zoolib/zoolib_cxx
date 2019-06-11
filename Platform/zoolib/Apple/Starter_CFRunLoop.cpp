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

#include "zoolib/Apple/Starter_CFRunLoop.h"

#if ZCONFIG_API_Enabled(Starter_CFRunLoop)

#include "zoolib/Apple/Util_CF_Context.h"

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_CFRunLoop

// Usable pre 10.5
#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
	static CFRunLoopRef spRunLoopMain = ::CFRunLoopGetCurrent();
#endif

ZRef<Starter_CFRunLoop> Starter_CFRunLoop::sMain()
	{
	static ZRef<Starter_CFRunLoop> spStarter;
	if (not spStarter)
		{
		#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
			ZRef<Starter_CFRunLoop> theStarter = new Starter_CFRunLoop(spRunLoopMain);
		#else
			ZRef<Starter_CFRunLoop> theStarter = new Starter_CFRunLoop(::CFRunLoopGetMain());
		#endif
		spStarter.AtomicCAS(nullptr, theStarter.Get());
		}
	return spStarter;
	}

Starter_CFRunLoop::Starter_CFRunLoop(CFRunLoopRef iRunLoop)
:	fRunLoop(iRunLoop)
	{}

Starter_CFRunLoop::~Starter_CFRunLoop()
	{}

void Starter_CFRunLoop::Initialize()
	{
	Starter::Initialize();

	fObserver = sAdopt& ::CFRunLoopObserverCreate(
		nullptr, // allocator
		kCFRunLoopAllActivities,// kCFRunLoopBeforeTimers, // activities
		true, // repeats
		0, // order
		spCallback,
		Util_CF::Context<CFRunLoopObserverContext>(this->GetWeakRefProxy()));

	::CFRunLoopAddObserver(fRunLoop, fObserver, kCFRunLoopCommonModes);
	}

void Starter_CFRunLoop::Finalize()
	{
	::CFRunLoopObserverInvalidate(fObserver);

	Starter::Finalize();
	}

void Starter_CFRunLoop::AddMode(CFStringRef iMode)
	{
	::CFRunLoopAddObserver(fRunLoop, fObserver, iMode);
	}

bool Starter_CFRunLoop::pTrigger()
	{
	::CFRunLoopWakeUp(fRunLoop);
	return true;
	}

void Starter_CFRunLoop::spCallback(
	CFRunLoopObserverRef observer, CFRunLoopActivity activity, void* info)
	{
	if (ZRef<Starter_CFRunLoop> theStarter =
		WP<Starter_CFRunLoop>(static_cast<WeakRefProxy*>(info)))
		{ theStarter->pInvokeClearQueue(); }
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Starter_CFRunLoop)
