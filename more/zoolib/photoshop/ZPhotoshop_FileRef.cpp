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
#include "zoolib/photoshop/ZPhotoshop_Util.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZMemory.h"
#include "zoolib/ZStream_Memory.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZStrim_Stream.h"
#include "zoolib/ZStdInt.h"
#include "zoolib/ZTextCoder_Win.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUnicode.h"
#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZUtil_string.h"

#include <stdexcept> // For runtime_error
#include <cstdio> // For sscanf

#include "PIDefines.h"
#include "PIGetPathSuite.h"
#include "PIHandleSuite.h"
#include "PIUSuites.h"

// =================================================================================================
// MARK: - Fixup, for building with ancient MW

#ifdef __PIMac__

#include ZMACINCLUDE2(CoreFoundation,CFBundle.h)

#if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) \
	|| MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_2

extern "C" typedef
OSErr
(*FSNewAliasMinimalUnicode_Ptr)(
  const FSRef *    targetParentRef,
  UniCharCount     targetNameLength,
  const UniChar *  targetName,
  AliasHandle *    inAlias,
  Boolean *        isDirectory)            /* can be NULL */;

static OSErr
FSNewAliasMinimalUnicode(
  const FSRef *    targetParentRef,
  UniCharCount     targetNameLength,
  const UniChar *  targetName,
  AliasHandle *    inAlias,
  Boolean *        isDirectory)            /* can be NULL */
	{
	if (CFBundleRef bundleRef = ::CFBundleGetBundleWithIdentifier(CFSTR("com.apple.Carbon")))
		{
		if (FSNewAliasMinimalUnicode_Ptr theProc = (FSNewAliasMinimalUnicode_Ptr)
			CFBundleGetFunctionPointerForName(bundleRef, CFSTR("FSNewAliasMinimalUnicode")))
			{
			return theProc(targetParentRef, targetNameLength, targetName, inAlias, isDirectory);
			}
		}
	return -1;
	}

#endif

#endif // __PIMac__

// =================================================================================================
// MARK: - ZPhotoshop suites, for local use

static AutoSuite<PSHandleSuite2>
	spPSHandle(kPSHandleSuite, kPSHandleSuiteVersion2);


namespace ZooLib {

using std::string;

// =================================================================================================
// MARK: - UseHandle (anonymous)

namespace { // anonymous

class UseHandle
	{
public:
	UseHandle(Handle iHandle);
	~UseHandle();

	void* Ptr() const;
	size_t Size() const;

private:
	Handle fHandle;
	::Ptr fPtr;
	Boolean fOldLock;
	};

UseHandle::UseHandle(Handle iHandle)
:	fHandle(iHandle)
	{ spPSHandle->SetLock(fHandle, true, &fPtr, &fOldLock); }

UseHandle::~UseHandle()
	{ spPSHandle->SetLock(fHandle, fOldLock, &fPtr, &fOldLock); }

void* UseHandle::Ptr() const
	{ return fPtr; }

size_t UseHandle::Size() const
	{ return spPSHandle->GetSize(fHandle); }

} // anonymous namespace

// =================================================================================================
// MARK: - Helpers

static Handle spHandleDuplicate(Handle iHandle)
	{
	if (not iHandle)
		return nullptr;

	size_t theSize = spPSHandle->GetSize(iHandle);
	Handle result = spPSHandle->New(theSize);
	ZMemCopy(UseHandle(result).Ptr(), iHandle[0], theSize);
	return result;
	}

static void spHandleDispose(Handle iHandle)
	{
	if (iHandle)
		spPSHandle->Dispose(iHandle);
	}

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

#if ZCONFIG_SPI_Enabled(Carbon)

static ZTrail spMacAsTrail(short iVRefNum, long iDirID, const unsigned char* iName)
	{
	// If we're on an old PS, we;ll need to convert from MacRoman to UTF-8

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
				theResult.PrependComp(ZUtil_string::sFromPString(&volumeName[0]));
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

			theResult.PrependComp(ZUtil_string::sFromPString(&directoryName[0]));

			iDirID = thePB.dirInfo.ioDrParID;
			}
		}

	// By handling fName here we avoid cases 2, 4, 6 above.
	if (iName[0])
		theResult.AppendComp(ZUtil_string::sFromPString(&iName[0]));

	theResult.PrependComp("Volumes");

	return theResult;
	}

#endif // ZCONFIG_SPI_Enabled(Carbon)

#if ZCONFIG_SPI_Enabled(Carbon64)

static ZQ<ZTrail> spMacAsTrail(const FSRef& iFSRef)
	{
	// Use 1024, PATH_MAX not defined in old Mac headers.
	char buffer[1024];
	if (noErr == ::FSRefMakePath(&iFSRef, (UInt8*)buffer, 1024))
		return ZTrail(buffer);

	return null;
	}

#endif // ZCONFIG_SPI_Enabled(Carbon64)

#if defined(__PIWin__)

static UINT spSystemCodePage()
	{
	static bool sFetched = false;
	static UINT sValue;
	if (!sFetched)
		{
		char theCP[10];
		int length = ::GetLocaleInfoA(
			::GetSystemDefaultLCID(),
			LOCALE_IDEFAULTANSICODEPAGE,
			theCP,
			countof(theCP));

		std::sscanf(theCP, "%d", &sValue);
		sFetched = true;
		}
	return sValue;
	}

#endif // defined(__PIWin__)

// =================================================================================================
// MARK: - ZooLib::ZPhotoshop

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

ZQ<ZTrail> sAsTrail(const SPPlatformFileSpecification& iSpec)
	{
	#if ZCONFIG_SPI_Enabled(Carbon)

		return spMacAsTrail(iSpec.vRefNum, iSpec.parID, iSpec.name);

	#elif ZCONFIG_SPI_Enabled(Carbon64)

		return spMacAsTrail(iSpec.mReference);

	#elif defined(__PIWin__)

		AutoSuite<PSGetPathSuite1> theSPGetPath(kPSGetPathSuite, kPSGetPathSuiteVersion1);
		char buf[1024];
		theSPGetPath->GetPathName(const_cast<SPPlatformFileSpecification*>(&iSpec),
			buf, sizeof(buf));
		return sWinAsTrail(buf);

	#endif
	}

// =================================================================================================
// MARK: - FileRef

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

		FSRef parentFSRef;
		if (noErr != ::FSPathMakeRef(
			(const UInt8*)(("/" + iTrail.Branch().AsString()).c_str()),
			&parentFSRef, nullptr))
			{
			throw std::runtime_error("FileRef::FileRef, couldn't get parentFSRef");
			}

		const string16 leaf = ZUnicode::sAsUTF16(iTrail.Leaf());

		OSErr result = ::FSNewAliasMinimalUnicode(
			&parentFSRef,
			leaf.size(), (const UniChar*)leaf.c_str(),
			(AliasHandle*)&fHandle, nullptr);

		if (result != noErr && result != fnfErr || !fHandle)
			throw std::runtime_error("FileRef::FileRef, couldn't create AliasHandle");

	#elif defined(__PIWin__)

		#ifdef kPSAliasSuite

			const string16 asWin = ZUnicode::sAsUTF16(spTrailAsWin(iTrail));

			AutoSuite<PSAliasSuite> thePSAlias(kPSAliasSuite, kPSAliasSuiteVersion1);
			thePSAlias->WinNewAliasFromWidePath((uint16*)asWin.c_str(), &fHandle);

		#else

			// The only mention of the 'utxt'-tagged format is in this message:
			// <http://forums.adobe.com/message/2356596>

			const string theWinPath = spTrailAsWin(iTrail);
			ZStreamRWPos_RAM buffer;
			if (ZPhotoshop::sGetHostVersion_Major() <= ZCONFIG_Photoshop_SDKVersion_CS1)
				{
				// We're being hosted by an old version of photoshop. Convert our
				// UTF8 string to the 8-bit system codepage.
				ZStrimW_StreamEncoder(new ZTextEncoder_Win(spSystemCodePage()), buffer)
					.Write(theWinPath);

				buffer.WriteInt8(0);

				size_t stringSize = buffer.GetSize();
				fHandle = spPSHandle->New(stringSize);
				buffer.SetPosition(0);
				buffer.Read(UseHandle(fHandle).Ptr(), stringSize);
				}
			else
				{
				// We can use the 'utxt' tagged format.
				ZStrimW_StreamUTF16LE(buffer)
					.Write(theWinPath);

				buffer.WriteInt16(0);

				const uint32 stringSize = buffer.GetSize();
				const uint32 handleSize = stringSize + 12;
				fHandle = spPSHandle->New(handleSize);
				UseHandle useHandle(fHandle);

				uint32* header = static_cast<uint32*>(useHandle.Ptr());
				header[0] = ZFOURCC('u', 't', 'x', 't');
				header[1] = handleSize;
				header[2] = stringSize / 2;
				buffer.SetPosition(0);
				buffer.Read(&header[3], stringSize);
				}

		#endif

	#else

		#error Unsupported platform

	#endif
	}

ZQ<ZTrail> FileRef::AsTrail() const
	{
	#if defined(__PIMac__)

		#if defined(kPSAliasSuite) // <-- This is not the right check, really

			HFSUniStr255 targetName;
			HFSUniStr255 volumeName;
			ZRef<CFStringRef> thePath;
			FSAliasInfoBitmap theFSAIB;
			FSAliasInfo theFSAI;

			OSErr theErr = ::FSCopyAliasInfo(
				(AliasHandle)fHandle,
				&targetName, &volumeName,
				&thePath.OParam(),
				&theFSAIB, &theFSAI);

			if ((theErr == noErr || theErr == fnfErr) && thePath)
				return ZTrail(ZUtil_CF::sAsUTF8(thePath));

		#else

			FSSpec result;
			Boolean wasChanged;
			if (noErr == ::ResolveAlias(nullptr, (AliasHandle)fHandle, &result, &wasChanged))
				return sAsTrail(reinterpret_cast<const SPPlatformFileSpecification&>(result));

		#endif

	#elif defined(__PIWin__)

		// Need to look at this again when we're doing 64 bit compiles.
//##		ZAssertCompile(ZIntTrait_T<sizeof(size_t)>::eIs32Bit);

		UseHandle useHandle(fHandle);
		size_t handleSize = useHandle.Size();
		const uint32* header = static_cast<uint32*>(useHandle.Ptr());
		if (handleSize >= 12)
			{
			// The handle is large enough.
			if (header[0] == ZFOURCC('u', 't', 'x', 't'))
				{
				// It has the 'utxt' prefix.
				if (header[1] == handleSize)
					{
					// Its stored block size matches the handle size.
					if (header[2] == (handleSize - 12) / 2)
						{
						// The count of code units is correct.
						const string8 theWinPath = ZUnicode::sAsUTF8((const UTF16*)&header[3]);
						return sWinAsTrail(theWinPath);
						}
					}
				}
			}
		// The handle did not pass our screening, assume it's
		// an 8 bit string in the system codepage, skipping the NUL terminator.
		ZStreamRPos_Memory theStreamR(header, handleSize - 1);
		const string8 theWinPath =
			ZStrimR_StreamDecoder(new ZTextDecoder_Win(spSystemCodePage()), theStreamR).ReadAll8();
		return sWinAsTrail(theWinPath);

	#else

		#error Unsupported platform

	#endif

	return null;
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
	// When the Handle passed to ActionSuite GetAlias methods contains
	// the bytes 'utxt' that tells PS that it should return a UTF16
	// string, with a special header that distinguishes it from the
	// regular 8-bit encoding using the system codepage.
	fHandle = reinterpret_cast<Handle>('utxt');
	return fHandle;
	}

} // namespace ZPhotoshop
} // namespace ZooLib
