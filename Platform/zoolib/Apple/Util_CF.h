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

#ifndef __ZooLib_Apple_Util_CF_h__
#define __ZooLib_Apple_Util_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/UnicodeString.h"

#include "zoolib/Apple/ZRef_CF.h"

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF

ZP<CFStringRef> sString();
ZP<CFStringRef> sString(const string8& iString8);
ZP<CFStringRef> sString(const string16& iString16);

ZP<CFMutableStringRef> sStringMutable();
ZP<CFMutableStringRef> sStringMutable(const string8& iString8);
ZP<CFMutableStringRef> sStringMutable(const string16& iString16);
ZP<CFMutableStringRef> sStringMutable(CFStringRef iCFString);

// -----------------

ZP<CFDictionaryRef> sDictionary();

ZP<CFMutableDictionaryRef> sDictionaryMutable();
ZP<CFMutableDictionaryRef> sDictionaryMutable(CFDictionaryRef iCFDictionary);

// -----------------

ZP<CFArrayRef> sArray();

ZP<CFMutableArrayRef> sArrayMutable();
ZP<CFMutableArrayRef> sArrayMutable(CFArrayRef iCFArray);

// -----------------

ZP<CFDataRef> sData();
ZP<CFDataRef> sData(const void* iSource, size_t iSize);

ZP<CFMutableDataRef> sDataMutable();
ZP<CFMutableDataRef> sDataMutable(size_t iSize);
ZP<CFMutableDataRef> sDataMutable(CFDataRef iCFData);

// -----------------

string8 sAsUTF8(CFStringRef iCFString);
string16 sAsUTF16(CFStringRef iCFString);

// -----------------

CFTypeRef sIfIs(CFTypeID iTypeID, CFTypeRef iTypeRef);

template <typename Ref_p>
Ref_p sStaticCast(CFTypeRef iTypeRef)
	{ return (Ref_p)(iTypeRef); }

template <typename Ref_p>
Ref_p sStaticCastIf(CFTypeID iTypeID, CFTypeRef iTypeRef)
	{ return (Ref_p)(sIfIs(iTypeID, iTypeRef)); }

// This generally can't work as CFTypeIDs are often/usually allocated at runtime.
//template <typename Ref_p, CFTypeID TypeID_p>
//Ref_p sDynamicCast(CFTypeRef iTypeRef)
//	{ return (Ref_p)(sIfIs(TypeID_p, iTypeRef)); }

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZooLib_Apple_Util_CF_h__
