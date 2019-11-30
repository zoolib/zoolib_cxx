/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Apple_Util_NS_Any_h__
#define __ZooLib_Apple_Util_NS_Any_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(CocoaFoundation)

#include "zoolib/Val_ZZ.h"

#import <Foundation/NSObject.h>

namespace ZooLib {
namespace Util_NS {

// =================================================================================================
#pragma mark - Util_NS

Val_ZZ sDAsZZ(const Val_ZZ& iDefault, NSObject* iVal);
Val_ZZ sAsZZ(NSObject* iVal);

NSObject* sDAsNSObject(NSObject* iDefault, const Val_ZZ& iVal);
NSObject* sAsNSObject(const Val_ZZ& iVal);

} // namespace Util_NS
} // namespace ZooLib

// =================================================================================================
#pragma mark - asAnyWithDefault

@interface NSObject (ZooLib_Any_Additions)
-(ZooLib::Val_ZZ)asZZWithDefault:(const ZooLib::Val_ZZ&)iDefault;
@end

#endif // ZCONFIG_SPI_Enabled(CocoaFoundation)

#endif // __ZooLib_Apple_Util_NS_Any_h__
