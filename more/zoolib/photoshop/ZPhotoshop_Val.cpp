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
#include "zoolib/ZMemory.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_CFType.h"

#include "ASZStringSuite.h"

#include "PIHandleSuite.h"
#include "PITerminology.h"
#include "PIUSuites.h"

#ifdef __PIMac__
#	if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
#		include ZMACINCLUDE3(ApplicationServices,AE,AEObjects.h)
#	else
#		include ZMACINCLUDE3(CoreServices,AE,AEObjects.h)
#	endif
#endif

#include <map>

// =================================================================================================
#pragma mark -
#pragma mark * Fixup, for building with ancient MW

#ifdef __PIMac__

#include ZMACINCLUDE2(CoreFoundation,CFBundle.h)

#if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_2

typedef UInt32                          FSAliasInfoBitmap;
enum {
  kFSAliasInfoNone              = 0x00000000, /* no valid info*/
  kFSAliasInfoVolumeCreateDate  = 0x00000001, /* volume creation date is valid*/
  kFSAliasInfoTargetCreateDate  = 0x00000002, /* target creation date is valid*/
  kFSAliasInfoFinderInfo        = 0x00000004, /* file type and creator are valid*/
  kFSAliasInfoIsDirectory       = 0x00000008, /* isDirectory boolean is valid*/
  kFSAliasInfoIDs               = 0x00000010, /* parentDirID and nodeID are valid*/
  kFSAliasInfoFSInfo            = 0x00000020, /* filesystemID and signature are valid*/
  kFSAliasInfoVolumeFlags       = 0x00000040 /* volumeIsBootVolume, volumeIsAutomounted, volumeIsEjectable and volumeHasPersistentFileIDs are valid*/
};

/* info block to pass to FSCopyAliasInfo */
struct FSAliasInfo {
  UTCDateTime         volumeCreateDate;
  UTCDateTime         targetCreateDate;
  OSType              fileType;
  OSType              fileCreator;
  UInt32              parentDirID;
  UInt32              nodeID;
  UInt16              filesystemID;
  UInt16              signature;
  Boolean             volumeIsBootVolume;
  Boolean             volumeIsAutomounted;
  Boolean             volumeIsEjectable;
  Boolean             volumeHasPersistentFileIDs;
  Boolean             isDirectory;
};

extern "C" typedef
OSStatus (*FSCopyAliasInfo_Ptr)(
	AliasHandle          inAlias,
	HFSUniStr255 *       targetName,       /* can be NULL */
	HFSUniStr255 *       volumeName,       /* can be NULL */
	CFStringRef *        pathString,       /* can be NULL */
	FSAliasInfoBitmap *  whichInfo,        /* can be NULL */
	FSAliasInfo *        info)             /* can be NULL */;

static OSStatus FSCopyAliasInfo(
	AliasHandle          inAlias,
	HFSUniStr255 *       targetName,       /* can be NULL */
	HFSUniStr255 *       volumeName,       /* can be NULL */
	CFStringRef *        pathString,       /* can be NULL */
	FSAliasInfoBitmap *  whichInfo,        /* can be NULL */
	FSAliasInfo *        info)             /* can be NULL */
	{
	if (CFBundleRef bundleRef = ::CFBundleGetBundleWithIdentifier(CFSTR("com.apple.Carbon")))
		{
		if (FSCopyAliasInfo_Ptr theProc = (FSCopyAliasInfo_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("FSCopyAliasInfo")))
			{
			return theProc(inAlias, targetName, volumeName, pathString, whichInfo, info);
			}
		}
	return -1;
	}

#endif
#endif // __PIMac__

// =================================================================================================
#pragma mark -
#pragma mark * Using statements

NAMESPACE_ZOOLIB_BEGIN

using std::map;
using std::pair;

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop suites, for local use

static AutoSuite<PSActionDescriptorProcs>
	spPSActionDescriptor(kPSActionDescriptorSuite, kPSActionDescriptorSuiteVersion);

static AutoSuite<PSActionControlProcs>
	spPSActionControl(kPSActionControlSuite, kPSActionControlSuitePrevVersion);

static AutoSuite<PSActionReferenceProcs>
	spPSActionReference(kPSActionReferenceSuite, kPSActionReferenceSuiteVersion);

static AutoSuite<PSActionListProcs>
	spPSActionList(kPSActionListSuite, kPSActionListSuiteVersion);

#ifdef kPSAliasSuite
	static AutoSuite<PSAliasSuite>
		spPSAlias(kPSAliasSuite, kPSAliasSuiteVersion1);
#endif

static AutoSuite<ASZStringSuite>
	spASZString(kASZStringSuite, kASZStringSuiteVersion1);

static AutoSuite<PSHandleSuite2>
	spPSHandle(kPSHandleSuite, kPSHandleSuiteVersion2);

// =================================================================================================
#pragma mark -
#pragma mark * ZRef support

template <>
void sRetain_T(ASZByteRun* iString)
	{
	if (iString)
		spASZString->AddRef(iString);
	}

template <>
void sRelease_T(ASZByteRun* iString)
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

static PIActionList spDuplicate(PIActionList iSource)
	{
	PIActionList dummy;
	spPSActionList->Make(&dummy);

	spPSActionList->PutList(dummy, iSource);

	PIActionList result;
	spPSActionList->GetList(dummy, 0, &result);

	spPSActionList->Free(dummy);

	return result;	
	}

static PIActionDescriptor spDuplicate(PIActionDescriptor iSource)
	{
	PIActionList dummy;
	spPSActionList->Make(&dummy);

	spPSActionList->PutObject(dummy, typeObject, iSource);

	DescriptorClassID theType;
	PIActionDescriptor result;
	spPSActionList->GetObject(dummy, 0, &theType, &result);

	spPSActionList->Free(dummy);

	return result;	
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
:	fEnumType(iEnumType),
	fValue(spAsRuntimeTypeID(iValue))
	{}

Enumerated::Enumerated(const string8& iEnumType, EnumID iValue)
:	fEnumType(spAsRuntimeTypeID(iEnumType)),
	fValue(iValue)
	{}

Enumerated::Enumerated(const string8& iEnumType, const string8& iValue)
:	fEnumType(spAsRuntimeTypeID(iEnumType)),
	fValue(spAsRuntimeTypeID(iValue))
	{}

// =================================================================================================
#pragma mark -
#pragma mark * FileRef

static Handle sHandleDuplicate(Handle iHandle)
	{
	if (!iHandle)
		return nullptr;

	size_t theSize = spPSHandle->GetSize(iHandle);
	Handle result = spPSHandle->New(theSize);
	ZBlockCopy(iHandle[0], result[0], theSize);
	return result;
	}

static void sHandleDispose(Handle iHandle)
	{
	if (iHandle)
		spPSHandle->Dispose(iHandle);
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
	#else
		#ifdef __PIMac__

			spPSAlias->MacNewAliasFromCString(iPathPOSIX.c_str(), (AliasHandle*)&fHandle);

		#elif defined(__PIWin__)

			string16 asWin = ZUnicode::sAsUTF16(sPOSIXToWin(iPathPOSIX));
			
			spPSAlias->WinNewAliasFromWidePath((uint16*)asWin.c_str(), &fHandle);		

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

		HFSUniStr255 targetName;
		HFSUniStr255 volumeName;
		ZRef<CFStringRef> thePath;
		FSAliasInfoBitmap theFSAIB = 0;
		FSAliasInfo theFSAI;

		OSErr theErr = ::FSCopyAliasInfo(
			(AliasHandle)fHandle,
			&targetName, &volumeName,
			&thePath.GetPtrRef(),
			&theFSAIB, &theFSAI);

		if (theErr == noErr || theErr == fnfErr)
			{
			return ZUtil_CFType::sAsUTF8(thePath);
			}

	#elif defined(__PIWin__)

		if (size_t theSize = spPSHandle->GetSize(fHandle))
			{
			Ptr pointer;
			Boolean oldLock;
			spPSHandle->SetLock(fHandle, true, &pointer, &oldLock);

			string8 result;
			if (pointer)
				result = (char*)pointer;

			spPSHandle->SetLock(fHandle, oldLock, &pointer, &oldLock);
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
:	fDCI(0),
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
						string8 theName(0, theLength + 1);
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

Val::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(fAny.type() != typeid(void)); }

ZAny Val::AsAny() const
	{
	if (const Map* theVal = ZAnyCast<Map>(&fAny))
		return theVal->AsAny();

	if (const List* theVal = ZAnyCast<List>(&fAny))
		return theVal->AsAny();
		
	return fAny;
	}

void Val::swap(Val& iOther)
	{ std::swap(fAny, iOther.fAny); }

Val::Val()
	{}

Val::Val(const Val& iOther)
:	fAny(iOther.fAny)
	{}

Val::~Val()
	{}

Val& Val::operator=(const Val& iOther)
	{
	fAny = iOther.fAny;
	return *this;
	}

Val::Val(int32 iVal) : fAny(iVal) {}

Val::Val(double iVal) : fAny(iVal) {}

Val::Val(bool iVal) : fAny(iVal) {}

Val::Val(const string8& iVal) : fAny(iVal) {}

Val::Val(const Data& iVal) : fAny(iVal) {}

Val::Val(const UnitFloat& iVal) : fAny(iVal) {}

Val::Val(const Enumerated& iVal) : fAny(iVal) {}

Val::Val(const FileRef& iVal) : fAny(iVal) {}

Val::Val(const List& iVal) : fAny(iVal) {}

Val::Val(const Map& iVal) : fAny(iVal) {}

Val::Val(const Spec& iVal) : fAny(iVal) {}

void Val::Clear()
	{ fAny = ZAny(); }

template <class S>
bool Val::QGet_T(S& oVal) const
	{
	if (const S* theVal = ZAnyCast<S>(&fAny))
		{
		oVal = *theVal;
		return true;
		}
	return false;
	}

template <class S>
S Val::DGet_T(const S& iDefault) const
	{
	if (const S* theVal = ZAnyCast<S>(&fAny))
		return *theVal;
	return iDefault;
	}

template <class S>
S Val::Get_T() const
	{
	if (const S* theVal = ZAnyCast<S>(&fAny))
		return *theVal;
	return S();
	}

template <class S>
void Val::Set_T(const S& iVal)
	{
	fAny = iVal;
	}

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
ZMACRO_ZValAccessors_Def_Entry(Val, List, List)
ZMACRO_ZValAccessors_Def_Entry(Val, Map, Map)
ZMACRO_ZValAccessors_Def_Entry(Val, Spec, Spec)

// =================================================================================================
#pragma mark -
#pragma mark * List and Map Getter/Setter stuff

#define GETTERCASES(SUITE, P0, P1) \
	case typeInteger: { int32 theVal; \
		if (noErr == SUITE->GetInteger(P0, P1, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeFloat: { double theVal; \
		if (noErr == SUITE->GetFloat(P0, P1, &theVal)) { oVal = theVal; return true; } \
		break; } \
	case typeUnitFloat: { UnitFloat theVal; \
		if (noErr == SUITE->GetUnitFloat(P0, P1, &theVal.fUnitID, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; } \
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
	case typeBoolean: { Boolean theVal; \
		if (noErr == SUITE->GetBoolean(P0, P1, &theVal)) \
			{ oVal = bool(theVal); return true; } \
		break; } \
	case typeValueList: { PIActionList theVal; \
		if (noErr == SUITE->GetList(P0, P1, &theVal)) \
			{ oVal = List(Adopt(theVal)); return true; } \
		break; } \
	case typeObject: { \
		DescriptorClassID theDCID; \
		PIActionDescriptor theVal; \
		if (noErr == SUITE->GetObject(P0, P1, &theDCID, &theVal)) \
			{ oVal = Map(theDCID, Adopt(theVal)); return true; } \
		break; } \
	/* global object? */ \
	case typeEnumerated: { Enumerated theVal; \
		if (noErr == SUITE->GetEnumerated(P0, P1, &theVal.fEnumType, &theVal.fValue)) \
			{ oVal = theVal; return true; } \
		break; } \
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
	case typeClass: \
		{ \
		ZUnimplemented(); \
		break; \
		} \
	/* global class? */ \
	case typeAlias: \
	case typePath: { FileRef theVal; \
		if (noErr == SUITE->GetAlias(P0, P1, &theVal.OParam())) \
			{ oVal = theVal; return true; } \
		break; } \
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
	else if (const int32* theVal = ZAnyCast<int32>(&iVal.fAny)) \
		{ SUITE->PutInteger(PARAM, *theVal); return; } \
	else if (const float* theVal = ZAnyCast<float>(&iVal.fAny)) \
		{ SUITE->PutFloat(PARAM, *theVal); return; } \
	else if (const UnitFloat* theVal = ZAnyCast<UnitFloat>(&iVal.fAny)) \
		{ SUITE->PutUnitFloat(PARAM, theVal->fUnitID, theVal->fValue); return; } \
	else if (const string8* theVal = ZAnyCast<string8>(&iVal.fAny)) \
		{ SUITE->PutString(PARAM, const_cast<char*>(theVal->c_str())); return; } \
	else if (const bool* theVal = ZAnyCast<bool>(&iVal.fAny)) \
		{ SUITE->PutBoolean(PARAM, *theVal); return; } \
	else if (const List* theVal = ZAnyCast<List>(&iVal.fAny)) \
		{ SUITE->PutList(PARAM, theVal->GetActionList()); return; } \
	else if (const Map* theVal = ZAnyCast<Map>(&iVal.fAny)) \
		{ SUITE->PutObject(PARAM, theVal->GetType(), theVal->IParam()); return; } \
	/* global object? */ \
	else if (const Enumerated* theVal = ZAnyCast<Enumerated>(&iVal.fAny)) \
		{ SUITE->PutEnumerated(PARAM, theVal->fEnumType, theVal->fValue); return; } \
	else if (const Spec* theVal = ZAnyCast<Spec>(&iVal.fAny)) \
		{ \
		PIActionReference tempRef = theVal->MakeRef(); \
		SUITE->PutReference(PARAM, tempRef); \
		spPSActionReference->Free(tempRef); \
		return; \
		} \
/*Hmmm	else if (const ClassID* theVal = ZAnyCast<ClassID>(&iVal.fAny)) \
		{ ZUnimplemented(); } Hmm??? SUITE->PutInteger(PARAM, *theVal); return; } */\
	else if (const FileRef* theVal = ZAnyCast<FileRef>(&iVal.fAny)) \
		{ SUITE->PutAlias(PARAM, theVal->Get()); return; } \
	else if (const Data* theVal = ZAnyCast<Data>(&iVal.fAny)) \
		{ SUITE->PutData(PARAM, theVal->GetSize(), const_cast<void*>(theVal->GetData())); return; }

// =================================================================================================
#pragma mark -
#pragma mark * List

List::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->Count()); }

ZAny List::AsAny() const
	{
	vector<ZAny> theVector;
	if (size_t theCount = this->Count())
		{
		theVector.reserve(theCount);
		for (size_t x = 0; x < theCount; ++x)
			theVector.push_back(this->Get(x).AsAny());
		}
	return theVector;
	}

void List::swap(List& iOther)
	{ std::swap(fAL, iOther.fAL); }

List::List()
	{ spPSActionList->Make(&fAL); }

List::List(const List& iOther)
:	fAL(spDuplicate(iOther.fAL))
	{}

List::~List()
	{ spPSActionList->Free(fAL); }

List& List::operator=(const List& iOther)
	{
	if (this != &iOther)
		{
		spPSActionList->Free(fAL);
		fAL = spDuplicate(iOther.fAL);
		}
	return *this;
	}

List::List(PIActionList iOther)
:	fAL(spDuplicate(iOther))
	{}

List::List(Adopt_T<PIActionList> iOther)
:	fAL(iOther.Get())
	{}

List& List::operator=(PIActionList iOther)
	{
	spPSActionList->Free(fAL);
	fAL = spDuplicate(iOther);
	return *this;
	}

List& List::operator=(Adopt_T<PIActionList> iOther)
	{
	spPSActionList->Free(fAL);
	fAL = iOther.Get();
	return *this;
	}

size_t List::Count() const
	{
	uint32 result;
	if (noErr == spPSActionList->GetCount(fAL, &result))
		return result;
	return 0;
	}

void List::Clear()
	{
	spPSActionList->Free(fAL);
	spPSActionList->Make(&fAL);	
	}

bool List::QGet(size_t iIndex, Val& oVal) const
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

Val List::DGet(const Val& iDefault, size_t iIndex) const
	{
	Val result;
	if (this->QGet(iIndex, result))
		return result;
	return iDefault;
	}

Val List::Get(size_t iIndex) const
	{ return this->DGet(Val(), iIndex); }

void List::Append(const Val& iVal)
	{
	SETTERCASES(spPSActionList, fAL)
	}

PIActionList List::GetActionList() const
	{ return fAL; }

// =================================================================================================
#pragma mark -
#pragma mark * Map

Map::operator operator_bool_type() const
	{ return operator_bool_generator_type::translate(this->pCount()); }

ZAny Map::AsAny() const
	{
	map<string, ZAny> theMap;
	for (Index_t i = this->Begin(), end = this->End(); i != end; ++i)
		theMap.insert(pair<string, ZAny>(this->NameOf(i), this->Get(i)));

	return theMap;
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
:	fType(iType),
	fAD(iOther.Get())
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

void Map::Set(KeyID iKey, const Val& iVal)
	{
	#define COMMA() ,

	SETTERCASES(spPSActionDescriptor, fAD COMMA() iKey)

	#undef COMMA
	}

void Map::Set(const string8& iName, const Val& iVal)
	{ this->Set(spAsKeyID(iName), iVal); }

void Map::Set(Index_t iIndex, const Val& iVal)
	{ this->Set(this->KeyOf(iIndex), iVal); }

void Map::Erase(KeyID iKey)
	{ spPSActionDescriptor->Erase(fAD, iKey); }

void Map::Erase(const string8& iName)
	{ this->Erase(spAsKeyID(iName)); }

void Map::Erase(Index_t iIndex)
	{ this->Erase(this->KeyOf(iIndex)); }

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
