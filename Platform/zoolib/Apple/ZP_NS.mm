// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/ZP_NS.h"

#include "zoolib/ZDebug.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#import <Foundation/NSObject.h>

// =================================================================================================
#pragma mark - id and NSObject, sRetain and sRelease

void sRetain(struct objc_object& iOb)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRetain(&iOb);
	#else
		[((id)&iOb) retain];
	#endif
	}

void sRetain(NSObject& iNSObject)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRetain((__bridge CFTypeRef)&iNSObject);
	#else
		[&iNSObject retain];
	#endif
	}

void sRelease(struct objc_object& iOb)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRelease(&iOb);
	#else
		[((id)&iOb) release];
	#endif
	}

void sRelease(NSObject& iNSObject)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		CFRelease((__bridge CFTypeRef)&iNSObject);
	#else
		[&iNSObject release];
	#endif
	}

void sCheck(struct objc_object* iP)
	{ ZAssertStop(1, iP); }

void sCheck(NSObject* iP)
	{ ZAssertStop(1, iP); }

int sRetainCount(struct objc_object* iP)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		return (int)CFGetRetainCount(iP);
	#else
		return (int)[((id)iP) retainCount];
	#endif
	}

int sRetainCount(NSObject* iP)
	{
	#if ZCONFIG(Compiler,Clang) && __has_feature(objc_arc)
		return (int)CFGetRetainCount((__bridge CFTypeRef)iP);
	#else
		return (int)[iP retainCount];
	#endif
	}

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)
