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

#include <typeinfo>

NAMESPACE_ZOOLIB_BEGIN

using std::type_info;

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

bool ZVal_CFType::sFromAny(const ZAny& iAny, ZVal_CFType& oVal)
	{ return ZUtil_CFType::sAsCFType(iAny, oVal); }

ZVal_Any ZVal_CFType::AsVal_Any() const
	{ return this->AsVal_Any(ZVal_Any()); }

ZVal_Any ZVal_CFType::AsVal_Any(const ZVal_Any& iDefault) const
	{ return ZUtil_CFType::sAsVal_Any(this, iDefault); }

ZVal_CFType::ZVal_CFType()
	{}

ZVal_CFType::ZVal_CFType(const ZVal_CFType& iOther)
:	inherited(iOther)
	{}

ZVal_CFType::~ZVal_CFType()
	{}

ZVal_CFType& ZVal_CFType::operator=(const ZVal_CFType& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZVal_CFType::ZVal_CFType(const ZRef<CFTypeRef>& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(int8 iVal)
:	inherited(
	NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt8Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int16 iVal)
:	inherited(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt16Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int32 iVal)
:	inherited(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(int64 iVal)
:	inherited(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(bool iVal)
:	inherited(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

ZVal_CFType::ZVal_CFType(float iVal)
:	inherited(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat32Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(double iVal)
:	inherited(
		NoRetain(CFTypeRef(::CFNumberCreate(kCFAllocatorDefault, kCFNumberFloat64Type, &iVal))))
	{}

ZVal_CFType::ZVal_CFType(const string8& iVal)
:	inherited(sString(iVal))
	{}

ZVal_CFType::ZVal_CFType(const string16& iVal)
:	inherited(sString(iVal))
	{}

ZVal_CFType::ZVal_CFType(CFStringRef iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZData_CFType& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZList_CFType& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZMap_CFType& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(CFDataRef iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(CFArrayRef iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(CFDictionaryRef iVal)
:	inherited(iVal)
	{}

ZVal_CFType& ZVal_CFType::operator=(CFTypeRef iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

void ZVal_CFType::Clear()
	{ inherited::Clear(); }

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

	#if !ZCONFIG_Is64Bit
		if (sGetNumber_T(*this, kCFNumberLongType, oVal))
			return true;
	#endif

	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int64>(int64& oVal) const
	{
	if (sGetNumber_T(*this, kCFNumberSInt64Type, oVal))
		return true;
	if (sGetNumber_T(*this, kCFNumberLongLongType, oVal))
		return true;

	#if ZCONFIG_Is64Bit
		if (sGetNumber_T(*this, kCFNumberLongType, oVal))
			return true;
	#endif

	return false;
	}

template <>
bool ZVal_CFType::QGet_T<bool>(bool& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		{
		oVal = ::CFBooleanGetValue(static_cast<CFBooleanRef>(Get()));
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
		oVal = sAsUTF8(static_cast<CFStringRef>(Get()));
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZRef<CFStringRef> >(ZRef<CFStringRef>& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFStringGetTypeID())
		{
		oVal = static_cast<CFStringRef>(Get());
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZData_CFType>(ZData_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFDataGetTypeID())
		{
		oVal = static_cast<CFDataRef>(Get());
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZList_CFType>(ZList_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		{
		oVal = static_cast<CFArrayRef>(Get());
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZMap_CFType>(ZMap_CFType& oVal) const
	{
	if (::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		{
		oVal = static_cast<CFDictionaryRef>(Get());
		return true;
		}
	return false;
	}

template <>
void ZVal_CFType::Set_T<int8>(const int8& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int16>(const int16& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int32>(const int32& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int64>(const int64& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void ZVal_CFType::Set_T<bool>(const bool& iVal)
	{ inherited::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void ZVal_CFType::Set_T<float>(const float& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<double>(const double& iVal)
	{ inherited::operator=(sNumber_T(kCFNumberFloat64Type, iVal)); }

template <>
void ZVal_CFType::Set_T<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void ZVal_CFType::Set_T<ZRef<CFStringRef> >(const ZRef<CFStringRef>& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZData_CFType>(const ZData_CFType& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZList_CFType>(const ZList_CFType& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZMap_CFType>(const ZMap_CFType& iVal)
	{ inherited::operator=(iVal); }

CFTypeRef& ZVal_CFType::OParam()
	{
	inherited::Clear();
	return this->GetPtrRef();
	}
	
// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, CFString, ZRef<CFStringRef>)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Data, ZData_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, List, ZList_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Map, ZMap_CFType)

// =================================================================================================
#pragma mark -
#pragma mark * ZList_CFType

ZList_Any ZList_CFType::AsList_Any() const
	{ return this->AsList_Any(ZVal_Any()); }

ZList_Any ZList_CFType::AsList_Any(const ZVal_Any& iDefault) const
	{ return ZUtil_CFType::sAsList_Any(this->pArray(), iDefault); }

ZList_CFType::operator bool() const
	{ return this->Count(); }

ZList_CFType::ZList_CFType()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

ZList_CFType::ZList_CFType(const ZList_CFType& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZList_CFType::~ZList_CFType()
	{}

ZList_CFType& ZList_CFType::operator=(const ZList_CFType& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZList_CFType::ZList_CFType(const ZRef<CFMutableArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZList_CFType::ZList_CFType(const ZRef<CFArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZList_CFType& ZList_CFType::operator=(const ZRef<CFMutableArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZList_CFType& ZList_CFType::operator=(const ZRef<CFArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t ZList_CFType::Count() const
	{
	if (CFArrayRef theArray = this->pArray())
		return ::CFArrayGetCount(theArray);
	return 0;
	}

void ZList_CFType::Clear()
	{ inherited::Clear(); }

bool ZList_CFType::QGet(size_t iIndex, ZVal_CFType& oVal) const
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

ZVal_CFType ZList_CFType::DGet(const ZVal_CFType& iDefault, size_t iIndex) const
	{
	ZVal_CFType result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

ZVal_CFType ZList_CFType::Get(size_t iIndex) const
	{ return this->DGet(ZVal_CFType(), iIndex); }

void ZList_CFType::Set(size_t iIndex, const ZVal_CFType& iVal)
	{ ::CFArraySetValueAtIndex(this->pTouch(), iIndex, iVal); }

void ZList_CFType::Erase(size_t iIndex)
	{
	CFMutableArrayRef theArray = this->pTouch();
	if (const size_t theCount = ::CFArrayGetCount(theArray))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(theArray, iIndex);
		}
	}

void ZList_CFType::Insert(size_t iIndex, const ZVal_CFType& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	const size_t theCount = ::CFArrayGetCount(theArray);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(theArray, iIndex, iVal);
	}

void ZList_CFType::Append(const ZVal_CFType& iVal)
	{ ::CFArrayAppendValue(this->pTouch(), iVal); }

CFArrayRef& ZList_CFType::OParam()
	{
	inherited::Clear();
	return this->GetPtrRef();
	}

CFArrayRef ZList_CFType::pArray() const
	{ return inherited::Get(); }

CFMutableArrayRef ZList_CFType::pTouch()
	{
	ZRef<CFMutableArrayRef> theMutableArray;
	if (CFArrayRef theArray = this->pArray())
		{
		if (!fMutable || ::CFGetRetainCount(theArray) > 1)
			{
			theMutableArray = sArrayMutable(theArray);
			inherited::operator=(theMutableArray);
			}
		else
			{
			theMutableArray = const_cast<CFMutableArrayRef>(theArray);
			}
		}
	else
		{
		theMutableArray = sArrayMutable();
		inherited::operator=(theMutableArray);
		}
	fMutable = true;
	return theMutableArray;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_CFType

ZMap_Any ZMap_CFType::AsMap_Any() const
	{ return this->AsMap_Any(ZVal_Any()); }

ZMap_Any ZMap_CFType::AsMap_Any(const ZVal_Any& iDefault) const
	{ return ZUtil_CFType::sAsMap_Any(this->pDictionary(), iDefault); }

ZMap_CFType::operator bool() const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		return ::CFDictionaryGetCount(theDictionary);
	return false;
	}

ZMap_CFType::ZMap_CFType()
:	inherited(sDictionaryMutable())
,	fMutable(true)
	{}

ZMap_CFType::ZMap_CFType(const ZMap_CFType& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZMap_CFType::~ZMap_CFType()
	{}

ZMap_CFType& ZMap_CFType::operator=(const ZMap_CFType& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZMap_CFType::ZMap_CFType(const ZRef<CFDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CFType::ZMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_CFType& ZMap_CFType::operator=(const ZRef<CFMutableDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZMap_CFType& ZMap_CFType::operator=(const ZRef<CFDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

void ZMap_CFType::Clear()
	{ inherited::Clear(); }

bool ZMap_CFType::QGet(const string8& iName, ZVal_CFType& oVal) const
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

bool ZMap_CFType::QGet(CFStringRef iName, ZVal_CFType& oVal) const
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

ZVal_CFType ZMap_CFType::DGet(const ZVal_CFType& iDefault, const string8& iName) const
	{
	ZVal_CFType theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_CFType ZMap_CFType::DGet(const ZVal_CFType& iDefault, CFStringRef iName) const
	{
	ZVal_CFType theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_CFType ZMap_CFType::Get(const string8& iName) const
	{ return this->DGet(ZVal_CFType(), iName); }

ZVal_CFType ZMap_CFType::Get(CFStringRef iName) const
	{ return this->DGet(ZVal_CFType(), iName); }

void ZMap_CFType::Set(const string8& iName, const ZVal_CFType& iVal)
	{ ::CFDictionarySetValue(this->pTouch(), sString(iName), iVal); }

void ZMap_CFType::Set(CFStringRef iName, const ZVal_CFType& iVal)
	{ ::CFDictionarySetValue(this->pTouch(), iName, iVal); }

void ZMap_CFType::Erase(const string8& iName)
	{ ::CFDictionaryRemoveValue(this->pTouch(), sString(iName)); }

void ZMap_CFType::Erase(CFStringRef iName)
	{ ::CFDictionaryRemoveValue(this->pTouch(), iName); }

CFDictionaryRef& ZMap_CFType::OParam()
	{
	inherited::Clear();
	return this->GetPtrRef();
	}

CFDictionaryRef ZMap_CFType::pDictionary() const
	{ return inherited::Get(); }

CFMutableDictionaryRef ZMap_CFType::pTouch()
	{
	ZRef<CFMutableDictionaryRef> theMutableDictionary;
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (!fMutable || ::CFGetRetainCount(theDictionary) > 1)
			{
			theMutableDictionary = sDictionaryMutable(theDictionary);
			inherited::operator=(theMutableDictionary);
			}
		else
			{
			theMutableDictionary = const_cast<CFMutableDictionaryRef>(theDictionary);
			}
		}
	else
		{
		theMutableDictionary = sDictionaryMutable();
		inherited::operator=(theMutableDictionary);
		}
	fMutable = true;
	return theMutableDictionary;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(CFType)
