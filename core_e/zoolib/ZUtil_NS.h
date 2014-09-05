/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#ifndef __ZUtil_NS_h__
#define __ZUtil_NS_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

// Urg. We need to include this before any std C++ headers when building
// with Cocotron, otherwise va_list isn't appropriately visible to ObjC code.
#include <Foundation/Foundation.h>

#include "zoolib/ZUnicodeString.h"

// =================================================================================================
// MARK: - ZUtil_NS

namespace ZooLib {
namespace ZUtil_NS {

NSString* sString();
NSString* sString(const string8& iString8);
NSString* sString(const string16& iString16);

NSMutableString* sStringMutable();
NSMutableString* sStringMutable(const string8& iString8);
NSMutableString* sStringMutable(const string16& iString16);
NSMutableString* sStringMutable(NSString* iNSString);

// -----------------

NSDictionary* sDictionary();

NSMutableDictionary* sDictionaryMutable();
NSMutableDictionary* sDictionaryMutable(NSDictionary* iNSDictionary);

// -----------------

NSArray* sArray();

NSMutableArray* sArrayMutable();
NSMutableArray* sArrayMutable(NSArray* iNSArray);

// -----------------

NSData* sData();
NSData* sData(const void* iSource, size_t iSize);

NSMutableData* sDataMutable();
NSMutableData* sDataMutable(size_t iSize);
NSMutableData* sDataMutable(const void* iSource, size_t iSize);
NSMutableData* sDataMutable(NSData* iNSData);

// -----------------

string8 sAsUTF8(NSString* iNSString);
string16 sAsUTF16(NSString* iNSString);

} // namespace ZUtil_NS
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZUtil_NS_h__
