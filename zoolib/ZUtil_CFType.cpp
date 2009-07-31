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

#include "zoolib/ZTime.h"
#include "zoolib/ZVal_Any.h"

#include <map>
#include <vector>

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;
using std::string;
using std::vector;

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

ZRef<CFDataRef> sData()
	{ return NoRetain(::CFDataCreate(kCFAllocatorDefault, 0, 0)); }

ZRef<CFDataRef> sData(const void* iSource, size_t iSize)
	{
	return NoRetain(::CFDataCreate(kCFAllocatorDefault,
		static_cast<const UInt8*>(iSource), iSize));
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

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	CFStringRef theKey = static_cast<CFStringRef>(iKey);
	CFTypeRef theValue = static_cast<CFTypeRef>(iValue);

	ZValMap_Any* theMap = static_cast<ZValMap_Any*>(iRefcon);
	theMap->Set(sAsUTF8(theKey), sAsAny(theValue));
	}

bool sQAsAny(ZRef<CFTypeRef> iVal, ZAny& oVal)
	{
	CFTypeRef theCFTypeRef = iVal;
	if (!theCFTypeRef)
		{
		oVal = ZAny();
		return true;
		}

	const CFTypeID theTypeID = ::CFGetTypeID(theCFTypeRef);

	if (theTypeID == ::CFStringGetTypeID())
		{
		oVal = sAsUTF8(static_cast<CFStringRef>(theCFTypeRef));
		return true;
		}

	if (theTypeID == ::CFDictionaryGetTypeID())
		{
		CFDictionaryRef theDictionaryRef = static_cast<CFDictionaryRef>(theCFTypeRef);
		ZValMap_Any theMap;
		::CFDictionaryApplyFunction(theDictionaryRef, spGatherContents, &theMap);
		oVal = theMap;
		return true;
		}

	if (theTypeID == ::CFArrayGetTypeID())
		{
		CFArrayRef theArrayRef = static_cast<CFArrayRef>(theCFTypeRef);
		ZValList_Any theList;

		for (size_t x = 0, theCount = ::CFArrayGetCount(theArrayRef); x < theCount; ++x)
			theList.Append(sAsAny(::CFArrayGetValueAtIndex(theArrayRef, x)));

		oVal = theList;

		return true;
		}

	if (theTypeID == ::CFBooleanGetTypeID())
		{
		oVal = bool(::CFBooleanGetValue(static_cast<CFBooleanRef>(theCFTypeRef)));
		return true;
		}
	
	if (theTypeID == ::CFDateGetTypeID())
		{
		oVal = ZTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime(static_cast<CFDateRef>(theCFTypeRef)));
		return true;
		}
	
	if (theTypeID == ::CFDataGetTypeID())
		{
		CFDataRef theDataRef = static_cast<CFDataRef>(theCFTypeRef);
		size_t theLength = ::CFDataGetLength(theDataRef);
		const char* theData
			= static_cast<const char*>(static_cast<const void*>(::CFDataGetBytePtr(theDataRef)));
		oVal = vector<char>(theData, theData + theLength);
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
				oVal = theValue;
				return true;
				}
			case kCFNumberSInt16Type:
			case kCFNumberShortType:
				{
				int16 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
				oVal = theValue;
				return true;
				}
			case kCFNumberSInt32Type:
			case kCFNumberIntType:
				{
				int32 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
				oVal = theValue;
				return true;
				}
			case kCFNumberSInt64Type:
			case kCFNumberLongLongType:
				{
				int64 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
				oVal = theValue;
				return true;
				}
			case kCFNumberFloat32Type:
			case kCFNumberFloatType:
				{
				float theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
				oVal = theValue;
				return true;
				}
			case kCFNumberFloat64Type:
			case kCFNumberDoubleType:
				{
				double theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
				oVal = theValue;
				return true;
				}
			}
		}

	return false;
	}

ZAny sAsAny(ZRef<CFTypeRef> iVal)
	{
	ZAny result;
	if (sQAsAny(iVal, result))
		return result;
	return ZAny();
	}

bool sQAsCFType(const ZAny& iAny, ZRef<CFTypeRef>& oVal)
	{
	if (false)
		{}
	else if (iAny.type() == typeid(void))
		{
		oVal.Clear();
		}
	else if (const string8* theValue = ZAnyCast<string8>(&iAny))
		{
		oVal = sString(*theValue);
		}
	else if (const vector<char>* theValue = ZAnyCast<vector<char> >(&iAny))
		{
		if (size_t theSize = theValue->size())
			oVal = sData(&(*theValue)[0], theSize);
		else
			oVal = sData();
		}
	else if (const vector<ZAny>* theValue = ZAnyCast<vector<ZAny> >(&iAny))
		{
		ZRef<CFMutableArrayRef> theArray;
		for (vector<ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			::CFArrayAppendValue(theArray, sAsCFType(*i));
			}
		oVal = theArray;
		}
	else if (const ZValList_Any* theValue = ZAnyCast<ZValList_Any>(&iAny))
		{
		ZRef<CFMutableArrayRef> theArray;
		for (size_t x = 0, count = theValue->Count(); x < count; ++x)
			{
			::CFArrayAppendValue(theArray, sAsCFType(theValue->Get(x)));
			}
		oVal = theArray;
		}
	else if (const map<string, ZAny>* theValue = ZAnyCast<map<string, ZAny> >(&iAny))
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (map<string, ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			::CFDictionarySetValue(theDictionary, sString((*i).first), sAsCFType((*i).second));
			}
		oVal = theDictionary;
		}
	else if (const ZValMap_Any* theValue = ZAnyCast<ZValMap_Any>(&iAny))
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (ZValMap_Any::Index_t i = theValue->Begin(), end = theValue->End();
			i != end; ++i)
			{
			::CFDictionarySetValue(theDictionary,
				sString(theValue->NameOf(i)), sAsCFType(theValue->Get(i)));
			}
		oVal = theDictionary;
		}
	else if (const bool* theValue = ZAnyCast<bool>(&iAny))
		{
		if (*theValue)
			oVal = kCFBooleanTrue;
		else
			oVal = kCFBooleanFalse;
		}
	else if (const ZTime* theValue = ZAnyCast<ZTime>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFDateCreate(kCFAllocatorDefault,
			theValue->fVal - kCFAbsoluteTimeIntervalSince1970));
		}
	else if (const int8* theValue = ZAnyCast<int8>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberSInt8Type, theValue));
		}
	else if (const int16* theValue = ZAnyCast<int16>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberSInt16Type, theValue));
		}
	else if (const int32* theValue = ZAnyCast<int32>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberSInt32Type, theValue));
		}
	else if (const int64* theValue = ZAnyCast<int64>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberSInt64Type, theValue));
		}
	else if (const float* theValue = ZAnyCast<float>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberFloatType, theValue));
		}
	else if (const double* theValue = ZAnyCast<double>(&iAny))
		{
		oVal = Adopt_T<CFTypeRef>(::CFNumberCreate(
			kCFAllocatorDefault, kCFNumberDoubleType, theValue));
		}
	else
		{
		return false;
		}
	return false;
	}

ZRef<CFTypeRef> sAsCFType(const ZAny& iAny)
	{
	ZRef<CFTypeRef> result;
	if (sQAsCFType(iAny, result))
		return result;

	return ZRef<CFTypeRef>();
	}

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
