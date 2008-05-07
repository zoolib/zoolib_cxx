/* ------------------------------------------------------------
Copyright (c) 2002 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZDCPixmapCoder_JPEGLib__
#define __ZDCPixmapCoder_JPEGLib__ 1
#include "zconfig.h"
#include "ZCONFIG_SPI.h"

#include "ZDCPixmapCoder.h"

#if ZCONFIG_SPI_Enabled(JPEGLib)

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapEncoder_JPEGLib

class ZDCPixmapEncoder_JPEGLib : public ZDCPixmapEncoder
	{
public:
	static void sWritePixmap(const ZStreamW& iStream, const ZDCPixmap& iPixmap, int iQuality);

	ZDCPixmapEncoder_JPEGLib(int iQuality);
	virtual ~ZDCPixmapEncoder_JPEGLib();

// From ZDCPixmapEncoder
	virtual void Imp_Write(const ZStreamW& iStream,
		const void* iBaseAddress,
		const ZDCPixmapNS::RasterDesc& iRasterDesc,
		const ZDCPixmapNS::PixelDesc& iPixelDesc,
		const ZRect& iBounds);

private:
	int fQuality;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZDCPixmapDecoder_JPEGLib

class ZDCPixmapDecoder_JPEGLib : public ZDCPixmapDecoder
	{
public:
	static ZDCPixmap sReadPixmap(const ZStreamR& iStream);

	ZDCPixmapDecoder_JPEGLib();
	virtual ~ZDCPixmapDecoder_JPEGLib();

// From ZDCPixmapDecoder
	virtual void Imp_Read(const ZStreamR& iStream, ZDCPixmap& oPixmap);
	};

#endif // ZCONFIG_SPI_Enabled(JPEGLib)

#endif // __ZDCPixmapCoder_JPEGLib__
