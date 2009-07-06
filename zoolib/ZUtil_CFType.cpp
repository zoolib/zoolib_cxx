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

#include "zoolib/ZRef_CFType.h"

#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

using std::vector;

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_CFType {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

static CFStringRef sEmptyCFString = CFSTR("");

ZRef<CFStringRef> sString(const string8& iString8)
	{ return NoRetain(sCreateCFString_UTF8(iString8)); }

ZRef<CFStringRef> sString(const string16& iString16)
	{ return NoRetain(sCreateCFString_UTF16(iString16)); }

ZRef<CFMutableStringRef> sStringMutable()
	{ return NoRetain(::CFStringCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableStringRef> sStringMutable(const string8& iString8)
	{ return sStringMutable(sString(iString8)); }

ZRef<CFMutableStringRef> sStringMutable(const string16& iString16)
	{ return sStringMutable(sString(iString16)); }

ZRef<CFMutableStringRef> sStringMutable(const ZRef<CFStringRef>& iCFString)
	{ return NoRetain(::CFStringCreateMutableCopy(kCFAllocatorDefault, 0, iCFString)); }

ZRef<CFDictionaryRef> sDictionary(const ZTuple& iTuple)
	{ return NoRetain(sCreateCFDictionary(iTuple)); }

ZRef<CFMutableDictionaryRef> sDictionaryMutable()
	{
	return NoRetain(::CFDictionaryCreateMutable(kCFAllocatorDefault, 1,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}
ZRef<CFMutableDictionaryRef> sDictionaryMutable(const ZRef<CFDictionaryRef>& iCFDictionary)
	{ return NoRetain(::CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, iCFDictionary)); }

ZRef<CFArrayRef> sArray(const std::vector<ZTValue>& iVector)
	{ return NoRetain(sCreateCFArray(iVector)); }

ZRef<CFMutableArrayRef> sArrayMutable()
	{ return NoRetain(::CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable(const ZRef<CFArrayRef>& iCFArray)
	{ return NoRetain(::CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, iCFArray)); }

ZRef<CFTypeRef> sType(const ZTValue& iTV)
	{ return NoRetain(sCreateCFType(iTV)); }

ZType sTypeOf(CFTypeRef iCFType)
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
		else if (theTypeID == ::CFDateGetTypeID())
			{
			return eZType_Time;
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

string8 sAsUTF8(CFStringRef iCFString)
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

string16 sAsUTF16(CFStringRef iCFString)
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

static void sApplier(const void* iKey, const void* iValue, void* iRefcon)
	{
	// This could be dodgy if iKey or iValue are not CF types.
	static_cast<ZTuple*>(iRefcon)->Set(
		ZTName(sAsUTF8(static_cast<CFStringRef>(iKey))),
		sAsTV(static_cast<CFTypeRef>(iValue)));
	}

ZTuple sAsTuple(CFDictionaryRef iCFDictionary)
	{
	ZTuple result;
	::CFDictionaryApplyFunction(iCFDictionary, sApplier, &result);
	return result;
	}

static void sApplier(const void* iValue, void* iRefcon)
	{
	static_cast<vector<ZTValue>*>(iRefcon)->push_back(
		sAsTV(static_cast<CFTypeRef>(iValue)));
	}

void sAsVector(CFArrayRef iCFArray, vector<ZTValue>& oVector)
	{
	const CFRange theRange = { 0, ::CFArrayGetCount(iCFArray)};
	::CFArrayApplyFunction(iCFArray, theRange, sApplier, &oVector);	
	}

ZTValue sAsTV(CFTypeRef iCFType)
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
			ZTValue result;
			sAsVector(static_cast<CFArrayRef>(iCFType), result.MutableList().MutableVector());
			return result;
			}
		else if (theTypeID == ::CFStringGetTypeID())
			{
			return ZTValue(sAsUTF8(static_cast<CFStringRef>(iCFType)));
			}
		else if (theTypeID == ::CFBooleanGetTypeID())
			{
			return ZTValue(bool(::CFBooleanGetValue(static_cast<CFBooleanRef>(iCFType))));
			}
		else if (theTypeID == ::CFDateGetTypeID())
			{
			return ZTValue(ZTime(kCFAbsoluteTimeIntervalSince1970
				+ ::CFDateGetAbsoluteTime(static_cast<CFDateRef>(iCFType))));
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

CFStringRef sCreateCFString_UTF8(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return ::CFStringCreateWithBytes(nullptr,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false);
		}

	::CFRetain(sEmptyCFString);
	return sEmptyCFString;
	}

CFStringRef sCreateCFString_UTF16(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return ::CFStringCreateWithCharacters(nullptr,
			reinterpret_cast<const UniChar*>(iString16.data()), sourceSize);
		}

	::CFRetain(sEmptyCFString);
	return sEmptyCFString;
	}

CFDictionaryRef sCreateCFDictionary(const ZTuple& iTuple)
	{
	// Build local vector of keys and values.
	vector<ZRef<CFStringRef> > keys;
	vector<ZRef<CFTypeRef> > values;

	for (ZTuple::const_iterator i = iTuple.begin(); i != iTuple.end(); ++i)
		{
		keys.push_back(sString(iTuple.NameOf(i).AsString()));
		values.push_back(sType(iTuple.Get(i)));
		}

	CFDictionaryRef theDictionaryRef = ::CFDictionaryCreate(kCFAllocatorDefault,
		(CFTypeRef*)&keys[0], (CFTypeRef*)&values[0], keys.size(),
		&kCFCopyStringDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

	return theDictionaryRef;
	}

CFArrayRef sCreateCFArray(const vector<ZTValue>& iVector)
	{
	vector<ZRef<CFTypeRef> > values;
	for (vector<ZTValue>::const_iterator i = iVector.begin(); i != iVector.end(); ++i)
		values.push_back(sType(*i));

	CFArrayRef theCFArrayRef = ::CFArrayCreate(kCFAllocatorDefault,
		(CFTypeRef*)&values[0], values.size(), &kCFTypeArrayCallBacks);

	ZAssert(values.size() == ::CFArrayGetCount(theCFArrayRef));

	return theCFArrayRef;
	}

CFTypeRef sCreateCFType(const ZTValue& iTV)
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
			return sCreateCFArray(iTV.GetList().GetVector());
			}
		case eZType_Raw:
			{
			const ZMemoryBlock theMB = iTV.GetRaw();
			return ::CFDataCreate(kCFAllocatorDefault,
				static_cast<const UInt8*>(theMB.GetData()), theMB.GetSize());
			}
		case eZType_Bool:
			{
			if (iTV.GetBool())
				return kCFBooleanTrue;
			else
				return kCFBooleanFalse;
			}
		case eZType_Time:
			{
			return ::CFDateCreate(kCFAllocatorDefault,
				iTV.GetTime().fVal - kCFAbsoluteTimeIntervalSince1970);
			}
		case eZType_Int8:
			{
			const int8 theValue = iTV.GetInt8();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberSInt8Type, &theValue);
			}
		case eZType_Int16:
			{
			const int16 theValue = iTV.GetInt16();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberSInt16Type, &theValue);
			}
		case eZType_Int32:
			{
			const int32 theValue = iTV.GetInt32();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberSInt32Type, &theValue);
			}
		case eZType_Int64:
			{
			const int64 theValue = iTV.GetInt64();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberSInt64Type, &theValue);
			}
		case eZType_Float:
			{
			const float theValue = iTV.GetFloat();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberFloatType, &theValue);
			}
		case eZType_Double:
			{
			const double theValue = iTV.GetDouble();
			return ::CFNumberCreate(kCFAllocatorDefault,
				kCFNumberDoubleType, &theValue);
			}
		}
	return nullptr;
	}

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
