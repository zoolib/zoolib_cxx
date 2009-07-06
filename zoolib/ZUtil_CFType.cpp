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

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFBase.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
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
	{ return NoRetain(::CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, iCFDictionary)); }

ZRef<CFArrayRef> sArray()
	{ return NoRetain(::CFArrayCreate(kCFAllocatorDefault, nullptr, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable()
	{ return NoRetain(::CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable(const ZRef<CFArrayRef>& iCFArray)
	{ return NoRetain(::CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, iCFArray)); }

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
