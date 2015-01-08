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

#include "zoolib/Apple/Util_CF.h"

#include "zoolib/ZTime.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFData.h)
#include ZMACINCLUDE2(CoreFoundation,CFDate.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

using std::pair;
using std::vector;

namespace ZooLib {
namespace Util_CF {

// =================================================================================================
// MARK: - Util_CF

static Data_Any spAsData_Any(CFDataRef iCFData)
	{
	if (size_t theLength = ::CFDataGetLength(iCFData))
		return Data_Any(::CFDataGetBytePtr(iCFData), theLength);
	return Data_Any();
	}

Seq_Any sAsSeq_Any(const Any& iDefault, CFArrayRef iCFArray)
	{
	Seq_Any theSeq;

	for (size_t xx = 0, theCount = ::CFArrayGetCount(iCFArray); xx < theCount; ++xx)
		theSeq.Append(sDAsAny(iDefault, ::CFArrayGetValueAtIndex(iCFArray, xx)));

	return theSeq;
	}

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	CFStringRef theKey = static_cast<CFStringRef>(iKey);
	CFTypeRef theValue = static_cast<CFTypeRef>(iValue);

	pair<const Any*,Map_Any*>* thePair =
		static_cast<pair<const Any*,Map_Any*>*>(iRefcon);

	thePair->second->Set(sAsUTF8(theKey), sDAsAny(*thePair->first, theValue));
	}

Map_Any sAsMap_Any(const Any& iDefault, CFDictionaryRef iCFDictionary)
	{
	Map_Any theMap;
	pair<const Any*, Map_Any*> thePair(&iDefault, &theMap);
	::CFDictionaryApplyFunction(iCFDictionary, spGatherContents, &thePair);
	return theMap;
	}

Any sDAsAny(const Any& iDefault, CFTypeRef iVal)
	{
	if (not iVal)
		return iDefault;

	const CFTypeID theTypeID = ::CFGetTypeID(iVal);

	#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
		&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
		if (theTypeID == ::CFNullGetTypeID())
			return Any();
	#endif

	if (theTypeID == ::CFStringGetTypeID())
		return Any(sAsUTF8((CFStringRef)iVal));

	if (theTypeID == ::CFDictionaryGetTypeID())
		return Any(sAsMap_Any(iDefault, (CFDictionaryRef)iVal));

	if (theTypeID == ::CFArrayGetTypeID())
		return Any(sAsSeq_Any(iDefault, (CFArrayRef)iVal));

	if (theTypeID == ::CFBooleanGetTypeID())
		return Any(bool(::CFBooleanGetValue((CFBooleanRef)iVal)));

	if (theTypeID == ::CFDateGetTypeID())
		{
		return Any(ZTime(kCFAbsoluteTimeIntervalSince1970
			+ ::CFDateGetAbsoluteTime((CFDateRef)iVal)));
		}

	if (theTypeID == ::CFDataGetTypeID())
		return Any(spAsData_Any((CFDataRef)iVal));

	if (theTypeID == ::CFNumberGetTypeID())
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

	return iDefault;
	}

Any sAsAny(CFTypeRef iVal)
	{ return sDAsAny(Any(), iVal); }

static ZRef<CFTypeRef> spMakeNumber(CFNumberType iType, const void* iVal)
	{ return sAdopt& ::CFNumberCreate(nullptr, iType, iVal); }

ZRef<CFTypeRef> sDAsCFType(CFTypeRef iDefault, const Any& iVal)
	{
	if (false)
		{}
	else if (iVal.IsNull())
		{
		#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
			&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
			return kCFNull;
		#else
			return null;
		#endif
		}
	else if (const string8* theValue = iVal.PGet<string8>())
		{
		return sString(*theValue);
		}
	else if (const vector<char>* theValue = iVal.PGet<vector<char> >())
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const Data_Any* theValue = iVal.PGet<Data_Any>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetPtr(), theSize);
		else
			return sData();
		}
	else if (const Seq_Any* theValue = iVal.PGet<Seq_Any>())
		{
		ZRef<CFMutableArrayRef> theArray;
		for (size_t xx = 0, count = theValue->Count(); xx < count; ++xx)
			::CFArrayAppendValue(theArray, sDAsCFType(iDefault, theValue->Get(xx)));
		return theArray;
		}
	else if (const Map_Any* theValue = iVal.PGet<Map_Any>())
		{
		ZRef<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (Map_Any::Index_t ii = theValue->Begin(), end = theValue->End();
			ii != end; ++ii)
			{
			::CFDictionarySetValue(theDictionary,
				sString(theValue->NameOf(ii)), sDAsCFType(iDefault, theValue->Get(ii)));
			}
		return theDictionary;
		}
	else if (const bool* theValue = iVal.PGet<bool>())
		{
		if (*theValue)
			return kCFBooleanTrue;
		else
			return kCFBooleanFalse;
		}
	else if (const ZTime* theValue = iVal.PGet<ZTime>())
		{
		return sAdopt& ::CFDateCreate(nullptr, theValue->fVal - kCFAbsoluteTimeIntervalSince1970);
		}
	else if (const char* theValue = iVal.PGet<char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const unsigned char* theValue = iVal.PGet<unsigned char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const signed char* theValue = iVal.PGet<signed char>())
		{
		return spMakeNumber(kCFNumberSInt8Type, theValue);
		}
	else if (const short* theValue = iVal.PGet<short>())
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const unsigned short* theValue = iVal.PGet<unsigned short>())
		{
		return spMakeNumber(kCFNumberSInt16Type, theValue);
		}
	else if (const int* theValue = iVal.PGet<int>())
		{
		if (ZIntIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned int* theValue = iVal.PGet<unsigned int>())
		{
		if (ZIntIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const long* theValue = iVal.PGet<long>())
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const unsigned long* theValue = iVal.PGet<unsigned long>())
		{
		if (ZLongIs32Bit)
			return spMakeNumber(kCFNumberSInt32Type, theValue);
		else
			return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const int64* theValue = iVal.PGet<int64>())
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const uint64* theValue = iVal.PGet<uint64>())
		{
		return spMakeNumber(kCFNumberSInt64Type, theValue);
		}
	else if (const float* theValue = iVal.PGet<float>())
		{
		return spMakeNumber(kCFNumberFloatType, theValue);
		}
	else if (const double* theValue = iVal.PGet<double>())
		{
		return spMakeNumber(kCFNumberDoubleType, theValue);
		}

	return iDefault;
	}

ZRef<CFTypeRef> sAsCFType(const Any& iVal)
	{ return sDAsCFType(nullptr, iVal); }

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
