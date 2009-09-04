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

#include "zoolib/photoshop/ZPhotoshop_FileRef.h"

#include "zoolib/ZMemory.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_CFType.h"

#include "PIDefines.h"
#include "PIGetPathSuite.h"
#include "PIHandleSuite.h"
#include "PIUSuites.h"

NAMESPACE_ZOOLIB_BEGIN

using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * ZPhotoshop suites, for local use

//###undef kPSAliasSuite
#ifdef kPSAliasSuite
	static AutoSuite<PSAliasSuite>
		spPSAlias(kPSAliasSuite, kPSAliasSuiteVersion1);
#endif

static AutoSuite<PSHandleSuite2>
	spPSHandle(kPSHandleSuite, kPSHandleSuiteVersion2);

// =================================================================================================
#pragma mark -
#pragma mark * Helpers

#if defined(__PIMac__)

static ZTrail spMacAsTrail(short iVRefNum, long iDirID, const unsigned char* iName)
	{
	ZTrail theResult;

	for (;;)
		{
		if (iDirID == fsRtParID)
			{
			// Case 1. We've topped out and can bail
			break;
			}
		else if (iDirID == fsRtDirID)
			{
			// Case 3: We're pointing at the top level directory on volume vRefNum.
			Str31 volumeName;
			HParamBlockRec thePB;
			thePB.volumeParam.ioNamePtr = &volumeName[0]; // returns name
			thePB.volumeParam.ioVRefNum = iVRefNum;
			thePB.volumeParam.ioVolIndex = 0;

			if (noErr == PBHGetVInfoSync(&thePB))
				theResult.PrependComp(ZString::sFromPString(&volumeName[0]));
			break;
			}
		else
			{
			// Case 5 -- We're referencing the directory with id iDirID on
			// volume iVRefNum. The branch is therefore the parent of this
			// directory.
			Str31 directoryName;
			CInfoPBRec thePB;
			thePB.dirInfo.ioNamePtr = &directoryName[0];
			thePB.dirInfo.ioFDirIndex = -1;
			thePB.dirInfo.ioVRefNum = iVRefNum;
			thePB.dirInfo.ioDrDirID = iDirID;

			if (noErr != PBGetCatInfoSync(&thePB))
				break;

			theResult.PrependComp(ZString::sFromPString(&directoryName[0]));

			iDirID = thePB.dirInfo.ioDrParID;
			}
		}

	// By handling fName here we avoid cases 2, 4, 6 above.
	if (iName[0])
		theResult.AppendComp(ZString::sFromPString(&iName[0]));

	theResult.PrependComp("Volumes");

	return theResult;
	}

#endif // defined(__PIMac__)

static string8 spTrailAsWin(const ZTrail& iTrail)
	{
	string8 result;
	if (iTrail.Count() > 0)
		{
		result = iTrail.At(0) + ":\\";
		if (iTrail.Count() > 1)
			result += iTrail.SubTrail(1).AsString("\\", "");
		}
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZooLib::ZPhotoshop

namespace ZPhotoshop {

ZTrail sWinAsTrail(const string8& iWin)
	{
	ZTrail result = ZTrail("\\", "", "", iWin);
	if (result.Count())
		{
		const string firstComp = result.At(0);
		if (firstComp.size() > 1)
			{
			if (firstComp[1] == ':')
				result = firstComp.substr(0, 1) + result.SubTrail(1);
			}
		}
	return result;
	}

ZTrail sAsTrail(const SPPlatformFileSpecification& iSpec)
	{
	#if defined(__PIMac__)
		return spMacAsTrail(iSpec.vRefNum, iSpec.parID, iSpec.name);
	#endif

	#if defined(__PIWin__)
		AutoSuite<PSGetPathSuite1> theSPGetPath(kPSGetPathSuite, kPSGetPathSuiteVersion1);
		char buf[1024];
		theSPGetPath->GetPathName(const_cast<SPPlatformFileSpecification*>(&iSpec),
			buf, sizeof(buf));
		return sWinAsTrail(buf);
	#endif
	}

// =================================================================================================
#pragma mark -
#pragma mark * FileRef

static Handle spHandleDuplicate(Handle iHandle)
	{
	if (!iHandle)
		return nullptr;

	size_t theSize = spPSHandle->GetSize(iHandle);
	Handle result = spPSHandle->New(theSize);
	ZBlockCopy(iHandle[0], result[0], theSize);
	return result;
	}

static void spHandleDispose(Handle iHandle)
	{
	if (iHandle)
		spPSHandle->Dispose(iHandle);
	}

FileRef::FileRef()
:	fHandle(nullptr)
	{}

FileRef::FileRef(const FileRef& iOther)
:	fHandle(spHandleDuplicate(iOther.fHandle))
	{}

FileRef::~FileRef()
	{ spHandleDispose(fHandle); }

FileRef& FileRef::operator=(const FileRef& iOther)
	{
	if (this != &iOther)
		{
		spHandleDispose(fHandle);
		fHandle = spHandleDuplicate(iOther.fHandle);
		}
	return *this;
	}

FileRef::FileRef(Handle iHandle)
:	fHandle(spHandleDuplicate(iHandle))
	{}

FileRef::FileRef(Adopt_T<Handle> iOther)
:	fHandle(iOther.Get())
	{}

FileRef& FileRef::operator=(Handle iHandle)
	{
	spHandleDispose(fHandle);
	fHandle = spHandleDuplicate(iHandle);
	return *this;
	}

FileRef& FileRef::operator=(Adopt_T<Handle> iOther)
	{
	spHandleDispose(fHandle);
	fHandle = iOther.Get();
	return *this;
	}

FileRef::FileRef(const ZTrail& iTrail)
:	fHandle(nullptr)
	{
	#ifdef __PIMac__
		// Run the path through CFURL, to expand tildes and so forth.
		ZRef<CFURLRef> theURL =
			Adopt(::CFURLCreateWithFileSystemPath(kCFAllocatorDefault,
			ZUtil_CFType::sString(iTrail.AsString()), kCFURLPOSIXPathStyle, false));

		#if defined(kPSAliasSuite)

			ZRef<CFStringRef> asCFString =
				Adopt(::CFURLCopyFileSystemPath(theURL, kCFURLPOSIXPathStyle));

			const string asPOSIX = ZUtil_CFType::sAsUTF8(asCFString);

			spPSAlias->MacNewAliasFromCString(asPOSIX.c_str(), (AliasHandle*)&fHandle);

		#else

			// We didn't pick up kPSAliasSuite from the include of PIUSuites.h,
			// so we must be on the old SDK, and need to transform iPathPOSIX to
			// an HFS path.			
			
			ZRef<CFStringRef> asCFString =
				Adopt(::CFURLCopyFileSystemPath(theURL, kCFURLHFSPathStyle));

			const string asHFS = ZUtil_CFType::sAsUTF8(asCFString);

			OSErr theErr = ::NewAliasMinimalFromFullPath(
				asHFS.length(), asHFS.c_str(),
				nullptr, nullptr, &(AliasHandle)fHandle);

		#endif

	#elif defined(__PIWin__)

		string16 asWin = ZUnicode::sAsUTF16(spTrailAsWin(iTrail));
		
		spPSAlias->WinNewAliasFromWidePath((uint16*)asWin.c_str(), &fHandle);		

	#else

		#error Unsupported platform

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
	spHandleDispose(fHandle);
	fHandle = nullptr;
	return fHandle;
	}

ZTrail FileRef::AsTrail() const
	{
	#if defined(__PIMac__)

		#if defined(kPSAliasSuite)

			HFSUniStr255 targetName;
			HFSUniStr255 volumeName;
			ZRef<CFStringRef> thePath;
			FSAliasInfoBitmap theFSAIB;
			FSAliasInfo theFSAI;

			OSErr theErr = ::FSCopyAliasInfo(
				(AliasHandle)fHandle,
				&targetName, &volumeName,
				&thePath.GetPtrRef(),
				&theFSAIB, &theFSAI);

			if ((theErr == noErr || theErr == fnfErr) && thePath)
				return ZTrail(ZUtil_CFType::sAsUTF8(thePath));

		#else

			FSSpec result;
			Boolean wasChanged;
			if (noErr == ::ResolveAlias(nullptr, (AliasHandle)fHandle, &result, &wasChanged))
				return spMacAsTrail(result.vRefNum, result.parID, result.name);

		#endif

	#elif defined(__PIWin__)

		if (size_t theSize = spPSHandle->GetSize(fHandle))
			{
			Ptr pointer;
			Boolean oldLock;
			spPSHandle->SetLock(fHandle, true, &pointer, &oldLock);

			ZTrail result(false);
			if (pointer)
				result = sWinAsTrail((const char*)pointer);

			spPSHandle->SetLock(fHandle, oldLock, &pointer, &oldLock);
			return result;
			}
	
	#else

		#error Unsupported platform

	#endif

	return ZTrail(false);
	}

} // namespace ZPhotoshop

NAMESPACE_ZOOLIB_END
