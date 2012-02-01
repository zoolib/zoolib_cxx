/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZMacFixup.h"

#include "zoolib/ZUtil_Mac_HL.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include "zoolib/ZDC_QD.h"
#include "zoolib/ZMacOSX.h"
#include "zoolib/ZMemory.h" // For ZMemZero
#include "zoolib/ZUtil_Mac_LL.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,Gestalt.h)
#include ZMACINCLUDE3(CoreServices,CarbonCore,Resources.h)
#include ZMACINCLUDE3(Carbon,HIToolbox,AEInteraction.h)

#include <stdexcept>

using std::max;
using std::runtime_error;
using std::string;

namespace ZooLib {

// =================================================================================================

void ZUtil_Mac_HL::sPixmapsFromCIconHandle(CIconHandle inHandle, ZDCPixmap* outColorPixmap, ZDCPixmap* outMonoPixmap, ZDCPixmap* outMaskPixmap)
	{
	ZAssert(inHandle);

	ZUtil_Mac_LL::HandleLocker locker1((Handle)inHandle);
	// Do the color pixmap first
	if (outColorPixmap)
		{
		ZUtil_Mac_LL::HandleLocker locker2((Handle)inHandle[0]->iconData);
		*outColorPixmap = ZDCPixmap(new ZDCPixmapRep_QD(&inHandle[0]->iconPMap, *inHandle[0]->iconData, inHandle[0]->iconPMap.bounds));
		}
	if (outMonoPixmap)
		{
		long theHeight = inHandle[0]->iconMask.bounds.bottom - inHandle[0]->iconMask.bounds.top;
		long theRowBytes = inHandle[0]->iconMask.rowBytes;
		*outMonoPixmap = ZDCPixmap(new ZDCPixmapRep_QD(&inHandle[0]->iconBMap, &inHandle[0]->iconMaskData[(theRowBytes*theHeight)/2], inHandle[0]->iconBMap.bounds, false));
		}
	// Note that we invert the pixmap -- this is because we use white as the indication that something is opaque
	if (outMaskPixmap)
		*outMaskPixmap = ZDCPixmap(new ZDCPixmapRep_QD(&inHandle[0]->iconMask, &inHandle[0]->iconMaskData, inHandle[0]->iconMask.bounds, true));
	}

CIconHandle ZUtil_Mac_HL::sCIconHandleFromPixmapCombo(const ZDCPixmapCombo& inPixmapCombo)
	{ return sCIconHandleFromPixmaps(inPixmapCombo.GetColor(), inPixmapCombo.GetMono(), inPixmapCombo.GetMask()); }

static void spInvert(void* iAddress, size_t iCount)
	{
	uint8* theAddress = static_cast<uint8*>(iAddress);
	while (iCount--)
		{
		*theAddress = *theAddress ^ 0xFF;
		++theAddress;
		}
	}

CIconHandle ZUtil_Mac_HL::sCIconHandleFromPixmaps(const ZDCPixmap& inColorPixmap, const ZDCPixmap& inMonoPixmap, const ZDCPixmap& inMaskPixmap)
	{
	// We insist on a valid color or mono pixmap and a valid mask
	ZAssert((inColorPixmap || inMonoPixmap) && inMaskPixmap);

	// We conform to the size of the color pixmap
	ZPoint theSize = ZPoint::sZero;
	if (inColorPixmap)
		theSize = inColorPixmap.Size();
	else
		theSize = inMonoPixmap.Size();

	size_t monoRowBytes = ((1 * theSize.h + 31) / 32) * 4;

	CIconHandle theCIconHandle = (CIconHandle)::NewHandleClear(sizeof(CIcon) - sizeof(short) + 2 * monoRowBytes * theSize.v);
	ZUtil_Mac_LL::HandleLocker locker1((Handle)theCIconHandle);

	ZUtil_Mac_LL::sSetupPixMapColor(theSize, 32, theCIconHandle[0]->iconPMap);

	ZDCPixmapNS::RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = 32;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = theCIconHandle[0]->iconPMap.rowBytes & 0x3FFF;
	theRasterDesc.fRowCount = theSize.v;
	theRasterDesc.fFlipped = false;

	// Figure out how much space we need for the color pixmap's data
	theCIconHandle[0]->iconData = ::NewHandleClear(theRasterDesc.fRowBytes * theSize.v);
	ZUtil_Mac_LL::HandleLocker locker2(theCIconHandle[0]->iconData);

	ZDCPixmapNS::PixelDesc thePixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0);
	if (inColorPixmap)
		inColorPixmap.CopyTo(ZPoint(0, 0), theCIconHandle[0]->iconData[0], theRasterDesc, thePixelDesc, ZRect(theSize));
	else
		inMonoPixmap.CopyTo(ZPoint(0, 0), theCIconHandle[0]->iconData[0], theRasterDesc, thePixelDesc, ZRect(theSize));
	// Hmm, we're supposed to invert the color pixmap.

	// We have the color portion set up. Now grab the B & W version, either from inMonoPixmap or from inColorPixmap
	theRasterDesc.fPixvalDesc.fDepth = 1;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = monoRowBytes;
	theRasterDesc.fRowCount = theSize.v;
	theRasterDesc.fFlipped = false;
	thePixelDesc = ZDCPixmapNS::PixelDesc(1, 0);

	theCIconHandle[0]->iconBMap.baseAddr = nullptr;
	theCIconHandle[0]->iconBMap.rowBytes = monoRowBytes;
	theCIconHandle[0]->iconBMap.bounds.left = 0;
	theCIconHandle[0]->iconBMap.bounds.top = 0;
	theCIconHandle[0]->iconBMap.bounds.right = theSize.h;
	theCIconHandle[0]->iconBMap.bounds.bottom = theSize.v;

	void* tempAddress = ((char*)&theCIconHandle[0]->iconMaskData) + monoRowBytes * theSize.v;
	if (inMonoPixmap)
		inMonoPixmap.CopyTo(ZPoint(0, 0), tempAddress, theRasterDesc, thePixelDesc, ZRect(theSize));
	else
		inColorPixmap.CopyTo(ZPoint(0, 0), tempAddress, theRasterDesc, thePixelDesc, ZRect(theSize));

	theCIconHandle[0]->iconMask.baseAddr = nullptr;
	theCIconHandle[0]->iconMask.rowBytes = monoRowBytes;
	theCIconHandle[0]->iconMask.bounds.left = 0;
	theCIconHandle[0]->iconMask.bounds.top = 0;
	theCIconHandle[0]->iconMask.bounds.right = theSize.h;
	theCIconHandle[0]->iconMask.bounds.bottom = theSize.v;

	inMaskPixmap.CopyTo(ZPoint(0, 0), theCIconHandle[0]->iconMaskData, theRasterDesc, thePixelDesc, ZRect(theSize));

	theCIconHandle[0]->iconMask.baseAddr = nullptr;

	return theCIconHandle;
	}

IconFamilyHandle ZUtil_Mac_HL::sNewIconFamily()
	{
	IconFamilyHandle theIconFamilyHandle = (IconFamilyHandle)::NewHandle(sizeof(IconFamilyResource) - sizeof(IconFamilyElement));
	if (theIconFamilyHandle)
		{
		theIconFamilyHandle[0]->resourceType = kIconFamilyType;
		int32 theSize = ::GetHandleSize(reinterpret_cast<Handle>(theIconFamilyHandle));
		theIconFamilyHandle[0]->resourceSize = theSize;
		}
	return theIconFamilyHandle;
	}

void ZUtil_Mac_HL::sDisposeIconFamily(IconFamilyHandle inIconFamilyHandle)
	{
	if (inIconFamilyHandle)
		::DisposeHandle(reinterpret_cast<Handle>(inIconFamilyHandle));
	}

namespace ZUtil_Mac_HL {

static struct _SizeKindResMapping
	{
	IconSize fIconSize;
	IconKind fIconKind;
	uint32 fResType;
	}
sSizeKindResMapping[] = 
	{
	{ eMini, eData1, kMini1BitMask },
	{ eMini, eData4, kMini4BitData },
	{ eMini, eData8, kMini8BitData },
	{ eMini, eMask1, kMini1BitMask },

	{ eSmall, eData1, kSmall1BitMask },
	{ eSmall, eData4, kSmall4BitData },
	{ eSmall, eData8, kSmall8BitData },
	{ eSmall, eData32, kSmall32BitData },
	{ eSmall, eMask1, kSmall1BitMask },
	{ eSmall, eMask8, kSmall8BitMask },

	{ eLarge, eData1, kLarge1BitMask },
	{ eLarge, eData4, kLarge4BitData },
	{ eLarge, eData8, kLarge8BitData },
	{ eLarge, eData32, kLarge32BitData },
	{ eLarge, eMask1, kLarge1BitMask },
	{ eLarge, eMask8, kLarge8BitMask },

	{ eHuge, eData1, kHuge1BitMask },
	{ eHuge, eData4, kHuge4BitData },
	{ eHuge, eData8, kHuge8BitData },
	{ eHuge, eData32, kHuge32BitData },
	{ eHuge, eMask1, kHuge1BitMask },
	{ eHuge, eMask8, kHuge8BitMask }
	};

static struct _KindDepthCTableMapping
	{
	IconKind fIconKind;
	int16 fDepth;
	int16 fCTableID;
	bool fHasMask;
	bool fIsMask;
	bool fNeeds8BitDeepMasks;
	bool fNeeds32BitIcons;
	}
sKindDepthCTableMapping[] = 
	{
	{ eData1, 1, 33, true, false, false, false },
	{ eData4, 4, 68, false, false, false, false },
	{ eData8, 8, 72, false, false, false, false },
	{ eData32, 32, 0, false, false, false, true },
	{ eMask1, 1, 33, true, true, false, false },
	{ eMask8, 8, 40, false, true, true, false }
	};

static struct _SizePixelMapping
	{
	IconSize fIconSize;
	ZCoord fPixels;
	bool fNeeds48PixelIcons;
	}
sSizePixelMapping[] = 
	{
	{ eMini, 12, false },
	{ eSmall, 16, false },
	{ eLarge, 32, false },
	{ eHuge, 48, true }
	};

} // namespace ZUtil_Mac_HL

static bool spGestalt_IconUtilitiesHas48PixelIcons()
	{
	long response;
	OSErr err = ::Gestalt(gestaltIconUtilitiesAttr, &response);
	if (err == noErr)
		return 0 != (response & (1 << gestaltIconUtilitiesHas48PixelIcons));
	return false;
	}

static bool spGestalt_IconUtilitiesHas32BitIcons()
	{
	// AG 2000-07-22. Guess what, the window manager on iMacs (not sure which
	// OS version) doesn't like deep icons either, so this is switched off for now,
	// same as deep masks below. Fixed bug #152.
#if 1 // ##
	long response;
	OSErr err = ::Gestalt(gestaltIconUtilitiesAttr, &response);
	if (err == noErr)
		return 0 != (response & (1 << gestaltIconUtilitiesHas32BitIcons));
#endif // 0
	return false;
	}

static bool spGestalt_IconUtilitiesHas8BitDeepMasks()
	{
	// AG 2000-04-01. Window Manager seems to have problems with 8 bit masks, as in
	// they don't work. So I'm switching this off for now.
#if 1 // ##
	long response;
	OSErr err = ::Gestalt(gestaltIconUtilitiesAttr, &response);
	if (err == noErr)
		return 0 != (response & (1 << gestaltIconUtilitiesHas8BitDeepMasks));
#endif // 0
	return false;
	}

void ZUtil_Mac_HL::sAddPixmapToIconFamily(const ZDCPixmap& inPixmap, IconKind inAsIconKind, IconFamilyHandle inIconFamilyHandle)
	{
// Figure out a bunch of stuff
	ZPoint sourceSize = inPixmap.Size();
	ZCoord sourceWidthHeight = max(sourceSize.h, sourceSize.v);

	bool has48PixelIcons = spGestalt_IconUtilitiesHas48PixelIcons();

	ZCoord theCroppedWidthHeight = 48;
	IconSize theIconSize = eHuge;
	if (!has48PixelIcons)
		{
		theCroppedWidthHeight = 32;
		theIconSize = eLarge;
		}

	for (int x = 0; x < countof(sSizePixelMapping); ++x)
		{
		if (sSizePixelMapping[x].fPixels >= sourceWidthHeight
			&& (!sSizePixelMapping[x].fNeeds48PixelIcons || has48PixelIcons))
			{
			theIconSize = sSizePixelMapping[x].fIconSize;
			theCroppedWidthHeight = sSizePixelMapping[x].fPixels;
			break;
			}
		}

	int16 destDepth = 32;
	int16 destCTableID = 0;
	bool hasMask = false;
	bool isMask = false;
	for (int x = 0; x < countof(sKindDepthCTableMapping); ++x)
		{
		if (sKindDepthCTableMapping[x].fIconKind == inAsIconKind)
			{
			destDepth = sKindDepthCTableMapping[x].fDepth;
			destCTableID = sKindDepthCTableMapping[x].fCTableID;
			hasMask = sKindDepthCTableMapping[x].fHasMask;
			isMask = sKindDepthCTableMapping[x].fIsMask;
			break;
			}
		}

	bool needsInvert = false;
	ZDCPixmapNS::PixelDesc destPixelDesc;
	switch (destDepth)
		{
		case 1:
			{
			needsInvert = !isMask;
			destPixelDesc = ZDCPixmapNS::PixelDesc(1, 0);
			break;
			}
		case 2:
		case 4:
		case 8:
			{
			destPixelDesc = ZUtil_Mac_LL::sGetIndexedPixelDesc(destCTableID);
			break;
			}
		case 16:
			{
			destPixelDesc = ZDCPixmapNS::PixelDesc(0x7C00, 0x03E0, 0x001F, 0x0000);
			break;
			}
		case 24:
			{
			destPixelDesc = ZDCPixmapNS::PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);
			break;
			}
		default:
		case 32:
			{
			destPixelDesc = ZDCPixmapNS::PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);
			break;
			}
		}

	uint32 theResType = 0;
	for (int x = 0; x < countof(sSizeKindResMapping); ++x)
		{
		if (sSizeKindResMapping[x].fIconSize == theIconSize
			&& sSizeKindResMapping[x].fIconKind == inAsIconKind)
			{
			theResType = sSizeKindResMapping[x].fResType;
			break;
			}
		}

	// Note that we *do not* force four byte alignment -- small icons are two bytes wide only,
	// the others will naturally have four byte alignment because of depth or size.
	int16 destRowBytes = ((destDepth * theCroppedWidthHeight) + 7) / 8;

	// Let's see if we already have data (in case we're adding a mask)
	Handle theDataHandle = ::NewHandle(0);
	::GetIconFamilyData(inIconFamilyHandle, theResType, theDataHandle);
	int32 theHandleSize = ::GetHandleSize(theDataHandle);

	if (theHandleSize == 0)
		{
		// There's no extant data in inIconFamilyHandle, so we need to allocate it.
		if (hasMask)
			{
			::SetHandleSize(theDataHandle, 2 * destRowBytes * theCroppedWidthHeight);
			ZMemZero(*theDataHandle, 2 * destRowBytes * theCroppedWidthHeight);
			}
		else
			{
			::SetHandleSize(theDataHandle, destRowBytes * theCroppedWidthHeight);
			ZMemZero(*theDataHandle, destRowBytes * theCroppedWidthHeight);
			}
		}

	ZUtil_Mac_LL::HandleLocker theDataHandleLocker(theDataHandle);

	ZDCPixmapNS::RasterDesc destRasterDesc;
	destRasterDesc.fPixvalDesc.fDepth = destDepth;
	destRasterDesc.fPixvalDesc.fBigEndian = true;
	destRasterDesc.fRowBytes = destRowBytes;
	destRasterDesc.fRowCount = theCroppedWidthHeight;
	destRasterDesc.fFlipped = false;

	Ptr destAddress = theDataHandle[0];
	if (hasMask && isMask)
		destAddress += destRowBytes * theCroppedWidthHeight;

	inPixmap.CopyTo(ZPoint(0, 0),
					destAddress,
					destRasterDesc,
					destPixelDesc,
					ZRect(theCroppedWidthHeight, theCroppedWidthHeight));

	if (needsInvert)
		spInvert(destAddress, destRowBytes * theCroppedWidthHeight);

	::SetIconFamilyData(inIconFamilyHandle, theResType, theDataHandle);
	theDataHandleLocker.Orphan();
	::DisposeHandle(theDataHandle);
	}

ZDCPixmap ZUtil_Mac_HL::sPixmapFromIconFamily(IconFamilyHandle inIconFamilyHandle, IconSize inIconSize, IconKind inIconKind)
	{
	ZDCPixmap theDCPixmap;

// Figure out what resource we're looking for
	uint32 theResType = 0;
	for (int x = 0; x < countof(sSizeKindResMapping); ++x)
		{
		if (sSizeKindResMapping[x].fIconSize == inIconSize && sSizeKindResMapping[x].fIconKind == inIconKind)
			{
			theResType = sSizeKindResMapping[x].fResType;
			break;
			}
		}

	if (theResType == 0)
		return theDCPixmap;

	Handle theIconDataHandle = ::NewHandle(0);
	::GetIconFamilyData(inIconFamilyHandle, theResType, theIconDataHandle);
	if (::GetHandleSize(theIconDataHandle) != 0)
		{
		try
			{
			theDCPixmap = ZUtil_Mac_HL::sPixmapFromIconDataHandle(theIconDataHandle, inIconSize, inIconKind);
			}
		catch (...)
			{
			::DisposeHandle(theIconDataHandle);
			throw;
			}
		}

	::DisposeHandle(theIconDataHandle);
	return theDCPixmap;
	}

ZDCPixmap ZUtil_Mac_HL::sPixmapFromIconSuite(IconSuiteRef inIconSuite, IconSize inIconSize, IconKind inIconKind)
	{
	ZDCPixmap theDCPixmap;

// Figure out what resource we're looking for
	uint32 theResType = 0;
	for (int x = 0; x < countof(sSizeKindResMapping); ++x)
		{
		if (sSizeKindResMapping[x].fIconSize == inIconSize && sSizeKindResMapping[x].fIconKind == inIconKind)
			{
			theResType = sSizeKindResMapping[x].fResType;
			break;
			}
		}

	if (theResType == 0)
		return theDCPixmap;

	Handle theIconDataHandle = nullptr;
	if (noErr == ::GetIconFromSuite(&theIconDataHandle, inIconSuite, theResType))
		theDCPixmap = sPixmapFromIconDataHandle(theIconDataHandle, inIconSize, inIconKind);

	return theDCPixmap;
	}

ZDCPixmap ZUtil_Mac_HL::sPixmapFromIconDataHandle(Handle inIconDataHandle, IconSize inIconSize, IconKind inIconKind)
	{
// Got the data, now to unmunge it.
	ZCoord theWidthHeight = 0;
	for (int x = 0; x < countof(sSizePixelMapping); ++x)
		{
		if (sSizePixelMapping[x].fIconSize == inIconSize)
			{
			theWidthHeight = sSizePixelMapping[x].fPixels;
			break;
			}
		}

	int16 theDepth = 0;
	int16 theCTableID = 0;
	bool hasMask = false;
	bool isMask = false;
	for (int x = 0; x < countof(sKindDepthCTableMapping); ++x)
		{
		if (sKindDepthCTableMapping[x].fIconKind == inIconKind)
			{
			theDepth = sKindDepthCTableMapping[x].fDepth;
			theCTableID = sKindDepthCTableMapping[x].fCTableID;
			hasMask = sKindDepthCTableMapping[x].fHasMask;
			isMask = sKindDepthCTableMapping[x].fIsMask;
			break;
			}
		}

	int32 theRowBytes = ((theWidthHeight * theDepth + 31) / 32) * 4;
	bool needsInvert = false;
	ZDCPixmapNS::PixelDesc thePixelDesc;
	switch (theDepth)
		{
		case 1:
			{
			theRowBytes = (theWidthHeight + 7 ) / 8;
			needsInvert = !isMask;
			thePixelDesc = ZDCPixmapNS::PixelDesc(1, 0);
			break;
			}
		case 2:
		case 4:
		case 8:
			{
			thePixelDesc = ZUtil_Mac_LL::sGetIndexedPixelDesc(theCTableID);
			break;
			}
		case 16:
			{
			thePixelDesc = ZDCPixmapNS::PixelDesc(0x7C00, 0x03E0, 0x001F, 0x0000);
			break;
			}
		case 24:
			{
			thePixelDesc = ZDCPixmapNS::PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);
			break;
			}
		default:
		case 32:
			{
			thePixelDesc = ZDCPixmapNS::PixelDesc(0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000);
			break;
			}
		}

	ZDCPixmapNS::RasterDesc theRasterDesc;
	theRasterDesc.fPixvalDesc.fDepth = theDepth;
	theRasterDesc.fPixvalDesc.fBigEndian = true;
	theRasterDesc.fRowBytes = theRowBytes;
	theRasterDesc.fRowCount = theWidthHeight;
	theRasterDesc.fFlipped = false;

	ZDCPixmap theDCPixmap(new ZDCPixmapRep(new ZDCPixmapRaster_Simple(theRasterDesc), ZRect(theWidthHeight, theWidthHeight), thePixelDesc));

	ZUtil_Mac_LL::HandleLocker locker(inIconDataHandle);

	theDCPixmap.CopyFrom(ZPoint(0, 0),
						inIconDataHandle[0] + (hasMask && isMask ? (theRowBytes * theWidthHeight) : 0),
						theRasterDesc,
						thePixelDesc,
						ZRect(theWidthHeight, theWidthHeight));
	if (needsInvert)
		{
		// This step could be avoided if we had separate source and destination
		// PixelDescs, and could let the blit take care of the transform.
		spInvert(theDCPixmap.GetBaseAddress(), theRowBytes * theWidthHeight);
		}

	return theDCPixmap;
	}

IconRef ZUtil_Mac_HL::sIconRefFromPixmaps(const ZDCPixmap& inColorPixmap, const ZDCPixmap& inMonoPixmap, const ZDCPixmap& inMaskPixmap)
	{
#if ZCONFIG(Processor, PPC)
	// AG 99-01-11. AAArrrrgggghhhhhh. There I was thinking that the new Icon Services stuff was a sweet addition to our
	// MacOS capabilities. Wrong! With the current headers and import library there appears to be no way to fabricate an arbitrary
	// IconRef -- we can *register* a new icon, but it has to have a creator and type attached to it, which is not what we want.

	// AG 99-05-19. I've since talked with a couple of people, and the recommendation from Apple is to use a fake creator/type combo,
	// and manage the lifetime of the registration explicitly.

	ZAssertStop(1, (inColorPixmap || inMonoPixmap) && inMaskPixmap);
	IconFamilyHandle theIconFamilyHandle = sNewIconFamily();

	if (inColorPixmap)
		sAddPixmapToIconFamily(inColorPixmap, eData8, theIconFamilyHandle);

//##	if (inColorPixmap)
//##		sAddPixmapToIconFamily(inColorPixmap, eData32, theIconFamilyHandle);

	if (inMonoPixmap)
		sAddPixmapToIconFamily(inMonoPixmap, eData1, theIconFamilyHandle);

	if (inMaskPixmap)
		sAddPixmapToIconFamily(inMaskPixmap, eMask1, theIconFamilyHandle);

	IconRef theIconRef = nullptr;

	static uint32 fakeType = rand();
	if (noErr == ::RegisterIconRefFromIconFamily('ZLIB', ++fakeType, theIconFamilyHandle, &theIconRef))
		{
		// Acquire another reference to the iconRef
		::AcquireIconRef(theIconRef);
		// And unregister the iconRef -- it should mean that when the IconRef's refcount hits
		// zero it will simply disappear from the system. I have checked for leaks and none seem to be occurring.
		::UnregisterIconRef('ZLIB', fakeType);

		UInt16 ownerCount;
        ::GetIconRefOwners(theIconRef, &ownerCount);
        if (ownerCount == 2)
			{
			// According to Laurence Harris some versions of the OS do not decrement
			// the refcount when UnregisterIconRef is called, so we do it manually.
            ::ReleaseIconRef(theIconRef);
			}
		}
	sDisposeIconFamily(theIconFamilyHandle);

	return theIconRef;
#else // ZCONFIG(Processor, PPC)
	return nullptr;
#endif // ZCONFIG(Processor, PPC)
	}

// =================================================================================================

static pascal Boolean sAppleEventIdleProc(EventRecord* inEventRecord, long* outSleepTime, RgnHandle* outMouseRgn)
	{
	#if ZCONFIG(Thread_API, Mac)
		ZThreadTM_Yield();
	#endif

/*	if (ZApp::sGet())
		{
		ZDCRgn theDCRgn;
//##		ZApp::sGet()->AppleEventIdleProc(theEventRecord, sleepTime, theDCRgn);
		::MacCopyRgn(theDCRgn, *outMouseRgn);
		}*/
	return false;
	}

static AEIdleUPP sAEIdleUPP = NewAEIdleUPP(sAppleEventIdleProc);

bool ZUtil_Mac_HL::sInteractWithUser()
	{
	OSErr result = ::AEInteractWithUser(kNoTimeOut, nullptr, sAEIdleUPP);
	if (result == noErr)
		return true;
	return false;
	}

void ZUtil_Mac_HL::sPreDialog()
	{
	ZUtil_Mac_HL::sInteractWithUser();

	#if ZCONFIG_SPI_Enabled(MacClassic)
		ZOSApp_Mac::sGet()->PreDialog();
		ZOSApp_Mac::sGet()->UpdateAllWindows();
	#endif
	}

void ZUtil_Mac_HL::sPostDialog()
	{
	#if ZCONFIG_SPI_Enabled(MacClassic)
		ZOSApp_Mac::sGet()->PostDialog();
	#endif
	}

static ModalFilterUPP sModalFilterUPP = NewModalFilterUPP(ZUtil_Mac_HL::sModalFilter);
pascal Boolean ZUtil_Mac_HL::sModalFilter(DialogPtr theDialog, EventRecord* inOutEventRecord, short* inOutItemHit)
	{
//##	if (ZApp::sGet())
//##		return ZApp::sGet()->ModalFilter(theDialog, theEventRecord, itemHit);
	return false;
	}

string ZUtil_Mac_HL::sGetVersionString()
	{
// Returns the version number as described in the .shortVersion field of the version resource
	string version;

	if (Handle theHandle = ::Get1Resource('vers', 1))
		{
		ZUtil_Mac_LL::HandleLocker theLocker(theHandle);
		StringPtr versStr = ((VersRecHndl)theHandle)[0]->shortVersion;
		version = string((char*)&versStr[1], (size_t)versStr[0]);
		}

	return version;
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================

#if ZCONFIG_SPI_Enabled(QuickDraw)

#include "zoolib/ZDC_QD.h"
#include "zoolib/ZStream.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacMemory.h)
#include ZMACINCLUDE3(ApplicationServices,QD,QDOffscreen.h)

namespace ZooLib {

namespace { // anonymous
struct PixmapFromStreamPICTInfo
	{
	PixmapFromStreamPICTInfo* fPrev;
	PixmapFromStreamPICTInfo* fNext;
	GWorldPtr fGWorldPtr;
	const ZStreamR* fStream;
	bool fStreamOkay;
	};
} // anonymous namespace

static PixmapFromStreamPICTInfo* sPixmapFromStreamPICTInfo_Head = nullptr;
static ZMutex sPixmapFromStreamPICT_Mutex;

static DEFINE_API(void) sPixmapFromStreamPICT_GetPicProc(void* dataPtr, short byteCount)
	{
	PixmapFromStreamPICTInfo* current = nullptr;
	try
		{
		CGrafPtr currentPort;
		GDHandle currentGDHandle;
		::GetGWorld(&currentPort, &currentGDHandle);
		sPixmapFromStreamPICT_Mutex.Acquire();
		current = sPixmapFromStreamPICTInfo_Head;
		while (current && current->fGWorldPtr != currentPort)
			current = current->fNext;
		sPixmapFromStreamPICT_Mutex.Release();

		ZAssertStop(0, current != nullptr);

		current->fStream->Read(dataPtr, byteCount);
		}
	catch (...)
		{
#if ACCESSOR_CALLS_ARE_FUNCTIONS
		SetQDError(noMemForPictPlaybackErr);
#else
		*((QDErr*)(0x0D6E)) = noMemForPictPlaybackErr; //## might be a problem under Windows. -ec 01.12.21
									// AG 2002-07-08. Except that ACCESSOR_CALLS_ARE_FUNCTIONS is true when building for windows.
#endif
		if (current)
			current->fStreamOkay = false;
		}
	}

static QDGetPicUPP sPixmapFromStreamPICT_GetPicProcUPP = NewQDGetPicUPP(sPixmapFromStreamPICT_GetPicProc);

ZDCPixmap ZUtil_Mac_HL::sPixmapFromStreamPICT(const ZStreamR& inStream)
	{
	ZDCPixmap thePixmap;
	PicHandle thePicHandle = nullptr;
	try
		{
		// Create the dummy PicHandle
		thePicHandle = (PicHandle)::NewHandleClear(sizeof(Picture));
		::HLock((Handle)thePicHandle);

		// Read the header
		inStream.Read(*thePicHandle, sizeof(Picture));
		::HUnlock((Handle)thePicHandle);

		// Get the picture's bounds
		ZRect theBounds = thePicHandle[0]->picFrame;
		theBounds -= theBounds.TopLeft();

		// Instantiate an offscreen port of the correct size
		ZDC_Off theDC(ZDCScratch::sGet(), theBounds.Size(), ZDCPixmapNS::eFormatEfficient_Color_32);
		theDC.SetInk(ZRGBColor::sWhite);
		theDC.Fill(theBounds);

		// Declare up the info structure
		PixmapFromStreamPICTInfo theInfo;
		theInfo.fStreamOkay = true;
		theInfo.fStream = &inStream;

		// Get the DC ready for use
		ZDCSetupForQD theSetup(theDC, false);

		// Shove the get pic proc into place
		GDHandle myGD;
		::GetGWorld(&theInfo.fGWorldPtr, &myGD);
		CQDProcs myBottleNecks;
		::SetStdCProcs(&myBottleNecks);
		myBottleNecks.getPicProc = sPixmapFromStreamPICT_GetPicProcUPP;

#if ACCESSOR_CALLS_ARE_FUNCTIONS
		CQDProcs* oldBottleneckProcs = GetPortGrafProcs(theInfo.fGWorldPtr);
		SetPortGrafProcs(theInfo.fGWorldPtr, &myBottleNecks);
#else
		CQDProcs* oldBottleneckProcs = theInfo.fGWorldPtr->grafProcs;
		theInfo.fGWorldPtr->grafProcs = &myBottleNecks;
#endif

		// Link theInfo onto the global list
		sPixmapFromStreamPICT_Mutex.Acquire();
		theInfo.fPrev = nullptr;
		theInfo.fNext = sPixmapFromStreamPICTInfo_Head;
		if (sPixmapFromStreamPICTInfo_Head)
			sPixmapFromStreamPICTInfo_Head->fPrev = &theInfo;
		sPixmapFromStreamPICTInfo_Head = &theInfo;
		sPixmapFromStreamPICT_Mutex.Release();

		// Draw the picture
		Rect qdFrame = theBounds + theSetup.GetOffset();
		::DrawPicture(thePicHandle, &qdFrame);

		// Unlink theInfo
		sPixmapFromStreamPICT_Mutex.Acquire();
		if (theInfo.fPrev)
			theInfo.fPrev->fNext = theInfo.fNext;
		if (theInfo.fNext)
			theInfo.fNext->fPrev = theInfo.fPrev;
		if (sPixmapFromStreamPICTInfo_Head == &theInfo)
			sPixmapFromStreamPICTInfo_Head = theInfo.fNext;
		sPixmapFromStreamPICT_Mutex.Release();

		// Restore the graf procs
#if ACCESSOR_CALLS_ARE_FUNCTIONS
		SetPortGrafProcs(theInfo.fGWorldPtr, oldBottleneckProcs);
#else
		theInfo.fGWorldPtr->grafProcs = oldBottleneckProcs;
#endif

		::DisposeHandle((Handle)thePicHandle);

		thePicHandle = nullptr;

		if (!theInfo.fStreamOkay)
			throw runtime_error("ZGraphicsUtil::sPixmapFromStreamPICT, there was a problem of some kind");

		thePixmap = theDC.GetPixmap(theBounds);
		}
	catch (...)
		{
		if (thePicHandle)
			::DisposeHandle((Handle)thePicHandle);
		throw;
		}

	return thePixmap;
	}

} // namespace ZooLib
#endif // ZCONFIG_SPI_Enabled(QuickDraw)
