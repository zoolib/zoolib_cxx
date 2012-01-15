/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZFileFormat_JPEG_h__
#define __ZFileFormat_JPEG_h__ 1
#include "zconfig.h"

#include "zoolib/ZStream.h"
#include "zoolib/ZTypes.h"

#include <vector>

namespace ZooLib {
namespace FileFormat {
namespace JPEG {

// =================================================================================================
// MARK: - ZFileFormat_JPEG

enum
	{
	eJPEG_TEM = 0x01,
	eJPEG_RES_Begin = 0x02,
	eJPEG_RES_End = 0xC0,

	eJPEG_SOF_0 = 0xC0,
	eJPEG_SOF_1 = 0xC1,
	eJPEG_SOF_2 = 0xC2,
	eJPEG_SOF_3 = 0xC3,

	eJPEG_DHT = 0xC4,

	eJPEG_SOF_5 = 0xC5,
	eJPEG_SOF_6 = 0xC6,
	eJPEG_SOF_7 = 0xC7,

	eJPEG_JPG = 0xC8,

	eJPEG_SOF_9 = 0xC9,
	eJPEG_SOF_A = 0xCA,
	eJPEG_SOF_B = 0xCB,

	eJPEG_DAC = 0xCC,

	eJPEG_SOF_D = 0xCD,
	eJPEG_SOF_E = 0xCE,
	eJPEG_SOF_F = 0xCF,

	eJPEG_RST_0 = 0xD0,
	eJPEG_RST_1 = 0xD1,
	eJPEG_RST_2 = 0xD2,
	eJPEG_RST_3 = 0xD3,
	eJPEG_RST_4 = 0xD4,
	eJPEG_RST_5 = 0xD5,
	eJPEG_RST_6 = 0xD6,
	eJPEG_RST_7 = 0xD7,

	eJPEG_SOI = 0xD8,
	eJPEG_EOI = 0xD9,
	eJPEG_SOS = 0xDA,
	eJPEG_DQT = 0xDB,
	eJPEG_DNL = 0xDC,
	eJPEG_DRI = 0xDD,
	eJPEG_DHP = 0xDE,
	eJPEG_EXP = 0xDF,

	eJPEG_APP_0 = 0xE0,
	eJPEG_APP_1 = 0xE1,
	eJPEG_APP_2 = 0xE2,
	eJPEG_APP_3 = 0xE3,
	eJPEG_APP_4 = 0xE4,
	eJPEG_APP_5 = 0xE5,
	eJPEG_APP_6 = 0xE6,
	eJPEG_APP_7 = 0xE7,
	eJPEG_APP_8 = 0xE8,
	eJPEG_APP_9 = 0xE9,
	eJPEG_APP_A = 0xEA,
	eJPEG_APP_B = 0xEB,
	eJPEG_APP_C = 0xEC,
	eJPEG_APP_D = 0xED,
	eJPEG_APP_E = 0xEE,
	eJPEG_APP_F = 0xEF,

	eJPEG_JPG_0 = 0xF0,
	eJPEG_JPG_1 = 0xF1,
	eJPEG_JPG_2 = 0xF2,
	eJPEG_JPG_3 = 0xF3,
	eJPEG_JPG_4 = 0xF4,
	eJPEG_JPG_5 = 0xF5,
	eJPEG_JPG_6 = 0xF6,
	eJPEG_JPG_7 = 0xF7,
	eJPEG_JPG_8 = 0xF8,
	eJPEG_JPG_9 = 0xF9,
	eJPEG_JPG_A = 0xFA,
	eJPEG_JPG_B = 0xFB,
	eJPEG_JPG_C = 0xFC,
	eJPEG_JPG_D = 0xFD,

	eJPEG_COM = 0xFE
	};

const char* sSegmentAsText(uint8 iSegment);
bool sIs_SOF(uint8 iSegment);
bool sIs_APP(uint8 iSegment);

// =================================================================================================
// MARK: - StreamR_Segment

class StreamR_Segment : public ZStreamR
	{
public:
	StreamR_Segment(uint8& oSegmentType, const ZStreamR& iStreamR);
	StreamR_Segment(uint8& oSegmentType, bool iSkipOnDestroy, const ZStreamR& iStreamR);
	~StreamR_Segment();

// From ZStreamR
	virtual void Imp_Read(void* oDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(double iTimeout);

	virtual void Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
		uint64* oCountRead, uint64* oCountWritten);

	virtual void Imp_Skip(uint64 iCount, uint64* oCountSkipped);

private:
	void pInit(uint8& oSegmentType, bool iSkipOnDestroy);

	const ZStreamR& fStreamR;
	bool fSkipOnDestroy;
	size_t fCountRemaining;
	};

} // namespace JPEG
} // namespace FileFormat
} // namespace ZooLib

#endif // __ZFileFormat_JPEG_h__
