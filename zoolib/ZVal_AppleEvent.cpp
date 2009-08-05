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

#include "zoolib/ZVal_AppleEvent.h"

#if ZCONFIG_SPI_Enabled(AppleEvent)

#include "zoolib/ZDebug.h"
#include "zoolib/ZTypes.h"

#include <string>

using std::string;

#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
#	define ZCONFIG_Has_typeUTF8Text 1
#else
#	define ZCONFIG_Has_typeUTF8Text 0
#endif

NAMESPACE_ZOOLIB_BEGIN

// TODO look at TN2046 "AEStream and friends"

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static AEKeyword spAsAEKeyword(const string& iName)
	{
	AEKeyword result = 0;
	const size_t theSize = iName.size();
	if (theSize > 0)
		{
		result |= uint8(iName[0]) << 24;
		if (theSize > 1)
			{
			result |= uint8(iName[1]) << 16;
			if (theSize > 2)
				{
				result |= uint8(iName[2]) << 8;
				if (theSize > 3)
					{
					result |= uint8(iName[3]);
					}
				}
			}
		}
	return result;
	}

string sAEKeywordAsString(AEKeyword iKeyword)
	{
	if (ZCONFIG(Endian, Big))
		{
		return string((char*)&iKeyword, 4);
		}
	else
		{
		string result;
		result += char((iKeyword >> 24) & 0xFF);
		result += char((iKeyword >> 16) & 0xFF);
		result += char((iKeyword >> 8) & 0xFF);
		result += char((iKeyword) & 0xFF);
		return result;
		}
	}


static ZVal_Any spAsVal_Any(const AEDesc& iDesc, const ZVal_Any& iDefault);

static ZList_Any spAsList_Any(const AEDesc& iDesc, const ZVal_Any& iDefault)
	{
	ZList_Any theList;

	long count;
	if (noErr == ::AECountItems(&iDesc, &count))
		{
		for (size_t x = 0; x < count; ++x)
			{
			ZVal_AppleEvent result;
			if (noErr == ::AEGetNthDesc(&iDesc, x + 1, typeWildCard, nullptr, &result.OParam()))
				theList.Append(spAsVal_Any(result, iDefault));
			else
				theList.Append(iDefault);
			}
		}

	return theList;
	}

static ZMap_Any spAsMap_Any(const AEDesc& iDesc, const ZVal_Any& iDefault)
	{
	ZMap_Any theMap;

	long count;
	if (noErr == ::AECountItems(&iDesc, &count))
		{
		for (size_t x = 0; x < count; ++x)
			{
			AEKeyword key;
			ZVal_AppleEvent result;
			if (noErr == ::AEGetNthDesc(&iDesc, x + 1, typeWildCard, &key, &result.OParam()))
				theMap.Set(sAEKeywordAsString(key), spAsVal_Any(result, iDefault));
			}
		}

	return theMap;
	}

#define AEGETTER(cpp_t, DescType) \
	case DescType: \
		{ \
		cpp_t theVal; \
		::AEGetDescData(&iDesc, &theVal, sizeof(theVal)); \
		return theVal; \
		} \

static ZVal_Any spAsVal_Any(const AEDesc& iDesc, const ZVal_Any& iDefault)
	{
	if (typeAEList == iDesc.descriptorType)
		return spAsList_Any(iDesc, iDefault);

	if (::AECheckIsRecord(&iDesc))
		return spAsMap_Any(iDesc, iDefault);

	switch (iDesc.descriptorType)
		{
		AEGETTER(int16, typeSInt16)
		AEGETTER(int32, typeSInt32)
		AEGETTER(uint32, typeUInt32)
		AEGETTER(int64, typeSInt64)
		AEGETTER(float, typeIEEE32BitFloatingPoint)
		AEGETTER(double, typeIEEE64BitFloatingPoint)
		AEGETTER(FSRef, typeFSRef)
		AEGETTER(FSSpec, typeFSS)

		case typeTrue: return true;
		case typeFalse: return false;
		case typeBoolean:
			{
			Boolean theFlag;
			::AEGetDescData(&iDesc, &theFlag, sizeof(theFlag));
			return true && theFlag;
			}

		#if ZCONFIG_Has_typeUTF8Text
		case typeUTF8Text:
			{
			string result;
			const size_t theSize = ::AEGetDescDataSize(&iDesc);
			if (theSize)
				{
				result.resize(theSize);
				::AEGetDescData(&iDesc, const_cast<char*>(result.data()), theSize);
				}
			return result;
			}
		#endif

		case typeChar:
			{
			string result;
			const size_t theSize = ::AEGetDescDataSize(&iDesc);
			if (theSize)
				{
				result.resize(theSize);
				::AEGetDescData(&iDesc, const_cast<char*>(result.data()), theSize);
				}
			return result;
			}

//		AEGETTER(ZHandle_T<AliasHandle>, typeAlias)		
		}

	return iDefault;
	}

#undef AEGETTER

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_AppleEvent

ZVal_Any ZVal_AppleEvent::AsVal_Any(const ZVal_Any& iDefault) const
	{ return spAsVal_Any(*this, iDefault); }

ZVal_AppleEvent::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(descriptorType != typeNull); }

void ZVal_AppleEvent::swap(ZVal_AppleEvent& iOther)
	{ std::swap(static_cast<AEDesc&>(*this), static_cast<AEDesc&>(iOther)); }

ZVal_AppleEvent::ZVal_AppleEvent()
	{
	// AEInitializeDesc doesn't exist in older CarbonLibs, although it's
	// in the headers. Use AEInitializeDescInline instead.
	::AEInitializeDescInline(this);
	}

ZVal_AppleEvent::ZVal_AppleEvent(const ZVal_AppleEvent& iOther)
	{ ::AEDuplicateDesc(&iOther, this); }

ZVal_AppleEvent::~ZVal_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZVal_AppleEvent& ZVal_AppleEvent::operator=(const ZVal_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;
	}

ZVal_AppleEvent::ZVal_AppleEvent(const AEDesc& iOther)
	{ ::AEDuplicateDesc(&iOther, this); }

ZVal_AppleEvent& ZVal_AppleEvent::operator=(const AEDesc& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;
	}

ZVal_AppleEvent::ZVal_AppleEvent(bool iVal)
	{ ::AECreateDesc(iVal ? typeTrue : typeFalse, nullptr, 0, this); }

ZVal_AppleEvent::ZVal_AppleEvent(const std::string& iVal)
	{
	#if ZCONFIG_Has_typeUTF8Text
		::AECreateDesc(typeUTF8Text, iVal.data(), iVal.size(), this);
	#else
		::AECreateDesc(typeChar, iVal.data(), iVal.size(), this);
	#endif
	}

ZVal_AppleEvent::ZVal_AppleEvent(const ZList_AppleEvent& iVal)
	{ ::AEDuplicateDesc(&iVal, this); }

ZVal_AppleEvent::ZVal_AppleEvent(const ZMap_AppleEvent& iVal)
	{ ::AEDuplicateDesc(&iVal, this); }

void ZVal_AppleEvent::Clear()
	{ ::AEDisposeDesc(this); }

template <>
bool ZVal_AppleEvent::QGet_T<bool>(bool& oVal) const
	{
	// What about typeBoolean?
	if (typeTrue == descriptorType)
		{
		oVal = true;
		return true;
		}
	else if (typeFalse == descriptorType)
		{
		oVal = false;
		return true;
		}
	else if (typeBoolean == descriptorType)
		{
		Boolean theFlag;
		::AEGetDescData(this, &theFlag, sizeof(theFlag));
		oVal = theFlag;
		return true;
		}
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<string>(string& oVal) const
	{
	if (false)
		{}
	#if ZCONFIG_Has_typeUTF8Text
	else if (typeUTF8Text == descriptorType)
		{
		const size_t theSize = ::AEGetDescDataSize(this);
		oVal.resize(theSize);
		if (theSize)
			{
			::AEGetDescData(this, const_cast<char*>(oVal.data()), theSize);
			return true;
			}
		}
	#endif
	else if (typeChar == descriptorType)
		{
		const size_t theSize = ::AEGetDescDataSize(this);
		oVal.resize(theSize);
		if (theSize)
			{
			::AEGetDescData(this, const_cast<char*>(oVal.data()), theSize);
			return true;
			}
		}
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<ZList_AppleEvent>(ZList_AppleEvent& oVal) const
	{
	if (typeAEList == descriptorType)
		{
		oVal = *static_cast<const AEDescList*>(this);
		return true;
		}
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<ZMap_AppleEvent>(ZMap_AppleEvent& oVal) const
	{
	if (::AECheckIsRecord(this))
		{
		oVal = *static_cast<const AERecord*>(this);
		return true;
		}
	return false;
	}

template <class S>
bool ZVal_AppleEvent::QGet_T(S& oVal) const
	{
	if (ZAELookup_CPP2Desc<S>::sDescType == descriptorType)
		{
		::AEGetDescData(this, &oVal, sizeof(oVal));
		return true;
		}
	return false;
	}

template <>
void ZVal_AppleEvent::Set_T<bool>(const bool& iVal)
	{
	::AEDisposeDesc(this);
	::AECreateDesc(iVal ? typeTrue : typeFalse, nullptr, 0, this);
	}

template <>
void ZVal_AppleEvent::Set_T<std::string>(const std::string& iVal)
	{
	#if ZCONFIG_Has_typeUTF8Text
		::AEReplaceDescData(typeUTF8Text, iVal.data(), iVal.size(), this);
	#else
		::AEReplaceDescData(typeChar, iVal.data(), iVal.size(), this);
	#endif
	}

template <>
void ZVal_AppleEvent::Set_T<ZList_AppleEvent>(const ZList_AppleEvent& iVal)
	{
	::AEDisposeDesc(this);
	::AEDuplicateDesc(&iVal, this);
	}

template <>
void ZVal_AppleEvent::Set_T<ZMap_AppleEvent>(const ZMap_AppleEvent& iVal)
	{
	::AEDisposeDesc(this);
	::AEDuplicateDesc(&iVal, this);
	}

template <class S>
void ZVal_AppleEvent::Set_T(const S& iVal)
	{
	const ZAEValRef_T<S> theValRef(iVal);
	::AEReplaceDescData(theValRef.sDescType, theValRef.Ptr(), theValRef.Size(), this);
	}

AEDesc& ZVal_AppleEvent::OParam()
	{
	::AEDisposeDesc(this);
	return *this;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_AppleEvent typename accessors

ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int16, int16)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int64, int64)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Float, float)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Double, double)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, String, std::string)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, List, ZList_AppleEvent)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Map, ZMap_AppleEvent)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, FSSpec, FSSpec)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, FSRef, FSRef)

// =================================================================================================
#pragma mark -
#pragma mark * ZList_AppleEvent

ZList_Any ZList_AppleEvent::AsList_Any(const ZVal_Any& iDefault) const
	{ return spAsList_Any(*this, iDefault); }

ZList_AppleEvent::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

void ZList_AppleEvent::swap(ZList_AppleEvent& iOther)
	{ std::swap(static_cast<AEDesc&>(*this), static_cast<AEDesc&>(iOther)); }

ZList_AppleEvent::ZList_AppleEvent()
	{ ::AECreateList(nullptr, 0, false, this); }

ZList_AppleEvent::ZList_AppleEvent(const ZList_AppleEvent& iOther)
	{
	ZAssert(typeAEList == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZList_AppleEvent::~ZList_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZList_AppleEvent& ZList_AppleEvent::operator=(const ZList_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAEList == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZList_AppleEvent::ZList_AppleEvent(const AEDescList& iOther)
	{
	if (typeAEList == iOther.descriptorType)
		::AEDuplicateDesc(&iOther, this);
	else
		::AECreateList(nullptr, 0, false, this);
	}

ZList_AppleEvent& ZList_AppleEvent::operator=(const AEDescList& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAEList == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

size_t ZList_AppleEvent::Count() const
	{
	long result;
	if (noErr == ::AECountItems(this, &result))
		return result;
	return 0;
	}

void ZList_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, false, this);
	}

bool ZList_AppleEvent::ZList_AppleEvent::QGet(size_t iIndex, ZVal_AppleEvent& oVal) const
	{ return noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &oVal.OParam()); }

ZVal_AppleEvent ZList_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, size_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &result.OParam()))
		return result;
	return iDefault;
	}

ZVal_AppleEvent ZList_AppleEvent::Get(size_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &result.OParam()))
		return result;
	return ZVal_AppleEvent();
	}

void ZList_AppleEvent::Erase(size_t iIndex)
	{ ::AEDeleteItem(this, iIndex + 1); }

void ZList_AppleEvent::Set(size_t iIndex, const AEDesc& iVal)
	{ ::AEPutDesc(this, iIndex + 1, &iVal); }

void ZList_AppleEvent::Insert(size_t iIndex, const AEDesc& iVal)
	{
	long theCount;
	if (noErr == ::AECountItems(this, &theCount))
		{
		AEDesc temp;
		for (size_t x = theCount - 1; x > iIndex; ++x)
			{
			::AEGetNthDesc(this, x + 1, typeWildCard, nullptr, &temp);
			::AEPutDesc(this, x + 2, &iVal);
			}
		::AEPutDesc(this, iIndex + 1, &iVal);
		}
	}

void ZList_AppleEvent::Append(const AEDesc& iVal)
	{ ::AEPutDesc(this, 0, &iVal); }

void ZList_AppleEvent::Append(const ZVal_AppleEvent& iVal)
	{ ::AEPutDesc(this, 0, &iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZMap_AppleEvent

ZMap_Any ZMap_AppleEvent::AsMap_Any(const ZVal_Any& iDefault) const
	{ return spAsMap_Any(*this, iDefault); }

ZMap_AppleEvent::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->pCount()); }

void ZMap_AppleEvent::swap(ZMap_AppleEvent& iOther)
	{ std::swap(static_cast<AEDesc&>(*this), static_cast<AEDesc&>(iOther)); }

ZMap_AppleEvent::ZMap_AppleEvent()
	{ ::AECreateList(nullptr, 0, true, this); }

ZMap_AppleEvent::ZMap_AppleEvent(const ZMap_AppleEvent& iOther)
	{
	ZAssert(::AECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	}

ZMap_AppleEvent::~ZMap_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZMap_AppleEvent& ZMap_AppleEvent::operator=(const ZMap_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(::AECheckIsRecord(&iOther));
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZMap_AppleEvent::ZMap_AppleEvent(AEKeyword iType)
	{
	::AECreateList(nullptr, 0, true, this);
	descriptorType = iType;
	}

ZMap_AppleEvent::ZMap_AppleEvent(AEKeyword iType, const AERecord& iOther)
	{
	ZAssert(::AECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	descriptorType = iType;
	}

ZMap_AppleEvent::ZMap_AppleEvent(AEKeyword iType, const ZMap_AppleEvent& iOther)
	{
	ZAssert(::AECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	descriptorType = iType;
	}

ZMap_AppleEvent::ZMap_AppleEvent(const AERecord& iOther)
	{
	if (::AECheckIsRecord(&iOther))
		::AEDuplicateDesc(&iOther, this);
	else
		::AECreateList(nullptr, 0, true, this);
	}

ZMap_AppleEvent& ZMap_AppleEvent::operator=(const AERecord& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(::AECheckIsRecord(&iOther));
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

void ZMap_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, true, this);
	}

bool ZMap_AppleEvent::QGet(AEKeyword iName, ZVal_AppleEvent& oVal) const
	{ return noErr ==::AEGetKeyDesc(this, iName, typeWildCard, &oVal.OParam()); }

bool ZMap_AppleEvent::QGet(const string& iName, ZVal_AppleEvent& oVal) const
	{ return noErr == ::AEGetKeyDesc(this, spAsAEKeyword(iName), typeWildCard, &oVal.OParam()); }

bool ZMap_AppleEvent::QGet(Index_t iIndex, ZVal_AppleEvent& oVal) const
	{
	return noErr == ::AEGetNthDesc(this, iIndex.GetIndex() + 1, typeWildCard,
		nullptr, &oVal.OParam());
	}

ZVal_AppleEvent ZMap_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, AEKeyword iName) const
	{
	ZVal_AppleEvent result;
	if (this->QGet(iName, result))
		return result;
	return iDefault;
	}

ZVal_AppleEvent ZMap_AppleEvent::DGet(
	const ZVal_AppleEvent& iDefault, const std::string& iName) const
	{
	ZVal_AppleEvent result;
	if (this->QGet(iName, result))
		return result;
	return iDefault;
	}

ZVal_AppleEvent ZMap_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, Index_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

ZVal_AppleEvent ZMap_AppleEvent::Get(AEKeyword iName) const
	{ return this->DGet(ZVal_AppleEvent(), iName); }
	
ZVal_AppleEvent ZMap_AppleEvent::Get(const std::string& iName) const
	{ return this->DGet(ZVal_AppleEvent(), iName); }

ZVal_AppleEvent ZMap_AppleEvent::Get(Index_t iIndex) const
	{ return this->DGet(ZVal_AppleEvent(), iIndex); }

void ZMap_AppleEvent::Set(AEKeyword iName, const AEDesc& iVal)
	{ ::AEPutKeyDesc(this, iName, &iVal); }

void ZMap_AppleEvent::Set(const string& iName, const AEDesc& iVal)
	{ ::AEPutKeyDesc(this, spAsAEKeyword(iName), &iVal); }

void ZMap_AppleEvent::Set(Index_t iIndex, const AEDesc& iVal)
	{ ::AEPutDesc(this, iIndex.GetIndex() + 1, &iVal); }

void ZMap_AppleEvent::Erase(AEKeyword iName)
	{ ::AEDeleteKeyDesc(this, iName); }

void ZMap_AppleEvent::Erase(const string& iName)
	{ ::AEDeleteKeyDesc(this, spAsAEKeyword(iName)); }

void ZMap_AppleEvent::Erase(Index_t iIndex)
	{  ::AEDeleteItem(this, iIndex.GetIndex() + 1); }

AEDesc& ZMap_AppleEvent::OParam()
	{
	::AEDisposeDesc(this);
	return *this;
	}

bool ZMap_AppleEvent::QGetAttr(AEKeyword iName, ZVal_AppleEvent& oVal) const
	{ return noErr == ::AEGetAttributeDesc(this, iName, typeWildCard, &oVal.OParam()); }

ZVal_AppleEvent ZMap_AppleEvent::DGetAttr(const ZVal_AppleEvent& iDefault, AEKeyword iName) const
	{
	ZVal_AppleEvent theVal;
	if (this->QGetAttr(iName, theVal))
		return theVal;
	return iDefault;
	}

ZVal_AppleEvent ZMap_AppleEvent::GetAttr(AEKeyword iName) const
	{ return this->DGetAttr(ZVal_AppleEvent(), iName); }

void ZMap_AppleEvent::SetAttr(AEKeyword iName, const AEDesc& iVal)
	{ ::AEPutAttributeDesc(this, iName, &iVal); }

AEKeyword ZMap_AppleEvent::GetType() const
	{ return descriptorType; }

ZMap_AppleEvent::Index_t ZMap_AppleEvent::Begin() const
	{ return Index_t(0); }

ZMap_AppleEvent::Index_t ZMap_AppleEvent::End() const
	{ return Index_t(this->pCount()); }

AEKeyword ZMap_AppleEvent::KeyOf(Index_t iIndex) const
	{
	AEKeyword theKey;
	if (noErr == ::AEGetNthPtr(this, iIndex.GetIndex() + 1, typeWildCard, &theKey,
		nullptr, nullptr, 0, nullptr))
		{
		return theKey;
		}
	return 0;
	}

string ZMap_AppleEvent::NameOf(Index_t iIndex) const
	{
	AEKeyword theKey;
	if (noErr == ::AEGetNthPtr(this, iIndex.GetIndex() + 1, typeWildCard, &theKey,
		nullptr, nullptr, 0, nullptr))
		{
		return sAEKeywordAsString(theKey);
		}
	return string();
	}

ZMap_AppleEvent::Index_t ZMap_AppleEvent::IndexOf(AEKeyword iName) const
	{
	for (size_t x = 0, count = this->pCount(); x < count; ++x)
		{
		if (iName == this->pKeyOf(x))
			return Index_t(x);
		}
	return Index_t(this->pCount());
	}

ZMap_AppleEvent::Index_t ZMap_AppleEvent::IndexOf(const std::string& iName) const
	{ return this->IndexOf(spAsAEKeyword(iName)); }

ZMap_AppleEvent::Index_t ZMap_AppleEvent::IndexOf(
	const ZMap_AppleEvent& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;

	const AEKeyword theKey = iOther.KeyOf(iOtherIndex);
	if (theKey == this->KeyOf(iOtherIndex))
		return iOtherIndex;

	return this->IndexOf(theKey);
	}

size_t ZMap_AppleEvent::pCount() const
	{
	long theCount;
	if (noErr == ::AECountItems(this, &theCount))
		return theCount;
	return 0;
	}

AEKeyword ZMap_AppleEvent::pKeyOf(size_t iIndex) const
	{
	AEKeyword theKey;
	if (noErr == ::AEGetNthPtr(this, iIndex + 1, typeWildCard, &theKey,
		nullptr, nullptr, 0, nullptr))
		{
		return theKey;
		}
	return 0;
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(AppleEvent)
