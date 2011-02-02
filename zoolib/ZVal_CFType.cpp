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

namespace ZooLib {

using ZUtil_CFType::sAsUTF8;
using ZUtil_CFType::sString;
using ZUtil_CFType::sStringMutable;
using ZUtil_CFType::sArrayMutable;
using ZUtil_CFType::sDictionaryMutable;

template <>
int sCompare_T(const ZVal_CFType& iL, const ZVal_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const ZSeq_CFType& iL, const ZSeq_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const ZMap_CFType& iL, const ZMap_CFType& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

namespace {

template <class S>
ZQ<S> spGetNumber_T(CFTypeRef iTypeRef, CFNumberType iNumberType)
	{
	if (iTypeRef && ::CFGetTypeID(iTypeRef) == ::CFNumberGetTypeID())
		{
		CFNumberRef theNumberRef = static_cast<CFNumberRef>(iTypeRef);
		if (::CFNumberGetType(theNumberRef) == iNumberType)
			{
			ZQ<S> result;
			::CFNumberGetValue(theNumberRef, iNumberType, &result.OParam());
			return result;
			}
		}
	return null;
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
	if (CFTypeRef theVal = inherited::Get())
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

ZVal_CFType::ZVal_CFType(char iVal)
:	inherited(spNumber_T(kCFNumberCharType, iVal))
	{}

ZVal_CFType::ZVal_CFType(unsigned char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(signed char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(wchar_t iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(short iVal)
:	inherited(spNumber_T(kCFNumberShortType, iVal))
	{}

ZVal_CFType::ZVal_CFType(unsigned short iVal)
:	inherited(spNumber_T(kCFNumberSInt16Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(int iVal)
:	inherited(spNumber_T(kCFNumberIntType, iVal))
	{}

ZVal_CFType::ZVal_CFType(unsigned int iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(long iVal)
:	inherited(spNumber_T(kCFNumberLongType, iVal))
	{}

ZVal_CFType::ZVal_CFType(unsigned long iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CFType::ZVal_CFType(long long iVal)
:	inherited(spNumber_T(kCFNumberLongLongType, iVal))
	{}

ZVal_CFType::ZVal_CFType(unsigned long long iVal)
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

template <>
ZQ<char> ZVal_CFType::QGet<char>() const
	{
	if (ZQ<char> theQ = spGetNumber_T<char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<char> theQ = spGetNumber_T<char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned char> ZVal_CFType::QGet<unsigned char>() const
	{
	if (ZQ<unsigned char> theQ = spGetNumber_T<unsigned char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<unsigned char> theQ = spGetNumber_T<unsigned char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<signed char> ZVal_CFType::QGet<signed char>() const
	{
	if (ZQ<signed char> theQ = spGetNumber_T<signed char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<signed char> theQ = spGetNumber_T<signed char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<short> ZVal_CFType::QGet<short>() const
	{
	if (ZQ<short> theQ = spGetNumber_T<short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<short> theQ = spGetNumber_T<short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned short> ZVal_CFType::QGet<unsigned short>() const
	{
	if (ZQ<unsigned short> theQ = spGetNumber_T<unsigned short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<unsigned short> theQ = spGetNumber_T<unsigned short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
ZQ<int> ZVal_CFType::QGet<int>() const
	{
	if (ZQ<int> theQ = spGetNumber_T<int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<int> theQ = spGetNumber_T<int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned int> ZVal_CFType::QGet<unsigned int>() const
	{
	if (ZQ<unsigned int> theQ = spGetNumber_T<unsigned int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned int> theQ = spGetNumber_T<unsigned int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
ZQ<long> ZVal_CFType::QGet<long>() const
	{
	if (ZQ<long> theQ = spGetNumber_T<long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<long> theQ = spGetNumber_T<long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned long> ZVal_CFType::QGet<unsigned long>() const
	{
	if (ZQ<unsigned long> theQ = spGetNumber_T<unsigned long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned long> theQ = spGetNumber_T<unsigned long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
ZQ<long long> ZVal_CFType::QGet<long long>() const
	{
	if (ZQ<long long> theQ = spGetNumber_T<long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<long long> theQ = spGetNumber_T<long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned long long> ZVal_CFType::QGet<unsigned long long>() const
	{
	if (ZQ<unsigned long long> theQ = spGetNumber_T<unsigned long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<unsigned long long> theQ = spGetNumber_T<unsigned long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
ZQ<bool> ZVal_CFType::QGet<bool>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		return !!::CFBooleanGetValue(this->StaticCast<CFBooleanRef>());
	return null;
	}

template <>
ZQ<float> ZVal_CFType::QGet<float>() const
	{
	if (ZQ<float> theQ = spGetNumber_T<float>(*this, kCFNumberFloat32Type))
		return theQ;
	if (ZQ<float> theQ = spGetNumber_T<float>(*this, kCFNumberFloatType))
		return theQ;
	return null;
	}

template <>
ZQ<double> ZVal_CFType::QGet<double>() const
	{
	if (ZQ<double> theQ = spGetNumber_T<double>(*this, kCFNumberFloat64Type))
		return theQ;
	if (ZQ<double> theQ = spGetNumber_T<double>(*this, kCFNumberDoubleType))
		return theQ;
	return null;
	}

template <>
ZQ<string8> ZVal_CFType::QGet<string8>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return sAsUTF8(this->StaticCast<CFStringRef>());
	return null;
	}

template <>
ZQ<ZRef<CFStringRef> > ZVal_CFType::QGet<ZRef<CFStringRef> >() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return this->StaticCast<CFStringRef>();
	return null;
	}

template <>
ZQ<ZData_CFType> ZVal_CFType::QGet<ZData_CFType>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDataGetTypeID())
		return this->StaticCast<CFDataRef>();
	return null;
	}

template <>
ZQ<ZSeq_CFType> ZVal_CFType::QGet<ZSeq_CFType>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		return this->StaticCast<CFArrayRef>();
	return null;
	}

template <>
ZQ<ZMap_CFType> ZVal_CFType::QGet<ZMap_CFType>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		return this->StaticCast<CFDictionaryRef>();
	return null;
	}

template <>
void ZVal_CFType::Set<int8>(const int8& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void ZVal_CFType::Set<int16>(const int16& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void ZVal_CFType::Set<int32>(const int32& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void ZVal_CFType::Set<int64>(const int64& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void ZVal_CFType::Set<bool>(const bool& iVal)
	{ inherited::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void ZVal_CFType::Set<float>(const float& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void ZVal_CFType::Set<double>(const double& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat64Type, iVal)); }

template <>
void ZVal_CFType::Set<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void ZVal_CFType::Set<ZRef<CFStringRef> >(const ZRef<CFStringRef>& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set<ZData_CFType>(const ZData_CFType& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set<ZSeq_CFType>(const ZSeq_CFType& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CFType::Set<ZMap_CFType>(const ZMap_CFType& iVal)
	{ inherited::operator=(iVal); }

ZVal_CFType ZVal_CFType::Get(const string8& iName) const
	{ return this->GetMap().Get(iName); }

ZVal_CFType ZVal_CFType::Get(CFStringRef iName) const
	{ return this->GetMap().Get(iName); }

ZVal_CFType ZVal_CFType::Get(size_t iIndex) const
	{ return this->GetSeq().Get(iIndex); }

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

ZQ<ZVal_CFType> ZSeq_CFType::QGet(size_t iIndex) const
	{
	if (CFArrayRef theArray = this->pArray())
		{
		if (size_t theCount = ::CFArrayGetCount(theArray))
			{
			if (iIndex < theCount)
				return ::CFArrayGetValueAtIndex(theArray, iIndex);
			}
		}
	return null;
	}

ZVal_CFType ZSeq_CFType::DGet(const ZVal_CFType& iDefault, size_t iIndex) const
	{
	if (ZQ<ZVal_CFType> theQ = this->QGet(iIndex))
		return theQ.Get();
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

ZQ<ZVal_CFType> ZMap_CFType::QGet(const string8& iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, sString(iName)))
			return theVal;
		}
	return null;
	}

ZQ<ZVal_CFType> ZMap_CFType::QGet(CFStringRef iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, iName))
			return theVal;
		}
	return null;
	}

ZVal_CFType ZMap_CFType::DGet(const ZVal_CFType& iDefault, const string8& iName) const
	{
	if (ZQ<ZVal_CFType> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_CFType ZMap_CFType::DGet(const ZVal_CFType& iDefault, CFStringRef iName) const
	{
	if (ZQ<ZVal_CFType> theQ = this->QGet(iName))
		return theQ.Get();
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

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(CFType)
