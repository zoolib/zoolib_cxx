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

ZVal_Any sAsVal_Any(ZRef<CFTypeRef> iVal, const ZVal_Any& iDefault)
	{
	CFTypeRef theCFTypeRef = iVal;
	if (!theCFTypeRef)
		return ZVal_Any();

	const CFTypeID theTypeID = ::CFGetTypeID(theCFTypeRef);

	if (theTypeID == ::CFNullGetTypeID())
		return ZVal_Any();

	if (theTypeID == ::CFStringGetTypeID())
		return sAsUTF8(static_cast<CFStringRef>(theCFTypeRef));

	if (theTypeID == ::CFDictionaryGetTypeID())
		return sAsMap_Any(static_cast<CFDictionaryRef>(theCFTypeRef), iDefault);

	if (theTypeID == ::CFArrayGetTypeID())
		return sAsList_Any(static_cast<CFArrayRef>(theCFTypeRef), iDefault);

	if (theTypeID == ::CFBooleanGetTypeID())
		return bool(::CFBooleanGetValue(static_cast<CFBooleanRef>(theCFTypeRef)));
	
	if (theTypeID == ::CFDateGetTypeID())
		{
		return ZTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime(static_cast<CFDateRef>(theCFTypeRef)));
		}
	
	if (theTypeID == ::CFDataGetTypeID())
		return sAsData_Any(static_cast<CFDataRef>(theCFTypeRef));
	
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
				return theValue;
				}
			case kCFNumberSInt16Type:
			case kCFNumberShortType:
				{
				int16 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
				return theValue;
				}
			case kCFNumberSInt32Type:
			case kCFNumberIntType:
				{
				int32 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
				return theValue;
				}
			case kCFNumberSInt64Type:
			case kCFNumberLongLongType:
				{
				int64 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
				return theValue;
				}
			case kCFNumberFloat32Type:
			case kCFNumberFloatType:
				{
				float theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
				return theValue;
				}
			case kCFNumberFloat64Type:
			case kCFNumberDoubleType:
				{
				double theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
				return theValue;
				}
			}
		}

	return iDefault;
	}

ZVal_Any sAsVal_Any(ZRef<CFTypeRef> iVal)
	{ return sAsVal_Any(iVal, ZVal_Any()); }

ZData_Any sAsData_Any(const ZRef<CFDataRef>& iCFData)
	{
	ZData_Any theData;
	if (size_t theLength = ::CFDataGetLength(iCFData))
		return ZData_Any(::CFDataGetBytePtr(iCFData), theLength);
	return ZData_Any();
	}

ZList_Any sAsList_Any(const ZRef<CFArrayRef>& iCFArray, const ZVal_Any& iDefault)
	{
	ZList_Any theList;

	for (size_t x = 0, theCount = ::CFArrayGetCount(iCFArray); x < theCount; ++x)
		theList.Append(sAsVal_Any(::CFArrayGetValueAtIndex(iCFArray, x), iDefault));

	return theList;
	}

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	CFStringRef theKey = static_cast<CFStringRef>(iKey);
	CFTypeRef theValue = static_cast<CFTypeRef>(iValue);

	pair<ZMap_Any*, const ZVal_Any*>* thePair =
		static_cast<pair<ZMap_Any*, const ZVal_Any*>*>(iRefcon);

	thePair->first->Set(sAsUTF8(theKey), sAsVal_Any(theValue, *thePair->second));
	}

ZMap_Any sAsMap_Any(const ZRef<CFDictionaryRef>& iCFDictionary, const ZVal_Any& iDefault)
	{
	ZMap_Any theMap;
	pair<ZMap_Any*, const ZVal_Any*> thePair(&theMap, &iDefault);
	::CFDictionaryApplyFunction(iCFDictionary, spGatherContents, &thePair);
	return theMap;
	}

static ZRef<CFTypeRef> spMakeNumber(CFNumberType iType, const void* iVal)
	{ return Adopt_T<CFTypeRef>(::CFNumberCreate( kCFAllocatorDefault, iType, iVal)); }

ZRef<CFTypeRef> sAsCFType(const ZAny& iAny, const ZRef<CFTypeRef>& iDefault)
	{
	if (false)
		{}
	else if (iAny.type() == typeid(void))
		{
		return kCFNull; //??
		}
	else if (const string8* theValue = ZAnyCast<string8>(&iAny))
		{
		return sString(*theValue);
		}
	else if (const vector<char>* theValue = ZAnyCast<vector<char> >(&iAny))
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const ZData_Any* theValue = ZAnyCast<ZData_Any>(&iAny))
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetData(), theSize);
		else
			return sData();
		}
	else if (const vector<ZAny>* theValue = ZAnyCast<vector<ZAny> >(&iAny))
		{
		ZRef<CFMutableArrayRef> theArray;
		for (vector<ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			::CFArrayAppendValue(theArray, sAsCFType(*i, iDefault));
			}
		return theArray;
		}
	else if (const ZList_Any* theValue = ZAnyCast<ZList_Any>(&iAny))
		{
		ZRef<CFMutableArrayRef> theArray;
		for (size_t x = 0, count = theValue->Count(); x < count; ++x)
			{
			::CFArrayAppendValue(theArray, sAsCFType(theValue->Get(x), iDefault));
			}
		return theArray;
		}
	else if (const map<string, ZAny>* theValue = ZAnyCast<map<string, ZAny> >(&iAny))
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (map<string, ZAny>::const_iterator i = theValue->begin(), end = theValue->end();
			i != end; ++i)
			{
			::CFDictionarySetValue(theDictionary,
				sString((*i).first), sAsCFType((*i).second, iDefault));
			}
		return theDictionary;
		}
	else if (const ZMap_Any* theValue = ZAnyCast<ZMap_Any>(&iAny))
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (ZMap_Any::Index_t i = theValue->Begin(), end = theValue->End();
			i != end; ++i)
			{
			::CFDictionarySetValue(theDictionary,
				sString(theValue->NameOf(i)), sAsCFType(theValue->Get(i), iDefault));
			}
		return theDictionary;
		}
	else if (const bool* theValue = ZAnyCast<bool>(&iAny))
		{
		if (*theValue)
			return kCFBooleanTrue;
		else
			return kCFBooleanFalse;
		}
	else if (const ZTime* theValue = ZAnyCast<ZTime>(&iAny))
		{
		return Adopt_T<CFTypeRef>(::CFDateCreate(kCFAllocatorDefault,
			theValue->fVal - kCFAbsoluteTimeIntervalSince1970));
		}
	else if (const char* theValue = ZAnyCast<char>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const unsigned char* theValue = ZAnyCast<unsigned char>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const signed char* theValue = ZAnyCast<signed char>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const short* theValue = ZAnyCast<short>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const unsigned short* theValue = ZAnyCast<unsigned short>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const int* theValue = ZAnyCast<int>(&iAny))
		{
		if (ZIntIs32Bit)	
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned int* theValue = ZAnyCast<unsigned int>(&iAny))
		{
		if (ZIntIs32Bit)	
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const long* theValue = ZAnyCast<long>(&iAny))
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned long* theValue = ZAnyCast<unsigned long>(&iAny))
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const int64* theValue = ZAnyCast<int64>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const uint64* theValue = ZAnyCast<uint64>(&iAny))
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const float* theValue = ZAnyCast<float>(&iAny))
		{
		return spMakeNumber(kCFNumberFloatType, theValue);
		}
	else if (const double* theValue = ZAnyCast<double>(&iAny))
		{
		return spMakeNumber(kCFNumberDoubleType, theValue);
		}

	return iDefault;
	}

ZRef<CFTypeRef> sAsCFType(const ZAny& iAny)
	{ return sAsCFType(iAny, ZRef<CFTypeRef>()); }

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
