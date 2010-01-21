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

#include "zoolib/photoshop/ZPhotoshop_Val.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZVal_Any.h"

#include "ASZStringSuite.h"
#include "PITerminology.h"
#include "PIUSuites.h"

#if ZCONFIG_SPI_Enabled(Carbon64)
#	if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
#		include ZMACINCLUDE3(ApplicationServices,AE,AEObjects.h)
#	else
#		include ZMACINCLUDE3(CoreServices,AE,AEObjects.h)
#	endif
#endif

#if ZCONFIG_SPI_Enabled(Win)
	enum
		{
		typeSInt32 = typeInteger,
		typeIEEE64BitFloatingPoint = typeFloat
		};
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Seq and Map Copy, Getter, Setter macros

#define COPYFROMTO(SUITE, iSource, iKey, iType, DEST) \
	switch (iType) \
		{ \
		case typeSInt32: \
			{ \
			int32 theVal; \
			if (noErr == SUITE->GetInteger(iSource, iKey, &theVal)) \
				SUITE->PutInteger(DEST, theVal); \
			break; \
			} \
		case typeIEEE64BitFloatingPoint: \
			{ \
			double theVal; \
			if (noErr == SUITE->GetFloat(iSource, iKey, &theVal)) \
				SUITE->PutFloat(DEST, theVal); \
			break; \
			} \
		case typeUnitFloat: \
			{ \
			UnitFloat theVal; \
			if (noErr == SUITE->GetUnitFloat(iSource, iKey, &theVal.fUnitID, &theVal.fValue)) \
				SUITE->PutUnitFloat(DEST, theVal.fUnitID, theVal.fValue); \
			break; \
			} \
		case typeChar: \
			{ \
			uint32 theLength; \
			if (noErr == SUITE->GetStringLength(iSource, iKey, &theLength)) \
				{ \
				string8 theVal(size_t(theLength + 1), ' '); \
				if (0 == theLength \
					|| noErr == SUITE->GetString(iSource, iKey, &theVal[0], theLength + 1)) \
					{ \
					SUITE->PutString(DEST, const_cast<char*>(theVal.c_str())); \
					} \
				} \
			break; \
			} \
		case typeBoolean: \
			{ \
			Boolean theVal; \
			if (noErr == SUITE->GetBoolean(iSource, iKey, &theVal)) \
				SUITE->PutBoolean(DEST, theVal); \
			break; \
			} \
		case typeEnumerated: \
			{ \
			Enumerated theVal; \
			if (noErr == SUITE->GetEnumerated(iSource, iKey, &theVal.fEnumType, &theVal.fValue)) \
				SUITE->PutEnumerated(DEST, theVal.fEnumType, theVal.fValue); \
			break; \
			} \
		case typeObjectSpecifier: \
			{ \
			PIActionReference theVal; \
			if (noErr == SUITE->GetReference(iSource, iKey, &theVal)) \
				{ \
				SUITE->PutReference(DEST, theVal); \
				/*??spPSActionReference->Free(theVal);*/ \
				} \
			break; \
			} \
		case typeValueList: \
			{ \
			PIActionList theVal; \
			if (noErr == SUITE->GetList(iSource, iKey, &theVal)) \
				{ \
				theVal = spDuplicate(theVal); \
				SUITE->PutList(DEST, theVal); \
				} \
			break; \
			} \
		case typeObject: \
			{ \
			DescriptorClassID theDCID; \
			PIActionDescriptor theVal; \
			if (noErr == SUITE->GetObject(iSource, iKey, &theDCID, &theVal)) \
				{ \
				theVal = spDuplicate(theVal); \
				SUITE->PutObject(DEST, theDCID, theVal); \
				} \
			break; \
			} \
		/* global object? */ \
		case typeClass: \
			{ \
			ZUnimplemented(); \
			break; \
			} \
		/* global class? */ \
		case typeAlias: \
		case typePath: \
			{ \
			FileRef theVal; \
			if (noErr == SUITE->GetAlias(iSource, iKey, &theVal.OParam())) \
				SUITE->PutAlias(DEST, theVal.Get()); \
			break; \
			} \
		case typeRawData: \
			{ \
			int32 theLength; \
			if (noErr == SUITE->GetDataLength(iSource, iKey, &theLength)) \
				{ \
				Data theVal(theLength); \
				if (0 == theLength \
					|| noErr == SUITE->GetData(iSource, iKey, theVal.GetData())) \
					{ \
					SUITE->PutData(DEST, theVal.GetSize(), theVal.GetData()); \
					} \
				} \
			break; \
			} \
		} \

#define GETTERCASES(SUITE, P0, P1) \
	case typeSInt32: \
		{ \
		int32 theVal; \
		if (noErr == SUITE->GetInteger(P0, P1, &theVal)) \
			{ oVal = theVal; return true; } \
		break; \
		} \
	case typeIEEE64BitFloatingPoint: \
		{ \
		double theVal; \
		if (noErr == SUITE->GetFloat(P0, P1, &theVal)) \
			{ oVal = theVal; return true; } \
		break; \
		} \
	case typeUnitFloat: \
		{ \
		UnitFloat theVal; \
		if (noErr == SUITE->GetUnitFloat(P0, P1, &theVal.fUnitID, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; \
		} \
	case typeChar: \
		{ \
		uint32 theLength; \
		if (noErr == SUITE->GetStringLength(P0, P1, &theLength)) \
			{ \
			string8 result(size_t(theLength + 1), ' '); \
			if (0 == theLength || noErr == SUITE->GetString(P0, P1, &result[0], theLength + 1)) \
				{ \
				oVal = result.substr(0, theLength); \
				return true; \
				} \
			} \
		break; \
		} \
	case typeBoolean: \
		{ \
		Boolean theVal; \
		if (noErr == SUITE->GetBoolean(P0, P1, &theVal)) \
			{ oVal = bool(theVal); return true; } \
		break; \
		} \
	case typeEnumerated: \
		{ \
		Enumerated theVal; \
		if (noErr == SUITE->GetEnumerated(P0, P1, &theVal.fEnumType, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; \
		} \
	case typeObjectSpecifier: \
		{ \
		PIActionReference theVal; \
		if (noErr == SUITE->GetReference(P0, P1, &theVal)) \
			{ \
			oVal = Spec(Adopt(theVal)); \
			return true; \
			} \
		break; \
		} \
	case typeValueList: \
		{ \
		PIActionList theVal; \
		if (noErr == SUITE->GetList(P0, P1, &theVal)) \
			{ oVal = Seq(Adopt(theVal)); return true; } \
		break; \
		} \
	case typeObject: \
		{ \
		DescriptorClassID theDCID; \
		PIActionDescriptor theVal; \
		if (noErr == SUITE->GetObject(P0, P1, &theDCID, &theVal)) \
			{ oVal = Map(theDCID, Adopt(theVal)); return true; } \
		break; \
		} \
	/* global object? */ \
	case typeClass: \
		{ \
		ZUnimplemented(); \
		break; \
		} \
	/* global class? */ \
	case typeAlias: \
	case typePath: \
		{ \
		FileRef theVal; \
		if (noErr == SUITE->GetAlias(P0, P1, &theVal.OParam())) \
			{ oVal = theVal; return true; } \
		break; \
		} \
	case typeRawData: \
		{ \
		int32 theLength; \
		if (noErr == SUITE->GetDataLength(P0, P1, &theLength)) \
			{ \
			Data result(theLength); \
			if (0 == theLength || noErr == SUITE->GetData(P0, P1, result.GetData())) \
				{ \
				oVal = result; \
				return true; \
				} \
			} \
		break; \
		} \

#define SETTERCASES(SUITE, PARAM) \
	if (false) \
		{} \
	else if (const int32* theVal = iVal.PGet_T<int32>()) \
		{ SUITE->PutInteger(PARAM, *theVal); } \
	else if (const float* theVal = iVal.PGet_T<float>()) \
		{ SUITE->PutFloat(PARAM, *theVal); } \
	else if (const UnitFloat* theVal = iVal.PGet_T<UnitFloat>()) \
		{ SUITE->PutUnitFloat(PARAM, theVal->fUnitID, theVal->fValue); } \
	else if (const string8* theVal = iVal.PGet_T<string8>()) \
		{ SUITE->PutString(PARAM, const_cast<char*>(theVal->c_str())); } \
	else if (const bool* theVal = iVal.PGet_T<bool>()) \
		{ SUITE->PutBoolean(PARAM, *theVal); } \
	else if (const Seq* theVal = iVal.PGet_T<Seq>()) \
		{ SUITE->PutList(PARAM, theVal->IParam()); } \
	else if (const Map* theVal = iVal.PGet_T<Map>()) \
		{ SUITE->PutObject(PARAM, theVal->GetType(), theVal->IParam()); } \
	/* global object? */ \
	else if (const Enumerated* theVal = iVal.PGet_T<Enumerated>()) \
		{ SUITE->PutEnumerated(PARAM, theVal->fEnumType, theVal->fValue); } \
	else if (const Spec* theVal = iVal.PGet_T<Spec>()) \
		{ \
		PIActionReference tempRef = theVal->MakeRef(); \
		SUITE->PutReference(PARAM, tempRef); \
		spPSActionReference->Free(tempRef); \
		} \
/*Hmmm	else if (const ClassID* theVal = iVal.PGet_T<ClassID>()) \
		{ ZUnimplemented(); } Hmm??? SUITE->PutInteger(PARAM, *theVal); } */\
	else if (const FileRef* theVal = iVal.PGet_T<FileRef>()) \
		{ SUITE->PutAlias(PARAM, theVal->Get()); } \
	else if (const Data* theVal = iVal.PGet_T<Data>()) \
		{ SUITE->PutData(PARAM, theVal->GetSize(), const_cast<void*>(theVal->GetData())); }

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop suites, for local use

static AutoSuite<PSActionDescriptorProcs>
	spPSActionDescriptor(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);

static AutoSuite<PSActionControlProcs>
	spPSActionControl(kPSActionControlSuite, kPSActionControlSuiteVersion);

static AutoSuite<PSActionReferenceProcs>
	spPSActionReference(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion);

static AutoSuite<PSActionListProcs>
	spPSActionList(kPSActionListSuite, kPSActionListSuiteVersion);

static AutoSuite<ASZStringSuite>
	spASZString(kASZStringSuite, kASZStringSuiteVersion1);

// =================================================================================================
#pragma mark -
#pragma mark * Using statements

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;

// =================================================================================================
#pragma mark -
#pragma mark * Support for ZRef<ASZString>

template <>
void sRetain_T(struct ASZByteRun*& ioString)
	{
	if (ioString)
		spASZString->AddRef(ioString);
	}

template <>
void sRelease_T(struct ASZByteRun* iString)
	{
	if (iString)
		spASZString->Release(iString);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop

namespace ZPhotoshop {

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

static PIActionDescriptor spDuplicate(PIActionDescriptor iSource);

static PIActionList spDuplicate(PIActionList iSource)
	{
	PIActionList theDest;
	spPSActionList->Make(&theDest);
	uint32 theCount;
	if (noErr == spPSActionList->GetCount(iSource, &theCount))
		{
		for (size_t x = 0; x < theCount; ++x)
			{
			TypeID theType;
			if (noErr != spPSActionList->GetType(iSource, x, &theType))
				break;
			COPYFROMTO(spPSActionList, iSource, x, theType, theDest)
			}
		}
	return theDest;
	}

static PIActionDescriptor spDuplicate(PIActionDescriptor iSource)
	{
	PIActionDescriptor theDest;
	spPSActionDescriptor->Make(&theDest);
	uint32 theCount;
	if (noErr == spPSActionDescriptor->GetCount(iSource, &theCount))
		{
		for (size_t x = 0; x < theCount; ++x)
			{
			KeyID theKey;
			if (noErr != spPSActionDescriptor->GetKey(iSource, x, &theKey))
				break;
			TypeID theType;
			if (noErr != spPSActionDescriptor->GetType(iSource, theKey, &theType))
				break;

			#define COMMA() ,

			COPYFROMTO(spPSActionDescriptor, iSource, theKey, theType, theDest COMMA() theKey)

			#undef COMMA
			}
		}
	return theDest;
	}

static string16 spAsString16(const ZRef<ASZString>& iString)
	{
	if (size_t theLength = spASZString->LengthAsUnicodeCString(iString))
		{
		string16 result(0, theLength);
		if (noErr == spASZString->AsUnicodeCString(
			iString, (ASUnicode*)&result[0], theLength, false))
			{
			return result;
			}
		}
	return string16();
	}

static ZRef<ASZString> spAsASZString(const string16& iString)
	{
	if (size_t theLength = iString.length())
		{
		ZRef<ASZString> result;
		if (noErr == spASZString->MakeFromUnicode((ASUnicode*)&iString[0], theLength * 2,
			&result.GetPtrRef()))
			{
			return result;
			}
		}
	return ZRef<ASZString>();
	}

static TypeID spAsRuntimeTypeID(const string8& iString)
	{
	TypeID theTypeID;
	if (sAsRuntimeTypeID(iString, theTypeID))
		return theTypeID;
	return 0;
	}

static string8 spFromRuntimeTypeID(TypeID iTypeID)
	{
	string8 result;
	if (sFromRuntimeTypeID(iTypeID, result))
		return result;
	return string8();
	}

static KeyID spAsKeyID(const string8& iName)
	{ return spAsRuntimeTypeID(iName); }

static string8 spAsString(KeyID iKeyID)
	{ return spFromRuntimeTypeID(iKeyID); }

// =================================================================================================
#pragma mark -
#pragma mark * Public utilities

bool sAsRuntimeTypeID(const string8& iString, TypeID& oTypeID)
	{
	if (noErr == spPSActionControl->StringIDToTypeID(const_cast<char*>(iString.c_str()), &oTypeID))
		return true;
	return false;
	}

static bool spAppendIfASCII(char iChar, string& ioString)
	{
	if (iChar < 32 || iChar > 126)
		return false;
	ioString += iChar;
	return true;
	}

static bool spIntAsString(int32 iInt, string& oString)
	{
	oString.clear();

	if (!spAppendIfASCII(iInt >> 24, oString))
		return false;

	if (!spAppendIfASCII(iInt >> 16, oString))
		return false;

	if (!spAppendIfASCII(iInt >> 8, oString))
		return false;

	if (!spAppendIfASCII(iInt, oString))
		return false;

	return true;
	}

bool sFromRuntimeTypeID(TypeID iTypeID, string8& oString)
	{
	char buf[1024];
	if (noErr == spPSActionControl->TypeIDToStringID(iTypeID, buf, sizeof(buf)-1))
		{
		if (buf[0])
			{
			oString = buf;
			return true;
			}
		}
	return spIntAsString(iTypeID, oString);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ClassID

ClassID::ClassID(const string8& iName)
:	fDCI(spAsRuntimeTypeID(iName))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Enumerated

Enumerated::Enumerated(EnumTypeID iEnumType, const string8& iValue)
:	fEnumType(iEnumType)
,	fValue(spAsRuntimeTypeID(iValue))
	{}

Enumerated::Enumerated(const string8& iEnumType, EnumID iValue)
:	fEnumType(spAsRuntimeTypeID(iEnumType))
,	fValue(iValue)
	{}

Enumerated::Enumerated(const string8& iEnumType, const string8& iValue)
:	fEnumType(spAsRuntimeTypeID(iEnumType))
,	fValue(spAsRuntimeTypeID(iValue))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Spec::Entry

Spec::Entry::Entry()
:	fDCI(0)
,	fFormID(0)
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
	theEntry.fDCI = iClassID.GetDCI();
	theEntry.fFormID = formClass;
	return theEntry;
	}

Spec::Entry Spec::Entry::sEnum(ClassID iClassID, const Enumerated& iEnum)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	sConstruct_T(theEntry.fData.fBytes, iEnum);
	theEntry.fFormID = formEnumerated;
	return theEntry;
	}

Spec::Entry Spec::Entry::sIdentifier(ClassID iClassID, uint32 iIdentifier)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	theEntry.fData.fAsIdentifier = iIdentifier;
	theEntry.fFormID = formIdentifier;
	return theEntry;
	}

Spec::Entry Spec::Entry::sIndex(ClassID iClassID, uint32 iIndex)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	theEntry.fData.fAsIndex = iIndex;
	theEntry.fFormID = formIndex;
	return theEntry;
	}

Spec::Entry Spec::Entry::sName(ClassID iClassID, const string8& iName)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	sConstruct_T(theEntry.fData.fBytes, iName);
	theEntry.fFormID = formName;
	return theEntry;
	}

Spec::Entry Spec::Entry::sName(ClassID iClassID, const ZRef<ASZString>& iName)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	string8 theString8 = ZUnicode::sAsUTF8(spAsString16(iName));
	sConstruct_T(theEntry.fData.fBytes, theString8);
	theEntry.fFormID = formName;
	return theEntry;
	}

Spec::Entry Spec::Entry::sOffset(ClassID iClassID, int32 iOffset)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
	theEntry.fData.fAsOffset = iOffset;
	theEntry.fFormID = formOffset;
	return theEntry;
	}

Spec::Entry Spec::Entry::sProperty(ClassID iClassID, KeyID iKeyID)
	{
	Entry theEntry;
	theEntry.fDCI = iClassID.GetDCI();
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
	fDCI = iOther.fDCI;
	fFormID = iOther.fFormID;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Spec

Spec Spec::sClass(ClassID iClassID)
	{ return Entry::sClass(iClassID); }

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

Spec Spec::sProperty(ClassID iClassID, const string8& iName)
	{ return Entry::sProperty(iClassID, spAsRuntimeTypeID(iName)); }

Spec::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(!fEntries.empty()); }

void Spec::swap(Spec& iOther)
	{ fEntries.swap(iOther.fEntries); }

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
	{ spConvert(iOther, fEntries); }

Spec::Spec(Adopt_T<PIActionReference> iOther)
	{ spConvert(iOther.Get(), fEntries); }

Spec& Spec::operator=(PIActionReference iOther)
	{
	vector<Entry> newEntries;
	spConvert(iOther, newEntries);
	std::swap(fEntries, newEntries);
	return *this;
	}

Spec& Spec::operator=(Adopt_T<PIActionReference> iOther)
	{
	vector<Entry> newEntries;
	spConvert(iOther.Get(), newEntries);
	std::swap(fEntries, newEntries);
	spPSActionReference->Free(iOther.Get());
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
		OSErr theErr = spPSActionControl->Get(&result.OParam(), theRef);
		if (noErr != theErr)
			result.Clear();
		spPSActionReference->Free(theRef);
		}
	return result;
	}

PIActionReference Spec::MakeRef() const
	{
	bool allOK = true;
	PIActionReference theRef = nullptr;
	
	if (noErr == spPSActionReference->Make(&theRef))
		{
		for (vector<Entry>::const_reverse_iterator i = fEntries.rbegin();
			allOK && i != fEntries.rend(); ++i)
			{
			const Entry& theEntry = *i;
			switch (theEntry.fFormID)
				{
				case formClass:
					{
					if (noErr != spPSActionReference->PutClass(theRef, theEntry.fDCI))
						allOK = false;
					break;
					}
				case formEnumerated:
					{
					if (noErr != spPSActionReference->PutEnumerated(theRef, theEntry.fDCI,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fEnumType,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fValue))
						{ allOK = false; }
					break;
					}
				case formIdentifier:
					{
					if (noErr != spPSActionReference->PutIdentifier(theRef,
						theEntry.fDCI,
						theEntry.fData.fAsIdentifier))
						{ allOK = false; }
					break;
					}
				case formIndex:
					{
					if (noErr != spPSActionReference->PutIndex(theRef, theEntry.fDCI,
						theEntry.fData.fAsIndex))
						{ allOK = false; }
					break;
					}
				case formName:
					{
					const string8& theName = *sFetch_T<string8>(theEntry.fData.fBytes);
					if (noErr != spPSActionReference->PutName(theRef, theEntry.fDCI,
						const_cast<char*>(theName.c_str())))
						{ allOK = false; }
					break;
					}
				case formOffset:
					{
					if (noErr != spPSActionReference->PutOffset(theRef, theEntry.fDCI,
						theEntry.fData.fAsOffset))
						{ allOK = false; }
					break;
					}
				case formProperty:
					{
					if (noErr != spPSActionReference->PutProperty(theRef, theEntry.fDCI,
						theEntry.fData.fAsProperty))
						{ allOK = false; }
					break;
					}
				}
			}
		}

	return theRef;
	}

void Spec::spConvert(PIActionReference iRef, vector<Entry>& oEntries)
	{
	// TODO Check this
	bool freeRef = false;
	bool allOK = true;
	while (iRef && allOK)
		{
		FormID theFormID;
		if (noErr != spPSActionReference->GetForm(iRef, &theFormID))
			break;

		DescriptorClassID theClassID;
		if (noErr != spPSActionReference->GetDesiredClass(iRef, &theClassID))
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
				if (noErr != spPSActionReference->GetEnumerated(
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
				if (noErr != spPSActionReference->GetIdentifier(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sIdentifier(theClassID, theVal));
				break;
				}
			case formIndex:
				{
				uint32 theVal;
				if (noErr != spPSActionReference->GetIndex(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sIndex(theClassID, theVal));
				break;
				}
			case formName:
				{
				#if (kPSActionReferenceSuiteVersion >= 3)
					ZRef<ASZString> theVal;
					if (noErr != spPSActionReference->GetNameZString(iRef, &theVal.GetPtrRef()))
						allOK = false;
					else
						oEntries.push_back(Entry::sName(theClassID, theVal));
				#else
					uint32 theLength;
					if (noErr != spPSActionReference->GetNameLength(iRef, &theLength))
						{
						allOK = false;
						}
					else
						{
						string8 theName(size_t(theLength + 1), ' ');
						if (noErr != spPSActionReference->GetName(iRef,
							const_cast<char*>(theName.data()), theLength + 1))
							{
							allOK = false;
							}
						else
							{
							oEntries.push_back(
								Entry::sName(theClassID, theName.substr(0, theLength)));
							}
						}
				#endif
				break;
				}
			case formOffset:
				{
				int32 theVal;
				if (noErr != spPSActionReference->GetOffset(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sOffset(theClassID, theVal));
				break;
				}
			case formProperty:
				{
				KeyID theVal;
				if (noErr != spPSActionReference->GetProperty(iRef, &theVal))
					allOK = false;
				else
					oEntries.push_back(Entry::sProperty(theClassID, theVal));
				break;
				}
			}
		PIActionReference container;
		if (noErr != spPSActionReference->GetContainer(iRef, &container))
			container = nullptr;
		if (freeRef)
			spPSActionReference->Free(iRef);
		iRef = container;
		freeRef = true;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Val

ZAny Val::AsAny() const
	{ return this->AsAny(ZAny()); }

ZAny Val::AsAny(const ZAny& iDefault) const
	{
	if (const Map* theVal = this->PGet_T<Map>())
		return ZAny(theVal->AsAny(iDefault));

	if (const Seq* theVal = this->PGet_T<Seq>())
		return ZAny(theVal->AsAny(iDefault));
		
	return *this;
	}

Val::Val()
	{}

Val::Val(const Val& iOther)
:	ZAny((const ZAny&)iOther)
	{}

Val::~Val()
	{}

Val& Val::operator=(const Val& iOther)
	{
	ZAny::operator=((const ZAny&)iOther);
	return *this;
	}

Val::Val(const ZAny& iOther)
:	ZAny(iOther)
	{}

Val::Val(int32 iVal)
:	ZAny(iVal)
	{}

Val::Val(double iVal)
:	ZAny(iVal)
	{}

Val::Val(bool iVal)
:	ZAny(iVal)
	{}

Val::Val(const string8& iVal)
:	ZAny(iVal)
	{}

Val::Val(const Data& iVal)
:	ZAny(iVal)
	{}

Val::Val(const UnitFloat& iVal)
:	ZAny(iVal)
	{}

Val::Val(const Enumerated& iVal)
:	ZAny(iVal)
	{}

Val::Val(const FileRef& iVal)
:	ZAny(iVal)
	{}

Val::Val(const Seq& iVal)
:	ZAny(iVal)
	{}

Val::Val(const Map& iVal)
:	ZAny(iVal)
	{}

Val::Val(const Spec& iVal)
:	ZAny(iVal)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * Val typename accessors

ZMACRO_ZValAccessors_Def_Entry(Val, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(Val, Double, double)
ZMACRO_ZValAccessors_Def_Entry(Val, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(Val, String, string8)
ZMACRO_ZValAccessors_Def_Entry(Val, Data, Data)
ZMACRO_ZValAccessors_Def_Entry(Val, ClassID, ClassID)
ZMACRO_ZValAccessors_Def_Entry(Val, UnitFloat, UnitFloat)
ZMACRO_ZValAccessors_Def_Entry(Val, Enumerated, Enumerated)
ZMACRO_ZValAccessors_Def_Entry(Val, FileRef, FileRef)
ZMACRO_ZValAccessors_Def_Entry(Val, Seq, Seq)
ZMACRO_ZValAccessors_Def_Entry(Val, Map, Map)
ZMACRO_ZValAccessors_Def_Entry(Val, Spec, Spec)

// =================================================================================================
#pragma mark -
#pragma mark * Seq

Seq::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

ZAny Seq::AsAny() const
	{ return this->AsAny(ZAny()); }

ZAny Seq::AsAny(const ZAny& iDefault) const
	{
	ZSeq_Any theList;
	if (size_t theCount = this->Count())
		{
		for (size_t x = 0; x < theCount; ++x)
			theList.Append(this->Get(x).AsAny(iDefault));
		}
	return ZAny(theList);
	}

void Seq::swap(Seq& iOther)
	{ std::swap(fAL, iOther.fAL); }

Seq::Seq()
	{ spPSActionList->Make(&fAL); }

Seq::Seq(const Seq& iOther)
:	fAL(spDuplicate(iOther.fAL))
	{}

Seq::~Seq()
	{ spPSActionList->Free(fAL); }

Seq& Seq::operator=(const Seq& iOther)
	{
	if (this != &iOther)
		{
		spPSActionList->Free(fAL);
		fAL = spDuplicate(iOther.fAL);
		}
	return *this;
	}

Seq::Seq(PIActionList iOther)
:	fAL(spDuplicate(iOther))
	{}

Seq::Seq(Adopt_T<PIActionList> iOther)
:	fAL(iOther.Get())
	{}

Seq& Seq::operator=(PIActionList iOther)
	{
	spPSActionList->Free(fAL);
	fAL = spDuplicate(iOther);
	return *this;
	}

Seq& Seq::operator=(Adopt_T<PIActionList> iOther)
	{
	spPSActionList->Free(fAL);
	fAL = iOther.Get();
	return *this;
	}

size_t Seq::Count() const
	{
	uint32 result;
	if (noErr == spPSActionList->GetCount(fAL, &result))
		return result;
	return 0;
	}

void Seq::Clear()
	{
	spPSActionList->Free(fAL);
	spPSActionList->Make(&fAL);	
	}

bool Seq::QGet(size_t iIndex, Val& oVal) const
	{
	if (iIndex >= this->Count())
		return false;

	TypeID theType;
	if (noErr != spPSActionList->GetType(fAL, iIndex, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(spPSActionList, fAL, iIndex)
		default:
			ZUnimplemented();
		}
	return false;
	}

Val Seq::DGet(const Val& iDefault, size_t iIndex) const
	{
	Val result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Val Seq::Get(size_t iIndex) const
	{ return this->DGet(Val(), iIndex); }

Seq& Seq::Append(const Val& iVal)
	{
	SETTERCASES(spPSActionList, fAL)
	return *this;
	}

PIActionList& Seq::OParam()
	{
	if (fAL)
		spPSActionList->Free(fAL);
	fAL = nullptr;
	return fAL;
	}

PIActionList Seq::IParam() const
	{ return fAL; }

PIActionList Seq::Orphan()
	{
	PIActionList result = fAL;
	fAL = nullptr;
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Map

Map::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->pCount()); }

ZAny Map::AsAny() const
	{ return this->AsAny(ZAny()); }

ZAny Map::AsAny(const ZAny& iDefault) const
	{
	ZMap_Any theMap;
	for (Index_t i = this->Begin(), end = this->End(); i != end; ++i)
		theMap.Set(this->NameOf(i), this->Get(i).AsAny(iDefault));
	return ZAny(theMap);
	}

void Map::swap(Map& iOther)
	{ std::swap(fAD, iOther.fAD); }

Map::Map()
:	fType(typeObject)
	{ spPSActionDescriptor->Make(&fAD); }

Map::Map(const Map& iOther)
:	fType(iOther.fType)
,	fAD(spDuplicate(iOther.fAD))
	{}

Map::~Map()
	{
	if (fAD)
		spPSActionDescriptor->Free(fAD);
	}

Map& Map::operator=(const Map& iOther)
	{
	if (this != &iOther)
		{
		spPSActionDescriptor->Free(fAD);
		fType = iOther.fType;
		fAD = spDuplicate(iOther.fAD);
		}
	return *this;
	}

Map::Map(KeyID iType, const Map& iOther)
:	fType(iType)
,	fAD(spDuplicate(iOther.fAD))
	{}

Map::Map(const string8& iType, const Map& iOther)
:	fType(spAsKeyID(iType))
,	fAD(spDuplicate(iOther.fAD))
	{}

Map::Map(KeyID iType, PIActionDescriptor iOther)
:	fType(iType)
,	fAD(spDuplicate(iOther))
	{}

Map::Map(const string8& iType, PIActionDescriptor iOther)
:	fType(spAsKeyID(iType))
,	fAD(spDuplicate(iOther))
	{}

Map::Map(KeyID iType, Adopt_T<PIActionDescriptor> iOther)
:	fType(iType)
,	fAD(iOther.Get())
	{}

Map::Map(const string8& iType, Adopt_T<PIActionDescriptor> iOther)
:	fType(spAsKeyID(iType))
,	fAD(iOther.Get())
	{}

void Map::Clear()
	{ spPSActionDescriptor->Clear(fAD); }

bool Map::QGet(KeyID iKey, Val& oVal) const
	{
	if (!fAD)
		return false;

	TypeID theType;
	if (noErr != spPSActionDescriptor->GetType(fAD, iKey, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(spPSActionDescriptor, fAD, iKey)
		default:
			ZUnimplemented();
		}
	return false;
	}

bool Map::QGet(const string8& iName, Val& oVal) const
	{ return this->QGet(spAsKeyID(iName), oVal); }

bool Map::QGet(Index_t iIndex, Val& oVal) const
	{ return this->QGet(this->KeyOf(iIndex), oVal); }

Val Map::DGet(const Val& iDefault, KeyID iKey) const
	{
	Val result;
	if (this->QGet(iKey, result))
		return result;
	return iDefault;
	}

Val Map::DGet(const Val& iDefault, const string8& iName) const
	{
	Val result;
	if (this->QGet(iName, result))
		return result;
	return iDefault;
	}

Val Map::DGet(const Val& iDefault, Index_t iIndex) const
	{
	Val result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Val Map::Get(KeyID iKey) const
	{ return this->DGet(Val(), iKey); }

Val Map::Get(const string8& iName) const
	{ return this->DGet(Val(), iName); }

Val Map::Get(Index_t iIndex) const
	{ return this->DGet(Val(), iIndex); }

Map& Map::Set(KeyID iKey, const Val& iVal)
	{
	#define COMMA() ,

	SETTERCASES(spPSActionDescriptor, fAD COMMA() iKey)

	#undef COMMA

	return *this;
	}

Map& Map::Set(const string8& iName, const Val& iVal)
	{
	this->Set(spAsKeyID(iName), iVal);
	return *this;
	}

Map& Map::Set(Index_t iIndex, const Val& iVal)
	{
	this->Set(this->KeyOf(iIndex), iVal);
	return *this;
	}

Map& Map::Erase(KeyID iKey)
	{
	spPSActionDescriptor->Erase(fAD, iKey);
	return *this;
	}

Map& Map::Erase(const string8& iName)
	{
	this->Erase(spAsKeyID(iName));
	return *this;
	}

Map& Map::Erase(Index_t iIndex)
	{
	this->Erase(this->KeyOf(iIndex));
	return *this;
	}

PIActionDescriptor& Map::OParam()
	{
	if (fAD)
		spPSActionDescriptor->Free(fAD);
	fAD = nullptr;
	return fAD;
	}

PIActionDescriptor Map::IParam() const
	{ return fAD; }

Map::Index_t Map::Begin() const
	{ return Index_t(0); }

Map::Index_t Map::End() const
	{ return Index_t(this->pCount()); }

KeyID Map::KeyOf(Index_t iIndex) const
	{
	if (iIndex.GetIndex() < this->pCount())
		{
		KeyID result;
		if (noErr == spPSActionDescriptor->GetKey(fAD, iIndex.GetIndex(), &result))
			return result;
		}
	return 0;	
	}

string8 Map::NameOf(Index_t iIndex) const
	{
	if (KeyID theKey = this->KeyOf(iIndex))
		return spAsString(theKey);
	return string8();
	}

Map::Index_t Map::IndexOf(KeyID iKey) const
	{
	size_t count = this->pCount();
	for (size_t x = 0; x < count; ++x)
		{
		KeyID theKey;
		if (noErr == spPSActionDescriptor->GetKey(fAD, x, &theKey))
			{
			if (theKey == iKey)
				return Index_t(x);
			}
		}
	return Index_t(count);
	}

Map::Index_t Map::IndexOf(const string8& iName) const
	{ return this->IndexOf(spAsKeyID(iName)); }

Map::Index_t Map::IndexOf(const Map& iOther, const Index_t& iOtherIndex) const
	{
	if (this == &iOther)
		return iOtherIndex;

	const KeyID theKey = iOther.KeyOf(iOtherIndex);
	if (theKey == this->KeyOf(iOtherIndex))
		return iOtherIndex;

	return this->IndexOf(theKey);
	}

KeyID Map::GetType() const
	{ return fType; }

PIActionDescriptor Map::Orphan()
	{
	PIActionDescriptor result = fAD;
	fAD = nullptr;
	return result;
	}

size_t Map::pCount() const
	{
	uint32 result;
	if (noErr == spPSActionDescriptor->GetCount(fAD, &result))
		return result;
	return 0;
	}

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
