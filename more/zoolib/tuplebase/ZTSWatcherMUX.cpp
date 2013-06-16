/* -------------------------------------------------------------------------------------------------
Copyright (c) 2006 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTSWatcherMUX.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

namespace ZooLib {

using namespace ZUtil_STL;

using std::map;
using std::pair;
using std::set;
using std::vector;

namespace { // anonymous
enum EReg { eReg_Fresh, eReg_Pending, eReg_Done };
} // anonymous namespace

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX::PQuery

class ZTSWatcherMUX::DLink_PQuery_Sync
:	public DListLink<ZTSWatcherMUX::PQuery,
		ZTSWatcherMUX::DLink_PQuery_Sync, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::DLink_PQuery_Cached
:	public DListLink<ZTSWatcherMUX::PQuery,
		ZTSWatcherMUX::DLink_PQuery_Cached, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::PQuery
:	public ZTSWatcherMUX::DLink_PQuery_Sync,
	public ZTSWatcherMUX::DLink_PQuery_Cached
	{
private:
	PQuery(); // Not implemented

public:
	explicit PQuery(int64 iRefcon, bool iPrefetch, const ZMemoryBlock& iMB);

	int64 fRefcon;
	EReg fReg;
	ZMemoryBlock fMB;
	bool fPrefetch;
	vector<uint64> fResults;

	DListHead<DLink_WQuery_PQuery> fWQueries;
	};

ZTSWatcherMUX::PQuery::PQuery(int64 iRefcon, bool iPrefetch, const ZMemoryBlock& iMB)
:	fRefcon(iRefcon),
	fReg(eReg_Fresh),
	fMB(iMB),
	fPrefetch(iPrefetch)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX::WQuery

// Links together all the WQueries that reference a PQuery.
class ZTSWatcherMUX::DLink_WQuery_PQuery
:	public DListLink<ZTSWatcherMUX::WQuery,
		ZTSWatcherMUX::DLink_WQuery_PQuery, ZTSWatcherMUX::kDebug>
	{};

// Links together all the WQuerys in a Watcher whose PQuery has changed
// since the watcher last called Sync.
class ZTSWatcherMUX::DLink_WQuery_Tripped
:	public DListLink<ZTSWatcherMUX::WQuery,
		ZTSWatcherMUX::DLink_WQuery_Tripped, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::WQuery
:	public ZTSWatcherMUX::DLink_WQuery_PQuery,
	public ZTSWatcherMUX::DLink_WQuery_Tripped
	{
private:
	WQuery(); // Not implemented

public:
	explicit WQuery(Watcher* iWatcher, PQuery* iPQuery, int64 iRefcon);

	Watcher* fWatcher;
	PQuery* fPQuery;
	int64 fRefcon;
	};

ZTSWatcherMUX::WQuery::WQuery(Watcher* iWatcher, PQuery* iPQuery, int64 iRefcon)
:	fWatcher(iWatcher),
	fPQuery(iPQuery),
	fRefcon(iRefcon)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX::PTuple

class ZTSWatcherMUX::DLink_PTuple_Sync
:	public DListLink<ZTSWatcherMUX::PTuple,
		ZTSWatcherMUX::DLink_PTuple_Sync, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::DLink_PTuple_Cached
:	public DListLink<ZTSWatcherMUX::PTuple,
		ZTSWatcherMUX::DLink_PTuple_Cached, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::PTuple
:	public ZTSWatcherMUX::DLink_PTuple_Sync,
	public ZTSWatcherMUX::DLink_PTuple_Cached
	{
private:
	PTuple(); // Not implemented

public:
	explicit PTuple(uint64 iID);

	uint64 fID;
	EReg fReg;
	bool fHasValueForServer;
	ZTuple fValue;

	DListHead<DLink_WTuple_PTuple> fWTuples;
	Watcher* fWrittenBy;
	};

ZTSWatcherMUX::PTuple::PTuple(uint64 iID)
:	fID(iID),
	fReg(eReg_Fresh),
	fHasValueForServer(false),
	fWrittenBy(nullptr)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX::WTuple

// Links together all the WTuples that reference a PTuple.
class ZTSWatcherMUX::DLink_WTuple_PTuple
:	public DListLink<ZTSWatcherMUX::WTuple,
		ZTSWatcherMUX::DLink_WTuple_PTuple, ZTSWatcherMUX::kDebug>
	{};

// Links together all the WTuples in a Watcher whose PTuple has changed
// since the watcher last called Sync.
class ZTSWatcherMUX::DLink_WTuple_Tripped
:	public DListLink<ZTSWatcherMUX::WTuple,
		ZTSWatcherMUX::DLink_WTuple_Tripped, ZTSWatcherMUX::kDebug>
	{};

class ZTSWatcherMUX::WTuple
:	public ZTSWatcherMUX::DLink_WTuple_PTuple,
	public ZTSWatcherMUX::DLink_WTuple_Tripped
	{
private:
	WTuple(); // Not implemented

public:
	explicit WTuple(Watcher* iWatcher, PTuple* iPTuple);

	Watcher* fWatcher;
	PTuple* fPTuple;
	};

ZTSWatcherMUX::WTuple::WTuple(Watcher* iWatcher, PTuple* iPTuple)
:	fWatcher(iWatcher),
	fPTuple(iPTuple)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX::Watcher

class ZTSWatcherMUX::Watcher : public ZTSWatcher
	{
	friend class ZTSWatcherMUX;
private:
	Watcher(ZRef<ZTSWatcherMUX> iMUX, bool iAlwaysForceSync);
	virtual ~Watcher();

// From ZCounted via ZTSWatcher
	virtual void Finalize();

// From ZTSWatcher
	virtual bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	virtual bool Sync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		vector<uint64>& oAddedIDs,
		vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		map<int64, vector<uint64> >& oChangedQueries);

	virtual void SetCallback(Callback_t iCallback, void* iRefcon);

private:
	ZRef<ZTSWatcherMUX> fMUX;

	const bool fAlwaysForceSync;

	map<int64, WQuery> fWQueries;

	map<uint64, WTuple> fWTuples;

	DListHead<DLink_WQuery_Tripped> fWQueries_Tripped;

	DListHead<DLink_WTuple_Tripped> fWTuples_Tripped;

	Callback_t fCallback;
	void* fCallbackRefcon;
	};

ZTSWatcherMUX::Watcher::Watcher(ZRef<ZTSWatcherMUX> iMUX, bool iAlwaysForceSync)
:	fMUX(iMUX),
	fAlwaysForceSync(iAlwaysForceSync)
	{}

ZTSWatcherMUX::Watcher::~Watcher()
	{}

void ZTSWatcherMUX::Watcher::Finalize()
	{
	fMUX->Watcher_Finalize(this);
	}

bool ZTSWatcherMUX::Watcher::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	return fMUX->Watcher_AllocateIDs(this, iCount, oBaseID, oCountIssued);
	}

bool ZTSWatcherMUX::Watcher::Sync(
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	return fMUX->Watcher_Sync(this,
		iRemovedIDs, iRemovedIDsCount,
		iAddedIDs, iAddedIDsCount,
		iRemovedQueries, iRemovedQueriesCount,
		iAddedQueries, iAddedQueriesCount,
		oAddedIDs,
		oChangedTupleIDs, oChangedTuples,
		iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
		oChangedQueries);
	}

void ZTSWatcherMUX::Watcher::SetCallback(Callback_t iCallback, void* iRefcon)
	{
	fMUX->Watcher_SetCallback(this, iCallback, iRefcon);
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherMUX

ZTSWatcherMUX::ZTSWatcherMUX(ZRef<ZTSWatcher> iTSWatcher,
	size_t iCacheSize_Queries, size_t iCacheSize_Tuples)
:	fCurrentSyncNumber(0),
	fNextSyncNumber(1),
	fForceNextSync(false),
	fTSWatcher(iTSWatcher),
	fNextQueryRefcon(1),
	fCacheSize_Queries(iCacheSize_Queries),
	fCacheSize_Tuples(iCacheSize_Tuples)
	{
	fTSWatcher->SetCallback(spCallback, this);
	}

ZTSWatcherMUX::~ZTSWatcherMUX()
	{
	ZAssertStop(kDebug, fWatchers.empty());
	if (fTSWatcher)
		fTSWatcher->SetCallback(nullptr, nullptr);

	// Purge the caches, so the list destructors' assertions don't trip.
	while (fPQueries_Cached)
		{
		PQuery* thePQuery = sGetEraseBackMust<PQuery>(fPQueries_Cached);
		sEraseMust(fRefcon_To_PQuery, thePQuery->fRefcon);
		sEraseMust(fMB_To_PQuery, thePQuery->fMB);
		}

	while (fPTuples_Cached)
		{
		PTuple* thePTuple = sGetEraseBackMust<PTuple>(fPTuples_Cached);
		sEraseMust(fPTuples, thePTuple->fID);
		}

	if (!fTSWatcher)
		{
		// Our underlying connection had failed, so purge stuff manually.
		for (map<ZMemoryBlock, PQuery>::iterator i = fMB_To_PQuery.begin();
			i != fMB_To_PQuery.end(); ++i)
			{
			PQuery* thePQuery = &i->second;
			sQErase(fPQueries_Sync, thePQuery);
			sEraseMust(kDebug, fRefcon_To_PQuery, thePQuery->fRefcon);
			}
		fMB_To_PQuery.clear();

		for (map<uint64, PTuple>::iterator i = fPTuples.begin();
			i != fPTuples.end(); ++i)
			{
			sQErase(fPTuples_Sync, &i->second);
			}
		fPTuples.clear();
		}
	}

ZRef<ZTSWatcher> ZTSWatcherMUX::MakeTSWatcher()
	{ return this->NewWatcher(false); }

ZRef<ZTSWatcher> ZTSWatcherMUX::NewWatcher(bool iAlwaysForceSync)
	{
	ZRef<Watcher> theWatcher = new Watcher(this, iAlwaysForceSync);
	ZMutexLocker locker(fMutex_Structure);
	fWatchers.insert(theWatcher.Get());
	return theWatcher;
	}

void ZTSWatcherMUX::Watcher_Finalize(Watcher* iWatcher)
	{
	ZMutexLocker locker(fMutex_Structure);
	if (not iWatcher->FinishFinalize())
		return;

	for (map<int64, WQuery>::iterator i = iWatcher->fWQueries.begin();
		i != iWatcher->fWQueries.end(); ++i)
		{
		WQuery* theWQuery = &i->second;
		PQuery* thePQuery = theWQuery->fPQuery;
		sEraseMust(thePQuery->fWQueries, theWQuery);
		sErase(iWatcher->fWQueries_Tripped, theWQuery);
		sInsertBack(fPQueries_Sync, thePQuery);
		}
	iWatcher->fWQueries.clear();

	for (map<uint64, WTuple>::iterator i = iWatcher->fWTuples.begin();
		i != iWatcher->fWTuples.end(); ++i)
		{
		WTuple* theWTuple = &i->second;
		PTuple* thePTuple = theWTuple->fPTuple;
		sEraseMust(thePTuple->fWTuples, theWTuple);
		sQErase(iWatcher->fWTuples_Tripped, theWTuple);
		sQInsertBack(fPTuples_Sync, thePTuple);
		}
	iWatcher->fWTuples.clear();

	sEraseMust(kDebug, fWatchers, iWatcher);

	delete iWatcher;
	}

bool ZTSWatcherMUX::Watcher_AllocateIDs(Watcher* iWatcher,
	size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	ZMutexLocker locker(fMutex_Structure);

	ZRef<ZTSWatcher> localTSWatcherRef = fTSWatcher;

	locker.Release();

	if (localTSWatcherRef && localTSWatcherRef->AllocateIDs(iCount, oBaseID, oCountIssued))
		return true;

	locker.Acquire();
	fTSWatcher.Clear();
	return false;
	}

bool ZTSWatcherMUX::Watcher_Sync(Watcher* iWatcher,
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZMutexLocker locker(fMutex_Structure);

	if (!fTSWatcher)
		return false;

	// waitForSync gets set when this watcher has any tuples to write,
	// or if it adds tuples or queries that are not already locally known.
	bool waitForSync = (iWrittenTuplesCount != 0);

	for (size_t x = 0; x < iAddedIDsCount; ++x)
		{
		const uint64 theID = iAddedIDs[x];
		map<uint64, WTuple>::iterator i = iWatcher->fWTuples.lower_bound(theID);
		ZAssertStop(kDebug, i == iWatcher->fWTuples.end() || i->first != theID);

		PTuple* thePTuple = this->pGetPTuple(theID);
		sQErase(fPTuples_Cached, thePTuple);

		WTuple* theWTuple = &iWatcher->fWTuples.insert(i,
			pair<uint64, WTuple>(theID, WTuple(iWatcher, thePTuple)))->second;

		sInsertBackMust(thePTuple->fWTuples, theWTuple);

		if (thePTuple->fReg == eReg_Done)
			{
			sInsertBackMust(iWatcher->fWTuples_Tripped, theWTuple);
			}
		else
			{
			waitForSync = true;
			sInsertBackMust(fPTuples_Sync, thePTuple);
			}
		}

	for (size_t x = 0; x < iRemovedIDsCount; ++x)
		{
		const uint64 theID = iRemovedIDs[x];
		map<uint64, WTuple>::iterator i = iWatcher->fWTuples.find(theID);
		ZAssertStop(kDebug, i != iWatcher->fWTuples.end());
		WTuple* theWTuple = &i->second;
		PTuple* thePTuple = theWTuple->fPTuple;
		sEraseMust(thePTuple->fWTuples, theWTuple);
		sQErase(iWatcher->fWTuples_Tripped, theWTuple);
		iWatcher->fWTuples.erase(i);
		if (!thePTuple->fWTuples)
			{
			sQInsertBack(fPTuples_Sync, thePTuple);
			ZAssertStop(kDebug, !fPTuples_Cached.Contains(thePTuple));
			}
		}

	for (size_t x = 0; x < iWrittenTuplesCount; ++x)
		{
		PTuple* thePTuple = this->pGetPTuple(iWrittenTupleIDs[x]);
		// It's feasible that a client may write to a recently un-registered tuple,
		// in which case this check would be wrong:
		// ZAssertStop(kDebug, !fPTuples_Cached.Contains(thePTuple));

		thePTuple->fHasValueForServer = true;
		thePTuple->fValue = iWrittenTuples[x];
		thePTuple->fWrittenBy = iWatcher;
		sQInsertBack(fPTuples_Sync, thePTuple);
		for (DListIterator<WTuple, DLink_WTuple_PTuple>
			iter = thePTuple->fWTuples;iter; iter.Advance())
			{
			WTuple* theWTuple = iter.Current();
			sQInsertBack(theWTuple->fWatcher->fWTuples_Tripped, theWTuple);
			}
		}

	for (size_t x = 0; x < iAddedQueriesCount; ++x)
		{
		const int64 theWRefcon = iAddedQueries[x].fRefcon;
		map<int64, WQuery>::iterator i = iWatcher->fWQueries.lower_bound(theWRefcon);
		ZAssertStop(kDebug, i == iWatcher->fWQueries.end() || i->first != theWRefcon);

		PQuery* thePQuery = this->pGetPQuery(iAddedQueries[x]);
		WQuery* theWQuery = &iWatcher->fWQueries.insert(i,
			pair<int64, WQuery>(theWRefcon, WQuery(iWatcher, thePQuery, theWRefcon)))->second;

		sQErase(fPQueries_Cached, thePQuery);
		sInsertBackMust(thePQuery->fWQueries, theWQuery);

		if (thePQuery->fReg == eReg_Done)
			{
			sInsertBackMust(iWatcher->fWQueries_Tripped, theWQuery);
			}
		else
			{
			waitForSync = true;
			sQInsertBack(fPQueries_Sync, thePQuery);
			}
		}

	for (size_t x = 0; x < iRemovedQueriesCount; ++x)
		{
		int64 theWRefcon = iRemovedQueries[x];
		map<int64, WQuery>::iterator i = iWatcher->fWQueries.find(theWRefcon);
		ZAssertStop(kDebug, i != iWatcher->fWQueries.end());
		WQuery* theWQuery = &i->second;
		PQuery* thePQuery = theWQuery->fPQuery;
		sEraseMust(thePQuery->fWQueries, theWQuery);
		sQErase(iWatcher->fWQueries_Tripped, theWQuery);
		iWatcher->fWQueries.erase(i);
		if (!thePQuery->fWQueries)
			{
			sQInsertBack(fPQueries_Sync, thePQuery);
			ZAssertStop(kDebug, !fPQueries_Cached.Contains(thePQuery));
			}
		}

	if (!this->pSyncAll(waitForSync, iWatcher))
		{
		return false;
		}

	set<uint64> allResults;
	for (DListEraser<WQuery, DLink_WQuery_Tripped>
		iter = iWatcher->fWQueries_Tripped;iter; iter.Advance())
		{
		WQuery* theWQuery = iter.Current();
		PQuery* thePQuery = theWQuery->fPQuery;
		allResults.insert(thePQuery->fResults.begin(), thePQuery->fResults.end());
		oChangedQueries.insert(
			pair<int64, vector<uint64> >(theWQuery->fRefcon, thePQuery->fResults));
		}

	// Walk every ID returned by a changed query and see if it
	// should be returned as if it had been server added.
	for (set<uint64>::iterator i = allResults.begin(); i != allResults.end(); ++i)
		{
		const uint64 theID = *i;
		map<uint64, WTuple>::iterator iterWTuples = iWatcher->fWTuples.lower_bound(theID);
		if (iterWTuples == iWatcher->fWTuples.end() || iterWTuples->first != theID)
			{
			// This ID has not been explicitly added by this watcher.
			if (PTuple* thePTuple = this->pGetPTupleIfExtant(theID))
				{
				// But it's been added by someone.
				if (thePTuple->fReg == eReg_Done)
					{
					// And is known to the server.
					WTuple* theWTuple = &iWatcher->fWTuples.insert(iterWTuples,
						pair<uint64, WTuple>(theID, WTuple(iWatcher, thePTuple)))->second;

					sQErase(fPTuples_Cached, thePTuple);
					sInsertBackMust(thePTuple->fWTuples, theWTuple);

					sInsertBackMust(iWatcher->fWTuples_Tripped, theWTuple);
					oAddedIDs.push_back(theID);
					}
				}
			}
		}

	oChangedTupleIDs.reserve(iWatcher->fWTuples_Tripped.Size());
	oChangedTuples.reserve(iWatcher->fWTuples_Tripped.Size());
	for (DListEraser<WTuple, DLink_WTuple_Tripped>
		iter = iWatcher->fWTuples_Tripped;iter; iter.Advance())
		{
		PTuple* thePTuple = iter.Current()->fPTuple;
		if (thePTuple->fWrittenBy == iWatcher)
			{
			thePTuple->fWrittenBy = nullptr;
			}
		else
			{
			oChangedTupleIDs.push_back(thePTuple->fID);
			oChangedTuples.push_back(thePTuple->fValue);
			}
		}

	return true;
	}

void ZTSWatcherMUX::Watcher_SetCallback(Watcher* iWatcher,
	ZTSWatcher::Callback_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	iWatcher->fCallback = iCallback;
	iWatcher->fCallbackRefcon = iRefcon;
	if (iWatcher->fWTuples_Tripped || iWatcher->fWQueries_Tripped)
		{
		if (iWatcher->fCallback)
			iWatcher->fCallback(iWatcher->fCallbackRefcon);
		}
	}

bool ZTSWatcherMUX::pSyncAll(bool iWaitForSync, Watcher* iWatcher)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());

	vector<uint64> addedIDs;
	vector<PTuple*> addedPTuples;
	vector<uint64> writtenIDs;
	vector<ZTuple> writtenTuples;

	addedIDs.reserve(fPTuples_Sync.Size());
	addedPTuples.reserve(fPTuples_Sync.Size());
	writtenIDs.reserve(fPTuples_Sync.Size());
	writtenTuples.reserve(fPTuples_Sync.Size());

	for (DListEraser<PTuple, DLink_PTuple_Sync>
		iter = fPTuples_Sync;iter; iter.Advance())
		{
		PTuple* thePTuple = iter.Current();
		if (thePTuple->fHasValueForServer)
			{
			thePTuple->fHasValueForServer = false;
			writtenIDs.push_back(thePTuple->fID);
			writtenTuples.push_back(thePTuple->fValue);
			}

		if (sIsEmpty(thePTuple->fWTuples))
			{
			ZAssertStop(kDebug, thePTuple->fReg != eReg_Pending);
			if (thePTuple->fReg == eReg_Done)
				{
				// In the case of a write to a recently un-registered tuple, it
				// may be on the cached list already, so do a conditional
				// insert (see commented-out assertion in
				// pSync's handling of iWrittenTuples).
				sQInsertFront(fPTuples_Cached, thePTuple);
				}
			else
				{
				// It's a tuple that got written, but not explicitly
				// added. The likelihood is that there's a query registered that
				// will pick this up and would return the just-written value
				// to us. So we add it, and after calling Sync we'll put it
				// on the Sync list so that the next pSyncAll may remove it.
				thePTuple->fReg = eReg_Pending;
				addedIDs.push_back(thePTuple->fID);
				addedPTuples.push_back(thePTuple);
				}
			}
		else
			{
			if (thePTuple->fReg == eReg_Fresh)
				{
				thePTuple->fReg = eReg_Pending;
				addedIDs.push_back(thePTuple->fID);
				addedPTuples.push_back(thePTuple);
				}
			}
		}

	vector<uint64> removedIDs;
	if (fPTuples_Cached.Size() > fCacheSize_Tuples)
		{
		removedIDs.reserve(fPTuples_Cached.Size() - fCacheSize_Tuples);
		while (fPTuples_Cached.Size() > fCacheSize_Tuples)
			{
			PTuple* thePTuple = sGetEraseBackMust<PTuple>(fPTuples_Cached);
			if (thePTuple->fReg == eReg_Done)
				{
				thePTuple->fReg = eReg_Fresh; // Not strictly necessary
				removedIDs.push_back(thePTuple->fID);
				}
			sEraseMust(kDebug, fPTuples, thePTuple->fID);
			}
		}

	vector<ZTSWatcher::AddedQueryCombo> addedQueries;
	for (DListEraser<PQuery, DLink_PQuery_Sync>
		iter = fPQueries_Sync;iter; iter.Advance())
		{
		PQuery* thePQuery = iter.Current();
		if (sIsEmpty(thePQuery->fWQueries))
			{
			ZAssertStop(kDebug, thePQuery->fReg != eReg_Pending);
			sInsertFrontMust(fPQueries_Cached, thePQuery);
			}
		else
			{
			if (thePQuery->fReg == eReg_Fresh)
				{
				thePQuery->fReg = eReg_Pending;
				ZTSWatcher::AddedQueryCombo theAQC;
				theAQC.fRefcon = thePQuery->fRefcon;
				theAQC.fPrefetch = thePQuery->fPrefetch;
				theAQC.fMemoryBlock = thePQuery->fMB;
				addedQueries.push_back(theAQC);
				}
			}
		}

	vector<int64> removedQueries;
	if (fPQueries_Cached.Size() > fCacheSize_Queries)
		{
		removedQueries.reserve(fPQueries_Cached.Size() - fCacheSize_Queries);
		while (fPQueries_Cached.Size() > fCacheSize_Queries)
			{
			PQuery* thePQuery = sGetEraseBackMust<PQuery>(fPQueries_Cached);
			if (thePQuery->fReg == eReg_Done)
				{
				thePQuery->fReg = eReg_Fresh; // Not strictly necessary
				removedQueries.push_back(thePQuery->fRefcon);
				}
			sEraseMust(kDebug, fRefcon_To_PQuery, thePQuery->fRefcon);
			sEraseMust(kDebug, fMB_To_PQuery, thePQuery->fMB);
			}
		}


	if (not iWatcher->fAlwaysForceSync && !fForceNextSync && !iWaitForSync)
		{
		// We're not being forced to call Sync, nor does our caller
		// want us to wait for Sync.
		if (removedIDs.empty()
			&& addedIDs.empty()
			&& removedQueries.empty()
			&& addedQueries.empty()
			&& writtenIDs.empty())
			{
			// And we haven't got any new work to do, so we can return.
			return true;
			}
		}

	if (const int64 currentSyncNumber = fCurrentSyncNumber)
		{
		// Wait till the running Sync has completed, in case it's
		// fetching data underlying a registration our caller has added.
		while (currentSyncNumber == fCurrentSyncNumber)
			fCondition.Wait(fMutex_Structure);

		// The other call to Sync may have had IDs added by the server,
		// so filter those out of the addedIDs vector we just built.
		vector<uint64> newAddedIDs;
		newAddedIDs.reserve(addedIDs.size());

		vector<PTuple*> newAddedPTuples;
		newAddedPTuples.reserve(addedPTuples.size());

		for (vector<uint64>::iterator i = addedIDs.begin(); i != addedIDs.end(); ++i)
			{
			const uint64 theID = *i;
			PTuple* thePTuple = this->pGetPTupleMustExist(theID);
			ZAssertStop(kDebug, thePTuple->fReg != eReg_Fresh);
			if (thePTuple->fReg == eReg_Pending)
				{
				newAddedIDs.push_back(theID);
				newAddedPTuples.push_back(thePTuple);
				}
			}
		addedIDs.swap(newAddedIDs);
		addedPTuples.swap(newAddedPTuples);
		}

	if (not iWatcher->fAlwaysForceSync && !fForceNextSync)
		{
		// We're not being forced to call Sync.
		if (removedIDs.empty()
			&& addedIDs.empty()
			&& removedQueries.empty()
			&& addedQueries.empty()
			&& writtenIDs.empty())
			{
			// And we haven't got any new work to do, so we can return.
			return true;
			}
		}

	// We're committed to calling Sync, so we can switch off the flag
	// that may have forced us to this point.
	fForceNextSync = false;

	// Record that we're calling Sync
	fCurrentSyncNumber = fNextSyncNumber++;

	ZRef<ZTSWatcher> localTSWatcherRef = fTSWatcher;

	fMutex_Structure.Release();

	vector<uint64> serverAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	if (!localTSWatcherRef->Sync(
			sFirstOrNil(removedIDs), removedIDs.size(),
			sFirstOrNil(addedIDs), addedIDs.size(),
			sFirstOrNil(removedQueries), removedQueries.size(),
			sFirstOrNil(addedQueries), addedQueries.size(),
			serverAddedIDs,
			changedTupleIDs, changedTuples,
			sFirstOrNil(writtenIDs), sFirstOrNil(writtenTuples), writtenIDs.size(),
			changedQueries))
		{
		fMutex_Structure.Acquire();
		fCurrentSyncNumber = 0;
		fCondition.Broadcast();
		fTSWatcher.Clear();
		return false;
		}

	fMutex_Structure.Acquire();

	// Mark pending PTuples as being done, and put any that are unsed
	// on the Sync list, to be examined by a later invocation of pSyncAll.
	for (vector<PTuple*>::iterator i = addedPTuples.begin(); i != addedPTuples.end(); ++i)
		{
		PTuple* thePTuple = *i;
		ZAssertStop(kDebug, thePTuple->fReg == eReg_Pending);
		thePTuple->fReg = eReg_Done;
		if (sIsEmpty(thePTuple->fWTuples))
			sQInsertBack(fPTuples_Sync, thePTuple);
		}

	for (vector<uint64>::iterator i = serverAddedIDs.begin(); i != serverAddedIDs.end(); ++i)
		{
		PTuple* thePTuple = this->pGetPTuple(*i);
		ZAssertStop(kDebug, !fPTuples_Cached.Contains(thePTuple));
		if (thePTuple->fReg == eReg_Fresh)
			{
			// We just allocated this PTuple.
			thePTuple->fReg = eReg_Done;
			}
		else if (thePTuple->fReg == eReg_Pending)
			{
			// Some other thread had come in and allocated the PTuple,
			// and has this ID on its addedIDs. The post processing it does
			// after waiting for this Sync to complete will remove it
			// from that thread's addedIDs.
			thePTuple->fReg = eReg_Done;
			}
		else
			{
			// Oops. The server claims to have added this ID,
			// but we think it's already registered.
			ZDebugStop(0);
			}
		}

	set<Watcher*> watchersToCallback;
	ZAssertStop(kDebug, changedTupleIDs.size() == changedTuples.size());

	for (size_t x = 0, count = changedTupleIDs.size(); x < count; ++x)
		{
		const uint64 theID = changedTupleIDs[x];
		if (PTuple* thePTuple = this->pGetPTupleIfExtant(theID))
			{
			// No other thread caused this PTuple to be released whilst Sync was running.
			ZAssertStop(kDebug, thePTuple->fReg == eReg_Done);
			if (!thePTuple->fHasValueForServer)
				{
				// Nothing touched this tuple whilst Sync was running.
				for (DListIterator<WTuple, DLink_WTuple_PTuple>
					iter = thePTuple->fWTuples;iter; iter.Advance())
					{
					WTuple* theWTuple = iter.Current();
					sQInsertBack(theWTuple->fWatcher->fWTuples_Tripped, theWTuple);
					watchersToCallback.insert(theWTuple->fWatcher);
					}
				thePTuple->fValue = changedTuples[x];
				}
			}
		}

	for (map<int64, vector<uint64> >::iterator i = changedQueries.begin();
		i != changedQueries.end(); ++i)
		{
		int64 theRefcon = i->first;
		if (PQuery* thePQuery = this->pGetPQueryIfExtant(theRefcon))
			{
			// No other thread caused this pQuery to be released whilst Sync was running.
			ZAssertStop(kDebug, thePQuery->fReg != eReg_Fresh);
			thePQuery->fReg = eReg_Done;
			for (DListIterator<WQuery, DLink_WQuery_PQuery>
				iter = thePQuery->fWQueries;iter; iter.Advance())
				{
				WQuery* theWQuery = iter.Current();
				sInsertBack(theWQuery->fWatcher->fWQueries_Tripped, theWQuery);
				watchersToCallback.insert(theWQuery->fWatcher);
				}
			thePQuery->fResults.swap(i->second);
			}
		}

	fCurrentSyncNumber = 0;
	fCondition.Broadcast();

	for (set<Watcher*>::iterator i = watchersToCallback.begin();
		i != watchersToCallback.end(); ++i)
		{
		Watcher* theWatcher = *i;
		if (theWatcher != iWatcher)
			{
			if (theWatcher->fCallback)
				theWatcher->fCallback(theWatcher->fCallbackRefcon);
			}
		}

	return true;
	}

ZTSWatcherMUX::PQuery* ZTSWatcherMUX::pGetPQuery(const ZTSWatcher::AddedQueryCombo& iAQC)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());
	ZMemoryBlock theMB = iAQC.fMemoryBlock;
	if (not theMB.GetSize())
		iAQC.fTBQuery.ToStream(ZStreamRWPos_MemoryBlock(theMB));

	map<ZMemoryBlock, PQuery>::iterator i = fMB_To_PQuery.lower_bound(theMB);
	if (i != fMB_To_PQuery.end() && i->first == theMB)
		return &i->second;

	int64 theRefcon = fNextQueryRefcon++;
	PQuery* thePQuery = &fMB_To_PQuery.insert(i,
		pair<ZMemoryBlock, PQuery>(theMB, PQuery(theRefcon, iAQC.fPrefetch, theMB)))->second;

	fRefcon_To_PQuery.insert(pair<int64, PQuery*>(theRefcon, thePQuery));
	return thePQuery;
	}

ZTSWatcherMUX::PQuery* ZTSWatcherMUX::pGetPQueryIfExtant(int64 iRefcon)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());
	map<int64, PQuery*>::iterator i = fRefcon_To_PQuery.find(iRefcon);
	if (i != fRefcon_To_PQuery.end())
		return i->second;
	return nullptr;
	}

ZTSWatcherMUX::PTuple* ZTSWatcherMUX::pGetPTuple(uint64 iID)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());
	map<uint64, PTuple>::iterator i = fPTuples.lower_bound(iID);
	if (i != fPTuples.end() && i->first == iID)
		return &i->second;

	PTuple* thePTuple = &fPTuples.insert(i, pair<uint64, PTuple>(iID, PTuple(iID)))->second;
	return thePTuple;
	}

ZTSWatcherMUX::PTuple* ZTSWatcherMUX::pGetPTupleMustExist(uint64 iID)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());
	map<uint64, PTuple>::iterator i = fPTuples.find(iID);
	ZAssertStop(kDebug, i != fPTuples.end());
	return &i->second;
	}

ZTSWatcherMUX::PTuple* ZTSWatcherMUX::pGetPTupleIfExtant(uint64 iID)
	{
	ZAssertStop(kDebug, fMutex_Structure.IsLocked());
	map<uint64, PTuple>::iterator i = fPTuples.find(iID);
	if (i != fPTuples.end())
		return &i->second;
	return nullptr;
	}

void ZTSWatcherMUX::pCallback()
	{
	ZMutexLocker locker(fMutex_Structure);
	if (!fForceNextSync)
		{
		fForceNextSync = true;
		// Calling any registered watcher's callback should be enough
		// to get Sync called, and thus pSyncAll, which may then tickle any
		// further watcher that needs it.
		for (set<Watcher*>::iterator i = fWatchers.begin();
			i != fWatchers.end(); ++i)
			{
			Watcher* theWatcher = *i;
			if (theWatcher->fCallback)
				{
				ZRef<Watcher> localWatcherRef = theWatcher;
				// There is still a race here -- the user of the watcher
				// could remove the callback as we're trying to call it.
				locker.Release();
				localWatcherRef->fCallback(localWatcherRef->fCallbackRefcon);
				break;
				}
			}
		}
	}

void ZTSWatcherMUX::spCallback(void* iRefcon)
	{ static_cast<ZTSWatcherMUX*>(iRefcon)->pCallback(); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_TSWatcherMUX

ZTSWatcherFactory_TSWatcherMUX::ZTSWatcherFactory_TSWatcherMUX(
	ZRef<ZTSWatcherMUX> iMUX, bool iAlwaysForceSync)
:	fMUX(iMUX),
	fAlwaysForceSync(iAlwaysForceSync)
	{}

ZRef<ZTSWatcher> ZTSWatcherFactory_TSWatcherMUX::MakeTSWatcher()
	{ return fMUX->NewWatcher(fAlwaysForceSync); }

} // namespace ZooLib

