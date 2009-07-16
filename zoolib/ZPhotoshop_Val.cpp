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
#include "zoolib/ZMemory.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUnicode.h"

#include "ASZStringSuite.h"

#include "PIHandleSuite.h"
#include "PITerminology.h"
#include "PIUSuites.h"

using std::vector;

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop suites

static AutoSuite<PSActionDescriptorProcs>
	sPSActionDescriptor(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);

static AutoSuite<PSActionControlProcs>
	sPSActionControl(kPSActionControlSuite, kPSActionControlSuitePrevVersion);

static AutoSuite<PSActionReferenceProcs>
	sPSActionReference(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion);

static AutoSuite<PSActionListProcs>
	sPSActionList(kPSActionListSuite, kPSActionListSuiteVersion);

#ifdef kPSAliasSuite
	static AutoSuite<PSAliasSuite>
		sPSAlias(kPSAliasSuite, kPSAliasSuiteVersion1);
#endif

static AutoSuite<ASZStringSuite>
	sASZString(kASZStringSuite, kASZStringSuiteVersion1);

static AutoSuite<PSHandleSuite2>
	sPSHandle(kPSHandleSuite, kPSHandleSuiteVersion2);

// =================================================================================================
#pragma mark -
#pragma mark * ZRef support

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

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop

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

	ClassID theType;
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

static TypeID sAsRuntimeTypeID(const string8& iString)
	{
	TypeID theTypeID;
	if (sAsRuntimeTypeID(iString, theTypeID))
		return theTypeID;
	return 0;
	}

static string8 sFromRuntimeTypeID(TypeID iTypeID)
	{
	string8 result;
	if (sFromRuntimeTypeID(iTypeID, result))
		return result;
	return string8();
	}

static KeyID sAsKeyID(const string8& iName)
	{ return sAsRuntimeTypeID(iName); }

static string8 sAsString(KeyID iKeyID)
	{ return sFromRuntimeTypeID(iKeyID); }

string8 sWinToPOSIX(const string8& iWin)
	{
	ZTrail theTrail("\\", "", "", iWin);
	string8 result;
	if (theTrail.Count() > 0)
		{
		result += "/" + theTrail.At(0).substr(0, 1) + "/";
		if (theTrail.Count() > 1)
			result += theTrail.SubTrail(1).AsString();
		}
	return result;
	}

string8 sPOSIXToWin(const string8& iPOSIX)
	{
	ZTrail theTrail = iPOSIX;
	string8 result;
	if (theTrail.Count() > 0)
		{
		result = theTrail.At(0) + ":\\";
		if (theTrail.Count() > 1)
			result += theTrail.SubTrail(1).AsString("\\", "");
		}
	return result;
	}

string8 sHFSToPOSIX(const string8& iHFS)
	{
	ZTrail theTrail(":", "", "", iHFS);
	theTrail = "Volumes" + theTrail;
	return "/" + theTrail.AsString();
	}


// =================================================================================================
#pragma mark -
#pragma mark * Public utilities

bool sAsRuntimeTypeID(const string8& iString, TypeID& oTypeID)
	{
	if (noErr == sPSActionControl->StringIDToTypeID(const_cast<char*>(iString.c_str()), &oTypeID))
		return true;
	return false;
	}

bool sFromRuntimeTypeID(TypeID iTypeID, string8& oString)
	{
	char buf[1024];
	if (noErr == sPSActionControl->TypeIDToStringID(iTypeID, buf, sizeof(buf)-1))
		{
		oString = buf;
		return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Enumerated

Enumerated::Enumerated(EnumTypeID iEnumType, const string8& iValue)
:	fEnumType(iEnumType),
	fValue(sAsRuntimeTypeID(iValue))
	{}

Enumerated::Enumerated(const string8& iEnumType, EnumID iValue)
:	fEnumType(sAsRuntimeTypeID(iEnumType)),
	fValue(iValue)
	{}

Enumerated::Enumerated(const string8& iEnumType, const string8& iValue)
:	fEnumType(sAsRuntimeTypeID(iEnumType)),
	fValue(sAsRuntimeTypeID(iValue))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * FileRef

static Handle sHandleDuplicate(Handle iHandle)
	{
	if (!iHandle)
		return nullptr;

	size_t theSize = sPSHandle->GetSize(iHandle);
	Handle result = sPSHandle->New(theSize);
	ZBlockCopy(iHandle[0], result[0], theSize);
	return result;
	}

static void sHandleDispose(Handle iHandle)
	{
	if (iHandle)
		sPSHandle->Dispose(iHandle);
	}

FileRef::FileRef()
:	fHandle(nullptr)
	{}

FileRef::FileRef(const FileRef& iOther)
:	fHandle(sHandleDuplicate(iOther.fHandle))
	{}

FileRef::~FileRef()
	{ sHandleDispose(fHandle); }

FileRef& FileRef::operator=(const FileRef& iOther)
	{
	if (this != &iOther)
		{
		sHandleDispose(fHandle);
		fHandle = sHandleDuplicate(iOther.fHandle);
		}
	return *this;
	}

FileRef::FileRef(Handle iHandle)
:	fHandle(sHandleDuplicate(iHandle))
	{}

FileRef::FileRef(Adopt_T<Handle> iOther)
:	fHandle(iOther.Get())
	{}

FileRef& FileRef::operator=(Handle iHandle)
	{
	sHandleDispose(fHandle);
	fHandle = sHandleDuplicate(iHandle);
	return *this;
	}

FileRef& FileRef::operator=(Adopt_T<Handle> iOther)
	{
	sHandleDispose(fHandle);
	fHandle = iOther.Get();
	return *this;
	}

FileRef::FileRef(const string8& iPathPOSIX)
:	fHandle(nullptr)
	{
	#ifndef kPSAliasSuite
		// We didn't pick up kPSAliasSuite from the include of PIUSuites.h,
		// so we must be on the old SDK.
		
		FSRef theFSRef;
		Boolean isDir;
		if (noErr == ::FSPathMakeRef((const UInt8*)iPathPOSIX.c_str(), &theFSRef, &isDir))
			::FSNewAliasMinimal(&theFSRef, (AliasHandle*)&fHandle);
		
	#else
		#ifdef __PIMac__

			sPSAlias->MacNewAliasFromCString(iPathPOSIX.c_str(), (AliasHandle*)&fHandle);

		#elif defined(__PIWin__)

			string16 asWin = ZUnicode::sAsUTF16(sPOSIXToWin(iPathPOSIX));
			
			sPSAlias->WinNewAliasFromWidePath((uint16*)asWin.c_str(), &fHandle);		

		#else

			#error Unsupported platform

		#endif
	#endif
	}

Handle FileRef::Get() const
	{ return fHandle; }

Handle FileRef::Orphan()
	{
	Handle result = fHandle;
	fHandle = 0;
	return result;
	}

Handle& FileRef::OParam()
	{
	sHandleDispose(fHandle);
	fHandle = nullptr;
	return fHandle;
	}

string8 FileRef::AsPathPOSIX() const
	{
	string8 thePath = this->pAsString();

	#if defined(__PIWin__)
		thePath = sWinToPOSIX(thePath);
	#endif

	return thePath;
	}

string8 FileRef::AsPathNative() const
	{
	return this->pAsString();
	}

string8 FileRef::pAsString()  const
	{
	#ifdef __PIMac__

		FSRef theFSRef;
		Boolean wasChanged = false;
		if (noErr == ::FSResolveAlias(nullptr, (AliasHandle)fHandle, &theFSRef, &wasChanged))
			{
			char pathBuf[1024];
			if (noErr == ::FSRefMakePath(&theFSRef, (UInt8*)pathBuf, sizeof(pathBuf)))
				return pathBuf;
			}

	#elif defined(__PIWin__)

		if (size_t theSize = sPSHandle->GetSize(fHandle))
			{
			Ptr pointer;
			Boolean oldLock;
			sPSHandle->SetLock(fHandle, true, &pointer, &oldLock);

			string8 result;
			if (pointer)
				result = (char*)pointer;

			sPSHandle->SetLock(fHandle, oldLock, &pointer, &oldLock);
			return result;
			}
	
	#else

		#error Unsupported platform

	#endif

	return string8();
	}

// =================================================================================================
#pragma mark -
#pragma mark * Spec::Entry

Spec::Entry::Entry()
:	fClassID(0),
	fFormID(0)
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
	fClassID = iOther.fClassID;
	fFormID = iOther.fFormID;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Spec

Spec Spec::sClass(ClassID iClassID)
	{ return Entry::sClass(iClassID); }

Spec Spec::sEnum(ClassID iClassID, EnumTypeID iEnumType, EnumID iValue)
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

Spec Spec::sProperty(ClassID iClassID, const string8& iName)
	{ return Entry::sProperty(iClassID, sAsRuntimeTypeID(iName)); }

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
		OSErr theErr = sPSActionControl->Get(&result.OParam(), theRef);
		if (noErr != theErr)
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
					if (noErr != sPSActionReference->PutEnumerated(theRef, theEntry.fClassID,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fEnumType,
						sFetch_T<Enumerated>(theEntry.fData.fBytes)->fValue))
						{ allOK = false; }
					break;
					}
				case formIdentifier:
					{
					if (noErr != sPSActionReference->PutIdentifier(theRef, theEntry.fClassID,
						theEntry.fData.fAsIdentifier))
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
					if (noErr != sPSActionReference->PutName(theRef, theEntry.fClassID,
						const_cast<char*>(theName.c_str())))
						{ allOK = false; }
					break;
					}
				case formOffset:
					{
					if (noErr != sPSActionReference->PutOffset(theRef, theEntry.fClassID,
						theEntry.fData.fAsOffset))
						{ allOK = false; }
					break;
					}
				case formProperty:
					{
					if (noErr != sPSActionReference->PutProperty(theRef, theEntry.fClassID,
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
						string8 theName(0, theLength + 1);
						if (noErr != sPSActionReference->GetName(iRef,
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
		PIActionReference container;
		if (noErr != sPSActionReference->GetContainer(iRef, &container))
			container = nullptr;
		if (freeRef)
			sPSActionReference->Free(iRef);
		iRef = container;
		freeRef = true;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * Val

Val::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fType); }

void Val::swap(Val& iOther)
	{
	const size_t theSize = sizeof(fData);

	struct dummy { char bytes[theSize]; };

	dummy& self = *(dummy*)(fData.fBytes);
	dummy& other = *(dummy*)(iOther.fData.fBytes);

	dummy temp = self;
	self = other;
	other = temp;
	}

Val::Val()
:	fType(0)
	{}

Val::Val(const Val& iOther)
	{ this->pCopy(iOther); }

Val::~Val()
	{ this->pRelease(); }

Val& Val::operator=(const Val& iOther)
	{
	if (this != &iOther)
		{
		this->pRelease();
		this->pCopy(iOther);
		}
	return *this;
	}

Val::Val(int32 iVal)
	{
	fData.fAsInt32 = iVal;
	fType = typeInteger;
	}

Val::Val(double iVal)
	{
	fData.fAsDouble = iVal;
	fType = typeFloat;
	}

Val::Val(bool iVal)
	{
	fData.fAsBool = iVal;
	fType = typeBoolean;
	}

Val::Val(const string8& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeChar;
	}

Val::Val(const ZValData_ZooLib& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeRawData;
	}

Val::Val(UnitFloat iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeUnitFloat;
	}

Val::Val(Enumerated iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeEnumerated;
	}

Val::Val(const FileRef& iFileRef)
	{
	sConstruct_T(fData.fBytes, iFileRef);
	fType = typePlatformFilePath;
	}

Val::Val(const List& iVal)
	{
	sConstruct_T(fData.fBytes, iVal);
	fType = typeValueList;
	}

Val::Val(const Map& iVal)
	{
	sConstruct_T<>(fData.fBytes, iVal);
	fType = typeObject;
	}

Val::Val(const Spec& iVal)
	{
	sConstruct_T<>(fData.fBytes, iVal);
	fType = typeObjectSpecifier;
	}

void Val::Clear()
	{
	this->pRelease();
	fType = 0;
	}

template <>
bool Val::QGet_T<int32>(int32& oVal) const
	{
	if (typeInteger == fType)
		{
		oVal = fData.fAsInt32;
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<double>(double& oVal) const
	{
	if (typeFloat == fType)
		{
		oVal = fData.fAsDouble;
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<bool>(bool& oVal) const
	{
	if (typeBoolean == fType)
		{
		oVal = fData.fAsBool;
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<string8>(string8& oVal) const
	{
	if (typeChar == fType)
		{
		oVal = *sFetch_T<string8>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<ZValData_ZooLib>(ZValData_ZooLib& oVal) const
	{
	if (typeRawData == fType)
		{
		oVal = *sFetch_T<ZValData_ZooLib>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<UnitFloat>(UnitFloat& oVal) const
	{
	if (typeUnitFloat == fType)
		{
		oVal = *sFetch_T<UnitFloat>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<Enumerated>(Enumerated& oVal) const
	{
	if (typeEnumerated == fType)
		{
		oVal = *sFetch_T<Enumerated>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<FileRef>(FileRef& oVal) const
	{
	if (typePath == fType || typeAlias == fType)
		{
		oVal = *sFetch_T<FileRef>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<List>(List& oVal) const
	{
	if (typeValueList == fType)
		{
		oVal = *sFetch_T<List>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<Map>(Map& oVal) const
	{
	if (typeObject == fType || typeGlobalObject == fType)
		{
		oVal = *sFetch_T<Map>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
bool Val::QGet_T<Spec>(Spec& oVal) const
	{
	if (typeObjectSpecifier == fType)
		{
		oVal = *sFetch_T<Spec>(fData.fBytes);
		return true;
		}
	return false;
	}

template <>
void Val::Set_T<int32>(const int32& iVal)
	{
	this->pRelease();
	fData.fAsInt32 = iVal;
	fType = typeInteger;
	}

template <>
void Val::Set_T<bool>(const bool& iVal)
	{
	this->pRelease();
	fData.fAsBool = iVal;
	fType = typeBoolean;
	}

template <>
void Val::Set_T<double>(const double& iVal)
	{
	this->pRelease();
	fData.fAsDouble = iVal;
	fType = typeFloat;
	}

template <>
void Val::Set_T<string8>(const string8& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeChar;
	}

template <>
void Val::Set_T<ZValData_ZooLib>(const ZValData_ZooLib& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeRawData;
	}

template <>
void Val::Set_T<UnitFloat>(const UnitFloat& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeUnitFloat;
	}

template <>
void Val::Set_T<Enumerated>(const Enumerated& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeEnumerated;
	}

template <>
void Val::Set_T<FileRef>(const FileRef& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typePlatformFilePath;
	}

template <>
void Val::Set_T<List>(const List& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeValueList;
	}

template <>
void Val::Set_T<Map>(const Map& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeObject;
	}

template <>
void Val::Set_T<Spec>(const Spec& iVal)
	{
	this->pRelease();
	sConstruct_T(fData.fBytes, iVal);
	fType = typeObjectSpecifier;
	}

void Val::pRelease()
	{
	const KeyID theType = fType;
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
			sDestroy_T<ZValData_ZooLib>(fData.fBytes);
			break;
			}
		case typePath:
		case typeAlias:
			{
			sDestroy_T<FileRef>(fData.fBytes);
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

void Val::pCopy(const Val& iOther)
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
			sCopyConstruct_T<ZValData_ZooLib>(iOther.fData.fBytes, fData.fBytes);
			break;
			}
		case typePath:
		case typeAlias:
			{
			sCopyConstruct_T<FileRef>(iOther.fData.fBytes, fData.fBytes);
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

// =================================================================================================
#pragma mark -
#pragma mark * Val typename accessors

ZMACRO_ZValAccessors_Def_Entry(Val, Int32, int32)
ZMACRO_ZValAccessors_Def_Entry(Val, Double, double)
ZMACRO_ZValAccessors_Def_Entry(Val, Bool, bool)
ZMACRO_ZValAccessors_Def_Entry(Val, String, string8)
ZMACRO_ZValAccessors_Def_Entry(Val, Data, ZValData_ZooLib)
ZMACRO_ZValAccessors_Def_Entry(Val, UnitFloat, UnitFloat)
ZMACRO_ZValAccessors_Def_Entry(Val, Enumerated, Enumerated)
ZMACRO_ZValAccessors_Def_Entry(Val, FileRef, FileRef)
ZMACRO_ZValAccessors_Def_Entry(Val, List, List)
ZMACRO_ZValAccessors_Def_Entry(Val, Map, Map)
ZMACRO_ZValAccessors_Def_Entry(Val, Spec, Spec)

// =================================================================================================
#pragma mark -
#pragma mark * List and Map Getter/Setter stuff

#define COMMA() ,

#define GETTERCASES(SUITE, PARAM) \
	case typeInteger: { int32 theVal; \
		if (noErr == SUITE->GetInteger(PARAM, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeFloat: { double theVal; \
		if (noErr == SUITE->GetFloat(PARAM, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeUnitFloat: { UnitFloat theVal; \
		if (noErr == SUITE->GetUnitFloat(PARAM, &theVal.fUnitID, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; } \
	case typeChar: \
		{ \
		uint32 theLength; \
		if (noErr == SUITE->GetStringLength(PARAM, &theLength)) \
			{ \
			string8 result(size_t(theLength + 1), ' '); \
			if (0 == theLength || noErr == SUITE->GetString(PARAM, &result[0], theLength + 1)) \
				{ \
				oVal = result.substr(0, theLength); \
				return true; \
				} \
			} \
		break; \
		} \
	case typeBoolean: { Boolean theVal; \
		if (noErr == SUITE->GetBoolean(PARAM, &theVal)) \
			{ oVal = bool(theVal); return true; } \
		break; } \
	case typeValueList: { PIActionList theVal; \
		if (noErr == SUITE->GetList(PARAM, &theVal)) \
			{ oVal = List(Adopt(theVal)); return true; } \
		break; } \
	case typeObject: { \
		ClassID theDCID; \
		PIActionDescriptor theVal; \
		if (noErr == SUITE->GetObject(PARAM, &theDCID, &theVal)) \
			{ oVal = Map(theDCID, Adopt(theVal)); return true; } \
		break; } \
	/* global object? */ \
	case typeEnumerated: { Enumerated theVal; \
		if (noErr == SUITE->GetEnumerated(PARAM, &theVal.fEnumType, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
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
	case typeClass: \
		{ \
		ZUnimplemented(); \
		break; \
		} \
	/* global class? */ \
	case typeAlias: \
	case typePath: { FileRef theVal; \
		if (noErr == SUITE->GetAlias(PARAM, &theVal.OParam())) \
			{ oVal = theVal; return true; } \
		break; } \
	case typeRawData: \
		{ \
		int32 theLength; \
		if (noErr == SUITE->GetDataLength(PARAM, &theLength)) \
			{ \
			Data result(theLength); \
			if (0 == theLength || noErr == SUITE->GetData(PARAM, result.GetData())) \
				{ \
				oVal = result; \
				return true; \
				} \
			} \
		break; \
		} \


#define SETTERCASES(SUITE, PARAM) \
	case typeInteger: { SUITE->PutInteger(PARAM, iVal.fData.fAsInt32); return; } \
	case typeFloat: { SUITE->PutFloat(PARAM, iVal.fData.fAsDouble); return; } \
	case typeUnitFloat: \
		{ \
		SUITE->PutUnitFloat(PARAM, \
			sFetch_T<UnitFloat>(iVal.fData.fBytes)->fUnitID, \
			sFetch_T<UnitFloat>(iVal.fData.fBytes)->fValue); \
		return; \
		} \
	case typeChar: \
		{ \
		SUITE->PutString(PARAM, \
			const_cast<char*>(sFetch_T<string8>(iVal.fData.fBytes)->c_str())); \
		return; \
		} \
	case typeBoolean: { SUITE->PutBoolean(PARAM, iVal.fData.fAsBool); return; } \
	case typeValueList: \
		{ \
		SUITE->PutList(PARAM, \
			sFetch_T<List>(iVal.fData.fBytes)->GetActionList()); \
		return; \
		} \
	case typeObject: \
		{ \
		SUITE->PutObject(PARAM, \
			sFetch_T<Map>(iVal.fData.fBytes)->GetType(), \
			sFetch_T<Map>(iVal.fData.fBytes)->IParam()); \
		return; \
		} \
	/* global object? */ \
	case typeEnumerated: \
		{ \
		SUITE->PutEnumerated(PARAM, \
			sFetch_T<Enumerated>(iVal.fData.fBytes)->fEnumType, \
			sFetch_T<Enumerated>(iVal.fData.fBytes)->fValue); \
		return; \
		} \
	case typeObjectSpecifier: \
		{ \
		PIActionReference tempRef = sFetch_T<Spec>(iVal.fData.fBytes)->MakeRef(); \
		SUITE->PutReference(PARAM, tempRef); \
		sPSActionReference->Free(tempRef); \
		return; \
		} \
	case typeClass: \
		{ \
		ZUnimplemented(); \
		break; \
		} \
	/* global class? */ \
	case typePath: \
		{ \
		SUITE->PutAlias(PARAM, \
			sFetch_T<FileRef>(iVal.fData.fBytes)->Get()); \
		return; \
		} \
	case typeRawData: \
		{ \
		SUITE->PutData(PARAM, \
			sFetch_T<Data>(iVal.fData.fBytes)->GetSize(), \
			const_cast<void*>(sFetch_T<Data>(iVal.fData.fBytes)->GetData())); \
		return; \
		} \

// =================================================================================================
#pragma mark -
#pragma mark * List

List::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

void List::swap(List& iOther)
	{ std::swap(fAL, iOther.fAL); }

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

List::List(Adopt_T<PIActionList> iOther)
:	fAL(iOther.Get())
	{}

List& List::operator=(PIActionList iOther)
	{
	sPSActionList->Free(fAL);
	fAL = sDuplicate(iOther);
	return *this;
	}

List& List::operator=(Adopt_T<PIActionList> iOther)
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

bool List::QGet(size_t iIndex, Val& oVal) const
	{
	if (iIndex >= this->Count())
		return false;

	TypeID theType;
	if (noErr != sPSActionList->GetType(fAL, iIndex, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(sPSActionList, fAL COMMA() iIndex)
		default:
			ZUnimplemented();
		}
	return false;
	}

Val List::DGet(size_t iIndex, const Val& iDefault) const
	{
	Val result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Val List::Get(size_t iIndex) const
	{ return this->DGet(iIndex, Val()); }

void List::Append(const Val& iVal)
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

void Map::swap(Map& iOther)
	{ std::swap(fAD, iOther.fAD); }

Map::Map()
:	fType(typeObject)
	{ sPSActionDescriptor->Make(&fAD); }

Map::Map(const Map& iOther)
:	fType(iOther.fType)
,	fAD(sDuplicate(iOther.fAD))
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
		fType = iOther.fType;
		fAD = sDuplicate(iOther.fAD);
		}
	return *this;
	}

Map::Map(KeyID iType, PIActionDescriptor iOther)
:	fType(iType)
,	fAD(sDuplicate(iOther))
	{}

Map::Map(const string8& iType, PIActionDescriptor iOther)
:	fType(sAsKeyID(iType))
,	fAD(sDuplicate(iOther))
	{}

Map::Map(KeyID iType, Adopt_T<PIActionDescriptor> iOther)
:	fType(iType),
	fAD(iOther.Get())
	{}

Map::Map(const string8& iType, Adopt_T<PIActionDescriptor> iOther)
:	fType(sAsKeyID(iType))
,	fAD(iOther.Get())
	{}

void Map::Clear()
	{ sPSActionDescriptor->Clear(fAD); }

bool Map::QGet(KeyID iKey, Val& oVal) const
	{
	if (!fAD)
		return false;

	TypeID theType;
	if (noErr != sPSActionDescriptor->GetType(fAD, iKey, &theType))
		return false;

	switch (theType)
		{
		GETTERCASES(sPSActionDescriptor, fAD COMMA() iKey)
		default:
			ZUnimplemented();
		}
	return false;
	}

bool Map::QGet(const string8& iName, Val& oVal) const
	{ return this->QGet(sAsKeyID(iName), oVal); }

bool Map::QGet(Index_t iIndex, Val& oVal) const
	{ return this->QGet(this->KeyOf(iIndex), oVal); }

Val Map::DGet(KeyID iKey, const Val& iDefault) const
	{
	Val result;
	if (this->QGet(iKey, result))
		return result;
	return iDefault;
	}

Val Map::DGet(const string8& iName, const Val& iDefault) const
	{
	Val result;
	if (this->QGet(iName, result))
		return result;
	return iDefault;
	}

Val Map::DGet(Index_t iIndex, const Val& iDefault) const
	{
	Val result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Val Map::Get(KeyID iKey) const
	{ return this->DGet(iKey, Val()); }

Val Map::Get(const string8& iName) const
	{ return this->DGet(iName, Val()); }

Val Map::Get(Index_t iIndex) const
	{ return this->DGet(iIndex, Val()); }

void Map::Set(KeyID iKey, const Val& iVal)
	{
	switch (iVal.fType)
		{
		SETTERCASES(sPSActionDescriptor, fAD COMMA() iKey)
		default:
			ZUnimplemented();//?
		}
	}

void Map::Set(const string8& iName, const Val& iVal)
	{ this->Set(sAsKeyID(iName), iVal); }

void Map::Set(Index_t iIndex, const Val& iVal)
	{ this->Set(this->KeyOf(iIndex), iVal); }

void Map::Erase(KeyID iKey)
	{ sPSActionDescriptor->Erase(fAD, iKey); }

void Map::Erase(const string8& iName)
	{ this->Erase(sAsKeyID(iName)); }

void Map::Erase(Index_t iIndex)
	{ this->Erase(this->KeyOf(iIndex)); }

PIActionDescriptor& Map::OParam()
	{
	if (fAD)
		sPSActionDescriptor->Free(fAD);
	fAD = nullptr;
	return fAD;
	}

PIActionDescriptor Map::IParam() const
	{ return fAD; }

Map::Index_t Map::begin() const
	{ return Index_t(0); }

Map::Index_t Map::end() const
	{ return Index_t(this->pCount()); }

KeyID Map::KeyOf(Index_t iIndex) const
	{
	if (iIndex.GetIndex() < this->pCount())
		{
		KeyID result;
		if (noErr == sPSActionDescriptor->GetKey(fAD, iIndex.GetIndex(), &result))
			return result;
		}
	return 0;	
	}

string8 Map::NameOf(Index_t iIndex) const
	{
	if (KeyID theKey = this->KeyOf(iIndex))
		return sAsString(theKey);
	return string8();
	}

Map::Index_t Map::IndexOf(KeyID iKey) const
	{
	size_t count = this->pCount();
	for (size_t x = 0; x < count; ++x)
		{
		KeyID theKey;
		if (noErr == sPSActionDescriptor->GetKey(fAD, x, &theKey))
			{
			if (theKey == iKey)
				return Index_t(x);
			}
		}
	return Index_t(count);
	}

Map::Index_t Map::IndexOf(const string8& iName) const
	{ return this->IndexOf(sAsKeyID(iName)); }

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
	if (noErr == sPSActionDescriptor->GetCount(fAD, &result))
		return result;
	return 0;
	}

#undef COMMA

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
