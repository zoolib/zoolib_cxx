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

#include "zoolib/ZUtil_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZRefCFType.h"

#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

static CFStringRef sEmptyCFString = CFSTR("");

CFStringRef ZUtil_CFType::sCreateCFString_UTF8(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return ::CFStringCreateWithBytes(nil,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false);
		}

	::CFRetain(sEmptyCFString);
	return sEmptyCFString;
	}

CFStringRef ZUtil_CFType::sCreateCFString_UTF16(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return ::CFStringCreateWithCharacters(nil,
				reinterpret_cast<const UniChar*>(iString16.data()), sourceSize);
		}

	::CFRetain(sEmptyCFString);
	return sEmptyCFString;
	}

CFMutableStringRef ZUtil_CFType::sCreateMutableCFString_UTF8(const string8& iString8)
	{
	ZRef<CFStringRef> theStringRef = NoRetain(sCreateCFString_UTF8(iString8));
	CFMutableStringRef theMutableStringRef = ::CFStringCreateMutableCopy(0, 0, theStringRef);
	return theMutableStringRef;
	}

CFMutableStringRef ZUtil_CFType::sCreateMutableCFString_UTF16(const string16& iString16)
	{
	ZRef<CFStringRef> theStringRef = NoRetain(sCreateCFString_UTF16(iString16));
	CFMutableStringRef theMutableStringRef = ::CFStringCreateMutableCopy(0, 0, theStringRef);
	return theMutableStringRef;
	}

string8 ZUtil_CFType::sAsUTF8(const CFStringRef& iCFString)
	{
	if (const char *s = ::CFStringGetCStringPtr(iCFString, kCFStringEncodingUTF8))
		return string8(s);

	const CFIndex sourceCU = ::CFStringGetLength(iCFString);
	if (sourceCU == 0)
		return string8();

	// Worst case is six bytes per code unit.
	const size_t bufferSize = sourceCU * 6;
	string8 result(bufferSize, 0);

	UInt8* buffer = reinterpret_cast<UInt8*>(const_cast<char*>(result.data()));

	CFIndex bufferUsed;
	::CFStringGetBytes(iCFString, CFRangeMake(0, sourceCU),
		kCFStringEncodingUTF8, 1, false,
		buffer, bufferSize, &bufferUsed);

	result.resize(bufferUsed);

	return result;
	}

string16 ZUtil_CFType::sAsUTF16(const CFStringRef& iCFString)
	{
	const CFIndex sourceCU = ::CFStringGetLength(iCFString);
	if (sourceCU == 0)
		return string16();

	if (const UniChar* s = ::CFStringGetCharactersPtr(iCFString))
		return string16(reinterpret_cast<const UTF16*>(s), sourceCU);

	string16 result(sourceCU, 0);

	UniChar* buffer = reinterpret_cast<UniChar*>(const_cast<UTF16*>(result.data()));

	::CFStringGetCharacters(iCFString, CFRangeMake(0, sourceCU), buffer);
	return result;
	}

ZType ZUtil_CFType::sTypeOf(CFTypeRef iCFType)
	{
	if (iCFType)
		{
		const CFTypeID theTypeID = ::CFGetTypeID(iCFType);
		if (theTypeID == ::CFDictionaryGetTypeID())
			{
			return eZType_Tuple;
			}
		else if (theTypeID == ::CFArrayGetTypeID())
			{
			return eZType_Vector;
			}
		else if (theTypeID == ::CFStringGetTypeID())
			{
			return eZType_String;
			}
		else if (theTypeID == ::CFBooleanGetTypeID())
			{
			return eZType_Bool;
			}
		else if (theTypeID == ::CFDataGetTypeID())
			{
			return eZType_Raw;
			}
		else if (theTypeID == ::CFNumberGetTypeID())
			{
			const CFNumberRef theNumberRef = static_cast<CFNumberRef>(iCFType);
			switch (::CFNumberGetType(theNumberRef))
				{
				case kCFNumberSInt8Type:
				case kCFNumberCharType:
					{
					return eZType_Int8;
					}
				case kCFNumberSInt16Type:
				case kCFNumberShortType:
					{
					return eZType_Int16;
					}
				case kCFNumberSInt32Type:
				case kCFNumberIntType:
					{
					return eZType_Int32;
					}
				case kCFNumberSInt64Type:
				case kCFNumberLongLongType:
					{
					return eZType_Int64;
					}
				case kCFNumberFloat32Type:
				case kCFNumberFloatType:
					{
					return eZType_Float;
					}
				case kCFNumberFloat64Type:
				case kCFNumberDoubleType:
					{
					return eZType_Double;
					}
				}
			}
		}
	return eZType_Null;
	}

ZTValue ZUtil_CFType::sAsTV(CFTypeRef iCFType)
	{
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

	return ZTValue();
	}

CFTypeRef ZUtil_CFType::sCreateCFType(const ZTValue& iTV)
	{
	switch (iTV.TypeOf())
		{
		case eZType_Tuple:
			{
			return sCreateCFDictionary(iTV.GetTuple());
			}
		case eZType_String:
			{
			return sCreateCFString_UTF8(iTV.GetString());
			}
		case eZType_Vector:
			{
			return sCreateCFArray(iTV.GetVector());
			}
		case eZType_Raw:
			{
			const void* theAddress;
			size_t theSize;
			iTV.GetRawAttributes(&theAddress, &theSize);
			return ::CFDataCreate(nil, static_cast<const UInt8*>(theAddress), theSize);
			}
		case eZType_Bool:
			{
			if (iTV.GetBool())
				return kCFBooleanTrue;
			else
				return kCFBooleanFalse;
			}
		case eZType_Int8:
			{
			const int8 theValue = iTV.GetInt8();
			return ::CFNumberCreate(nil, kCFNumberSInt8Type, &theValue);
			}
		case eZType_Int16:
			{
			const int16 theValue = iTV.GetInt16();
			return ::CFNumberCreate(nil, kCFNumberSInt16Type, &theValue);
			}
		case eZType_Int32:
			{
			const int32 theValue = iTV.GetInt32();
			return ::CFNumberCreate(nil, kCFNumberSInt32Type, &theValue);
			}
		case eZType_Int64:
			{
			const int64 theValue = iTV.GetInt64();
			return ::CFNumberCreate(nil, kCFNumberSInt64Type, &theValue);
			}
		case eZType_Float:
			{
			const float theValue = iTV.GetFloat();
			return ::CFNumberCreate(nil, kCFNumberFloatType, &theValue);
			}
		case eZType_Double:
			{
			const double theValue = iTV.GetDouble();
			return ::CFNumberCreate(nil, kCFNumberDoubleType, &theValue);
			}
		}
	return nil;
	}

static void sApplier(const void* iKey, const void* iValue, void* iRefcon)
	{
	// This could be dodgy if iKey or iValue are not CF types.
	static_cast<ZTuple*>(iRefcon)->SetValue(
		ZTName(ZUtil_CFType::sAsUTF8(static_cast<CFStringRef>(iKey))),
		ZUtil_CFType::sAsTV(static_cast<CFTypeRef>(iValue)));
	}

ZTuple ZUtil_CFType::sAsTuple(CFDictionaryRef iCFDictionary)
	{
	ZTuple result;
	::CFDictionaryApplyFunction(iCFDictionary, sApplier, &result);
	return result;
	}

CFDictionaryRef ZUtil_CFType::sCreateCFDictionary(const ZTuple& iTuple)
	{
	// Build local vector of keys and values.
	vector<ZRef<CFStringRef> > keys;
	vector<ZRef<CFTypeRef> > values;

	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		{
		ZRef<CFStringRef> theStringRef
			= NoRetain(sCreateCFString_UTF8(iTuple.NameOf(i).AsString()));
		keys.push_back(theStringRef);

		ZRef<CFTypeRef> theTypeRef = NoRetain(sCreateCFType(iTuple.GetValue(i)));
		values.push_back(theTypeRef);
		}

	CFDictionaryRef theDictionaryRef = ::CFDictionaryCreate(kCFAllocatorDefault,
		(CFTypeRef*)&keys[0], (CFTypeRef*)&values[0], keys.size(),
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	return theDictionaryRef;
	}

static void sApplier(const void* iValue, void* iRefcon)
	{
	static_cast<vector<ZTValue>*>(iRefcon)->push_back(
		ZUtil_CFType::sAsTV(static_cast<CFTypeRef>(iValue)));
	}

void ZUtil_CFType::sAsVector(CFArrayRef iCFArray, vector<ZTValue>& oVector)
	{
	const CFRange theRange = { 0, ::CFArrayGetCount(iCFArray)};
	::CFArrayApplyFunction(iCFArray, theRange, sApplier, &oVector);	
	}

CFArrayRef ZUtil_CFType::sCreateCFArray(const vector<ZTValue>& iVector)
	{
	vector<ZRef<CFTypeRef> > values;
	for (vector<ZTValue>::const_iterator i = iVector.begin(); i != iVector.end(); ++i)
		values.push_back(ZRef<CFTypeRef>(NoRetain(sCreateCFType(*i))));

	CFArrayRef theCFArrayRef = ::CFArrayCreate(kCFAllocatorDefault,
		(CFTypeRef*)&values[0], values.size(), &kCFTypeArrayCallBacks );

	ZAssert(values.size() == ::CFArrayGetCount(theCFArrayRef));

	return theCFArrayRef;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
