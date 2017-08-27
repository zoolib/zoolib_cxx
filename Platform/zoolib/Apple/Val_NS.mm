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

#include "zoolib/Apple/Val_NS.h"

#if ZCONFIG_SPI_Enabled(Cocoa)

#include "zoolib/Apple/Util_NS.h"

namespace ZooLib {

using std::type_info;

using Util_NS::sAsUTF8;
using Util_NS::sString;
using Util_NS::sStringMutable;
using Util_NS::sArrayMutable;
using Util_NS::sDictionaryMutable;

// =================================================================================================
#pragma mark -
#pragma mark Helpers (anonymous)

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
#pragma mark Val_NS

/**
\class Val_NS
\ingroup ZVal

\brief Compatible with ZVal & NSObject
*/

Val_NS::operator bool() const
	{
	if (NSObject* theVal = inherited::Get())
		{
		if ([theVal isMemberOfClass:[NSNull class]])
			return false;
		return true;
		}
	return false;
	}

Val_NS::Val_NS()
	{}

Val_NS::Val_NS(const Val_NS& iOther)
:	inherited(iOther)
	{}

Val_NS::~Val_NS()
	{}

Val_NS& Val_NS::operator=(const Val_NS& iOther)
	{
	inherited::operator=(iOther);
	return *this;
	}

Val_NS::Val_NS(char iVal)
:	inherited([NSNumber numberWithChar:iVal])
	{}

Val_NS::Val_NS(signed char iVal)
:	inherited([NSNumber numberWithChar:iVal])
	{}

Val_NS::Val_NS(unsigned char iVal)
:	inherited([NSNumber numberWithUnsignedChar:iVal])
	{}

Val_NS::Val_NS(wchar_t iVal)
:	inherited([NSNumber numberWithUnsignedInt:iVal])
	{}

Val_NS::Val_NS(short iVal)
:	inherited([NSNumber numberWithShort:iVal])
	{}

Val_NS::Val_NS(unsigned short iVal)
:	inherited([NSNumber numberWithUnsignedShort:iVal])
	{}

Val_NS::Val_NS(int iVal)
:	inherited([NSNumber numberWithInt:iVal])
	{}

Val_NS::Val_NS(unsigned int iVal)
:	inherited([NSNumber numberWithUnsignedInt:iVal])
	{}

Val_NS::Val_NS(long iVal)
:	inherited([NSNumber numberWithLong:iVal])
	{}

Val_NS::Val_NS(unsigned long iVal)
:	inherited([NSNumber numberWithUnsignedLong:iVal])
	{}

Val_NS::Val_NS(long long iVal)
:	inherited([NSNumber numberWithLongLong:iVal])
	{}

Val_NS::Val_NS(unsigned long long iVal)
:	inherited([NSNumber numberWithLongLong:iVal])
	{}

Val_NS::Val_NS(bool iVal)
:	inherited([NSNumber numberWithBool:iVal ? YES : NO])
	{}

Val_NS::Val_NS(float iVal)
:	inherited([NSNumber numberWithFloat:iVal])
	{}

Val_NS::Val_NS(double iVal)
:	inherited([NSNumber numberWithDouble:iVal])
	{}

Val_NS::Val_NS(long double iVal)
:	inherited([NSNumber numberWithDouble:double(iVal)])
	{}

Val_NS::Val_NS(const char* iVal)
:	inherited(sString(iVal))
	{}

Val_NS::Val_NS(const string8& iVal)
:	inherited(sString(iVal))
	{}

Val_NS::Val_NS(const string16& iVal)
:	inherited(sString(iVal))
	{}

Val_NS::Val_NS(CFStringRef iVal)
:	inherited((NSString*)iVal)
	{}

template <>
const ZQ<char> Val_NS::QGet<char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber charValue];
	return null;
	}

template <>
const ZQ<unsigned char> Val_NS::QGet<unsigned char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedCharValue];
	return null;
	}

template <>
const ZQ<signed char> Val_NS::QGet<signed char>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber charValue];
	return null;
	}

template <>
const ZQ<short> Val_NS::QGet<short>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber shortValue];
	return null;
	}

template <>
const ZQ<unsigned short> Val_NS::QGet<unsigned short>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedShortValue];
	return null;
	}

template <>
const ZQ<int> Val_NS::QGet<int>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber intValue];
	return null;
	}

template <>
const ZQ<unsigned int> Val_NS::QGet<unsigned int>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedIntValue];
	return null;
	}

template <>
const ZQ<long> Val_NS::QGet<long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber longValue];
	return null;
	}

template <>
const ZQ<unsigned long> Val_NS::QGet<unsigned long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedLongValue];
	return null;
	}

template <>
const ZQ<long long> Val_NS::QGet<long long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber longLongValue];
	return null;
	}

template <>
const ZQ<unsigned long long> Val_NS::QGet<unsigned long long>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber unsignedLongLongValue];
	return null;
	}

template <>
const ZQ<bool> Val_NS::QGet<bool>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber boolValue];
	return null;
	}

template <>
const ZQ<float> Val_NS::QGet<float>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber floatValue];
	return null;
	}

template <>
const ZQ<double> Val_NS::QGet<double>() const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		return [asNumber doubleValue];
	return null;
	}

template <>
const ZQ<string8> Val_NS::QGet<string8>() const
	{
	if (NSString* asString = spAs_T<NSString>(inherited::Get()))
		return sAsUTF8(asString);
	return null;
	}

template <>
const ZQ<Val_NS::NSStringPtr> Val_NS::QGet<Val_NS::NSStringPtr>() const
	{
	if (NSString* asString = spAs_T<NSString>(inherited::Get()))
		return asString;
	return null;
	}

template <>
const ZQ<Data_NS> Val_NS::QGet<Data_NS>() const
	{
	if (NSData* asData = spAs_T<NSData>(inherited::Get()))
		return asData;
	return null;
	}

template <>
const ZQ<Seq_NS> Val_NS::QGet<Seq_NS>() const
	{
	if (NSArray* asArray = spAs_T<NSArray>(inherited::Get()))
		return asArray;
	return null;
	}

template <>
const ZQ<Map_NS> Val_NS::QGet<Map_NS>() const
	{
	if (NSDictionary* asDictionary = spAs_T<NSDictionary>(inherited::Get()))
		return asDictionary;
	return null;
	}

template <>
void Val_NS::Set<int8>(const int8& iVal)
	{ inherited::operator=([NSNumber numberWithChar:iVal]); }

template <>
void Val_NS::Set<int16>(const int16& iVal)
	{ inherited::operator=([NSNumber numberWithShort:iVal]); }

template <>
void Val_NS::Set<int32>(const int32& iVal)
	{ inherited::operator=([NSNumber numberWithInt:iVal]); }

template <>
void Val_NS::Set<int64>(const int64& iVal)
	{ inherited::operator=([NSNumber numberWithLongLong:iVal]); }

template <>
void Val_NS::Set<bool>(const bool& iVal)
	{ inherited::operator=([NSNumber numberWithBool:iVal]); }

template <>
void Val_NS::Set<float>(const float& iVal)
	{ inherited::operator=([NSNumber numberWithFloat:iVal]); }

template <>
void Val_NS::Set<double>(const double& iVal)
	{ inherited::operator=([NSNumber numberWithDouble:iVal]); }

template <>
void Val_NS::Set<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void Val_NS::Set<Val_NS::NSStringPtr>(const Val_NS::NSStringPtr& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_NS::Set<Data_NS>(const Data_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_NS::Set<Seq_NS>(const Seq_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void Val_NS::Set<Map_NS>(const Map_NS& iVal)
	{ inherited::operator=(iVal); }

const Val_NS Val_NS::Get(const string8& iName) const
	{ return this->Get<Map_NS>().Get(iName); }

const Val_NS Val_NS::Get(NSString* iName) const
	{ return this->Get<Map_NS>().Get(iName); }

const Val_NS Val_NS::Get(size_t iIndex) const
	{ return this->Get<Seq_NS>().Get(iIndex); }

// =================================================================================================
#pragma mark -
#pragma mark Seq_NS

Seq_NS::operator bool() const
	{ return this->Size(); }

Seq_NS::Seq_NS()
:	inherited(sArrayMutable())
,	fMutable(true)
	{}

Seq_NS::Seq_NS(const Seq_NS& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Seq_NS::~Seq_NS()
	{}

Seq_NS& Seq_NS::operator=(const Seq_NS& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Seq_NS::Seq_NS(NSMutableArray* iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Seq_NS::Seq_NS(NSArray* iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Seq_NS::Seq_NS(const Adopt_T<NSMutableArray*>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Seq_NS::Seq_NS(const Adopt_T<NSArray*>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Seq_NS& Seq_NS::operator=(NSMutableArray* iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Seq_NS& Seq_NS::operator=(NSArray* iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Seq_NS& Seq_NS::operator=(const Adopt_T<NSMutableArray*>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Seq_NS& Seq_NS::operator=(const Adopt_T<NSArray*>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

size_t Seq_NS::Size() const
	{
	if (NSArray* theArray = this->pArray())
		return [theArray count];
	return 0;
	}

const ZQ<Val_NS> Seq_NS::QGet(size_t iIndex) const
	{
	if (NSArray* theArray = this->pArray())
		{
		if (size_t theCount = [theArray count])
			{
			if (iIndex < theCount)
				return Val_NS([theArray objectAtIndex:iIndex]);
			}
		}
	return null;
	}

const Val_NS Seq_NS::DGet(const Val_NS& iDefault, size_t iIndex) const
	{
	if (ZQ<Val_NS> theQ = this->QGet(iIndex))
		return *theQ;
	return iDefault;
	}

const Val_NS Seq_NS::Get(size_t iIndex) const
	{ return this->DGet(Val_NS(), iIndex); }

Seq_NS& Seq_NS::Set(size_t iIndex, const Val_NS& iVal)
	{
	[this->pTouch() replaceObjectAtIndex:iIndex withObject:iVal.Get()];
	return *this;
	}

Seq_NS& Seq_NS::Erase(size_t iIndex)
	{
	NSMutableArray* theArray = this->pTouch();
	if (const size_t theCount = [theArray count])
		{
		if (iIndex < theCount)
			[theArray removeObjectAtIndex:iIndex];
		}
	return *this;
	}

Seq_NS& Seq_NS::Insert(size_t iIndex, const Val_NS& iVal)
	{
	NSMutableArray* theArray = this->pTouch();
	const size_t theCount = [theArray count];
	if (iIndex <= theCount)
		[theArray insertObject:iVal.Get() atIndex:iIndex];
	return *this;
	}

Seq_NS& Seq_NS::Append(const Val_NS& iVal)
	{
	[this->pTouch() addObject:iVal.Get()];
	return *this;
	}

NSArray* Seq_NS::pArray() const
	{ return inherited::Get(); }

NSMutableArray* Seq_NS::pTouch()
	{
	ZRef<NSMutableArray> theMutableArray;
	if (NSArray* theArray = this->pArray())
		{
		if (not fMutable || [theArray retainCount] > 1)
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
#pragma mark Map_NS

Map_NS::operator bool() const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		return [theDictionary count];
	return false;
	}

Map_NS::Map_NS()
:	inherited(sDictionaryMutable())
,	fMutable(true)
	{}

Map_NS::Map_NS(const Map_NS& iOther)
:	inherited(iOther)
,	fMutable(iOther.fMutable)
	{}

Map_NS::~Map_NS()
	{}

Map_NS& Map_NS::operator=(const Map_NS& iOther)
	{
	inherited::operator=(iOther);
	fMutable = iOther.fMutable;
	return *this;
	}

Map_NS::Map_NS(NSDictionary* iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Map_NS::Map_NS(NSMutableDictionary* iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Map_NS::Map_NS(const Adopt_T<NSDictionary*>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

Map_NS::Map_NS(const Adopt_T<NSMutableDictionary*>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

Map_NS& Map_NS::operator=(NSMutableDictionary* iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Map_NS& Map_NS::operator=(NSDictionary* iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

Map_NS& Map_NS::operator=(const Adopt_T<NSMutableDictionary*>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

Map_NS& Map_NS::operator=(const Adopt_T<NSDictionary*>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

const ZQ<Val_NS> Map_NS::QGet(const string8& iName) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:sString(iName)])
			return theVal;
		}
	return null;
	}

const ZQ<Val_NS> Map_NS::QGet(NSString* iName) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:iName])
			return theVal;
		}
	return null;
	}

const Val_NS Map_NS::DGet(const Val_NS& iDefault, const string8& iName) const
	{
	if (ZQ<Val_NS> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const Val_NS Map_NS::DGet(const Val_NS& iDefault, NSString* iName) const
	{
	if (ZQ<Val_NS> theQ = this->QGet(iName))
		return *theQ;
	return iDefault;
	}

const Val_NS Map_NS::Get(const string8& iName) const
	{ return this->DGet(Val_NS(), iName); }

const Val_NS Map_NS::Get(NSString* iName) const
	{ return this->DGet(Val_NS(), iName); }

const Val_NS Map_NS::Get(CFStringRef iName) const
	{ return this->Get((NSString*)iName); }

Map_NS& Map_NS::Set(const string8& iName, const Val_NS& iVal)
	{
	[this->pTouch() setValue:iVal.Get() forKey:sString(iName)];
	return *this;
	}

Map_NS& Map_NS::Set(NSString* iName, const Val_NS& iVal)
	{
	[this->pTouch() setValue:iVal.Get() forKey:iName];
	return *this;
	}

Map_NS& Map_NS::Set(CFStringRef iName, const Val_NS& iVal)
	{ return this->Set((NSString*)iName, iVal); }

Map_NS& Map_NS::Erase(const string8& iName)
	{
	[this->pTouch() removeObjectForKey:sString(iName)];
	return *this;
	}

Map_NS& Map_NS::Erase(NSString* iName)
	{
	[this->pTouch() removeObjectForKey:iName];
	return *this;
	}

Map_NS& Map_NS::Erase(CFStringRef iName)
	{ return this->Erase((NSString*)iName); }

NSDictionary* Map_NS::pDictionary() const
	{ return inherited::Get(); }

NSMutableDictionary* Map_NS::pTouch()
	{
	ZRef<NSMutableDictionary> theMutableDictionary;
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (not fMutable || [theDictionary retainCount] > 1)
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
