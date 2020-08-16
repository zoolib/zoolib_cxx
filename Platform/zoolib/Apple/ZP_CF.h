// Copyright (c) 2009 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_ZP_CF_h__
#define __ZooLib_Apple_ZP_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#include "zoolib/ZP.h"

#if ZCONFIG_SPI_Enabled(CFType)

typedef unsigned long CFTypeID;

typedef const void * CFTypeRef;

typedef const struct __CFNull * CFNullRef;

typedef const struct __CFArray * CFArrayRef;
typedef struct __CFArray * CFMutableArrayRef;

typedef const struct __CFData * CFDataRef;
typedef struct __CFData * CFMutableDataRef;

typedef const struct __CFDictionary * CFDictionaryRef;
typedef struct __CFDictionary * CFMutableDictionaryRef;

typedef const struct __CFString * CFStringRef;
typedef struct __CFString * CFMutableStringRef;

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_ZP_CF_h__
