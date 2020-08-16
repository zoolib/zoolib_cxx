// Copyright (c) 2007 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Util_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF

static CFStringRef spEmptyCFString = CFSTR("");

ZP<CFStringRef> sString()
	{ return spEmptyCFString; }

ZP<CFStringRef> sString(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return sAdopt& ::CFStringCreateWithBytes(nullptr,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false);
		}

	return spEmptyCFString;
	}

ZP<CFStringRef> sString(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return sAdopt& ::CFStringCreateWithCharacters(nullptr,
			reinterpret_cast<const UniChar*>(iString16.data()), sourceSize);
		}

	return spEmptyCFString;
	}

ZP<CFMutableStringRef> sStringMutable()
	{ return sAdopt& ::CFStringCreateMutable(nullptr, 0); }

ZP<CFMutableStringRef> sStringMutable(const string8& iString8)
	{ return sStringMutable(sString(iString8)); }

ZP<CFMutableStringRef> sStringMutable(const string16& iString16)
	{ return sStringMutable(sString(iString16)); }

ZP<CFMutableStringRef> sStringMutable(CFStringRef iCFString)
	{ return sAdopt& ::CFStringCreateMutableCopy(nullptr, 0, iCFString); }

ZP<CFDictionaryRef> sDictionary()
	{
	return sAdopt& ::CFDictionaryCreate(nullptr, nullptr, nullptr, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	}

ZP<CFMutableDictionaryRef> sDictionaryMutable()
	{
	return sAdopt& ::CFDictionaryCreateMutable(nullptr, 0,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	}

ZP<CFMutableDictionaryRef> sDictionaryMutable(CFDictionaryRef iCFDictionary)
	{
	return sAdopt& ::CFDictionaryCreateMutableCopy(nullptr,
		::CFDictionaryGetCount(iCFDictionary), iCFDictionary);
	}

ZP<CFArrayRef> sArray()
	{ return sAdopt& ::CFArrayCreate(nullptr, nullptr, 0, &kCFTypeArrayCallBacks); }

ZP<CFMutableArrayRef> sArrayMutable()
	{ return sAdopt& ::CFArrayCreateMutable(nullptr, 0, &kCFTypeArrayCallBacks); }

ZP<CFMutableArrayRef> sArrayMutable(CFArrayRef iCFArray)
	{ return sAdopt& ::CFArrayCreateMutableCopy(nullptr, ::CFArrayGetCount(iCFArray), iCFArray); }

ZP<CFDataRef> sData()
	{ return sAdopt& ::CFDataCreate(nullptr, 0, 0); }

ZP<CFDataRef> sData(const void* iSource, size_t iSize)
	{ return sAdopt& ::CFDataCreate(nullptr, static_cast<const UInt8*>(iSource), iSize); }

ZP<CFMutableDataRef> sDataMutable()
	{ return sAdopt& ::CFDataCreateMutable(nullptr, 0); }

ZP<CFMutableDataRef> sDataMutable(size_t iSize)
	{
	ZP<CFMutableDataRef> theData = sAdopt& ::CFDataCreateMutable(nullptr, 0);
	::CFDataSetLength(theData, iSize);
	return theData;
	}

ZP<CFMutableDataRef> sDataMutable(CFDataRef iCFData)
	{ return sAdopt& ::CFDataCreateMutableCopy(nullptr, 0, iCFData); }

// =================================================================================================
#pragma mark - Util_CF

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

// =================================================================================================
#pragma mark - Util_CF

CFTypeRef sIfIs(CFTypeID iTypeID, CFTypeRef iTypeRef)
	{
	if (iTypeRef && iTypeID == ::CFGetTypeID(iTypeRef))
		return iTypeRef;
	return nullptr;
	}

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
