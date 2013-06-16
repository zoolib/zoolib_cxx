/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/tuplebase/ZTSWatcherServerAsync.h"

#include "zoolib/tuplebase/ZTSWatcherDefines.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::map;
using std::pair;
using std::vector;

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherServerAsync

ZTSWatcherServerAsync::ZTSWatcherServerAsync(
	ZRef<ZTaskMaster> iTaskMaster,
	ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW,
	ZRef<ZTSWatcher> iTSWatcher)
:	ZTask(iTaskMaster),
	ZCommer(iStreamerR, iStreamerW),
	fTSWatcher(iTSWatcher),
	fSendClose(false),
	fCallbackNeeded(false),
	fSyncNeeded(false),
	fIDsNeeded(0)
	{
	fTSWatcher->SetCallback(spCallback, this);
	}

ZTSWatcherServerAsync::~ZTSWatcherServerAsync()
	{
	}

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

void ZTSWatcherServerAsync::Kill()
	{
// ??
	ZMutexLocker locker(fMutex);
	fSendClose = true;
	locker.Release();
	ZStreamerWriter::Wake();
	}

bool ZTSWatcherServerAsync::Read(const ZStreamR& iStreamR)
	{
	if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
		s << "Read, start";

	EReq theReq = (EReq)iStreamR.ReadUInt8();

	switch (theReq)
		{
		case eReq_Close:
			{
			ZMutexLocker locker(fMutex);
			fSendClose = true;
			locker.Release();
			ZStreamerWriter::Wake();
			return false;
			}
		case eReq_IDs:
			{
			if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
				s << "Read, eReq_IDs";
			const size_t theIDsNeeded = iStreamR.ReadCount();
			ZMutexLocker locker(fMutex);
			fIDsNeeded += theIDsNeeded;
			locker.Release();
			ZStreamerWriter::Wake();
			break;
			}
		case eReq_Sync:
			{
			if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
				s << "Read, eReq_Sync";
			vector<uint64> removedIDs;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				removedIDs.reserve(theCount);
				while (theCount--)
					removedIDs.push_back(iStreamR.ReadUInt64());
				}

			vector<uint64> addedIDs;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				addedIDs.reserve(theCount);
				while (theCount--)
					addedIDs.push_back(iStreamR.ReadUInt64());
				}

			vector<int64> removedQueries;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				removedQueries.reserve(theCount);
				while (theCount--)
					removedQueries.push_back(iStreamR.ReadInt64());
				}

			vector<ZTSWatcher::AddedQueryCombo> addedQueries;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				addedQueries.reserve(theCount);
				while (theCount--)
					{
					const int64 theRefcon = iStreamR.ReadInt64();
					const bool thePrefetch = iStreamR.ReadBool();
					const size_t theSize = iStreamR.ReadCount();

					ZTSWatcher::AddedQueryCombo theCombo(theSize);
					theCombo.fRefcon = theRefcon;
					theCombo.fPrefetch = thePrefetch;

					iStreamR.Read(theCombo.fMemoryBlock.GetPtrMutable(), theSize);

					addedQueries.push_back(theCombo);
					}
				}

			vector<uint64> writtenTupleIDs;
			vector<ZTuple> writtenTuples;
			bool writeNeededSort = false;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				writtenTupleIDs.reserve(theCount);
				writtenTuples.reserve(theCount);
				uint64 priorID = 0;
				while (theCount--)
					{
					const uint64 currentID = iStreamR.ReadUInt64();
					if (priorID >= currentID)
						writeNeededSort = true;
					priorID = currentID;

					writtenTupleIDs.push_back(currentID);
					writtenTuples.push_back(ZTuple(iStreamR));
					}

				if (writeNeededSort)
					spSort(writtenTupleIDs, writtenTuples);
				}

			ZMutexLocker locker(fMutex);

			ZAssert(fRemovedIDs.empty());
			ZAssert(fAddedIDs.empty());
			ZAssert(fRemovedQueries.empty());
			ZAssert(fAddedQueries.empty());
			ZAssert(fWrittenTupleIDs.empty());
			ZAssert(fWrittenTuples.empty());
			ZAssert(!fSyncNeeded);

			fRemovedIDs.swap(removedIDs);
			fAddedIDs.swap(addedIDs);
			fRemovedQueries.swap(removedQueries);
			fAddedQueries.swap(addedQueries);
			fWrittenTupleIDs.swap(writtenTupleIDs);
			fWrittenTuples.swap(writtenTuples);

			fSyncNeeded = true;

			locker.Release();
			ZStreamerWriter::Wake();
			break;
			}
		}
	return true;
	}

bool ZTSWatcherServerAsync::Write(const ZStreamW& iStreamW)
	{
	bool wroteAnything = false;

	ZMutexLocker locker(fMutex);
	if (fSendClose)
		{
		iStreamW.WriteUInt8(eResp_Close);
		return false;
		}
	else if (fIDsNeeded)
		{
		if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
			s << "Write, sending IDs";
		const size_t countNeeded = fIDsNeeded;
		fIDsNeeded = 0;
		locker.Release();

		uint64 theBaseID;
		size_t countIssued;

		bool success = fTSWatcher->AllocateIDs(countNeeded, theBaseID, countIssued);

		if (!success)
			{
			iStreamW.WriteUInt8(eResp_Close);
			return false;
			}

		iStreamW.WriteUInt8(eResp_IDs);
		iStreamW.WriteUInt64(theBaseID);
		iStreamW.WriteCount(countIssued);

		wroteAnything = true;

		if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
			s << "Write, sent IDs";
		}
	else if (fSyncNeeded)
		{
		if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
			s << "Write, sync";
		fSyncNeeded = false;

		vector<uint64> removedIDs;
		vector<uint64> addedIDs;
		vector<int64> removedQueries;
		vector<ZTSWatcher::AddedQueryCombo> addedQueries;
		vector<uint64> writtenTupleIDs;
		vector<ZTuple> writtenTuples;

		fRemovedIDs.swap(removedIDs);
		fAddedIDs.swap(addedIDs);
		fRemovedQueries.swap(removedQueries);
		fAddedQueries.swap(addedQueries);
		fWrittenTupleIDs.swap(writtenTupleIDs);
		fWrittenTuples.swap(writtenTuples);

		locker.Release();

		vector<uint64> watcherAddedIDs;
		vector<uint64> changedTupleIDs;
		vector<ZTuple> changedTuples;
		map<int64, vector<uint64> > changedQueries;

		using ZUtil_STL::sFirstOrNil;
		bool success = fTSWatcher->Sync(
			sFirstOrNil(removedIDs), removedIDs.size(),
			sFirstOrNil(addedIDs), addedIDs.size(),
			sFirstOrNil(removedQueries), removedQueries.size(),
			sFirstOrNil(addedQueries), addedQueries.size(),
			watcherAddedIDs,
			changedTupleIDs, changedTuples,
			sFirstOrNil(writtenTupleIDs), sFirstOrNil(writtenTuples), writtenTupleIDs.size(),
			changedQueries);

		if (!success)
			{
			iStreamW.WriteUInt8(eResp_Close);
			return false;
			}

		iStreamW.WriteUInt8(eResp_SyncResults);

		iStreamW.WriteCount(watcherAddedIDs.size());
		for (vector<uint64>::const_iterator
			i = watcherAddedIDs.begin(), theEnd = watcherAddedIDs.end();
			i != theEnd; ++i)
			{
			iStreamW.WriteUInt64(*i);
			}

		iStreamW.WriteCount(changedTupleIDs.size());
		vector<ZTuple>::const_iterator iterCT = changedTuples.begin();
		for (vector<uint64>::const_iterator
			i = changedTupleIDs.begin(), theEnd = changedTupleIDs.end();
			i != theEnd; ++i, ++iterCT)
			{
			iStreamW.WriteUInt64(*i);
			(*iterCT).ToStream(iStreamW);
			}

		iStreamW.WriteCount(changedQueries.size());
		for (map<int64, vector<uint64> >::const_iterator
			i = changedQueries.begin(), theEnd = changedQueries.end();
			i != theEnd; ++i)
			{
			iStreamW.WriteInt64((*i).first);
			const vector<uint64>& theVector = (*i).second;
			iStreamW.WriteCount(theVector.size());
			for (vector<uint64>::const_iterator j = theVector.begin(); j != theVector.end(); ++j)
				iStreamW.WriteUInt64(*j);
			}

		wroteAnything = true;
		}
	else if (fCallbackNeeded)
		{
		if (ZLOG(s, eDebug, "ZTSWatcherServerAsync"))
			s << "Write, callback";
		fCallbackNeeded = false;
		locker.Release();
		iStreamW.WriteUInt8(eResp_SyncSuggested);

		wroteAnything = true;
		}

	if (wroteAnything)
		ZStreamerWriter::Wake();
	else
		iStreamW.Flush();

	return true;
	}

void ZTSWatcherServerAsync::Finished()
	{
	fTSWatcher->SetCallback(nullptr, nullptr);
	ZTask::pFinished();
	}

void ZTSWatcherServerAsync::pCallback()
	{
	ZMutexLocker locker(fMutex);
	if (!fCallbackNeeded)
		{
		fCallbackNeeded = true;
		ZStreamerWriter::Wake();
		}
	}

void ZTSWatcherServerAsync::spCallback(void* iRefcon)
	{ static_cast<ZTSWatcherServerAsync*>(iRefcon)->pCallback(); }

} // namespace ZooLib
