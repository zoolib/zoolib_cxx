// Copyright (c) 2021 Andrew Green and Mark/Space, Inc. MIT License. http://www.zoolib.org

#include "zoolib/Pull_bplist.h"

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_XX_Count.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/ParseException.h"
#include "zoolib/Stringf.h"
#include "zoolib/Time.h"
#include "zoolib/Unicode.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_STL_vector.h"
#include "zoolib/UUID.h"

// FIXME. We need a strategy for large blists loaded into 32 bit processes.
// Possibly we throw when we encounter >4GB elements.

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"

namespace ZooLib {

using namespace PullPush;
using namespace Util_STL;

using std::vector;

// =================================================================================================
#pragma mark -

const PPT Start_Seq_Array::sPPT = ZP<Marker>(new Start_Seq_Array);

bool Start_Seq_Array::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Seq_Array>())
			return true;
		}
	return false;
	}

// -----

const PPT Start_Seq_SetOrdered::sPPT = ZP<Marker>(new Start_Seq_SetOrdered);

bool Start_Seq_SetOrdered::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Seq_SetOrdered>())
			return true;
		}
	return false;
	}

// -----

const PPT Start_Seq_SetUnordered::sPPT = ZP<Marker>(new Start_Seq_SetUnordered);

bool Start_Seq_SetUnordered::sIs(const PPT& iPPT)
	{
	if (const ZP<Marker> theMarker = sGet<ZP<Marker>>(iPPT))
		{
		if (theMarker.DynamicCast<Start_Seq_SetUnordered>())
			return true;
		}
	return false;
	}

// =================================================================================================
#pragma mark - sPull_bplist_Push_PPT

static uint64 spEReadUnsigned(const ChanR_Bin& iChanR, uint8 iIntSize)
	{
	switch (iIntSize)
		{
		case 1: return sEReadBE<uint8>(iChanR);
		case 2: return sEReadBE<uint16>(iChanR);
		case 4: return sEReadBE<uint32>(iChanR);
		case 8: return sEReadBE<uint64>(iChanR);
		}
	sThrow_ParseException(sStringf("spEReadUnsigned, illegal iIntSize: %d", int(iIntSize)));
	}

static int64 spEReadInt(const ChanR_Bin& iChanR, uint8 iIntSize)
	{
	switch (iIntSize)
		{
		case 1: return sEReadBE<int8>(iChanR);
		case 2: return sEReadBE<int16>(iChanR);
		case 4: return sEReadBE<int32>(iChanR);
		case 8: return sEReadBE<int64>(iChanR);
		}
	sThrow_ParseException(sStringf("spEReadInt, illegal iIntSize: %d", int(iIntSize)));
	}

static uint64 spReadLength(const ChanR_Bin& iChanR, uint8 iInfo)
	{
	if (iInfo != 0x0F)
		return iInfo;

	uint8 theByte = sEReadBE<uint8>(iChanR);
	if ((theByte & 0xF0) != 0x10)
		sThrow_ParseException("Length high nibble != 1");

	return spEReadUnsigned(iChanR, 1 << (theByte & 0xF));
	}

static vector<uint64> spReadOffsets(uint8 iObjectRefSize, const ChanR_Bin& iChanR, uint64 iCount)
	{
	vector<uint64> result;
	while (iCount--)
		sPushBack(result, spEReadUnsigned(iChanR, iObjectRefSize));
	return result;
	}

static string8 spReadString8(uint8 iType, uint8 iInfo, const ChanR_Bin& iChanR)
	{
	const uint64 theLength = spReadLength(iChanR, iInfo);
	if (iType == 5 || iType == 7)
		{
		// ASCII is 5, UTF8 is 7. As ASCII is a subset of UTF8, we can use the same code
		// to read both. If the ASCII is actually iso8859-1, then we'd need to handle
		// codepoints 0x80-0xFF specially.
		return sReadString(iChanR, theLength);
		}

	// This is uglier than I'd prefer.
	string16 theString16(theLength, 0);
	sEReadMem(iChanR, &theString16[0], theLength * 2);
	if (ZCONFIG(Endian, Little))
		{
		for (size_t xx = 0; xx < theLength; ++xx)
			theString16[xx] = sByteSwapped(theString16[xx]);
		}
	return Unicode::sAsUTF8(theString16);
	}

static void spPull_bplist_Push_PPT(uint8 iObjectRefSize, const vector<uint64>& iOffsets,
	uint64 iObjectIndex,
	const ChanRPos_Bin& iChanRPos, const ChanW_PPT& iChanW)
	{
	const uint64 theOffset = iOffsets.at(iObjectIndex);
	sPosSet(iChanRPos, theOffset);
	const uint8 theByte = sERead<uint8>(iChanRPos);
	const uint8 theType = theByte >> 4;
	const uint8 theInfo = theByte & 0xF;

	switch (theType)
		{
		case 0: // Simple
			{
			switch (theInfo)
				{
				case 0:
					{
					sPush(null, iChanW);
					return;
					}
				case 8:
					{
					sPush(false, iChanW);
					return;
					}
				case 9:
					{
					sPush(true, iChanW);
					return;
					}
				case 0xE: // 16 byte UUID
					{
					UUID theUUID;
					sEReadMem(iChanRPos, &theUUID, 16);
					sPush(theUUID, iChanW);
					return;
					}
				case 0xF: // Fill byte
					{
					sPush(null, iChanW);
					return;
					}

//				// Recognized but unhandled
//				case 0xC: // URL with no base URL (v1.0 and later)
//					{
//					sPush(null, iChanW);
//					return;
//					}
//				case 0xD: // with base URL (v1.0 and later)
//					{
//					sPush(null, iChanW);
//					return;
//					}
				}
			sThrow_ParseException(sStringf("Unhandled simple: %d", theInfo));
			}
		case 1: // Integer
			{
			const int64 theInt = spEReadInt(iChanRPos, 1 << theInfo);
			sPush(theInt, iChanW);
			return;
			}
		case 2: // Rat
			{
			if (4 == (1 << theInfo))
				sPush(sEReadBE<float>(iChanRPos), iChanW);
			else if (8 == (1 << theInfo))
				sPush(sEReadBE<double>(iChanRPos), iChanW);
			else
				sThrow_ParseException(sStringf("Invalid real, log2(size): %d", theInfo));
			return;
			}
		case 3: // Date
			{
			if (theInfo != 0x3)
				sThrow_ParseException(sStringf("Unknown date sub-type: %d", theInfo));
			sPush(UTCDateTime(sEReadBE<double>(iChanRPos) + Time::kEpochDelta_2001_To_1970), iChanW);
			return;
			}
		case 4: // Data (binary)
			{
			const uint64 theLength = spReadLength(iChanRPos, theInfo);
			sPush(sRead_T<Data_ZZ>(iChanRPos, theLength), iChanW);
			return;
			}
		case 5: // ASCII string
		case 6: // UTF16BE string
		case 7: // UTF8 string
			{
			sPush(spReadString8(theType, theInfo, iChanRPos), iChanW);
			return;
			}
		case 8: // UID
			{
			const uint64 theLength = spReadLength(iChanRPos, theInfo);
			bplist_UID theUID(size_t(theLength),0);
			sEReadMem(iChanRPos, &theUID.Mut()[0], theLength);
			sPush(theUID, iChanW);
			return;
			}
		case 0xD: // Dictionary
			{
			const uint64 theCount = spReadLength(iChanRPos, theInfo);
			sPush_Start_Map(iChanW);

			vector<uint64> nameIndices = spReadOffsets(iObjectRefSize, iChanRPos, theCount);
			vector<uint64> valueIndices = spReadOffsets(iObjectRefSize, iChanRPos, theCount);

			for (uint64 xx = 0; xx < theCount; ++xx)
				{
				// Read name. Has to be ASCII string for now.
				sPosSet(iChanRPos, iOffsets.at(nameIndices.at(xx)));
				const uint8 theByte = sERead<uint8>(iChanRPos);
				const uint8 theType = theByte >> 4;
				if (theType != 5 && theType != 6 && theType != 7)
					{
					if (ZLOGF(cc, eDebug))
						cc << "Non-string key";
					}
				else
					{
					const string8 theName = spReadString8(theType, theByte & 0xF, iChanRPos);
					sPush(sName(theName), iChanW);

					spPull_bplist_Push_PPT(iObjectRefSize, iOffsets, valueIndices.at(xx), iChanRPos, iChanW);
					}
				}
			sPush_End(iChanW);
			return;
			}
		case 0xA: case 0xB: case 0xC:
			{
			const uint64 theCount = spReadLength(iChanRPos, theInfo);
			switch (theType)
				{
				case 0xA: sPush(Start_Seq_Array::sPPT, iChanW); break;
				case 0xB: sPush(Start_Seq_SetOrdered::sPPT, iChanW); break;
				case 0xC: sPush(Start_Seq_SetUnordered::sPPT, iChanW); break;
				}

			vector<uint64> theIndices = spReadOffsets(iObjectRefSize, iChanRPos, theCount);
			for (uint64 xx = 0; xx < theCount; ++xx)
				spPull_bplist_Push_PPT(iObjectRefSize, iOffsets, theIndices[xx], iChanRPos, iChanW);

			sPush_End(iChanW);
			return;
			}
		}
	sThrow_ParseException(sStringf("Unhandled type: %d", theType));
	}

void sPull_bplist_Push_PPT(const ChanRPos_Bin& iChanRPos, const ChanW_PPT& iChanW)
	{
	if (not sRead_String(iChanRPos, "bplist"))
		sThrow_ParseException("Not a bplist");

	const int versionMajor = sEReadBE<char>(iChanRPos);
	/* const int versionMinor = */ sEReadBE<char>(iChanRPos);
	if (versionMajor != 0x30 && versionMajor != 0x31)
		sThrow_ParseException("bplist version not supported");

	const uint64 theSize = sSize(iChanRPos);

	sPosSet(iChanRPos, theSize - 32);
	const uint32 zeroBytes0 = sEReadBE<uint32>(iChanRPos);
	const uint16 zeroBytes1 = sEReadBE<uint16>(iChanRPos);

	if (zeroBytes0 || zeroBytes1)
		sThrow_ParseException("Malformed bplist");

	const uint8 offsetSize = sEReadBE<uint8>(iChanRPos);
	if (offsetSize != 1 && offsetSize != 2 && offsetSize != 4 && offsetSize != 8)
		sThrow_ParseException("Malformed bplist");

	const uint8 objectRefSize = sEReadBE<uint8>(iChanRPos);
	if (objectRefSize != 1 && objectRefSize != 2 && objectRefSize != 4 && objectRefSize != 8)
		sThrow_ParseException("Malformed bplist");

	const uint64 numObjects = sEReadBE<uint64>(iChanRPos);
	const uint64 topObjectIndex = sEReadBE<uint64>(iChanRPos);
	const uint64 offsetTableOffset = sEReadBE<uint64>(iChanRPos);

	sPosSet(iChanRPos, offsetTableOffset);
	vector<uint64> theOffsets = spReadOffsets(offsetSize, iChanRPos, numObjects);

	spPull_bplist_Push_PPT(objectRefSize, theOffsets, topObjectIndex, iChanRPos, iChanW);
	}

// =================================================================================================
#pragma mark - sChannerR_PPT_xx

ZP<ChannerR_PPT> sChannerR_PPT_bplist(const ZP<Channer<ChanRPos_Bin>>& iChanner)
	{ sStartPullPush(sCallable(sPull_bplist_Push_PPT), iChanner); }

} // namespace ZooLib

#pragma clang diagnostic pop
