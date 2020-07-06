// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Apple/Util_CF_ZZ.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/StdInt.h"
#include "zoolib/UTCDateTime.h"

#include "zoolib/Apple/Util_CF.h"

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
#pragma mark - Util_CF

static Data_ZZ spAsData_ZZ(CFDataRef iCFData)
	{
	if (size_t theLength = ::CFDataGetLength(iCFData))
		return Data_ZZ(::CFDataGetBytePtr(iCFData), theLength);
	return Data_ZZ();
	}

Seq_ZZ sAsSeq_ZZ(const Val_ZZ& iDefault, CFArrayRef iCFArray)
	{
	Seq_ZZ theSeq;

	for (size_t xx = 0, theCount = ::CFArrayGetCount(iCFArray); xx < theCount; ++xx)
		theSeq.Append(sDAsZZ(iDefault, ::CFArrayGetValueAtIndex(iCFArray, xx)).As<Val_ZZ>());

	return theSeq;
	}

static void spGatherContents(const void* iKey, const void* iValue, void* iRefcon)
	{
	CFStringRef theKey = static_cast<CFStringRef>(iKey);
	CFTypeRef theValue = static_cast<CFTypeRef>(iValue);

	pair<const Val_ZZ*,Map_ZZ*>* thePair =
		static_cast<pair<const Val_ZZ*,Map_ZZ*>*>(iRefcon);

	thePair->second->Set(sAsUTF8(theKey), sDAsZZ(*thePair->first, theValue).As<Val_ZZ>());
	}

Map_ZZ sAsMap_ZZ(const Val_ZZ& iDefault, CFDictionaryRef iCFDictionary)
	{
	Map_ZZ theMap;
	pair<const Val_ZZ*, Map_ZZ*> thePair(&iDefault, &theMap);
	::CFDictionaryApplyFunction(iCFDictionary, spGatherContents, &thePair);
	return theMap;
	}

ZQ<Val_ZZ> sQAsZZ(CFTypeRef iVal)
	{
	if (not iVal)
		return null;

	const CFTypeID theTypeID = ::CFGetTypeID(iVal);

	#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
		&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
		if (theTypeID == ::CFNullGetTypeID())
			return Val_ZZ();
	#endif

	if (theTypeID == ::CFStringGetTypeID())
		return Val_ZZ(sAsUTF8((CFStringRef)iVal));

	if (theTypeID == ::CFDictionaryGetTypeID())
		return Val_ZZ(sAsMap_ZZ(Val_ZZ(), (CFDictionaryRef)iVal));

	if (theTypeID == ::CFArrayGetTypeID())
		return Val_ZZ(sAsSeq_ZZ(Val_ZZ(), (CFArrayRef)iVal));

	if (theTypeID == ::CFDataGetTypeID())
		return Val_ZZ(spAsData_ZZ((CFDataRef)iVal));

	if (ZQ<Any> theQ = sQSimpleAsAny(theTypeID, iVal))
		return theQ->As<Val_ZZ>();

	return null;
	}

Val_ZZ sDAsZZ(const Val_ZZ& iDefault, CFTypeRef iVal)
	{
	if (ZQ<Val_ZZ> theQ = sQAsZZ(iVal))
		return *theQ;

	return iDefault;
	}

Val_ZZ sAsZZ(CFTypeRef iVal)
	{
	if (ZQ<Val_ZZ> theQ = sQAsZZ(iVal))
		return *theQ;
	return Val_ZZ();
	}

// =================================================================================================
#pragma mark - Util_CF

static ZP<CFTypeRef> spMakeNumber(CFNumberType iType, const void* iVal)
	{ return sAdopt& ::CFNumberCreate(nullptr, iType, iVal); }

ZP<CFTypeRef> sDAsCFType(CFTypeRef iDefault, const Val_ZZ& iVal)
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
	else if (const vector<char>* theValue = iVal.PGet<vector<char>>())
		{
		if (size_t theSize = theValue->size())
			return sData(&(*theValue)[0], theSize);
		else
			return sData();
		}
	else if (const Data_ZZ* theValue = iVal.PGet<Data_ZZ>())
		{
		if (size_t theSize = theValue->GetSize())
			return sData(theValue->GetPtr(), theSize);
		else
			return sData();
		}
	else if (const Seq_ZZ* theValue = iVal.PGet<Seq_ZZ>())
		{
		ZP<CFMutableArrayRef> theArray;
		for (size_t xx = 0, count = theValue->Count(); xx < count; ++xx)
			::CFArrayAppendValue(theArray, sDAsCFType(iDefault, theValue->Get(xx).As<Val_ZZ>()));
		return theArray;
		}
	else if (const Map_ZZ* theValue = iVal.PGet<Map_ZZ>())
		{
		ZP<CFMutableDictionaryRef> theDictionary = sDictionaryMutable();
		for (Map_ZZ::Index_t ii = theValue->Begin(), end = theValue->End();
			ii != end; ++ii)
			{
			::CFDictionarySetValue(theDictionary,
				sString(theValue->NameOf(ii)), sDAsCFType(iDefault, theValue->Get(ii).As<Val_ZZ>()));
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
	else if (const UTCDateTime* theValue = iVal.PGet<UTCDateTime>())
		{
		return sAdopt& ::CFDateCreate(nullptr, sGet(*theValue) - kCFAbsoluteTimeIntervalSince1970);
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

ZP<CFTypeRef> sAsCFType(const Val_ZZ& iVal)
	{ return sDAsCFType(nullptr, iVal); }

} // namespace Util_CF
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
