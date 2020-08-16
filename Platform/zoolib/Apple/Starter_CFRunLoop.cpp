// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

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

ZP<Starter_CFRunLoop> Starter_CFRunLoop::sMain()
	{
	static ZP<Starter_CFRunLoop> spStarter;
	if (not spStarter)
		{
		#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
			ZP<Starter_CFRunLoop> theStarter = new Starter_CFRunLoop(spRunLoopMain);
		#else
			ZP<Starter_CFRunLoop> theStarter = new Starter_CFRunLoop(::CFRunLoopGetMain());
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
		Util_CF::Context<CFRunLoopObserverContext>(this->GetWPProxy()));

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
	if (ZP<Starter_CFRunLoop> theStarter =
		WP<Starter_CFRunLoop>(static_cast<WPProxy*>(info)))
		{ theStarter->pInvokeClearQueue(); }
	}

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Starter_CFRunLoop)
