// Copyright (c) 2007 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Util_NS_Any.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Apple/Util_NS.h"

#include "zoolib/Unicode.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Val_ZZ.h"

#import <Foundation/NSDate.h>
#import <Foundation/NSString.h>

using std::vector;

using namespace ZooLib;

namespace ZooLib {
namespace Util_NS {

// =================================================================================================
#pragma mark - Util_NS

Val_ZZ sDAsZZ(const Val_ZZ& iDefault, NSObject* iVal)
	{
	if (iVal)
		{
		@try
			{
			return [iVal asZZWithDefault:iDefault];
			}
		@catch (id ex)
			{}
		}
	return iDefault;
	}

Val_ZZ sAsZZ(NSObject* iVal)
	{ return sDAsZZ(Val_ZZ(), iVal); }

NSObject* sDAsNSObject(NSObject* iDefault, const Val_ZZ& iVal)
	{
	if (false)
		{}
	else if (iVal.IsNull())
		{
		return [NSNull null];
		}
	else if (const string8* theValue = iVal.PGet<string8>())
		{
		return sString(*theValue);
		}
	else if (const vector<char>* theValue = iVal.PGet<vector<char>>())
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const Data_ZZ* theValue = iVal.PGet<Data_ZZ>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetPtr(), theSize);
		else
			return sData();
		}
	else if (const Seq_ZZ* theValue = iVal.PGet<Seq_ZZ>())
		{
		NSMutableArray* theArray = sArrayMutable();
		for (size_t xx = 0, count = theValue->Size(); xx < count; ++xx)
			[theArray addObject:sDAsNSObject(iDefault, theValue->Get(xx))];
		return theArray;
		}
	else if (const Map_ZZ* theValue = iVal.PGet<Map_ZZ>())
		{
		NSMutableDictionary* theDictionary = sDictionaryMutable();
		for (Map_ZZ::Index_t ii = theValue->Begin(), end = theValue->End(); ii != end; ++ii)
			{
			[theDictionary
				setObject:sDAsNSObject(iDefault, theValue->Get(ii))
				forKey:sString(theValue->NameOf(ii))];
			}
		return theDictionary;
		}
	else if (const bool* theValue = iVal.PGet<bool>())
		{
		return [NSNumber numberWithBool:(BOOL)*theValue];
		}
	else if (const UTCDateTime* theValue = iVal.PGet<UTCDateTime>())
		{
		return [NSDate dateWithTimeIntervalSince1970:sGet(*theValue)];
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
	else if (const long double* theValue = iVal.PGet<long double>())
		{
		return [NSNumber numberWithDouble:double(*theValue)];
		}

	return iDefault;
	}

NSObject* sAsNSObject(const Val_ZZ& iVal)
	{ return sDAsNSObject([NSNull null], iVal); }

} // namespace Util_NS
} // namespace ZooLib

// =================================================================================================
@implementation NSObject (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{
	// Hmm, log and return null or what?
	ZDebugLogf(0, ("NSObject (ZooLib_Any_Additions) asZZWithDefault called"));
	return iDefault;
	}

@end

// =================================================================================================
@interface NSNull (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSNull (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{ return Val_ZZ(); }

@end

// =================================================================================================
@interface NSDictionary (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSDictionary (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{
	Map_ZZ result;
	for (id theKey, ii = [self keyEnumerator]; (theKey = [ii nextObject]); /*no inc*/)
		{
		const string8 theName = Util_NS::sAsUTF8((NSString*)theKey);
		const Val_ZZ theVal = [[self objectForKey:theKey] asZZWithDefault:iDefault];
		result.Set(theName, theVal);
		}
	return result;
	}

@end

// =================================================================================================
@interface NSArray (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSArray (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{
	Seq_ZZ result;
	for (id theValue, ii = [self objectEnumerator]; (theValue = [ii nextObject]); /*no inc*/)
		result.Append([theValue asZZWithDefault:iDefault].As<Val_ZZ>());
	return result;
	}

@end

// =================================================================================================
@interface NSData (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSData (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{ return Data_ZZ([self bytes], [self length]); }

@end

// =================================================================================================
@interface NSString (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSString (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{ return string8([self UTF8String]); }

@end

// =================================================================================================
@interface NSNumber (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSNumber (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{ return int64([self longLongValue]); }

@end

// =================================================================================================
@interface NSDate (ZooLib_Any_Additions)
-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault;
@end

@implementation NSDate (ZooLib_Any_Additions)

-(Val_ZZ)asZZWithDefault:(const Val_ZZ&)iDefault
	{ return ZooLib::UTCDateTime([self timeIntervalSince1970]); }

@end

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)
