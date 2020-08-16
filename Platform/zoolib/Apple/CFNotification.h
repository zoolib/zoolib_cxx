// Copyright (c) 2010 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_CFNotification_h__
#define __ZooLib_Apple_CFNotification_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CoreFoundation)

#include "zoolib/Callable.h"

#include <CoreFoundation/CFNotificationCenter.h>

#include <string>

namespace ZooLib {

// =================================================================================================
#pragma mark - CFNotification

class CFNotification
:	public Counted
	{
public:
	typedef Callable<void(ZP<CFNotification>, CFDictionaryRef)> Callable_t;

	CFNotification(void* iObject, const std::string& iName, ZP<Callable_t> iCallable);
	CFNotification(void* iObject, CFStringRef iName, ZP<Callable_t> iCallable);

	virtual ~CFNotification();

// From Counted
	virtual void Initialize();
	virtual void Finalize();

// Our protocol
	void* GetObject();

	std::string GetName();
	CFStringRef GetName_CFStringRef();

private:
	static void spCallback(CFNotificationCenterRef center,
		void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo);

	void* fObject;
	ZQ<std::string> fName_String;
	ZP<CFStringRef> fName_CFStringRef;
	ZP<Callable_t> fCallable;
	};

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CoreFoundation)
#endif // __ZooLib_Apple_CFNotification_h__
