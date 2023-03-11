// Copyright (c) 2021 Andrew Green and Mark/Space, Inc. MIT License. http://www.zoolib.org

#include "zoolib/Push_bplist.h"

#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_XX_Count.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Log.h"
#include "zoolib/Unicode.h"
#include "zoolib/UTCDateTime.h"
#include "zoolib/Util_Debug.h" // For sPrettyName
#include "zoolib/Util_STL_vector.h"

namespace ZooLib {

using namespace Util_STL;

using std::vector;

// =================================================================================================
#pragma mark -

static void spWriteUnsigneds(const ChanW_Bin& iChanW,
	uint8 iIntWidth, const vector<uint64>& iIndices)
	{
	switch (iIntWidth)
		{
		case 1:
			{
			for (auto&& index: iIndices)
				sEWriteBE<uint8>(iChanW, index);
			break;
			}
		case 2:
			{
			for (auto&& index: iIndices)
				sEWriteBE<uint16>(iChanW, index);
			break;
			}
		case 4:
			{
			for (auto&& index: iIndices)
				sEWriteBE<uint32>(iChanW, uint32(index));
			break;
			}
		case 8:
			{
			for (auto&& index: iIndices)
				sEWriteBE<uint64>(iChanW, index);
			break;
			}
		default:
			{
			ZUnimplemented();
			}
		}
	}

static void spWriteUnsigned(const ChanW_Bin& iChanW, uint64 iUnsigned)
	{
	if (iUnsigned < (1ULL << 8))
		{
		sEWriteBE<uint8>(iChanW, 0x10);
		sEWriteBE<uint8>(iChanW, iUnsigned);
		}
	else if (iUnsigned < (1ULL << 16))
		{
		sEWriteBE<uint8>(iChanW, 0x11);
		sEWriteBE<uint16>(iChanW, iUnsigned);
		}
	else if (iUnsigned < (1ULL << 32))
		{
		sEWriteBE<uint8>(iChanW, 0x12);
		sEWriteBE<uint32>(iChanW, uint32(iUnsigned));
		}
	else
		{
		sEWriteBE<uint8>(iChanW, 0x13);
		sEWriteBE<uint64>(iChanW, iUnsigned);
		}
	}

static void spWriteTypeCount(const ChanW_Bin& iChanW, uint8 iType, uint64 iCount)
	{
	if (iCount >= 0 && iCount < 0xF)
		{
		sEWriteBE<uint8>(iChanW, (iType << 4) | iCount);
		}
	else
		{
		sEWriteBE<uint8>(iChanW, (iType << 4) | 0xF);
		spWriteUnsigned(iChanW, iCount);
		}
	}

static void spFromZZ_Push_bplist(const Val_ZZ& iVal, const ChanW_XX_Count<ChanW_Bin>& iChanW,
	uint8 iObjectRefWidth, vector<uint64>& ioOffsets)
	{
	if (auto theP = iVal.PGet<Map_ZZ>())
		{
		vector<uint64> nameIndices;
		for (auto&& curNameVal: *theP)
			{
			const string8& asString = curNameVal.first.AsString8();
			spWriteTypeCount(iChanW, 0x5, asString.length());
			sEWrite(iChanW, asString);
			nameIndices.push_back(ioOffsets.size() - 1);
			ioOffsets.push_back(iChanW.GetCount());
			}

		vector<uint64> valueIndices;
		for (auto&& curNameVal: *theP)
			{
			spFromZZ_Push_bplist(curNameVal.second, iChanW, iObjectRefWidth, ioOffsets);
			valueIndices.push_back(ioOffsets.size() - 1);
			ioOffsets.push_back(iChanW.GetCount());
			}

		spWriteTypeCount(iChanW, 0xD, nameIndices.size());
		spWriteUnsigneds(iChanW, iObjectRefWidth, nameIndices);
		spWriteUnsigneds(iChanW, iObjectRefWidth, valueIndices);
		}

	else if (auto theP = iVal.PGet<Seq_ZZ>())
		{
		vector<uint64> valueIndices;
		for (auto&& curVal: *theP)
			{
			spFromZZ_Push_bplist(curVal, iChanW, iObjectRefWidth, ioOffsets);
			valueIndices.push_back(ioOffsets.size() - 1);
			ioOffsets.push_back(iChanW.GetCount());
			}

		spWriteTypeCount(iChanW, 0xA, valueIndices.size());
		spWriteUnsigneds(iChanW, iObjectRefWidth, valueIndices);
		}

	else if (auto theP = iVal.PGet<bool>())
		{
		if (*theP)
			sEWriteBE<uint8>(iChanW, 0x09);
		else
			sEWriteBE<uint8>(iChanW, 0x08);
		}

	else if (ZQ<int64> theQ = sQCoerceInt(iVal))
		{
		// By inspection, negative integers are written as unsigned 64 bit values.
		spWriteUnsigned(iChanW, *theQ);
		}

	else if (auto theP = iVal.PGet<float>())
		{
		sEWriteBE<uint8>(iChanW, 0x22);
		sEWriteBE<float>(iChanW, *theP);
		}

	else if (auto theP = iVal.PGet<double>())
		{
		sEWriteBE<uint8>(iChanW, 0x23);
		sEWriteBE<double>(iChanW, *theP);
		}

	else if (auto theP = iVal.PGet<string8>())
		{
		// Other code *really* wants type 5 (ASCII), it doesn't like 5 (UTF8)
		spWriteTypeCount(iChanW, 0x5, theP->length());
		sEWrite(iChanW, *theP);
		}

	else if (auto theP = iVal.PGet<Data_ZZ>())
		{
		spWriteTypeCount(iChanW, 0x4, theP->GetSize());
		sEWriteMem(iChanW, sPaC(*theP));
		}

	else if (auto theP = iVal.PGet<UTCDateTime>())
		{
		sEWriteBE<uint8>(iChanW, 0x33); // Subtype 3 (8 bytes)
		sEWriteBE<double>(iChanW, *theP - Time::kEpochDelta_2001_To_1970);
		}

	else
		{
		// We don't handle null -- that seems to require `bplist1?` format. Which
		// also means we can't punt when we find any other value we can't represent.
		// Abort for now, till we see how much of a problem this is.

		if (ZLOGF(w, eErr))
			w << "Couldn't write " << Util_Debug::sPrettyName(iVal.Type());

		ZUnimplemented();
		}
	}

static uint64 spCountObjectRefs(const Val_ZZ& iVal)
	{
	if (auto&& theP = iVal.PGet<Map_ZZ>())
		{
		uint64 count = 0;
		for (auto&& nv: *theP)
			{
			count += 1; // For the name
			count += 1; // For the value
			count += spCountObjectRefs(nv.second); // For anything it contains
			}
		return count;
		}

	if (auto&& theP = iVal.PGet<Seq_ZZ>())
		{
		uint64 count = 0;
		for (auto&& vv: *theP)
			{
			count += 1; // For the value
			count += spCountObjectRefs(vv); // For anything it contains
			}
		return count;
		}

	return 0;
	}

static uint8 spIntWidthRequired(uint64 iUnsigned)
	{
	if (iUnsigned < (1ULL << 8))
		return 1;

	if (iUnsigned < (1ULL << 16))
		return 2;

	if (iUnsigned < (1ULL << 32))
		return 4;

	return 8;
	}

void sFromZZ_Push_bplist(const Val_ZZ& iVal, const ChanW_Bin& iChanW)
	{
	ChanW_XX_Count<ChanW_Bin> theChanW(iChanW);

// Write the header.
	sEWrite(theChanW, "bplist00");

// Write the objects, populating the offset table as we go
	const uint8 theObjectRefWidth = spIntWidthRequired(spCountObjectRefs(iVal) + 1);

	vector<uint64> theOffsets;
	theOffsets.push_back(theChanW.GetCount());
	spFromZZ_Push_bplist(iVal, theChanW, theObjectRefWidth, theOffsets);

// Write the offset table
	const uint64 beginningOfOffsetTable = theChanW.GetCount();

	const uint8 theOffsetWidthRequired = spIntWidthRequired(beginningOfOffsetTable);

	spWriteUnsigneds(theChanW, theOffsetWidthRequired, theOffsets);

// Write the trailer.

	// Six zero bytes
	sEWriteBE<uint8>(theChanW, 0);
	sEWriteBE<uint8>(theChanW, 0);
	sEWriteBE<uint8>(theChanW, 0);
	sEWriteBE<uint8>(theChanW, 0);
	sEWriteBE<uint8>(theChanW, 0);
	sEWriteBE<uint8>(theChanW, 0);

	// Number of bytes per offset entry (1, 2, 4 or 8)
	sEWriteBE<uint8>(theChanW, theOffsetWidthRequired);

	// Object ref size
	sEWriteBE<uint8>(theChanW, theObjectRefWidth);

	// Number of objects
	sEWriteBE<uint64>(theChanW, theOffsets.size());

	// Top object index
	sEWriteBE<uint64>(theChanW, theOffsets.size() - 1);

	// Offset table offset
	sEWriteBE<uint64>(theChanW, beginningOfOffsetTable);
	}

} // namespace ZooLib
