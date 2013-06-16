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

#include "zoolib/tuplebase/ZTSoup.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZStringf.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace ZooLib {

using namespace ZUtil_STL;

#define ASSERTLOCKED(a) ZAssertStop(ZTSoup::kDebug, a.IsLocked())
#define ASSERTUNLOCKED(a) ZAssertStop(ZTSoup::kDebug, !a.IsLocked())

// =================================================================================================
/**
\defgroup Tuplesoup
\sa Tuplestore

*/

// =================================================================================================
#pragma mark -
#pragma mark * PSieve

class ZTSoup::DLink_PSieve_Update
:	public DListLink<ZTSoup::PSieve, ZTSoup::DLink_PSieve_Update, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PSieve_Sync
:	public DListLink<ZTSoup::PSieve, ZTSoup::DLink_PSieve_Sync, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PSieve_Changed
:	public DListLink<ZTSoup::PSieve, ZTSoup::DLink_PSieve_Changed, ZTSoup::kDebug>
	{};

class ZTSoup::PSieve
:	public ZTSoup::DLink_PSieve_Update,
	public ZTSoup::DLink_PSieve_Sync,
	public ZTSoup::DLink_PSieve_Changed
	{
public:
	ZRef<ZTSoup> fTSoup;
	ZTBQuery fTBQuery;

	DListHead<DLink_ZTSieve_Using> fTSieves_Using;

	bool fWatcherKnown;

	bool fPrefetch;

	bool fHasResults;

	vector<uint64> fResults_Local_Prior;
	vector<uint64> fResults_Local_Current;
	vector<uint64> fResults_Remote;

	bool fHasDiffs;
	set<uint64> fAdded;
	set<uint64> fRemoved;
	};

// =================================================================================================
#pragma mark -
#pragma mark * PCrouton

class ZTSoup::DLink_PCrouton_Update
:	public DListLink<ZTSoup::PCrouton, ZTSoup::DLink_PCrouton_Update, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PCrouton_Sync
:	public DListLink<ZTSoup::PCrouton, ZTSoup::DLink_PCrouton_Sync, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PCrouton_Changed
:	public DListLink<ZTSoup::PCrouton, ZTSoup::DLink_PCrouton_Changed, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PCrouton_Syncing
:	public DListLink<ZTSoup::PCrouton, ZTSoup::DLink_PCrouton_Syncing, ZTSoup::kDebug>
	{};

class ZTSoup::DLink_PCrouton_Pending
:	public DListLink<ZTSoup::PCrouton, ZTSoup::DLink_PCrouton_Pending, ZTSoup::kDebug>
	{};

class ZTSoup::PCrouton
:	public ZTSoup::DLink_PCrouton_Update,
	public ZTSoup::DLink_PCrouton_Sync,
	public ZTSoup::DLink_PCrouton_Changed,
	public ZTSoup::DLink_PCrouton_Syncing,
	public ZTSoup::DLink_PCrouton_Pending
	{
public:
	ZRef<ZTSoup> fTSoup;
	uint64 fID;

	DListHead<DLink_ZTCrouton_Using> fTCroutons_Using;

	bool fWatcherKnown;

	bool fHasValue;

	ZTuple fValue;

	bool fWrittenLocally;
	bool fHasValue_ForWatcher;

	ZTuple fValue_ForWatcher;
	ZTuple fValue_FromWatcher;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZTSoup

/**
\class ZTSoup
\nosubgrouping
\ingroup Tuplesoup

ZTSoup and its associated classes ZTSieve and ZTCrouton provide an alternative means of working
with data in a tuplestore.

*/

string ZTSoup::sAsString(ELoaded iLoaded)
	{
	switch (iLoaded)
		{
		case eLoaded_Local: return "eLoaded_Local";
		case eLoaded_Update: return "eLoaded_Update";
		case eLoaded_Registration: return "eLoaded_Registration";
		}
	return "!!eLoaded__UNKNOWN!!";
	}

std::string ZTSoup::sAsString(EChanged iChanged)
	{
	switch (iChanged)
		{
		case eChanged_Local: return "eChanged_Local";
		case eChanged_Remote: return "eChanged_Remote";
		case eChanged_Loaded: return "eChanged_Loaded";
		}
	return "!!eChanged__UNKNOWN!!";
	}

ZTSoup::ZTSoup(ZRef<ZTSWatcher> iTSWatcher)
:	fTSWatcher(iTSWatcher),
	fMutex_CallSync("ZTSoup::fMutex_CallSync"),
	fMutex_CallUpdate("ZTSoup::fMutex_CallUpdate"),
	fMutex_Structure("ZTSoup::fMutex_Structure"),
	fCalled_UpdateNeeded(false),
	fCalled_SyncNeeded(false)
	{
	fTSWatcher->SetCallback(pCallback_TSWatcher, this);
	}

ZTSoup::~ZTSoup()
	{
	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "destroying";
	}

bool ZTSoup::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	ZMutexLocker locker_TSWatcher(fMutex_TSWatcher);
	ZRef<ZTSWatcher> theTSWatcher = fTSWatcher;
	if (!theTSWatcher)
		return false;
	locker_TSWatcher.Release();

	if (theTSWatcher->AllocateIDs(iCount, oBaseID, oCountIssued))
		return true;

	locker_TSWatcher.Acquire();
	fTSWatcher.Clear();

	oCountIssued = 0;
	return false;
	}

uint64 ZTSoup::AllocateID()
	{
	ZTime start = ZTime::sSystem();
	uint64 theID;
	size_t countIssued;
	if (!this->AllocateIDs(1, theID, countIssued) || !countIssued)
		return 0;

	if (ZLOG(s, eInfo, "ZTSoup"))
		s.Writef("AllocateID, elapsed: %gms", 1000*(ZTime::sSystem() - start));

	return theID;
	}

void ZTSoup::Set(uint64 iID, const ZTuple& iTuple)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);
	PCrouton* thePCrouton = this->pGetPCrouton(iID);
	this->pSet(locker_Structure, thePCrouton, iTuple);
	}

uint64 ZTSoup::Add(const ZTuple& iTuple)
	{
	uint64 theID = this->AllocateID();
	this->Set(theID, iTuple);
	return theID;
	}

void ZTSoup::Register(ZRef<ZTSieve> iTSieve, const ZTBQuery& iTBQuery)
	{
	this->Register(iTSieve, iTBQuery, true);
	}

void ZTSoup::Register(ZRef<ZTSieve> iTSieve, const ZTBQuery& iTBQuery, bool iPrefetch)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	iTSieve->fJustRegistered = true;

	PSieve* thePSieve;
	map<ZTBQuery, PSieve>::iterator position = fTBQuery_To_PSieve.lower_bound(iTBQuery);
	if (position != fTBQuery_To_PSieve.end() && position->first == iTBQuery)
		{
		thePSieve = &position->second;
		if (iPrefetch && !thePSieve->fPrefetch)
			{
			thePSieve->fPrefetch = true;
			sQInsertBack(fPSieves_Update, thePSieve);
			}
		}
	else
		{
		thePSieve = &fTBQuery_To_PSieve.
			insert(position, pair<ZTBQuery, PSieve>(iTBQuery, PSieve()))->second;

		thePSieve->fTSoup = this;
		thePSieve->fTBQuery = iTBQuery;
		thePSieve->fWatcherKnown = false;
		thePSieve->fPrefetch = iPrefetch;
		thePSieve->fHasResults = false;
		thePSieve->fHasDiffs = false;

		sInsertBackMust(fPSieves_Update, thePSieve);

		this->pTriggerUpdate();
		}

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		{
		s.Writef("Registering against sieve, ID: %p", thePSieve);
		if (!thePSieve->fWatcherKnown)
			s << ", not known to watcher";
		}

	ZTSieve* theTSieve = iTSieve.Get();
	theTSieve->fPSieve = thePSieve;
	sInsertBackMust(thePSieve->fTSieves_Using, theTSieve);

	if (thePSieve->fHasResults)
		{
		locker_Structure.Release();
		this->Loaded(iTSieve, eLoaded_Registration);
		}
	}

void ZTSoup::Register(ZRef<ZTCrouton> iTCrouton, uint64 iID)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	PCrouton* thePCrouton = this->pGetPCrouton(iID);

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		{
		s.Writef("Registering TCrouton, ID: %llX", iID);
		if (!thePCrouton->fWatcherKnown)
			s << ", not known to watcher";
		}

	ZTCrouton* theTCrouton = iTCrouton.Get();
	iTCrouton->fPCrouton = thePCrouton;
	sInsertBackMust(thePCrouton->fTCroutons_Using, theTCrouton);

	sQInsertBack(fPCroutons_Update, thePCrouton);
	sQErase(fPCroutons_Pending, thePCrouton);
	this->pTriggerUpdate();

	if (thePCrouton->fHasValue)
		{
		locker_Structure.Release();
		this->Loaded(iTCrouton, eLoaded_Registration);
		}
	}

void ZTSoup::Register(ZRef<ZTCrouton> iTCrouton)
	{ this->Register(iTCrouton, this->AllocateID()); }

bool ZTSoup::Sync()
	{
	ZMutexLocker locker_CallSync(fMutex_CallSync);
	ZMutexLocker locker_Structure(fMutex_Structure);

	ZMutexLocker locker_TSWatcher(fMutex_TSWatcher);
	ZRef<ZTSWatcher> theTSWatcher = fTSWatcher;
	if (!theTSWatcher)
		return false;
	locker_TSWatcher.Release();

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Sync start";

	fCalled_SyncNeeded = false;

	vector<uint64> removedIDs, addedIDs;

	// We put written tuples into a map, so that they can
	// be extracted in sorted order, required by ZTSWatcher::Sync.
	map<uint64, ZTuple> writtenTupleMap;

	if (sNotEmpty(fPCroutons_Sync))
		{
		removedIDs.reserve(fPCroutons_Sync.Size());
		addedIDs.reserve(fPCroutons_Sync.Size());
		for (DListEraser<PCrouton, DLink_PCrouton_Sync> iter = fPCroutons_Sync;
			iter; iter.Advance())
			{
			PCrouton* thePCrouton = iter.Current();

			if (thePCrouton->fHasValue_ForWatcher)
				{
				thePCrouton->fHasValue_ForWatcher = false;
				writtenTupleMap.insert(
					pair<uint64, ZTuple>(thePCrouton->fID, thePCrouton->fValue_ForWatcher));

				if (!thePCrouton->fWatcherKnown)
					{
					thePCrouton->fWatcherKnown = true;
					addedIDs.push_back(thePCrouton->fID);
					}

				// Put it on fPCroutons_Syncing, which will be transcribed
				// to fCroutons_Pending after we've called our watcher.
				sQInsertBack(fPCroutons_Syncing, thePCrouton);
				}
			else if (thePCrouton->fTCroutons_Using)
				{
				// It's in use.
				if (!thePCrouton->fWatcherKnown)
					{
					// But not known to the watcher.
					thePCrouton->fWatcherKnown = true;
					addedIDs.push_back(thePCrouton->fID);
					}
				}
			else if (thePCrouton->fWatcherKnown)
				{
				// It's not in use, and is known to the watcher.
				thePCrouton->fWatcherKnown = false;
				removedIDs.push_back(thePCrouton->fID);
				thePCrouton->fHasValue = false;
				sQInsertBack(fPCroutons_Update, thePCrouton);
				}
			else
				{
				// It's not in use, and not known to the watcher, so it should have
				// been pulled from the sync list by update and deleted.
#if 0 //##
				if (ZLOG(s, eNotice, "ZTSoup"))
					{
					s << "Got a PCrouton on the sync list that maybe shouldn't be there: "
						<< " On update list? "
						<< (static_cast<DLink_PCrouton_Update*>(thePCrouton)->fNext
							? "yes " : "no ")
						<< sStringf("%llX: ", thePCrouton->fID)
						<< thePCrouton->fValue;
					}
#endif
				}
			}
		}

	vector<int64> removedQueries;
	vector<ZTSWatcher::AddedQueryCombo> addedQueries;

	for (DListEraser<PSieve, DLink_PSieve_Sync> iter = fPSieves_Sync;
		iter; iter.Advance())
		{
		PSieve* thePSieve = iter.Current();

		if (thePSieve->fTSieves_Using)
			{
			// It's in use.
			if (!thePSieve->fWatcherKnown)
				{
				// But not known to the watcher.
				thePSieve->fWatcherKnown = true;

				ZTSWatcher::AddedQueryCombo theCombo;
				theCombo.fRefcon = reinterpret_cast<int64>(thePSieve);
				theCombo.fTBQuery = thePSieve->fTBQuery;
				theCombo.fPrefetch = thePSieve->fPrefetch;
				addedQueries.push_back(theCombo);
				}
			}
		else if (thePSieve->fWatcherKnown)
			{
			// It's not in use, and is known to the watcher.
			thePSieve->fWatcherKnown = false;
			thePSieve->fHasResults = false;
			removedQueries.push_back(reinterpret_cast<int64>(thePSieve));
			sQInsertBack(fPSieves_Update, thePSieve);
			}
		else
			{
			// Shouldn't still be on the sync list if it's not in use and not known to the watcher
#if 0 //##
			if (ZLOG(s, eNotice, "ZTSoup"))
				{
				s << "Got a PSieve on the sync list that maybe shouldn't be there: "
					<< " On update list? "
					<< (static_cast<DLink_PSieve_Update*>(thePSieve)->fNext ? "yes " : "no ")
					<< sStringf("ID: %llX, value: ", reinterpret_cast<int64>(thePSieve))
					<< thePSieve->fTBQuery.AsTuple();
				}
#endif
			}
		}

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Sync releasing lock";
	locker_Structure.Release();

	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;
	writtenTupleIDs.reserve(writtenTupleMap.size());
	writtenTuples.reserve(writtenTupleMap.size());
	for (map<uint64, ZTuple>::const_iterator i = writtenTupleMap.begin();
		i != writtenTupleMap.end(); ++i)
		{
		writtenTupleIDs.push_back(i->first);
		writtenTuples.push_back(i->second);
		}

	vector<uint64> watcherAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	if (!theTSWatcher->Sync(
		sFirstOrNil(removedIDs), removedIDs.size(),
		sFirstOrNil(addedIDs), addedIDs.size(),
		sFirstOrNil(removedQueries), removedQueries.size(),
		sFirstOrNil(addedQueries), addedQueries.size(),
		watcherAddedIDs,
		changedTupleIDs, changedTuples,
		sFirstOrNil(writtenTupleIDs), sFirstOrNil(writtenTuples), writtenTupleIDs.size(),
		changedQueries))
		{
		// Possibly should move fPCroutons_Syncing stuff onto fPCroutons_Pending.
		// Might allow destructor to execute without needing explicit Purge.
		locker_TSWatcher.Acquire();
		fTSWatcher.Clear();
		return false;
		}

	locker_Structure.Acquire();

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Sync acquired lock";


	for (vector<uint64>::iterator iterWatcherAddedIDs = watcherAddedIDs.begin(),
		theEnd = watcherAddedIDs.end();
		iterWatcherAddedIDs != theEnd; ++iterWatcherAddedIDs)
		{
		PCrouton* thePCrouton = this->pGetPCrouton(*iterWatcherAddedIDs);
		ZAssertStop(ZTSoup::kDebug, !thePCrouton->fWatcherKnown);
		thePCrouton->fWatcherKnown = true;
		// Stick it on the pending list, so it will last past
		// the end of the next update.
		sQInsertBack(fPCroutons_Pending, thePCrouton);
		}

	for (vector<uint64>::iterator iterChangedTuples = changedTupleIDs.begin(),
		theEnd = changedTupleIDs.end();
		iterChangedTuples != theEnd; ++iterChangedTuples)
		{
		map<uint64, PCrouton>::iterator iterPCrouton =
			fID_To_PCrouton.find(*iterChangedTuples);

		// We never toss a PCrouton that has not positively been unregistered.
		ZAssertStop(ZTSoup::kDebug, fID_To_PCrouton.end() != iterPCrouton);

		PCrouton* thePCrouton = &(*iterPCrouton).second;
		if (!thePCrouton->fWrittenLocally)
			{
			thePCrouton->fValue_FromWatcher =
				*(changedTuples.begin() + (iterChangedTuples - changedTupleIDs.begin()));

			sQInsertBack(fPCroutons_Changed, thePCrouton);
			}
		}

	for (map<int64, vector<uint64> >::iterator i = changedQueries.begin(),
		theEnd = changedQueries.end();
		i != theEnd; ++i)
		{
		PSieve* thePSieve = reinterpret_cast<PSieve*>((*i).first);
		thePSieve->fResults_Remote.swap((*i).second);
		sQInsertBack(fPSieves_Changed, thePSieve);
		}

	for (DListEraser<PCrouton, DLink_PCrouton_Syncing>
		iter = fPCroutons_Syncing;
		iter; iter.Advance())
		{
		sQInsertBack(fPCroutons_Pending, iter.Current());
		}

	locker_CallSync.Release();

	if (fPSieves_Update || fPSieves_Changed
		|| fPCroutons_Update || fPCroutons_Changed
		|| fPCroutons_Pending)
		{
		this->pTriggerUpdate();
		}

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Sync exit";

	return true;
	}

void ZTSoup::Update()
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	fCalled_UpdateNeeded = false;

	if (!fPSieves_Update && !fPSieves_Changed
		&& !fPCroutons_Update && !fPCroutons_Changed
		&& !fPCroutons_Pending)
		{
		return;
		}

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		{
		s << "Update start";
		}

	for (DListEraser<PCrouton, DLink_PCrouton_Update> iter = fPCroutons_Update;
		iter; iter.Advance())
		{
		PCrouton* thePCrouton = iter.Current();

		if (thePCrouton->fWrittenLocally)
			{
			// A local change has been made, mark it so that Sync knows to
			// send the value to the watcher.
			thePCrouton->fWrittenLocally = false;
			thePCrouton->fHasValue_ForWatcher = true;
			thePCrouton->fValue_ForWatcher = thePCrouton->fValue;
			sQInsertBack(fPCroutons_Sync, thePCrouton);
			}

		if (!thePCrouton->fHasValue_ForWatcher)
			{
			// It's not waiting to be sent to the watcher.
			if (thePCrouton->fTCroutons_Using)
				{
				// It's in use.
				if (!thePCrouton->fWatcherKnown)
					{
					// But not known to the watcher.
					sQInsertBack(fPCroutons_Sync, thePCrouton);
					}
				}
			else if (!fPCroutons_Syncing.Contains(thePCrouton)
				&& !fPCroutons_Pending.Contains(thePCrouton))
				{
				// It's not in use, and it's not syncing or pending.
				if (thePCrouton->fWatcherKnown)
					{
					// It's not in use, not syncing or pending, and is known to the watcher.
					sQInsertBack(fPCroutons_Sync, thePCrouton);
					}
				else
					{
					// It's not in use, not syncing or pending, is not known
					// to the watcher so we can toss it.
					sQErase(fPCroutons_Sync, thePCrouton);
					sQErase(fPCroutons_Changed, thePCrouton);
					if (ZLOG(s, eDebug + 1, "ZTSoup"))
						s.Writef("Deleting PCrouton, ID: %llX", thePCrouton->fID);

					sEraseMust(kDebug, fID_To_PCrouton, thePCrouton->fID);
					}
				}
			}
		}

	for (DListEraser<PSieve, DLink_PSieve_Update> iter = fPSieves_Update;
		iter; iter.Advance())
		{
		PSieve* thePSieve = iter.Current();

		if (thePSieve->fTSieves_Using)
			{
			// It's in use.
			if (!thePSieve->fWatcherKnown)
				{
				// But not known to the watcher.
				sQInsertBack(fPSieves_Sync, thePSieve);
				}
			}
		else if (thePSieve->fWatcherKnown)
			{
			// It's not in use and is known to the watcher.
			sQInsertBack(fPSieves_Sync, thePSieve);
			}
		else
			{
			// It's not in use, is not known to the watcher, so we can toss it.
			sQErase(fPSieves_Sync, thePSieve);
			sQErase(fPSieves_Changed, thePSieve);
			sEraseMust(kDebug, fTBQuery_To_PSieve, thePSieve->fTBQuery);
			}
		}

	// Pick up remotely changed croutons.
	set<ZRef<ZTCrouton> > croutonsChanged;
	set<ZRef<ZTCrouton> > croutonsLoaded;
	for (DListEraser<PCrouton, DLink_PCrouton_Changed>
		iter = fPCroutons_Changed;
		iter; iter.Advance())
		{
		PCrouton* thePCrouton = iter.Current();

		thePCrouton->fValue = thePCrouton->fValue_FromWatcher;

		if (thePCrouton->fHasValue)
			{
			for (DListIterator<ZTCrouton, DLink_ZTCrouton_Using>
				iter = thePCrouton->fTCroutons_Using;
				iter; iter.Advance())
				{ croutonsChanged.insert(iter.Current()); }
			}
		else
			{
			thePCrouton->fHasValue = true;
			for (DListIterator<ZTCrouton, DLink_ZTCrouton_Using>
				iter = thePCrouton->fTCroutons_Using;
				iter; iter.Advance())
				{ croutonsLoaded.insert(iter.Current()); }
			}

		}

	// Pick up remotely changed sieves
	set<ZRef<ZTSieve> > sievesChanged;
	set<ZRef<ZTSieve> > sievesLoaded;
	for (DListEraser<PSieve, DLink_PSieve_Changed> iter = fPSieves_Changed;
		iter; iter.Advance())
		{
		PSieve* thePSieve = iter.Current();

		thePSieve->fResults_Local_Prior.swap(thePSieve->fResults_Local_Current);
		thePSieve->fResults_Remote.swap(thePSieve->fResults_Local_Current);
		thePSieve->fHasDiffs = false;
		thePSieve->fAdded.clear();
		thePSieve->fRemoved.clear();

		// Need to separate stuff that's just loaded from stuff that's changed.
		if (thePSieve->fHasResults)
			{
			for (DListIterator<ZTSieve, DLink_ZTSieve_Using>
				iter = thePSieve->fTSieves_Using;
				iter; iter.Advance())
				{
				ZTSieve* theTSieve = iter.Current();
				theTSieve->fJustRegistered = false;
				sievesChanged.insert(theTSieve);
				}
			}
		else
			{
			thePSieve->fHasResults = true;
			for (DListIterator<ZTSieve, DLink_ZTSieve_Using>
				iter = thePSieve->fTSieves_Using;
				iter; iter.Advance())
				{
				ZTSieve* theTSieve = iter.Current();
				theTSieve->fJustRegistered = false;
				sievesLoaded.insert(theTSieve);
				}
			}
		}

	if (fPCroutons_Pending)
		{
		if (ZLOG(s, eDebug + 1, "ZTSoup"))
			s.Writef("Moving %zu croutons from pending to update", fPCroutons_Pending.Size());

		for (DListEraser<PCrouton, DLink_PCrouton_Pending>
			iter = fPCroutons_Pending;
			iter; iter.Advance())
			{
			sQInsertBack(fPCroutons_Update, iter.Current());
			}

		this->pTriggerUpdate();
		}

	if (sNotEmpty(fPCroutons_Sync) || sNotEmpty(fPSieves_Sync))
		this->pTriggerSync();

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Update releasing lock";

	locker_Structure.Release();

	this->Updated(sievesLoaded, sievesChanged, croutonsLoaded, croutonsChanged);

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		s << "Update exit";
	}

void ZTSoup::Purge()
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);
	if (fTSWatcher)
		{
		// We have a ZTSWatcher, so our connection is good, and we should rely on
		// our owner having done a proper tear down.
		return;
		}

	for (map<ZTBQuery, PSieve>::iterator i = fTBQuery_To_PSieve.begin();
		i != fTBQuery_To_PSieve.end(); ++i)
		{
		PSieve* thePSieve = &i->second;

		ZAssertStop(ZTSoup::kDebug, !thePSieve->fTSieves_Using);

		sQErase(fPSieves_Update, thePSieve);
		sQErase(fPSieves_Sync, thePSieve);
		sQErase(fPSieves_Changed, thePSieve);
		}
	fTBQuery_To_PSieve.clear();

	for (map<uint64, PCrouton>::iterator i = fID_To_PCrouton.begin();
		i != fID_To_PCrouton.end(); ++i)
		{
		PCrouton* thePCrouton = &i->second;

		ZAssertStop(ZTSoup::kDebug, !thePCrouton->fTCroutons_Using);

		sQErase(fPCroutons_Update, thePCrouton);
		sQErase(fPCroutons_Sync, thePCrouton);
		sQErase(fPCroutons_Changed, thePCrouton);
		sQErase(fPCroutons_Syncing, thePCrouton);
		sQErase(fPCroutons_Pending, thePCrouton);
		}
	fID_To_PCrouton.clear();
	}

void ZTSoup::Updated(
	const set<ZRef<ZTSieve> >& iSievesLoaded,
	const set<ZRef<ZTSieve> >& iSievesChanged,
	const set<ZRef<ZTCrouton> >& iCroutonsLoaded,
	const set<ZRef<ZTCrouton> >& iCroutonsChanged)
	{
	this->Loaded(iSievesLoaded, eLoaded_Update);
	this->Changed(iSievesChanged, eChanged_Remote);

	this->Loaded(iCroutonsLoaded, eLoaded_Update);
	this->Changed(iCroutonsChanged, eChanged_Remote);
	}

void ZTSoup::Loaded(const set<ZRef<ZTSieve> >& iTSieves, ELoaded iLoaded)
	{
	for (set<ZRef<ZTSieve> >::const_iterator i = iTSieves.begin(),
		theEnd = iTSieves.end(); i != theEnd; ++i)
		{
		this->Loaded(*i, iLoaded);
		}
	}

void ZTSoup::Changed(const set<ZRef<ZTSieve> >& iTSieves, EChanged iChanged)
	{
	for (set<ZRef<ZTSieve> >::const_iterator i = iTSieves.begin(),
		theEnd = iTSieves.end(); i != theEnd; ++i)
		{
		this->Changed(*i, iChanged);
		}
	}

void ZTSoup::Loaded(const set<ZRef<ZTCrouton> >& iTCroutons, ELoaded iLoaded)
	{
	for (set<ZRef<ZTCrouton> >::const_iterator i = iTCroutons.begin(),
		theEnd = iTCroutons.end(); i != theEnd; ++i)
		{
		this->Loaded(*i, iLoaded);
		}
	}

void ZTSoup::Changed(const set<ZRef<ZTCrouton> >& iTCroutons, EChanged iChanged)
	{
	for (set<ZRef<ZTCrouton> >::const_iterator i = iTCroutons.begin(),
		theEnd = iTCroutons.end(); i != theEnd; ++i)
		{
		this->Changed(*i, iChanged);
		}
	}

void ZTSoup::Loaded(const ZRef<ZTSieve>& iTSieve, ELoaded iLoaded)
	{
	try { iTSieve->Loaded(iLoaded); }
	catch (...) {}
	}

void ZTSoup::Changed(const ZRef<ZTSieve>& iTSieve, EChanged iChanged)
	{
	try { iTSieve->Changed(iChanged); }
	catch (...) {}
	}

void ZTSoup::Loaded(const ZRef<ZTCrouton>& iTCrouton, ELoaded iLoaded)
	{
	try { iTCrouton->Loaded(iLoaded); }
	catch (...) {}
	}

void ZTSoup::Changed(const ZRef<ZTCrouton>& iTCrouton, EChanged iChanged)
	{
	try { iTCrouton->Changed(iChanged); }
	catch (...) {}
	}

void ZTSoup::pCallback_TSWatcher(void* iRefcon)
	{
	ZTSoup* theTSoup = static_cast<ZTSoup*>(iRefcon);

	ASSERTUNLOCKED(theTSoup->fMutex_Structure);

	ZMutexLocker locker(theTSoup->fMutex_Structure);

	theTSoup->pTriggerSync();
	}

void ZTSoup::pDisposingTSieve(ZTSieve* iTSieve)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	PSieve* thePSieve = iTSieve->fPSieve;

	sEraseMust(thePSieve->fTSieves_Using, iTSieve);

	if (!thePSieve->fTSieves_Using)
		{
		sQInsertBack(fPSieves_Update, thePSieve);
		this->pTriggerUpdate();
		}
	}

void ZTSoup::pDisposingTCrouton(ZTCrouton* iTCrouton)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	PCrouton* thePCrouton = iTCrouton->fPCrouton;

	if (ZLOG(s, eDebug + 1, "ZTSoup"))
		{
		s.Writef("Disposing TCrouton, ID: %llX", thePCrouton->fID);
		}

	sEraseMust(thePCrouton->fTCroutons_Using, iTCrouton);

	if (!thePCrouton->fTCroutons_Using)
		{
		sQErase(fPCroutons_Update, thePCrouton);
		this->pTriggerUpdate();
		}
	}

bool ZTSoup::pSieveIsLoaded(ZTSieve* iTSieve)
	{
	ZMutexLocker locker(fMutex_Structure);

	return iTSieve->fPSieve->fHasResults;
	}

const std::vector<uint64>& ZTSoup::pSieveGet(ZTSieve* iTSieve)
	{
	ZMutexLocker locker(fMutex_Structure);
	return iTSieve->fPSieve->fResults_Local_Current;
	}

void ZTSoup::pSieveGet(ZTSieve* iTSieve, vector<uint64>& oCurrent)
	{
	ZMutexLocker locker(fMutex_Structure);

	if (iTSieve->fPSieve->fHasResults)
		oCurrent = iTSieve->fPSieve->fResults_Local_Current;
	else
		oCurrent.clear();
	}

void ZTSoup::pSieveGet(ZTSieve* iTSieve, set<uint64>& oCurrent)
	{
	ZMutexLocker locker(fMutex_Structure);

	oCurrent.clear();
	oCurrent.insert(iTSieve->fPSieve->fResults_Local_Current.begin(),
		iTSieve->fPSieve->fResults_Local_Current.end());
	}

void ZTSoup::pSieveGetAdded(ZTSieve* iTSieve, set<uint64>& oAdded)
	{
	ZMutexLocker locker(fMutex_Structure);

	if (iTSieve->fJustRegistered)
		{
		oAdded.clear();
		oAdded.insert(iTSieve->fPSieve->fResults_Local_Current.begin(),
			iTSieve->fPSieve->fResults_Local_Current.end());
		}
	else
		{
		this->pCheckSieveDiffs(iTSieve->fPSieve);
		oAdded = iTSieve->fPSieve->fAdded;
		}
	}

void ZTSoup::pSieveGetRemoved(ZTSieve* iTSieve, set<uint64>& oRemoved)
	{
	ZMutexLocker locker(fMutex_Structure);

	if (iTSieve->fJustRegistered)
		{
		oRemoved.clear();
		}
	else
		{
		this->pCheckSieveDiffs(iTSieve->fPSieve);
		oRemoved = iTSieve->fPSieve->fRemoved;
		}
	}

void ZTSoup::pCheckSieveDiffs(PSieve* iPSieve)
	{
	ASSERTLOCKED(fMutex_Structure);

	if (not iPSieve->fHasDiffs)
		{
		iPSieve->fHasDiffs = true;
		ZAssertStop(kDebug, iPSieve->fAdded.empty());
		ZAssertStop(kDebug, iPSieve->fRemoved.empty());

		set<uint64> current(
			iPSieve->fResults_Local_Current.begin(), iPSieve->fResults_Local_Current.end());

		set<uint64> prior(
			iPSieve->fResults_Local_Prior.begin(), iPSieve->fResults_Local_Prior.end());

		set_difference(current.begin(), current.end(),
					prior.begin(), prior.end(),
					inserter(iPSieve->fAdded, iPSieve->fAdded.end()));

		set_difference(prior.begin(), prior.end(),
					current.begin(), current.end(),
					inserter(iPSieve->fRemoved, iPSieve->fRemoved.end()));
		}
	}

bool ZTSoup::pCroutonIsLoaded(ZTCrouton* iTCrouton)
	{
	ZMutexLocker locker(fMutex_Structure);

	return iTCrouton->fPCrouton->fHasValue;
	}

ZTuple ZTSoup::pCroutonGet(ZTCrouton* iTCrouton)
	{
	ZMutexLocker locker(fMutex_Structure);

	if (iTCrouton->fPCrouton->fHasValue)
		return iTCrouton->fPCrouton->fValue;

	return ZTuple();
	}

ZTSoup::PCrouton* ZTSoup::pGetPCrouton(uint64 iID)
	{
	ASSERTLOCKED(fMutex_Structure);

	map<uint64, PCrouton>::iterator position = fID_To_PCrouton.lower_bound(iID);
	if (position != fID_To_PCrouton.end() && position->first == iID)
		return &position->second;

	PCrouton* thePCrouton = &fID_To_PCrouton.
		insert(position, pair<uint64, PCrouton>(iID, PCrouton()))->second;

	thePCrouton->fTSoup = this;
	thePCrouton->fID = iID;
	thePCrouton->fWatcherKnown = false;
	thePCrouton->fHasValue = false;
	thePCrouton->fWrittenLocally = false;
	thePCrouton->fHasValue_ForWatcher = false;

	return thePCrouton;
	}

void ZTSoup::pSetCroutonFromTCrouton(ZTCrouton* iTCrouton, const ZTuple& iTuple)
	{
	ZMutexLocker locker_CallUpdate(fMutex_CallUpdate);
	ZMutexLocker locker_Structure(fMutex_Structure);

	// There must be at least one ZTCrouton using the PCrouton, or else how were we called?
	ZAssertStop(kDebug, iTCrouton->fPCrouton->fTCroutons_Using);
	this->pSet(locker_Structure, iTCrouton->fPCrouton, iTuple);
	}

void ZTSoup::pSet(ZMutexLocker& iLocker_Structure, PCrouton* iPCrouton, const ZTuple& iTuple)
	{
	ASSERTLOCKED(fMutex_CallUpdate);
	ASSERTLOCKED(fMutex_Structure);

	bool hadValue = iPCrouton->fHasValue;
	iPCrouton->fHasValue = true;
	iPCrouton->fValue = iTuple;
	iPCrouton->fWrittenLocally = true;

	sQInsertBack(fPCroutons_Update, iPCrouton);

	this->pTriggerUpdate();

	set<ZRef<ZTCrouton> > localTCroutons;

	for (DListIterator<ZTCrouton, DLink_ZTCrouton_Using>
		iter = iPCrouton->fTCroutons_Using; iter; iter.Advance())
		{ localTCroutons.insert(iter.Current());}

	iLocker_Structure.Release();

	if (hadValue)
		this->Changed(localTCroutons, eChanged_Local);
	else
		this->Loaded(localTCroutons, eLoaded_Local);
	}

void ZTSoup::pTriggerUpdate()
	{
	ASSERTLOCKED(fMutex_Structure);

	if (!fCalled_UpdateNeeded)
		{
		fCalled_UpdateNeeded = true;
		this->UpdateNeeded();
		}
	}

void ZTSoup::pTriggerSync()
	{
	ASSERTLOCKED(fMutex_Structure);

	if (!fCalled_SyncNeeded)
		{
		fCalled_SyncNeeded = true;
		this->SyncNeeded();
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSieve

ZTSieve::ZTSieve()
:	fPSieve(nullptr),
	fJustRegistered(false)
	{}

ZTSieve::~ZTSieve()
	{
	if (fPSieve)
		fPSieve->fTSoup->pDisposingTSieve(this);
	}

ZRef<ZTSoup> ZTSieve::GetTSoup()
	{ return fPSieve->fTSoup; }

ZTBQuery ZTSieve::GetTBQuery()
	{ return fPSieve->fTBQuery; }

bool ZTSieve::IsLoaded()
	{ return fPSieve->fTSoup->pSieveIsLoaded(this); }

const std::vector<uint64>& ZTSieve::Get()
	{ return fPSieve->fTSoup->pSieveGet(this); }

void ZTSieve::Get(vector<uint64>& oResults)
	{ fPSieve->fTSoup->pSieveGet(this, oResults); }

void ZTSieve::Get(set<uint64>& oResults)
	{ fPSieve->fTSoup->pSieveGet(this, oResults); }

void ZTSieve::GetAdded(set<uint64>& oAdded)
	{ fPSieve->fTSoup->pSieveGetAdded(this, oAdded); }

void ZTSieve::GetRemoved(set<uint64>& oRemoved)
	{ fPSieve->fTSoup->pSieveGetRemoved(this, oRemoved); }

void ZTSieve::Loaded(ZTSoup::ELoaded iLoaded)
	{ this->Changed(ZTSoup::eChanged_Loaded); }

void ZTSieve::Changed(ZTSoup::EChanged iChanged)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTCrouton

ZTCrouton::ZTCrouton()
:	fPCrouton(nullptr)
	{}

ZTCrouton::~ZTCrouton()
	{
	if (fPCrouton)
		fPCrouton->fTSoup->pDisposingTCrouton(this);
	}

ZRef<ZTSoup> ZTCrouton::GetTSoup()
	{ return fPCrouton->fTSoup; }

uint64 ZTCrouton::GetID()
	{ return fPCrouton->fID; }

bool ZTCrouton::IsLoaded()
	{ return fPCrouton->fTSoup->pCroutonIsLoaded(this); }

ZTuple ZTCrouton::Get()
	{ return fPCrouton->fTSoup->pCroutonGet(this); }

void ZTCrouton::Set(const ZTuple& iTuple)
	{ fPCrouton->fTSoup->pSetCroutonFromTCrouton(this, iTuple); }

void ZTCrouton::Loaded(ZTSoup::ELoaded iLoaded)
	{ this->Changed(ZTSoup::eChanged_Loaded); }

void ZTCrouton::Changed(ZTSoup::EChanged iChanged)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTCrouton_Bowl

ZTCrouton_Bowl::ZTCrouton_Bowl()
	{}

void ZTCrouton_Bowl::Changed(ZTSoup::EChanged iChanged)
	{
	if (ZRef<ZTBowl> theBowl = fTBowl)
		theBowl->CroutonChanged(this, iChanged);
	}

ZRef<ZTBowl> ZTCrouton_Bowl::GetBowl() const
	{ return fTBowl; }

// =================================================================================================
#pragma mark -
#pragma mark * ZTBowl

ZTBowl::ZTBowl()
	{}

ZTBowl::~ZTBowl()
	{
	for (vector<ZRef<ZTCrouton> >::iterator i = fTCroutons.begin(); i != fTCroutons.end(); ++i)
		(*i).StaticCast<ZTCrouton_Bowl>()->fTBowl.Clear();
	fTCroutons.clear();
	}

void ZTBowl::Changed(ZTSoup::EChanged iChanged)
	{
	set<uint64> removedIDs;
	this->GetRemoved(removedIDs);

	if (!removedIDs.empty())
		{
		for (vector<ZRef<ZTCrouton> >::iterator i = fTCroutons.begin();
			i != fTCroutons.end(); /*no inc*/)
			{
			if (sContains(removedIDs, (*i)->GetID()))
				{
				ZRef<ZTCrouton> theTCrouton = *i;
				if (ZLOG(s, eDebug, "ZTBowl"))
					{
					s << "Changed, removing TCrouton, ID: "
						<< sStringf("%llX", theTCrouton->GetID())
						<< ", Address: " << sStringf("%p", theTCrouton.Get());
					}

				(*i).StaticCast<ZTCrouton_Bowl>()->fTBowl.Clear();
				i = fTCroutons.erase(i);
				}
			else
				{
				++i;
				}
			}
		}

	ZRef<ZTBowl> selfRef = this;
	set<uint64> addedIDs;
	this->GetAdded(addedIDs);
	for (set<uint64>::const_iterator i = addedIDs.begin(); i != addedIDs.end(); ++i)
		{
		ZRef<ZTCrouton_Bowl> newCrouton = this->MakeCrouton();
		newCrouton->fTBowl = selfRef;
		fTCroutons.push_back(newCrouton);

		if (ZLOG(s, eDebug, "ZTBowl"))
			{
			s << "Changed, added TCrouton, ID: " << sStringf("%llX", *i)
				<< ", Address: " << sStringf("%p", newCrouton.Get());
			}

		this->GetTSoup()->Register(newCrouton, *i);
		}
	}

ZRef<ZTCrouton_Bowl> ZTBowl::MakeCrouton()
	{ return new ZTCrouton_Bowl; }

void ZTBowl::CroutonChanged(ZRef<ZTCrouton> iCrouton, ZTSoup::EChanged iChanged)
	{}

const vector<ZRef<ZTCrouton> >& ZTBowl::GetCroutons()
	{ return fTCroutons; }

} // namespace ZooLib

