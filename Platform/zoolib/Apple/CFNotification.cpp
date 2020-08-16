// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/CFNotification.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/Apple/Util_CF.h"

namespace ZooLib {

// CFNotificationCenter does not have as robust a protocol as CFRunLoop, so there is the
// potential for races in this code. If unregistration occurs on a thread other than that
// on which notifications are delivered, you may wish to rethink things.

// =================================================================================================
#pragma mark - CFNotification

CFNotification::CFNotification(
	void* iObject, const std::string& iName, ZP<Callable_t> iCallable)
:	fObject(iObject)
,	fName_String(iName)
,	fCallable(iCallable)
	{}

CFNotification::CFNotification(
	void* iObject, CFStringRef iName, ZP<Callable_t> iCallable)
:	fObject(iObject)
,	fName_CFStringRef(iName)
,	fCallable(iCallable)
	{}

CFNotification::~CFNotification()
	{}

void CFNotification::Initialize()
	{
	Counted::Initialize();

	::CFNotificationCenterAddObserver(
		::CFNotificationCenterGetLocalCenter(),
		this,
		spCallback,
		this->GetName_CFStringRef(),
		fObject,
		CFNotificationSuspensionBehaviorDeliverImmediately);
	}

void CFNotification::Finalize()
	{
	::CFNotificationCenterRemoveEveryObserver(::CFNotificationCenterGetLocalCenter(), this);

	Counted::Finalize();
	}

void* CFNotification::GetObject()
	{ return fObject; }

std::string CFNotification::GetName()
	{
	if (not fName_String)
		fName_String = Util_CF::sAsUTF8(fName_CFStringRef);
	return *fName_String;
	}

CFStringRef CFNotification::GetName_CFStringRef()
	{
	if (not fName_CFStringRef)
		fName_CFStringRef = Util_CF::sString(*fName_String);
	return fName_CFStringRef;
	}

void CFNotification::spCallback(CFNotificationCenterRef center,
	void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo)
	{
	ZP<CFNotification> theObserver = static_cast<CFNotification*>(observer);
	sCall(theObserver->fCallable, theObserver, userInfo);
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
