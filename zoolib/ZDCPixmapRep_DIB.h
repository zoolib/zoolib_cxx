/* -------------------------------------------------------------------------------------------------
Copyright (c) 2013 Andrew Green
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

#ifndef __ZDCPixmapRep_DIB_h__
#define __ZDCPixmapRep_DIB_h__ 1
#include "zconfig.h"

#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(GDI)

#include "zoolib/ZCompat_Win.h"

#include "zoolib/ZDCPixmap.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCPixmapRep_DIB declaration

class ZDCPixmapRep_DIB : public ZDCPixmapRep
	{
public:
	ZDCPixmapRep_DIB(ZRef<ZDCPixmapRaster> iBuffer,
		const ZRectPOD& iBounds,
		const ZDCPixmapNS::PixelDesc& iPixelDesc);

	ZDCPixmapRep_DIB(HDC iHDC, HBITMAP iHBITMAP, bool iForce32BPP = false);

	virtual ~ZDCPixmapRep_DIB();

	void GetDIBStuff(BITMAPINFO*& oBITMAPINFO, char*& oBits, ZRectPOD* oBounds);

protected:
	BITMAPINFO* fBITMAPINFO;
	int fChangeCount;
	};

ZRef<ZDCPixmapRep_DIB> sPixmapRep_DIB(ZRef<ZDCPixmapRep> iRep);

} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(GDI)

#endif // __ZDCPixmapRep_DIB_h__
