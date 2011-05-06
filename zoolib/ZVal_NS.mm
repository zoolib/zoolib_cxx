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

#include "zoolib/ZVal_NS.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/ZUtil_NSObject.h"

namespace ZooLib {

namespace ZUtil_NS = ZUtil_NSObject;

using std::type_info;

using ZUtil_NS::sAsUTF8;
using ZUtil_NS::sString;
using ZUtil_NS::sStringMutable;
using ZUtil_NS::sArrayMutable;
using ZUtil_NS::sDictionaryMutable;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers (anonymous)

namespace { // anonymous

template <class T>
T* spAs_T(NSObject* iObj)
	{
	if ([iObj isKindOfClass:[T class]])
		return static_cast<T*>(iObj);
	return nullptr;
	}

NSNumber* spAsNumber(NSObject* iObj)
	{ return spAs_T<NSNumber>(iObj); }

} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_NS

/**
\class ZVal_NS
\ingroup ZVal

\brief Compatible with ZVal & NSObject
*/

ZAny ZVal_NS::AsAny() const
	{ return this->DAsAny(ZAny()); }

ZAny ZVal_NS::DAsAny(const ZAny& iDefault) const
	{ return ZUtil_NS::sDAsAny(iDefault, inherited::Get()); }

ZVal_NS::operator bool() const
	{
	if (NSObject* theVal = inherited::Get())
		{
//		#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
//			&& MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
			if ([theVal isMemberOfClass:[NSNull class]])
				return false;
//		#endif
		return true;
		}
	return false;
	}

ZVal_NS::ZVal_NS()
	{}

ZVal_NS::ZVal_NS(const ZVal_NS& iOther)
:	inherited(iOther)
	{}

ZVal_NS::~ZVal_NS()
	{}

ZVal_NS& ZVal_NS::operator=(const ZVal_NS& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

ZVal_NS::ZVal_NS(char iVal)
:	inherited([NSNumber numberWithChar:iVal])
	{}

ZVal_NS::ZVal_NS(signed char iVal)
:	inherited([NSNumber numberWithChar:iVal])
	{}

ZVal_NS::ZVal_NS(unsigned char iVal)
:	inherited([NSNumber numberWithUnsignedChar:iVal])
	{}

ZVal_NS::ZVal_NS(wchar_t iVal)
:	inherited([NSNumber numberWithUnsignedInt:iVal])
	{}

ZVal_NS::ZVal_NS(short iVal)
:	inherited([NSNumber numberWithShort:iVal])
	{}

ZVal_NS::ZVal_NS(unsigned short iVal)
:	inherited([NSNumber numberWithUnsignedShort:iVal])
	{}

ZVal_NS::ZVal_NS(int iVal)
:	inherited([NSNumber numberWithInt:iVal])
	{}

ZVal_NS::ZVal_NS(unsigned int iVal)
:	inherited([NSNumber numberWithUnsignedInt:iVal])
	{}

ZVal_NS::ZVal_NS(long iVal)
:	inherited([NSNumber numberWithLong:iVal])
	{}

ZVal_NS::ZVal_NS(unsigned long iVal)
:	inherited([NSNumber numberWithUnsignedLong:iVal])
	{}

ZVal_NS::ZVal_NS(long long iVal)
:	inherited([NSNumber numberWithLongLong:iVal])
	{}

ZVal_NS::ZVal_NS(unsigned long long iVal)
:	inherited([NSNumber numberWithLongLong:iVal])
	{}

ZVal_NS::ZVal_NS(bool iVal)
:	inherited([NSNumber numberWithBool:iVal ? YES : NO])
	{}

ZVal_NS::ZVal_NS(float iVal)
:	inherited([NSNumber numberWithFloat:iVal])
	{}

ZVal_NS::ZVal_NS(double iVal)
:	inherited([NSNumber numberWithDouble:iVal])
	{}

ZVal_NS::ZVal_NS(const char* iVal)
:	inherited(sString(iVal))
	{}

ZVal_NS::ZVal_NS(const string8& iVal)
:	inherited(sString(iVal))
	{}

ZVal_NS::ZVal_NS(const string16& iVal)
:	inherited(sString(iVal))
	{}

ZVal_NS::ZVal_NS(CFStringRef iVal)
:	inherited((NSObject*)iVal)
	{}

template <>
ZQ<char> ZVal_NS::QGet<char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber charValue];
	return null;
	}

template <>
ZQ<unsigned char> ZVal_NS::QGet<unsigned char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedCharValue];
	return null;
	}

template <>
ZQ<signed char> ZVal_NS::QGet<signed char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber charValue];
	return null;
	}

template <>
ZQ<short> ZVal_NS::QGet<short>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber shortValue];
	return null;
	}

template <>
ZQ<unsigned short> ZVal_NS::QGet<unsigned short>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedShortValue];
	return null;
	}

template <>
ZQ<int> ZVal_NS::QGet<int>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber intValue];
	return null;
	}

template <>
ZQ<unsigned int> ZVal_NS::QGet<unsigned int>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedIntValue];
	return null;
	}

template <>
ZQ<long> ZVal_NS::QGet<long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber longValue];
	return null;
	}

template <>
ZQ<unsigned long> ZVal_NS::QGet<unsigned long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedLongValue];
	return null;
	}

template <>
ZQ<long long> ZVal_NS::QGet<long long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber longLongValue];
	return null;
	}

template <>
ZQ<unsigned long long> ZVal_NS::QGet<unsigned long long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedLongLongValue];
	return null;
	}

template <>
ZQ<bool> ZVal_NS::QGet<bool>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber boolValue];
	return null;
	}

template <>
ZQ<float> ZVal_NS::QGet<float>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber floatValue];
	return null;
	}

template <>
ZQ<double> ZVal_NS::QGet<double>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber doubleValue];
	return null;
	}

template <>
ZQ<string8> ZVal_NS::QGet<string8>() const
	{
	if (NSString* asString = spAs_T<NSString>(inherited::Get()))
		return sAsUTF8(asString);
	return null;
	}

template <>
ZQ<ZVal_NS::NSStringPtr> ZVal_NS::QGet<ZVal_NS::NSStringPtr>() const
	{
	if (NSString* asString = spAs_T<NSString>(inherited::Get()))
		return asString;
	return null;
	}

template <>
ZQ<ZData_NS> ZVal_NS::QGet<ZData_NS>() const
	{
	if (NSData* asData = spAs_T<NSData>(inherited::Get()))
		return asData;
	return null;
	}

template <>
ZQ<ZSeq_NS> ZVal_NS::QGet<ZSeq_NS>() const
	{
	if (NSArray* asArray = spAs_T<NSArray>(inherited::Get()))
		return asArray;
	return null;
	}

template <>
ZQ<ZMap_NS> ZVal_NS::QGet<ZMap_NS>() const
	{
	if (NSDictionary* asDictionary = spAs_T<NSDictionary>(inherited::Get()))
		return asDictionary;
	return null;
	}

template <>
void ZVal_NS::Set<int8>(const int8& iVal)
	{ inherited::operator=([NSNumber numberWithChar:iVal]); }

template <>
void ZVal_NS::Set<int16>(const int16& iVal)
	{ inherited::operator=([NSNumber numberWithShort:iVal]); }

template <>
void ZVal_NS::Set<int32>(const int32& iVal)
	{ inherited::operator=([NSNumber numberWithInt:iVal]); }

template <>
void ZVal_NS::Set<int64>(const int64& iVal)
	{ inherited::operator=([NSNumber numberWithLongLong:iVal]); }

template <>
void ZVal_NS::Set<bool>(const bool& iVal)
	{ inherited::operator=([NSNumber numberWithBool:iVal]); }

template <>
void ZVal_NS::Set<float>(const float& iVal)
	{ inherited::operator=([NSNumber numberWithFloat:iVal]); }

template <>
void ZVal_NS::Set<double>(const double& iVal)
	{ inherited::operator=([NSNumber numberWithDouble:iVal]); }

template <>
void ZVal_NS::Set<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void ZVal_NS::Set<ZVal_NS::NSStringPtr>(const ZVal_NS::NSStringPtr& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set<ZData_NS>(const ZData_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set<ZSeq_NS>(const ZSeq_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set<ZMap_NS>(const ZMap_NS& iVal)
	{ inherited::operator=(iVal); }

ZVal_NS ZVal_NS::Get(const string8& iName) const
	{ return this->GetMap().Get(iName); }

ZVal_NS ZVal_NS::Get(NSString* iName) const
	{ return this->GetMap().Get(iName); }

ZVal_NS ZVal_NS::Get(size_t iIndex) const
	{ return this->GetSeq().Get(iIndex); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_NS typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_NS)
ZMACRO_ZValAccessors_Def_Entry(ZVal_NS, NSString, ZVal_NS::NSStringPtr)
ZMACRO_ZValAccessors_Def_Entry(ZVal_NS, Data, ZData_NS)
ZMACRO_ZValAccessors_Def_Entry(ZVal_NS, Seq, ZSeq_NS)
ZMACRO_ZValAccessors_Def_Entry(ZVal_NS, Map, ZMap_NS)

// =================================================================================================
#pragma mark -
#pragma mark * ZSeq_NS

ZSeq_NS::operator bool() const
	{ return this->Count(); }

ZSeq_NS::ZSeq_NS()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

ZSeq_NS::ZSeq_NS(const ZSeq_NS& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZSeq_NS::~ZSeq_NS()
	{}

ZSeq_NS& ZSeq_NS::operator=(const ZSeq_NS& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZSeq_NS::ZSeq_NS(NSMutableArray* iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZSeq_NS::ZSeq_NS(NSArray* iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_NS::ZSeq_NS(const Adopt_T<NSMutableArray>& iOther)
:	inherited(iOther.Get())
,	fMutable(true)
	{}

ZSeq_NS::ZSeq_NS(const Adopt_T<NSArray>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_NS& ZSeq_NS::operator=(NSMutableArray* iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZSeq_NS& ZSeq_NS::operator=(NSArray* iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZSeq_NS& ZSeq_NS::operator=(const Adopt_T<NSMutableArray>& iOther)
	{
	inherited::operator=(iOther.Get());
	fMutable = true;
	return *this;
	}

ZSeq_NS& ZSeq_NS::operator=(const Adopt_T<NSArray>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t ZSeq_NS::Count() const
	{
	if (NSArray* theArray = this->pArray())
		return [theArray count];
	return 0;
	}

ZQ<ZVal_NS> ZSeq_NS::QGet(size_t iIndex) const
	{
	if (NSArray* theArray = this->pArray())
		{
		if (size_t theCount = [theArray count])
			{
			if (iIndex < theCount)
				return ZVal_NS([theArray objectAtIndex:iIndex]);
			}
		}
	return null;
	}

ZVal_NS ZSeq_NS::DGet(const ZVal_NS& iDefault, size_t iIndex) const
	{
	if (ZQ<ZVal_NS> theQ = this->QGet(iIndex))
		return theQ.Get();
	return iDefault;
	}

ZVal_NS ZSeq_NS::Get(size_t iIndex) const
	{ return this->DGet(ZVal_NS(), iIndex); }

ZSeq_NS& ZSeq_NS::Set(size_t iIndex, const ZVal_NS& iVal)
	{
	[this->pTouch() replaceObjectAtIndex:iIndex withObject:iVal.Get()];
	return *this;
	}

ZSeq_NS& ZSeq_NS::Erase(size_t iIndex)
	{
	NSMutableArray* theArray = this->pTouch();
	if (const size_t theCount = [theArray count])
		{
		if (iIndex < theCount)
			[theArray removeObjectAtIndex:iIndex];
		}
	return *this;
	}

ZSeq_NS& ZSeq_NS::Insert(size_t iIndex, const ZVal_NS& iVal)
	{
	NSMutableArray* theArray = this->pTouch();
	const size_t theCount = [theArray count];
	if (iIndex <= theCount)
		[theArray insertObject:iVal.Get() atIndex:iIndex];
	return *this;
	}

ZSeq_NS& ZSeq_NS::Append(const ZVal_NS& iVal)
	{
	[this->pTouch() addObject:iVal.Get()];
	return *this;
	}

NSArray* ZSeq_NS::pArray() const
	{ return inherited::Get(); }

NSMutableArray* ZSeq_NS::pTouch()
	{
	ZRef<NSMutableArray> theMutableArray;
	if (NSArray* theArray = this->pArray())
		{
		if (!fMutable || [theArray retainCount] > 1)
			{
			theMutableArray = sArrayMutable(theArray);
			inherited::operator=(theMutableArray);
			}
		else
			{
			theMutableArray = static_cast<NSMutableArray*>(theArray);
			}
		}
	else
		{
		theMutableArray = sArrayMutable();
		inherited::operator=(theMutableArray);
		}
	fMutable = true;
	return theMutableArray.Get();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_NS

ZMap_NS::operator bool() const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		return [theDictionary count];
	return false;
	}

ZMap_NS::ZMap_NS()
:	inherited(sDictionaryMutable())
,	fMutable(true)
	{}

ZMap_NS::ZMap_NS(const ZMap_NS& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

ZMap_NS::~ZMap_NS()
	{}

ZMap_NS& ZMap_NS::operator=(const ZMap_NS& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

ZMap_NS::ZMap_NS(NSDictionary* iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_NS::ZMap_NS(NSMutableDictionary* iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_NS::ZMap_NS(const Adopt_T<NSDictionary>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_NS::ZMap_NS(const Adopt_T<NSMutableDictionary>& iOther)
:	inherited(iOther.Get())
,	fMutable(true)
	{}

ZMap_NS& ZMap_NS::operator=(NSMutableDictionary* iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(NSDictionary* iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(const Adopt_T<NSMutableDictionary>& iOther)
	{
	inherited::operator=(iOther.Get());
	fMutable = true;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(const Adopt_T<NSDictionary>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZQ<ZVal_NS> ZMap_NS::QGet(const string8& iName) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:sString(iName)])
			return theVal;
		}
	return null;
	}

ZQ<ZVal_NS> ZMap_NS::QGet(NSString* iName) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:iName])
			return theVal;
		}
	return null;
	}

ZVal_NS ZMap_NS::DGet(const ZVal_NS& iDefault, const string8& iName) const
	{
	if (ZQ<ZVal_NS> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_NS ZMap_NS::DGet(const ZVal_NS& iDefault, NSString* iName) const
	{
	if (ZQ<ZVal_NS> theQ = this->QGet(iName))
		return theQ.Get();
	return iDefault;
	}

ZVal_NS ZMap_NS::Get(const string8& iName) const
	{ return this->DGet(ZVal_NS(), iName); }

ZVal_NS ZMap_NS::Get(NSString* iName) const
	{ return this->DGet(ZVal_NS(), iName); }

ZVal_NS ZMap_NS::Get(CFStringRef iName) const
	{ return this->Get((NSString*)iName); }

ZMap_NS& ZMap_NS::Set(const string8& iName, const ZVal_NS& iVal)
	{
	[this->pTouch() setValue:iVal.Get() forKey:sString(iName)];
	return *this;
	}

ZMap_NS& ZMap_NS::Set(NSString* iName, const ZVal_NS& iVal)
	{
	[this->pTouch() setValue:iVal.Get() forKey:iName];
	return *this;
	}

ZMap_NS& ZMap_NS::Set(CFStringRef iName, const ZVal_NS& iVal)
	{ return this->Set((NSString*)iName, iVal); }

ZMap_NS& ZMap_NS::Erase(const string8& iName)
	{
	[this->pTouch() removeObjectForKey:sString(iName)];
	return *this;
	}

ZMap_NS& ZMap_NS::Erase(NSString* iName)
	{
	[this->pTouch() removeObjectForKey:iName];
	return *this;
	}

ZMap_NS& ZMap_NS::Erase(CFStringRef iName)
	{ return this->Erase((NSString*)iName); }

NSDictionary* ZMap_NS::pDictionary() const
	{ return inherited::Get(); }

NSMutableDictionary* ZMap_NS::pTouch()
	{
	ZRef<NSMutableDictionary> theMutableDictionary;
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (!fMutable || [theDictionary retainCount] > 1)
			{
			theMutableDictionary = sDictionaryMutable(theDictionary);
			inherited::operator=(theMutableDictionary);
			}
		else
			{
			theMutableDictionary = static_cast<NSMutableDictionary*>(theDictionary);
			}
		}
	else
		{
		theMutableDictionary = sDictionaryMutable();
		inherited::operator=(theMutableDictionary);
		}
	fMutable = true;
	return theMutableDictionary.Get();
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Cocoa)
