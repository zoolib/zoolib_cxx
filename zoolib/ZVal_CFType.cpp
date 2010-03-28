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

#include "zoolib/ZCompare_T.h"
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

template <> int sCompare_T(const ZVal_CFType& iL, const ZVal_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <> int sCompare_T(const ZSeq_CFType& iL, const ZSeq_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <> int sCompare_T(const ZMap_CFType& iL, const ZMap_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace {

template <class S>
bool spGetNumber_T(CFTypeRef iTypeRef, CFNumberType iNumberType, S& oVal)
	{
	if (iTypeRef && ::CFGetTypeID(iTypeRef) == ::CFNumberGetTypeID())
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
ZRef<CFTypeRef> spNumber_T(CFNumberType iNumberType, const S& iVal)
	{ return Adopt_T<CFTypeRef>(::CFNumberCreate(kCFAllocatorDefault, iNumberType, &iVal)); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType

/**
\class ZVal_CFType
\ingroup ZVal

\brief Compatible with ZVal & CFTypeRef
*/

ZAny ZVal_CFType::AsAny() const
	{ return this->DAsAny(ZAny()); }

ZAny ZVal_CFType::DAsAny(const ZAny& iDefault) const
	{ return ZUtil_CFType::sDAsAny(ZAny(), *this); }

ZVal_CFType::operator bool() const
	{
	if (CFTypeRef theVal = this->Get())
		{
		#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
			&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
			if (::CFGetTypeID(theVal) == ::CFNullGetTypeID())
				return false;
		#endif
		return true;
		}
	return false;
	}

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

ZVal_CFType::ZVal_CFType(CFTypeRef iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(const ZRef<CFTypeRef>& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(const Adopt_T<CFTypeRef>& iVal)
:	inherited(iVal)
	{}

ZVal_CFType::ZVal_CFType(int8 iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(int16 iVal)
:	inherited(spNumber_T(kCFNumberSInt16Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(int32 iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(int64 iVal)
:	inherited(spNumber_T(kCFNumberSInt64Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(bool iVal)
:	inherited(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

ZVal_CFType::ZVal_CFType(float iVal)
:	inherited(spNumber_T(kCFNumberFloat32Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(double iVal)
:	inherited(spNumber_T(kCFNumberFloat64Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(const char* iVal)
:	inherited(sString(iVal))
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

ZVal_CFType::ZVal_CFType(const ZSeq_CFType& iVal)
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

ZVal_CFType& ZVal_CFType::operator=(const ZRef<CFTypeRef>& iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

ZVal_CFType& ZVal_CFType::operator=(const Adopt_T<CFTypeRef>& iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

void ZVal_CFType::Clear()
	{ inherited::Clear(); }

template <>
bool ZVal_CFType::QGet_T<int8>(int8& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberSInt8Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberCharType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int16>(int16& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberSInt16Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberShortType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int32>(int32& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberSInt32Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberIntType, oVal))
		return true;

	#if !ZCONFIG_Is64Bit
		if (spGetNumber_T(*this, kCFNumberLongType, oVal))
			return true;
	#endif

	return false;
	}

template <>
bool ZVal_CFType::QGet_T<int64>(int64& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberSInt64Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberLongLongType, oVal))
		return true;

	#if ZCONFIG_Is64Bit
		if (spGetNumber_T(*this, kCFNumberLongType, oVal))
			return true;
	#endif

	return false;
	}

template <>
bool ZVal_CFType::QGet_T<bool>(bool& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		{
		oVal = ::CFBooleanGetValue(this->StaticCast<CFBooleanRef>());
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<float>(float& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberFloat32Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberFloatType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<double>(double& oVal) const
	{
	if (spGetNumber_T(*this, kCFNumberFloat64Type, oVal))
		return true;
	if (spGetNumber_T(*this, kCFNumberDoubleType, oVal))
		return true;
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<string8>(string8& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		{
		oVal = sAsUTF8(this->StaticCast<CFStringRef>());
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZRef<CFStringRef> >(ZRef<CFStringRef>& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		{
		oVal = this->StaticCast<CFStringRef>();
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZData_CFType>(ZData_CFType& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDataGetTypeID())
		{
		oVal = this->StaticCast<CFDataRef>();
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZSeq_CFType>(ZSeq_CFType& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		{
		oVal = this->StaticCast<CFArrayRef>();
		return true;
		}
	return false;
	}

template <>
bool ZVal_CFType::QGet_T<ZMap_CFType>(ZMap_CFType& oVal) const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		{
		oVal = this->StaticCast<CFDictionaryRef>();
		return true;
		}
	return false;
	}

template <>
void ZVal_CFType::Set_T<int8>(const int8& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int16>(const int16& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int32>(const int32& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<int64>(const int64& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void ZVal_CFType::Set_T<bool>(const bool& iVal)
	{ inherited::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void ZVal_CFType::Set_T<float>(const float& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void ZVal_CFType::Set_T<double>(const double& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat64Type, iVal)); }

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
void ZVal_CFType::Set_T<ZSeq_CFType>(const ZSeq_CFType& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set_T<ZMap_CFType>(const ZMap_CFType& iVal)
	{ inherited::operator=(iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CFType typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, CFString, ZRef<CFStringRef>)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Data, ZData_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Seq, ZSeq_CFType)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CFType, Map, ZMap_CFType)

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_CFType

ZSeq_Any ZSeq_CFType::AsSeq_Any(const ZAny& iDefault) const
	{ return ZUtil_CFType::sAsSeq_Any(iDefault, this->pArray()); }

ZSeq_CFType::operator bool() const
	{ return this->Count(); }

ZSeq_CFType::ZSeq_CFType()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

ZSeq_CFType::ZSeq_CFType(const ZSeq_CFType& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZSeq_CFType::~ZSeq_CFType()
	{}

ZSeq_CFType& ZSeq_CFType::operator=(const ZSeq_CFType& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZSeq_CFType::ZSeq_CFType(CFMutableArrayRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZSeq_CFType::ZSeq_CFType(CFArrayRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_CFType::ZSeq_CFType(const ZRef<CFMutableArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZSeq_CFType::ZSeq_CFType(const ZRef<CFArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_CFType::ZSeq_CFType(const Adopt_T<CFMutableArrayRef>& iOther)
:	inherited(ZRef<CFMutableArrayRef>(iOther))
,	fMutable(true)
	{}

ZSeq_CFType::ZSeq_CFType(const Adopt_T<CFArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_CFType& ZSeq_CFType::operator=(CFMutableArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::operator=(CFArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::operator=(const ZRef<CFMutableArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::operator=(const ZRef<CFArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::operator=(const Adopt_T<CFMutableArrayRef>& iOther)
	{
	inherited::operator=(ZRef<CFMutableArrayRef>(iOther));
	fMutable = true;
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::operator=(const Adopt_T<CFArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t ZSeq_CFType::Count() const
	{
	if (CFArrayRef theArray = this->pArray())
		return ::CFArrayGetCount(theArray);
	return 0;
	}

void ZSeq_CFType::Clear()
	{ inherited::Clear(); }

bool ZSeq_CFType::QGet(size_t iIndex, ZRef<CFTypeRef>& oVal) const
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

ZVal_CFType ZSeq_CFType::DGet(const ZVal_CFType& iDefault, size_t iIndex) const
	{
	ZVal_CFType result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

ZVal_CFType ZSeq_CFType::Get(size_t iIndex) const
	{ return this->DGet(ZVal_CFType(), iIndex); }

ZSeq_CFType& ZSeq_CFType::Set(size_t iIndex, const ZVal_CFType& iVal)
	{
	::CFArraySetValueAtIndex(this->pTouch(), iIndex, iVal);
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::Erase(size_t iIndex)
	{
	CFMutableArrayRef theArray = this->pTouch();
	if (const size_t theCount = ::CFArrayGetCount(theArray))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(theArray, iIndex);
		}
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::Insert(size_t iIndex, const ZVal_CFType& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	const size_t theCount = ::CFArrayGetCount(theArray);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(theArray, iIndex, iVal);
	return *this;
	}

ZSeq_CFType& ZSeq_CFType::Append(const ZVal_CFType& iVal)
	{
	::CFArrayAppendValue(this->pTouch(), iVal);
	return *this;
	}

CFArrayRef ZSeq_CFType::pArray() const
	{ return inherited::Get(); }

CFMutableArrayRef ZSeq_CFType::pTouch()
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

ZMap_Any ZMap_CFType::AsMap_Any(const ZAny& iDefault) const
	{ return ZUtil_CFType::sAsMap_Any(iDefault, this->pDictionary()); }

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

ZMap_CFType::ZMap_CFType(CFDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CFType::ZMap_CFType(CFMutableDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_CFType::ZMap_CFType(const ZRef<CFDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CFType::ZMap_CFType(const ZRef<CFMutableDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_CFType::ZMap_CFType(const Adopt_T<CFDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CFType::ZMap_CFType(const Adopt_T<CFMutableDictionaryRef>& iOther)
:	inherited(ZRef<CFMutableDictionaryRef>(iOther))
,	fMutable(true)
	{}

ZMap_CFType& ZMap_CFType::operator=(CFMutableDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZMap_CFType& ZMap_CFType::operator=(CFDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

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

ZMap_CFType& ZMap_CFType::operator=(const Adopt_T<CFMutableDictionaryRef>& iOther)
	{
	inherited::operator=(ZRef<CFMutableDictionaryRef>(iOther));
	fMutable = true;
	return *this;
	}

ZMap_CFType& ZMap_CFType::operator=(const Adopt_T<CFDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

void ZMap_CFType::Clear()
	{ inherited::Clear(); }

bool ZMap_CFType::QGet(const string8& iName, ZRef<CFTypeRef>& oVal) const
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

bool ZMap_CFType::QGet(CFStringRef iName, ZRef<CFTypeRef>& oVal) const
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

ZMap_CFType& ZMap_CFType::Set(const string8& iName, const ZVal_CFType& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), sString(iName), iVal);
	return *this;
	}

ZMap_CFType& ZMap_CFType::Set(CFStringRef iName, const ZVal_CFType& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), iName, iVal);
	return *this;
	}

ZMap_CFType& ZMap_CFType::Erase(const string8& iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), sString(iName));
	return *this;
	}

ZMap_CFType& ZMap_CFType::Erase(CFStringRef iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), iName);
	return *this;
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
