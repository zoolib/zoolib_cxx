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

NAMESPACE_ZOOLIB_BEGIN

namespace ZUtil_NS = ZUtil_NSObject;

using std::type_info;

using ZUtil_NS::sAsUTF8;
using ZUtil_NS::sString;
using ZUtil_NS::sStringMutable;
using ZUtil_NS::sArrayMutable;
using ZUtil_NS::sDictionaryMutable;

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

template <class T>
T* spAs_T(NSObject* iObj)
	{
	if ([iObj isMemberOfClass:[NSNumber class]])
		return static_cast<T*>(iObj);
	return nullptr;
	}

NSNumber* spAsNumber(NSObject* iObj)
	{ return spAs_T<NSNumber>(iObj); }

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

ZVal_NS::ZVal_NS(NSObject* iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(const ZRef<NSObject>& iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(const Adopt_T<NSObject>& iVal)
:	inherited(iVal.Get())
	{}

ZVal_NS::ZVal_NS(int8 iVal)
:	inherited([NSNumber numberWithChar:iVal])
	{}

ZVal_NS::ZVal_NS(int16 iVal)
:	inherited([NSNumber numberWithShort:iVal])
	{}

ZVal_NS::ZVal_NS(int32 iVal)
:	inherited([NSNumber numberWithLong:iVal])
	{}

ZVal_NS::ZVal_NS(int64 iVal)
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

ZVal_NS::ZVal_NS(NSString* iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(const ZData_NS& iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(const ZSeq_NS& iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(const ZMap_NS& iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(NSData* iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(NSArray* iVal)
:	inherited(iVal)
	{}

ZVal_NS::ZVal_NS(NSDictionary* iVal)
:	inherited(iVal)
	{}

ZVal_NS& ZVal_NS::operator=(NSObject* iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

ZVal_NS& ZVal_NS::operator=(const ZRef<NSObject>& iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

ZVal_NS& ZVal_NS::operator=(const Adopt_T<NSObject>& iVal)
	{
	inherited::operator=(iVal);
	return *this;
	}

void ZVal_NS::Clear()
	{ inherited::Clear(); }

template <>
bool ZVal_NS::QGet_T<int8>(int8& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber charValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<int16>(int16& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber shortValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<int32>(int32& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber intValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<int64>(int64& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber longLongValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<bool>(bool& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber boolValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<float>(float& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber floatValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<double>(double& oVal) const
	{
	if (NSNumber* asNumber = spAsNumber(inherited::Get()))
		{
		oVal = [asNumber doubleValue];
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<string8>(string8& oVal) const
	{
	if (NSString* asString = spAs_T<NSString>(inherited::Get()))
		{
		oVal = sAsUTF8(asString);
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<ZRef<NSString> >(ZRef<NSString>& oVal) const
	{
	if (ZRef<NSString> asString = spAs_T<NSString>(inherited::Get()))
		{
		oVal = asString;
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<ZData_NS>(ZData_NS& oVal) const
	{
	if (ZRef<NSData> asData = spAs_T<NSData>(inherited::Get()))
		{
		oVal = asData;
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<ZSeq_NS>(ZSeq_NS& oVal) const
	{
	if (ZRef<NSArray> asArray = spAs_T<NSArray>(inherited::Get()))
		{
		oVal = asArray;
		return true;
		}
	return false;
	}

template <>
bool ZVal_NS::QGet_T<ZMap_NS>(ZMap_NS& oVal) const
	{
	if (ZRef<NSDictionary> asDictionary = spAs_T<NSDictionary>(inherited::Get()))
		{
		oVal = asDictionary;
		return true;
		}
	return false;
	}

template <>
void ZVal_NS::Set_T<int8>(const int8& iVal)
	{ inherited::operator=([NSNumber numberWithChar:iVal]); }

template <>
void ZVal_NS::Set_T<int16>(const int16& iVal)
	{ inherited::operator=([NSNumber numberWithShort:iVal]); }

template <>
void ZVal_NS::Set_T<int32>(const int32& iVal)
	{ inherited::operator=([NSNumber numberWithInt:iVal]); }

template <>
void ZVal_NS::Set_T<int64>(const int64& iVal)
	{ inherited::operator=([NSNumber numberWithLongLong:iVal]); }

template <>
void ZVal_NS::Set_T<bool>(const bool& iVal)
	{ inherited::operator=([NSNumber numberWithBool:iVal]); }

template <>
void ZVal_NS::Set_T<float>(const float& iVal)
	{ inherited::operator=([NSNumber numberWithFloat:iVal]); }

template <>
void ZVal_NS::Set_T<double>(const double& iVal)
	{ inherited::operator=([NSNumber numberWithDouble:iVal]); }

template <>
void ZVal_NS::Set_T<string8>(const string8& iVal)
	{ inherited::operator=(sString(iVal)); }

template <>
void ZVal_NS::Set_T<ZRef<NSString> >(const ZRef<NSString>& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set_T<ZData_NS>(const ZData_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set_T<ZSeq_NS>(const ZSeq_NS& iVal)
	{ inherited::operator=(iVal); }

template <>
void ZVal_NS::Set_T<ZMap_NS>(const ZMap_NS& iVal)
	{ inherited::operator=(iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_NS typename accessors

ZMACRO_ZValAccessors_Def_Std(ZVal_NS)
ZMACRO_ZValAccessors_Def_Entry(ZVal_NS, NSString, ZRef<NSString>)
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

ZSeq_NS::ZSeq_NS(const ZRef<NSMutableArray>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZSeq_NS::ZSeq_NS(const ZRef<NSArray>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZSeq_NS::ZSeq_NS(const Adopt_T<NSMutableArray>& iOther)
:	inherited(ZRef<NSMutableArray>(iOther))
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

ZSeq_NS& ZSeq_NS::operator=(const ZRef<NSMutableArray>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZSeq_NS& ZSeq_NS::operator=(const ZRef<NSArray>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZSeq_NS& ZSeq_NS::operator=(const Adopt_T<NSMutableArray>& iOther)
	{
	inherited::operator=(ZRef<NSMutableArray>(iOther));
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

void ZSeq_NS::Clear()
	{ inherited::Clear(); }

bool ZSeq_NS::QGet(size_t iIndex, ZRef<NSObject>& oVal) const
	{
	if (NSArray* theArray = this->pArray())
		{
		if (size_t theCount = [theArray count])
			{
			if (iIndex < theCount)
				{
				oVal = [theArray objectAtIndex:iIndex];
				return true;
				}
			}
		}
	return false;
	}

ZVal_NS ZSeq_NS::DGet(const ZVal_NS& iDefault, size_t iIndex) const
	{
	ZVal_NS result;
	if (this->QGet(iIndex, result))
		return result;
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

ZMap_NS::ZMap_NS(const ZRef<NSDictionary>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_NS::ZMap_NS(const ZRef<NSMutableDictionary>& iOther)
:	inherited(iOther)
,	fMutable(true)
	{}

ZMap_NS::ZMap_NS(const Adopt_T<NSDictionary>& iOther)
:	inherited(iOther)
,	fMutable(false)
	{}

ZMap_NS::ZMap_NS(const Adopt_T<NSMutableDictionary>& iOther)
:	inherited(ZRef<NSMutableDictionary>(iOther))
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

ZMap_NS& ZMap_NS::operator=(const ZRef<NSMutableDictionary>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = true;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(const ZRef<NSDictionary>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(const Adopt_T<NSMutableDictionary>& iOther)
	{
	inherited::operator=(ZRef<NSMutableDictionary>(iOther));
	fMutable = true;
	return *this;
	}

ZMap_NS& ZMap_NS::operator=(const Adopt_T<NSDictionary>& iOther)
	{
	inherited::operator=(iOther);
	fMutable = false;
	return *this;
	}

void ZMap_NS::Clear()
	{ inherited::Clear(); }

bool ZMap_NS::QGet(const string8& iName, ZRef<NSObject>& oVal) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:sString(iName)])
			{
			oVal = theVal;
			return true;
			}
		}
	return false;
	}

bool ZMap_NS::QGet(NSString* iName, ZRef<NSObject>& oVal) const
	{
	if (NSDictionary* theDictionary = this->pDictionary())
		{
		if (NSObject* theVal = [theDictionary objectForKey:iName])
			{
			oVal = theVal;
			return true;
			}
		}
	return false;
	}

ZVal_NS ZMap_NS::DGet(const ZVal_NS& iDefault, const string8& iName) const
	{
	ZVal_NS theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_NS ZMap_NS::DGet(const ZVal_NS& iDefault, NSString* iName) const
	{
	ZVal_NS theVal;
	if (this->QGet(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_NS ZMap_NS::Get(const string8& iName) const
	{ return this->DGet(ZVal_NS(), iName); }

ZVal_NS ZMap_NS::Get(NSString* iName) const
	{ return this->DGet(ZVal_NS(), iName); }

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

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(Cocoa)
