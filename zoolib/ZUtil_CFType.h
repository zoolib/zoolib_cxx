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

#ifndef __ZUtil_CFType__
#define __ZUtil_CFType__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZTuple.h"
#include "zoolib/ZUnicode.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFBase.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CFType

namespace ZUtil_CFType {

CFStringRef sCreateCFString_UTF8(const string8& iString8);
CFStringRef sCreateCFString_UTF16(const string16& iString16);

string8 sAsUTF8(const CFStringRef& iCFString);
string16 sAsUTF16(const CFStringRef& iCFString);

ZType sTypeOf(CFTypeRef iCFType);

ZTValue sAsTV(CFTypeRef iCFType);
CFTypeRef sCreateCFType(const ZTValue& iTV);

ZTuple sAsTuple(CFDictionaryRef iCFDictionary);
CFDictionaryRef sCreateCFDictionary(const ZTuple& iTuple);

void sAsVector(CFArrayRef iCFArray, std::vector<ZTValue>& oVector);
CFArrayRef sCreateCFArray(const std::vector<ZTValue>& iVector);

} // namespace ZUtil_CFType

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)

#endif // __ZUtil_CFType__
