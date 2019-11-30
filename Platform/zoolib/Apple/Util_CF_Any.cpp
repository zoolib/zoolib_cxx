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

#include "zoolib/Apple/Util_CF_Any.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/UTCDateTime.h"

#include "zoolib/Apple/Util_CF.h"

#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

//using std::pair;
//using std::vector;

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
#pragma mark - Util_CF

ZQ<Any> sQSimpleAsAny(CFTypeID iTypeID, CFTypeRef iVal)
	{
	if (iTypeID == ::CFBooleanGetTypeID())
		return Any(bool(::CFBooleanGetValue((CFBooleanRef)iVal)));

	if (iTypeID == ::CFNumberGetTypeID())
		{
		const CFNumberRef theNumberRef = (CFNumberRef)iVal;
		switch (::CFNumberGetType(theNumberRef))
			{
			case kCFNumberSInt8Type:
			case kCFNumberCharType:
				{
				int8 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt8Type, &theValue);
				return Any(theValue);
				}
			case kCFNumberSInt16Type:
			case kCFNumberShortType:
				{
				int16 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt16Type, &theValue);
				return Any(theValue);
				}
			case kCFNumberSInt32Type:
			case kCFNumberIntType:
				{
				int32 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt32Type, &theValue);
				return Any(theValue);
				}
			case kCFNumberSInt64Type:
			case kCFNumberLongLongType:
				{
				int64 theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberSInt64Type, &theValue);
				return Any(theValue);
				}
			case kCFNumberFloat32Type:
			case kCFNumberFloatType:
				{
				float theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat32Type, &theValue);
				return Any(theValue);
				}
			case kCFNumberFloat64Type:
			case kCFNumberDoubleType:
				{
				double theValue;
				::CFNumberGetValue(theNumberRef, kCFNumberFloat64Type, &theValue);
				return Any(theValue);
				}
			default:
				break;
			}
		}

	if (iTypeID == ::CFDateGetTypeID())
		{
		return Any(UTCDateTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime((CFDateRef)iVal)));
		}

	return null;
	}

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
