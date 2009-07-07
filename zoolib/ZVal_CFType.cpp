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

#include "zoolib/ZUtil_CFType.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

NAMESPACE_ZOOLIB_BEGIN

using ZUtil_CFType::sAsUTF8;
using ZUtil_CFType::sString;
using ZUtil_CFType::sStringMutable;
using ZUtil_CFType::sArrayMutable;
using ZUtil_CFType::sDictionaryMutable;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

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

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

ZVal_CFType::operator bool() const
	{ return fCFTypeRef; }

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
:	fCFTypeRef(sString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const string16& iVal)
:	fCFTypeRef(sString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const ZValList_CFType& iVal)
:	fCFTypeRef(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZValMap_CFType& iVal)
:	fCFTypeRef(iVal)
	{}

void ZVal_CFType::Clear()
	{ fCFTypeRef.Clear(); }

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
bool ZVal_CFType::QGet_T<string8>(string8& oVal) const
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFStringGetTypeID())
		{
		oVal = sAsUTF8(static_cast<CFStringRef>(CFTypeRef(fCFTypeRef)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValData_CFType>(ZValData_CFType& oVal) const
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFDataGetTypeID())
		{
		oVal = (static_cast<CFDataRef>(CFTypeRef(fCFTypeRef)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValList_CFType>(ZValList_CFType& oVal) const
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFArrayGetTypeID())
		{
		oVal = (static_cast<CFArrayRef>(CFTypeRef(fCFTypeRef)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValMap_CFType>(ZValMap_CFType& oVal) const
	{
	if (::CFGetTypeID(fCFTypeRef) == ::CFDictionaryGetTypeID())
		{
		oVal = (static_cast<CFDictionaryRef>(CFTypeRef(fCFTypeRef)));
		return true;
		}
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

template <>
void ZVal_CFType::Set_T<string8>(const string8& iVal)
	{ fCFTypeRef = sString(iVal); }

template <>
void ZVal_CFType::Set_T<ZValData_CFType>(const ZValData_CFType& iVal)
	{ fCFTypeRef = iVal; }

template <>
void ZVal_CFType::Set_T<ZValList_CFType>(const ZValList_CFType& iVal)
	{ fCFTypeRef = iVal; }

template <>
void ZVal_CFType::Set_T<ZValMap_CFType>(const ZValMap_CFType& iVal)
	{ fCFTypeRef = iVal; }

CFTypeRef& ZVal_CFType::OParam()
	{
	fCFTypeRef.Clear();
	return fCFTypeRef.GetPtrRef();
	}

ZVal_CFType::operator CFTypeRef() const
	{ return fCFTypeRef; }

ZMACRO_ZValAccessors_Def_Std(ZVal_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Data, ZValData_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, List, ZValList_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Map, ZValMap_CFType)

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_CFType

ZValList_CFType::operator bool() const
	{ return this->Count(); }

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

ZValList_CFType& ZValList_CFType::operator=(const ZRef<CFMutableArrayRef>& iOther)
	{
	fCFMutableArrayRef = iOther;
	fCFArrayRef.Clear();
	return *this;
	}

ZValList_CFType& ZValList_CFType::operator=(const ZRef<CFArrayRef>& iOther)
	{
	fCFMutableArrayRef.Clear();
	fCFArrayRef = iOther;
	return *this;
	}

size_t ZValList_CFType::Count() const
	{
	if (fCFMutableArrayRef)
		return ::CFArrayGetCount(fCFMutableArrayRef);
	else if (fCFArrayRef)
		return ::CFArrayGetCount(fCFArrayRef);
	else
		return 0;
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
	else if (fCFArrayRef)
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

ZVal_CFType ZValList_CFType::DGet(size_t iIndex, const ZVal_CFType& iDefault) const
	{
	ZVal_CFType result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

ZVal_CFType ZValList_CFType::Get(size_t iIndex) const
	{ return this->DGet(iIndex, ZVal_CFType()); }

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

CFArrayRef& ZValList_CFType::OParam()
	{
	fCFMutableArrayRef.Clear();
	fCFArrayRef.Clear();
	return fCFArrayRef.GetPtrRef();
	}

ZValList_CFType::operator CFTypeRef() const
	{
	if (fCFMutableArrayRef)
		return fCFMutableArrayRef;
	return fCFArrayRef;
	}

ZValList_CFType::operator CFArrayRef() const
	{
	if (fCFMutableArrayRef)
		return fCFMutableArrayRef;
	return fCFArrayRef;
	}

void ZValList_CFType::pTouch()
	{
	if (!fCFMutableArrayRef || ::CFGetRetainCount(fCFMutableArrayRef) > 1)
		{
		if (fCFMutableArrayRef)
			{
			fCFMutableArrayRef = sArrayMutable(fCFMutableArrayRef);
			}
		else if (fCFArrayRef)
			{
			fCFMutableArrayRef = sArrayMutable(fCFArrayRef);
			fCFArrayRef.Clear();
			}
		else
			{
			fCFMutableArrayRef = sArrayMutable();
			}
		}
	ZAssert(!fCFArrayRef);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_CFType

ZValMap_CFType::operator bool() const
	{
	if (fCFMutableDictionaryRef)
		return ::CFDictionaryGetCount(fCFMutableDictionaryRef);
	else if (fCFDictionaryRef)
		return ::CFDictionaryGetCount(fCFDictionaryRef);
	else
		return false;
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

ZValMap_CFType& ZValMap_CFType::operator=(const ZRef<CFMutableDictionaryRef>& iOther)
	{
	fCFMutableDictionaryRef = iOther;
	fCFDictionaryRef.Clear();
	return *this;
	}

ZValMap_CFType& ZValMap_CFType::operator=(const ZRef<CFDictionaryRef>& iOther)
	{
	fCFMutableDictionaryRef.Clear();
	fCFDictionaryRef = iOther;
	return *this;
	}

void ZValMap_CFType::Clear()
	{
	fCFMutableDictionaryRef = sDictionaryMutable();
	fCFDictionaryRef.Clear();
	}

bool ZValMap_CFType::QGet(const string8& iName, ZVal_CFType& oVal) const
	{
	ZRef<CFStringRef> theName = sString(iName);
	if (fCFMutableDictionaryRef)
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFMutableDictionaryRef, theName))
			{
			oVal = theVal;
			return true;
			}
		}
	else if (fCFDictionaryRef)
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
	else if (fCFDictionaryRef)
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(fCFDictionaryRef, iName))
			{
			oVal = theVal;
			return true;
			}
		}

	return false;
	}

ZVal_CFType ZValMap_CFType::DGet(const string8& iName, const ZVal_CFType& iDefault) const
	{
	ZVal_CFType theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_CFType ZValMap_CFType::DGet(CFStringRef iName, const ZVal_CFType& iDefault) const
	{
	ZVal_CFType theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_CFType ZValMap_CFType::Get(const string8& iName) const
	{ return this->DGet(iName, ZVal_CFType()); }

ZVal_CFType ZValMap_CFType::Get(CFStringRef iName) const
	{ return this->DGet(iName, ZVal_CFType()); }

void ZValMap_CFType::Set(const string8& iName, const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFDictionarySetValue(fCFMutableDictionaryRef, sString(iName), iVal);
	}

void ZValMap_CFType::Set(CFStringRef iName, const ZVal_CFType& iVal)
	{
	this->pTouch();
	::CFDictionarySetValue(fCFMutableDictionaryRef, iName, iVal);
	}

void ZValMap_CFType::Erase(const string8& iName)
	{
	this->pTouch();
	::CFDictionaryRemoveValue(fCFMutableDictionaryRef, sString(iName));
	}

void ZValMap_CFType::Erase(CFStringRef iName)
	{
	this->pTouch();
	::CFDictionaryRemoveValue(fCFMutableDictionaryRef, iName);
	}

CFDictionaryRef& ZValMap_CFType::OParam()
	{
	fCFMutableDictionaryRef.Clear();
	fCFDictionaryRef.Clear();
	return fCFDictionaryRef.GetPtrRef();
	}

ZValMap_CFType::operator CFTypeRef() const
	{
	if (fCFMutableDictionaryRef)
		return fCFMutableDictionaryRef;
	return fCFDictionaryRef;
	}

ZValMap_CFType::operator CFDictionaryRef() const
	{
	if (fCFMutableDictionaryRef)
		return fCFMutableDictionaryRef;
	return fCFDictionaryRef;
	}

void ZValMap_CFType::pTouch()
	{
	if (!fCFMutableDictionaryRef || ::CFGetRetainCount(fCFMutableDictionaryRef) > 1)
		{
		if (fCFMutableDictionaryRef)
			{
			fCFMutableDictionaryRef = sDictionaryMutable(fCFMutableDictionaryRef);
			}
		else if (fCFDictionaryRef)
			{
			fCFMutableDictionaryRef = sDictionaryMutable(fCFDictionaryRef);
			fCFDictionaryRef.Clear();
			}
		else
			{
			fCFMutableDictionaryRef = sDictionaryMutable();
			}
		}
	ZAssert(!fCFDictionaryRef);
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
