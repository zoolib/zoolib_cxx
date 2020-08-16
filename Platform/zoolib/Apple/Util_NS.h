// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#ifndef __ZooLib_Apple_Util_NS_h__
#define __ZooLib_Apple_Util_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

// Urg. We need to include this before any std C++ headers when building
// with Cocotron, otherwise va_list isn't appropriately visible to ObjC code.
#include <Foundation/Foundation.h>

#include "zoolib/UnicodeString.h"

// =================================================================================================
#pragma mark - Util_NS

namespace ZooLib {
namespace Util_NS {

NSString* sString();
NSString* sString(const string8& iString8);
NSString* sString(const string16& iString16);

NSMutableString* sStringMutable();
NSMutableString* sStringMutable(const string8& iString8);
NSMutableString* sStringMutable(const string16& iString16);
NSMutableString* sStringMutable(NSString* iNSString);

// -----------------

NSDictionary* sDictionary();

NSMutableDictionary* sDictionaryMutable();
NSMutableDictionary* sDictionaryMutable(NSDictionary* iNSDictionary);

// -----------------

NSArray* sArray();

NSMutableArray* sArrayMutable();
NSMutableArray* sArrayMutable(NSArray* iNSArray);

// -----------------

NSData* sData();
NSData* sData(const void* iSource, size_t iSize);

NSMutableData* sDataMutable();
NSMutableData* sDataMutable(size_t iSize);
NSMutableData* sDataMutable(const void* iSource, size_t iSize);
NSMutableData* sDataMutable(NSData* iNSData);

// -----------------

string8 sAsUTF8(NSString* iNSString);
string16 sAsUTF16(NSString* iNSString);

} // namespace Util_NS
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZooLib_Apple_Util_NS_h__
