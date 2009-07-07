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

//ZVal_CFType::operator bool() const
//	{ return fCFTypeRef; }

ZVal_CFType::ZVal_CFType()
	{}

ZVal_CFType::ZVal_CFType(const ZVal_CFType& iOther)
:	ZRef<CFTypeRef>(iOther)
	{}

ZVal_CFType::~ZVal_CFType()
	{}

ZVal_CFType& ZVal_CFType::operator=(const ZVal_CFType& iOther)
	{
	ZRef<CFTypeRef>::operator=(iOther);
	return *this;
	}

ZVal_CFType::ZVal_CFType(const ZRef<CFTypeRef>& iVal)
:	ZRef<CFTypeRef>(iVal)
	{}

ZVal_CFType::ZVal_CFType(int8 iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt8Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int16 iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt16Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int32 iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int64 iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(bool iVal)
:	ZRef<CFTypeRef>(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

ZVal_CFType::ZVal_CFType(float iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(double iVal)
:	ZRef<CFTypeRef>(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(const string8& iVal)
:	ZRef<CFTypeRef>(sString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const string16& iVal)
:	ZRef<CFTypeRef>(sString(iVal))
	{}

#if 0
ZVal_CFType::ZVal_CFType(const ZRef<CFStringRef>& iVal)
:	ZRef<CFTypeRef>(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZRef<CFDataRef>& iVal)
:	ZRef<CFTypeRef>(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZRef<CFArrayRef>& iVal)
:	ZRef<CFTypeRef>(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZRef<CFDictionaryRef>& iVal)
:	ZRef<CFTypeRef>(iVal)
	{}
#endif

void ZVal_CFType::Clear()
	{ ZRef<CFTypeRef>::Clear(); }

template <>
bool ZVal_CFType::QGet_T<int8>(int8& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberSInt8Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberCharType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int16>(int16& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberSInt16Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberShortType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int32>(int32& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberSInt32Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberIntType, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberLongType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int64>(int64& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberSInt64Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberLongLongType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<bool>(bool& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		{
		oVal = ::CFBooleanGetValue(static_cast<CFBooleanRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<float>(float& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberFloat32Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberFloatType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<double>(double& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberFloat64Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberDoubleType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<string8>(string8& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFStringGetTypeID())
		{
		oVal = sAsUTF8(static_cast<CFStringRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZRef<CFStringRef> >(ZRef<CFStringRef>& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFStringGetTypeID())
		{
		oVal = (static_cast<CFStringRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValData_CFType>(ZValData_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFDataGetTypeID())
		{
		oVal = (static_cast<CFDataRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValList_CFType>(ZValList_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		{
		oVal = (static_cast<CFArrayRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZValMap_CFType>(ZValMap_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		{
		oVal = (static_cast<CFDictionaryRef>(CFTypeRef(*this)));
		return true;
		}
	return false;
	}

template <>
void ZVal_CFType::Set_T<int8>(const int8& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int16>(const int16& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int32>(const int32& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int64>(const int64& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void ZVal_CFType::Set_T<bool>(const bool& iVal)
	{ ZRef<CFTypeRef>::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void ZVal_CFType::Set_T<float>(const float& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<double>(const double& iVal)
	{ ZRef<CFTypeRef>::operator=(sNumber_T(kCFNumberFloat64Type, iVal)); }

template <>
void ZVal_CFType::Set_T<string8>(const string8& iVal)
	{ ZRef<CFTypeRef>::operator=(sString(iVal)); }

template <>
void ZVal_CFType::Set_T<ZRef<CFStringRef> >(const ZRef<CFStringRef>& iVal)
	{ ZRef<CFTypeRef>::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZValData_CFType>(const ZValData_CFType& iVal)
	{ ZRef<CFTypeRef>::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZValList_CFType>(const ZValList_CFType& iVal)
	{ ZRef<CFTypeRef>::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZValMap_CFType>(const ZValMap_CFType& iVal)
	{ ZRef<CFTypeRef>::operator=(iVal); }

CFTypeRef& ZVal_CFType::OParam()
	{
	ZRef<CFTypeRef>::Clear();
	return this->GetPtrRef();
	}
	
ZMACRO_ZValAccessors_Def_Std(ZVal_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, CFString, ZRef<CFStringRef>)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Data, ZValData_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, List, ZValList_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Map, ZValMap_CFType)

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_CFType

ZValList_CFType::operator bool() const
	{ return this->Count(); }

ZValList_CFType::ZValList_CFType()
:	ZRef<CFArrayRef>(sArrayMutable())
,	fMutable(true)
	{}

ZValList_CFType::ZValList_CFType(const ZValList_CFType& iOther)
:	ZRef<CFArrayRef>(iOther)
,	fMutable(iOther.fMutable)
	{}

ZValList_CFType::~ZValList_CFType()
	{}

ZValList_CFType& ZValList_CFType::operator=(const ZValList_CFType& iOther)
	{
	ZRef<CFArrayRef>::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZValList_CFType::ZValList_CFType(const ZRef<CFMutableArrayRef>& iOther)
:	ZRef<CFArrayRef>(iOther)
,	fMutable(true)
	{}

ZValList_CFType::ZValList_CFType(const ZRef<CFArrayRef>& iOther)
:	ZRef<CFArrayRef>(iOther)
,	fMutable(false)
	{}

ZValList_CFType& ZValList_CFType::operator=(const ZRef<CFMutableArrayRef>& iOther)
	{
	ZRef<CFArrayRef>::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZValList_CFType& ZValList_CFType::operator=(const ZRef<CFArrayRef>& iOther)
	{
	ZRef<CFArrayRef>::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t ZValList_CFType::Count() const
	{
	if (CFArrayRef theArray = this->pArray())
		return ::CFArrayGetCount(theArray);
	return 0;
	}

void ZValList_CFType::Clear()
	{ ZRef<CFArrayRef>::Clear(); }

bool ZValList_CFType::QGet(size_t iIndex, ZVal_CFType& oVal) const
	{
	if (CFArrayRef theArray = this->pArray())
		{
		if (size_t theCount = ::CFArrayGetCount(theArray))
			{
			if (iIndex < theCount)
				{
				oVal = ::CFArrayGetValueAtIndex(theArray, iIndex);
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
	CFMutableArrayRef theArray = this->pTouch();
	::CFArraySetValueAtIndex(theArray, iIndex, iVal);
	}

void ZValList_CFType::Erase(size_t iIndex)
	{
	CFMutableArrayRef theArray = this->pTouch();
	if (const size_t theCount = ::CFArrayGetCount(theArray))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(theArray, iIndex);
		}
	}

void ZValList_CFType::Insert(size_t iIndex, const ZVal_CFType& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	const size_t theCount = ::CFArrayGetCount(theArray);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(theArray, iIndex, iVal);
	}

void ZValList_CFType::Append(const ZVal_CFType& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	::CFArrayAppendValue(theArray, iVal);
	}

CFArrayRef& ZValList_CFType::OParam()
	{
	ZRef<CFArrayRef>::Clear();
	return this->GetPtrRef();
	}

CFArrayRef ZValList_CFType::pArray() const
	{ return ZRef<CFArrayRef>::Get(); }

CFMutableArrayRef ZValList_CFType::pTouch()
	{
	CFMutableArrayRef theMutableArray;
	if (CFArrayRef theArray = this->pArray())
		{
		if (!fMutable || ::CFGetRetainCount(theArray) > 1)
			{
			theMutableArray = sArrayMutable(theArray);
			ZRef<CFArrayRef>::operator=(theMutableArray);
			}
		else
			{
			theMutableArray = const_cast<CFMutableArrayRef>(theArray);
			}
		}
	else
		{
		theMutableArray = sArrayMutable();
		ZRef<CFArrayRef>::operator=(theMutableArray);
		}
	fMutable = true;
	return theMutableArray;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_CFType

ZValMap_CFType::operator bool() const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		return ::CFDictionaryGetCount(theDictionary);
	return false;
	}

ZValMap_CFType::ZValMap_CFType()
:	ZRef<CFDictionaryRef>(sDictionaryMutable())
,	fMutable(true)
	{}

ZValMap_CFType::ZValMap_CFType(const ZValMap_CFType& iOther)
:	ZRef<CFDictionaryRef>(iOther)
,	fMutable(iOther.fMutable)
	{}

ZValMap_CFType::~ZValMap_CFType()
	{}

ZValMap_CFType& ZValMap_CFType::operator=(const ZValMap_CFType& iOther)
	{
	ZRef<CFDictionaryRef>::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZValMap_CFType::ZValMap_CFType(const ZRef<CFDictionaryRef>& iOther)
:	ZRef<CFDictionaryRef>(iOther)
,	fMutable(false)
	{}

ZValMap_CFType::ZValMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther)
:	ZRef<CFDictionaryRef>(iOther)
,	fMutable(true)
	{}

ZValMap_CFType& ZValMap_CFType::operator=(const ZRef<CFMutableDictionaryRef>& iOther)
	{
	ZRef<CFDictionaryRef>::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZValMap_CFType& ZValMap_CFType::operator=(const ZRef<CFDictionaryRef>& iOther)
	{
	ZRef<CFDictionaryRef>::operator=(iOther);
	fMutable = false;
	return *this;
	}

void ZValMap_CFType::Clear()
	{ ZRef<CFDictionaryRef>::Clear(); }

bool ZValMap_CFType::QGet(const string8& iName, ZVal_CFType& oVal) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, sString(iName)))
			{
			oVal = theVal;
			return true;
			}
		}
	return false;
	}

bool ZValMap_CFType::QGet(CFStringRef iName, ZVal_CFType& oVal) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, iName))
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
	CFMutableDictionaryRef theDictionary = this->pTouch();
	::CFDictionarySetValue(theDictionary, sString(iName), iVal);
	}

void ZValMap_CFType::Set(CFStringRef iName, const ZVal_CFType& iVal)
	{
	CFMutableDictionaryRef theDictionary = this->pTouch();
	::CFDictionarySetValue(theDictionary, iName, iVal);
	}

void ZValMap_CFType::Erase(const string8& iName)
	{
	CFMutableDictionaryRef theDictionary = this->pTouch();
	::CFDictionaryRemoveValue(theDictionary, sString(iName));
	}

void ZValMap_CFType::Erase(CFStringRef iName)
	{
	CFMutableDictionaryRef theDictionary = this->pTouch();
	::CFDictionaryRemoveValue(theDictionary, iName);
	}

CFDictionaryRef& ZValMap_CFType::OParam()
	{
	ZRef<CFDictionaryRef>::Clear();
	return this->GetPtrRef();
	}

CFDictionaryRef ZValMap_CFType::pDictionary() const
	{ return ZRef<CFDictionaryRef>::Get(); }

CFMutableDictionaryRef ZValMap_CFType::pTouch()
	{
	CFMutableDictionaryRef theMutableDictionary;
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (!fMutable || ::CFGetRetainCount(theDictionary) > 1)
			{
			theMutableDictionary = sDictionaryMutable(theDictionary);
			ZRef<CFDictionaryRef>::operator=(theMutableDictionary);
			}
		else
			{
			theMutableDictionary = const_cast<CFMutableDictionaryRef>(theDictionary);
			}
		}
	else
		{
		theMutableDictionary = sDictionaryMutable();
		ZRef<CFDictionaryRef>::operator=(theMutableDictionary);
		}
	fMutable = true;
	return theMutableDictionary;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
