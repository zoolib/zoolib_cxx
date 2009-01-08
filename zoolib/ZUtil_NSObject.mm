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

#import <Foundation/NSString.h>

NAMESPACE_ZOOLIB_USING

using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_NSObject

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
	ZUnimplemented();
#if 0
	if (iCFType)
		{
		const CFTypeID theTypeID = ::CFGetTypeID(iCFType);
		if (theTypeID == ::CFDictionaryGetTypeID())
			{
			return sAsTuple(static_cast<CFDictionaryRef>(iCFType));
			}
		else if (theTypeID == ::CFArrayGetTypeID())
			{
			vector<ZTValue> theVector;
			sAsVector(static_cast<CFArrayRef>(iCFType), theVector);
			return theVector;
			}
		else if (theTypeID == ::CFStringGetTypeID())
			{
			return ZTValue(sAsUTF8(static_cast<CFStringRef>(iCFType)));
			}
		else if (theTypeID == ::CFBooleanGetTypeID())
			{
			return ZTValue(bool(::CFBooleanGetValue(static_cast<CFBooleanRef>(iCFType))));
			}
		else if (theTypeID == ::CFDataGetTypeID())
			{
			CFDataRef theDataRef = static_cast<CFDataRef>(iCFType);
			size_t theLength = ::CFDataGetLength(theDataRef);
			const void* theData = ::CFDataGetBytePtr(theDataRef);
			return ZTValue(theData, theLength);
			}
		else if (theTypeID == ::CFNumberGetTypeID())
			{
			const CFNumberRef theNumberRef = static_cast<CFNumberRef>(iCFType);
			switch (::CFNumberGetType(theNumberRef))
				{
				case kCFNumberSInt8Type:
				case kCFNumberCharType:
					{
					int8 theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberSInt8Type, &theValue);
					return ZTValue(theValue);
					}
				case kCFNumberSInt16Type:
				case kCFNumberShortType:
					{
					int16 theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
					return ZTValue(theValue);
					}
				case kCFNumberSInt32Type:
				case kCFNumberIntType:
					{
					int32 theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
					return ZTValue(theValue);
					}
				case kCFNumberSInt64Type:
				case kCFNumberLongLongType:
					{
					int64 theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
					return ZTValue(theValue);
					}
				case kCFNumberFloat32Type:
				case kCFNumberFloatType:
					{
					float theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
					return ZTValue(theValue);
					}
				case kCFNumberFloat64Type:
				case kCFNumberDoubleType:
					{
					double theValue;
					::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
					return ZTValue(theValue);
					}
				}
			}
		}
#endif
	return ZTValue();
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
			return sCreateNSArray(iTV.GetVector());
			}
		case eZType_Raw:
			{
			const void* theAddress;
			size_t theSize;
			iTV.GetRawAttributes(&theAddress, &theSize);
			return [[NSData alloc] initWithBytes:theAddress length:theSize];
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
	return nil;
	}

ZTuple ZUtil_NSObject::sAsTuple(NSDictionary* iNSDictionary)
	{
	ZTuple result;
	for (id theKey, i = [iNSDictionary keyEnumerator]; (theKey = [i nextObject]); /*no inc*/)
		{
		const ZTValue theValue = sAsTV([iNSDictionary objectForKey:theKey]);
		result.SetValue(sAsUTF8((NSString*)theKey), theValue);
		}
	return result;
	}

NSDictionary* ZUtil_NSObject::sCreateNSDictionary(const ZTuple& iTuple)
	{
	// Build local vector of keys and values.
//	vector<NSString*> keys;
	vector<id> keys;
	vector<id> values;

	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		{
		id theKey = sCreateNSString_UTF8(iTuple.NameOf(i).AsString());
		id theValue = sCreateNSObject(iTuple.GetValue(i));

		keys.push_back(theKey);
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

#endif // ZCONFIG_SPI_Enabled(Cocoa)
