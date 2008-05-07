/* ------------------------------------------------------------
Copyright (c) 2001 Andrew Green and Learning in Motion, Inc.
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------ */

#include "ZUtil_Asset.h"

#include "ZAsset_MacOS.h"
#include "ZAsset_POSIX.h"
#include "ZAsset_Std.h"
#include "ZAsset_Win.h"
#include "ZFile_POSIX.h"
#include "ZStream_Mac.h"
#include "ZStream_PageBuffered.h"
#include "ZString.h" // For pascal string conversions

using std::string;
using std::vector;

/**
\namespace ZUtil_Asset
\brief Functions mainly concerned with instantiating assets from data in the executable
file or in external files.
\ingroup group_Asset
*/

// =================================================================================================

#if ZCONFIG(OS, POSIX)

#include <sys/types.h> // For lseek
#include <fcntl.h> // For open
#include <unistd.h> // For lseek

static const char sMagicText[] = "ZooLib Appended Data v1.0";
static const size_t sMagicTextSize = sizeof(sMagicText);

static bool sGetAssetTreeInfoFromExecutable(const string& iName, int& oFD, size_t& oStart, size_t& oLength)
	{
	ZRef<ZFileLoc_POSIX> theFileLoc = ZFileLoc_POSIX::sGet_App();
	if (!theFileLoc)
		return false;

	string thePath = theFileLoc->pGetPath();

	oFD = ::open(thePath.c_str(), O_RDONLY);

	if (oFD < 0)
		return false;

	try
		{
		// We don't want theStream to adopt the file descriptor \a oFD.
		ZStreamRPos_File_POSIX theStream(oFD, false);

		size_t theSize = theStream.GetSize();
		const size_t trailerSize = sMagicTextSize + sizeof(int32) + sizeof(int32);

		// Check for there being at least enough room for an empty trailer.
		if (theSize >= trailerSize)
			{
			// There's enough space for our trailer, consisting of the magic text
			// and the two 32 bit offsets to asset data and the table of contents.
			theStream.SetPosition(theSize - trailerSize);
			size_t offsetOfData = theStream.ReadInt32();
			size_t offsetOfTOC = theStream.ReadInt32();
			if (theSize >= trailerSize + offsetOfData)
				{
				// The data offset is within the file.
				char checkedText[sMagicTextSize];
				theStream.Read(checkedText, sMagicTextSize);
				if (0 == memcmp(checkedText, sMagicText, sMagicTextSize))
					{
					// The magic text matches.
					if (theSize >= trailerSize + offsetOfTOC)
						{
						// The table of contents is also within the file.
						theStream.SetPosition(theSize - trailerSize - offsetOfTOC);
						size_t countOfChunks = theStream.ReadInt32();
						for (size_t x = 0; x < countOfChunks; ++x)
							{
							oStart = theStream.ReadInt32() + theSize - trailerSize - offsetOfData;
							oLength = theStream.ReadInt32();
							string currentName;
							if (size_t nameLength = theStream.ReadInt32())
								{
								currentName.resize(nameLength);
								theStream.Read(&currentName[0], nameLength);
								}
							if (iName == currentName)
								return true;
							}
						}
					}
				}
			}
		}
	catch (...)
		{}
	::close(oFD);
	return false;
	}

#endif // ZCONFIG(OS, POSIX)

// =================================================================================================

#if ZCONFIG(OS, Win32)

#include "ZUnicode.h"
#include "ZUtil_Win.h"

#if defined(__MINGW32__)

// MinGW screws up its headers -- callback functions for A and W have the same signatures,
// which is okay if you're only building for one or the other. We build for both, so we
// provide a corrected definition.

typedef ENUMRESNAMEPROC ENUMRESNAMEPROCA;
typedef ENUMRESNAMEPROC ENUMRESNAMEPROCW;

#if 0
extern "C"
{
typedef BOOL (CALLBACK* ENUMRESNAMEPROCA)(HMODULE hModule, LPCSTR lpType, LPSTR lpName, LONG_PTR lParam);
WINBASEAPI BOOL WINAPI EnumResourceNamesA(IN HMODULE hModule, IN LPCSTR lpType, IN ENUMRESNAMEPROCA lpEnumFunc, IN LONG_PTR lParam );
}

#endif
#endif //  defined(__MINGW32__)

static BOOL CALLBACK sEnumResNameCallbackW(HMODULE iHMODULE, const UTF16* iType, UTF16* iName, LONG iParam)
	{
	if (iName[0])
		reinterpret_cast<vector<string>*>(iParam)->push_back(ZUnicode::sAsUTF8(iName));

	return TRUE;
	}

static BOOL CALLBACK sEnumResNameCallbackA(HMODULE iHMODULE, const char* iType, char* iName, LONG iParam)
	{
	if (iName[0])
		reinterpret_cast<vector<string>*>(iParam)->push_back(iName);

	return TRUE;
	}

#endif // ZCONFIG(OS, Win32)

// =================================================================================================

#if ZCONFIG(OS, Be)
#	include <app/Application.h>
#	include <storage/Resources.h>
#endif

#if ZCONFIG(OS, MacOSX)
#	include <CarbonCore/Resources.h>
#elif ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOS7)
#	include <Resources.h>
#endif

// =================================================================================================

/**
\fn void ZUtil_Asset::sGetAssetTreeNamesFromExecutable(vector<string>&)

\brief Puts the names of all assets in the executable into \a oAssetTreeNames.

As part of the build process multiple zao chunks can be incorporated
into the executable. On Mac and Windows it's done by creating resources of
type 'ZAO_' with whatever names you choose.

On POSIX zac, the ZooLib Asset Compiler, physically appends the zao chunks
to the executable, followed by a table of contents and then a trailer containing
key offsets and magic text. The file thus looks like this:

\verbatim
Regular executable data.
(Normal end of file, everything following is simply 'bolted' onto
the end of the file, and thus will get lost when strip is used)
zao chunk 0
zao chunk 1
...
zao chunk n
(Table of contents)
	Count of chunks
		Distance from start of chunk 0 to start of this chunk
		Number of bytes in chunk
		Length of chunk name
		Chunk name
(Trailer)
	Distance from trailer start to first chunk
	Distance from trailer start to table of contents
	Magic text: "ZooLib Appended Data v1.0"
\endverbatim
*/
void ZUtil_Asset::sGetAssetTreeNamesFromExecutable(vector<string>& oAssetTreeNames)
	{
	oAssetTreeNames.clear();

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

	short count = ::CountResources('ZAO_');
	::SetResLoad(false);
	for (short index = 1; index <= count; ++index)
		{
		if (Handle theResourceHandle = ::GetIndResource('ZAO_', index))
			{
			short theResourceID;
			ResType theResourceType;
			Str255 theResourceName;
			::GetResInfo(theResourceHandle, &theResourceID, &theResourceType, theResourceName);
			string resName = ZString::sFromPString(theResourceName);
			if (resName.size())
				{
				vector<string>::iterator theIter = lower_bound(oAssetTreeNames.begin(), oAssetTreeNames.end(), resName);
				if (theIter == oAssetTreeNames.end() || *theIter != resName)
					oAssetTreeNames.insert(theIter, resName);
				}
			}
		}
	::SetResLoad(true);

#elif ZCONFIG(OS, Win32)

	if (ZUtil_Win::sUseWAPI())
		{
		::EnumResourceNamesW(::GetModuleHandleW(0), L"ZAO_",
			(ENUMRESNAMEPROCW)sEnumResNameCallbackW, reinterpret_cast<LONG>(&oAssetTreeNames));
		}
	else
		{
		::EnumResourceNamesA(::GetModuleHandleA(0), "ZAO_",
			(ENUMRESNAMEPROCA)sEnumResNameCallbackA, reinterpret_cast<LONG>(&oAssetTreeNames));
		}

#elif ZCONFIG(OS, POSIX)

	if (ZRef<ZStreamerRPos> theStreamer = ZFileSpec::sApp().OpenRPos())
		{
		const ZStreamRPos& theStream = theStreamer->GetStreamRPos();
		size_t theSize = theStream.GetSize();
		size_t trailerSize = sMagicTextSize + sizeof(int32) + sizeof(int32);
		// Check for there being at least enough room for an empty trailer
		if (theSize >= trailerSize)
			{
			// Look for our trailer
			theStream.SetPosition(theSize - trailerSize);
			size_t offsetOfData = theStream.ReadInt32();
			size_t offsetOfTOC = theStream.ReadInt32();
			if (theSize >= trailerSize + offsetOfData)
				{
				char checkedText[sMagicTextSize];
				theStream.Read(checkedText, sMagicTextSize);
				if (0 == memcmp(checkedText, sMagicText, sMagicTextSize))
					{
					theStream.SetPosition(theSize - trailerSize - offsetOfTOC);
					size_t countOfChunks = theStream.ReadInt32();
					for (size_t x = 0; x < countOfChunks; ++x)
						{
						theStream.ReadInt32(); // Skip the data offset
						theStream.ReadInt32(); // Skip the data length
						size_t nameLength = theStream.ReadInt32();
						string currentName;
						currentName.resize(nameLength);
						theStream.Read(&currentName[0], nameLength);
						oAssetTreeNames.push_back(currentName);
						}
					}
				}
			}
		}

#elif ZCONFIG(OS, Be)
#endif
	}

// =================================================================================================

/**
\fn ZRef<ZAssetTree> ZUtil_Asset::sGetAssetTreeFromExecutable(const string&)

\brief Return a new ZAssetTree instance referencing the zao data
named \a iAssetTreeName in the executable.

It is important to recognize that each invocation of this method creates
a \em new instance of ZAssetTree. It may therefore load or map all the data contained in
the tree.  Your application will probably call this method indirectly by calling
ZUtil_Asset::sGetAssetRootFromExecutable, but it should do so only once, keeping the asset
in a global static or in an instance variable of your application object (or equivalent).

\sa ZUtil_Asset::sGetAssetTreeNamesFromExecutable
*/
ZRef<ZAssetTree> ZUtil_Asset::sGetAssetTreeFromExecutable(const string& iAssetTreeName)
	{
#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon) || ZCONFIG(OS, MacOSX)

	Str255 theResourceName;
	ZString::sToPString(iAssetTreeName, theResourceName, 255);

	if (ZCONFIG(OS, MacOSX))
		{
		// This variant sucks the entire resource into memory, which is what
		// MacOS X does anyway even when SetResLoad is false.
		if (Handle theResourceHandle = ::GetNamedResource('ZAO_', theResourceName))
			return new ZAssetTree_MacOS_Resource(theResourceHandle, true);
		}
	else
		{
		// This variant uses the partial resource API to randomly access the resource
		// without pulling the entire contents into memory. We also interpose a
		// ZStreamerRPos_PageBuffered with eight 1K buffers to improve the performance
		// of small reads.
		::SetResLoad(false);
		Handle theResourceHandle = ::GetNamedResource('ZAO_', theResourceName);
		::SetResLoad(true);
		if (theResourceHandle)
			{
			ZRef<ZStreamerRPos> theStreamer = new ZStreamerRPos_Mac_PartialResource(theResourceHandle, true);
			theStreamer = new ZStreamerRPos_PageBuffered(8, 1024, theStreamer);
			return new ZAssetTree_Std_Streamer(theStreamer);
			}
		}

#elif ZCONFIG(OS, Win32)

	HINSTANCE theHINSTANCE;
	if (ZUtil_Win::sUseWAPI())
		theHINSTANCE = ::GetModuleHandleW(nil);
	else
		theHINSTANCE = ::GetModuleHandleA(nil);

	return new ZAssetTree_Win_MultiResource(theHINSTANCE, "ZAO_", iAssetTreeName);

#elif ZCONFIG(OS, POSIX)

	int theFD;
	size_t theStart, theLength;
	if (sGetAssetTreeInfoFromExecutable(iAssetTreeName, theFD, theStart, theLength))
		return new ZAssetTree_POSIX_MemoryMapped(theFD, true, theStart, theLength);

#elif ZCONFIG(OS, Be)

	// AG 2000-01-28. I know, I know, this is not thread safe.
	static BResources* sBResources = nil;
	if (!sBResources)
		{
		app_info info;
		if (B_OK == be_roster->GetRunningAppInfo(be_app->Team(), &info))
			{
			BFile theBFile(&info.ref, O_RDONLY);
			if (B_OK == theBFile.InitCheck())
				sBResources = new BResources(&theBFile);
			}
		}

	if (sBResources)
		{
		size_t theSize;
		if (const void* theResourceData = sBResources->LoadResource((type_code)'ZAO_', iAssetTreeName.c_str(), &theSize))
			return new ZAssetTree_Std_Memory_StaticData(theResourceData, theSize);
		}

#endif // ZCONFIG(OS)

	return ZRef<ZAssetTree>();
	}


/**
\fn ZAsset ZUtil_Asset::sGetAssetRootFromExecutable(const string&)

\brief Return an asset representing the root asset in the tree in the
executable file named \a iAssetTreeName.

\sa ZUtil_Asset::sGetAssetTreeFromExecutable.
*/
ZAsset ZUtil_Asset::sGetAssetRootFromExecutable(const string& iAssetTreeName)
	{
	if (ZRef<ZAssetTree> theAssetTree = sGetAssetTreeFromExecutable(iAssetTreeName))
		return theAssetTree->GetRoot();
	return ZAsset();
	}


/**
\fn ZRef<ZAssetTree> ZUtil_Asset::sGetAssetTreeFromFileSpec(const ZFileSpec&)

\brief Return a new ZAssetTree instance referencing the zao data in the
file referenced by \a iFileSpec.

It is important to recognize that each invocation of this method creates
a \em new instance of ZAssetTree. It may therefore load or map all the data contained in
the tree.  Your application will probably call this method indirectly by calling
ZUtil_Asset::sGetAssetRootFromFileSpec, but it should do so only once, keeping the asset
in a global static or in an instance variable of your application object (or equivalent).

\sa ZUtil_Asset::sGetAssetTreeNamesFromExecutable
*/
ZRef<ZAssetTree> ZUtil_Asset::sGetAssetTreeFromFileSpec(const ZFileSpec& iFileSpec)
	{
	ZRef<ZAssetTree> theAssetTree;

#if ZCONFIG(OS, MacOS7) || ZCONFIG(OS, Carbon)
	try
		{
		if (ZRef<ZStreamerRPos> theStreamer = iFileSpec.OpenRPos())
			theAssetTree = new ZAssetTree_Std_Streamer(new ZStreamerRPos_PageBuffered(8, 1024, theStreamer));
		}
	catch (...)
		{}

#elif ZCONFIG(OS, Win32)

	try
		{
		string thePath = iFileSpec.AsString_Native();
		HANDLE theFileHANDLE;
		if (ZUtil_Win::sUseWAPI())
			{
			theFileHANDLE = ::CreateFileW(ZUnicode::sAsUTF16(thePath).c_str(), // the path
											GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
											nil, // No security attributes
											OPEN_EXISTING, // Open the file only if it exists
											FILE_ATTRIBUTE_NORMAL, // No special attributes
											nil);// No template file
			}
		else
			{
			theFileHANDLE = ::CreateFileA(thePath.c_str(), // the path
											GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
											nil, // No security attributes
											OPEN_EXISTING, // Open the file only if it exists
											FILE_ATTRIBUTE_NORMAL, // No special attributes
											nil);// No template file
			}

		if (theFileHANDLE != INVALID_HANDLE_VALUE)
			{
			DWORD sizeHigh;	
			DWORD sizeLow = ::GetFileSize(theFileHANDLE, &sizeHigh);
			theAssetTree = new ZAssetTree_Win_MemoryMapped(theFileHANDLE, true, 0, sizeLow);
			}
		}
	catch (...)
		{}

#elif ZCONFIG(OS, POSIX)

	try
		{
		int theFD = ::open(iFileSpec.AsString_Native().c_str(), O_RDONLY);
		if (theFD >= 0)
			{
			int theLength = ::lseek(theFD, 0, SEEK_END);
			theAssetTree = new ZAssetTree_POSIX_MemoryMapped(theFD, true, 0, theLength);
			}
		}
	catch (...)
		{}

#elif ZCONFIG(OS, Be)

#endif // ZCONFIG(OS)

	if (!theAssetTree)
		{
		// We failed to get the asset tree instantiated, either because we just fell through
		// to here directly (BeOS), or because an exception occurred.
		if (ZRef<ZStreamerRPos> theStreamer = iFileSpec.OpenRPos())
			{
			try
				{
				theAssetTree = new ZAssetTree_Std_Streamer(theStreamer);
				}
			catch (...)
				{}
			}
		}

	return theAssetTree;
	}


/**
\fn ZAsset ZUtil_Asset::sGetAssetRootFromFileSpec(const ZFileSpec&)

\brief Return an asset representing the root asset in the tree
in the file referenced by \a iFileSpec.

\sa ZUtil_Asset::sGetAssetTreeFromFileSpec
*/
ZAsset ZUtil_Asset::sGetAssetRootFromFileSpec(const ZFileSpec& iFileSpec)
	{
	if (ZRef<ZAssetTree> theAssetTree = sGetAssetTreeFromFileSpec(iFileSpec))
		return theAssetTree->GetRoot();
	return ZAsset();
	}

ZAsset ZUtil_Asset::sCreateOverlay(const vector<ZAsset>& iAssets, bool iLowestToHighest)
	{
	if (iAssets.size() == 0)
		return ZAsset();

	if (iAssets.size() == 1)
		return iAssets.front();

	// ZAssetRep_Overlay requires that we give it a list of asset reps, ordered from
	// highest priority to lowest. If \param iLowestToHighest is true then we have to
	// build the vector in reverse order.

	vector<ZRef<ZAssetRep> > assetReps;
	if (iLowestToHighest)
		{
		for (vector<ZAsset>::const_reverse_iterator i = iAssets.rbegin(); i != iAssets.rend(); ++i)
			{
			if (ZRef<ZAssetRep> theRep = (*i).GetRep())
				assetReps.push_back(theRep);
			}
		}
	else
		{
		for (vector<ZAsset>::const_iterator i = iAssets.begin(); i != iAssets.end(); ++i)
			{
			if (ZRef<ZAssetRep> theRep = (*i).GetRep())
				assetReps.push_back(theRep);
			}
		}
	
	return ZAsset(new ZAssetRep_Overlay(assetReps));
	}
