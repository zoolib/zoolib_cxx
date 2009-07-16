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

#include "zoolib/ZVal_ZooLib.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_CFType {

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

static CFStringRef sEmptyCFString = CFSTR("");

ZRef<CFStringRef> sString()
	{ return sEmptyCFString; }

ZRef<CFStringRef> sString(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return NoRetain(::CFStringCreateWithBytes(nullptr,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false));
		}

	return sEmptyCFString;
	}

ZRef<CFStringRef> sString(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return NoRetain(::CFStringCreateWithCharacters(nullptr,
			reinterpret_cast<const UniChar*>(iString16.data()), sourceSize));
		}

	return sEmptyCFString;
	}

ZRef<CFMutableStringRef> sStringMutable()
	{ return NoRetain(::CFStringCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableStringRef> sStringMutable(const string8& iString8)
	{ return sStringMutable(sString(iString8)); }

ZRef<CFMutableStringRef> sStringMutable(const string16& iString16)
	{ return sStringMutable(sString(iString16)); }

ZRef<CFMutableStringRef> sStringMutable(const ZRef<CFStringRef>& iCFString)
	{ return NoRetain(::CFStringCreateMutableCopy(kCFAllocatorDefault, 0, iCFString)); }

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

ZRef<CFDictionaryRef> sDictionary()
	{
	return NoRetain(::CFDictionaryCreate(kCFAllocatorDefault, nullptr, nullptr, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}

ZRef<CFMutableDictionaryRef> sDictionaryMutable()
	{
	return NoRetain(::CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}

ZRef<CFMutableDictionaryRef> sDictionaryMutable(const ZRef<CFDictionaryRef>& iCFDictionary)
	{
	return NoRetain(::CFDictionaryCreateMutableCopy(kCFAllocatorDefault,
		::CFDictionaryGetCount(iCFDictionary), iCFDictionary));
	}

ZRef<CFArrayRef> sArray()
	{ return NoRetain(::CFArrayCreate(kCFAllocatorDefault, nullptr, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable()
	{ return NoRetain(::CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable(const ZRef<CFArrayRef>& iCFArray)
	{
	return NoRetain(::CFArrayCreateMutableCopy(kCFAllocatorDefault,
		::CFArrayGetCount(iCFArray), iCFArray));
	}

ZRef<CFMutableDataRef> sDataMutable()
	{ return NoRetain(::CFDataCreateMutable(kCFAllocatorDefault, 0)); }

ZRef<CFMutableDataRef> sDataMutable(size_t iSize)
	{
	ZRef<CFMutableDataRef> theData = NoRetain(::CFDataCreateMutable(kCFAllocatorDefault, 0));
	::CFDataSetLength(theData, iSize);
	return theData;
	}

ZRef<CFMutableDataRef> sDataMutable(const ZRef<CFDataRef>& iCFData)
	{ return NoRetain(::CFDataCreateMutableCopy(kCFAllocatorDefault, 0, iCFData)); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

bool sQAsVal_ZooLib(ZRef<CFTypeRef> iVal, ZVal_ZooLib& oVal)
	{
	CFTypeRef theCFTypeRef = iVal;
	if (!theCFTypeRef)
		{
		oVal.Clear();
		return true;
		}

	const CFTypeID theTypeID = ::CFGetTypeID(theCFTypeRef);

	if (theTypeID == ::CFStringGetTypeID())
		{
		oVal = ZVal_ZooLib(sAsUTF8(static_cast<CFStringRef>(theCFTypeRef)));
		return true;
		}

	if (theTypeID == ::CFBooleanGetTypeID())
		{
		oVal = ZVal_ZooLib(bool(::CFBooleanGetValue(static_cast<CFBooleanRef>(theCFTypeRef))));
		return true;
		}
	
	if (theTypeID == ::CFDateGetTypeID())
		{
		oVal = ZVal_ZooLib(ZTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime(static_cast<CFDateRef>(theCFTypeRef))));
		return true;
		}
	
	if (theTypeID == ::CFDataGetTypeID())
		{
		CFDataRef theDataRef = static_cast<CFDataRef>(theCFTypeRef);
		size_t theLength = ::CFDataGetLength(theDataRef);
		const void* theData = ::CFDataGetBytePtr(theDataRef);
		oVal = ZVal_ZooLib(theData, theLength);
		return true;
		}
	
	if (theTypeID == ::CFNumberGetTypeID())
		{
		const CFNumberRef theNumberRef = static_cast<CFNumberRef>(theCFTypeRef);
		switch (::CFNumberGetType(theNumberRef))
			{
			case kCFNumberSInt8Type:
			case kCFNumberCharType:
				{
				int8 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt8Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			case kCFNumberSInt16Type:
			case kCFNumberShortType:
				{
				int16 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			case kCFNumberSInt32Type:
			case kCFNumberIntType:
				{
				int32 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			case kCFNumberSInt64Type:
			case kCFNumberLongLongType:
				{
				int64 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			case kCFNumberFloat32Type:
			case kCFNumberFloatType:
				{
				float theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			case kCFNumberFloat64Type:
			case kCFNumberDoubleType:
				{
				double theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
				oVal = ZVal_ZooLib(theValue);
				return true;
				}
			}
		}

	return false;
	}

ZVal_ZooLib sAsVal_ZooLib(ZRef<CFTypeRef> iVal)
	{
	ZVal_ZooLib result;
	if (sQAsVal_ZooLib(iVal, result))
		return result;
	return ZVal_ZooLib();
	}

bool sQAsCFType(const ZVal_ZooLib& iVal, ZRef<CFTypeRef>& oVal)
	{
	switch (iVal.TypeOf())
		{
		case eZType_Null:
			{
			oVal.Clear();
			return true;
			}
		case eZType_String:
			{
			oVal = sString(iVal.GetString());
			return true;
			}
		case eZType_Raw:
			{
			const ZValData_ZooLib& theData = iVal.GetData();
			oVal = Adopt_T<CFTypeRef>(::CFDataCreate(kCFAllocatorDefault,
				static_cast<const UInt8*>(theData.GetData()), theData.GetSize()));
			return true;
			}
		case eZType_Bool:
			{
			if (iVal.GetBool())
				oVal = kCFBooleanTrue;
			else
				oVal = kCFBooleanFalse;
			return true;
			}
		case eZType_Time:
			{
			oVal = Adopt_T<CFTypeRef>(::CFDateCreate(kCFAllocatorDefault,
				iVal.GetTime().fVal - kCFAbsoluteTimeIntervalSince1970));
			return true;
			}
		case eZType_Int8:
			{
			const int8 theValue = iVal.GetInt8();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberSInt8Type, &theValue));
			return true;
			}
		case eZType_Int16:
			{
			const int16 theValue = iVal.GetInt16();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberSInt16Type, &theValue));
			return true;
			}
		case eZType_Int32:
			{
			const int32 theValue = iVal.GetInt32();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberSInt32Type, &theValue));
			return true;
			}
		case eZType_Int64:
			{
			const int64 theValue = iVal.GetInt64();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberSInt64Type, &theValue));
			return true;
			}
		case eZType_Float:
			{
			const float theValue = iVal.GetFloat();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberFloatType, &theValue));
			return true;
			}
		case eZType_Double:
			{
			const double theValue = iVal.GetDouble();
			oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
				kCFAllocatorDefault, kCFNumberDoubleType, &theValue));
			return true;
			}
		}
	return false;
	}

ZRef<CFTypeRef> sAsCFType(const ZVal_ZooLib& iVal)
	{
	ZRef<CFTypeRef> result;
	if (sQAsCFType(iVal, result))
		return result;

	return ZRef<CFTypeRef>();
	}

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
