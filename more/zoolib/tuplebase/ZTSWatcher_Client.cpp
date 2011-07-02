/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTSWatcher_Client.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZStream_Count.h"
#include "zoolib/ZUtil_STL.h"
#include "zoolib/tuplebase/ZUtil_Strim_Tuple.h"

namespace ZooLib {

using std::map;
using std::pair;
using std::set;
using std::vector;

#ifdef ZCONFIG_TSWatcherClient_ShowCommsTuples
#	define kDebug_ShowCommsTuples ZCONFIG_TSWatcherClient_ShowCommsTuples
#else
#	define kDebug_ShowCommsTuples 0
#endif

#ifdef ZCONFIG_TSWatcherClient_ShowStats
#	define kDebug_ShowStats ZCONFIG_TSWatcherClient_ShowStats
#else
#	define kDebug_ShowStats 0
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Helper functions

static inline void spWriteCount(const ZStreamW& iStreamW, uint32 iCount)
	{ iStreamW.WriteCount(iCount); }

static inline uint32 spReadCount(const ZStreamR& iStreamR)
	{ return iStreamR.ReadCount(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Client

ZTSWatcher_Client::ZTSWatcher_Client
	(bool iSupports2, bool iSupports3, bool iSupports4, ZRef<ZStreamerRW> iStreamerRW)
:	fSupports2(iSupports2),
	fSupports3(iSupports3),
	fSupports4(iSupports4),
	fStreamerRW(iStreamerRW)
	{}

ZTSWatcher_Client::~ZTSWatcher_Client()
	{
	ZMutexLocker locker(fMutex);
	if (fStreamerRW)
		{
		try
			{
			ZTuple request;
			request.SetString("What", "Close");
			request.ToStream(fStreamerRW->GetStreamW());
			}
		catch (...)
			{}
		}
	}

bool ZTSWatcher_Client::Sync
	(const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZMutexLocker locker(fMutex);
	ZRef<ZStreamerRW> theStreamerRW = fStreamerRW;
	if (!theStreamerRW)
		return false;

	const ZStreamR& theStreamR = theStreamerRW->GetStreamR();
	const ZStreamW& theStreamW = theStreamerRW->GetStreamW();
	try
		{
		if (fSupports4)
			{
			this->pSync4
				(theStreamR, theStreamW,
				iRemovedIDs, iRemovedIDsCount,
				iAddedIDs, iAddedIDsCount,
				iRemovedQueries, iRemovedQueriesCount,
				iAddedQueries, iAddedQueriesCount,
				oAddedIDs,
				oChangedTupleIDs, oChangedTuples,
				iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
				oChangedQueries);
			}
		else if (fSupports3)
			{
			oAddedIDs.clear();
			this->pSync3
				(theStreamR, theStreamW,
				iRemovedIDs, iRemovedIDsCount,
				iAddedIDs, iAddedIDsCount,
				iRemovedQueries, iRemovedQueriesCount,
				iAddedQueries, iAddedQueriesCount,
				oChangedTupleIDs, oChangedTuples,
				iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
				oChangedQueries);
			}
		else if (fSupports2)
			{
			oAddedIDs.clear();
			this->pSync2
				(theStreamR, theStreamW,
				iRemovedIDs, iRemovedIDsCount,
				iAddedIDs, iAddedIDsCount,
				iRemovedQueries, iRemovedQueriesCount,
				iAddedQueries, iAddedQueriesCount,
				oChangedTupleIDs, oChangedTuples,
				iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
				oChangedQueries);
			}
		else
			{
			this->pSync1
				(theStreamR, theStreamW,
				iRemovedIDs, iRemovedIDsCount,
				iAddedIDs, iAddedIDsCount,
				iRemovedQueries, iRemovedQueriesCount,
				iAddedQueries, iAddedQueriesCount,
				oAddedIDs,
				oChangedTupleIDs, oChangedTuples,
				iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
				oChangedQueries);
			}
		return true;
		}
	catch (...)
		{}

	return false;
	}

bool ZTSWatcher_Client::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	ZMutexLocker locker(fMutex);
	try
		{
		ZTuple request;
		request.SetString("What", "AllocateIDs");
		request.SetInt32("Count", iCount);
		request.ToStream(fStreamerRW->GetStreamW());
		fStreamerRW->GetStreamW().Flush();

		ZTuple response(fStreamerRW->GetStreamR());
		oBaseID = response.GetInt64("BaseID");
		oCountIssued = response.GetInt32("Count");
		return true;
		}
	catch (...)
		{}
	return false;
	}


void ZTSWatcher_Client::pSync1
	(const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	const ZTime start = ZTime::sSystem();

	ZTuple request;

	if (iRemovedIDsCount)
		{
		std::copy(iRemovedIDs, iRemovedIDs + iRemovedIDsCount,
			back_inserter(request.SetMutableVector("removedIDs")));
		}

	if (iAddedIDsCount)
		{
		std::copy(iAddedIDs, iAddedIDs + iAddedIDsCount,
			back_inserter(request.SetMutableVector("addedIDs")));
		}

	if (iRemovedQueriesCount)
		{
		std::copy(iRemovedQueries, iRemovedQueries + iRemovedQueriesCount,
			back_inserter(request.SetMutableVector("removedQueries")));
		}


	if (iAddedQueriesCount)
		{
		vector<ZTValue>& addedQueriesV = request.SetMutableVector("addedQueries");
		for (size_t count = iAddedQueriesCount; count; --count)
			{
			const AddedQueryCombo& theAQC = *iAddedQueries++;

			ZTuple temp;

			temp.SetInt64("refcon", theAQC.fRefcon);

			if (theAQC.fPrefetch)
				temp.SetBool("prefetch", true);

			ZTBQuery theTBQuery = theAQC.fTBQuery;

			if (!theTBQuery)
				{
				ZAssert(theAQC.fMemoryBlock);
				theTBQuery = ZTBQuery(ZStreamRPos_MemoryBlock(theAQC.fMemoryBlock));
				}
			temp.SetTuple("query", theTBQuery.AsTuple());

			addedQueriesV.push_back(temp);
			}
		}


	if (iWrittenTuplesCount)
		{
		vector<ZTValue>& writtenTuplesV = request.SetMutableVector("writtenTuples");
		for (size_t count = iWrittenTuplesCount; count; --count)
			{
			ZTuple temp;
			temp.SetID("ID", *iWrittenTupleIDs++);
			temp.SetTuple("tuple", *iWrittenTuples++);
			writtenTuplesV.push_back(temp);
			}
		}

	request.SetString("What", "DoIt");
	if (kDebug_ShowCommsTuples)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			s << "<< " << request;
		}

	const ZTime beforeSend = ZTime::sSystem();

	uint64 bytesWritten;
	if (kDebug_ShowStats)
		request.ToStream(ZStreamW_Count(bytesWritten, iStreamW));
	else
		request.ToStream(iStreamW);


	iStreamW.Flush();

	const ZTime afterSend = ZTime::sSystem();


	uint64 bytesRead;
	#if kDebug_ShowStats
		ZStreamU_Unreader theStreamR(iStreamR);
		theStreamR.ReadInt8();
		theStreamR.Unread();
		const ZTime startReceiving = ZTime::sSystem();
		ZTuple response(ZStreamR_Count(bytesRead, theStreamR));
	#else
		const ZTime startReceiving = ZTime::sSystem();
		ZTuple response(iStreamR);
	#endif

	const ZTime afterReceive = ZTime::sSystem();

	if (kDebug_ShowCommsTuples)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			s << ">> " << response;
		}

	oAddedIDs.clear();
	response.Get("addedTuples").GetSeq().GetVector_T(back_inserter(oAddedIDs), uint64());


	oChangedTupleIDs.clear();
	oChangedTuples.clear();
	const vector<ZTValue>& changedTuplesV = response.Get("changedTuples").GetSeq().GetVector();
	if (size_t theCount = changedTuplesV.size())
		{
		oChangedTupleIDs.reserve(theCount);
		oChangedTuples.reserve(theCount);
		for (vector<ZTValue>::const_iterator
			i = changedTuplesV.begin(), theEnd = changedTuplesV.end();
			i != theEnd; ++i)
			{
			const ZTuple& entry = (*i).GetTuple();

			uint64 theID;
			if (entry.QGetID("ID", theID))
				{
				ZTuple theTuple;
				if (entry.Get("tuple").QGetTuple(theTuple))
					{
					oChangedTupleIDs.push_back(theID);
					oChangedTuples.push_back(theTuple);
					}
				}
			}
		}


	oChangedQueries.clear();
	const vector<ZTValue>& changedQueriesV = response.Get("changedQueries").GetSeq().GetVector();
	for (vector<ZTValue>::const_iterator
		i = changedQueriesV.begin(), theEnd = changedQueriesV.end();
		i != theEnd; ++i)
		{
		const ZTuple& entry = (*i).GetTuple();

		int64 theRefcon;
		if (entry.QGetInt64("refcon", theRefcon))
			{
			pair<map<int64, vector<uint64> >::iterator, bool> pos =
				oChangedQueries.insert(pair<int64, vector<uint64> >(theRefcon, vector<uint64>()));
			vector<uint64>& theIDs = (*pos.first).second;
			theIDs.reserve(entry.Get("IDs").GetSeq().Count());
			entry.Get("IDs").GetSeq().GetVector_T(back_inserter(theIDs), uint64());
			}
		}

	const ZTime allDone = ZTime::sSystem();

	if (kDebug_ShowStats)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			{
			bool isEmpty =
				!iRemovedIDsCount
				&& !iAddedIDsCount
				&& !iRemovedQueriesCount
				&& !iWrittenTuplesCount
				&& !iAddedQueriesCount
				&& oAddedIDs.empty()
				&& oChangedTupleIDs.empty()
				&& oChangedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";
			s.Writef("1 %7.3fp %7.3fs %7.3fd %7.3fr %7.3fe - "
				"%3dt- %3dt+ %3dq- %3dtw %3dq+ %3dt+ %3d~t %3d~q - "
				"%6d> %6d<",
				1000*(beforeSend-start),
				1000*(afterSend-beforeSend),
				1000*(startReceiving-afterSend),
				1000*(afterReceive-startReceiving),
				1000*(allDone-afterReceive),
				iRemovedIDsCount,
				iAddedIDsCount,
				iRemovedQueriesCount,
				iWrittenTuplesCount,
				iAddedQueriesCount,
				oAddedIDs.size(),
				oChangedTupleIDs.size(),
				oChangedQueries.size(),
				size_t(bytesWritten),
				size_t(bytesRead)
				);
			}
		}
	}

void ZTSWatcher_Client::pSync2
	(const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZAssert(fSupports2);

	const ZTime beforeSend = ZTime::sSystem();

	ZTuple request;
	request.SetString("What", "NewDoIt");
	request.ToStream(iStreamW);

	uint64 bytesWritten;
	#if kDebug_ShowStats
		ZStreamW_Count theStreamW(bytesWritten, iStreamW);
	#else
		const ZStreamW& theStreamW = iStreamW;
	#endif

	spWriteCount(theStreamW, iRemovedIDsCount);
	for (size_t count = iRemovedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iRemovedIDs++);


	spWriteCount(theStreamW, iAddedIDsCount);
	for (size_t count = iAddedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iAddedIDs++);


	spWriteCount(theStreamW, iRemovedQueriesCount);
	for (size_t count = iRemovedQueriesCount; count; --count)
		theStreamW.WriteInt64(*iRemovedQueries++);


	spWriteCount(theStreamW, iAddedQueriesCount);
	for (size_t count = iAddedQueriesCount; count; --count)
		{
		const AddedQueryCombo& theAQC = *iAddedQueries++;
		theStreamW.WriteInt64(theAQC.fRefcon);
		if (theAQC.fMemoryBlock)
			{
			theStreamW.Write(theAQC.fMemoryBlock.GetData(), theAQC.fMemoryBlock.GetSize());
			}
		else
			{
			ZAssert(theAQC.fTBQuery);
			theAQC.fTBQuery.ToStream(theStreamW);
			}
		}


	spWriteCount(theStreamW, iWrittenTuplesCount);
	for (size_t count = iWrittenTuplesCount; count; --count)
		{
		theStreamW.WriteInt64(*iWrittenTupleIDs++);
		(*iWrittenTuples++).ToStream(theStreamW);
		}


	iStreamW.Flush();

	const ZTime afterSend = ZTime::sSystem();


	uint64 bytesRead;
	#if kDebug_ShowStats
		ZStreamU_Unreader theStreamU(iStreamR);
		theStreamU.ReadInt8();
		theStreamU.Unread();
		const ZTime startReceiving = ZTime::sSystem();
		ZStreamR_Count theStreamR(bytesRead, theStreamU);
	#else
		const ZTime startReceiving = ZTime::sSystem();
		const ZStreamR& theStreamR = iStreamR;
	#endif


	oChangedTupleIDs.clear();
	oChangedTuples.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		oChangedTupleIDs.reserve(theCount);
		oChangedTuples.reserve(theCount);
		while (theCount--)
			{
			oChangedTupleIDs.push_back(theStreamR.ReadUInt64());
			oChangedTuples.push_back(ZTuple(theStreamR));
			}
		}


	oChangedQueries.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		while (theCount--)
			{
			int64 theRefcon = theStreamR.ReadInt64();
			pair<map<int64, vector<uint64> >::iterator, bool> pos =
				oChangedQueries.insert(pair<int64, vector<uint64> >(theRefcon, vector<uint64>()));

			vector<uint64>& theVector = (*pos.first).second;
			if (uint32 theIDCount = spReadCount(theStreamR))
				{
				theVector.reserve(theIDCount);
				while (theIDCount--)
					theVector.push_back(theStreamR.ReadUInt64());
				}
			}
		}

	const ZTime allDone = ZTime::sSystem();

	if (kDebug_ShowStats)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			{
			bool isEmpty =
				!iRemovedIDsCount
				&& !iAddedIDsCount
				&& !iRemovedQueriesCount
				&& !iWrittenTuplesCount
				&& !iAddedQueriesCount
				&& oChangedTupleIDs.empty()
				&& oChangedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";
			s.Writef("2 %7.3fs %7.3fd %7.3fr - "
				"%3dt- %3dt+ %3dq- %3dtw %3dq+ %3d~t %3d~q - "
				"%6d> %6d<",
				1000*(afterSend-beforeSend),
				1000*(startReceiving-afterSend),
				1000*(allDone-startReceiving),
				iRemovedIDsCount,
				iAddedIDsCount,
				iRemovedQueriesCount,
				iWrittenTuplesCount,
				iAddedQueriesCount,
				oChangedTupleIDs.size(),
				oChangedQueries.size(),
				size_t(bytesWritten),
				size_t(bytesRead)
				);
			}
		}
	}

void ZTSWatcher_Client::pSync3
	(const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZAssert(fSupports3);

	const ZTime beforeSend = ZTime::sSystem();

	ZTuple request;
	request.SetString("What", "DoIt3");
	request.ToStream(iStreamW);

	uint64 bytesWritten;
	#if kDebug_ShowStats
		ZStreamW_Count theStreamW(bytesWritten, iStreamW);
	#else
		const ZStreamW& theStreamW = iStreamW;
	#endif


	spWriteCount(theStreamW, iRemovedIDsCount);
	for (size_t count = iRemovedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iRemovedIDs++);


	spWriteCount(theStreamW, iAddedIDsCount);
	for (size_t count = iAddedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iAddedIDs++);


	spWriteCount(theStreamW, iRemovedQueriesCount);
	for (size_t count = iRemovedQueriesCount; count; --count)
		theStreamW.WriteInt64(*iRemovedQueries++);


	spWriteCount(theStreamW, iAddedQueriesCount);
	for (size_t count = iAddedQueriesCount; count; --count)
		{
		const AddedQueryCombo& theAQC = *iAddedQueries++;

		theStreamW.WriteInt64(theAQC.fRefcon);
		ZMemoryBlock theMB = theAQC.fMemoryBlock;

		if (!theMB)
			{
			ZAssert(theAQC.fTBQuery);
			theAQC.fTBQuery.ToStream(ZStreamRWPos_MemoryBlock(theMB, 1024));
			}

		spWriteCount(theStreamW, theMB.GetSize());
		theStreamW.Write(theMB.GetData(), theMB.GetSize());
		}


	spWriteCount(theStreamW, iWrittenTuplesCount);
	for (size_t count = iWrittenTuplesCount; count; --count)
		{
		theStreamW.WriteInt64(*iWrittenTupleIDs++);
		(*iWrittenTuples++).ToStream(theStreamW);
		}


	iStreamW.Flush();

	const ZTime afterSend = ZTime::sSystem();


	uint64 bytesRead;
	#if kDebug_ShowStats
		ZStreamU_Unreader theStreamU(iStreamR);
		theStreamU.ReadInt8();
		theStreamU.Unread();
		const ZTime startReceiving = ZTime::sSystem();
		ZStreamR_Count theStreamR(bytesRead, theStreamU);
	#else
		const ZTime startReceiving = ZTime::sSystem();
		const ZStreamR& theStreamR = iStreamR;
	#endif


	oChangedTupleIDs.clear();
	oChangedTuples.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		oChangedTupleIDs.reserve(theCount);
		oChangedTuples.reserve(theCount);
		while (theCount--)
			{
			oChangedTupleIDs.push_back(theStreamR.ReadUInt64());
			oChangedTuples.push_back(ZTuple(theStreamR));
			}
		}


	oChangedQueries.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		while (theCount--)
			{
			int64 theRefcon = theStreamR.ReadInt64();
			pair<map<int64, vector<uint64> >::iterator, bool> pos =
				oChangedQueries.insert(pair<int64, vector<uint64> >(theRefcon, vector<uint64>()));

			vector<uint64>& theVector = (*pos.first).second;
			if (uint32 theIDCount = spReadCount(theStreamR))
				{
				theVector.reserve(theIDCount);
				while (theIDCount--)
					theVector.push_back(theStreamR.ReadUInt64());
				}
			}
		}


	const ZTime allDone = ZTime::sSystem();

	if (kDebug_ShowStats)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			{
			bool isEmpty =
				!iRemovedIDsCount
				&& !iAddedIDsCount
				&& !iRemovedQueriesCount
				&& !iWrittenTuplesCount
				&& !iAddedQueriesCount
				&& oChangedTupleIDs.empty()
				&& oChangedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";
			s.Writef("3 %7.3fs %7.3fd %7.3fr - "
				"%3dt- %3dt+ %3dq- %3dtw %3dq+ %3d~t %3d~q - "
				"%6d> %6d<",
				1000*(afterSend-beforeSend),
				1000*(startReceiving-afterSend),
				1000*(allDone-startReceiving),
				iRemovedIDsCount,
				iAddedIDsCount,
				iRemovedQueriesCount,
				iWrittenTuplesCount,
				iAddedQueriesCount,
				oChangedTupleIDs.size(),
				oChangedQueries.size(),
				size_t(bytesWritten),
				size_t(bytesRead)
				);
			}
		}
	}

void ZTSWatcher_Client::pSync4
	(const ZStreamR& iStreamR, const ZStreamW& iStreamW,
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZAssert(fSupports4);

	const ZTime beforeSend = ZTime::sSystem();

	ZTuple request;
	request.SetString("What", "DoIt4");
	request.ToStream(iStreamW);

	uint64 bytesWritten;
	#if kDebug_ShowStats
		ZStreamW_Count theStreamW(bytesWritten, iStreamW);
	#else
		const ZStreamW& theStreamW = iStreamW;
	#endif


	spWriteCount(theStreamW, iRemovedIDsCount);
	for (size_t count = iRemovedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iRemovedIDs++);


	spWriteCount(theStreamW, iAddedIDsCount);
	for (size_t count = iAddedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iAddedIDs++);


	spWriteCount(theStreamW, iRemovedQueriesCount);
	for (size_t count = iRemovedQueriesCount; count; --count)
		theStreamW.WriteInt64(*iRemovedQueries++);


	spWriteCount(theStreamW, iAddedQueriesCount);
	for (size_t count = iAddedQueriesCount; count; --count)
		{
		const AddedQueryCombo& theAQC = *iAddedQueries++;

		theStreamW.WriteInt64(theAQC.fRefcon);
		theStreamW.WriteBool(theAQC.fPrefetch);

		ZMemoryBlock theMB = theAQC.fMemoryBlock;

		if (!theMB)
			{
			ZAssert(theAQC.fTBQuery);
			theAQC.fTBQuery.ToStream(ZStreamRWPos_MemoryBlock(theMB, 1024));
			}

		spWriteCount(theStreamW, theMB.GetSize());
		theStreamW.Write(theMB.GetData(), theMB.GetSize());
		}


	spWriteCount(theStreamW, iWrittenTuplesCount);
	for (size_t count = iWrittenTuplesCount; count; --count)
		{
		theStreamW.WriteInt64(*iWrittenTupleIDs++);
		(*iWrittenTuples++).ToStream(theStreamW);
		}


	iStreamW.Flush();

	const ZTime afterSend = ZTime::sSystem();


	uint64 bytesRead;
	#if kDebug_ShowStats
		ZStreamU_Unreader theStreamU(iStreamR);
		theStreamU.ReadInt8();
		theStreamU.Unread();
		const ZTime startReceiving = ZTime::sSystem();
		ZStreamR_Count theStreamR(bytesRead, theStreamU);
	#else
		const ZTime startReceiving = ZTime::sSystem();
		const ZStreamR& theStreamR = iStreamR;
	#endif


	oAddedIDs.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		oAddedIDs.reserve(theCount);
		while (theCount--)
			oAddedIDs.push_back(theStreamR.ReadUInt64());
		}

	oChangedTupleIDs.clear();
	oChangedTuples.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		oChangedTupleIDs.reserve(theCount);
		oChangedTuples.reserve(theCount);
		while (theCount--)
			{
			oChangedTupleIDs.push_back(theStreamR.ReadUInt64());
			oChangedTuples.push_back(ZTuple(theStreamR));
			}
		}


	oChangedQueries.clear();
	if (uint32 theCount = spReadCount(theStreamR))
		{
		while (theCount--)
			{
			int64 theRefcon = theStreamR.ReadInt64();
			pair<map<int64, vector<uint64> >::iterator, bool> pos =
				oChangedQueries.insert(pair<int64, vector<uint64> >(theRefcon, vector<uint64>()));

			vector<uint64>& theVector = (*pos.first).second;
			if (uint32 theIDCount = spReadCount(theStreamR))
				{
				theVector.reserve(theIDCount);
				while (theIDCount--)
					theVector.push_back(theStreamR.ReadUInt64());
				}
			}
		}

	const ZTime allDone = ZTime::sSystem();

	if (kDebug_ShowStats)
		{
		if (ZLOG(s, eInfo, "ZTSWatcher_Client"))
			{
			bool isEmpty =
				!iRemovedIDsCount
				&& !iAddedIDsCount
				&& !iRemovedQueriesCount
				&& !iWrittenTuplesCount
				&& !iAddedQueriesCount
				&& oAddedIDs.empty()
				&& oChangedTupleIDs.empty()
				&& oChangedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";
			s.Writef("4 %7.3fs %7.3fd %7.3fr - "
				"%3dt- %3dt+ %3dq- %3dtw %3dq+ %3dt+ %3d~t %3d~q - "
				"%6d> %6d<",
				1000*(afterSend-beforeSend),
				1000*(startReceiving-afterSend),
				1000*(allDone-startReceiving),
				iRemovedIDsCount,
				iAddedIDsCount,
				iRemovedQueriesCount,
				iWrittenTuplesCount,
				iAddedQueriesCount,
				oAddedIDs.size(),
				oChangedTupleIDs.size(),
				oChangedQueries.size(),
				size_t(bytesWritten),
				size_t(bytesRead)
				);
			}
		}
	}

void ZTSWatcher_Client::SetCallback(Callback_t iCallback, void* iRefcon)
	{
	// With the synchronous protocol there is no way for the
	// server to trigger a callback.
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_Client

ZTSWatcherFactory_Client::ZTSWatcherFactory_Client
	(ZRef<ZStreamerRWFactory> iStreamerRWFactory)
:	fStreamerRWFactory(iStreamerRWFactory)
	{}

ZTSWatcherFactory_Client::~ZTSWatcherFactory_Client()
	{}

ZRef<ZTSWatcher> ZTSWatcherFactory_Client::MakeTSWatcher()
	{
	if (fStreamerRWFactory)
		{
		if (ZRef<ZStreamerRW> theSRW = fStreamerRWFactory->MakeStreamerRW())
			return new ZTSWatcher_Client(true, true, true, theSRW);
		}

	return null;
	}

} // namespace ZooLib

