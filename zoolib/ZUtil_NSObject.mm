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

#include "zoolib/ZUtil_NSObject.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZUnicode.h"

#import <Foundation/NSString.h>

using std::string;
using std::vector;

NAMESPACE_ZOOLIB_USING

// =================================================================================================

@interface NSObject (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end

@interface NSDictionary (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end

@interface NSArray (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end

@interface NSString (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end

@interface NSNumber (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end

/*
@interface NSData (ZTValueAdditions)
-(ZTValue)AsZTValue;
@end
*/

// =================================================================================================

@implementation NSObject (ZTValueAdditions)

-(ZTValue)AsZTValue
	{
	// Hmm, log and return null or what?
	ZDebugLogf(0, ("NSObject (ZTValueAdditions) AsZTValue called"));
	return ZTValue();
	}

@end

// =================================================================================================

@implementation NSDictionary (ZTValueAdditions)

-(ZTValue)AsZTValue
	{
	ZTuple result;
	for (id theKey, i = [self keyEnumerator]; (theKey = [i nextObject]); /*no inc*/)
		{
		const ZTValue theValue = [[self objectForKey:theKey] AsZTValue];
		const string theName = ZUtil_NSObject::sAsUTF8((NSString*)theKey);
		result.Set(theName, theValue);
		}
	return result;
	}

@end

// =================================================================================================

@implementation NSArray (ZTValueAdditions)

-(ZTValue)AsZTValue
	{
	ZTValue result;
	vector<ZTValue>& theVec = result.MutableList().MutableVector();
	for (id theValue, i = [self objectEnumerator]; (theValue = [i nextObject]); /*no inc*/)
		theVec.push_back([theValue AsZTValue]);
	return result;
	}

@end

// =================================================================================================

@implementation NSString (ZTValueAdditions)

-(ZTValue)AsZTValue
	{
	return ZTValue([self UTF8String]);
	}

@end

// =================================================================================================

@implementation NSNumber (ZTValueAdditions)

-(ZTValue)AsZTValue
	{
	return ZTValue([self longLongValue]);
	}

@end

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_NSObject

NAMESPACE_ZOOLIB_BEGIN

NSString* ZUtil_NSObject::sCreateNSString_UTF8(const string8& iString8)
	{ return [[NSString alloc] initWithUTF8String:iString8.c_str()]; }

NSString* ZUtil_NSObject::sCreateNSString_UTF16(const string16& iString16)
	{
	return [[NSString alloc]
		initWithCharacters:(const unichar*)iString16.c_str()
		length:iString16.length()];
	}

string8 ZUtil_NSObject::sAsUTF8(NSString* iNSString)
	{ return [iNSString UTF8String]; }

string16 ZUtil_NSObject::sAsUTF16(NSString* iNSString)
	{ return ZUnicode::sAsUTF16([iNSString UTF8String]); }

ZTValue ZUtil_NSObject::sAsTV(id iNSObject)
	{
	return [iNSObject AsZTValue];
	}

id ZUtil_NSObject::sCreateNSObject(const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Tuple:
			{
			return sCreateNSDictionary(iTV.GetTuple());
			}
		case eZType_String:
			{
			return sCreateNSString_UTF8(iTV.GetString());
			}
		case eZType_Vector:
			{
			return sCreateNSArray(iTV.GetList().GetVector());
			}
		case eZType_Raw:
			{
			const ZValData_ZooLib theData = iTV.GetData();
			return [[NSData alloc] initWithBytes:theData.GetData() length:theData.GetSize()];
			}
		case eZType_Bool:
			{
			return [[NSNumber alloc] initWithBool:iTV.GetBool()];
			}
		case eZType_Int8:
			{
			return [[NSNumber alloc] initWithChar:iTV.GetInt8()];
			}
		case eZType_Int16:
			{
			return [[NSNumber alloc] initWithShort:iTV.GetInt16()];
			}
		case eZType_Int32:
			{
			return [[NSNumber alloc] initWithInt:iTV.GetInt32()]; //?? int or long??
			}
		case eZType_Int64:
			{
			return [[NSNumber alloc] initWithLongLong:iTV.GetInt64()];
			}
		case eZType_Float:
			{
			return [[NSNumber alloc] initWithFloat:iTV.GetFloat()];
			}
		case eZType_Double:
			{
			return [[NSNumber alloc] initWithDouble:iTV.GetDouble()];
			}
		}
	return nullptr;
	}

ZTuple ZUtil_NSObject::sAsTuple(NSDictionary* iNSDictionary)
	{
	ZTuple result;
	for (id theKey, i = [iNSDictionary keyEnumerator]; (theKey = [i nextObject]); /*no inc*/)
		{
		const ZTValue theValue = sAsTV([iNSDictionary objectForKey:theKey]);
		result.Set(sAsUTF8((NSString*)theKey), theValue);
		}
	return result;
	}

NSDictionary* ZUtil_NSObject::sCreateNSDictionary(const ZTuple& iTuple)
	{
	vector<id> keys;
	vector<id> values;

	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		{
		id theKey = sCreateNSString_UTF8(iTuple.NameOf(i).AsString());
		keys.push_back(theKey);

		id theValue = sCreateNSObject(iTuple.Get(i));
		values.push_back(theValue);
		}

	return [[NSDictionary alloc]
		initWithObjects:&values[0]
		forKeys:&keys[0]
		count:values.size()];
	}

void ZUtil_NSObject::sAsVector(NSArray* iNSArray, vector<ZTValue>& oVector)
	{
	for (id theValue, i = [iNSArray objectEnumerator]; (theValue = [i nextObject]); /*no inc*/)
		oVector.push_back(sAsTV(theValue));
	}

NSArray* ZUtil_NSObject::sCreateNSArray(const vector<ZTValue>& iVector)
	{
	vector<id> values;
	for (vector<ZTValue>::const_iterator i = iVector.begin(); i != iVector.end(); ++i)
		values.push_back(sCreateNSObject(*i));

	return [[NSArray alloc]
		initWithObjects:&values[0]
		count:values.size()];
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Cocoa)
