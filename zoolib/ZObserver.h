/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#ifndef __ZObserver__
#define __ZObserver__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/ZCallable.h"
#include "zoolib/ZQ.h"

#include <CoreFoundation/CFNotificationCenter.h>

#include <string>

#if __OBJC__
	@class NSString;
#endif

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZObserver

class ZObserver: public ZCounted
	{
public:
	typedef ZCallable<void(ZRef<ZObserver>, CFDictionaryRef)> Callable;

	ZObserver(void* iObject, const std::string& iName, ZRef<Callable> iCallable);
	ZObserver(void* iObject, CFStringRef iName, ZRef<Callable> iCallable);
	#if __OBJC__
		ZObserver(void* iObject, NSString* iName, ZRef<Callable> iCallable);
	#endif
	virtual ~ZObserver();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void* GetObject();
	std::string GetName();
	CFStringRef GetName_CFStringRef();
	#if __OBJC__
		NSString* GetName_NSString();
	#endif

private:
	static void spCallback(CFNotificationCenterRef center,
		void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo);

	void* fObject;
	ZQ<std::string> fName_String;
	ZQ<ZRef<CFStringRef> > fName_CFStringRef;
	ZRef<Callable> fCallable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
#endif // __ZObserver__
