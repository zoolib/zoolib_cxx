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

#include "zoolib/ZUtil_Mac_LL.h"
#include "zoolib/ZDC_QD.h"
#include "zoolib/ZDebug.h"
#include "zoolib/ZMacOSX.h"
#include "zoolib/ZMemory.h"

#include <map>
#include <new> // for bad_alloc

#if ZCONFIG_SPI_Enabled(Carbon)
	#include ZMACINCLUDE3(CoreServices,CarbonCore,Resources.h)
#endif

using std::bad_alloc;
using std::map;
using std::pair;
using std::vector;

namespace ZooLib {

#define kDebug_Mac 3
// =================================================================================================

#if ZCONFIG_SPI_Enabled(Carbon)

void ZUtil_Mac_LL::sSetupPixMapColor(ZPoint inSize, int32 inDepth, PixMap& oPixMap)
	{
	ZAssert(inDepth == 16 || inDepth == 24 || inDepth == 32);

	#if ZCONFIG_SPI_Enabled(Carbon)
		ZAssert(inDepth != 24);
	#endif

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = (((inSize.h * inDepth + 31) / 32) * 4) | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = inSize.h;
	oPixMap.bounds.bottom = inSize.v;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = RGBDirect;
	oPixMap.cmpCount = 3;
	oPixMap.pmTable = reinterpret_cast<CTabHandle>(::NewHandle(sizeof(ColorTable)));
	if (oPixMap.pmTable == nullptr)
		throw bad_alloc();
	oPixMap.pmTable[0]->ctFlags = 0;
	oPixMap.pmTable[0]->ctSize = 0;

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	switch (inDepth)
		{
		case 16:
			{
			oPixMap.cmpSize = 5;
			oPixMap.pmTable[0]->ctSeed = 15;
			break;
			}
		case 24:
			{
			oPixMap.cmpSize = 8;
			oPixMap.pmTable[0]->ctSeed = 24;
			break;
			}
		case 32:
			{
			oPixMap.cmpSize = 8;
			oPixMap.pmTable[0]->ctSeed = 32;
			break;
			}
		}
	}

void ZUtil_Mac_LL::sSetupPixMapColor(
	size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap)
	{
	ZAssert(inDepth == 16 || inDepth == 24 || inDepth == 32);

	#if ZCONFIG_SPI_Enabled(Carbon)
		ZAssert(inDepth != 24);
	#endif

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = inRowBytes | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = (inRowBytes * 8) / inDepth;
	oPixMap.bounds.bottom = inHeight;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = RGBDirect;
	oPixMap.cmpCount = 3;
	oPixMap.pmTable = reinterpret_cast<CTabHandle>(::NewHandle(sizeof(ColorTable)));
	if (oPixMap.pmTable == nullptr)
		throw bad_alloc();
	oPixMap.pmTable[0]->ctFlags = 0;
	oPixMap.pmTable[0]->ctSize = 0;

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	switch (inDepth)
		{
		case 16:
			{
			oPixMap.cmpSize = 5;
			oPixMap.pmTable[0]->ctSeed = 15;
			break;
			}
		case 24:
			{
			oPixMap.cmpSize = 8;
			oPixMap.pmTable[0]->ctSeed = 24;
			break;
			}
		case 32:
			{
			oPixMap.cmpSize = 8;
			oPixMap.pmTable[0]->ctSeed = 32;
			break;
			}
		}
	}

// =================================================================================================

static CTabHandle sMakeCTabHandle_ByCount(int iCount)
	{
	const size_t handleSize = sizeof(ColorTable) + (iCount - 1) * sizeof(CSpecArray);
	CTabHandle result = reinterpret_cast<CTabHandle>(::NewHandleClear(handleSize));
	if (!result)
		throw bad_alloc();

	result[0]->ctSeed = ::GetCTSeed();
	result[0]->ctFlags = 0;
	result[0]->ctSize = iCount - 1;
	return result;
	}

static CTabHandle sMakeCTabHandle_ByDepth(int32 iDepth)
	{
	return sMakeCTabHandle_ByCount(1 << iDepth);
	}

static CTabHandle sMakeCTabHandle_Gray(int32 iDepth)
	{
	const int count = 1 << iDepth;
	CTabHandle result = sMakeCTabHandle_ByCount(count);

	const int countMinus1 = count - 1;
	for (int x = 0; x < count; ++x)
		{
		const uint16 comp = (x * 0xFFFF) / countMinus1;
		ColorSpec& theCS = result[0]->ctTable[x];
		theCS.value = x;
		theCS.rgb.red = comp;
		theCS.rgb.green = comp;
		theCS.rgb.blue = comp;
		}
	result[0]->ctSeed = ::GetCTSeed();
	result[0]->ctFlags = 0;
	result[0]->ctSize = countMinus1;
	return result;
	}

void ZUtil_Mac_LL::sSetupPixMapGray(ZPoint inSize, int32 inDepth, PixMap& oPixMap)
	{
	ZAssert(inDepth == 1 || inDepth == 2 || inDepth == 4 || inDepth == 8);

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = (((inSize.h * inDepth + 31) / 32) * 4) | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = inSize.h;
	oPixMap.bounds.bottom = inSize.v;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = 0;
	oPixMap.cmpCount = 1;
	oPixMap.cmpSize = inDepth;
	oPixMap.pmTable = sMakeCTabHandle_Gray(inDepth);

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	}

void ZUtil_Mac_LL::sSetupPixMapGray(
	size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap)
	{
	ZAssert(inDepth == 1 || inDepth == 2 || inDepth == 4 || inDepth == 8);

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = inRowBytes | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = (inRowBytes * 8) / inDepth;
	oPixMap.bounds.bottom = inHeight;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = 0;
	oPixMap.cmpCount = 1;
	oPixMap.cmpSize = inDepth;
	oPixMap.pmTable = sMakeCTabHandle_Gray(inDepth);

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	}

// =================================================================================================

void ZUtil_Mac_LL::sSetupPixMapIndexed(ZPoint inSize, int32 inDepth, PixMap& oPixMap)
	{
	ZAssert(inDepth == 1 || inDepth == 2 || inDepth == 4 || inDepth == 8);

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = (((inSize.h * inDepth + 31) / 32) * 4) | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = inSize.h;
	oPixMap.bounds.bottom = inSize.v;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = 0;
	oPixMap.cmpCount = 1;
	oPixMap.cmpSize = inDepth;
	oPixMap.pmTable = sMakeCTabHandle_ByDepth(inDepth);

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	}

void ZUtil_Mac_LL::sSetupPixMapIndexed(
	size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap)
	{
	ZAssert(inDepth == 1 || inDepth == 2 || inDepth == 4 || inDepth == 8);

	oPixMap.baseAddr = nullptr;
	oPixMap.pmTable = nullptr;

	oPixMap.rowBytes = inRowBytes | 0x8000;

	oPixMap.bounds.left = 0;
	oPixMap.bounds.top = 0;
	oPixMap.bounds.right = (inRowBytes * 8) / inDepth;
	oPixMap.bounds.bottom = inHeight;
	oPixMap.pmVersion = baseAddr32;
	oPixMap.packType = 0;
	oPixMap.packSize = 0;
	oPixMap.hRes = 72<<16; // Fixed point
	oPixMap.vRes = 72<<16; // Fixed point
	oPixMap.pixelSize = inDepth;
	oPixMap.pixelType = 0;
	oPixMap.cmpCount = 1;
	oPixMap.cmpSize = inDepth;
	oPixMap.pmTable = sMakeCTabHandle_ByDepth(inDepth);

#if OLDPIXMAPSTRUCT
	oPixMap.planeBytes = 0;
	oPixMap.pmReserved = 0;
#else // OLDPIXMAPSTRUCT
	oPixMap.pixelFormat = inDepth;
	oPixMap.pmExt = nullptr;
#endif // OLDPIXMAPSTRUCT
	}

// =================================================================================================

void ZUtil_Mac_LL::sPixmapFromPixPatHandle(PixPatHandle inHandle,
	ZDCPixmap* outColorPixmap, ZDCPattern* outPattern)
	{
	ZUtil_Mac_LL::HandleLocker locker1((Handle)inHandle);

	if (outColorPixmap)
		{
		ZUtil_Mac_LL::HandleLocker locker2((Handle)inHandle[0]->patMap);
		ZUtil_Mac_LL::HandleLocker locker3((Handle)inHandle[0]->patData);
		*outColorPixmap = ZDCPixmap(new ZDCPixmapRep_QD(
			inHandle[0]->patMap[0],
			*inHandle[0]->patData,
			inHandle[0]->patMap[0]->bounds));
		}

	if (outPattern)
		*outPattern = *reinterpret_cast<ZDCPattern*>(&inHandle[0]->pat1Data);
	}

// =================================================================================================

static short sClosestPowerOfTwo(short inValue)
	{
	ZAssertStop(kDebug_Mac, inValue > 0 && inValue <= 0x4000);

	short thePower = 1;
	while (thePower <= inValue)
		thePower <<= 1;
	return thePower >> 1;
	}

PixPatHandle ZUtil_Mac_LL::sPixPatHandleFromPixmap(const ZDCPixmap& inPixmap)
	{
	ZRef<ZDCPixmapRep_QD> thePixmapRep_QD = ZDCPixmapRep_QD::sAsPixmapRep_QD(inPixmap.GetRep());
	const PixMap& sourcePixMap = thePixmapRep_QD->GetPixMap();

	PixPatHandle thePixPatHandle = ::NewPixPat();

	thePixPatHandle[0]->patMap[0]->rowBytes = sourcePixMap.rowBytes;
	thePixPatHandle[0]->patMap[0]->bounds = sourcePixMap.bounds;
	thePixPatHandle[0]->patMap[0]->pmVersion = sourcePixMap.pmVersion;
	thePixPatHandle[0]->patMap[0]->packType = sourcePixMap.packType;
	thePixPatHandle[0]->patMap[0]->packSize = sourcePixMap.packSize;
	thePixPatHandle[0]->patMap[0]->hRes = sourcePixMap.hRes;
	thePixPatHandle[0]->patMap[0]->vRes = sourcePixMap.vRes;
	thePixPatHandle[0]->patMap[0]->pixelType = sourcePixMap.pixelType;
	thePixPatHandle[0]->patMap[0]->pixelSize = sourcePixMap.pixelSize;
	thePixPatHandle[0]->patMap[0]->cmpCount = sourcePixMap.cmpCount;
	thePixPatHandle[0]->patMap[0]->cmpSize = sourcePixMap.cmpSize;
#if OLDPIXMAPSTRUCT
	thePixPatHandle[0]->patMap[0]->planeBytes = sourcePixMap.planeBytes;
	thePixPatHandle[0]->patMap[0]->pmReserved = sourcePixMap.pmReserved;
#else
	thePixPatHandle[0]->patMap[0]->pixelFormat = sourcePixMap.pixelFormat;
	thePixPatHandle[0]->patMap[0]->pmExt = sourcePixMap.pmExt;
#endif

	// Copy the color table
	size_t cTabSize = ::GetHandleSize((Handle)sourcePixMap.pmTable);
	::SetHandleSize((Handle)thePixPatHandle[0]->patMap[0]->pmTable, cTabSize);
	ZMemCopy(thePixPatHandle[0]->patMap[0]->pmTable[0], sourcePixMap.pmTable[0], cTabSize);

	// And all the pixel data
	size_t pixelDataSize = (sourcePixMap.rowBytes & 0x3FFF) * sourcePixMap.bounds.bottom;
	::SetHandleSize((Handle)thePixPatHandle[0]->patData, pixelDataSize);
	ZMemCopy(thePixPatHandle[0]->patData[0], sourcePixMap.baseAddr, pixelDataSize);

	::PixPatChanged(thePixPatHandle);

	return thePixPatHandle;
	}

// =================================================================================================

ZDCPixmapNS::PixelDesc ZUtil_Mac_LL::sCTabHandleToPixelDesc(CTabHandle inCTabHandle)
	{
	ZUtil_Mac_LL::HandleLocker locker1((Handle)inCTabHandle);
	ColorSpec* curColorSpec = &inCTabHandle[0]->ctTable[0];
	vector<ZRGBColorPOD> theColors(inCTabHandle[0]->ctSize + 1);
	for (vector<ZRGBColorPOD>::iterator i = theColors.begin();
		i != theColors.end(); ++i, ++curColorSpec)
		{
		(*i).red = curColorSpec->rgb.red;
		(*i).green = curColorSpec->rgb.green;
		(*i).blue = curColorSpec->rgb.blue;
		(*i).alpha = 0xFFFFU;
		}
	return ZDCPixmapNS::PixelDesc(&theColors[0], theColors.size());
	}

// =================================================================================================

void ZUtil_Mac_LL::sSetWindowManagerPort()
	{
	#if ZCONFIG_SPI_Enabled(MacClassic)

		CGrafPtr windowManagerPort;
		::GetCWMgrPort(&windowManagerPort);
		::SetGWorld(windowManagerPort, nullptr);

	#elif ZCONFIG_SPI_Enabled(Carbon)

		if (ZMacOSX::sIsMacOSX())
			{
			::SetPort(nullptr);
			}
		else
			{
			typedef pascal void (*GetCWMgrPortProcPtr) (CGrafPtr * wMgrCPort);

			static GetCWMgrPortProcPtr sGetCWMgrPortProcPtr = nullptr;
			static bool sChecked = false;

			if (!sChecked)
				{
				CFragConnectionID connID = kInvalidID;
				if (noErr == ::GetSharedLibrary("\pInterfaceLib",
					kCompiledCFragArch, kReferenceCFrag, &connID, nullptr, nullptr))
					{
					::FindSymbol(connID, "\pGetCWMgrPort",
						reinterpret_cast<Ptr*>(&sGetCWMgrPortProcPtr), nullptr);
					}
				sChecked = true;
				}

			if (sGetCWMgrPortProcPtr)
				{
				CGrafPtr windowManagerPort;
				sGetCWMgrPortProcPtr(&windowManagerPort);
				::SetGWorld(windowManagerPort, nullptr);
				}
			}

	#endif
	}

static ZMutex sMutex_CTable;
static std::map<int16, ZDCPixmapNS::PixelDesc> sMap_CTable;
ZDCPixmapNS::PixelDesc ZUtil_Mac_LL::sGetIndexedPixelDesc(int16 iCTableID)
	{
	ZMutexLocker locker(sMutex_CTable);

	std::map<int16, ZDCPixmapNS::PixelDesc>::iterator iter = sMap_CTable.find(iCTableID);
	if (iter != sMap_CTable.end())
		return iter->second;

	CTabHandle theCTabHandle = ::GetCTable(iCTableID);
	ZDCPixmapNS::PixelDesc thePD = ZUtil_Mac_LL::sCTabHandleToPixelDesc(theCTabHandle);
	::DisposeHandle((Handle)theCTabHandle);

	sMap_CTable.insert(pair<int16, ZDCPixmapNS::PixelDesc>(iCTableID, thePD));

	return thePD;
	}


// =================================================================================================
// MARK: - ZUtil_Mac_LL::RefCountedCTabHandle

ZUtil_Mac_LL::RefCountedCTabHandle::~RefCountedCTabHandle()
	{
	if (fCTabHandle)
		::DisposeHandle((Handle)fCTabHandle);
	}

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveRestorePort

ZUtil_Mac_LL::SaveRestorePort::SaveRestorePort()
	{
	::GetGWorld(&fPreConstruct_GrafPtr, &fPreConstruct_GDHandle);
	}

ZUtil_Mac_LL::SaveRestorePort::~SaveRestorePort()
	{
	::SetGWorld(fPreConstruct_GrafPtr, fPreConstruct_GDHandle);
	}

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveSetBlackWhite

ZUtil_Mac_LL::SaveSetBlackWhite::SaveSetBlackWhite()
	{
	::GetForeColor(&fPreConstruct_ForeColor);
	::GetBackColor(&fPreConstruct_BackColor);
	::ForeColor(blackColor);
	::BackColor(whiteColor);
	}

ZUtil_Mac_LL::SaveSetBlackWhite::~SaveSetBlackWhite()
	{
	::RGBForeColor(&fPreConstruct_ForeColor);
	::RGBBackColor(&fPreConstruct_BackColor);
	}

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveRestoreResFile

#if ZCONFIG_SPI_Enabled(Carbon)

ZUtil_Mac_LL::SaveRestoreResFile::SaveRestoreResFile()
	{
	fPreConstruct_ResFile = ::CurResFile();
	}

ZUtil_Mac_LL::SaveRestoreResFile::SaveRestoreResFile(short iResFile)
	{
	fPreConstruct_ResFile = ::CurResFile();
	if (iResFile != -1)
		::UseResFile(iResFile);
	}

ZUtil_Mac_LL::SaveRestoreResFile::~SaveRestoreResFile()
	{
	if (fPreConstruct_ResFile != -1)
		::UseResFile(fPreConstruct_ResFile);
	}

#endif // ZCONFIG_SPI_Enabled(Carbon)

} // namespace ZooLib
