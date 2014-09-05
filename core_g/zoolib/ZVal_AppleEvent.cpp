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

using std::string;

#if defined(MAC_OS_X_VERSION_MIN_REQUIRED) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_2
	#define ZCONFIG_Has_typeUTF8Text 1
#else
	#define ZCONFIG_Has_typeUTF8Text 0
#endif

// =================================================================================================
// MARK: - spAECheckIsRecord

#if TARGET_RT_MAC_CFM

#include ZMACINCLUDE2(CoreFoundation,CFBundle.h)

// We're building for CFM, so AECheckIsRecord is not in CarbonLib. Manually load the entry point.

extern "C" typedef
Boolean (*AECheckIsRecord_Ptr)(const AEDesc* theDesc);

static bool spAECheckIsRecord(const AEDesc* theDesc)
	{
	static bool spLoaded = false;
	static AECheckIsRecord_Ptr spProc = nullptr;

	if (!spLoaded)
		{
		CFBundleRef bundleRef =
			::CFBundleGetBundleWithIdentifier(CFSTR("com.apple.ApplicationServices"));
		ZAssert(bundleRef);

		spProc = (AECheckIsRecord_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("AECheckIsRecord"));
		ZAssert(spProc);

		spLoaded = true;
		}

	return spProc(theDesc);
	}

#else // TARGET_RT_MAC_CFM

static bool spAECheckIsRecord(const AEDesc* theDesc)
	{ return ::AECheckIsRecord(theDesc); }

#endif // TARGET_RT_MAC_CFM

// =================================================================================================
#pragma mark -
#pragma mark *

namespace ZooLib {

// TODO look at TN2046 "AEStream and friends"

// =================================================================================================
// MARK: - Helper functions

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

static ZAny spDAsAny(const ZAny& iDefault, const AEDesc& iDesc);

static ZSeq_Any spAsSeq_Any(const ZAny& iDefault, const AEDesc& iDesc)
	{
	ZSeq_Any theSeq;

	long count;
	if (noErr == ::AECountItems(&iDesc, &count))
		{
		for (long x = 0; x < count; ++x)
			{
			ZVal_AppleEvent result;
			if (noErr == ::AEGetNthDesc(&iDesc, x + 1, typeWildCard, nullptr, &result.OParam()))
				theSeq.Append(spDAsAny(iDefault, result));
			else
				theSeq.Append(iDefault);
			}
		}

	return theSeq;
	}

static ZMap_Any spAsMap_Any(const ZAny& iDefault, const AEDesc& iDesc)
	{
	ZMap_Any theMap;

	long count;
	if (noErr == ::AECountItems(&iDesc, &count))
		{
		for (long x = 0; x < count; ++x)
			{
			AEKeyword key;
			ZVal_AppleEvent result;
			if (noErr == ::AEGetNthDesc(&iDesc, x + 1, typeWildCard, &key, &result.OParam()))
				theMap.Set(sAEKeywordAsString(key), spDAsAny(iDefault, result));
			}
		}

	return theMap;
	}

#define AEGETTER(cpp_t, DescType) \
	case DescType: \
		{ \
		cpp_t theVal; \
		::AEGetDescData(&iDesc, &theVal, sizeof(theVal)); \
		return ZAny(theVal); \
		} \

static ZAny spDAsAny(const ZAny& iDefault, const AEDesc& iDesc)
	{
	if (spAECheckIsRecord(&iDesc))
		return ZAny(spAsMap_Any(iDefault, iDesc));

	if (typeAEList == iDesc.descriptorType)
		return ZAny(spAsSeq_Any(iDefault, iDesc));

	switch (iDesc.descriptorType)
		{
		AEGETTER(int16, typeSInt16)
		AEGETTER(int32, typeSInt32)
		AEGETTER(uint32, typeUInt32)
		AEGETTER(int64, typeSInt64)
		AEGETTER(float, typeIEEE32BitFloatingPoint)
		AEGETTER(double, typeIEEE64BitFloatingPoint)
		AEGETTER(FSRef, typeFSRef)
		#if ZCONFIG_SPI_Enabled(Carbon)
			AEGETTER(FSSpec, typeFSS)
		#endif

		case typeTrue: return ZAny(true);
		case typeFalse: return ZAny(false);
		case typeBoolean:
			{
			Boolean theFlag;
			::AEGetDescData(&iDesc, &theFlag, sizeof(theFlag));
			return ZAny(true && theFlag);
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
			return ZAny(result);
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
			return ZAny(result);
			}

//		AEGETTER(ZHandle_T<AliasHandle>, typeAlias)
		}

	return iDefault;
	}

#undef AEGETTER

// =================================================================================================
// MARK: - ZVal_AppleEvent

/**
\class ZVal_AppleEvent
\ingroup ZVal

\brief Compatible with ZVal & AEDesc
*/

ZAny ZVal_AppleEvent::AsAny() const
	{ return this->DAsAny(ZAny()); }

ZAny ZVal_AppleEvent::DAsAny(const ZAny& iDefault) const
	{ return spDAsAny(iDefault, *this); }

ZVal_AppleEvent::operator operator_bool() const
	{ return operator_bool_gen::translate(descriptorType != typeNull); }

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

ZVal_AppleEvent::ZVal_AppleEvent(const ZSeq_AppleEvent& iVal)
	{ ::AEDuplicateDesc(&iVal, this); }

ZVal_AppleEvent::ZVal_AppleEvent(const ZMap_AppleEvent& iVal)
	{ ::AEDuplicateDesc(&iVal, this); }

void ZVal_AppleEvent::Clear()
	{ ::AEDisposeDesc(this); }

template <>
const ZQ<bool> ZVal_AppleEvent::QGet<bool>() const
	{
	if (typeTrue == descriptorType)
		{
		return true;
		}
	else if (typeFalse == descriptorType)
		{
		return false;
		}
	else if (typeBoolean == descriptorType)
		{
		Boolean theFlag;
		::AEGetDescData(this, &theFlag, sizeof(theFlag));
		return true && theFlag;
		}
	return null;
	}

template <>
const ZQ<string> ZVal_AppleEvent::QGet<string>() const
	{
	if (false)
		{}
	#if ZCONFIG_Has_typeUTF8Text
	else if (typeUTF8Text == descriptorType)
		{
		const size_t theSize = ::AEGetDescDataSize(this);
		string theString(' ', theSize);
		if (theSize)
			::AEGetDescData(this, const_cast<char*>(theString.data()), theSize);
		return theString;
		}
	#endif
	else if (typeChar == descriptorType)
		{
		const size_t theSize = ::AEGetDescDataSize(this);
		string theString(' ', theSize);
		if (theSize)
			::AEGetDescData(this, const_cast<char*>(theString.data()), theSize);
		return theString;
		}
	return null;
	}

template <>
const ZQ<ZSeq_AppleEvent> ZVal_AppleEvent::QGet<ZSeq_AppleEvent>() const
	{
	if (typeAEList == descriptorType)
		return ZSeq_AppleEvent(*static_cast<const AEDescList*>(this));

	return null;
	}

template <>
const ZQ<ZMap_AppleEvent> ZVal_AppleEvent::QGet<ZMap_AppleEvent>() const
	{
	if (spAECheckIsRecord(this))
		return ZMap_AppleEvent(*static_cast<const AEDescList*>(this));

	return null;
	}

template <class S>
const ZQ<S> ZVal_AppleEvent::QGet() const
	{
	const DescType desiredDescType = ZAELookup_CPP2Desc<S>::sDescType;
	if (desiredDescType == descriptorType)
		{
		S theVal;
		::AEGetDescData(this, &theVal, sizeof(theVal));
		return theVal;
		}

	AEDesc coerced;
	::AEInitializeDescInline(&coerced);
	if (noErr == ::AECoerceDesc(this, desiredDescType, &coerced))
		{
		S theVal;
		::AEGetDescData(&coerced, &theVal, sizeof(theVal));
		::AEDisposeDesc(&coerced);
		return theVal;
		}

	::AEDisposeDesc(&coerced);
	return null;
	}

template <>
void ZVal_AppleEvent::Set<bool>(const bool& iVal)
	{
	::AEDisposeDesc(this);
	::AECreateDesc(iVal ? typeTrue : typeFalse, nullptr, 0, this);
	}

template <>
void ZVal_AppleEvent::Set<std::string>(const std::string& iVal)
	{
	#if ZCONFIG_Has_typeUTF8Text
		::AEReplaceDescData(typeUTF8Text, iVal.data(), iVal.size(), this);
	#else
		::AEReplaceDescData(typeChar, iVal.data(), iVal.size(), this);
	#endif
	}

template <>
void ZVal_AppleEvent::Set<ZSeq_AppleEvent>(const ZSeq_AppleEvent& iVal)
	{
	::AEDisposeDesc(this);
	::AEDuplicateDesc(&iVal, this);
	}

template <>
void ZVal_AppleEvent::Set<ZMap_AppleEvent>(const ZMap_AppleEvent& iVal)
	{
	::AEDisposeDesc(this);
	::AEDuplicateDesc(&iVal, this);
	}

template <class S>
void ZVal_AppleEvent::Set(const S& iVal)
	{
	const ZAEValRef_T<S> theValRef(iVal);
	::AEReplaceDescData(theValRef.sDescType, theValRef.Ptr(), theValRef.Size(), this);
	}

AEDesc& ZVal_AppleEvent::OParam()
	{
	::AEDisposeDesc(this);
	return *this;
	}

const ZVal_AppleEvent ZVal_AppleEvent::Get(const std::string& iName) const
	{ return this->GetMap().Get(iName); }

const ZVal_AppleEvent ZVal_AppleEvent::Get(size_t iIndex) const
	{ return this->GetSeq().Get(iIndex); }

// =================================================================================================
// MARK: - ZVal_AppleEvent typename accessors

ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Int16, int16)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Int32, int32)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Int64, int64)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Bool, bool)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Float, float)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Double, double)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, String, std::string)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Seq, ZSeq_AppleEvent)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, Map, ZMap_AppleEvent)
ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, FSRef, FSRef)

#if ZCONFIG_SPI_Enabled(Carbon)
	ZMACRO_ZValAccessors_Def_GetSet(ZVal_AppleEvent, FSSpec, FSSpec)
#endif

// =================================================================================================
// MARK: - ZSeq_AppleEvent

ZSeq_Any ZSeq_AppleEvent::AsSeq_Any(const ZAny& iDefault) const
	{ return spAsSeq_Any(iDefault, *this); }

void ZSeq_AppleEvent::swap(ZSeq_AppleEvent& iOther)
	{ std::swap(static_cast<AEDesc&>(*this), static_cast<AEDesc&>(iOther)); }

ZSeq_AppleEvent::ZSeq_AppleEvent()
	{ ::AECreateList(nullptr, 0, false, this); }

ZSeq_AppleEvent::ZSeq_AppleEvent(const ZSeq_AppleEvent& iOther)
	{
	ZAssert(typeAEList == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZSeq_AppleEvent::~ZSeq_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZSeq_AppleEvent& ZSeq_AppleEvent::operator=(const ZSeq_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAEList == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;
	}

ZSeq_AppleEvent::ZSeq_AppleEvent(const AEDescList& iOther)
	{
	if (typeAEList == iOther.descriptorType)
		::AEDuplicateDesc(&iOther, this);
	else
		::AECreateList(nullptr, 0, false, this);
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::operator=(const AEDescList& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAEList == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;
	}

size_t ZSeq_AppleEvent::Count() const
	{
	long result;
	if (noErr == ::AECountItems(this, &result))
		return result;
	return 0;
	}

void ZSeq_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, false, this);
	}

const ZQ<ZVal_AppleEvent> ZSeq_AppleEvent::QGet(size_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &result.OParam()))
		return result;
	return null;
	}

const ZVal_AppleEvent ZSeq_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, size_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &result.OParam()))
		return result;
	return iDefault;
	}

const ZVal_AppleEvent ZSeq_AppleEvent::Get(size_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, &result.OParam()))
		return result;
	return ZVal_AppleEvent();
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::Erase(size_t iIndex)
	{::AEDeleteItem(this, iIndex + 1);
	return *this;
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::Set(size_t iIndex, const AEDesc& iVal)
	{
	::AEPutDesc(this, iIndex + 1, &iVal);
	return *this;
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::Insert(size_t iIndex, const AEDesc& iVal)
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
	return *this;
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::Append(const AEDesc& iVal)
	{
	::AEPutDesc(this, 0, &iVal);
	return *this;
	}

ZSeq_AppleEvent& ZSeq_AppleEvent::Append(const ZVal_AppleEvent& iVal)
	{
	::AEPutDesc(this, 0, &iVal);
	return *this;
	}

// =================================================================================================
// MARK: - ZMap_AppleEvent

ZMap_Any ZMap_AppleEvent::AsMap_Any(const ZAny& iDefault) const
	{ return spAsMap_Any(iDefault, *this); }

void ZMap_AppleEvent::swap(ZMap_AppleEvent& iOther)
	{ std::swap(static_cast<AEDesc&>(*this), static_cast<AEDesc&>(iOther)); }

ZMap_AppleEvent::ZMap_AppleEvent()
	{ ::AECreateList(nullptr, 0, true, this); }

ZMap_AppleEvent::ZMap_AppleEvent(const ZMap_AppleEvent& iOther)
	{
	ZAssert(spAECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	}

ZMap_AppleEvent::~ZMap_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZMap_AppleEvent& ZMap_AppleEvent::operator=(const ZMap_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(spAECheckIsRecord(&iOther));
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
	ZAssert(spAECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	descriptorType = iType;
	}

ZMap_AppleEvent::ZMap_AppleEvent(AEKeyword iType, const ZMap_AppleEvent& iOther)
	{
	ZAssert(spAECheckIsRecord(&iOther));
	::AEDuplicateDesc(&iOther, this);
	descriptorType = iType;
	}

ZMap_AppleEvent::ZMap_AppleEvent(const AERecord& iOther)
	{
	if (spAECheckIsRecord(&iOther))
		::AEDuplicateDesc(&iOther, this);
	else
		::AECreateList(nullptr, 0, true, this);
	}

ZMap_AppleEvent& ZMap_AppleEvent::operator=(const AERecord& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(spAECheckIsRecord(&iOther));
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;
	}

void ZMap_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, true, this);
	}

const ZQ<ZVal_AppleEvent> ZMap_AppleEvent::QGet(AEKeyword iName) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetKeyDesc(this, iName, typeWildCard, &result.OParam()))
		return result;
	return null;
	}

const ZQ<ZVal_AppleEvent> ZMap_AppleEvent::QGet(const string& iName) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetKeyDesc(this, spAsAEKeyword(iName), typeWildCard, &result.OParam()))
		return result;
	return null;
	}

const ZQ<ZVal_AppleEvent> ZMap_AppleEvent::QGet(Index_t iIndex) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetNthDesc(this, iIndex.Get() + 1, typeWildCard,
		nullptr, &result.OParam()))
		{
		return result;
		}
	return null;
	}

const ZVal_AppleEvent ZMap_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, AEKeyword iName) const
	{
	if (ZQ<ZVal_AppleEvent> theVal = this->QGet(iName))
		return theVal.Get();
	return iDefault;
	}

const ZVal_AppleEvent ZMap_AppleEvent::DGet(
	const ZVal_AppleEvent& iDefault, const std::string& iName) const
	{
	if (ZQ<ZVal_AppleEvent> theVal = this->QGet(iName))
		return theVal.Get();
	return iDefault;
	}

const ZVal_AppleEvent ZMap_AppleEvent::DGet(const ZVal_AppleEvent& iDefault, Index_t iIndex) const
	{
	if (ZQ<ZVal_AppleEvent> theVal = this->QGet(iIndex))
		return theVal.Get();
	return iDefault;
	}

const ZVal_AppleEvent ZMap_AppleEvent::Get(AEKeyword iName) const
	{ return this->DGet(ZVal_AppleEvent(), iName); }

const ZVal_AppleEvent ZMap_AppleEvent::Get(const std::string& iName) const
	{ return this->DGet(ZVal_AppleEvent(), iName); }

const ZVal_AppleEvent ZMap_AppleEvent::Get(Index_t iIndex) const
	{ return this->DGet(ZVal_AppleEvent(), iIndex); }

ZMap_AppleEvent& ZMap_AppleEvent::Set(AEKeyword iName, const AEDesc& iVal)
	{
	::AEPutKeyDesc(this, iName, &iVal);
	return *this;
	}

ZMap_AppleEvent& ZMap_AppleEvent::Set(const string& iName, const AEDesc& iVal)
	{
	::AEPutKeyDesc(this, spAsAEKeyword(iName), &iVal);
	return *this;
	}

ZMap_AppleEvent& ZMap_AppleEvent::Set(Index_t iIndex, const AEDesc& iVal)
	{
	::AEPutDesc(this, iIndex.Get() + 1, &iVal);
	return *this;
	}

ZMap_AppleEvent& ZMap_AppleEvent::Erase(AEKeyword iName)
	{
	::AEDeleteKeyDesc(this, iName);
	return *this;
	}

ZMap_AppleEvent& ZMap_AppleEvent::Erase(const string& iName)
	{
	::AEDeleteKeyDesc(this, spAsAEKeyword(iName));
	return *this;
	}

ZMap_AppleEvent& ZMap_AppleEvent::Erase(Index_t iIndex)
	{
	::AEDeleteItem(this, iIndex.Get() + 1);
	return *this;
	}

AEDesc& ZMap_AppleEvent::OParam()
	{
	::AEDisposeDesc(this);
	return *this;
	}

const ZQ<ZVal_AppleEvent> ZMap_AppleEvent::QGetAttr(AEKeyword iName) const
	{
	ZVal_AppleEvent result;
	if (noErr == ::AEGetAttributeDesc(this, iName, typeWildCard, &result.OParam()))
		return result;
	return null;
	}

const ZVal_AppleEvent ZMap_AppleEvent::DGetAttr(const ZVal_AppleEvent& iDefault, AEKeyword iName) const
	{
	if (ZQ<ZVal_AppleEvent> theVal = this->QGetAttr(iName))
		return theVal.Get();
	return iDefault;
	}

const ZVal_AppleEvent ZMap_AppleEvent::GetAttr(AEKeyword iName) const
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
	if (noErr == ::AEGetNthPtr(this, iIndex.Get() + 1, typeWildCard, &theKey,
		nullptr, nullptr, 0, nullptr))
		{
		return theKey;
		}
	return 0;
	}

string ZMap_AppleEvent::NameOf(Index_t iIndex) const
	{
	AEKeyword theKey;
	if (noErr == ::AEGetNthPtr(this, iIndex.Get() + 1, typeWildCard, &theKey,
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

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(AppleEvent)
