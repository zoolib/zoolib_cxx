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

#ifndef __ZCFNotification__
#define __ZCFNotification__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/ZCallable.h"

#include <CoreFoundation/CFNotificationCenter.h>

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZCFNotification

class ZCFNotification: public ZCounted
	{
public:
	typedef ZCallable<void(const void*, const std::string&, CFDictionaryRef)> Callable;

	ZCFNotification(void* iObject, const std::string& iName, ZRef<Callable> iCallable);
	virtual ~ZCFNotification();

// From ZCounted
	virtual void Initialize();
	virtual void Finalize();

private:
	static void spCallback(CFNotificationCenterRef center,
		void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo);

	void* fObject;
	std::string fName;
	ZRef<Callable> fCallable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
#endif // __ZCFNotification__
