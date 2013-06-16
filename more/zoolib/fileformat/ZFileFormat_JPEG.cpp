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

#include "zoolib/fileformat/ZFileFormat_JPEG.h"

#include "zoolib/ZStream_Tee.h"
#include "zoolib/ZStreamRWPos_RAM.h"
#include "zoolib/ZStreamW_HexStrim.h"
#include "zoolib/ZStrim.h"

namespace ZooLib {
namespace FileFormat {
namespace JPEG {

using std::min;

static const char* const spSegmentNames[] =
	{
	"SOF_0", // 0xC0
	"SOF_1",
	"SOF_2",
	"SOF_3",

	"DHT",

	"SOF_5",
	"SOF_6",
	"SOF_7",

	"JPG",

	"SOF_9",
	"SOF_A",
	"SOF_B",

	"DAC",

	"SOF_D",
	"SOF_E",
	"SOF_F",

	"RST_0", // 0xD0
	"RST_1",
	"RST_2",
	"RST_3",
	"RST_4",
	"RST_5",
	"RST_6",
	"RST_7",

	"SOI",
	"EOI",
	"SOS",
	"DQT",
	"DNL",
	"DRI",
	"DHP",
	"EXP",

	"APP_0", // 0xE0
	"APP_1",
	"APP_2",
	"APP_3",
	"APP_4",
	"APP_5",
	"APP_6",
	"APP_7",
	"APP_8",
	"APP_9",
	"APP_A",
	"APP_B",
	"APP_C",
	"APP_D",
	"APP_E",
	"APP_F",

	"JPG_0", // 0xF0
	"JPG_1",
	"JPG_2",
	"JPG_3",
	"JPG_4",
	"JPG_5",
	"JPG_6",
	"JPG_7",
	"JPG_8",
	"JPG_9",
	"JPG_A",
	"JPG_B",
	"JPG_C",
	"JPG_D",

	"COM", // 0xFE
	};

// =================================================================================================
// MARK: - ZFileFormat_JPEG

const char* sSegmentAsText(uint8 iSegment)
	{
	if (iSegment == 0x01)
		return "TEM";

	if (iSegment >= 0xC0 && iSegment <= 0xFE)
		return spSegmentNames[iSegment - 0xC0];

	return "??";
	}

bool sIs_SOF(uint8 iSegment)
	{
	// Note that there are no segments of eJPEG_SOF_4, eJPEG_SOF_8
	// or eJPEG_SOF_C. Those bytes values (0xC4, 0xC8 and 0xCC)
	// are used for other purposes.
	switch (iSegment)
		{
		case eJPEG_SOF_0:
		case eJPEG_SOF_1:
		case eJPEG_SOF_2:
		case eJPEG_SOF_3:
		case eJPEG_SOF_5:
		case eJPEG_SOF_6:
		case eJPEG_SOF_7:
		case eJPEG_SOF_9:
		case eJPEG_SOF_A:
		case eJPEG_SOF_B:
		case eJPEG_SOF_D:
		case eJPEG_SOF_E:
		case eJPEG_SOF_F:
			return true;
		}
	return false;
	}

bool sIs_APP(uint8 iSegment)
	{
	switch (iSegment)
		{
		case eJPEG_APP_0:
		case eJPEG_APP_1:
		case eJPEG_APP_2:
		case eJPEG_APP_3:
		case eJPEG_APP_4:
		case eJPEG_APP_5:
		case eJPEG_APP_6:
		case eJPEG_APP_7:
		case eJPEG_APP_8:
		case eJPEG_APP_9:
		case eJPEG_APP_A:
		case eJPEG_APP_B:
		case eJPEG_APP_C:
		case eJPEG_APP_D:
		case eJPEG_APP_E:
		case eJPEG_APP_F:
			return true;
		}
	return false;
	}

// =================================================================================================
// MARK: - StreamR_Segment

/**
A read filter stream that examines its source stream for 0xFF 0xXX sequences. If
the 0xXX byte is a valid JPEG segment start value then the constructor's \a oSegmentType
output parameter has the value written to it, and if it's one of the segments that
have a 16 bit big-endian segment length following it then the stream remembers
that value in fCountRemaining. Subsequent reads against the stream are capped
by the value of fCountRemaining. The destructor may skip the source stream to the end
of the discovered segment, thus aligning the source stream with the start of
the next segment. If that's not desired then the constructor taking a \a iSkipOnDestroy
parameter can be used, passing false to indicate that the source stream should be
left alone by the destructor. You can always call \c SkipAll if you'd like to
force the source stream to be moved to the end of the segment. If no valid segment start
sequence can be found then \a oSegmentType will be set to zero.

\sa ZFileFormat_IFF
\sa ZFileFormat_QuickTime
*/

StreamR_Segment::StreamR_Segment(uint8& oSegmentType, const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{
	this->pInit(oSegmentType, true);
	}

StreamR_Segment::StreamR_Segment(
	uint8& oSegmentType, bool iSkipOnDestroy, const ZStreamR& iStreamR)
:	fStreamR(iStreamR)
	{
	this->pInit(oSegmentType, iSkipOnDestroy);
	}

StreamR_Segment::~StreamR_Segment()
	{
	if (fSkipOnDestroy && fCountRemaining)
		{
		try
			{
			fStreamR.Skip(fCountRemaining);
			}
		catch (...)
			{}
		}
	}

void StreamR_Segment::Imp_Read(void* oDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = reinterpret_cast<uint8*>(oDest);
	while (iCount && fCountRemaining)
		{
		size_t countRead;
		fStreamR.Read(localDest, min(iCount, fCountRemaining), &countRead);
		if (countRead == 0)
			break;
		localDest += countRead;
		fCountRemaining -= countRead;
		iCount -= countRead;
		}
	if (oCountRead)
		*oCountRead = localDest - reinterpret_cast<uint8*>(oDest);
	}

size_t StreamR_Segment::Imp_CountReadable()
	{ return min(fCountRemaining, fStreamR.CountReadable()); }

bool StreamR_Segment::Imp_WaitReadable(double iTimeout)
	{ return fStreamR.WaitReadable(iTimeout); }

void StreamR_Segment::Imp_CopyToDispatch(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamR.CopyTo(iStreamW, min(iCount, uint64(fCountRemaining)), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void StreamR_Segment::Imp_CopyTo(const ZStreamW& iStreamW, uint64 iCount,
	uint64* oCountRead, uint64* oCountWritten)
	{
	uint64 countRead;
	fStreamR.CopyTo(iStreamW, min(iCount, uint64(fCountRemaining)), &countRead, oCountWritten);
	fCountRemaining -= countRead;
	if (oCountRead)
		*oCountRead = countRead;
	}

void StreamR_Segment::Imp_Skip(uint64 iCount, uint64* oCountSkipped)
	{
	uint64 countSkipped;
	fStreamR.Skip(min(iCount, uint64(fCountRemaining)), &countSkipped);
	fCountRemaining -= countSkipped;
	if (oCountSkipped)
		*oCountSkipped = countSkipped;
	}

void StreamR_Segment::pInit(uint8& oSegmentType, bool iSkipOnDestroy)
	{
	fSkipOnDestroy = iSkipOnDestroy;
	fCountRemaining = 0;
	oSegmentType = 0;

	// Find the segment start by scanning for a 0xFF byte,
	// then examine the byte that follows to see what
	// kind of segment we've found, if any.
	bool gotFF = false;
	for (;;)
		{
		uint8 theByte;
		if (!fStreamR.ReadByte(theByte))
			{
			// The stream is truncated, return without
			// updating oSegmentType or fCountRemaining.
			return;
			}

		if (theByte == 0xFF)
			{
			gotFF = true;
			continue;
			}

		if (!gotFF)
			{
			// It's a byte without a preceding 0xFF,
			// just go around again.
			continue;
			}

		switch (theByte)
			{
			case eJPEG_TEM:
			case eJPEG_RST_0:
			case eJPEG_RST_1:
			case eJPEG_RST_2:
			case eJPEG_RST_3:
			case eJPEG_RST_4:
			case eJPEG_RST_5:
			case eJPEG_RST_6:
			case eJPEG_RST_7:
			case eJPEG_SOI:
			case eJPEG_EOI:
				{
				// A segment that has no following 16 bit length field.
				// fCountRemaining thus does not need to be changed
				// from the zero value it was initialized with.
				oSegmentType = theByte;
				break;
				}
			default:
				{
				if (theByte < eJPEG_RES_End)
					{
					// It's outside the range of valid segment markers.
					// We'll assume we're in some bogus data and go around again.
					gotFF = false;
					continue;
					}
				try
					{
					// Read the 16 bit length field that follows. Note
					// that the length includes the length field itself.
					fCountRemaining = fStreamR.ReadUInt16() - sizeof(uint16);
					oSegmentType = theByte;
					}
				catch (...)
					{
					// The stream was truncated. We'll drop out without
					// having set oSegmentType or changed fCountRemaining,
					// which will both remain zero and thus indicate that
					// the stream is malformed.
					}
				}
			}
		// This is the end of the loop, which is only gone around
		// by virtue of the explicit calls to continue above.
		return;
		}
	}

// =================================================================================================
// MARK: - ZFileFormat_JPEG, testing

void sDumpSegments(const ZStreamR& iStreamR, const ZStrimW& iStrimW);
void sDumpSegments(const ZStreamR& iStreamR, const ZStrimW& iStrimW)
	{
	for (;;)
		{
		uint8 segmentType;
		StreamR_Segment s(segmentType, iStreamR);
		if (!segmentType)
			break;

		const char* markerName = "";
		if (segmentType >= 0xC0)
			markerName = sSegmentAsText(segmentType - 0xC0);

		iStrimW.Writef("Type: %02X, %s ", segmentType, markerName);

		if (sIs_SOF(segmentType))
			{
			iStrimW.Writef("Precision: %d", s.ReadUInt8());
			iStrimW.Writef(", Y: %d", s.ReadUInt16());
			iStrimW.Writef(", X: %d", s.ReadUInt16());
			iStrimW.Writef(", Nf: %d", s.ReadUInt8());
			iStrimW.Writef("\n");
			}
		else if (sIs_APP(segmentType) || segmentType == eJPEG_COM)
			{
			iStrimW.Writef("\n");
			for (;;)
				{
				uint64 countRead;
				ZStreamRWPos_RAM theStream;
				s.CopyTo(ZStreamW_Tee(theStream,
					ZStreamW_HexStrim(" ", "", 100, iStrimW)), 16, &countRead, nullptr);

				if (countRead == 0)
					break;
				theStream.SetPosition(0);
				iStrimW.Write(" |");
				for (;;)
					{
					uint8 theByte;
					if (!theStream.ReadByte(theByte))
						break;
					if (theByte < 0x20 || theByte >= 0x7f)
						iStrimW.WriteCP('.');
					else
						iStrimW.WriteCP(theByte);
					}
				iStrimW.Write("|\n");
				}
			}
		else
			{
			uint64 theSize;
			s.SkipAll(&theSize);
			iStrimW.Writef("size: %lld\n", theSize);
			}
		}
	}

} // namespace JPEG
} // namespace FileFormat
} // namespace ZooLib
