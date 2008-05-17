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

#ifndef __ZUtil_NSObject__
#define __ZUtil_NSObject__ 1
#include "zconfig.h"
#include "ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "ZTuple.h"
#include "ZUnicode.h"

#include <Foundation/Foundation.h>

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_NSObject

namespace ZUtil_NSObject {

NSString* sCreateNSString_UTF8(const string8& iString8);
NSString* sCreateNSString_UTF16(const string16& iString16);

string8 sAsUTF8(NSString* iNSString);
string16 sAsUTF16(NSString* iNSString);

ZTValue sAsTV(id iNSObject);
id sCreateNSObject(const ZTValue& iTV);

ZTuple sAsTuple(NSDictionary* iNSDictionary);
NSDictionary* sCreateNSDictionary(const ZTuple& iTuple);

void sAsVector(NSArray* iNSArray, vector<ZTValue>& oVector);
NSArray* sCreateNSArray(const vector<ZTValue>& iVector);

} // namespace ZUtil_NSObject

#endif // ZCONFIG_SPI_Enabled(Cocoa)

#endif // __ZUtil_NSObject__
