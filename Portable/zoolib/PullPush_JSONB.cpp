// Copyright (c) 2018 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/PullPush_JSONB.h"

#include "zoolib/ChanR_UTF.h"
#include "zoolib/ChanR_Bin_More.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Channer_Bin.h"
#include "zoolib/Channer_UTF.h"
#include "zoolib/Coerce_Any.h"
#include "zoolib/Data_ZZ.h"
#include "zoolib/Log.h"
#include "zoolib/NameUniquifier.h"
#include "zoolib/ParseException.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_Chan.h"

#include <vector>

namespace ZooLib {

using namespace PullPush;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark -

namespace {

enum class EType : byte
	{
	Null = 0xE0,
	False = 0xE2,
	True = 0xE3,
	Int64 = 0xE4,
	Float64 = 0xE5,
	Binary_Chunked = 0xE7,
	UTF_Complete = 0xE8,
	Seq = 0xEA,
	Map = 0xED,
	End = 0xFF,
	};

} // namespace

// =================================================================================================
#pragma mark -

static void spPull_JSONB_Push_PPT(byte iType, const ChanR_Bin& iChanR,
	const ZP<Callable_JSONB_ReadFilter>& iReadFilter,
	const ChanW_PPT& iChanW);

bool sPull_JSONB_Push_PPT(const ChanR_Bin& iChanR,
	const ZP<Callable_JSONB_ReadFilter>& iReadFilter,
	const ChanW_PPT& iChanW)
	{
	if (NotQ<byte> theTypeQ = sQRead(iChanR))
		{
		return false;
		}
	else
		{
		spPull_JSONB_Push_PPT(*theTypeQ, iChanR, iReadFilter, iChanW);
		return true;
		}
	}

void spPull_JSONB_Push_PPT(byte iType, const ChanR_Bin& iChanR,
	const ZP<Callable_JSONB_ReadFilter>& iReadFilter,
	const ChanW_PPT& iChanW)
	{
	if (iReadFilter && sCall(iReadFilter, iType, iChanR, iChanW))
		return;

	switch (iType)
		{
		case 0xE0:
			{
			sPush(PPT(), iChanW);
			break;
			}
		case 0xE2:
			{
			sPush(false, iChanW);
			break;
			}
		case 0xE3:
			{
			sPush(true, iChanW);
			break;
			}
		case 0xE4:
			{
			sPush(sEReadBE<int64>(iChanR), iChanW);
			break;
			}
		case 0xE5:
			{
			sPush(sEReadBE<double>(iChanR), iChanW);
			break;
			}
		case 0xE7:
			{
			PullPushPair<byte> thePullPushPair = sMakePullPushPair<byte>();
			sPush(sGetClear(thePullPushPair.second), iChanW);
			sFlush(iChanW);
			for (;;)
				{
				if (uint64 theCount = sReadCount(iChanR))
					sECopyFully(iChanR, *thePullPushPair.first, theCount);
				else
					break;
				}
			sDisconnectWrite(*thePullPushPair.first);
			break;
			}
		case 0xE8:
			{
			sPush(sReadString(iChanR, size_t(sReadCount(iChanR))), iChanW);
			break;
			}
		case 0xEA:
			{
			sPush_Start_Seq(iChanW);
			for (;;)
				{
				if (NotQ<byte> theTypeQ = sQRead(iChanR))
					{
					sThrow_ParseException("Unexpected end of ChanR_Bin");
					}
				else if (*theTypeQ == 0xFF)
					{
					break;
					}
				else
					{
					spPull_JSONB_Push_PPT(*theTypeQ, iChanR, iReadFilter, iChanW);
					}
				}
			sPush_End(iChanW);
			break;
			}
		case 0xED:
			{
			sPush_Start_Map(iChanW);
			for (;;)
				{
				string theName = sReadCountPrefixedString(iChanR);
				if (NotQ<byte> theTypeQ = sQRead(iChanR))
					{
					sThrow_ParseException("Unexpected end of ChanR_Bin");
					}
				else if (*theTypeQ == 0xFF)
					{
					break;
					}
				else
					{
					sPush(sName(theName), iChanW);
					spPull_JSONB_Push_PPT(*theTypeQ, iChanR, iReadFilter, iChanW);
					}
				}
			sPush_End(iChanW);
			break;
			}
		default:
			{
			sThrow_ParseException(sStringf("JSONB unhandled type %d", int(iType)));
			break;
			}
		}
	}

// =================================================================================================
#pragma mark -

bool sPull_PPT_Push_JSONB(const ChanR_PPT& iChanR,
	const ZP<Callable_JSONB_WriteFilter>& iWriteFilter,
	const ChanW_Bin& iChanW)
	{
	ZQ<PPT> thePPTQ = sQRead(iChanR);

	if (not thePPTQ)
		return false;

	const PPT& thePPT = *thePPTQ;

	if (sIsEnd(thePPT))
		return false;

	if (iWriteFilter && sCall(iWriteFilter, thePPT, iChanR, iChanW))
		return true;

	if (const string* theString = sPGet<string>(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE8);
		sEWriteCountPrefixedString(iChanW, *theString);
		return true;
		}

	if (ZP<ChannerR_UTF> theChanner = sGet<ZP<ChannerR_UTF>>(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE8);
		sEWriteCountPrefixedString(iChanW, sReadAllUTF8(*theChanner));
		return true;
		}

	if (const Data_ZZ* theData = sPGet<Data_ZZ>(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE7);
		if (size_t theSize = theData->GetSize())
			{
			sEWriteCount(iChanW, theSize);
			sEWriteMem(iChanW, theData->GetPtr(), theData->GetSize());
			}
		sEWriteCount(iChanW, 0);
		return true;
		}

	if (ZP<ChannerR_Bin> theChanner = sGet<ZP<ChannerR_Bin>>(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE7);
		const size_t chunkSize = 64 * 1024;
		byte buffer[chunkSize];
		for (;;)
			{
			const size_t countRead = sReadFully(*theChanner, &buffer[0], chunkSize);
			sEWriteCount(iChanW, countRead);
			if (not countRead)
				break;
			sEWrite(iChanW, &buffer[0], countRead);
			}
		return true;
		}

	if (sIsStart_Map(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xED);
		for (;;)
			{
			if (NotQ<Name> theNameQ = sQEReadNameOrEnd(iChanR))
				{
				// Empty name
				sEWriteBE<uint8>(iChanW, 0);
				break;
				}
			else
				{
				sEWriteCountPrefixedString(iChanW, *theNameQ);
				if (not sPull_PPT_Push_JSONB(iChanR, iWriteFilter, iChanW))
					sThrow_ParseException("Require value after Name from ChanR_PPT");
				}
			}
		// Terminator
		sEWriteBE<uint8>(iChanW, 0xFF);
		return true;
		}

	if (sIsStart_Seq(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xEA);
		for (;;)
			{
			if (not sPull_PPT_Push_JSONB(iChanR, iWriteFilter, iChanW))
				break;
			}
		sEWriteBE<uint8>(iChanW, 0xFF);
		return true;
		}

	if (thePPT.IsNull())
		{
		sEWriteBE<uint8>(iChanW, 0xE0);
		return true;
		}

	if (const bool* p = sPGet<bool>(thePPT))
		{
		if (*p)
			sEWriteBE<uint8>(iChanW, 0xE3);
		else
			sEWriteBE<uint8>(iChanW, 0xE2);
		return true;
		}

	if (ZQ<int64> theQ = sQCoerceInt(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE4);
		sEWriteBE<int64>(iChanW, *theQ);
		return true;
		}

	if (ZQ<double> theQ = sQCoerceRat(thePPT))
		{
		sEWriteBE<uint8>(iChanW, 0xE5);
		sEWriteBE<double>(iChanW, *theQ);
		return true;
		}

	if (ZLOGF(w, eErr))
		w << "Couldn't write " << thePPT.Type().name();

	ZUnimplemented();
	}

} // namespace ZooLib
