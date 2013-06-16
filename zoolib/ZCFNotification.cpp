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

#include "zoolib/ZCFNotification.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/ZUtil_CF.h"

namespace ZooLib {

// CFNotificationCenter does not have as robust a protocol as CFRunLoop, so there is the
// potential for races in this code. If unregistration occurs on a thread other than that
// on which notifications are delivered, you may wish to rethink things.

// =================================================================================================
// MARK: - ZCFNotification

ZCFNotification::ZCFNotification(
	void* iObject, const std::string& iName, ZRef<Callable> iCallable)
:	fObject(iObject)
,	fName_String(iName)
,	fCallable(iCallable)
	{}

ZCFNotification::ZCFNotification(
	void* iObject, CFStringRef iName, ZRef<Callable> iCallable)
:	fObject(iObject)
,	fName_CFStringRef(iName)
,	fCallable(iCallable)
	{}

ZCFNotification::~ZCFNotification()
	{}

void ZCFNotification::Initialize()
	{
	ZCounted::Initialize();

	::CFNotificationCenterAddObserver(
		::CFNotificationCenterGetLocalCenter(),
		this,
		spCallback,
		this->GetName_CFStringRef(),
		fObject,
		CFNotificationSuspensionBehaviorDeliverImmediately);
	}

void ZCFNotification::Finalize()
	{
	::CFNotificationCenterRemoveEveryObserver(::CFNotificationCenterGetLocalCenter(), this);

	ZCounted::Finalize();
	}

void* ZCFNotification::GetObject()
	{ return fObject; }

std::string ZCFNotification::GetName()
	{
	if (not fName_String)
		fName_String = ZUtil_CF::sAsUTF8(fName_CFStringRef);
	return *fName_String;
	}

CFStringRef ZCFNotification::GetName_CFStringRef()
	{
	if (not fName_CFStringRef)
		fName_CFStringRef = ZUtil_CF::sString(*fName_String);
	return fName_CFStringRef;
	}

void ZCFNotification::spCallback(CFNotificationCenterRef center,
	void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo)
	{
	ZRef<ZCFNotification> theObserver = static_cast<ZCFNotification*>(observer);
	sCall(theObserver->fCallable, theObserver, userInfo);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
