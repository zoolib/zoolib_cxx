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

#include "zoolib/tuplebase/ZTS_Watchable.h"
#include "zoolib/tuplebase/ZTSWatcher.h"
#include "zoolib/tuplebase/ZTupleQuisitioner.h"

#include "zoolib/ZDebug.h"
#include "zoolib/ZDList.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::lower_bound;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace ZooLib {

using namespace ZUtil_STL;

#ifdef ZCONFIG_TS_Watchable_DumpStuff
#	define kDebug_DumpStuff ZCONFIG_TS_Watchable_DumpStuff
#else
#	define kDebug_DumpStuff 0
#endif

#ifdef ZCONFIG_TS_Watchable_Time
#	define kDebug_Time ZCONFIG_TS_Watchable_Time
#else
#	define kDebug_Time 0
#endif

#ifdef ZCONFIG_TS_Watchable_CountInvalidations
#	define kDebug_CountInvalidations ZCONFIG_TS_Watchable_CountInvalidations
#else
#	define kDebug_CountInvalidations 0
#endif

#define ASSERTLOCKED(a) ZAssertStop(kDebug, a.IsLocked())
#define ASSERTUNLOCKED(a) ZAssertStop(kDebug, !a.IsLocked())

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::PTuple

class ZTS_Watchable::PTuple
	{
public:
	PTuple(uint64 iID) : fID(iID) {}

	const uint64 fID;

	set<Watcher*> fUsingWatchers;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::PSpec

class ZTS_Watchable::PSpec
	{
public:
	PSpec(const ZTBSpec& iTBSpec)
	:	fTBSpec(iTBSpec)
		{ fTBSpec.GetPropNames(fPropNames); }

	const ZTBSpec& GetTBSpec() { return fTBSpec; }

	const set<ZTName>& GetPropNames() { return fPropNames; }

	ZTBSpec const fTBSpec;
	set<ZTName> fPropNames;

	set<PQuery*> fUsingPQueries;
	set<uint64> fResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::PQuery

class ZTS_Watchable::PQuery
	{
public:
	PQuery(const ZMemoryBlock& iMB, const ZTBQuery& iTBQuery)
	:	fMB(iMB), fTBQuery(iTBQuery), fResultsValid(false) {}

	ZMemoryBlock const fMB;
	ZTBQuery const fTBQuery;

	ZMutex fMutex;

	set<PSpec*> fPSpecs;

	bool fResultsValid;
	vector<uint64> fResults;

	DListHead<WatcherQueryUsing> fUsingWatcherQueries;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::WatcherQuery

class ZTS_Watchable::WatcherQueryTripped :
	public DListLink<ZTS_Watchable::WatcherQuery,
		ZTS_Watchable::WatcherQueryTripped, ZTS_Watchable::kDebug>
	{};

class ZTS_Watchable::WatcherQueryUsing :
	public DListLink<ZTS_Watchable::WatcherQuery,
		ZTS_Watchable::WatcherQueryUsing, ZTS_Watchable::kDebug>
	{};

class ZTS_Watchable::WatcherQuery :
	public ZTS_Watchable::WatcherQueryTripped,
	public ZTS_Watchable::WatcherQueryUsing
	{
public:
	WatcherQuery(Watcher* iWatcher, int64 iRefcon, PQuery* iPQuery, bool iPrefetch)
	:	fWatcher(iWatcher),
		fRefcon(iRefcon),
		fPQuery(iPQuery),
		fPrefetch(iPrefetch)
		{}

	Watcher* const fWatcher;
	int64 const fRefcon;
	PQuery* const fPQuery;
	bool const fPrefetch;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::Watcher

class ZTS_Watchable::Watcher : public ZTSWatcher
	{
public:
	Watcher(ZRef<ZTS_Watchable> iTS_Watchable);

// From ZCounted via ZTSWatcher
	virtual void Finalize();

// From ZTSWatcher
	virtual bool AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued);

	virtual bool Sync(
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		vector<uint64>& oAddedIDs,
		vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		map<int64, vector<uint64> >& oChangedQueries);

	virtual void SetCallback(Callback_t iCallback, void* iRefcon);

	ZRef<ZTS_Watchable> fTS_Watchable;

	set<PTuple*> fPTuples;
	set<PTuple*> fTrippedPTuples;

	map<int64, WatcherQuery*> fRefcon_To_WatcherQuery;
	DListHead<WatcherQueryTripped> fTrippedWatcherQueries;

	Callback_t fCallback;
	void* fRefcon;
	};

ZTS_Watchable::Watcher::Watcher(ZRef<ZTS_Watchable> iTS_Watchable)
:	fTS_Watchable(iTS_Watchable),
	fCallback(nullptr),
	fRefcon(nullptr)
	{}

void ZTS_Watchable::Watcher::Finalize()
	{ fTS_Watchable->Watcher_Finalize(this); }

bool ZTS_Watchable::Watcher::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	fTS_Watchable->Watcher_AllocateIDs(this, iCount, oBaseID, oCountIssued);
	return true;
	}

bool ZTS_Watchable::Watcher::Sync(
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	oAddedIDs.clear();
	fTS_Watchable->Watcher_Sync(this,
		iRemovedIDs, iRemovedIDsCount,
		iAddedIDs, iAddedIDsCount,
		iRemovedQueries, iRemovedQueriesCount,
		iAddedQueries, iAddedQueriesCount,
		oAddedIDs,
		oChangedTupleIDs, oChangedTuples,
		iWrittenTupleIDs, iWrittenTuples, iWrittenTuplesCount,
		oChangedQueries);

	return true;
	}

void ZTS_Watchable::Watcher::SetCallback(ZTSWatcher::Callback_t iCallback, void* iRefcon)
	{ fTS_Watchable->Watcher_SetCallback(this, iCallback, iRefcon); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable::Quisitioner

class ZTS_Watchable::Quisitioner : public ZTupleQuisitioner
	{
public:
	Quisitioner(ZRef<ZTS_Watchable> iTS_Watchable, PQuery* iPQuery);

// From ZTupleQuisitioner
	virtual void Search(const ZTBSpec& iTBSpec, set<uint64>& ioIDs);
	virtual void Search(const ZTBSpec& iTBSpec, vector<uint64>& oIDs);

	virtual void FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples);
	virtual ZTuple FetchTuple(uint64 iID);

private:
	ZRef<ZTS_Watchable> fTS_Watchable;
	PQuery* fPQuery;
	};

ZTS_Watchable::Quisitioner::Quisitioner(ZRef<ZTS_Watchable> iTS_Watchable, PQuery* iPQuery)
:	fTS_Watchable(iTS_Watchable),
	fPQuery(iPQuery)
	{}

void ZTS_Watchable::Quisitioner::Search(const ZTBSpec& iTBSpec, set<uint64>& ioIDs)
	{ fTS_Watchable->Quisitioner_Search(fPQuery, iTBSpec, ioIDs); }

void ZTS_Watchable::Quisitioner::Search(const ZTBSpec& iTBSpec, vector<uint64>& oIDs)
	{ fTS_Watchable->Quisitioner_Search(fPQuery, iTBSpec, oIDs); }

void ZTS_Watchable::Quisitioner::FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{ fTS_Watchable->Quisitioner_FetchTuples(iCount, iIDs, oTuples); }

ZTuple ZTS_Watchable::Quisitioner::FetchTuple(uint64 iID)
	{ return fTS_Watchable->Quisitioner_FetchTuple(iID); }

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable

ZTS_Watchable::ZTS_Watchable(ZRef<ZTS> iTS)
:	fTS(iTS),
	fMutex_Structure("ZTS_Watchable::fMutex_Structure")
	{}

ZTS_Watchable::~ZTS_Watchable()
	{
	ZMutexLocker locker(fMutex_Structure);

	ZAssertStop(kDebug, fWatchers.empty());

	// We're retaining PQueries past the point that they're in active
	// use by any watchers, so we need to explicitly delete any
	// remaining ones, and let the map's destructor take care of
	// actually destroying its entries, which will be stale pointers
	// to PQueries, and the ZTBQuery keys.

	for (map<ZMemoryBlock, PQuery*>::iterator i = fMB_To_PQuery.begin();
		i != fMB_To_PQuery.end(); ++i)
		{
		PQuery* thePQuery = (*i).second;
		for (set<PSpec*>::iterator
			i = thePQuery->fPSpecs.begin(), theEnd = thePQuery->fPSpecs.end();
			i != theEnd; ++i)
			{
			PSpec* thePSpec = *i;
			ZUtil_STL::sEraseMust(kDebug, thePSpec->fUsingPQueries, thePQuery);
			this->pReleasePSpec(thePSpec);
			}
		delete thePQuery;
		}
	}

void ZTS_Watchable::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCount)
	{ fTS->AllocateIDs(iCount, oBaseID, oCount); }

void ZTS_Watchable::SetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{ this->pSetTuples(iCount, iIDs, iTuples); }

void ZTS_Watchable::GetTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{ fTS->GetTuples(iCount, iIDs, oTuples); }

void ZTS_Watchable::Search(const ZTBSpec& iTBSpec, const set<uint64>& iSkipIDs, set<uint64>& oIDs)
	{ fTS->Search(iTBSpec, iSkipIDs, oIDs); }

ZMutexBase& ZTS_Watchable::GetReadLock()
	{ return fTS->GetReadLock(); }

ZMutexBase& ZTS_Watchable::GetWriteLock()
	{ return fTS->GetWriteLock(); }

ZRef<ZTSWatcher> ZTS_Watchable::NewWatcher()
	{
	Watcher* newWatcher = new Watcher(this);
	ZMutexLocker locker(fMutex_Structure);
	fWatchers.insert(newWatcher);
	return newWatcher;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable Watcher

void ZTS_Watchable::Watcher_Finalize(Watcher* iWatcher)
	{
	ZMutexLocker locker(fMutex_Structure);
	if (not iWatcher->FinishFinalize())
		return;

	ZUtil_STL::sEraseMust(kDebug, fWatchers, iWatcher);
	for (set<PTuple*>::iterator i = iWatcher->fPTuples.begin(),
		theEnd = iWatcher->fPTuples.end();
		i != theEnd; ++i)
		{
		PTuple* thePTuple = *i;
		ZUtil_STL::sEraseMust(kDebug, thePTuple->fUsingWatchers, iWatcher);
		this->pReleasePTuple(thePTuple);
		}

	for (map<int64, WatcherQuery*>::iterator i = iWatcher->fRefcon_To_WatcherQuery.begin(),
		theEnd = iWatcher->fRefcon_To_WatcherQuery.end();
		i != theEnd; ++i)
		{
		WatcherQuery* theWatcherQuery = (*i).second;

		// Detach theWatcherQuery from its PQuery.
		PQuery* thePQuery = theWatcherQuery->fPQuery;
		sEraseMust(thePQuery->fUsingWatcherQueries, theWatcherQuery);
		this->pReleasePQuery(thePQuery);

		// If it had been tripped it will be on this watcher's list
		// of tripped queries and must be removed or we'll trip an assertion
		// in theWatcherQuery's destructor.
		sQErase(iWatcher->fTrippedWatcherQueries, theWatcherQuery);

		delete theWatcherQuery;
		}

	delete iWatcher;
	}

void ZTS_Watchable::Watcher_AllocateIDs(
	Watcher* iWatcher, size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{ fTS->AllocateIDs(iCount, oBaseID, oCountIssued); }

template <typename C>
void sDumpIDs(const ZStrimW& s, const C& iIDs)
	{
	for (typename C::const_iterator i = iIDs.begin(); i != iIDs.end(); ++i)
		s.Writef("%llX, ", *i);
	}

void ZTS_Watchable::Watcher_SetCallback(Watcher* iWatcher,
	ZTSWatcher::Callback_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker(fMutex_Structure);
	iWatcher->fCallback = iCallback;
	iWatcher->fRefcon = iRefcon;

	if (iCallback)
		{
		if (iWatcher->fTrippedWatcherQueries || !iWatcher->fTrippedPTuples.empty())
			iWatcher->fCallback(iWatcher->fRefcon);
		}
	}

void ZTS_Watchable::Watcher_Sync(Watcher* iWatcher,
		const uint64* iRemovedIDs, size_t iRemovedIDsCount,
		const uint64* iAddedIDs, size_t iAddedIDsCount,
		const int64* iRemovedQueries, size_t iRemovedQueriesCount,
		const ZTSWatcher::AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
		vector<uint64>& oAddedIDs,
		vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
		const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
		map<int64, vector<uint64> >& oChangedQueries)
	{
	vector<ZTSWatcher::AddedQueryCombo> local_AddedQueries(
		iAddedQueries, iAddedQueries + iAddedQueriesCount);

	// Go through and ensure that the local data structures have ZTBQueries and
	// real ZMemoryBlocks. We do it here to simplify code that calls us.
	// For some it's easy to provide the ZMemoryBlock, for others its easy to
	// provide the ZTBQuery. Obviously it has to provide one or the other.

	// This section of code takes the structure lock intermittently, so conversion
	// between ZMemoryBlock and ZTBQuery runs independently of any other caller to Watcher_Sync.

	bool missingQueries = false;
	for (vector<ZTSWatcher::AddedQueryCombo>::iterator i = local_AddedQueries.begin();
		i != local_AddedQueries.end(); ++i)
		{
		ZTBQuery& theTBQueryRef = i->fTBQuery;
		ZMemoryBlock& theMBRef = i->fMemoryBlock;
		if (!theTBQueryRef)
			{
			// If no query is provided, then there must be an MB.
			ZAssertStop(kDebug, theMBRef.GetSize());
			missingQueries = true;
			}
		else if (!theMBRef.GetSize())
			{
			theTBQueryRef.ToStream(ZStreamRWPos_MemoryBlock(theMBRef));
			}
		}

	// By this point we have MBs for every query. We may not have ZTBQueries for them though.
	ZMutexLocker locker_Structure(fMutex_Structure);
	if (missingQueries)
		{
		missingQueries = false;
		for (vector<ZTSWatcher::AddedQueryCombo>::iterator i = local_AddedQueries.begin();
			i != local_AddedQueries.end(); ++i)
			{
			ZTBQuery& theTBQueryRef = i->fTBQuery;
			ZMemoryBlock& theMBRef = i->fMemoryBlock;

			if (!theTBQueryRef)
				{
				map<ZMemoryBlock, PQuery*>::iterator existing = fMB_To_PQuery.find(theMBRef);
				if (fMB_To_PQuery.end() == existing)
					missingQueries = true;
				else
					theTBQueryRef = existing->second->fTBQuery;
				}
			}
		}

	if (missingQueries)
		{
		locker_Structure.Release();

		// We're still missing one or more queries. We've grabbed everything from the
		// map that we can. We'll have to regenerate from MBs. It may be
		// that meanwhile something has come in, but wtf.
		for (vector<ZTSWatcher::AddedQueryCombo>::iterator i = local_AddedQueries.begin();
			i != local_AddedQueries.end(); ++i)
			{
			ZTBQuery& theTBQueryRef = i->fTBQuery;
			if (!theTBQueryRef)
				{
				ZMemoryBlock& theMBRef = i->fMemoryBlock;
				theTBQueryRef = ZTBQuery(ZStreamRPos_MemoryBlock(theMBRef));
				}
			}

		locker_Structure.Acquire();
		}


	// We hold the structure lock whilst we're updating the registered queries and tuples.

	for (size_t count = iRemovedIDsCount; count; --count)
		{
		PTuple* thePTuple = this->pGetPTupleExtant(*iRemovedIDs++);
		ZUtil_STL::sEraseMust(kDebug, thePTuple->fUsingWatchers, iWatcher);
		ZUtil_STL::sEraseMust(kDebug, iWatcher->fPTuples, thePTuple);
		ZUtil_STL::sErase(iWatcher->fTrippedPTuples, thePTuple);
		this->pReleasePTuple(thePTuple);
		}

	for (size_t count = iAddedIDsCount; count; --count)
		{
		PTuple* thePTuple = this->pGetPTuple(*iAddedIDs++);
		ZUtil_STL::sInsertMust(kDebug, thePTuple->fUsingWatchers, iWatcher);
		ZUtil_STL::sInsertMust(kDebug, iWatcher->fPTuples, thePTuple);
		ZUtil_STL::sInsertMust(kDebug, iWatcher->fTrippedPTuples, thePTuple);
		}

	for (size_t count = iRemovedQueriesCount; count; --count)
		{
		WatcherQuery* theWatcherQuery =
			sGetEraseMust(iWatcher->fRefcon_To_WatcherQuery, *iRemovedQueries++);

		sQErase(iWatcher->fTrippedWatcherQueries, theWatcherQuery);

		PQuery* thePQuery = theWatcherQuery->fPQuery;
		sEraseMust(thePQuery->fUsingWatcherQueries, theWatcherQuery);
		delete theWatcherQuery;
		this->pReleasePQuery(thePQuery);
		}

	for (vector<ZTSWatcher::AddedQueryCombo>::iterator i = local_AddedQueries.begin();
		i != local_AddedQueries.end(); ++i)
		{
		const ZMemoryBlock& theMB = i->fMemoryBlock;
		const int64 theRefcon = i->fRefcon;
		const bool thePrefetch = i->fPrefetch;

		map<ZMemoryBlock, PQuery*>::iterator position = fMB_To_PQuery.lower_bound(theMB);
		PQuery* thePQuery;
		if (position != fMB_To_PQuery.end() && position->first == theMB)
			{
			thePQuery = position->second;
			}
		else
			{
			thePQuery = new PQuery(theMB, i->fTBQuery);
			fMB_To_PQuery.insert(position, pair<ZMemoryBlock, PQuery*>(theMB, thePQuery));
			}

		WatcherQuery* theWatcherQuery =
			new WatcherQuery(iWatcher, theRefcon, thePQuery, thePrefetch);
		sInsertBackMust(thePQuery->fUsingWatcherQueries, theWatcherQuery);

		sInsertMust(iWatcher->fRefcon_To_WatcherQuery, theRefcon, theWatcherQuery);

		sInsertBackMust(iWatcher->fTrippedWatcherQueries, theWatcherQuery);
		}

	locker_Structure.Release();

	// We now do the write, if any.

	set<Watcher*> touchedWatchers;
	if (iWrittenTuplesCount)
		{
		ZLocker locker_Write(this->GetWriteLock());
		this->pSetTuples(iWrittenTuplesCount, iWrittenTupleIDs, iWrittenTuples, touchedWatchers);

		// Remove from fTrippedPTuples anything which we just wrote, still under the protection of
		// the write lock, but additionally of the structure lock.
		locker_Structure.Acquire();

		const uint64* writtenTupleIDsEnd = iWrittenTupleIDs + iWrittenTuplesCount;
		for (set<PTuple*>::iterator i = iWatcher->fTrippedPTuples.begin();
			i != iWatcher->fTrippedPTuples.end(); /*no inc*/)
			{
			PTuple* thePTuple = *i;
			const uint64 theID = (*i)->fID;

			// iWrittenTupleIDs/iWrittenTuples is sorted by ID.
			// If not then don't call this method.
			const uint64* wasWritten = lower_bound(iWrittenTupleIDs, writtenTupleIDsEnd, theID);
			if (wasWritten != writtenTupleIDsEnd && *wasWritten == theID)
				{
				iWatcher->fTrippedPTuples.erase(i);
				i = iWatcher->fTrippedPTuples.lower_bound(thePTuple);
				}
			else
				{
				++i;
				}
			}
		locker_Structure.Release();
		}

	// Hold the read lock whilst we fetch the values of changed tuples, and the
	// results of changed queries.
	ZLocker locker_Read(this->GetReadLock());

	locker_Structure.Acquire();

	if (size_t changedCount = iWatcher->fTrippedPTuples.size())
		{
		oChangedTupleIDs.resize(changedCount);
		vector<uint64>::iterator iter = oChangedTupleIDs.begin();
		for (set<PTuple*>::iterator
			i = iWatcher->fTrippedPTuples.begin(), theEnd = iWatcher->fTrippedPTuples.end();
			i != theEnd; ++i)
			{
			*iter++ = (*i)->fID;
			}
		iWatcher->fTrippedPTuples.clear();
		locker_Structure.Release();

		oChangedTuples.resize(oChangedTupleIDs.size());
		fTS->GetTuples(oChangedTupleIDs.size(), &oChangedTupleIDs[0], &oChangedTuples[0]);

		locker_Structure.Acquire();
		}

	for (DListEraser<WatcherQuery, WatcherQueryTripped>
		iter = iWatcher->fTrippedWatcherQueries;iter; iter.Advance())
		{
		WatcherQuery* theWatcherQuery = iter.Current();
		PQuery* thePQuery = theWatcherQuery->fPQuery;

		this->pUpdateQueryResults(thePQuery);

		ZUtil_STL::sInsertMust(kDebug,
			oChangedQueries, theWatcherQuery->fRefcon, thePQuery->fResults);

		if (theWatcherQuery->fPrefetch)
			{
			// Need to go through all the results and get the PTuple for each, and add
			// the current watcher to them if they're not already there.
			for (vector<uint64>::iterator i = thePQuery->fResults.begin(),
				theEnd = thePQuery->fResults.end(); i != theEnd; ++i)
				{
				const uint64 theID = *i;
				PTuple* thePTuple = this->pGetPTuple(theID);
				if (ZUtil_STL::sQInsert(thePTuple->fUsingWatchers, iWatcher))
					{
					ZUtil_STL::sInsertMust(kDebug, iWatcher->fPTuples, thePTuple);
					ZAssertStop(kDebug, !ZUtil_STL::sContains(oChangedTupleIDs, theID));
					// oAddedIDs is a vector, but we can unconditionally append to it
					// because the push_back is only called if thePTuple->fUsingWatchers
					// does not already include this watcher.
					oAddedIDs.push_back(theID);
					}
				}
			}
		}

	locker_Structure.Release();

	if (size_t addedSize = oAddedIDs.size())
		{
		oChangedTupleIDs.insert(oChangedTupleIDs.end(), oAddedIDs.begin(), oAddedIDs.end());
		size_t oldSize = oChangedTuples.size();
		oChangedTuples.resize(oldSize + addedSize);
		fTS->GetTuples(addedSize, &oChangedTupleIDs[oldSize], &oChangedTuples[oldSize]);
		}

	// Notify all other affected watchers that they should call Sync
	for (set<Watcher*>::iterator i = touchedWatchers.begin(), theEnd = touchedWatchers.end();
		i != theEnd; ++i)
		{
		Watcher* theWatcher = *i;
		if (theWatcher != iWatcher)
			{
			if (theWatcher->fCallback)
				theWatcher->fCallback(theWatcher->fRefcon);
			}
		}

	locker_Read.Release();
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable Quisitioner

void ZTS_Watchable::Quisitioner_Search(PQuery* iPQuery, const ZTBSpec& iTBSpec, set<uint64>& ioIDs)
	{
	ASSERTUNLOCKED(fMutex_Structure);
	ZMutexLocker locker(fMutex_Structure);

	PSpec* thePSpec = this->pGetPSpec(iTBSpec);

	ioIDs.insert(thePSpec->fResults.begin(), thePSpec->fResults.end());

	thePSpec->fUsingPQueries.insert(iPQuery);
	iPQuery->fPSpecs.insert(thePSpec);
	}

void ZTS_Watchable::Quisitioner_Search(PQuery* iPQuery,
	const ZTBSpec& iTBSpec, vector<uint64>& oIDs)
	{
	ASSERTUNLOCKED(fMutex_Structure);
	ZMutexLocker locker(fMutex_Structure);

	PSpec* thePSpec = this->pGetPSpec(iTBSpec);

	oIDs.insert(oIDs.end(), thePSpec->fResults.begin(), thePSpec->fResults.end());

	thePSpec->fUsingPQueries.insert(iPQuery);
	iPQuery->fPSpecs.insert(thePSpec);
	}

void ZTS_Watchable::Quisitioner_FetchTuples(size_t iCount, const uint64* iIDs, ZTuple* oTuples)
	{ fTS->GetTuples(iCount, iIDs, oTuples); }

ZTuple ZTS_Watchable::Quisitioner_FetchTuple(uint64 iID)
	{
	ZTuple result;
	fTS->GetTuples(1, &iID, &result);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTS_Watchable private

void ZTS_Watchable::pSetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples)
	{
	set<Watcher*> touchedWatchers;
	this->pSetTuples(iCount, iIDs, iTuples, touchedWatchers);
	for (set<Watcher*>::iterator i = touchedWatchers.begin(), theEnd = touchedWatchers.end();
		i != theEnd; ++i)
		{
		Watcher* theWatcher = *i;
		if (theWatcher->fCallback)
			theWatcher->fCallback(theWatcher->fRefcon);
		}
	}

void ZTS_Watchable::pSetTuples(size_t iCount, const uint64* iIDs, const ZTuple* iTuples,
	set<Watcher*>& ioTouchedWatchers)
	{
	vector<ZTuple> oldVector(iCount, ZTuple());
	fTS->GetTuples(iCount, iIDs, &oldVector[0]);

	ZMutexLocker locker(fMutex_Structure);
	for (size_t x = 0; x < iCount; ++x)
		{
		ZTuple oldTuple = oldVector[x];
		ZTuple newTuple = iTuples[x];
		this->pInvalidatePSpecs(oldTuple, newTuple, ioTouchedWatchers);
		this->pInvalidateTuple(iIDs[x], ioTouchedWatchers);
		}
	locker.Release();

	fTS->SetTuples(iCount, iIDs, iTuples);
	}

void ZTS_Watchable::pInvalidatePSpecs(const ZTuple& iOld, const ZTuple& iNew,
	set<Watcher*>& ioTouchedWatchers)
	{
	ASSERTLOCKED(fMutex_Structure);

	#if kDebug_CountInvalidations
	int propNamesInvalidated = 0;
	#endif

	set<PQuery*> toInvalidate;
	for (map<ZTName, set<PSpec*> >::iterator iter = fPropName_To_PSpec.begin(),
		theEnd = fPropName_To_PSpec.end();
		iter != theEnd; ++iter)
		{
		const ZTName& theName = (*iter).first;
		const ZTValue& theOld = iOld.Get(theName);
		const ZTValue& theNew = iNew.Get(theName);
		if (theOld != theNew)
			{
			#if kDebug_CountInvalidations
			size_t beforeSize = toInvalidate.size();
			size_t affectedSpecs = 0;
			#endif

			for (set<PSpec*>::iterator
				iterPSpec = (*iter).second.begin(), theEnd = (*iter).second.end();
				iterPSpec != theEnd; ++iterPSpec)
				{
				PSpec* curPSpec = *iterPSpec;
				if (curPSpec->GetTBSpec().Matches(iOld) != curPSpec->GetTBSpec().Matches(iNew))
					{
					toInvalidate.insert(
						curPSpec->fUsingPQueries.begin(), curPSpec->fUsingPQueries.end());

					#if kDebug_CountInvalidations
					++affectedSpecs;
					#endif
					}
				}
			#if kDebug_CountInvalidations
			if (ZLOG(s, eDebug + 2, "ZTS_Watchable"))
				{
				s << "Examined "
					<< ZString::sFromInt((*iter).second.size())
					<< " specs, invalidated "
					<< ZString::sFromInt(affectedSpecs)
					<< " of them, and thence "
					<< ZString::sFromInt(toInvalidate.size() - beforeSize)
					<< " queries by change to Name: " << theName
					<< "\nOld: " << theOld << "\nNew: " << theNew;
				}
			++propNamesInvalidated;
			#endif
			}
		}

	// We now have a set of all the PQueries affected by this tuple being touched.
	for (set<PQuery*>::iterator i = toInvalidate.begin(), theEnd = toInvalidate.end();
		i != theEnd; ++i)
		{
		PQuery* curPQuery = *i;

		for (DListIterator<WatcherQuery, WatcherQueryUsing>
			iter = curPQuery->fUsingWatcherQueries;iter; iter.Advance())
			{
			WatcherQuery* curWatcherQuery = iter.Current();
			sQInsertBack(curWatcherQuery->fWatcher->fTrippedWatcherQueries, curWatcherQuery);
			ioTouchedWatchers.insert(curWatcherQuery->fWatcher);
			}

		for (set<PSpec*>::iterator
			iterPSpec = curPQuery->fPSpecs.begin(), theEnd = curPQuery->fPSpecs.end();
			iterPSpec != theEnd; ++iterPSpec)
			{
			PSpec* curPSpec = *iterPSpec;
			ZUtil_STL::sEraseMust(kDebug, curPSpec->fUsingPQueries, curPQuery);
			this->pReleasePSpec(curPSpec);
			}
		curPQuery->fPSpecs.clear();

		curPQuery->fResults.clear();
		curPQuery->fResultsValid = false;
		// Give us a crack at actually tossing the PQuery, if it has no watchers.
		this->pReleasePQuery(curPQuery);
		}
	}

void ZTS_Watchable::pInvalidateTuple(uint64 iID, set<Watcher*>& ioTouchedWatchers)
	{
	ASSERTLOCKED(fMutex_Structure);

	map<uint64, PTuple*>::iterator iter = fID_To_PTuple.find(iID);
	if (iter == fID_To_PTuple.end())
		return;

	PTuple* thePTuple = (*iter).second;

	for (set<Watcher*>::iterator
		i = thePTuple->fUsingWatchers.begin(), theEnd = thePTuple->fUsingWatchers.end();
		i != theEnd; ++i)
		{
		(*i)->fTrippedPTuples.insert(thePTuple);
		ioTouchedWatchers.insert(*i);
		}
	}

ZTS_Watchable::PTuple* ZTS_Watchable::pGetPTupleExtant(uint64 iID)
	{
	ASSERTLOCKED(fMutex_Structure);

	map<uint64, PTuple*>::iterator position = fID_To_PTuple.find(iID);
	ZAssertStop(kDebug, position != fID_To_PTuple.end());
	return position->second;
	}

ZTS_Watchable::PTuple* ZTS_Watchable::pGetPTuple(uint64 iID)
	{
	ASSERTLOCKED(fMutex_Structure);

	map<uint64, PTuple*>::iterator position = fID_To_PTuple.lower_bound(iID);
	if (position != fID_To_PTuple.end() && position->first == iID)
		return position->second;

	PTuple* thePTuple = new PTuple(iID);
	fID_To_PTuple.insert(position, map<uint64, PTuple*>::value_type(iID, thePTuple));
	return thePTuple;
	}

void ZTS_Watchable::pReleasePTuple(PTuple* iPTuple)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (not iPTuple->fUsingWatchers.empty())
		return;

	ZUtil_STL::sEraseMust(kDebug, fID_To_PTuple, iPTuple->fID);
	delete iPTuple;
	}

ZTS_Watchable::PSpec* ZTS_Watchable::pGetPSpec(const ZTBSpec& iTBSpec)
	{
	ASSERTLOCKED(fMutex_Structure);

	map<ZTBSpec, PSpec*>::iterator position = fTBSpec_To_PSpec.lower_bound(iTBSpec);
	if (position != fTBSpec_To_PSpec.end() && position->first == iTBSpec)
		return position->second;

	PSpec* thePSpec = new PSpec(iTBSpec);

	fTBSpec_To_PSpec.insert(position, pair<ZTBSpec, PSpec*>(iTBSpec, thePSpec));

	const set<ZTName>& propNames = thePSpec->GetPropNames();

	for (set<ZTName>::const_iterator i = propNames.begin(), theEnd = propNames.end();
		i != theEnd; ++i)
		{
		map<ZTName, set<PSpec*> >::iterator j = fPropName_To_PSpec.find(*i);
		if (fPropName_To_PSpec.end() == j)
			{
			// Nothing yet stored under the propName *i.
			fPropName_To_PSpec.insert(pair<ZTName, set<PSpec*> >(*i, set<PSpec*>()));
			j = fPropName_To_PSpec.find(*i);
			}
		(*j).second.insert(thePSpec);
		}

	set<uint64> empty;
	fTS->Search(iTBSpec, empty, thePSpec->fResults);

	return thePSpec;
	}

void ZTS_Watchable::pReleasePSpec(PSpec* iPSpec)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (not iPSpec->fUsingPQueries.empty())
		return;

	const set<ZTName>& propNames = iPSpec->GetPropNames();

	for (set<ZTName>::const_iterator i = propNames.begin(), theEnd = propNames.end();
		i != theEnd; ++i)
		{
		map<ZTName, set<PSpec*> >::iterator j = fPropName_To_PSpec.find(*i);
		ZAssertStop(kDebug, j != fPropName_To_PSpec.end());
		ZUtil_STL::sEraseMust(kDebug, (*j).second, iPSpec);
		}

	ZUtil_STL::sEraseMust(kDebug, fTBSpec_To_PSpec, iPSpec->GetTBSpec());
	delete iPSpec;
	}

void ZTS_Watchable::pReleasePQuery(PQuery* iPQuery)
	{
	ASSERTLOCKED(fMutex_Structure);

	// Can't toss it if it's in use.
	if (iPQuery->fUsingWatcherQueries)
		return;

	// Don't toss it if it still has valid results.
	if (iPQuery->fResultsValid)
		return;

	for (set<PSpec*>::iterator i = iPQuery->fPSpecs.begin(), theEnd = iPQuery->fPSpecs.end();
		i != theEnd; ++i)
		{
		PSpec* thePSpec = *i;
		ZUtil_STL::sEraseMust(kDebug, thePSpec->fUsingPQueries, iPQuery);
		this->pReleasePSpec(thePSpec);
		}

	ZUtil_STL::sEraseMust(kDebug, fMB_To_PQuery, iPQuery->fMB);
	delete iPQuery;
	}

bool sDumpQuery;

void ZTS_Watchable::pUpdateQueryResults(PQuery* iPQuery)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (iPQuery->fResultsValid)
		return;

	iPQuery->fResultsValid = true;

	if (ZRef<ZTBQueryNode> theNode = iPQuery->fTBQuery.GetNode())
		{
		const ZTime start = ZTime::sSystem();

		fMutex_Structure.Release();

		ASSERTUNLOCKED(fMutex_Structure);

		Quisitioner(this, iPQuery).Query(theNode, nullptr, iPQuery->fResults);

		fMutex_Structure.Acquire();

		double elapsed = ZTime::sSystem() - start;
		if (false && elapsed >= 2.0)
			{
			if (ZLOG(s, eDebug, "ZTS_Watchable"))
				{
				s.Writef("Slow query, %.3fms\n", elapsed * 1000);
//##				s << ZUtil_Strim_Tuple::Format(iPQuery->fTBQuery.AsTuple(), 0, ZYadOptions(true));
				}
			}
		if (false && sDumpQuery)
			{
			// This is a temporary obscure debugging aid. will go soon.
			sDumpQuery = false;
			if (ZLOG(s, eDebug, "ZTS_Watchable"))
				{
//##				s << iPQuery->fTBQuery.AsTuple();
				}
			}
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcherFactory_TS_Watchable

ZTSWatcherFactory_TS_Watchable::ZTSWatcherFactory_TS_Watchable(ZRef<ZTS_Watchable> iTS_Watchable)
:	fTS_Watchable(iTS_Watchable)
	{}

ZRef<ZTSWatcher> ZTSWatcherFactory_TS_Watchable::MakeTSWatcher()
	{ return fTS_Watchable->NewWatcher(); }

} // namespace ZooLib
