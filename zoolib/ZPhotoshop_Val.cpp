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

#include "zoolib/ZPhotoshop_Val.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZUnicode.h"

#include "ASZStringSuite.h"
#include "PITerminology.h"
#include "PIUSuites.h"

using std::vector;

NAMESPACE_ZOOLIB_BEGIN

static AutoSuite<PSActionDescriptorProcs>
	sPSActionDescriptor(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);

static AutoSuite<PSActionControlProcs>
	sPSActionControl(kPSActionControlSuite, kPSActionControlSuitePrevVersion);

static AutoSuite<PSActionReferenceProcs>
	sPSActionReference(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion);

static AutoSuite<PSActionListProcs>
	sPSActionList(kPSActionListSuite, kPSActionListSuiteVersion);

static AutoSuite<ASZStringSuite>
	sASZString(kASZStringSuite, kASZStringSuiteVersion1);

template <>
void sRetain_T(ASZByteRun* iString)
	{
	if (iString)
		sASZString->AddRef(iString);
	}

template <>
void sRelease_T(ASZByteRun* iString)
	{
	if (iString)
		sASZString->Release(iString);
	}

namespace ZPhotoshop {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static PIActionList sDuplicate(PIActionList iSource)
	{
	PIActionList dummy;
	sPSActionList->Make(&dummy);

	sPSActionList->PutList(dummy, iSource);

	PIActionList result;
	sPSActionList->GetList(dummy, 0, &result);

	sPSActionList->Free(dummy);

	return result;	
	}

static PIActionDescriptor sDuplicate(PIActionDescriptor iSource)
	{
	PIActionList dummy;
	sPSActionList->Make(&dummy);

	sPSActionList->PutObject(dummy, typeObject, iSource);

	DescriptorClassID theType;
	PIActionDescriptor result;
	sPSActionList->GetObject(dummy, 0, &theType, &result);

	sPSActionList->Free(dummy);

	return result;	
	}

static string16 sAsString16(const ZRef<ASZString>& iString)
	{
	if (size_t theLength = sASZString->LengthAsUnicodeCString(iString))
		{
		string16 result(0, theLength);
		if (noErr == sASZString->AsUnicodeCString(
			iString, (ASUnicode*)&result[0], theLength, false))
			{
			return result;
			}
		}
	return string16();
	}

static ZRef<ASZString> sAsASZString(const string16& iString)
	{
	if (size_t theLength = iString.length())
		{
		ZRef<ASZString> result;
		if (noErr == sASZString->MakeFromUnicode((ASUnicode*)&iString[0], theLength * 2,
			&result.GetPtrRef()))
			{
			return result;
			}
		}
	return ZRef<ASZString>();
	}

static DescriptorKeyID sAsDescriptorKeyID(const string8& iName)
	{
	DescriptorKeyID result = 0;
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

static string8 sAsString(DescriptorKeyID iDescID)
	{
	if (ZCONFIG(Endian, Big))
		{
		return string8((char*)&iDescID, 4);
		}
	else
		{
		string8 result;
		result += char((iDescID >> 24) & 0xFF);
		result += char((iDescID >> 16) & 0xFF);
		result += char((iDescID >> 8) & 0xFF);
		result += char((iDescID) & 0xFF);
		return result;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Value

Value::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fType); }

Value::Value()
:	fType(0)
	{}

Value::Value(const Value& iOther)
	{ this->pCopy(iOther); }

Value::~Value()
	{ this->pRelease(); }

Value& Value::operator=(const Value& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopy(iOther);
		}
	return *this;
	}

Value::Value(int32 iVal)
	{
	fData.fAsInt32 = iVal;
	fType = typeInteger;
	}

Value::Value(double iVal)
	{
	fData.fAsDouble = iVal;
	fType = typeFloat;
	}

Value::Value(bool iVal)
	{
	fData.fAsBool = iVal;
	fType = typeBoolean;
	}

Value::Value(const string8& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeChar;
	}

Value::Value(const ZMemoryBlock& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeRawData;
	}

Value::Value(UnitFloat iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeUnitFloat;
	}

Value::Value(Enumerated iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeEnumerated;
	}

Value::Value(ClassID iType, const ZHandle_T<AliasHandle>& iHandle)
	{
	sConstruct_T(fData.fBytes, iHandle);
	fType = typePath;
	}

Value::Value(const List& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeValueList;
	}

Value::Value(const Map& iVal)
	{
	sConstruct_T<>(fData.fBytes, iVal);
	fType = typeObject;
	}

Value::Value(const Spec& iVal)
	{
	sConstruct_T<>(fData.fBytes, iVal);
	fType = typeObjectSpecifier;
	}

template <>
bool Value::QGet_T<int32>(int32& oVal) const
	{
	if (typeInteger == fType)
		{
		oVal = fData.fAsInt32;
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<double>(double& oVal) const
	{
	if (typeFloat == fType)
		{
		oVal = fData.fAsDouble;
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<bool>(bool& oVal) const
	{
	if (typeBoolean == fType)
		{
		oVal = fData.fAsBool;
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<string8>(string8& oVal) const
	{
	if (typeUnitFloat == fType)
		{
		oVal = *sFetch_T<string8>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<ZMemoryBlock>(ZMemoryBlock& oVal) const
	{
	if (typeRawData == fType)
		{
		oVal = *sFetch_T<ZMemoryBlock>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<UnitFloat>(UnitFloat& oVal) const
	{
	if (typeUnitFloat == fType)
		{
		oVal = *sFetch_T<UnitFloat>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<Enumerated>(Enumerated& oVal) const
	{
	if (typeEnumerated == fType)
		{
		oVal = *sFetch_T<Enumerated>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<List>(List& oVal) const
	{
	if (typeValueList == fType)
		{
		oVal = *sFetch_T<List>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<Map>(Map& oVal) const
	{
	if (typeObject == fType || typeGlobalObject == fType)
		{
		oVal = *sFetch_T<Map>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Value::QGet_T<Spec>(Spec& oVal) const
	{
	if (typeObjectSpecifier == fType)
		{
		oVal = *sFetch_T<Spec>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
void Value::Set_T<int32>(const int32& iVal)
	{
	this->pRelease();
	fData.fAsInt32 = iVal;
	fType = typeInteger;
	}

template <>
void Value::Set_T<bool>(const bool& iVal)
	{
	this->pRelease();
	fData.fAsBool = iVal;
	fType = typeBoolean;
	}

template <>
void Value::Set_T<double>(const double& iVal)
	{
	this->pRelease();
	fData.fAsDouble = iVal;
	fType = typeFloat;
	}

template <>
void Value::Set_T<string8>(const string8& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeChar;
	}

template <>
void Value::Set_T<ZMemoryBlock>(const ZMemoryBlock& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeRawData;
	}

template <>
void Value::Set_T<UnitFloat>(const UnitFloat& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeUnitFloat;
	}

template <>
void Value::Set_T<Enumerated>(const Enumerated& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeEnumerated;
	}

template <>
void Value::Set_T<List>(const List& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeValueList;
	}

template <>
void Value::Set_T<Map>(const Map& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeObject;
	}

template <>
void Value::Set_T<Spec>(const Spec& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeObjectSpecifier;
	}

void Value::pRelease()
	{
	const DescriptorKeyID theType = fType;
	fType = 0;
	switch (theType)
		{
		case typeChar:
			{
			sDestroy_T<string8>(fData.fBytes);
			break;
			}
		case typeRawData:
			{
			sDestroy_T<ZMemoryBlock>(fData.fBytes);
			break;
			}
		case typePath:
		case typeAlias:
			{
			sDestroy_T<ZHandle_T<AliasHandle> >(fData.fBytes);
			break;
			}
		case typeObject:
		case typeGlobalObject:
			{
			sDestroy_T<Map>(fData.fBytes);
			break;
			}
		case typeValueList:
			{
			sDestroy_T<List>(fData.fBytes);
			break;
			}
		case typeObjectSpecifier:
			{
			sDestroy_T<Spec>(fData.fBytes);
			break;
			}
		}
	}

void Value::pCopy(const Value& iOther)
	{
	switch (iOther.fType)
		{
		case typeInteger:
			{
			fData.fAsInt32 = iOther.fData.fAsInt32;
			break;
			}
		case typeFloat:
			{
			fData.fAsDouble = iOther.fData.fAsDouble;
			break;
			}
		case typeBoolean:
			{
			fData.fAsBool = iOther.fData.fAsBool;
			break;
			}
		case typeUnitFloat:
			{
			sCopyConstruct_T<UnitFloat>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeEnumerated:
			{
			sCopyConstruct_T<Enumerated>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeType:
		case typeGlobalClass:
			{
			fData.fAsClassID = iOther.fData.fAsClassID;
			break;
			}
		case typeChar:
			{
			sCopyConstruct_T<string8>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeRawData:
			{
			sCopyConstruct_T<ZMemoryBlock>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typePath:
		case typeAlias:
			{
			sCopyConstruct_T<ZHandle_T<AliasHandle> >(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeObject:
		case typeGlobalObject:
			{
			sCopyConstruct_T<Map>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeValueList:
			{
			sCopyConstruct_T<List>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typeObjectSpecifier:
			{
			sCopyConstruct_T<Spec>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		}
	fType = iOther.fType;
	}

ZMACRO_ZValAccessors_Def_Entry(Value, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(Value, Double, double)
ZMACRO_ZValAccessors_Def_Entry(Value, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(Value, String, string8)
ZMACRO_ZValAccessors_Def_Entry(Value, Raw, ZMemoryBlock)
ZMACRO_ZValAccessors_Def_Entry(Value, UnitFloat, UnitFloat)
ZMACRO_ZValAccessors_Def_Entry(Value, Enumerated, Enumerated)
ZMACRO_ZValAccessors_Def_Entry(Value, List, List)
ZMACRO_ZValAccessors_Def_Entry(Value, Map, Map)
ZMACRO_ZValAccessors_Def_Entry(Value, Spec, Spec)

// =================================================================================================
#pragma mark -
#pragma mark * List

List::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

List::List()
	{ sPSActionList->Make(&fAL); }

List::List(const List& iOther)
:	fAL(sDuplicate(iOther.fAL))
	{}

List::~List()
	{ sPSActionList->Free(fAL); }

List& List::operator=(const List& iOther)
	{
	if (this != &iOther)
		{
		sPSActionList->Free(fAL);
		fAL = sDuplicate(iOther.fAL);
		}
	return *this;
	}

List::List(PIActionList iOther)
:	fAL(sDuplicate(iOther))
	{}

List::List(Adopt_t<PIActionList> iOther)
:	fAL(iOther.Get())
	{}

List& List::operator=(PIActionList iOther)
	{
	sPSActionList->Free(fAL);
	fAL = sDuplicate(iOther);
	return *this;
	}

List& List::operator=(Adopt_t<PIActionList> iOther)
	{
	sPSActionList->Free(fAL);
	fAL = iOther.Get();
	return *this;
	}

size_t List::Count() const
	{
	uint32 result;
	if (noErr == sPSActionList->GetCount(fAL, &result))
		return result;
	return 0;
	}

void List::Clear()
	{
	sPSActionList->Free(fAL);
	sPSActionList->Make(&fAL);	
	}

#define COMMA ,

#define GETTERCASES(SUITE, PARAM) \
	case typeInteger: { int32 theVal; \
		if (noErr == SUITE->GetInteger(PARAM, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeFloat: { double theVal; \
		if (noErr == SUITE->GetFloat(PARAM, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeBoolean: { Boolean theVal; \
		if (noErr == SUITE->GetBoolean(PARAM, &theVal)) \
			{ oVal = bool(theVal); return true; } \
		break; } \
	case typeChar: \
		{ \
		uint32 theLength; \
		if (noErr == SUITE->GetStringLength(PARAM, &theLength)) \
			{ \
			string8 result('\0', theLength); \
			if (0 == theLength || noErr == SUITE->GetString(PARAM, &result[0], theLength)) \
				{ \
				oVal = result; \
				return true; \
				} \
			} \
		break; \
		} \
	case typeUnitFloat: { UnitFloat theVal; \
		if (noErr == SUITE->GetUnitFloat(PARAM, &theVal.fUnitID, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; } \
	case typeEnumerated: { Enumerated theVal; \
		if (noErr == SUITE->GetEnumerated(PARAM, &theVal.fEnumType, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; } \
	case typePath: \
		{ \
		ZUnimplemented(); \
		} \
	case typeValueList: { PIActionList theVal; \
		if (noErr == SUITE->GetList(PARAM, &theVal)) \
			{ oVal = List(Adopt(theVal)); return true; } \
		break; } \
	case typeObject: { \
		DescriptorClassID theDCID; \
		PIActionDescriptor theVal; \
		if (noErr == SUITE->GetObject(PARAM, &theDCID, &theVal)) \
			{ oVal = Map(Adopt(theVal)); return true; } \
		break; } \
	case typeObjectSpecifier: \
		{ \
		PIActionReference theVal; \
		if (noErr == SUITE->GetReference(PARAM, &theVal)) \
			{ \
			oVal = Spec(Adopt(theVal)); \
			return true; \
			} \
		break; \
		} \


#define SETTERCASES(SUITE, PARAM) \
	case typeInteger: { SUITE->PutInteger(PARAM, iVal.fData.fAsInt32); return; } \
	case typeFloat: { SUITE->PutFloat(PARAM, iVal.fData.fAsDouble); return; } \
	case typeBoolean: { SUITE->PutBoolean(PARAM, iVal.fData.fAsBool); return; } \
	case typeChar: \
		{ \
		SUITE->PutString(PARAM, \
			const_cast<char*>(sFetch_T<string8>(iVal.fData.fBytes)->c_str())); \
		return; \
		} \
	case typeRawData: \
		{ \
		ZUnimplemented(); \
		} \
	case typeUnitFloat: \
		{ \
		SUITE->PutUnitFloat(PARAM, \
			sFetch_T<UnitFloat>(iVal.fData.fBytes)->fUnitID, \
			sFetch_T<UnitFloat>(iVal.fData.fBytes)->fValue); \
		return; \
		} \
	case typeEnumerated: \
		{ \
		SUITE->PutEnumerated(PARAM, \
			sFetch_T<Enumerated>(iVal.fData.fBytes)->fEnumType, \
			sFetch_T<Enumerated>(iVal.fData.fBytes)->fValue); \
		return; \
		} \
	case typePath: \
		{ \
		ZUnimplemented(); \
		} \
	case typeValueList: \
		{ \
		SUITE->PutList(PARAM, \
			sFetch_T<List>(iVal.fData.fBytes)->GetActionList()); \
		return; \
		} \
	case typeObject: \
		{ \
		SUITE->PutObject(PARAM, \
			typeObject, \
			sFetch_T<Map>(iVal.fData.fBytes)->GetActionDescriptor()); \
		return; \
		} \
	case typeObjectSpecifier: \
		{ \
		PIActionReference tempRef = sFetch_T<Spec>(iVal.fData.fBytes)->MakeRef(); \
		SUITE->PutReference(PARAM, tempRef); \
		sPSActionReference->Free(tempRef); \
		return; \
		} \

bool List::QGet(size_t iIndex, Value& oVal) const
	{
	if (iIndex >= this->Count())
		return false;

	DescriptorTypeID theType;
	if (noErr != sPSActionList->GetType(fAL, iIndex, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(sPSActionList, fAL COMMA iIndex)
		default:
			ZUnimplemented();
		}
	return false;
	}

void List::Append(const Value& iVal)
	{
	switch (iVal.fType)
		{
		SETTERCASES(sPSActionList, fAL)
		default:
			ZUnimplemented();
		}
	}

PIActionList List::GetActionList() const
	{ return fAL; }

// =================================================================================================
#pragma mark -
#pragma mark * Map

Map::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->pCount()); }

Map::Map()
	{ sPSActionDescriptor->Make(&fAD); }

Map::Map(const Map& iOther)
:	fAD(sDuplicate(iOther.fAD))
	{}

Map::~Map()
	{
	if (fAD)
		sPSActionDescriptor->Free(fAD);
	}

Map& Map::operator=(const Map& iOther)
	{
	if (this != &iOther)
		{
		sPSActionDescriptor->Free(fAD);
		fAD = sDuplicate(iOther.fAD);
		}
	return *this;
	}

Map::Map(PIActionDescriptor iOther)
:	fAD(sDuplicate(iOther))
	{}

Map::Map(Adopt_t<PIActionDescriptor> iOther)
:	fAD(iOther.Get())
	{}

Map& Map::operator=(PIActionDescriptor iOther)
	{
	if (fAD)
		sPSActionDescriptor->Free(fAD);
	fAD = sDuplicate(iOther);
	return *this;
	}
	
Map& Map::operator=(Adopt_t<PIActionDescriptor> iOther)
	{
	if (fAD)
		sPSActionDescriptor->Free(fAD);
	fAD = iOther.Get();
	return *this;
	}

PIActionDescriptor* Map::ParamO()
	{
	if (fAD)
		sPSActionDescriptor->Free(fAD);
	fAD = nullptr;
	return &fAD;
	}

Map::const_iterator Map::begin()
	{ return const_iterator(0); }

Map::const_iterator Map::end()
	{ return const_iterator(this->pCount()); }

DescriptorKeyID Map::KeyOf(const_iterator iPropIter) const
	{
	if (iPropIter.GetIndex() < this->pCount())
		{
		DescriptorKeyID result;
		if (noErr == sPSActionDescriptor->GetKey(fAD, iPropIter.GetIndex(), &result))
			return result;
		}
	return 0;	
	}

std::string Map::NameOf(const_iterator iPropIter) const
	{
	if (iPropIter.GetIndex() < this->pCount())
		{
		DescriptorKeyID result;
		if (noErr == sPSActionDescriptor->GetKey(fAD, iPropIter.GetIndex(), &result))
			return sAsString(result);
		}
	return string8();
	}

void Map::Clear()
	{ sPSActionDescriptor->Clear(fAD); }

bool Map::QGet(DescriptorKeyID iName, Value& oVal) const
	{
	if (!fAD)
		return false;

	DescriptorTypeID theType;
	if (noErr != sPSActionDescriptor->GetType(fAD, iName, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(sPSActionDescriptor, fAD COMMA iName)
		default:
			ZUnimplemented();
		}
	return false;
	}

bool Map::QGet(const string8& iName, Value& oVal) const
	{ return this->QGet(sAsDescriptorKeyID(iName), oVal); }

bool Map::QGet(const_iterator iName, Value& oVal)
	{ return this->QGet(this->KeyOf(iName), oVal); }

void Map::Set(DescriptorKeyID iName, const Value& iVal)
	{
	switch (iVal.fType)
		{
		SETTERCASES(sPSActionDescriptor, fAD COMMA iName)
		default:
			ZUnimplemented();//?
		}
	}

void Map::Set(const string8& iName, const Value& iVal)
	{ this->Set(sAsDescriptorKeyID(iName), iVal); }

void Map::Set(const_iterator iName, const Value& iVal)
	{ this->Set(this->KeyOf(iName), iVal); }

void Map::Erase(DescriptorKeyID iName)
	{ sPSActionDescriptor->Erase(fAD, iName); }

void Map::Erase(const string8& iName)
	{ this->Erase(sAsDescriptorKeyID(iName)); }

void Map::Erase(const_iterator iName)
	{ this->Erase(this->KeyOf(iName)); }

PIActionDescriptor Map::GetActionDescriptor() const
	{ return fAD; }

size_t Map::pCount() const
	{
	uint32 result;
	if (noErr == sPSActionDescriptor->GetCount(fAD, &result))
		return result;
	return 0;
	}

#undef COMMA

// =================================================================================================
#pragma mark -
#pragma mark * Spec

Spec::Entry::Entry()
:	fFormID(0)
	{}

Spec::Entry::Entry(const Entry& iOther)
	{ this->pCopyFrom(iOther); }

Spec::Entry::~Entry()
	{ this->pRelease(); }

Spec::Entry& Spec::Entry::operator=(const Entry& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopyFrom(iOther);
		}
	return *this;
	}

Spec::Entry Spec::Entry::sClass(ClassID iClassID)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	theEntry.fFormID = formClass;
	return theEntry;
	}

Spec::Entry Spec::Entry::sEnum(ClassID iClassID, const Enumerated& iEnum)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	sConstruct_T(theEntry.fData.fBytes, iEnum);
	theEntry.fFormID = formEnumerated;
	return theEntry;
	}

Spec::Entry Spec::Entry::sIdentifier(ClassID iClassID, uint32 iIdentifier)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	theEntry.fData.fAsIdentifier = iIdentifier;
	theEntry.fFormID = formIdentifier;
	return theEntry;
	}

Spec::Entry Spec::Entry::sIndex(ClassID iClassID, uint32 iIndex)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	theEntry.fData.fAsIndex = iIndex;
	theEntry.fFormID = formIndex;
	return theEntry;
	}

Spec::Entry Spec::Entry::sName(ClassID iClassID, const string8& iName)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	sConstruct_T(theEntry.fData.fBytes, iName);
	theEntry.fFormID = formName;
	return theEntry;
	}

Spec::Entry Spec::Entry::sName(ClassID iClassID, const ZRef<ASZString>& iName)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	string8 theString8 = ZUnicode::sAsUTF8(sAsString16(iName));
	sConstruct_T(theEntry.fData.fBytes, theString8);
	theEntry.fFormID = formName;
	return theEntry;
	}

Spec::Entry Spec::Entry::sOffset(ClassID iClassID, int32 iOffset)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	theEntry.fData.fAsOffset = iOffset;
	theEntry.fFormID = formOffset;
	return theEntry;
	}

Spec::Entry Spec::Entry::sProperty(ClassID iClassID, KeyID iKeyID)
	{
	Entry theEntry;
	theEntry.fClassID = iClassID;
	theEntry.fData.fAsProperty = iKeyID;
	theEntry.fFormID = formProperty;
	return theEntry;
	}

void Spec::Entry::pRelease()
	{
	FormID theFormID = fFormID;
	fFormID = 0;
	switch (theFormID)
		{
		case formClass:
		case formIdentifier:
		case formIndex:
		case formOffset:
		case formProperty:
			break;
		case formEnumerated:
			{
			sDestroy_T<Enumerated>(fData.fBytes);
			break;
			}
		case formName:
			{
			sDestroy_T<string8>(fData.fBytes);
			break;
			}
		}
	}

void Spec::Entry::pCopyFrom(const Entry& iOther)
	{
	switch (iOther.fFormID)
		{
		case formClass:
			{
			break;
			}
		case formEnumerated:
			{
			sCopyConstruct_T<Enumerated>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case formIdentifier:
			{
			fData.fAsIdentifier = iOther.fData.fAsIdentifier;
			break;
			}
		case formIndex:
			{
			fData.fAsIndex = iOther.fData.fAsIndex;
			break;
			}
		case formName:
			{
			sCopyConstruct_T<string8>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case formOffset:
			{
			fData.fAsOffset = iOther.fData.fAsOffset;
			break;
			}
		case formProperty:
			{
			fData.fAsProperty = iOther.fData.fAsProperty;
			break;
			}
		}
	fFormID = iOther.fFormID;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Spec

Spec Spec::sClass(ClassID iClassID)
	{ return Entry::sClass(iClassID); }

Spec Spec::sEnum(ClassID iClassID, DescriptorEnumTypeID iEnumType, DescriptorEnumID iValue)
	{ return Entry::sEnum(iClassID, Enumerated(iEnumType, iValue)); }

Spec Spec::sEnum(ClassID iClassID, const Enumerated& iEnum)
	{ return Entry::sEnum(iClassID, iEnum); }

Spec Spec::sIdentifier(ClassID iClassID, uint32 iIdentifier)
	{ return Entry::sIdentifier(iClassID, iIdentifier); }

Spec Spec::sIndex(ClassID iClassID, uint32 iIndex)
	{ return Entry::sIndex(iClassID, iIndex); }

Spec Spec::sName(ClassID iClassID, const string8& iName)
	{ return Entry::sName(iClassID, iName); }

Spec Spec::sName(ClassID iClassID, const ZRef<ASZString>& iName)
	{ return Entry::sName(iClassID, iName); }

Spec Spec::sOffset(ClassID iClassID, int32 iOffset)
	{ return Entry::sOffset(iClassID, iOffset); }

Spec Spec::sProperty(ClassID iClassID, KeyID iKeyID)
	{ return Entry::sProperty(iClassID, iKeyID); }

Spec::Spec()
	{}

Spec::Spec(const Spec& iOther)
:	fEntries(iOther.fEntries)
	{}

Spec::~Spec()
	{}

Spec& Spec::operator=(const Spec& iOther)
	{
	fEntries = iOther.fEntries;
	return *this;
	}

Spec::Spec(const Entry& iEntry)
:	fEntries(1, iEntry)
	{}

Spec::Spec(PIActionReference iOther)
	{ pConvert(iOther, fEntries); }

Spec::Spec(Adopt_t<PIActionReference> iOther)
	{ pConvert(iOther.Get(), fEntries); }

Spec& Spec::operator=(PIActionReference iOther)
	{
	vector<Entry> newEntries;
	pConvert(iOther, newEntries);
	std::swap(fEntries, newEntries);
	return *this;
	}

Spec& Spec::operator=(Adopt_t<PIActionReference> iOther)
	{
	vector<Entry> newEntries;
	pConvert(iOther.Get(), newEntries);
	std::swap(fEntries, newEntries);
	sPSActionReference->Free(iOther.Get());
	return *this;
	}

Spec Spec::operator+(const Spec& iOther)
	{ return Spec(*this) += iOther; }

Spec& Spec::operator+=(const Spec& iOther)
	{
	fEntries.insert(fEntries.end(), iOther.fEntries.begin(), iOther.fEntries.end());
	return *this;
	}

Map Spec::Get() const
	{
	Map result;
	if (PIActionReference theRef = this->MakeRef())
		{
		if (noErr != sPSActionControl->Get(result.ParamO(), theRef))
			result.Clear();
		sPSActionReference->Free(theRef);
		}
	return result;
	}

PIActionReference Spec::MakeRef() const
	{
	bool allOK = true;
	PIActionReference theRef = nullptr;
	
	if (noErr == sPSActionReference->Make(&theRef))
		{
		for (vector<Entry>::const_reverse_iterator i = fEntries.rbegin();
			allOK && i != fEntries.rend(); ++i)
			{
			const Entry& theEntry = *i;
			switch (theEntry.fFormID)
				{
				case formClass:
					{
					if (noErr != sPSActionReference->PutClass(theRef, theEntry.fClassID))
						allOK = false;
					break;
					}
				case formEnumerated:
					{
					if (noErr != sPSActionReference->PutEnumerated(theRef,
						theEntry.fClassID,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fEnumType,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fValue))
						{ allOK = false; }
					break;
					}
				case formIdentifier:
					{
					if (noErr != sPSActionReference->PutIdentifier(theRef,
						theEntry.fClassID, theEntry.fData.fAsIdentifier))
						{ allOK = false; }
					break;
					}
				case formIndex:
					{
					if (noErr != sPSActionReference->PutIndex(theRef,
						theEntry.fClassID, theEntry.fData.fAsIndex))
						{ allOK = false; }
					break;
					}
				case formName:
					{
					const string8& theName = *sFetch_T<string8>(theEntry.fData.fBytes);
					if (noErr != sPSActionReference->PutName(theRef,
						theEntry.fClassID, const_cast<char*>(theName.c_str())))
						{ allOK = false; }
					break;
					}
				case formOffset:
					{
					if (noErr != sPSActionReference->PutOffset(theRef,
						theEntry.fClassID, theEntry.fData.fAsOffset))
						{ allOK = false; }
					break;
					}
				case formProperty:
					{
					if (noErr != sPSActionReference->PutProperty(theRef,
						theEntry.fClassID, theEntry.fData.fAsProperty))
						{ allOK = false; }
					break;
					}
				}
			}
		}

	return theRef;
	}

void Spec::pConvert(PIActionReference iRef, vector<Entry>& oEntries)
	{
	bool allOK = true;
	while (iRef && allOK)
		{
		DescriptorFormID theFormID;
		if (noErr != sPSActionReference->GetForm(iRef, &theFormID))
			break;

		ClassID theClassID;
		if (noErr != sPSActionReference->GetDesiredClass(iRef, &theClassID))
			break;

		switch (theFormID)
			{
			case formClass:
				{
				oEntries.push_back(Entry::sClass(theClassID));
				break;
				}
			case formEnumerated:
				{
				Enumerated theVal;
				if (noErr != sPSActionReference->GetEnumerated(
					iRef, &theVal.fEnumType, &theVal.fValue))
					{
					allOK = false;
					}
				else
					{
					oEntries.push_back(Entry::sEnum(theClassID, theVal));
					}
				break;
				}
			case formIdentifier:
				{
				uint32 theVal;
				if (noErr != sPSActionReference->GetIdentifier(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sIdentifier(theClassID, theVal));
				break;
				}
			case formIndex:
				{
				uint32 theVal;
				if (noErr != sPSActionReference->GetIndex(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sIndex(theClassID, theVal));
				break;
				}
			case formName:
				{
				#if (kPSActionReferenceSuiteVersion >= 3)
					ZRef<ASZString> theVal;
					if (noErr != sPSActionReference->GetNameZString(iRef, &theVal.GetPtrRef()))
						allOK = false;
					else
						oEntries.push_back(Entry::sName(theClassID, theVal));
				#else
					uint32 theLength;
					if (noErr != sPSActionReference->GetNameLength(iRef, &theLength))
						{
						allOK = false;
						}
					else
						{
						string8 theName(0, theLength);
						if (noErr != sPSActionReference->GetName(iRef,
							const_cast<char*>(theName.data()), theLength))
							{
							allOK = false;
							}
						else
							{
							oEntries.push_back(Entry::sName(theClassID, theName));
							}
						}
				#endif
				break;
				}
			case formOffset:
				{
				int32 theVal;
				if (noErr != sPSActionReference->GetOffset(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sOffset(theClassID, theVal));
				break;
				}
			case formProperty:
				{
				KeyID theVal;
				if (noErr != sPSActionReference->GetProperty(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sProperty(theClassID, theVal));
				break;
				}
			}
		}
	}

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
