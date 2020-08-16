// Copyright (c) 2008 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Util_NS.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Unicode.h"
#include "zoolib/Val_ZZ.h"

#import <Foundation/NSString.h>

using namespace ZooLib;

// =================================================================================================
#pragma mark - Util_NS

namespace ZooLib {
namespace Util_NS {

NSString* sString()
	{ return [NSString string]; }

NSString* sString(const string8& iString8)
	{ return [NSString stringWithUTF8String:iString8.c_str()]; }

NSString* sString(const string16& iString16)
	{
	return [NSString
		stringWithCharacters:(const unichar*)iString16.c_str()
		length:iString16.length()];
	}

NSMutableString* sStringMutable()
	{ return [NSMutableString string]; }

NSMutableString* sStringMutable(const string8& iString8)
	{ return [NSMutableString stringWithUTF8String:iString8.c_str()]; }

NSMutableString* sStringMutable(const string16& iString16)
	{
	return [NSMutableString
		stringWithCharacters:(const unichar*)iString16.c_str()
		length:iString16.length()];
	}

NSMutableString* sStringMutable(NSString* iNSString)
	{ return [NSMutableString stringWithString:iNSString]; }

// -----------------

NSDictionary* sDictionary()
	{ return [NSDictionary dictionary]; }

NSMutableDictionary* sDictionaryMutable()
	{ return [NSMutableDictionary dictionary]; }

NSMutableDictionary* sDictionaryMutable(NSDictionary* iNSDictionary)
	{ return [NSMutableDictionary dictionaryWithDictionary:iNSDictionary]; }

// -----------------

NSArray* sArray()
	{ return [NSArray array]; }

NSMutableArray* sArrayMutable()
	{ return [NSMutableArray array]; }

NSMutableArray* sArrayMutable(NSArray* iNSArray)
	{ return [NSMutableArray arrayWithArray:iNSArray]; }

// -----------------

NSData* sData()
	{ return [NSData data]; }

NSData* sData(const void* iSource, size_t iSize)
	{ return [NSData dataWithBytes:iSource length:iSize]; }

NSMutableData* sDataMutable()
	{ return [NSMutableData data]; }

NSMutableData* sDataMutable(size_t iSize)
	{
	NSMutableData* result = [NSMutableData data];
	[result setLength:iSize];
	return result;
	}

NSMutableData* sDataMutable(const void* iSource, size_t iSize)
	{ return [NSMutableData dataWithBytes:iSource length:iSize]; }

NSMutableData* sDataMutable(NSData* iNSData)
	{ return [NSMutableData dataWithData:iNSData]; }

// =================================================================================================
#pragma mark - Util_NS, conversions

string8 sAsUTF8(NSString* iNSString)
	{
	if (iNSString)
		return [iNSString UTF8String];
	return string8();
	}

string16 sAsUTF16(NSString* iNSString)
	{
	if (iNSString)
		return Unicode::sAsUTF16([iNSString UTF8String]);
	return string16();
	}

} // namespace Util_NS
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)
