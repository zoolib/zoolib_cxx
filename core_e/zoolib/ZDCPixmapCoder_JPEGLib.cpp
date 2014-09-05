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

#include "zoolib/ZDCPixmapCoder_JPEGLib.h"

#if ZCONFIG_SPI_Enabled(Win) && defined(__ZCompat_Win_h__)
	// Fix up some stuff that breaks jpeglib's jmorecfg.h when we've also already pulled in
	// the Windows Headers. That happens when we're compiling for Windows using ZCompat_Win
	// and CodeWarrior's precompiled headers.

	// jmorecfg.h unconditionally #defines FAR, so remove any existing definition.
	#undef FAR

	// Prevent jmorecfg.h from typedef-ing INT32 (which basetsd.h has already typedefed.)
	#define XMD_H

	// Prevent jmorecfg.h from typedef-ing boolean (which rpcndr.h has already typedefed.)
	#define HAVE_BOOLEAN

	// But our code still needs to see boolean as being an int.
	#define boolean int
#endif

#include <setjmp.h>
#include <vector>

extern "C" {
#include <jpeglib.h>
} // extern "C"

using std::min;
using std::vector;

namespace ZooLib {

using namespace ZDCPixmapNS;

// =================================================================================================
// MARK: - JPEGErrorMgr

namespace { // anonymous

class JPEGErrorMgr : public jpeg_error_mgr
	{
public:
	JPEGErrorMgr(struct jpeg_compress_struct& iJCS);
	JPEGErrorMgr(struct jpeg_decompress_struct& iJDS);

	void BeforeCall();
	void Fail();

private:
	void pInit(struct jpeg_error_mgr*& oErrField);

	jmp_buf fEnv;

	static void spErrorExit(j_common_ptr cinfo);
	static void spEmitMessage(j_common_ptr cinfo, int msg_level);
	static void spOutputMessage(j_common_ptr cinfo);
	static void spFormatMessage(j_common_ptr cinfo, char * buffer);
	static void spReset(j_common_ptr cinfo);
	};

JPEGErrorMgr::JPEGErrorMgr(struct jpeg_compress_struct& iJCS)
	{ this->pInit(iJCS.err); }

JPEGErrorMgr::JPEGErrorMgr(struct jpeg_decompress_struct& iJDS)
	{ this->pInit(iJDS.err); }

void JPEGErrorMgr::BeforeCall()
	{
	if (setjmp(fEnv))
		throw std::runtime_error("JPEGErrorMgr");
	}

void JPEGErrorMgr::Fail()
	{
	longjmp(fEnv, 1);
	}

void JPEGErrorMgr::pInit(struct jpeg_error_mgr*& oErrField)
	{
	oErrField = this;

	error_exit = spErrorExit;
	emit_message = spEmitMessage;
	output_message = spOutputMessage;
	format_message = spFormatMessage;
	reset_error_mgr = spReset;
	}

void JPEGErrorMgr::spErrorExit(j_common_ptr cinfo)
	{
	static_cast<JPEGErrorMgr*>(cinfo->err)->Fail();
	}

void JPEGErrorMgr::spEmitMessage(j_common_ptr cinfo, int msg_level)
	{}

void JPEGErrorMgr::spOutputMessage(j_common_ptr cinfo)
	{}

void JPEGErrorMgr::spFormatMessage(j_common_ptr cinfo, char * buffer)
	{}

void JPEGErrorMgr::spReset(j_common_ptr cinfo)
	{}

} // anonymous namespace

// =================================================================================================
// MARK: - JPEGWriter

namespace { // anonymous

class JPEGWriter : public jpeg_destination_mgr
	{
public:
	JPEGWriter(const ZStreamW& iStream);
	~JPEGWriter();

private:
	static void spInititialize(j_compress_ptr iCInfo);
	static boolean spEmptyOutputBuffer(j_compress_ptr iCInfo);
	static void spTerminate(j_compress_ptr iCInfo);

	const ZStreamW& fStream;
	vector<JOCTET> fBuffer;
	};

JPEGWriter::JPEGWriter(const ZStreamW& iStream)
:	fStream(iStream),
	fBuffer(1024)
	{
	init_destination = spInititialize;
	empty_output_buffer = spEmptyOutputBuffer;
	term_destination = spTerminate;
	}

JPEGWriter::~JPEGWriter()
	{}

void JPEGWriter::spInititialize(j_compress_ptr iCInfo)
	{
	JPEGWriter* theWriter = static_cast<JPEGWriter*>(iCInfo->dest);
	theWriter->next_output_byte = &theWriter->fBuffer[0];
	theWriter->free_in_buffer = theWriter->fBuffer.size();
	}

boolean JPEGWriter::spEmptyOutputBuffer(j_compress_ptr iCInfo)
	{
	try
		{
		JPEGWriter* theWriter = static_cast<JPEGWriter*>(iCInfo->dest);
		theWriter->fStream.Write(&theWriter->fBuffer[0], theWriter->fBuffer.size());
		theWriter->next_output_byte = &theWriter->fBuffer[0];
		theWriter->free_in_buffer = theWriter->fBuffer.size();

		return TRUE;
		}
	catch (...)
		{
		static_cast<JPEGErrorMgr*>(iCInfo->err)->Fail();
		}
	// Can't get here.
	ZUnimplemented();
	return FALSE;
	}

void JPEGWriter::spTerminate(j_compress_ptr iCInfo)
	{
	try
		{
		JPEGWriter* theWriter = static_cast<JPEGWriter*>(iCInfo->dest);
		if (size_t countToWrite = theWriter->next_output_byte - &theWriter->fBuffer[0])
			theWriter->fStream.Write(&theWriter->fBuffer[0], countToWrite);
		}
	catch (...)
		{
		static_cast<JPEGErrorMgr*>(iCInfo->err)->Fail();
		}
	}

} // anonymous namespace

// =================================================================================================
// MARK: - JPEGReader

namespace { // anonymous

class JPEGReader : public jpeg_source_mgr
	{
public:
	JPEGReader(const ZStreamR& iStream);
	~JPEGReader();

private:
	static void spInititialize(j_decompress_ptr iCInfo);
	static boolean spFillInputBuffer(j_decompress_ptr iCInfo);
	static void spSkip(j_decompress_ptr iCInfo, long num_bytes);
	static void spTerminate(j_decompress_ptr iCInfo);

	const ZStreamR& fStream;
	vector<JOCTET> fBuffer;
	};

JPEGReader::JPEGReader(const ZStreamR& iStream)
:	fStream(iStream),
	fBuffer(1024)
	{
	init_source = spInititialize;
	fill_input_buffer = spFillInputBuffer;
	skip_input_data = spSkip;
	resync_to_restart = jpeg_resync_to_restart;
	term_source = spTerminate;
	}

JPEGReader::~JPEGReader()
	{}

void JPEGReader::spInititialize(j_decompress_ptr iCInfo)
	{
	JPEGReader* theReader = static_cast<JPEGReader*>(iCInfo->src);
	theReader->next_input_byte = &theReader->fBuffer[0];
	theReader->bytes_in_buffer = 0;
	}

boolean JPEGReader::spFillInputBuffer(j_decompress_ptr iCInfo)
	{
	try
		{
		JPEGReader* theReader = static_cast<JPEGReader*>(iCInfo->src);

		size_t countAvailable = theReader->fStream.CountReadable();

		if (countAvailable == 0)
			countAvailable = theReader->fBuffer.size();
		else
			countAvailable = min(countAvailable, theReader->fBuffer.size());

		size_t countRead;
		theReader->fStream.Read(&theReader->fBuffer[0], countAvailable, &countRead);
		if (countRead == 0)
			{
			// Insert a fake EOI marker - as per jpeglib recommendation
			theReader->fBuffer[0] = 0xFF;
			theReader->fBuffer[1] = JPEG_EOI;
			theReader->bytes_in_buffer = 2;
			}
		else
			{
			theReader->next_input_byte = &theReader->fBuffer[0];
			theReader->bytes_in_buffer = countRead;
			}

		return TRUE;
		}
	catch (...)
		{
		static_cast<JPEGErrorMgr*>(iCInfo->err)->Fail();
		}
	// Can't get here.
	ZUnimplemented();
	return FALSE;
	}

void JPEGReader::spSkip(j_decompress_ptr iCInfo, long num_bytes)
	{
	JPEGReader* theReader = static_cast<JPEGReader*>(iCInfo->src);
	if (theReader->bytes_in_buffer >= num_bytes)
		{
		theReader->bytes_in_buffer -= num_bytes;
		theReader->next_input_byte += num_bytes;
		}
	else
		{
		num_bytes -= theReader->bytes_in_buffer;
		theReader->bytes_in_buffer = 0;
		theReader->fStream.Skip(num_bytes);
		}
	}

void JPEGReader::spTerminate(j_decompress_ptr iCInfo)
	{}

} // anonymous namespace

// =================================================================================================
// MARK: - ZDCPixmapEncoder_JPEGLib

void ZDCPixmapEncoder_JPEGLib::sWritePixmap(const ZStreamW& iStream,
	const ZDCPixmap& iPixmap, int iQuality)
	{
	ZDCPixmapEncoder_JPEGLib theEncoder(iQuality);
	ZDCPixmapEncoder::sWritePixmap(iStream, iPixmap, theEncoder);
	}

ZDCPixmapEncoder_JPEGLib::ZDCPixmapEncoder_JPEGLib(int iQuality)
	{
	fQuality = iQuality;
	}

ZDCPixmapEncoder_JPEGLib::~ZDCPixmapEncoder_JPEGLib()
	{}

void ZDCPixmapEncoder_JPEGLib::Imp_Write(const ZStreamW& iStream,
	const void* iBaseAddress,
	const RasterDesc& iRasterDesc,
	const PixelDesc& iPixelDesc,
	const ZRectPOD& iBounds)
	{
	jpeg_compress_struct theJCS;

	JPEGErrorMgr theEM(theJCS);

	theEM.BeforeCall();
	::jpeg_create_compress(&theJCS);

	theJCS.image_width = W(iBounds);
	theJCS.image_height = H(iBounds);

	JPEGWriter theJW(iStream);
	theJCS.dest = &theJW;

	vector<uint8> rowBufferVector;
	try
		{
		PixvalDesc destPixvalDesc;
		PixelDesc destPixelDesc;

		ZRef<PixelDescRep> thePixelDescRep = iPixelDesc.GetRep();

		if (thePixelDescRep.DynamicCast<PixelDescRep_Gray>())
			{
			theJCS.input_components = 1;
			theJCS.in_color_space = JCS_GRAYSCALE;
			rowBufferVector.resize(W(iBounds));

			destPixelDesc = PixelDesc(eFormatStandard_Gray_8);
			destPixvalDesc = PixvalDesc(eFormatStandard_Gray_8);
			}
		else
			{
			theJCS.input_components = 3;
			theJCS.in_color_space = JCS_RGB;
			rowBufferVector.resize(3 * W(iBounds));

			destPixelDesc = PixelDesc(eFormatStandard_RGB_24);
			destPixvalDesc = PixvalDesc(eFormatStandard_RGB_24);
			}

		theEM.BeforeCall();
		::jpeg_set_defaults(&theJCS);
		::jpeg_set_quality(&theJCS, fQuality, TRUE);
		theJCS.dct_method = JDCT_FASTEST;

		::jpeg_start_compress(&theJCS, TRUE);

		// Special-case RGB sources.
		bool done = false;
		if (PixelDescRep_Color* thePixelDescRep_Color =
			thePixelDescRep.DynamicCast<PixelDescRep_Color>())
			{
			PixelDescRep_Color* destPDR =
				destPixelDesc.GetRep().DynamicCast<PixelDescRep_Color>();

			if (false && thePixelDescRep_Color->Matches(destPDR))
				{
				done = true;
				JSAMPROW rowPtr[1];
				for (size_t y = iBounds.top; y < iBounds.bottom; ++y)
					{
					const JSAMPLE* sourceRowAddress =
						static_cast<const JSAMPLE*>(iRasterDesc.CalcRowAddress(iBaseAddress, y));
					sourceRowAddress += (3 * iBounds.left);
					rowPtr[0] = const_cast<JSAMPLE*>(sourceRowAddress);

					theEM.BeforeCall();
					::jpeg_write_scanlines(&theJCS, rowPtr, 1);
					}
				}
			}

		if (!done)
			{
			JSAMPROW rowPtr[1];
			rowPtr[0] = &rowBufferVector[0];
			for (size_t y = iBounds.top; y < iBounds.bottom; ++y)
				{
				const void* sourceRowAddress = iRasterDesc.CalcRowAddress(iBaseAddress, y);

				sBlitRow(
					sourceRowAddress, iRasterDesc.fPixvalDesc, iPixelDesc, iBounds.left,
					rowPtr[0], destPixvalDesc, destPixelDesc, 0,
					W(iBounds));

				theEM.BeforeCall();
				::jpeg_write_scanlines(&theJCS, rowPtr, 1);
				}
			}

		::jpeg_finish_compress(&theJCS);
		}
	catch (...)
		{
		::jpeg_destroy(reinterpret_cast<j_common_ptr>(&theJCS));
		throw;
		}

	::jpeg_destroy(reinterpret_cast<j_common_ptr>(&theJCS));
	}

// =================================================================================================
// MARK: - ZDCPixmapDecoder_JPEGLib

ZDCPixmap ZDCPixmapDecoder_JPEGLib::sReadPixmap(const ZStreamR& iStream)
	{
	ZDCPixmapDecoder_JPEGLib theDecoder;
	return ZDCPixmapDecoder::sReadPixmap(iStream, theDecoder);
	}

ZDCPixmapDecoder_JPEGLib::ZDCPixmapDecoder_JPEGLib()
	{}

ZDCPixmapDecoder_JPEGLib::~ZDCPixmapDecoder_JPEGLib()
	{}

void ZDCPixmapDecoder_JPEGLib::Imp_Read(const ZStreamR& iStream, ZDCPixmap& oPixmap)
	{
	struct jpeg_decompress_struct theJDS;

	JPEGErrorMgr theEM(theJDS);

	theEM.BeforeCall();
	::jpeg_create_decompress(&theJDS);

	JPEGReader theJR(iStream);
	theJDS.src = &theJR;
	try
		{
		theEM.BeforeCall();
		/*int result = */::jpeg_read_header(&theJDS, TRUE);

		::jpeg_start_decompress(&theJDS);

		PixelDesc sourcePixelDesc;
		PixvalDesc sourcePixvalDesc;
		vector<uint8> rowBufferVector;
		if (theJDS.out_color_space == JCS_GRAYSCALE)
			{
			sourcePixelDesc = PixelDesc(eFormatStandard_Gray_8);
			sourcePixvalDesc = PixvalDesc(eFormatStandard_Gray_8);
			rowBufferVector.resize(theJDS.image_width);

			oPixmap = ZDCPixmap(sPointPOD(theJDS.image_width, theJDS.image_height),
				eFormatEfficient_Gray_8);
			}
		else if (theJDS.out_color_space == JCS_RGB)
			{
			sourcePixelDesc = PixelDesc(eFormatStandard_RGB_24);
			sourcePixvalDesc = PixvalDesc(eFormatStandard_RGB_24);
			rowBufferVector.resize(3 * theJDS.image_width);

			oPixmap = ZDCPixmap(sPointPOD(theJDS.image_width, theJDS.image_height),
				eFormatEfficient_Color_24);
			}
		else
			{
			// TODO. What about other color spaces?
			ZUnimplemented();
			}

		const PixelDesc& destPixelDesc = oPixmap.GetPixelDesc();
		const RasterDesc& destRasterDesc = oPixmap.GetRasterDesc();
		void* destBaseAddress = oPixmap.GetBaseAddress();

		JSAMPROW rowPtr[1];
		rowPtr[0] = &rowBufferVector[0];
		while (theJDS.output_scanline < theJDS.output_height)
			{
			theEM.BeforeCall();
			int scanlinesRead = ::jpeg_read_scanlines(&theJDS, rowPtr, 1);
			ZAssertStop(1, scanlinesRead == 1);

			void* destRowAddress =
				destRasterDesc.CalcRowAddressDest(destBaseAddress, theJDS.output_scanline - 1);

			sBlitRow(
				rowPtr[0], sourcePixvalDesc, sourcePixelDesc, 0,
				destRowAddress, destRasterDesc.fPixvalDesc, destPixelDesc, 0,
				theJDS.image_width);
			}
		::jpeg_finish_decompress(&theJDS);
		}
	catch (...)
		{
		::jpeg_destroy_decompress(&theJDS);
		throw;
		}

	::jpeg_destroy_decompress(&theJDS);
	}

} // namespace ZooLib
