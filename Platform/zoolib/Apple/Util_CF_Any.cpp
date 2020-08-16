// Copyright (c) 2007 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Util_CF_Any.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/StdInt.h"
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
