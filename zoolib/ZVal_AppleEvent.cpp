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

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZVal_AppleEvent

ZVal_AppleEvent::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(descriptorType != typeNull); }

ZVal_AppleEvent::ZVal_AppleEvent()
	{ ::AEInitializeDesc(this); }

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

ZVal_AppleEvent::ZVal_AppleEvent(const bool& iVal)
	{ ::AECreateDesc(iVal ? typeTrue : typeFalse, nullptr, 0, this); }

ZVal_AppleEvent::ZVal_AppleEvent(const std::string& iVal)
	{ ::AECreateDesc(typeUTF8Text, iVal.data(), iVal.size(), this); }

AEDesc* ZVal_AppleEvent::ParamO()
	{
	::AEDisposeDesc(this);
	return this;
	}

template <>
bool ZVal_AppleEvent::QGet_T<bool>(bool& oVal) const
	{
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
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<string>(string& oVal) const
	{
#if !__MWERKS__
	if (typeUTF8Text == descriptorType)
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
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<ZValList_AppleEvent>(ZValList_AppleEvent& oVal) const
	{
	if (typeAEList == descriptorType)
		{
		oVal = *static_cast<const AEDescList*>(this);
		return true;
		}
	return false;
	}

template <>
bool ZVal_AppleEvent::QGet_T<ZValMap_AppleEvent>(ZValMap_AppleEvent& oVal) const
	{
	if (typeAERecord == descriptorType)
		{
		oVal = *static_cast<const AERecord*>(this);
		return true;
		}
	return false;
	}

template <class S>
bool ZVal_AppleEvent::QGet_T(S& oVal) const
	{
	if (ZAETypeMap_T<S>::sDescType == descriptorType)
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
#if !__MWERKS__
	::AEReplaceDescData(typeUTF8Text, iVal.data(), iVal.size(), this);
#endif
	}

template <>
void ZVal_AppleEvent::Set_T<ZValList_AppleEvent>(const ZValList_AppleEvent& iVal)
	{
	::AEDisposeDesc(this);
	::AEDuplicateDesc(&iVal, this);
	}

template <>
void ZVal_AppleEvent::Set_T<ZValMap_AppleEvent>(const ZValMap_AppleEvent& iVal)
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

ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int16, int16)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Int64, int64)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Float, float)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Double, double)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, String, std::string)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, List, ZValList_AppleEvent)
ZMACRO_ZValAccessors_Def_Entry(ZVal_AppleEvent, Map, ZValMap_AppleEvent)

ZMACRO_ZValAccessors_Def_Mac(ZVal_AppleEvent)

// =================================================================================================
#pragma mark -
#pragma mark * ZValList_AppleEvent

ZValList_AppleEvent::ZValList_AppleEvent()
	{ ::AECreateList(nullptr, 0, false, this); }

ZValList_AppleEvent::ZValList_AppleEvent(const ZValList_AppleEvent& iOther)
	{
	ZAssert(typeAEList == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZValList_AppleEvent::~ZValList_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZValList_AppleEvent& ZValList_AppleEvent::operator=(const ZValList_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAEList == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZValList_AppleEvent::ZValList_AppleEvent(const AEDescList& iOther)
	{
	ZAssert(typeAEList == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZValList_AppleEvent& ZValList_AppleEvent::operator=(const AEDescList& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAERecord == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZValList_AppleEvent::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

size_t ZValList_AppleEvent::Count() const
	{
	long result;
	if (noErr == ::AECountItems(this, &result))
		return result;
	return 0;
	}

void ZValList_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, false, this);
	}

bool ZValList_AppleEvent::QGet(size_t iIndex, ZVal_AppleEvent& oVal) const
	{
	if (noErr == ::AEGetNthDesc(this, iIndex + 1, typeWildCard, nullptr, oVal.ParamO()))
		return true;
	return false;
	}

void ZValList_AppleEvent::Erase(size_t iIndex)
	{ ::AEDeleteItem(this, iIndex + 1); }

void ZValList_AppleEvent::Set(size_t iIndex, const AEDesc& iVal)
	{ ::AEPutDesc(this, iIndex + 1, &iVal); }

void ZValList_AppleEvent::Insert(size_t iIndex, const AEDesc& iVal)
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

void ZValList_AppleEvent::Append(const AEDesc& iVal)
	{ ::AEPutDesc(this, 0, &iVal); }

void ZValList_AppleEvent::Append(const ZVal_AppleEvent& iVal)
	{ ::AEPutDesc(this, 0, &iVal); }

// =================================================================================================
#pragma mark -
#pragma mark * ZValMap_AppleEvent

ZValMap_AppleEvent::ZValMap_AppleEvent()
	{ ::AECreateList(nullptr, 0, true, this); }

ZValMap_AppleEvent::ZValMap_AppleEvent(const ZValMap_AppleEvent& iOther)
	{
	ZAssert(typeAERecord == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZValMap_AppleEvent::~ZValMap_AppleEvent()
	{ ::AEDisposeDesc(this); }

ZValMap_AppleEvent& ZValMap_AppleEvent::operator=(const ZValMap_AppleEvent& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAERecord == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZValMap_AppleEvent::ZValMap_AppleEvent(const AERecord& iOther)
	{
	ZAssert(typeAERecord == iOther.descriptorType);
	::AEDuplicateDesc(&iOther, this);
	}

ZValMap_AppleEvent& ZValMap_AppleEvent::operator=(const AERecord& iOther)
	{
	if (this != &iOther)
		{
		::AEDisposeDesc(this);
		ZAssert(typeAERecord == iOther.descriptorType);
		::AEDuplicateDesc(&iOther, this);
		}
	return *this;	
	}

ZValMap_AppleEvent::operator operator_bool_type() const
	{
	long result;
	return operator_bool_generator_type::translate
		(noErr == ::AECountItems(this, &result) && result);
	}

void ZValMap_AppleEvent::Clear()
	{
	::AEDisposeDesc(this);
	::AECreateList(nullptr, 0, true, this);
	}

bool ZValMap_AppleEvent::QGet(AEKeyword iName, ZVal_AppleEvent& oVal) const
	{
	if (noErr == ::AEGetKeyDesc(this, iName, typeWildCard, oVal.ParamO()))
		return true;
	return false;
	}

void ZValMap_AppleEvent::Set(AEKeyword iName, const AEDesc& iVal)
	{ ::AEPutKeyDesc(this, iName, &iVal); }

void ZValMap_AppleEvent::Erase(AEKeyword iName)
	{ ::AEDeleteKeyDesc(this, iName); }

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_SPI_Enabled(AppleEvent)
