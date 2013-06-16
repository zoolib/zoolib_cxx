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

#include "zoolib/tuplebase/ZTSWatcherServer.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZStream_Count.h"
#include "zoolib/ZStream_Tee.h"

namespace ZooLib {

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

#define kDebug_TSWatcherServer 1

#ifdef ZCONFIG_TSWatcherServer_ShowCommsTuples
#	define kDebug_ShowCommsTuples ZCONFIG_TSWatcherServer_ShowCommsTuples
#else
#	define kDebug_ShowCommsTuples 0
#endif

#ifdef ZCONFIG_TSWatcherServer_ShowTimes
#	define kDebug_ShowTimes ZCONFIG_TSWatcherServer_ShowTimes
#else
#	define kDebug_ShowTimes 1
#endif

// =================================================================================================
#pragma mark -
#pragma mark * Static helpers

static inline void spWriteCount(const ZStreamW& iStreamW, uint32 iCount)
	{ iStreamW.WriteCount(iCount); }

static inline uint32 spReadCount(const ZStreamR& iStreamR)
	{ return iStreamR.ReadCount(); }

static void spDumpRequest(ZRef<ZTSWatcher> iWatcher, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, "ZTSWatcherServer"))
		{
		s.Writef(">> ZTSWatcherServer: %08X ", iWatcher.GetObject());
		s << iTuple;
		}
#endif
	}

static void spSendResponse(ZRef<ZTSWatcher> iWatcher, const ZStreamW& iStreamW, const ZTuple& iTuple)
	{
#if kDebug_ShowCommsTuples
	if (ZLOG(s, eDebug, "ZTSWatcherServer"))
		{
		s.Writef("<< ZTSWatcherServer: %08X ", iWatcher.GetObject());
		s << iTuple;
		}
#endif
	iTuple.ToStream(iStreamW);
	}

static bool spAllocateIDs(ZRef<ZTSWatcher> iWatcher, const ZTuple& iReq, const ZStreamW& iStreamW)
	{
	size_t theCount = iReq.GetInt32("Count");
	uint64 baseID;
	size_t countIssued;
	if (not iWatcher->AllocateIDs(theCount, baseID, countIssued))
		return false;
	ZTuple response;
	response.SetInt64("BaseID", baseID);
	response.SetInt32("Count", countIssued);
	spSendResponse(iWatcher, iStreamW, response);
	iStreamW.Flush();
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync common code

// ZTSWatcher now requires that the tuples passed to it are sorted by ID, so it
// can efficiently identify overlaps between the set written by a client, and
// the set that has been written by others. Previously we'd passed a set<uint64>,
// for which that operation is obviously already efficient.

static void spSort(vector<uint64>& ioWrittenTupleIDs, vector<ZTuple>& ioWrittenTuples)
	{
	vector<uint64> newIDs;
	vector<ZTuple> newTuples;
	// Do a lower_bound insertion into newIDs, and
	// to equivalent offset of newTuples.
	for (vector<uint64>::iterator i = ioWrittenTupleIDs.begin(); i != ioWrittenTupleIDs.end(); ++i)
		{
		vector<uint64>::iterator pos = lower_bound(newIDs.begin(), newIDs.end(), *i);

		newTuples.insert(newTuples.begin() + (pos - newIDs.begin()),
			ioWrittenTuples[i - ioWrittenTupleIDs.begin()]);

		newIDs.insert(pos, *i);
		}
	ioWrittenTupleIDs.swap(newIDs);
	ioWrittenTuples.swap(newTuples);
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync1

// Unpacks a request in a ZTuple, and returns the results in a tuple.

static bool spSync1(
	double iReadTime, ZRef<ZTSWatcher> iWatcher, const ZTuple& iReq, const ZStreamW& iStreamW)
	{
	ZTime start = ZTime::sSystem();


	vector<uint64> removedIDs;
	foreachi (i, iReq.Get<ZSeq_Any>("removedIDs"))
		removedIDs.push_back(i->Get<uint64>());


	vector<uint64> addedIDs;
	foreachi (i, iReq.Get<ZSeq_Any>("addedIDs"))
		addedIDs.push_back(i->Get<uint64>());


	vector<int64> removedQueries;
	foreachi (i, iReq.Get<ZSeq_Any>("removedQueries"))
		removedQueries.push_back(i->Get<uint64>());


	vector<ZTSWatcher::AddedQueryCombo> addedQueries;

	foreachi (i, iReq.Get<ZSeq_Any>("addedQueries"))
		{
		const ZTuple& entry = (*i).Get<ZMap_Any>();
		int64 theRefcon;
		if (entry.QGetInt64("refcon", theRefcon))
			{
			if (ZQ<ZMap_Any> theTupleQ = entry.Get<ZMap_Any>("query"))
				{
				ZTSWatcher::AddedQueryCombo theCombo;
				theCombo.fRefcon = theRefcon;
				theCombo.fTBQuery = ZTBQuery(*theTupleQ);
				theCombo.fTBQuery.ToStream(ZStreamRWPos_MemoryBlock(theCombo.fMemoryBlock));
				theCombo.fPrefetch = entry.GetBool("prefetch");
				addedQueries.push_back(theCombo);
				}
			}
		}


	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;
	bool writeNeededSort = false;
	{
	uint64 lastID = 0;
	foreachi (i, iReq.Get<ZSeq_Any>("writtenTuples"))
		{
		const ZTuple& entry = (*i).Get<ZMap_Any>();
		uint64 theID;
		if (entry.QGetID("ID", theID))
			{
			if (ZQ<ZMap_Any> theTupleQ = entry.Get<ZMap_Any>("tuple"))
				{
				if (lastID >= theID)
					writeNeededSort = true;
				lastID = theID;

				writtenTupleIDs.push_back(theID);
				writtenTuples.push_back(*theTupleQ);
				}
			}
		}
	if (writeNeededSort)
		spSort(writtenTupleIDs, writtenTuples);
	}


	vector<uint64> watcherAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	ZTime beforeSync = ZTime::sSystem();

	if (not iWatcher->Sync(
		&removedIDs[0], removedIDs.size(),
		&addedIDs[0], addedIDs.size(),
		&removedQueries[0], removedQueries.size(),
		&addedQueries[0], addedQueries.size(),
		watcherAddedIDs,
		changedTupleIDs, changedTuples,
		&writtenTupleIDs[0], &writtenTuples[0], writtenTupleIDs.size(),
		changedQueries))
		{
		return false;
		}

	ZTime afterSync = ZTime::sSystem();

	ZTuple response;

	if (!watcherAddedIDs.empty())
		{
		foreacha (aa, watcherAddedIDs)
			response.Mut<ZSeq_Any>("addedTuples").Append(aa);
		}

	if (size_t theCount = changedTupleIDs.size())
		{
		ZAssertStop(kDebug_TSWatcherServer, changedTupleIDs.size() == changedTuples.size());
		ZSeq_Any& changedTuplesV = response.Mut<ZSeq_Any>("changedTuples");
		ZTuple temp;
		vector<ZTuple>::const_iterator iterCT = changedTuples.begin();
		for (vector<uint64>::const_iterator
			i = changedTupleIDs.begin(), theEnd = changedTupleIDs.end();
			i != theEnd; ++i, ++iterCT)
			{
			temp.SetID("ID", *i);
			temp.SetTuple("tuple", *iterCT);
			changedTuplesV.Append(temp);
			}
		}

	ZTime afterChangedTuples = ZTime::sSystem();

	if (size_t theCount = changedQueries.size())
		{
		ZSeq_Any& changedQueriesV = response.Mut<ZSeq_Any>("changedQueries");
		ZTuple temp;
		for (map<int64, vector<uint64> >::iterator
			i = changedQueries.begin(), theEnd = changedQueries.end();
			i != theEnd; ++i)
			{
			temp.SetInt64("refcon", i->first);

			foreacha (aa, i->second)
				temp.Mut<ZSeq_Any>("IDs").Append(aa);

			changedQueriesV.Append(temp);
			}
		}

	ZTime beforeSend = ZTime::sSystem();

	uint64 bytesWritten;
	if (kDebug_ShowTimes)
		spSendResponse(iWatcher, ZStreamW_Count(bytesWritten, iStreamW), response);
	else
		spSendResponse(iWatcher, iStreamW, response);

	iStreamW.Flush();

	ZTime afterSend = ZTime::sSystem();

	if (kDebug_ShowTimes)
		{
		if (ZLOG(s, eInfo, "ZTSWatcherServer"))
			{
			bool isEmpty =
				removedIDs.empty()
				&& addedIDs.empty()
				&& removedQueries.empty()
				&& writtenTuples.empty()
				&& addedQueries.empty()
				&& watcherAddedIDs.empty()
				&& changedTuples.empty()
				&& changedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";

#if 0 //##
			const char sortChar = writeNeededSort ? '!' : 'w';

			s.Writef("1 %7.3fr %7.3fe %7.3fd %7.3fpt %7.3fpq %7.3fs - "
				"%3dt- %3dt+ %3dq- %3dt%c %3dq+ %3dt+ %3d~t %3d~q - "
				"%6d",
				1000*iReadTime,
				1000*(beforeSync-start),
				1000*(afterSync-beforeSync),
				1000*(afterChangedTuples-afterSync),
				1000*(beforeSend-afterChangedTuples),
				1000*(afterSend-beforeSend),
				removedIDs.size(),
				addedIDs.size(),
				removedQueries.size(),
				writtenTuples.size(),
				sortChar,
				addedQueries.size(),
				watcherAddedIDs.size(),
				changedTuples.size(),
				changedQueries.size(),
				size_t(bytesWritten)
				);
#endif
			}
		}
	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync2, spSync3 and spSync4 common code

static void spSync_Initial(const ZStreamR& iStreamR,
	vector<uint64>& oRemovedIDs,
	vector<uint64>& oAddedIDs,
	vector<int64>& oRemovedQueries)
	{
	if (uint32 theCount = spReadCount(iStreamR))
		{
		oRemovedIDs.reserve(theCount);
		while (theCount--)
			oRemovedIDs.push_back(iStreamR.ReadUInt64());
		}


	if (uint32 theCount = spReadCount(iStreamR))
		{
		oAddedIDs.reserve(theCount);
		while (theCount--)
			oAddedIDs.push_back(iStreamR.ReadUInt64());
		}


	if (uint32 theCount = spReadCount(iStreamR))
		{
		oRemovedQueries.reserve(theCount);
		while (theCount--)
			oRemovedQueries.push_back(iStreamR.ReadInt64());
		}
	}


static bool spSync_NextBit(const ZStreamR& iStreamR,
	vector<uint64>& oWrittenTupleIDs,
	vector<ZTuple>& oWrittenTuples)
	{
	bool writeNeededSort = false;
	if (uint32 theCount = spReadCount(iStreamR))
		{
		oWrittenTupleIDs.reserve(theCount);
		oWrittenTuples.reserve(theCount);
		uint64 lastID = 0;
		while (theCount--)
			{
			uint64 currentID = iStreamR.ReadUInt64();
			if (lastID >= currentID)
				writeNeededSort = true;
			lastID = currentID;

			oWrittenTupleIDs.push_back(currentID);
			oWrittenTuples.push_back(ZTuple(iStreamR));
			}

		if (writeNeededSort)
			spSort(oWrittenTupleIDs, oWrittenTuples);
		}
	return writeNeededSort;
	}

static void spSync_LastBit(const ZStreamW& iStreamW,
	const vector<uint64>& iChangedTupleIDs, const vector<ZTuple>& iChangedTuples,
	const map<int64, vector<uint64> >& iChangedQueries)
	{
	ZAssertStop(kDebug_TSWatcherServer, iChangedTupleIDs.size() == iChangedTuples.size());

	spWriteCount(iStreamW, iChangedTupleIDs.size());

	vector<ZTuple>::const_iterator iterCT = iChangedTuples.begin();
	for (vector<uint64>::const_iterator
		i = iChangedTupleIDs.begin(), theEnd = iChangedTupleIDs.end();
		i != theEnd; ++i, ++iterCT)
		{
		iStreamW.WriteUInt64(*i);
		(*iterCT).ToStream(iStreamW);
		}

	spWriteCount(iStreamW, iChangedQueries.size());
	for (map<int64, vector<uint64> >::const_iterator
		i = iChangedQueries.begin(), theEnd = iChangedQueries.end();
		i != theEnd; ++i)
		{
		iStreamW.WriteInt64((*i).first);
		const vector<uint64>& theVector = (*i).second;
		spWriteCount(iStreamW, theVector.size());
		for (vector<uint64>::const_iterator j = theVector.begin(); j != theVector.end(); ++j)
			iStreamW.WriteUInt64(*j);
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync2

static bool spSync2(
	ZRef<ZTSWatcher> iWatcher, const ZStreamR& iStreamR_Real, const ZStreamW& iStreamW_Real)
	{
	ZTime start = ZTime::sSystem();

	uint64 bytesRead;
	#if kDebug_ShowTimes
		ZStreamR_Count iStreamR(bytesRead, iStreamR_Real);
	#else
		const ZStreamR& iStreamR = iStreamR_Real;
	#endif

	vector<uint64> removedIDs;
	vector<uint64> addedIDs;
	vector<int64> removedQueries;
	spSync_Initial(iStreamR, removedIDs, addedIDs, removedQueries);


	vector<ZTSWatcher::AddedQueryCombo> addedQueries;
	if (uint32 theCount = spReadCount(iStreamR))
		{
		addedQueries.reserve(theCount);
		while (theCount--)
			{
			ZTSWatcher::AddedQueryCombo theCombo;
			theCombo.fRefcon = iStreamR.ReadInt64();

			// This next line instantiates a ZTBQuery from iStreamR and assigns it to fTBQuery.
			// It pulls it from a ZStreamR_Tee that's reading from iStreamR, and writing to
			// a ZStreamRWPos_MemoryBlock whose output is fMemoryBlock.
			theCombo.fTBQuery = ZTBQuery(ZStreamR_Tee(
				iStreamR, ZStreamRWPos_MemoryBlock(theCombo.fMemoryBlock)));

			theCombo.fPrefetch = false;
			addedQueries.push_back(theCombo);
			}
		}


	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;
	spSync_NextBit(iStreamR, writtenTupleIDs, writtenTuples);


	vector<uint64> watcherAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	ZTime beforeSync = ZTime::sSystem();

	if (not iWatcher->Sync(
		&removedIDs[0], removedIDs.size(),
		&addedIDs[0], addedIDs.size(),
		&removedQueries[0], removedQueries.size(),
		&addedQueries[0], addedQueries.size(),
		watcherAddedIDs,
		changedTupleIDs, changedTuples,
		&writtenTupleIDs[0], &writtenTuples[0], writtenTupleIDs.size(),
		changedQueries))
		{
		return false;
		}

	ZTime afterSync = ZTime::sSystem();

	// We can't report server added IDs, so we don't mark any
	// queries as prefetched, so this must always be empty.
	ZAssertStop(kDebug_TSWatcherServer, watcherAddedIDs.empty());

	uint64 bytesWritten;
	#if kDebug_ShowTimes
		ZStreamW_Count iStreamW(bytesWritten, iStreamW_Real);
	#else
		const ZStreamW& iStreamW = iStreamW_Real;
	#endif

	spSync_LastBit(iStreamW, changedTupleIDs, changedTuples, changedQueries);

	iStreamW.Flush();

	ZTime afterSend = ZTime::sSystem();

	if (kDebug_ShowTimes)
		{
		if (ZLOG(s, eInfo, "ZTSWatcherServer"))
			{
			bool isEmpty =
				removedIDs.empty()
				&& addedIDs.empty()
				&& removedQueries.empty()
				&& writtenTuples.empty()
				&& addedQueries.empty()
				&& changedTuples.empty()
				&& changedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";

#if 0 //##
			s.Writef("2 %7.3fr %7.3fd %7.3fs - "
				"%3dt- %3dt+ %3dq- %3dtw %3dq+ %3d~t %3d~q - "
				"%6d< %6d>",
				1000*(beforeSync-start),
				1000*(afterSync-beforeSync),
				1000*(afterSend-afterSync),
				removedIDs.size(),
				addedIDs.size(),
				removedQueries.size(),
				writtenTuples.size(),
				addedQueries.size(),
				changedTuples.size(),
				changedQueries.size(),
				size_t(bytesRead),
				size_t(bytesWritten)
				);
#endif
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync3

static bool spSync3(
	ZRef<ZTSWatcher> iWatcher, const ZStreamR& iStreamR_Real, const ZStreamW& iStreamW_Real)
	{
	ZTime start = ZTime::sSystem();

	uint64 bytesRead;
	#if kDebug_ShowTimes
		ZStreamR_Count iStreamR(bytesRead, iStreamR_Real);
	#else
		const ZStreamR& iStreamR = iStreamR_Real;
	#endif

	vector<uint64> removedIDs;
	vector<uint64> addedIDs;
	vector<int64> removedQueries;
	spSync_Initial(iStreamR, removedIDs, addedIDs, removedQueries);

	// This is where we differ from spSync2 -- the caller will have prefixed the
	// each serialiazed ZTBQuery with a count of how many bytes follow. So we
	// do not have to interpret it to read it, so we may be able to avoid
	// interpreting it at all, if iWatcher currently has this query in use --
	// it will use the bytes in the ZMemoryBlock as the key to find the query.

	vector<ZTSWatcher::AddedQueryCombo> addedQueries;
	if (uint32 theCount = spReadCount(iStreamR))
		{
		addedQueries.reserve(theCount);
		while (theCount--)
			{
			int64 theRefcon = iStreamR.ReadInt64();
			size_t theSize = spReadCount(iStreamR);

			ZTSWatcher::AddedQueryCombo theCombo(theSize);
			theCombo.fRefcon = theRefcon;
			theCombo.fPrefetch = false;

			iStreamR.Read(theCombo.fMemoryBlock.GetPtrMutable(), theSize);

			addedQueries.push_back(theCombo);
			}
		}


	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;
	bool writeNeededSort = spSync_NextBit(iStreamR, writtenTupleIDs, writtenTuples);


	vector<uint64> watcherAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	ZTime beforeSync = ZTime::sSystem();

	if (not iWatcher->Sync(
		&removedIDs[0], removedIDs.size(),
		&addedIDs[0], addedIDs.size(),
		&removedQueries[0], removedQueries.size(),
		&addedQueries[0], addedQueries.size(),
		watcherAddedIDs,
		changedTupleIDs, changedTuples,
		&writtenTupleIDs[0], &writtenTuples[0], writtenTupleIDs.size(),
		changedQueries))
		{
		return false;
		}

	ZTime afterSync = ZTime::sSystem();

	// We can't report server added IDs, so we don't mark any
	// queries as prefetched, so this must always be empty.
	ZAssertStop(kDebug_TSWatcherServer, watcherAddedIDs.empty());

	uint64 bytesWritten;
	#if kDebug_ShowTimes
		ZStreamW_Count iStreamW(bytesWritten, iStreamW_Real);
	#else
		const ZStreamW& iStreamW = iStreamW_Real;
	#endif

	spSync_LastBit(iStreamW, changedTupleIDs, changedTuples, changedQueries);

	iStreamW.Flush();

	ZTime afterSend = ZTime::sSystem();

	if (kDebug_ShowTimes)
		{
		if (ZLOG(s, eInfo, "ZTSWatcherServer"))
			{
			bool isEmpty =
				removedIDs.empty()
				&& addedIDs.empty()
				&& removedQueries.empty()
				&& writtenTuples.empty()
				&& addedQueries.empty()
				&& changedTuples.empty()
				&& changedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";

			const char sortChar = writeNeededSort ? '!' : 'w';

#if 0 //##
			s.Writef("3 %7.3fr %7.3fd %7.3fs - "
				"%3dt- %3dt+ %3dq- %3dt%c %3dq+ %3d~t %3d~q - "
				"%6d< %6d>",
				1000*(beforeSync-start),
				1000*(afterSync-beforeSync),
				1000*(afterSend-afterSync),
				removedIDs.size(),
				addedIDs.size(),
				removedQueries.size(),
				writtenTuples.size(),
				sortChar,
				addedQueries.size(),
				changedTuples.size(),
				changedQueries.size(),
				size_t(bytesRead),
				size_t(bytesWritten)
				);
#endif
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * spSync4

static bool spSync4(
	ZRef<ZTSWatcher> iWatcher, const ZStreamR& iStreamR_Real, const ZStreamW& iStreamW_Real)
	{
	ZTime start = ZTime::sSystem();

	uint64 bytesRead;
	#if kDebug_ShowTimes
		ZStreamR_Count iStreamR(bytesRead, iStreamR_Real);
	#else
		const ZStreamR& iStreamR = iStreamR_Real;
	#endif

	vector<uint64> removedIDs;
	vector<uint64> addedIDs;
	vector<int64> removedQueries;
	spSync_Initial(iStreamR, removedIDs, addedIDs, removedQueries);

	// This is where our request parsinsg differs from spSync3.
	// The caller can mark a query as being prefetched (see line marked <--).

	vector<ZTSWatcher::AddedQueryCombo> addedQueries;
	if (uint32 theCount = spReadCount(iStreamR))
		{
		addedQueries.reserve(theCount);
		while (theCount--)
			{
			int64 theRefcon = iStreamR.ReadInt64();
			bool thePrefetch = iStreamR.ReadBool(); // <--
			size_t theSize = spReadCount(iStreamR);

			ZTSWatcher::AddedQueryCombo theCombo(theSize);
			theCombo.fRefcon = theRefcon;
			theCombo.fPrefetch = thePrefetch;

			iStreamR.Read(theCombo.fMemoryBlock.GetPtrMutable(), theSize);

			addedQueries.push_back(theCombo);
			}
		}


	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;
	bool writeNeededSort = spSync_NextBit(iStreamR, writtenTupleIDs, writtenTuples);


	vector<uint64> watcherAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	ZTime beforeSync = ZTime::sSystem();

	if (not iWatcher->Sync(
		&removedIDs[0], removedIDs.size(),
		&addedIDs[0], addedIDs.size(),
		&removedQueries[0], removedQueries.size(),
		&addedQueries[0], addedQueries.size(),
		watcherAddedIDs,
		changedTupleIDs, changedTuples,
		&writtenTupleIDs[0], &writtenTuples[0], writtenTupleIDs.size(),
		changedQueries))
		{
		return false;
		}

	ZTime afterSync = ZTime::sSystem();

	uint64 bytesWritten;
	#if kDebug_ShowTimes
		ZStreamW_Count iStreamW(bytesWritten, iStreamW_Real);
	#else
		const ZStreamW& iStreamW = iStreamW_Real;
	#endif

	// And this is the other change, where we send the list of
	// server-added tuple IDs. IDs that have shown up in a query
	// marked as prefetch, which the client has not registered with
	// us, and which we now consider to be registered. Their
	// actual content will be included with changedTupleIDs and changedTuples.

	spWriteCount(iStreamW, watcherAddedIDs.size());
	for (vector<uint64>::const_iterator
		i = watcherAddedIDs.begin(), theEnd = watcherAddedIDs.end();
		i != theEnd; ++i)
		{
		iStreamW.WriteUInt64(*i);
		}

	// From here on we're the same as spSync3.
	spSync_LastBit(iStreamW, changedTupleIDs, changedTuples, changedQueries);

	iStreamW.Flush();

	ZTime afterSend = ZTime::sSystem();

	if (kDebug_ShowTimes)
		{
		if (ZLOG(s, eInfo, "ZTSWatcherServer"))
			{
			bool isEmpty =
				removedIDs.empty()
				&& addedIDs.empty()
				&& removedQueries.empty()
				&& writtenTuples.empty()
				&& addedQueries.empty()
				&& watcherAddedIDs.empty()
				&& changedTuples.empty()
				&& changedQueries.empty();

			if (isEmpty)
				s << "!";
			else
				s << " ";

			const char sortChar = writeNeededSort ? '!' : 'w';

#if 0 //##
			s.Writef("4 %7.3fr %7.3fd %7.3fs - "
				"%3dt- %3dt+ %3dq- %3dt%c %3dq+ %3dt+ %3d~t %3d~q - "
				"%6d< %6d>",
				1000*(beforeSync-start),
				1000*(afterSync-beforeSync),
				1000*(afterSend-afterSync),
				removedIDs.size(),
				addedIDs.size(),
				removedQueries.size(),
				writtenTuples.size(),
				sortChar,
				addedQueries.size(),
				watcherAddedIDs.size(),
				changedTuples.size(),
				changedQueries.size(),
				size_t(bytesRead),
				size_t(bytesWritten)
				);
#endif
			}
		}

	return true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherServer

ZTSWatcherServer::ZTSWatcherServer(ZRef<ZTSWatcher> iWatcher)
:	fWatcher(iWatcher)
	{
	fWatcher->SetCallback(spCallback, this);
	}

void ZTSWatcherServer::Run(const ZStreamR& iStreamR, const ZStreamW& iStreamW)
	{
	double readTime = 0;
	for (;;)
		{
		#if kDebug_ShowTimes
			ZStreamU_Unreader theStreamU(iStreamR);
			theStreamU.ReadInt8();
			theStreamU.Unread();
			ZTime before = ZTime::sSystem();
			ZTuple theReq(theStreamU);
			readTime = ZTime::sSystem() - before;
		#else
			ZTuple theReq(iStreamR);
		#endif

		spDumpRequest(fWatcher, theReq);

		string theWhat;
		if (theReq.QGetString("What", theWhat))
			{
			if ("AllocateIDs" == theWhat)
				{
				if (!spAllocateIDs(fWatcher, theReq, iStreamW))
					break;
				}
			else if ("DoIt" == theWhat)
				{
				if (!spSync1(readTime, fWatcher, theReq, iStreamW))
					break;
				}
			else if ("NewDoIt" == theWhat || "DoIt2" == theWhat)
				{
				if (!spSync2(fWatcher, iStreamR, iStreamW))
					break;
				}
			else if ("DoIt3" == theWhat)
				{
				if (!spSync3(fWatcher, iStreamR, iStreamW))
					break;
				}
			else if ("DoIt4" == theWhat)
				{
				if (!spSync4(fWatcher, iStreamR, iStreamW))
					break;
				}
			else if ("Close" == theWhat)
				{
				break;
				}
			else
				{
				if (ZLOG(s, eErr, "ZTSWatcherServer"))
					s << "Unrecognized request: " << theWhat;
				break;
				}
			}
		}
	}

void ZTSWatcherServer::pCallback()
	{}

void ZTSWatcherServer::spCallback(void* iRefcon)
	{ static_cast<ZTSWatcherServer*>(iRefcon)->pCallback(); }

} // namespace ZooLib

