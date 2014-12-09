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

#ifndef __ZUtil_CF_h__
#define __ZUtil_CF_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/UnicodeString.h"

#include "zoolib/ZRef_CF.h"

namespace ZooLib {
namespace ZUtil_CF {

// =================================================================================================
// MARK: - ZUtil_CF

ZRef<CFStringRef> sString();
ZRef<CFStringRef> sString(const string8& iString8);
ZRef<CFStringRef> sString(const string16& iString16);

ZRef<CFMutableStringRef> sStringMutable();
ZRef<CFMutableStringRef> sStringMutable(const string8& iString8);
ZRef<CFMutableStringRef> sStringMutable(const string16& iString16);
ZRef<CFMutableStringRef> sStringMutable(CFStringRef iCFString);

// -----------------

ZRef<CFDictionaryRef> sDictionary();

ZRef<CFMutableDictionaryRef> sDictionaryMutable();
ZRef<CFMutableDictionaryRef> sDictionaryMutable(CFDictionaryRef iCFDictionary);

// -----------------

ZRef<CFArrayRef> sArray();

ZRef<CFMutableArrayRef> sArrayMutable();
ZRef<CFMutableArrayRef> sArrayMutable(CFArrayRef iCFArray);

// -----------------

ZRef<CFDataRef> sData();
ZRef<CFDataRef> sData(const void* iSource, size_t iSize);

ZRef<CFMutableDataRef> sDataMutable();
ZRef<CFMutableDataRef> sDataMutable(size_t iSize);
ZRef<CFMutableDataRef> sDataMutable(CFDataRef iCFData);

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

} // namespace ZUtil_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZUtil_CF_h__
