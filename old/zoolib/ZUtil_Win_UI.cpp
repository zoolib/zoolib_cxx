/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green
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

#include "zoolib/ZUtil_Win_UI.h"

#if ZCONFIG_SPI_Enabled(Win)

#include "zoolib/ZUtil_Win.h"

#include "zoolib/ZDC_GDI.h"
#include "zoolib/ZDCPixmap.h"
#include "zoolib/ZDebug.h"

namespace ZooLib {

#define kDebug_Win 2

bool ZUtil_Win::sDragFullWindows()
	{
	bool dragFullWindows = false;
	HKEY keyDragFullWindows;
	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_CURRENT_USER,
		"Control Panel\\Desktop", 0, KEY_QUERY_VALUE, &keyDragFullWindows))
		{
		BYTE buffer[100];
		DWORD bufferSize = 100;
		if (ERROR_SUCCESS == ::RegQueryValueExA(keyDragFullWindows,
			"DragFullWindows", NULL, NULL, buffer, &bufferSize))
			{
			if (buffer[0] == '1')
				dragFullWindows = true;
			}
		::RegCloseKey(keyDragFullWindows);
		}
	return dragFullWindows;
	}

static bool spMungeProc_Invert(ZCoord hCoord, ZCoord vCoord, ZRGBColorPOD& ioColor, void* iRefcon)
	{
	ioColor = ZRGBColor::sWhite - ioColor;
	return true;
	}

void ZUtil_Win::sPixmapsFromHICON(HICON iHICON,
	ZDCPixmap* oColorPixmap, ZDCPixmap* oMonoPixmap, ZDCPixmap* oMaskPixmap)
	{
	ZAssertStop(2, iHICON);
	ICONINFO theICONINFO;
	::GetIconInfo(iHICON, &theICONINFO);

	HDC dummyHDC = ::GetDC(nullptr);

	ZDCPixmap monoPixmap(new ZDCPixmapRep_DIB(dummyHDC, theICONINFO.hbmMask));

	if (theICONINFO.hbmColor)
		{
		if (oColorPixmap)
			*oColorPixmap = ZDCPixmap(new ZDCPixmapRep_DIB(dummyHDC, theICONINFO.hbmColor));
		if (oMaskPixmap)
			*oMaskPixmap = monoPixmap;
		}
	else
		{
		// theICONINFO.hbmColor is nullptr, so theICONINFO.hbmMask (and thus monoPixmap) contains
		// the mono pixmap and the mask stacked on top of each other.
		ZPoint monoSize = monoPixmap.Size();
		if (oMonoPixmap)
			*oMonoPixmap = ZDCPixmap(monoPixmap, ZRect(0, 0, monoSize.h, monoSize.v / 2));
		if (oMaskPixmap)
			*oMaskPixmap = ZDCPixmap(monoPixmap, ZRect(0, monoSize.v / 2, monoSize.h, monoSize.v));
		}

	if (oMaskPixmap)
		oMaskPixmap->Munge(spMungeProc_Invert, nullptr);

	if (theICONINFO.hbmMask)
		::DeleteObject(theICONINFO.hbmMask);
	if (theICONINFO.hbmMask)
		::DeleteObject(theICONINFO.hbmColor);

	::ReleaseDC(nullptr, dummyHDC);
	}

static HINSTANCE spGetAppModuleHandle()
	{
	if (ZUtil_Win::sUseWAPI())
		return ::GetModuleHandleW(0);
	else
		return ::GetModuleHandleA(0);
	}

HBITMAP ZUtil_Win::sLoadBitmapID(bool iFromApp, int iResourceID)
	{
	ZAssertStop(kDebug_Win, (iResourceID & 0xFFFF0000) == 0);
	HINSTANCE theHINSTANCE = nullptr;
	if (iFromApp)
		theHINSTANCE = spGetAppModuleHandle();

	if (ZUtil_Win::sUseWAPI())
		return ::LoadBitmapW(theHINSTANCE, MAKEINTRESOURCEW(iResourceID));
	else
		return ::LoadBitmapA(theHINSTANCE, MAKEINTRESOURCEA(iResourceID));
	}

HICON ZUtil_Win::sLoadIconID(bool iFromApp, int iResourceID)
	{
	ZAssertStop(kDebug_Win, (iResourceID & 0xFFFF0000) == 0);

	HINSTANCE theHINSTANCE = nullptr;
	if (iFromApp)
		theHINSTANCE = spGetAppModuleHandle();

	if (ZUtil_Win::sUseWAPI())
		return ::LoadIconW(theHINSTANCE, MAKEINTRESOURCEW(iResourceID));
	else
		return ::LoadIconA(theHINSTANCE, MAKEINTRESOURCEA(iResourceID));
	}

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(Win)
