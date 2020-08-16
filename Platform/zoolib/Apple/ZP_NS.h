// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_ZP_NS_h__
#define __ZooLib_Apple_ZP_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Apple/Compat_NSObject.h"

#include "zoolib/ZP.h"

// =================================================================================================
#pragma mark - id and NSObject, sRetain and sRelease

void sRetain(struct objc_object& iOb);
void sRetain(NSObject& iNSObject);

void sRelease(struct objc_object& iOb);
void sRelease(NSObject& iNSObject);

void sCheck(struct objc_object*);
void sCheck(NSObject*);

int sRetainCount(struct objc_object*);
int sRetainCount(NSObject*);

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZooLib_Apple_ZP_NS_h__
