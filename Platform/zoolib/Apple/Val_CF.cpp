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

#include "zoolib/Apple/Val_CF.h"

#if ZCONFIG_SPI_Enabled(CFType)

#include "zoolib/Compare_T.h"

#include "zoolib/Apple/Util_CF.h"

#include ZMACINCLUDE2(CoreFoundation,CFArray.h)
#include ZMACINCLUDE2(CoreFoundation,CFDictionary.h)
#include ZMACINCLUDE2(CoreFoundation,CFNumber.h)
#include ZMACINCLUDE2(CoreFoundation,CFString.h)

namespace ZooLib {

using Util_CF::sAsUTF8;
using Util_CF::sString;
using Util_CF::sStringMutable;
using Util_CF::sArrayMutable;
using Util_CF::sDictionaryMutable;

#if 0
// Not usable till I figure out compare for CFTypeRef
template <>
int sCompare_T(const Val_CF& iL, const Val_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const Seq_CF& iL, const Seq_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }

template <>
int sCompare_T(const Map_CF& iL, const Map_CF& iR)
	{ return sCompare_T(ZRef<CFTypeRef>(iL), ZRef<CFTypeRef>(iR)); }
#endif

// =================================================================================================
#pragma mark -
#pragma mark Helpers

namespace {

template <class S>
ZQ<S> spQGetNumber_T(CFTypeRef iTypeRef, CFNumberType iNumberType)
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
#pragma mark Val_CF

/**
\class Val_CF
\ingroup ZVal

\brief Compatible with ZVal & CFTypeRef
*/

Val_CF::Val_CF()
	{}

Val_CF::Val_CF(const Val_CF& iOther)
:	inherited(iOther)
	{}

Val_CF::~Val_CF()
	{}

Val_CF& Val_CF::operator=(const Val_CF& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Val_CF::Val_CF(char iVal)
:	inherited(spNumber_T(kCFNumberCharType, iVal))
	{}

Val_CF::Val_CF(unsigned char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

Val_CF::Val_CF(signed char iVal)
:	inherited(spNumber_T(kCFNumberSInt8Type, iVal))
	{}

Val_CF::Val_CF(wchar_t iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

Val_CF::Val_CF(short iVal)
:	inherited(spNumber_T(kCFNumberShortType, iVal))
	{}

Val_CF::Val_CF(unsigned short iVal)
:	inherited(spNumber_T(kCFNumberSInt16Type, iVal))
	{}

Val_CF::Val_CF(int iVal)
:	inherited(spNumber_T(kCFNumberIntType, iVal))
	{}

Val_CF::Val_CF(unsigned int iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

Val_CF::Val_CF(long iVal)
:	inherited(spNumber_T(kCFNumberLongType, iVal))
	{}

Val_CF::Val_CF(unsigned long iVal)
:	inherited(spNumber_T(kCFNumberSInt32Type, iVal))
	{}

Val_CF::Val_CF(long long iVal)
:	inherited(spNumber_T(kCFNumberLongLongType, iVal))
	{}

Val_CF::Val_CF(unsigned long long iVal)
:	inherited(spNumber_T(kCFNumberSInt64Type, iVal))
	{}

Val_CF::Val_CF(bool iVal)
:	inherited(iVal ? kCFBooleanTrue : kCFBooleanFalse)
	{}

Val_CF::Val_CF(float iVal)
:	inherited(spNumber_T(kCFNumberFloat32Type, iVal))
	{}

Val_CF::Val_CF(double iVal)
:	inherited(spNumber_T(kCFNumberFloat64Type, iVal))
	{}

Val_CF::Val_CF(long double iVal)
:	inherited(spNumber_T(kCFNumberFloat64Type, iVal))
	{}

Val_CF::Val_CF(const char* iVal)
:	inherited(sString(iVal))
	{}

Val_CF::Val_CF(const string8& iVal)
:	inherited(sString(iVal))
	{}

Val_CF::Val_CF(const string16& iVal)
:	inherited(sString(iVal))
	{}

Val_CF::Val_CF(CFStringRef iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(const Data_CF& iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(const Seq_CF& iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(const Map_CF& iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(CFDataRef iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(CFArrayRef iVal)
:	inherited(iVal)
	{}

Val_CF::Val_CF(CFDictionaryRef iVal)
:	inherited(iVal)
	{}

Val_CF& Val_CF::operator=(CFTypeRef iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

template <>
const ZQ<char> Val_CF::QGet<char>() const
	{
	if (ZQ<char> theQ = spQGetNumber_T<char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<char> theQ = spQGetNumber_T<char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
const ZQ<unsigned char> Val_CF::QGet<unsigned char>() const
	{
	if (ZQ<unsigned char> theQ = spQGetNumber_T<unsigned char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<unsigned char> theQ = spQGetNumber_T<unsigned char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
const ZQ<signed char> Val_CF::QGet<signed char>() const
	{
	if (ZQ<signed char> theQ = spQGetNumber_T<signed char>(*this, kCFNumberSInt8Type))
		return theQ;
	if (ZQ<signed char> theQ = spQGetNumber_T<signed char>(*this, kCFNumberCharType))
		return theQ;
	return null;
	}

template <>
const ZQ<short> Val_CF::QGet<short>() const
	{
	if (ZQ<short> theQ = spQGetNumber_T<short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<short> theQ = spQGetNumber_T<short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
const ZQ<unsigned short> Val_CF::QGet<unsigned short>() const
	{
	if (ZQ<unsigned short> theQ = spQGetNumber_T<unsigned short>(*this, kCFNumberSInt16Type))
		return theQ;
	if (ZQ<unsigned short> theQ = spQGetNumber_T<unsigned short>(*this, kCFNumberShortType))
		return theQ;
	return null;
	}

template <>
const ZQ<int> Val_CF::QGet<int>() const
	{
	if (ZQ<int> theQ = spQGetNumber_T<int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<int> theQ = spQGetNumber_T<int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
const ZQ<unsigned int> Val_CF::QGet<unsigned int>() const
	{
	if (ZQ<unsigned int> theQ = spQGetNumber_T<unsigned int>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned int> theQ = spQGetNumber_T<unsigned int>(*this, kCFNumberIntType))
		return theQ;
	return null;
	}

template <>
const ZQ<long> Val_CF::QGet<long>() const
	{
	if (ZQ<long> theQ = spQGetNumber_T<long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<long> theQ = spQGetNumber_T<long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
const ZQ<unsigned long> Val_CF::QGet<unsigned long>() const
	{
	if (ZQ<unsigned long> theQ = spQGetNumber_T<unsigned long>(*this, kCFNumberSInt32Type))
		return theQ;
	if (ZQ<unsigned long> theQ = spQGetNumber_T<unsigned long>(*this, kCFNumberLongType))
		return theQ;
	return null;
	}

template <>
const ZQ<long long> Val_CF::QGet<long long>() const
	{
	if (ZQ<long long> theQ = spQGetNumber_T<long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<long long> theQ = spQGetNumber_T<long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
const ZQ<unsigned long long> Val_CF::QGet<unsigned long long>() const
	{
	if (ZQ<unsigned long long> theQ = spQGetNumber_T<unsigned long long>(*this, kCFNumberSInt64Type))
		return theQ;
	if (ZQ<unsigned long long> theQ = spQGetNumber_T<unsigned long long>(*this, kCFNumberLongLongType))
		return theQ;
	return null;
	}

template <>
const ZQ<bool> Val_CF::QGet<bool>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFBooleanGetTypeID())
		return !!::CFBooleanGetValue(this->StaticCast<CFBooleanRef>());
	return null;
	}

template <>
const ZQ<float> Val_CF::QGet<float>() const
	{
	if (ZQ<float> theQ = spQGetNumber_T<float>(*this, kCFNumberFloat32Type))
		return theQ;
	if (ZQ<float> theQ = spQGetNumber_T<float>(*this, kCFNumberFloatType))
		return theQ;
	return null;
	}

template <>
const ZQ<double> Val_CF::QGet<double>() const
	{
	if (ZQ<double> theQ = spQGetNumber_T<double>(*this, kCFNumberFloat64Type))
		return theQ;
	if (ZQ<double> theQ = spQGetNumber_T<double>(*this, kCFNumberDoubleType))
		return theQ;
	return null;
	}

template <>
const ZQ<string8> Val_CF::QGet<string8>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return sAsUTF8(this->StaticCast<CFStringRef>());
	return null;
	}

template <>
const ZQ<ZRef<CFStringRef> > Val_CF::QGet<ZRef<CFStringRef> >() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFStringGetTypeID())
		return this->StaticCast<CFStringRef>();
	return null;
	}

template <>
const ZQ<Data_CF> Val_CF::QGet<Data_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDataGetTypeID())
		return this->StaticCast<CFDataRef>();
	return null;
	}

template <>
const ZQ<Seq_CF> Val_CF::QGet<Seq_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFArrayGetTypeID())
		return this->StaticCast<CFArrayRef>();
	return null;
	}

template <>
const ZQ<Map_CF> Val_CF::QGet<Map_CF>() const
	{
	if (*this && ::CFGetTypeID(*this) == ::CFDictionaryGetTypeID())
		return this->StaticCast<CFDictionaryRef>();
	return null;
	}

template <>
void Val_CF::Set<int8>(const int8& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt8Type, iVal)); }

template <>
void Val_CF::Set<int16>(const int16& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt16Type, iVal)); }

template <>
void Val_CF::Set<int32>(const int32& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt32Type, iVal)); }

template <>
void Val_CF::Set<int64>(const int64& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberSInt64Type, iVal)); }

template <>
void Val_CF::Set<bool>(const bool& iVal)
	{ inherited::operator=(iVal ? kCFBooleanTrue : kCFBooleanFalse); }

template <>
void Val_CF::Set<float>(const float& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat32Type, iVal)); }

template <>
void Val_CF::Set<double>(const double& iVal)
	{ inherited::operator=(spNumber_T(kCFNumberFloat64Type, iVal)); }

template <>
void Val_CF::Set<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void Val_CF::Set<ZRef<CFStringRef> >(const ZRef<CFStringRef>& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_CF::Set<Data_CF>(const Data_CF& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_CF::Set<Seq_CF>(const Seq_CF& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_CF::Set<Map_CF>(const Map_CF& iVal)
	{ inherited::operator=(iVal); }

const Val_CF Val_CF::Get(const string8& iName) const
	{ return this->Get<Map_CF>().Get(iName); }

const Val_CF Val_CF::Get(CFStringRef iName) const
	{ return this->Get<Map_CF>().Get(iName); }

const Val_CF Val_CF::Get(size_t iIndex) const
	{ return this->Get<Seq_CF>().Get(iIndex); }

// =================================================================================================
#pragma mark -
#pragma mark Seq_CF

Seq_CF::Seq_CF()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

Seq_CF::Seq_CF(const Seq_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Seq_CF::~Seq_CF()
	{}

Seq_CF& Seq_CF::operator=(const Seq_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Seq_CF::Seq_CF(CFMutableArrayRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Seq_CF::Seq_CF(CFArrayRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Seq_CF::Seq_CF(const Adopt_T<CFMutableArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Seq_CF::Seq_CF(const Adopt_T<CFArrayRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Seq_CF& Seq_CF::operator=(CFMutableArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Seq_CF& Seq_CF::operator=(CFArrayRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Seq_CF& Seq_CF::operator=(const Adopt_T<CFMutableArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Seq_CF& Seq_CF::operator=(const Adopt_T<CFArrayRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t Seq_CF::Size() const
	{
	if (CFArrayRef theArray = this->pArray())
		return ::CFArrayGetCount(theArray);
	return 0;
	}

const ZQ<Val_CF> Seq_CF::QGet(size_t iIndex) const
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

const Val_CF Seq_CF::DGet(const Val_CF& iDefault, size_t iIndex) const
	{
	if (ZQ<Val_CF> theQ = this->QGet(iIndex))
		return *theQ;
	return iDefault;
	}

const Val_CF Seq_CF::Get(size_t iIndex) const
	{ return this->DGet(Val_CF(), iIndex); }

Seq_CF& Seq_CF::Set(size_t iIndex, const Val_CF& iVal)
	{
	::CFArraySetValueAtIndex(this->pTouch(), iIndex, iVal);
	return *this;
	}

Seq_CF& Seq_CF::Erase(size_t iIndex)
	{
	CFMutableArrayRef theArray = this->pTouch();
	if (const size_t theCount = ::CFArrayGetCount(theArray))
		{
		if (iIndex < theCount)
			::CFArrayRemoveValueAtIndex(theArray, iIndex);
		}
	return *this;
	}

Seq_CF& Seq_CF::Insert(size_t iIndex, const Val_CF& iVal)
	{
	CFMutableArrayRef theArray = this->pTouch();
	const size_t theCount = ::CFArrayGetCount(theArray);
	if (iIndex <= theCount)
		::CFArrayInsertValueAtIndex(theArray, iIndex, iVal);
	return *this;
	}

Seq_CF& Seq_CF::Append(const Val_CF& iVal)
	{
	::CFArrayAppendValue(this->pTouch(), iVal);
	return *this;
	}

CFArrayRef Seq_CF::pArray() const
	{ return inherited::Get(); }

CFMutableArrayRef Seq_CF::pTouch()
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
#pragma mark Map_CF

Map_CF::Map_CF()
:	inherited(sDictionaryMutable())
,	fMutable(true)
	{}

Map_CF::Map_CF(const Map_CF& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Map_CF::~Map_CF()
	{}

Map_CF& Map_CF::operator=(const Map_CF& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Map_CF::Map_CF(CFDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Map_CF::Map_CF(CFMutableDictionaryRef iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Map_CF::Map_CF(const Adopt_T<CFDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Map_CF::Map_CF(const Adopt_T<CFMutableDictionaryRef>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Map_CF& Map_CF::operator=(CFMutableDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Map_CF& Map_CF::operator=(CFDictionaryRef iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Map_CF& Map_CF::operator=(const Adopt_T<CFMutableDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Map_CF& Map_CF::operator=(const Adopt_T<CFDictionaryRef>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

const ZQ<Val_CF> Map_CF::QGet(const string8& iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, sString(iName)))
			return theVal;
		}
	return null;
	}

const ZQ<Val_CF> Map_CF::QGet(CFStringRef iName) const
	{
	if (CFDictionaryRef theDictionary = this->pDictionary())
		{
		if (CFTypeRef theVal = ::CFDictionaryGetValue(theDictionary, iName))
			return theVal;
		}
	return null;
	}

const Val_CF Map_CF::DGet(const Val_CF& iDefault, const string8& iName) const
	{
	if (ZQ<Val_CF> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const Val_CF Map_CF::DGet(const Val_CF& iDefault, CFStringRef iName) const
	{
	if (ZQ<Val_CF> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const Val_CF Map_CF::Get(const string8& iName) const
	{ return this->DGet(Val_CF(), iName); }

const Val_CF Map_CF::Get(CFStringRef iName) const
	{ return this->DGet(Val_CF(), iName); }

Map_CF& Map_CF::Set(const string8& iName, const Val_CF& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), sString(iName), iVal);
	return *this;
	}

Map_CF& Map_CF::Set(CFStringRef iName, const Val_CF& iVal)
	{
	::CFDictionarySetValue(this->pTouch(), iName, iVal);
	return *this;
	}

Map_CF& Map_CF::Erase(const string8& iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), sString(iName));
	return *this;
	}

Map_CF& Map_CF::Erase(CFStringRef iName)
	{
	::CFDictionaryRemoveValue(this->pTouch(), iName);
	return *this;
	}

CFDictionaryRef Map_CF::pDictionary() const
	{ return inherited::Get(); }

CFMutableDictionaryRef Map_CF::pTouch()
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
