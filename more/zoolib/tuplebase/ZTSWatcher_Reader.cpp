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

#include "zoolib/tuplebase/ZTSWatcher_Reader.h"

#include "zoolib/tuplebase/ZTSWatcherDefines.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL.h"

namespace ZooLib {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Reader

ZTSWatcher_Reader::ZTSWatcher_Reader(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	ZStreamerReader(iStreamerR),
	fStreamerW(iStreamerW),
	fStreamsOkay(true),
	fSentClose(false),
	fBaseID(nullptr),
	fCountIssued(nullptr),
	fWatcherAddedIDs(nullptr),
	fChangedTupleIDs(nullptr),
	fChangedTuples(nullptr),
	fChangedQueries(nullptr),
	fCallback(nullptr),
	fRefcon(nullptr)
	{}

ZTSWatcher_Reader::~ZTSWatcher_Reader()
	{
	ZMutexLocker locker(fMutex_Structure);
	if (!fSentClose && fStreamsOkay)
		{
		fSentClose = true;
		ZMutexLocker locker_Stream(fMutex_Stream);
		try
			{
			const ZStreamW& theStreamW = fStreamerW->GetStreamW();
			theStreamW.WriteUInt8(eReq_Close);
			theStreamW.Flush();
			}
		catch (...)
			{}
		}
//	fStreamsOkay = false;
//	fCondition.Broadcast();
//	locker.Release();

//	this->WaitForDetach();
	}

void ZTSWatcher_Reader::ReadStarted()
	{
	ZMutexLocker locker(fMutex_Structure);
	fStreamsOkay = true;
	fCondition.Broadcast();
	}

void ZTSWatcher_Reader::ReadFinished()
	{
	ZMutexLocker locker(fMutex_Structure);
	fStreamsOkay = false;
	fCondition.Broadcast();
	}

bool ZTSWatcher_Reader::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	try
		{
		return this->pAllocateIDs(iCount, oBaseID, oCountIssued);
		}
	catch (...)
		{}

	ZMutexLocker locker(fMutex_Structure);
	fStreamsOkay = false;
	fCondition.Broadcast();
	return false;
	}

bool ZTSWatcher_Reader::pAllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	ZMutexLocker locker_Structure(fMutex_Structure);

	while (fBaseID && fStreamsOkay)
		fCondition.Wait(fMutex_Structure);

	if (!fStreamsOkay)
		return false;

	fBaseID = &oBaseID;
	fCountIssued = &oCountIssued;

	locker_Structure.Release();

	ZMutexLocker locker_Stream(fMutex_Stream);

	const ZStreamW& theStreamW = fStreamerW->GetStreamW();

	theStreamW.WriteUInt8(eReq_IDs);
	theStreamW.WriteCount(iCount);
	theStreamW.Flush();

	locker_Stream.Release();

	locker_Structure.Acquire();
	while (fBaseID && fStreamsOkay)
		fCondition.Wait(fMutex_Structure);

	return fStreamsOkay;
	}

bool ZTSWatcher_Reader::Sync(
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	std::vector<uint64>& oWatcherAddedIDs,
	std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	std::map<int64, std::vector<uint64> >& oChangedQueries)
	{
	try
		{
		return this->pSync(
			iRemovedIDs, iRemovedIDsCount,
			iAddedIDs, iAddedIDsCount,
			iRemovedQueries, iRemovedQueriesCount,
			iAddedQueries, iAddedQueriesCount,
			oWatcherAddedIDs,
			oChangedTupleIDs, oChangedTuples,
			iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
			oChangedQueries);
		}
	catch (...)
		{}

	ZMutexLocker locker(fMutex_Structure);
	fStreamsOkay = false;
	fCondition.Broadcast();
	return false;
	}

bool ZTSWatcher_Reader::pSync(
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	std::vector<uint64>& oWatcherAddedIDs,
	std::vector<uint64>& oChangedTupleIDs, std::vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	std::map<int64, std::vector<uint64> >& oChangedQueries)
	{
	ZMutexLocker locker_Structure(fMutex_Structure);

	while (fWatcherAddedIDs && fStreamsOkay)
		fCondition.Wait(fMutex_Structure);

	if (!fStreamsOkay)
		return false;

	fWatcherAddedIDs = &oWatcherAddedIDs;
	fChangedTupleIDs = &oChangedTupleIDs;
	fChangedTuples = &oChangedTuples;
	fChangedQueries = &oChangedQueries;

	locker_Structure.Release();

	ZMutexLocker locker_Stream(fMutex_Stream);

	const ZStreamW& theStreamW = fStreamerW->GetStreamW();

	theStreamW.WriteUInt8(eReq_Sync);

	theStreamW.WriteCount(iRemovedIDsCount);
	for (size_t count = iRemovedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iRemovedIDs++);


	theStreamW.WriteCount(iAddedIDsCount);
	for (size_t count = iAddedIDsCount; count; --count)
		theStreamW.WriteUInt64(*iAddedIDs++);


	theStreamW.WriteCount(iRemovedQueriesCount);
	for (size_t count = iRemovedQueriesCount; count; --count)
		theStreamW.WriteInt64(*iRemovedQueries++);


	theStreamW.WriteCount(iAddedQueriesCount);
	for (size_t count = iAddedQueriesCount; count; --count)
		{
		const AddedQueryCombo& theAQC = *iAddedQueries++;

		theStreamW.WriteInt64(theAQC.fRefcon);
		theStreamW.WriteBool(theAQC.fPrefetch);

		ZMemoryBlock theMB = theAQC.fMemoryBlock;

		if (0 == theMB.GetSize())
			{
			ZAssert(theAQC.fTBQuery);
			theAQC.fTBQuery.ToStream(ZStreamRWPos_MemoryBlock(theMB, 1024));
			}

		theStreamW.WriteCount(theMB.GetSize());
		theStreamW.Write(theMB.GetPtr(), theMB.GetSize());
		}


	theStreamW.WriteCount(iWrittenTuplesCount);
	for (size_t count = iWrittenTuplesCount; count; --count)
		{
		theStreamW.WriteInt64(*iWrittenTupleIDs++);
		(*iWrittenTuples++).ToStream(theStreamW);
		}

	theStreamW.Flush();
	locker_Stream.Release();

	locker_Structure.Acquire();

	while (fWatcherAddedIDs && fStreamsOkay)
		fCondition.Wait(fMutex_Structure);

	return fStreamsOkay;
	}

bool ZTSWatcher_Reader::Read(const ZStreamR& iStreamR)
	{
	EResp theResp = (EResp)iStreamR.ReadUInt8();
	switch (theResp)
		{
		case eResp_Close:
			{
			ZMutexLocker locker(fMutex_Structure);
			if (!fSentClose)
				{
				fSentClose = true;
				ZMutexLocker locker_Stream(fMutex_Stream);
				const ZStreamW& theStreamW = fStreamerW->GetStreamW();
				theStreamW.WriteUInt8(eReq_Close);
				theStreamW.Flush();
				}
			fStreamsOkay = false;
			fCondition.Broadcast();
			return false;
			}
		case eResp_IDs:
			{
			const uint64 theBaseID = iStreamR.ReadUInt64();
			const size_t theCountIssued = iStreamR.ReadCount();
			ZMutexLocker locker(fMutex_Structure);
			ZAssert(fBaseID);
			*fBaseID = theBaseID;
			*fCountIssued = theCountIssued;
			fBaseID = nullptr;
			fCountIssued = nullptr;
			fCondition.Broadcast();
			break;
			}
		case eResp_SyncResults:
			{
			vector<uint64> watcherAddedIDs;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				watcherAddedIDs.reserve(theCount);
				while (theCount--)
					watcherAddedIDs.push_back(iStreamR.ReadUInt64());
				}

			vector<uint64> changedTupleIDs;
			vector<ZTuple> changedTuples;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				changedTupleIDs.reserve(theCount);
				changedTuples.reserve(theCount);
				while (theCount--)
					{
					changedTupleIDs.push_back(iStreamR.ReadUInt64());
					changedTuples.push_back(ZTuple(iStreamR));
					}
				}

			map<int64, vector<uint64> > changedQueries;
			if (uint32 theCount = iStreamR.ReadCount())
				{
				while (theCount--)
					{
					int64 theRefcon = iStreamR.ReadInt64();
					pair<map<int64, vector<uint64> >::iterator, bool> pos =
						changedQueries.insert(pair<int64, vector<uint64> >(theRefcon, vector<uint64>()));

					vector<uint64>& theVector = (*pos.first).second;
					if (uint32 theIDCount = iStreamR.ReadCount())
						{
						theVector.reserve(theIDCount);
						while (theIDCount--)
							theVector.push_back(iStreamR.ReadUInt64());
						}
					}
				}

			ZMutexLocker locker(fMutex_Structure);
			ZAssert(fWatcherAddedIDs);

			fWatcherAddedIDs->swap(watcherAddedIDs);
			fWatcherAddedIDs = nullptr;

			fChangedTupleIDs->swap(changedTupleIDs);
			fChangedTupleIDs = nullptr;

			fChangedTuples->swap(changedTuples);
			fChangedTuples = nullptr;

			fChangedQueries->swap(changedQueries);
			fChangedQueries = nullptr;

			fCondition.Broadcast();
			break;
			}
		case eResp_SyncSuggested:
			{
			ZMutexLocker locker(fMutex_Structure);
			if (fCallback)
				fCallback(fRefcon);
			break;
			}
		}
	return fStreamsOkay;
	}

void ZTSWatcher_Reader::SetCallback(Callback_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	fCallback = iCallback;
	fRefcon = iRefcon;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_Reader

ZTSWatcherFactory_Reader::ZTSWatcherFactory_Reader(
	ZRef<ZStreamerRWFactory> iStreamerRWFactory)
:	fStreamerRWFactory(iStreamerRWFactory)
	{}

ZTSWatcherFactory_Reader::~ZTSWatcherFactory_Reader()
	{}

ZRef<ZTSWatcher> ZTSWatcherFactory_Reader::MakeTSWatcher()
	{
	if (fStreamerRWFactory)
		{
		if (ZRef<ZStreamerRW> theSRW = fStreamerRWFactory->MakeStreamerRW())
			{
			ZRef<ZTSWatcher_Reader> theTSWR = new ZTSWatcher_Reader(theSRW, theSRW);
			sStartWorkerRunner(theTSWR);
			return theTSWR;
			}
		}

	return null;
	}

} // namespace ZooLib

