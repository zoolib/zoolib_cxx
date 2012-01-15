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

#ifndef __ZUtil_Mac_LL_h__
#define __ZUtil_Mac_LL_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include "zoolib/ZCountedWithoutFinalize.h"
#include "zoolib/ZDCPixmapNS.h" 
#include "zoolib/ZGeom.h"

#include ZMACINCLUDE3(CoreServices,CarbonCore,MacMemory.h)
#include ZMACINCLUDE3(ApplicationServices,QD,QDOffscreen.h)

#endif // ZCONFIG_SPI_Enabled(Carbon)


#if ZCONFIG_SPI_Enabled(Carbon)

#include "zoolib/ZTypes.h"

#endif // ZCONFIG_SPI_Enabled(Carbon)

namespace ZooLib {

class ZDCPixmap;
class ZDCPattern;

namespace ZUtil_Mac_LL {

// =================================================================================================

#if ZCONFIG_SPI_Enabled(Carbon)

void sSetupPixMapColor(ZPoint inSize, int32 inDepth, PixMap& oPixMap);
void sSetupPixMapColor(size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap);

void sSetupPixMapGray(ZPoint inSize, int32 inDepth, PixMap& oPixMap);
void sSetupPixMapGray(size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap);

void sSetupPixMapIndexed(ZPoint inSize, int32 inDepth, PixMap& oPixMap);
void sSetupPixMapIndexed(size_t inRowBytes, int32 inDepth, int32 inHeight, PixMap& oPixMap);

void sPixmapFromPixPatHandle(PixPatHandle inHandle, ZDCPixmap* outColorPixmap, ZDCPattern* outPattern);

PixPatHandle sPixPatHandleFromPixmap(const ZDCPixmap& inPixmap);

ZDCPixmapNS::PixelDesc sCTabHandleToPixelDesc(CTabHandle inCTabHandle);

ZDCPixmapNS::PixelDesc sGetIndexedPixelDesc(int16 iCTableID);

void sSetWindowManagerPort();

// =================================================================================================
// MARK: - ZUtil_Mac_LL::RefCountedCTabHandle

class RefCountedCTabHandle : public ZCountedWithoutFinalize
	{
public:
	RefCountedCTabHandle(CTabHandle inCTabHandle) : fCTabHandle(inCTabHandle) {}
	virtual ~RefCountedCTabHandle();

	CTabHandle GetCTabHandle() { return fCTabHandle; }

private:
	CTabHandle fCTabHandle;
	};

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveRestorePort

class SaveRestorePort
	{
public:
	SaveRestorePort();
	~SaveRestorePort();

protected:
	CGrafPtr fPreConstruct_GrafPtr;
	GDHandle fPreConstruct_GDHandle;
	};

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveSetBlackWhite

class SaveSetBlackWhite
	{
public:
	SaveSetBlackWhite();
	~SaveSetBlackWhite();

protected:
	RGBColor fPreConstruct_ForeColor;
	RGBColor fPreConstruct_BackColor;
	};

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================
// MARK: - ZUtil_Mac_LL::SaveRestoreResFile

#if ZCONFIG_SPI_Enabled(Carbon)

class SaveRestoreResFile
	{
public:
	SaveRestoreResFile();
	SaveRestoreResFile(short iResFile);
	~SaveRestoreResFile();

protected:
	short fPreConstruct_ResFile;
	};

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================
// MARK: - ZUtil_Mac_LL::HandleLocker

#if ZCONFIG_SPI_Enabled(Carbon)

class HandleLocker
	{
public:
	HandleLocker(Handle inHandle) : fHandle(inHandle) { this->Internal_Adopt(); }
	~HandleLocker() { this->Internal_Orphan(); }

	void Adopt(Handle inHandle) { this->Internal_Orphan(); fHandle = inHandle; this->Internal_Adopt(); }
	void Orphan() { this->Internal_Orphan(); }

protected:
	void Internal_Adopt()
		{
		if (fHandle)
			{
			fOldState = ::HGetState(fHandle);
			::HLock(fHandle);
			}
		}

	void Internal_Orphan()
		{
		if (fHandle)
			::HSetState(fHandle, fOldState);
		fHandle = nullptr;
		}

	Handle fHandle;
	SignedByte fOldState;
	};

#endif // ZCONFIG_SPI_Enabled(Carbon)

// =================================================================================================

} // namespace ZUtil_Mac_LL

} // namespace ZooLib

#endif // __ZUtil_Mac_LL_h__
