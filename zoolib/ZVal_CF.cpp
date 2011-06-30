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

#include "zoolib/ZVal_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/ZCompare_T.h"
#include "zoolib/ZUtil_CF.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

namespace ZooLib {

using ZUtil_CF::sAsUTF8;
using ZUtil_CF::sString;
using ZUtil_CF::sStringMutable;
using ZUtil_CF::sArrayMutable;
using ZUtil_CF::sDictionaryMutable;

#if 0
// Not usable till I figure out compare for CFTypeRef
template <>
int sCompare_T(const ZVal_CF& iL, const ZVal_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const ZSeq_CF& iL, const ZSeq_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const ZMap_CF& iL, const ZMap_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }
#endif

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
#pragma mark * ZVal_CF

/**
\class ZVal_CF
\ingroup ZVal

\brief Compatible with ZVal & CFTypeRef
*/

ZAny ZVal_CF::AsAny() const
	{ return this->DAsAny(ZAny()); }

ZAny ZVal_CF::DAsAny(const ZAny& iDefault) const
	{ return ZUtil_CF::sDAsAny(ZAny(), *this); }

ZVal_CF::operator bool() const
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

ZVal_CF::ZVal_CF()
	{}

ZVal_CF::ZVal_CF(const ZVal_CF& iOther)
:	inherited(iOther)
	{}

ZVal_CF::~ZVal_CF()
	{}

ZVal_CF& ZVal_CF::operator=(const ZVal_CF& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZVal_CF::ZVal_CF(char iVal)
:	inherited(spNumber_T(kCFNumberCharType, iVal))
	{}

ZVal_CF::ZVal_CF(unsigned char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

ZVal_CF::ZVal_CF(signed char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

ZVal_CF::ZVal_CF(wchar_t iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CF::ZVal_CF(short iVal)
:	inherited(spNumber_T(kCFNumberShortType, iVal))
	{}

ZVal_CF::ZVal_CF(unsigned short iVal)
:	inherited(spNumber_T(kCFNumberSInt16Type, iVal))
	{}

ZVal_CF::ZVal_CF(int iVal)
:	inherited(spNumber_T(kCFNumberIntType, iVal))
	{}

ZVal_CF::ZVal_CF(unsigned int iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CF::ZVal_CF(long iVal)
:	inherited(spNumber_T(kCFNumberLongType, iVal))
	{}

ZVal_CF::ZVal_CF(unsigned long iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

ZVal_CF::ZVal_CF(long long iVal)
:	inherited(spNumber_T(kCFNumberLongLongType, iVal))
	{}

ZVal_CF::ZVal_CF(unsigned long long iVal)
:	inherited(spNumber_T(kCFNumberSInt64Type, iVal))
	{}

ZVal_CF::ZVal_CF(bool iVal)
:	inherited(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

ZVal_CF::ZVal_CF(float iVal)
:	inherited(spNumber_T(kCFNumberFloat32Type, iVal))
	{}

ZVal_CF::ZVal_CF(double iVal)
:	inherited(spNumber_T(kCFNumberFloat64Type, iVal))
	{}

ZVal_CF::ZVal_CF(const char* iVal)
:	inherited(sString(iVal))
	{}

ZVal_CF::ZVal_CF(const string8& iVal)
:	inherited(sString(iVal))
	{}

ZVal_CF::ZVal_CF(const string16& iVal)
:	inherited(sString(iVal))
	{}

ZVal_CF::ZVal_CF(CFStringRef iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(const ZData_CF& iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(const ZSeq_CF& iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(const ZMap_CF& iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(CFDataRef iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(CFArrayRef iVal)
:	inherited(iVal)
	{}

ZVal_CF::ZVal_CF(CFDictionaryRef iVal)
:	inherited(iVal)
	{}

ZVal_CF& ZVal_CF::operator=(CFTypeRef iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

template <>
ZQ<char> ZVal_CF::QGet<char>() const
	{
	if (ZQ<char> theQ = spGetNumber_T<char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<char> theQ = spGetNumber_T<char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned char> ZVal_CF::QGet<unsigned char>() const
	{
	if (ZQ<unsigned char> theQ = spGetNumber_T<unsigned char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<unsigned char> theQ = spGetNumber_T<unsigned char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<signed char> ZVal_CF::QGet<signed char>() const
	{
	if (ZQ<signed char> theQ = spGetNumber_T<signed char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<signed char> theQ = spGetNumber_T<signed char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
ZQ<short> ZVal_CF::QGet<short>() const
	{
	if (ZQ<short> theQ = spGetNumber_T<short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<short> theQ = spGetNumber_T<short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned short> ZVal_CF::QGet<unsigned short>() const
	{
	if (ZQ<unsigned short> theQ = spGetNumber_T<unsigned short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<unsigned short> theQ = spGetNumber_T<unsigned short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
ZQ<int> ZVal_CF::QGet<int>() const
	{
	if (ZQ<int> theQ = spGetNumber_T<int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<int> theQ = spGetNumber_T<int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned int> ZVal_CF::QGet<unsigned int>() const
	{
	if (ZQ<unsigned int> theQ = spGetNumber_T<unsigned int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned int> theQ = spGetNumber_T<unsigned int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
ZQ<long> ZVal_CF::QGet<long>() const
	{
	if (ZQ<long> theQ = spGetNumber_T<long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<long> theQ = spGetNumber_T<long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned long> ZVal_CF::QGet<unsigned long>() const
	{
	if (ZQ<unsigned long> theQ = spGetNumber_T<unsigned long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned long> theQ = spGetNumber_T<unsigned long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
ZQ<long long> ZVal_CF::QGet<long long>() const
	{
	if (ZQ<long long> theQ = spGetNumber_T<long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<long long> theQ = spGetNumber_T<long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
ZQ<unsigned long long> ZVal_CF::QGet<unsigned long long>() const
	{
	if (ZQ<unsigned long long> theQ = spGetNumber_T<unsigned long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<unsigned long long> theQ = spGetNumber_T<unsigned long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
ZQ<bool> ZVal_CF::QGet<bool>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		return !!::CFBooleanGetValue(this->StaticCast<CFBooleanRef>());
	return null;
	}

template <>
ZQ<float> ZVal_CF::QGet<float>() const
	{
	if (ZQ<float> theQ = spGetNumber_T<float>(*this, kCFNumberFloat32Type))
		return theQ;
	if (ZQ<float> theQ = spGetNumber_T<float>(*this, kCFNumberFloatType))
		return theQ;
	return null;
	}

template <>
ZQ<double> ZVal_CF::QGet<double>() const
	{
	if (ZQ<double> theQ = spGetNumber_T<double>(*this, kCFNumberFloat64Type))
		return theQ;
	if (ZQ<double> theQ = spGetNumber_T<double>(*this, kCFNumberDoubleType))
		return theQ;
	return null;
	}

template <>
ZQ<string8> ZVal_CF::QGet<string8>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return sAsUTF8(this->StaticCast<CFStringRef>());
	return null;
	}

template <>
ZQ<ZRef<CFStringRef> > ZVal_CF::QGet<ZRef<CFStringRef> >() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return this->StaticCast<CFStringRef>();
	return null;
	}

template <>
ZQ<ZData_CF> ZVal_CF::QGet<ZData_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDataGetTypeID())
		return this->StaticCast<CFDataRef>();
	return null;
	}

template <>
ZQ<ZSeq_CF> ZVal_CF::QGet<ZSeq_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		return this->StaticCast<CFArrayRef>();
	return null;
	}

template <>
ZQ<ZMap_CF> ZVal_CF::QGet<ZMap_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		return this->StaticCast<CFDictionaryRef>();
	return null;
	}

template <>
void ZVal_CF::Set<int8>(const int8& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void ZVal_CF::Set<int16>(const int16& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void ZVal_CF::Set<int32>(const int32& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void ZVal_CF::Set<int64>(const int64& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void ZVal_CF::Set<bool>(const bool& iVal)
	{ inherited::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void ZVal_CF::Set<float>(const float& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void ZVal_CF::Set<double>(const double& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat64Type, iVal)); }

template <>
void ZVal_CF::Set<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void ZVal_CF::Set<ZRef<CFStringRef> >(const ZRef<CFStringRef>& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CF::Set<ZData_CF>(const ZData_CF& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CF::Set<ZSeq_CF>(const ZSeq_CF& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_CF::Set<ZMap_CF>(const ZMap_CF& iVal)
	{ inherited::operator=(iVal); }

ZVal_CF ZVal_CF::Get(const string8& iName) const
	{ return this->GetMap().Get(iName); }

ZVal_CF ZVal_CF::Get(CFStringRef iName) const
	{ return this->GetMap().Get(iName); }

ZVal_CF ZVal_CF::Get(size_t iIndex) const
	{ return this->GetSeq().Get(iIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_CF typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_CF)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CF, CFString, ZRef<CFStringRef>)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CF, Data, ZData_CF)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CF, Seq, ZSeq_CF)
ZMACRO_ZValAccessors_Def_Entry(ZVal_CF, Map, ZMap_CF)

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_CF

ZSeq_Any ZSeq_CF::AsSeq_Any(const ZAny& iDefault) const
	{ return ZUtil_CF::sAsSeq_Any(iDefault, this->pArray()); }

ZSeq_CF::operator bool() const
	{ return this->Count(); }

ZSeq_CF::ZSeq_CF()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

ZSeq_CF::ZSeq_CF(const ZSeq_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZSeq_CF::~ZSeq_CF()
	{}

ZSeq_CF& ZSeq_CF::operator=(const ZSeq_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZSeq_CF::ZSeq_CF(CFMutableArrayRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZSeq_CF::ZSeq_CF(CFArrayRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_CF::ZSeq_CF(const Adopt_T<CFMutableArrayRef>& iOther)
:	inherited(ZRef<CFMutableArrayRef>(iOther))
,	fMutable(true)
	{}

ZSeq_CF::ZSeq_CF(const Adopt_T<CFArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_CF& ZSeq_CF::operator=(CFMutableArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZSeq_CF& ZSeq_CF::operator=(CFArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZSeq_CF& ZSeq_CF::operator=(const Adopt_T<CFMutableArrayRef>& iOther)
	{
	inherited::operator=(ZRef<CFMutableArrayRef>(iOther));
	fMutable = true;
	return *this;
	}

ZSeq_CF& ZSeq_CF::operator=(const Adopt_T<CFArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t ZSeq_CF::Count() const
	{
	if (CFArrayRef theArray = this->pArray())
		return ::CFArrayGetCount(theArray);
	return 0;
	}

ZQ<ZVal_CF> ZSeq_CF::QGet(size_t iIndex) const
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

ZVal_CF ZSeq_CF::DGet(const ZVal_CF& iDefault, size_t iIndex) const
	{
	if (ZQ<ZVal_CF> theQ = this->QGet(iIndex))
		return theQ.Get();
	return iDefault;
	}

ZVal_CF ZSeq_CF::Get(size_t iIndex) const
	{ return this->DGet(ZVal_CF(), iIndex); }

ZSeq_CF& ZSeq_CF::Set(size_t iIndex, const ZVal_CF& iVal)
	{
	::CFArraySetValueAtIndex(this->pTouch(), iIndex, iVal);
	return *this;
	}

ZSeq_CF& ZSeq_CF::Erase(size_t iIndex)
	{
	CFMutableArrayRef theArray = this->pTouch();
	if (const size_t theCount = ::CFArrayGetCount(theArray))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(theArray, iIndex);
		}
	return *this;
	}

ZSeq_CF& ZSeq_CF::Insert(size_t iIndex, const ZVal_CF& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	const size_t theCount = ::CFArrayGetCount(theArray);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(theArray, iIndex, iVal);
	return *this;
	}

ZSeq_CF& ZSeq_CF::Append(const ZVal_CF& iVal)
	{
	::CFArrayAppendValue(this->pTouch(), iVal);
	return *this;
	}

CFArrayRef ZSeq_CF::pArray() const
	{ return inherited::Get(); }

CFMutableArrayRef ZSeq_CF::pTouch()
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
#pragma mark * ZMap_CF

ZMap_Any ZMap_CF::AsMap_Any(const ZAny& iDefault) const
	{ return ZUtil_CF::sAsMap_Any(iDefault, this->pDictionary()); }

ZMap_CF::operator bool() const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		return ::CFDictionaryGetCount(theDictionary);
	return false;
	}

ZMap_CF::ZMap_CF()
:	inherited(sDictionaryMutable())
,	fMutable(true)
	{}

ZMap_CF::ZMap_CF(const ZMap_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZMap_CF::~ZMap_CF()
	{}

ZMap_CF& ZMap_CF::operator=(const ZMap_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZMap_CF::ZMap_CF(CFDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CF::ZMap_CF(CFMutableDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_CF::ZMap_CF(const Adopt_T<CFDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_CF::ZMap_CF(const Adopt_T<CFMutableDictionaryRef>& iOther)
:	inherited(ZRef<CFMutableDictionaryRef>(iOther))
,	fMutable(true)
	{}

ZMap_CF& ZMap_CF::operator=(CFMutableDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZMap_CF& ZMap_CF::operator=(CFDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZMap_CF& ZMap_CF::operator=(const Adopt_T<CFMutableDictionaryRef>& iOther)
	{
	inherited::operator=(ZRef<CFMutableDictionaryRef>(iOther));
	fMutable = true;
	return *this;
	}

ZMap_CF& ZMap_CF::operator=(const Adopt_T<CFDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZQ<ZVal_CF> ZMap_CF::QGet(const string8& iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, sString(iName)))
			return theVal;
		}
	return null;
	}

ZQ<ZVal_CF> ZMap_CF::QGet(CFStringRef iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, iName))
			return theVal;
		}
	return null;
	}

ZVal_CF ZMap_CF::DGet(const ZVal_CF& iDefault, const string8& iName) const
	{
	if (ZQ<ZVal_CF> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_CF ZMap_CF::DGet(const ZVal_CF& iDefault, CFStringRef iName) const
	{
	if (ZQ<ZVal_CF> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_CF ZMap_CF::Get(const string8& iName) const
	{ return this->DGet(ZVal_CF(), iName); }

ZVal_CF ZMap_CF::Get(CFStringRef iName) const
	{ return this->DGet(ZVal_CF(), iName); }

ZMap_CF& ZMap_CF::Set(const string8& iName, const ZVal_CF& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), sString(iName), iVal);
	return *this;
	}

ZMap_CF& ZMap_CF::Set(CFStringRef iName, const ZVal_CF& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), iName, iVal);
	return *this;
	}

ZMap_CF& ZMap_CF::Erase(const string8& iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), sString(iName));
	return *this;
	}

ZMap_CF& ZMap_CF::Erase(CFStringRef iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), iName);
	return *this;
	}

CFDictionaryRef ZMap_CF::pDictionary() const
	{ return inherited::Get(); }

CFMutableDictionaryRef ZMap_CF::pTouch()
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
