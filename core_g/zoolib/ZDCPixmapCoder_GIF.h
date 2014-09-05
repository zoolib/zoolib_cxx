/* -------------------------------------------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZDCPixmapCoder_GIF_h__
#define __ZDCPixmapCoder_GIF_h__ 1
#include "zconfig.h"

#include "zoolib/ZDCPixmapCoder.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCPixmapEncoder_GIF

class ZDCPixmapEncoder_GIF : public ZDCPixmapEncoder
	{
public:
	static void sWritePixmap(const ZStreamW& iStream, const ZDCPixmap& iPixmap);

	ZDCPixmapEncoder_GIF(bool iInterlace);
	ZDCPixmapEncoder_GIF(bool iInterlace, bool iNoPatent);
	ZDCPixmapEncoder_GIF(bool iInterlace, bool iNoPatent, uint32 iTransparentPixval);
	virtual ~ZDCPixmapEncoder_GIF();

// From ZDCPixmapEncoder
	virtual void Imp_Write(const ZStreamW& iStream,
		const void* iBaseAddress,
		const ZDCPixmapNS::RasterDesc& iRasterDesc,
		const ZDCPixmapNS::PixelDesc& iPixelDesc,
		const ZRectPOD& iBounds);

private:
	bool fInterlace;
	bool fNoPatent;
	uint32 fTransparentPixval;
	bool fTransparent;
	};

// =================================================================================================
// MARK: - ZDCPixmapDecoder_GIF

class ZDCPixmapDecoder_GIF : public ZDCPixmapDecoder
	{
public:
	static ZDCPixmap sReadPixmap(const ZStreamR& iStream);

	ZDCPixmapDecoder_GIF();
	virtual ~ZDCPixmapDecoder_GIF();

// From ZDCPixmapDecoder
	virtual void Imp_Read(const ZStreamR& iStream, ZDCPixmap& oPixmap);

private:
	ZRef<ZDCPixmapRaster_Simple> fRaster;
	ZDCPixmapNS::PixelDesc fPixelDesc;
	ZPointPOD fSize;
	bool fReadHeader;
	bool fReadEnd;
	bool fIs89a;
	};

} // namespace ZooLib

#endif // __ZDCPixmapCoder_GIF_h__
