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

#include "zoolib/ZDCPixmapCoder.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ZDCPixmapEncoder

void ZDCPixmapEncoder::sWritePixmap(const ZStreamW& iStream,
	const ZDCPixmap& iPixmap, ZDCPixmapEncoder& iEncoder)
	{
	iEncoder.Write(iStream, iPixmap);
	}

ZDCPixmapEncoder::ZDCPixmapEncoder()
	{}

ZDCPixmapEncoder::ZDCPixmapEncoder(const ZDCPixmapEncoder&)
	{}

ZDCPixmapEncoder& ZDCPixmapEncoder::operator=(const ZDCPixmapEncoder&)
	{
	return *this;
	}

ZDCPixmapEncoder::~ZDCPixmapEncoder()
	{
	}

void ZDCPixmapEncoder::Write(const ZStreamW& iStream, const ZDCPixmap& iPixmap)
	{
	ZRef<ZDCPixmapRep> theRep = iPixmap.GetRep();

	this->Imp_Write(iStream,
		theRep->GetRaster()->GetBaseAddress(),
		theRep->GetRaster()->GetRasterDesc(),
		theRep->GetPixelDesc(),
		theRep->GetBounds());
	}

void ZDCPixmapEncoder::Write(const ZStreamW& iStream,
	const void* iBaseAddress,
	const ZDCPixmapNS::RasterDesc& iRasterDesc,
	const ZDCPixmapNS::PixelDesc& iPixelDesc,
	const ZRectPOD& iBounds)
	{
	this->Imp_Write(iStream, iBaseAddress, iRasterDesc, iPixelDesc, iBounds);
	}

// =================================================================================================
// MARK: - ZDCPixmapDecoder

ZDCPixmap ZDCPixmapDecoder::sReadPixmap(const ZStreamR& iStream, ZDCPixmapDecoder& iDecoder)
	{ return iDecoder.Read(iStream); }

ZDCPixmapDecoder::ZDCPixmapDecoder()
	{}

ZDCPixmapDecoder::ZDCPixmapDecoder(const ZDCPixmapDecoder&)
	{}

ZDCPixmapDecoder& ZDCPixmapDecoder::operator=(const ZDCPixmapDecoder&)
	{
	return *this;
	}

ZDCPixmapDecoder::~ZDCPixmapDecoder()
	{
	}

ZDCPixmap ZDCPixmapDecoder::Read(const ZStreamR& iStream)
	{
	ZDCPixmap thePixmap;
	this->Imp_Read(iStream, thePixmap);
	return thePixmap;
	}

} // namespace ZooLib
