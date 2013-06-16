/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/tuplebase/ZTSWatcher_Latent.h"
#include "zoolib/ZUtil_STL.h"

namespace ZooLib {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Latent::PQuery

class ZTSWatcher_Latent::PQuery
	{
	PQuery(); // Not implemented
public:
	PQuery(int64 iRefcon, const ZTBSpec& iTBSpec);

	int64 fRefcon;
	ZTBSpec fTBSpec;
	set<PTuple*> fPTuples;
	set<uint64> fResults;
	};

ZTSWatcher_Latent::PQuery::PQuery(int64 iRefcon, const ZTBSpec& iTBSpec)
:	fRefcon(iRefcon),
	fTBSpec(iTBSpec)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Latent::PTuple

class ZTSWatcher_Latent::PTuple
	{
	PTuple(); // Not implemented
public:
	PTuple(uint64 iID);

	const uint64 fID;
	ZTuple fWrittenValue;
	bool fWritten;
	bool fServerKnown;
	bool fClientKnown;
	set<PQuery*> fPQueries;
	};

ZTSWatcher_Latent::PTuple::PTuple(uint64 iID)
:	fID(iID),
	fWritten(false),
	fServerKnown(false),
	fClientKnown(false)
	{}

// =================================================================================================
#pragma mark -
#pragma mark * ZTSWatcher_Latent

ZTSWatcher_Latent::ZTSWatcher_Latent(ZRef<ZTSWatcher> iTSWatcher)
:	fTSWatcher(iTSWatcher),
	fCallback(nullptr),
	fRefcon(nullptr),
	fMode(eMode_Normal),
	fHasChangesPtr(nullptr)
	{
	fTSWatcher->SetCallback(spCallback, this);
	}

ZTSWatcher_Latent::~ZTSWatcher_Latent()
	{
	if (fTSWatcher && fCallback)
		fTSWatcher->SetCallback(nullptr, nullptr);
	}

bool ZTSWatcher_Latent::AllocateIDs(size_t iCount, uint64& oBaseID, size_t& oCountIssued)
	{
	if (fTSWatcher && fTSWatcher->AllocateIDs(iCount, oBaseID, oCountIssued))
		return true;

	fTSWatcher.Clear();
	return false;
	}

bool ZTSWatcher_Latent::Sync(
	const uint64* iRemovedIDs, size_t iRemovedIDsCount,
	const uint64* iAddedIDs, size_t iAddedIDsCount,
	const int64* iRemovedQueries, size_t iRemovedQueriesCount,
	const AddedQueryCombo* iAddedQueries, size_t iAddedQueriesCount,
	vector<uint64>& oAddedIDs,
	vector<uint64>& oChangedTupleIDs, vector<ZTuple>& oChangedTuples,
	const uint64* iWrittenTupleIDs, const ZTuple* iWrittenTuples, size_t iWrittenTuplesCount,
	map<int64, vector<uint64> >& oChangedQueries)
	{
	ZMutexLocker locker_CallSync(fMutex_CallSync);
	ZMutexLocker locker_Save(fMutex_Save);

	if (!fTSWatcher)
		return false;

	// Take note of tuple registration changes.
	for (size_t x = 0; x < iRemovedIDsCount; ++x)
		{
		const uint64 theID = iRemovedIDs[x];
		PTuple* thePTuple = this->pGetPTupleExtant(theID);
		ZAssertStop(kDebug, thePTuple);
		ZAssertStop(kDebug, thePTuple->fClientKnown);
		thePTuple->fClientKnown = false;
		}

	set<uint64> justAdded;
	for (size_t x = 0; x < iAddedIDsCount; ++x)
		{
		const uint64 theID = iAddedIDs[x];
		PTuple& thePTuple = this->pGetPTuple(theID);
		ZAssertStop(kDebug, !thePTuple.fClientKnown);
		thePTuple.fClientKnown = true;
		justAdded.insert(theID);
		}

	// Similarly, take note of query registration changes. Note that
	// we only record those queries which are 'simple' and can be
	// interpreted locally.
	vector<int64> removedQueries;
	for (size_t x = 0; x < iRemovedQueriesCount; ++x)
		{
		const int64 theRefcon = iRemovedQueries[x];
		map<int64, PQuery>::iterator iterPQuery = fPQueries.find(theRefcon);
		if (iterPQuery != fPQueries.end())
			{
			PQuery& thePQuery = iterPQuery->second;
			for (set<PTuple*>::iterator i = thePQuery.fPTuples.begin();
				i != thePQuery.fPTuples.end(); ++i)
				{
				ZUtil_STL::sEraseMustContain(kDebug, (*i)->fPQueries, &thePQuery);
				}
			fPQueries.erase(iterPQuery);
			}

		removedQueries.push_back(theRefcon);
		}

	vector<ZTSWatcher::AddedQueryCombo> addedQueries;
	for (size_t x = 0; x < iAddedQueriesCount; ++x)
		{
		addedQueries.push_back(iAddedQueries[x]);
		this->pRegisterAQC(addedQueries.back());
		}

	set<PQuery*> changedPQueries;
	if (fMode != eMode_Normal)
		{
		for (size_t x = 0; x < iWrittenTuplesCount; ++x)
			{
			const uint64 theID = iWrittenTupleIDs[x];
			const ZTuple& theTuple = iWrittenTuples[x];

			PTuple& thePTuple = this->pGetPTuple(theID);
			thePTuple.fWrittenValue = theTuple;
			thePTuple.fWritten = true;
			}
		}
	else
		{
		// Only bother with updating thePTuple.fPQueries and vice
		// versa if we're in normal mode -- if we're discarding or
		// saving we're going to be tossing all this work anyway.
		for (size_t x = 0; x < iWrittenTuplesCount; ++x)
			{
			const uint64 theID = iWrittenTupleIDs[x];
			const ZTuple& theTuple = iWrittenTuples[x];

			PTuple& thePTuple = this->pGetPTuple(theID);
			thePTuple.fWrittenValue = theTuple;
			thePTuple.fWritten = true;

			for (set<PQuery*>::iterator i = thePTuple.fPQueries.begin();
				i != thePTuple.fPQueries.end(); ++i)
				{
				ZUtil_STL::sEraseMustContain(kDebug, (*i)->fPTuples, &thePTuple);
				}
			changedPQueries.insert(thePTuple.fPQueries.begin(), thePTuple.fPQueries.end());
			thePTuple.fPQueries.clear();

			for (std::map<int64, PQuery>::iterator i = fPQueries.begin();
				i != fPQueries.end(); ++i)
				{
				PQuery& thePQuery = i->second;
				if (thePQuery.fTBSpec.Matches(theTuple))
					{
					ZUtil_STL::sInsertMustNotContain(kDebug, thePQuery.fPTuples, &thePTuple);
					ZUtil_STL::sInsertMustNotContain(kDebug, thePTuple.fPQueries, &thePQuery);
					changedPQueries.insert(&thePQuery);
					}
				else if (ZUtil_STL::sContains(thePQuery.fResults, theID))
					{
					changedPQueries.insert(&thePQuery);
					}
				}
			}
		}

	// Generate the tuple registration changes to be passed to the server. When a local
	// write occurs on a tuple that's registered with the server we unregister it. A
	// discard will cause such tuples to be re-registered, and their value retrieved.
	set<uint64> additionalAdded;
	vector<uint64> removedIDs;
	vector<uint64> addedIDs;
	vector<uint64> writtenTupleIDs;
	vector<ZTuple> writtenTuples;

	bool hasChanges = false;
	for (map<uint64, PTuple>::iterator i = fPTuples.begin();
		i != fPTuples.end(); /*no inc*/)
		{
		const uint64 theID = i->first;
		PTuple& thePTuple = i->second;
		if (thePTuple.fWritten)
			{
			if (fMode == eMode_Normal)
				{
				hasChanges = true;
				if (thePTuple.fServerKnown)
					{
					thePTuple.fServerKnown = false;
					removedIDs.push_back(theID);
					}

				if (ZUtil_STL::sContains(justAdded, theID))
					additionalAdded.insert(theID);
				}
			else
				{
				thePTuple.fWritten = false;
				// Don't need to clear the reference to this PTuple by PQueries in
				// thePTuple.fPQueries, they will be tossed below.
				thePTuple.fPQueries.clear();
				if (fMode == eMode_Save)
					{
					writtenTupleIDs.push_back(theID);
					writtenTuples.push_back(thePTuple.fWrittenValue);
					}
				}
			}

		if (thePTuple.fWritten)
			{
			++i;
			}
		else
			{
			if (thePTuple.fClientKnown)
				{
				if (!thePTuple.fServerKnown)
					{
					thePTuple.fServerKnown = true;
					addedIDs.push_back(theID);
					}
				++i;
				}
			else
				{
				if (thePTuple.fServerKnown)
					{
					thePTuple.fServerKnown = false;
					removedIDs.push_back(theID);
					}
				fPTuples.erase(i);
				i = fPTuples.lower_bound(theID);
				}
			}
		}

	if (fMode != eMode_Normal)
		{
		for (std::map<int64, PQuery>::iterator i = fPQueries.begin();
			i != fPQueries.end(); ++i)
			{
			i->second.fPTuples.clear();
			changedPQueries.insert(&i->second);
			}
		}

	fMode = eMode_Normal;

	if (fHasChangesPtr)
		{
		*fHasChangesPtr = hasChanges;
		fHasChangesPtr = nullptr;
		}

	fCondition_Save.Broadcast();
	locker_Save.Release();

	ZAssertStop(kDebug, writtenTupleIDs.size() == writtenTuples.size());

	vector<uint64> serverAddedIDs;
	vector<uint64> changedTupleIDs;
	vector<ZTuple> changedTuples;
	map<int64, vector<uint64> > changedQueries;

	if (!fTSWatcher->Sync(
			&removedIDs[0], removedIDs.size(),
			&addedIDs[0], addedIDs.size(),
			&removedQueries[0], removedQueries.size(),
			&addedQueries[0], addedQueries.size(),
			serverAddedIDs,
			changedTupleIDs, changedTuples,
			&writtenTupleIDs[0], &writtenTuples[0], writtenTupleIDs.size(),
			changedQueries))
		{
		fTSWatcher.Clear();
		return false;
		}

	locker_Save.Acquire();

	oAddedIDs.reserve(serverAddedIDs.size());
	for (vector<uint64>::iterator i = serverAddedIDs.begin(); i != serverAddedIDs.end(); ++i)
		{
		const uint64 theID = *i;
		PTuple& thePTuple = this->pGetPTuple(theID);
		ZAssertStop(kDebug, !thePTuple.fServerKnown && !thePTuple.fClientKnown);
		thePTuple.fServerKnown = true;
		thePTuple.fClientKnown = true;
		oAddedIDs.push_back(theID);
		}

	const size_t changedTuplesSize = changedTupleIDs.size();
	ZAssertStop(kDebug, changedTuplesSize == changedTuples.size());

	oChangedTupleIDs.reserve(changedTuplesSize + additionalAdded.size());
	oChangedTuples.reserve(changedTuplesSize + additionalAdded.size());

	for (size_t x = 0; x < changedTuplesSize; ++x)
		{
		const uint64 theID = changedTupleIDs[x];
		if (ZCONFIG_Debug >= kDebug)
			{
			PTuple* thePTuple = this->pGetPTupleExtant(theID);
			ZAssertStop(kDebug, thePTuple);
			ZAssertStop(kDebug, !thePTuple->fWritten);
			ZAssertStop(kDebug, thePTuple->fClientKnown);
			ZAssertStop(kDebug, thePTuple->fServerKnown);
			}

		oChangedTupleIDs.push_back(theID);
		oChangedTuples.push_back(changedTuples[x]);
		}

	// Augment changed tuples with those just written and added by the user.
	for (set<uint64>::const_iterator i = additionalAdded.begin(); i != additionalAdded.end(); ++i)
		{
		const uint64 theID = *i;
		PTuple* thePTuple = this->pGetPTupleExtant(theID);
		ZAssertStop(kDebug, thePTuple);
		ZAssertStop(kDebug, thePTuple->fWritten);
		ZAssertStop(kDebug, thePTuple->fClientKnown);
		ZAssertStop(kDebug, !thePTuple->fServerKnown);

		oChangedTupleIDs.push_back(theID);
		oChangedTuples.push_back(thePTuple->fWrittenValue);
		}

	for (map<int64, vector<uint64> >::const_iterator i = changedQueries.begin();
		i != changedQueries.end(); ++i)
		{
		const int64 theRefcon = i->first;
		map<int64, PQuery>::iterator iterPQuery = fPQueries.find(theRefcon);
		if (iterPQuery == fPQueries.end())
			{
			oChangedQueries.insert(*i);
			}
		else
			{
			PQuery& thePQuery = iterPQuery->second;
			thePQuery.fResults.clear();
			thePQuery.fResults.insert(i->second.begin(), i->second.end());
			changedPQueries.insert(&thePQuery);
			}
		}

	for (set<PQuery*>::iterator i = changedPQueries.begin();
		i != changedPQueries.end(); ++i)
		{
		PQuery& thePQuery = **i;

		vector<uint64>& localResults =
			oChangedQueries.insert(pair<int64, vector<uint64> >
			(thePQuery.fRefcon, vector<uint64>())).first->second;

		localResults.reserve(thePQuery.fResults.size() + thePQuery.fPTuples.size());

		for (set<uint64>::const_iterator
			remoteIter = thePQuery.fResults.begin(),
			remoteEnd = thePQuery.fResults.end();
			remoteIter != remoteEnd; ++remoteIter)
			{
			const uint64 theID = *remoteIter;
			PTuple* thePTuple = this->pGetPTupleExtant(theID);
			if (!thePTuple || !thePTuple->fWritten)
				localResults.push_back(theID);
			}

		for (set<PTuple*>::iterator i = thePQuery.fPTuples.begin();
			i != thePQuery.fPTuples.end(); ++i)
			{
			localResults.push_back((*i)->fID);
			}
		}

	return true;
	}

void ZTSWatcher_Latent::SetCallback(Callback_t iCallback, void* iRefcon)
	{
	ZMutexLocker locker_Save(fMutex_Save);

	fCallback = iCallback;
	fRefcon = iRefcon;
	}

void ZTSWatcher_Latent::Save(bool iWaitForSync)
	{
	ZMutexLocker locker_Save(fMutex_Save);

	if (!fTSWatcher)
		return;

	if (fMode != eMode_Save)
		{
		fMode = eMode_Save;
		if (fCallback)
			fCallback(fRefcon);

		while (iWaitForSync && fMode != eMode_Normal)
			fCondition_Save.Wait(fMutex_Save);
		}
	}

void ZTSWatcher_Latent::Discard(bool iWaitForSync)
	{
	ZMutexLocker locker_Save(fMutex_Save);

	if (!fTSWatcher)
		return;

	if (fMode != eMode_Discard)
		{
		fMode = eMode_Discard;
		if (fCallback)
			fCallback(fRefcon);

		while (iWaitForSync && fMode != eMode_Normal)
			fCondition_Save.Wait(fMutex_Save);

		}
	}

bool ZTSWatcher_Latent::HasChanges(bool iWaitForSync)
	{
	ZMutexLocker locker_Save(fMutex_Save);

	if (!fTSWatcher)
		return false;

	bool hasChanges;
	if (iWaitForSync)
		{
		fHasChangesPtr = &hasChanges;

		if (fCallback)
			fCallback(fRefcon);

		while (fHasChangesPtr)
			fCondition_Save.Wait(fMutex_Save);
		}
	else
		{
		hasChanges = false;
		for (map<uint64, PTuple>::iterator i = fPTuples.begin();
			i != fPTuples.end(); /*no inc*/)
			{
			// const uint64 theID = i->first;
			PTuple& thePTuple = i->second;
			if (thePTuple.fWritten)
				{
				hasChanges = true;
				break;
				}
			}
		}
	return hasChanges;
	}

ZTSWatcher_Latent::PTuple& ZTSWatcher_Latent::pGetPTuple(uint64 iID)
	{
	map<uint64, PTuple>::iterator i = fPTuples.lower_bound(iID);
	if (i != fPTuples.end() && i->first == iID)
		return i->second;

	return fPTuples.insert(i, pair<uint64, PTuple>(iID, PTuple(iID)))->second;
	}

ZTSWatcher_Latent::PTuple* ZTSWatcher_Latent::pGetPTupleExtant(uint64 iID)
	{
	map<uint64, PTuple>::iterator i = fPTuples.lower_bound(iID);
	if (i != fPTuples.end() && i->first == iID)
		return &i->second;
	return nullptr;
	}

void ZTSWatcher_Latent::pRegisterAQC(AddedQueryCombo& ioAQC)
	{
	ioAQC.fPrefetch = false;

	if (not ioAQC.fTBQuery)
		{
		ZAssertStop(kDebug, ioAQC.fMemoryBlock);
		ioAQC.fTBQuery = ZTBQuery(ZStreamRPos_MemoryBlock(ioAQC.fMemoryBlock));
		}

	ZAssertStop(kDebug, !ZUtil_STL::sContains(fPQueries, ioAQC.fRefcon));

	const ZRef<ZTBQueryNode>& theNode = ioAQC.fTBQuery.GetNode();
	if (ZTBQueryNode_Combo* theNode_Combo =
		ZRefDynamicCast<ZTBQueryNode_Combo>(theNode))
		{
		if (theNode_Combo->GetSort().empty())
			{
			const vector<ZTBQueryNode_Combo::Intersection>& theIntersections =
				theNode_Combo->GetIntersections();
			if (theIntersections.size() == 1)
				{
				const ZTBQueryNode_Combo::Intersection& theIntersection = theIntersections.back();
				if (theIntersection.fNodes.size() == 1)
					{
					const ZRef<ZTBQueryNode>& theNode = theIntersection.fNodes.back();
					if (ZRefDynamicCast<ZTBQueryNode_All>(theNode))
						{
						fPQueries.insert(pair<int64, PQuery>(
								ioAQC.fRefcon, PQuery(ioAQC.fRefcon, theIntersection.fFilter)));
						}
					}
				}
			}
		}
	}

void ZTSWatcher_Latent::pCallback()
	{
	ZMutexLocker locker(fMutex_Save);
	if (fCallback)
		fCallback(fRefcon);
	}

void ZTSWatcher_Latent::spCallback(void* iRefcon)
	{ static_cast<ZTSWatcher_Latent*>(iRefcon)->pCallback(); }

} // namespace ZooLib

