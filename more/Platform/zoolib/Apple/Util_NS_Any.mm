/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/Apple/Util_NS_Any.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Apple/Util_NS.h"

#include "zoolib/Unicode.h"
#include "zoolib/Val_Any.h"

#include "zoolib/ZTime.h"

#import <Foundation/NSDate.h>
#import <Foundation/NSString.h>

using std::vector;

using namespace ZooLib;

namespace ZooLib {
namespace Util_NS {

// =================================================================================================
// MARK: - Util_NS

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
	else if (iVal.IsNull())
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
	else if (const Data_Any* theValue = iVal.PGet<Data_Any>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetPtr(), theSize);
		else
			return sData();
		}
	else if (const Seq_Any* theValue = iVal.PGet<Seq_Any>())
		{
		NSMutableArray* theArray = sArrayMutable();
		for (size_t xx = 0, count = theValue->Count(); xx < count; ++xx)
			[theArray addObject:sDAsNSObject(iDefault, theValue->Get(xx))];
		return theArray;
		}
	else if (const Map_Any* theValue = iVal.PGet<Map_Any>())
		{
		NSMutableDictionary* theDictionary = sDictionaryMutable();
		for (Map_Any::Index_t ii = theValue->Begin(), end = theValue->End(); ii != end; ++ii)
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
	else if (const long double* theValue = iVal.PGet<long double>())
		{
		return [NSNumber numberWithDouble:double(*theValue)];
		}

	return iDefault;
	}

NSObject* sAsNSObject(const ZAny& iVal)
	{ return sDAsNSObject([NSNull null], iVal); }

} // namespace Util_NS
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
	Map_Any result;
	for (id theKey, ii = [self keyEnumerator]; (theKey = [ii nextObject]); /*no inc*/)
		{
		const string8 theName = Util_NS::sAsUTF8((NSString*)theKey);
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
	Seq_Any result;
	for (id theValue, ii = [self objectEnumerator]; (theValue = [ii nextObject]); /*no inc*/)
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
	{ return ZAny(Data_Any([self bytes], [self length])); }

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
