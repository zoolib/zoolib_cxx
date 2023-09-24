// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Starter_CFRunLoop_h__
#define __ZooLib_Apple_Starter_CFRunLoop_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/Starter_EventLoopBase.h"

// CFRunLoop is not available in CW's headers
#ifndef ZCONFIG_API_Avail__Starter_CFRunLoop
	#if ZCONFIG_SPI_Enabled(CoreFoundation) && !ZCONFIG(Compiler,CodeWarrior)
		#define ZCONFIG_API_Avail__Starter_CFRunLoop 1
	#endif
#endif

#ifndef ZCONFIG_API_Avail__Starter_CFRunLoop
	#define ZCONFIG_API_Avail__Starter_CFRunLoop 0
#endif

#ifndef ZCONFIG_API_Desired__Starter_CFRunLoop
	#define ZCONFIG_API_Desired__Starter_CFRunLoop 1
#endif

#if ZCONFIG_API_Enabled(Starter_CFRunLoop)

#include ZMACINCLUDE2(CoreFoundation,CFRunLoop.h)

namespace ZooLib {

// =================================================================================================
#pragma mark - Starter_CFRunLoop

class Starter_CFRunLoop
:	public Starter_EventLoopBase
	{
public:
	static ZP<Starter_CFRunLoop> sMain();

	Starter_CFRunLoop(CFRunLoopRef iRunLoopRef);
	virtual ~Starter_CFRunLoop();

// From Counted via Starter_EventLoopBase
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void AddMode(CFStringRef iMode);

protected:
// From Starter_EventLoopBase
	virtual bool pTrigger();

private:
	static void spCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void* info);

	ZP<CFRunLoopRef> fRunLoop;
	ZP<CFRunLoopObserverRef> fObserver;
	};

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(Starter_CFRunLoop)

#endif // __ZooLib_Apple_Starter_CFRunLoop_h__
