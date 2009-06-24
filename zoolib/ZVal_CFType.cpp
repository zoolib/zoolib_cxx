/* -------------------------------------------------------------------------------------------------
Copyright (c) 2009 Andrew Green
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

#include "zoolib/ZVal_CFType.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZDebug.h"

#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace ZANONYMOUS {

CFStringRef sEmptyCFString = CFSTR("");

ZRef<CFStringRef> sCFString(const string8& iString8)
	{
	if (CFIndex sourceSize = iString8.size())
		{
		return NoRetain(::CFStringCreateWithBytes(nullptr,
			reinterpret_cast<const UInt8*>(iString8.data()), sourceSize,
			kCFStringEncodingUTF8, false));
		}
	return sEmptyCFString;
	}

ZRef<CFStringRef> sCFString(const string16& iString16)
	{
	if (CFIndex sourceSize = iString16.size())
		{
		return NoRetain(::CFStringCreateWithCharacters(nullptr,
			reinterpret_cast<const UniChar*>(iString16.data()), sourceSize));
		}
	return sEmptyCFString;
	}

ZRef<CFMutableArrayRef> sArrayMutable()
	{ return NoRetain(::CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks)); }

ZRef<CFMutableArrayRef> sArrayMutable(const ZRef<CFArrayRef>& iCFArray)
	{ return NoRetain(::CFArrayCreateMutableCopy(kCFAllocatorDefault, 0, iCFArray)); }

ZRef<CFMutableDictionaryRef> sDictionaryMutable()
	{
	return NoRetain(::CFDictionaryCreateMutable(kCFAllocatorDefault, 1,
		&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks));
	}

ZRef<CFMutableDictionaryRef> sDictionaryMutable(const ZRef<CFDictionaryRef>& iCFDictionary)
	{ return NoRetain(::CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, iCFDictionary)); }

template <class S>
bool sGetNumber_T(CFTypeRef iTypeRef, CFNumberType iNumberType, S& oVal)
	{
	if (::CFGetTypeID(iTypeRef) == ::CFNumberGetTypeID())
		{
		CFNumberRef theNumberRef = static_cast<CFNumberRef>(iTypeRef);
		if (::CFNumberGetType(theNumberRef) == iNumberType)
			{
			::CFNumberGetValue(theNumberRef, iNumberType, &oVal);
			return true;
			}
		}
	return false;
	}

template <class S>
ZRef<CFTypeRef> sNumber_T(CFNumberType iNumberType, const S& iVal)
	{ return NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, iNumberType, &iVal))); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

ZVal_CFType::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fCFTypeRef); }

ZVal_CFType::ZVal_CFType()
	{}

ZVal_CFType::ZVal_CFType(const ZVal_CFType& iOther)
:	fCFTypeRef(iOther.fCFTypeRef)
	{}

ZVal_CFType::~ZVal_CFType()
	{}

ZVal_CFType& ZVal_CFType::operator=(const ZVal_CFType& iOther)
	{
	fCFTypeRef = iOther.fCFTypeRef;
	return *this;
	}

ZVal_CFType::ZVal_CFType(ZRef<CFTypeRef> iCFTypeRef)
:	fCFTypeRef(iCFTypeRef)
	{}

ZVal_CFType::ZVal_CFType(int8 iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt8Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int16 iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt16Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int32 iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int64 iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(bool iVal)
:	fCFTypeRef(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

ZVal_CFType::ZVal_CFType(float iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(double iVal)
:	fCFTypeRef(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(const string8& iVal)
:	fCFTypeRef(sCFString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const string16& iVal)
:	fCFTypeRef(sCFString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const ZValList_CFType& iVal)
:	fCFTypeRef(iVal.GetCFArray())
	{}

ZVal_CFType::ZVal_CFType(const ZValMap_CFType& iVal)
:	fCFTypeRef(iVal.GetCFDictionary())
	{}

ZVal_CFType::operator CFTypeRef() const
	{ return fCFTypeRef; }

CFTypeRef* ZVal_CFType::ParamO()
	{
	fCFTypeRef.Clear();
	return &fCFTypeRef.GetPtrRef();
	}

template <>
bool ZVal_CFType::QGet_T<int8>(int8& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberSInt8Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberCharType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int16>(int16& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberSInt16Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberShortType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int32>(int32& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberSInt32Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberIntType, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberLongType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int64>(int64& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberSInt64Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberLongLongType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<bool>(bool& oVal) const
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFBooleanGetTypeID())
		{
		oVal = ::CFBooleanGetValue(static_cast<CFBooleanRef>(CFTypeRef(fCFTypeRef)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<float>(float& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberFloat32Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberFloatType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<double>(double& oVal) const
	{
	if (sGetNumber_T(fCFTypeRef, kCFNumberFloat64Type, oVal))
		return true;
	if (sGetNumber_T(fCFTypeRef, kCFNumberDoubleType, oVal))
		return true;
	return false;
	}

template <>
void ZVal_CFType::Set_T<int8>(const int8& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberSInt8Type, iVal); }

template <>
void ZVal_CFType::Set_T<int16>(const int16& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberSInt16Type, iVal); }

template <>
void ZVal_CFType::Set_T<int32>(const int32& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberSInt32Type, iVal); }

template <>
void ZVal_CFType::Set_T<int64>(const int64& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberSInt64Type, iVal); }

template <>
void ZVal_CFType::Set_T<bool>(const bool& iVal)
	{ fCFTypeRef = iVal ? kCFBooleanTrue : kCFBooleanFalse; }

template <>
void ZVal_CFType::Set_T<float>(const float& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberFloat32Type, iVal); }

template <>
void ZVal_CFType::Set_T<double>(const double& iVal)
	{ fCFTypeRef = sNumber_T(kCFNumberFloat64Type, iVal); }

ZMACRO_ZValAccessors_Def_Std(ZVal_CFType)

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_CFType

ZValList_CFType::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

ZValList_CFType::ZValList_CFType()
:	fCFMutableArrayRef(sArrayMutable())
	{}

ZValList_CFType::ZValList_CFType(const ZValList_CFType& iOther)
:	fCFMutableArrayRef(iOther.fCFMutableArrayRef)
,	fCFArrayRef(iOther.fCFArrayRef)
	{}

ZValList_CFType::~ZValList_CFType()
	{}

ZValList_CFType& ZValList_CFType::operator=(const ZValList_CFType& iOther)
	{
	fCFMutableArrayRef = iOther.fCFMutableArrayRef;
	fCFArrayRef = iOther.fCFArrayRef;
	ZAssert(!fCFMutableArrayRef || !fCFArrayRef);
	return *this;
	}

ZValList_CFType::ZValList_CFType(const ZRef<CFMutableArrayRef>& iOther)
:	fCFMutableArrayRef(iOther)
	{}

ZValList_CFType::ZValList_CFType(const ZRef<CFArrayRef>& iOther)
:	fCFArrayRef(iOther)
	{}

ZRef<CFArrayRef> ZValList_CFType::GetCFArray() const
	{
	if (fCFMutableArrayRef)
		return fCFMutableArrayRef;
	return fCFArrayRef;
	}

size_t ZValList_CFType::Count() const
	{
	if (fCFMutableArrayRef)
		return ::CFArrayGetCount(fCFMutableArrayRef);
	return ::CFArrayGetCount(fCFArrayRef);
	}

void ZValList_CFType::Clear()
	{
	fCFMutableArrayRef = sArrayMutable();
	fCFArrayRef.Clear();
	}

bool ZValList_CFType::QGet(size_t iIndex, ZVal_CFType& oVal) const
	{
	if (fCFMutableArrayRef)
		{
		if (size_t theCount = ::CFArrayGetCount(fCFMutableArrayRef))
			{
			if (iIndex < theCount)
				{
				oVal = ::CFArrayGetValueAtIndex(fCFMutableArrayRef, iIndex);
				return true;
				}
			}
		}
	else
		{
		if (size_t theCount = ::CFArrayGetCount(fCFArrayRef))
			{
			if (iIndex < theCount)
				{
				oVal = ::CFArrayGetValueAtIndex(fCFArrayRef, iIndex);
				return true;
				}
			}
		}
	return false;
	}

void ZValList_CFType::Set(size_t iIndex, const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFArraySetValueAtIndex(fCFMutableArrayRef, iIndex, iVal);
	}

void ZValList_CFType::Erase(size_t iIndex)
	{
	this->pTouch();
	if (size_t theCount = ::CFArrayGetCount(fCFMutableArrayRef))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(fCFMutableArrayRef, iIndex);
		}
	}

void ZValList_CFType::Insert(size_t iIndex, const ZVal_CFType& iVal)
	{
	this->pTouch();
	size_t theCount = ::CFArrayGetCount(fCFMutableArrayRef);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(fCFMutableArrayRef, iIndex, iVal);
	}

void ZValList_CFType::Append(const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFArrayAppendValue(fCFMutableArrayRef, iVal);
	}

void ZValList_CFType::pTouch()
	{
	if (!fCFMutableArrayRef || ::CFGetRetainCount(fCFMutableArrayRef) > 1)
		{
		if (fCFMutableArrayRef)
			{
			fCFMutableArrayRef = sArrayMutable(fCFMutableArrayRef);
			}
		else
			{
			ZAssert(fCFArrayRef);
			fCFMutableArrayRef = sArrayMutable(fCFArrayRef);
			fCFArrayRef.Clear();
			}
		}
	ZAssert(!fCFArrayRef);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_CFType

ZValMap_CFType::operator operator_bool_type() const
	{
	if (fCFMutableDictionaryRef)
		{
		return operator_bool_generator_type::translate(
			::CFDictionaryGetCount(fCFMutableDictionaryRef));
		}
	return operator_bool_generator_type::translate(::CFDictionaryGetCount(fCFDictionaryRef));
	}

ZValMap_CFType::ZValMap_CFType()
:	fCFMutableDictionaryRef(sDictionaryMutable())
	{}

ZValMap_CFType::ZValMap_CFType(const ZValMap_CFType& iOther)
:	fCFMutableDictionaryRef(iOther.fCFMutableDictionaryRef)
,	fCFDictionaryRef(iOther.fCFDictionaryRef)
	{}

ZValMap_CFType::~ZValMap_CFType()
	{}

ZValMap_CFType& ZValMap_CFType::operator=(const ZValMap_CFType& iOther)
	{
	fCFMutableDictionaryRef = iOther.fCFMutableDictionaryRef;
	fCFDictionaryRef = iOther.fCFDictionaryRef;
	ZAssert(!fCFMutableDictionaryRef || !fCFDictionaryRef);
	return *this;
	}

ZValMap_CFType::ZValMap_CFType(const ZRef<CFDictionaryRef>& iOther)
:	fCFDictionaryRef(iOther)
	{}

ZValMap_CFType::ZValMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther)
:	fCFMutableDictionaryRef(iOther)
	{}

ZRef<CFDictionaryRef> ZValMap_CFType::GetCFDictionary() const
	{
	if (fCFMutableDictionaryRef)
		return fCFMutableDictionaryRef;
	return fCFDictionaryRef;
	}

void ZValMap_CFType::Clear()
	{
	fCFMutableDictionaryRef = sDictionaryMutable();
	fCFDictionaryRef.Clear();
	}

bool ZValMap_CFType::QGet(const string8& iName, ZVal_CFType& oVal) const
	{
	ZRef<CFStringRef> theName = sCFString(iName);
	if (fCFMutableDictionaryRef)
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFMutableDictionaryRef, theName))
			{
			oVal = theVal;
			return true;
			}
		}
	else
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFDictionaryRef, theName))
			{
			oVal = theVal;
			return true;
			}
		}
	return false;
	}

bool ZValMap_CFType::QGet(CFStringRef iName, ZVal_CFType& oVal) const
	{
	if (fCFMutableDictionaryRef)
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFMutableDictionaryRef, iName))
			{
			oVal = theVal;
			return true;
			}
		}
	else
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFDictionaryRef, iName))
			{
			oVal = theVal;
			return true;
			}
		}

	return false;
	}

void ZValMap_CFType::Set(const string8& iName, const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFDictionarySetValue(fCFMutableDictionaryRef, sCFString(iName), iVal);
	}

void ZValMap_CFType::Set(CFStringRef iName, const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFDictionarySetValue(fCFMutableDictionaryRef, iName, iVal);
	}

void ZValMap_CFType::Erase(const string8& iName)
	{
	this->pTouch();
	::CFDictionaryRemoveValue(fCFMutableDictionaryRef, sCFString(iName));
	}

void ZValMap_CFType::Erase(CFStringRef iName)
	{
	this->pTouch();
	::CFDictionaryRemoveValue(fCFMutableDictionaryRef, iName);
	}

void ZValMap_CFType::pTouch()
	{
	if (!fCFMutableDictionaryRef || ::CFGetRetainCount(fCFMutableDictionaryRef) > 1)
		{
		if (fCFMutableDictionaryRef)
			{
			fCFMutableDictionaryRef = sDictionaryMutable(fCFMutableDictionaryRef);
			}
		else
			{
			ZAssert(fCFDictionaryRef);
			fCFMutableDictionaryRef = sDictionaryMutable(fCFDictionaryRef);
			fCFDictionaryRef.Clear();
			}
		}
	ZAssert(!fCFDictionaryRef);
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
