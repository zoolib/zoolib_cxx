/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZUtil_NS.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/ZTime.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZVal_Any.h"

#import <Foundation/NSDate.h>
#import <Foundation/NSString.h>

using std::vector;

using namespace ZooLib;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_NS

namespace ZooLib {
namespace ZUtil_NS {

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

// -----

NSDictionary* sDictionary()
	{ return [NSDictionary dictionary]; }

NSMutableDictionary* sDictionaryMutable()
	{ return [NSMutableDictionary dictionary]; }

NSMutableDictionary* sDictionaryMutable(NSDictionary* iNSDictionary)
	{ return [NSMutableDictionary dictionaryWithDictionary:iNSDictionary]; }

// -----

NSArray* sArray()
	{ return [NSArray array]; }

NSMutableArray* sArrayMutable()
	{ return [NSMutableArray array]; }

NSMutableArray* sArrayMutable(NSArray* iNSArray)
	{ return [NSMutableArray arrayWithArray:iNSArray]; }

// -----

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
#pragma mark -
#pragma mark * ZUtil_NS, conversions

string8 sAsUTF8(NSString* iNSString)
	{
	if (iNSString)
		return [iNSString UTF8String];
	return string8();
	}

string16 sAsUTF16(NSString* iNSString)
	{
	if (iNSString)
		return ZUnicode::sAsUTF16([iNSString UTF8String]);
	return string16();
	}

ZAny sDAsAny(const ZAny& iDefault, NSObject* iVal)
	{
	if (iVal)
		{
		@try
			{
			return [iVal asAnyWithDefault:iDefault];
			}
		@catch (id ex)
			{}
		}
	return iDefault;
	}

ZAny sAsAny(NSObject* iVal)
	{ return sDAsAny(ZAny(), iVal); }

NSObject* sDAsNSObject(NSObject* iDefault, const ZAny& iVal)
	{
	if (false)
		{}
	else if (!iVal)
		{
		return [NSNull null];
		}
	else if (const string8* theValue = iVal.PGet<string8>())
		{
		return sString(*theValue);
		}
	else if (const vector<char>* theValue = iVal.PGet<vector<char> >())
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const ZData_Any* theValue = iVal.PGet<ZData_Any>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetData(), theSize);
		else
			return sData();
		}
	else if (const ZSeq_Any* theValue = iVal.PGet<ZSeq_Any>())
		{
		NSMutableArray* theArray = sArrayMutable();
		for (size_t x = 0, count = theValue->Count(); x < count; ++x)
			[theArray addObject:sDAsNSObject(iDefault, theValue->Get(x))];
		return theArray;
		}
	else if (const ZMap_Any* theValue = iVal.PGet<ZMap_Any>())
		{
		NSMutableDictionary* theDictionary = sDictionaryMutable();
		for (ZMap_Any::Index_t i = theValue->Begin(), end = theValue->End(); i != end; ++i)
			{
			[theDictionary
				setObject:sDAsNSObject(iDefault, theValue->Get(i))
				forKey:sString(theValue->NameOf(i))];
			}
		return theDictionary;
		}
	else if (const bool* theValue = iVal.PGet<bool>())
		{
		return [NSNumber numberWithBool:(BOOL)*theValue];
		}
	else if (const ZTime* theValue = iVal.PGet<ZTime>())
		{
		return [NSDate dateWithTimeIntervalSince1970:theValue->fVal];
		}
	else if (const char* theValue = iVal.PGet<char>())
		{
		return [NSNumber numberWithChar:*theValue];
		}
	else if (const unsigned char* theValue = iVal.PGet<unsigned char>())
		{
		return [NSNumber numberWithUnsignedChar:*theValue];
		}
	else if (const signed char* theValue = iVal.PGet<signed char>())
		{
		return [NSNumber numberWithChar:*theValue];
		}
	else if (const short* theValue = iVal.PGet<short>())
		{
		return [NSNumber numberWithShort:*theValue];
		}
	else if (const unsigned short* theValue = iVal.PGet<unsigned short>())
		{
		return [NSNumber numberWithUnsignedShort:*theValue];
		}
	else if (const int* theValue = iVal.PGet<int>())
		{
		return [NSNumber numberWithInt:*theValue];
		}
	else if (const unsigned int* theValue = iVal.PGet<unsigned int>())
		{
		return [NSNumber numberWithUnsignedInt:*theValue];
		}
	else if (const long* theValue = iVal.PGet<long>())
		{
		return [NSNumber numberWithLong:*theValue];
		}
	else if (const unsigned long* theValue = iVal.PGet<unsigned long>())
		{
		return [NSNumber numberWithUnsignedLong:*theValue];
		}
	else if (const int64* theValue = iVal.PGet<int64>())
		{
		return [NSNumber numberWithLongLong:(long long)*theValue];
		}
	else if (const uint64* theValue = iVal.PGet<uint64>())
		{
		return [NSNumber numberWithUnsignedLongLong:(unsigned long long)*theValue];
		}
	else if (const float* theValue = iVal.PGet<float>())
		{
		return [NSNumber numberWithFloat:*theValue];
		}
	else if (const double* theValue = iVal.PGet<double>())
		{
		return [NSNumber numberWithDouble:*theValue];
		}

	return iDefault;
	}

NSObject* sAsNSObject(const ZAny& iVal)
	{ return sDAsNSObject([NSNull null], iVal); }

} // namespace ZUtil_NS
} // namespace ZooLib

// =================================================================================================
@implementation NSObject (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{
	// Hmm, log and return null or what?
	ZDebugLogf(0, ("NSObject (ZAny_Additions) asAnyWithDefault called"));
	return iDefault;
	}

@end

// =================================================================================================
@interface NSNull (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSNull (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{ return ZAny(); }

@end

// =================================================================================================
@interface NSDictionary (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSDictionary (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{
	ZMap_Any result;
	for (id theKey, i = [self keyEnumerator]; (theKey = [i nextObject]); /*no inc*/)
		{
		const string8 theName = ZUtil_NS::sAsUTF8((NSString*)theKey);
		const ZAny theVal = [[self objectForKey:theKey] asAnyWithDefault:iDefault];
		result.Set(theName, theVal);
		}
	return ZAny(result);
	}

@end

// =================================================================================================
@interface NSArray (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSArray (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{
	ZSeq_Any result;
	for (id theValue, i = [self objectEnumerator]; (theValue = [i nextObject]); /*no inc*/)
		result.Append([theValue asAnyWithDefault:iDefault]);
	return ZAny(result);
	}

@end

// =================================================================================================
@interface NSData (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSData (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{ return ZAny(ZData_Any([self bytes], [self length])); }

@end

// =================================================================================================
@interface NSString (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSString (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{ return ZAny(string8([self UTF8String])); }

@end

// =================================================================================================
@interface NSNumber (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSNumber (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{ return ZAny(int64([self longLongValue])); }

@end

// =================================================================================================
@interface NSDate (ZAny_Additions)
-(ZAny)asAnyWithDefault:(const ZAny&)iDefault;
@end

@implementation NSDate (ZAny_Additions)

-(ZAny)asAnyWithDefault:(const ZAny&)iDefault
	{ return ZAny(ZTime([self timeIntervalSince1970])); }

@end

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)
