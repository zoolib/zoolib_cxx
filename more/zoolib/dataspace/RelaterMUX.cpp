/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/RelaterMUX.h"

namespace ZooLib {
namespace Dataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace ZUtil_STL;

typedef map<int64,pair<ZRef<QueryEngine::Result>,ZRef<Event> > > Map_Refcon_Result;

// =================================================================================================
// MARK: - RelaterMUX::Relater_Client

class RelaterMUX::Relater_Client
:	public Relater
	{
public:
	Relater_Client(ZRef<RelaterMUX> iMUX)
	:	fMUX(iMUX)
		{}

// From ZCounted via Relater
	virtual void Finalize()
		{ fMUX->pFinalizeRelater_Client(this); }

// From Relater
	virtual bool Intersects(const RelHead& iRelHead)
		{ return fMUX->pIntersects(this, iRelHead); }

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount)
		{ fMUX->pModifyRegistrations(this, iAdded, iAddedCount, iRemoved, iRemovedCount); }

	virtual void CollectResults(vector<QueryResult>& oChanged)
		{
		Relater::pCalled_RelaterCollectResults();
		fMUX->pCollectResults(this, oChanged);
		}

// Our protocol
	void ResultsAvailable()
		{ Relater::pTrigger_RelaterResultsAvailable(); }

	ZRef<RelaterMUX> fMUX;

	map<int64,int64> fMap_ClientToPRefcon;
	Map_Refcon_Result fResults;
	};

// =================================================================================================
// MARK: - RelaterMUX

RelaterMUX::RelaterMUX(ZRef<Relater> iRelater)
:	fRelater(iRelater)
,	fResultsAvailable(false)
,	fNextPRefcon(1)
	{}

RelaterMUX::~RelaterMUX()
	{}

void RelaterMUX::Initialize()
	{
	RelaterFactory::Initialize();
	fRelater->SetCallable_RelaterResultsAvailable(
		sCallable(sWeakRef(this), &RelaterMUX::pResultsAvailable));
	}

void RelaterMUX::Finalize()
	{
	fRelater->SetCallable_RelaterResultsAvailable(null);
	RelaterFactory::Finalize();
	}

ZQ<ZRef<Relater> > RelaterMUX::QCall()
	{
	ZAcqMtxR acq(fMtxR);

	ZRef<Relater_Client> theRelater = new Relater_Client(this);
	fRelater_Clients.insert(theRelater.Get());
	return theRelater;
	}

bool RelaterMUX::pIntersects(ZRef<Relater_Client> iRelater, const RelHead& iRelHead)
	{ return fRelater->Intersects(iRelHead); }

void RelaterMUX::pModifyRegistrations(ZRef<Relater_Client> iRelater,
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	vector<AddedQuery> theAddedQueries;
	theAddedQueries.reserve(iAddedCount);
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		const int64 theClientRefcon = iAdded->GetRefcon();
		const int64 thePRefcon = fNextPRefcon++;

		sInsertMust(kDebug,
			iRelater->fMap_ClientToPRefcon, theClientRefcon, thePRefcon);

		sInsertMust(kDebug,
			fPRefconToClient, thePRefcon, make_pair(iRelater.Get(), theClientRefcon));

		theAddedQueries.push_back(AddedQuery(thePRefcon, iAdded->GetRel()));
		}

	vector<int64> removedQueries;
	removedQueries.reserve(iRemovedCount);
	while (iRemovedCount--)
		removedQueries.push_back(sGetEraseMust(kDebug, iRelater->fMap_ClientToPRefcon, *iRemoved++));

	guard.Release();

	fRelater->ModifyRegistrations(
		sFirstOrNil(theAddedQueries), theAddedQueries.size(),
		sFirstOrNil(removedQueries), removedQueries.size());
	}

void RelaterMUX::pCollectResults(ZRef<Relater_Client> iRelater,
	vector<QueryResult>& oChanged)
	{
	ZGuardMtxR guard(fMtxR);

	vector<QueryResult> changes;
	if (sGetSet(fResultsAvailable, false))
		{
		guard.Release();
		fRelater->CollectResults(changes);
		guard.Acquire();
		}

	foreachi (iterChanges, changes)
		{
		const pair<Relater_Client*,int64>& thePair =
			sGetMust(kDebug, fPRefconToClient, iterChanges->GetRefcon());

		thePair.first->fResults[thePair.second] =
			make_pair(iterChanges->GetResult(), iterChanges->GetEvent());
		}

	oChanged.clear();
	oChanged.reserve(iRelater->fResults.size());
	foreachi (iter, iRelater->fResults)
		oChanged.push_back(QueryResult(iter->first, iter->second.first, iter->second.second));

	iRelater->fResults.clear();
	}

void RelaterMUX::pResultsAvailable(ZRef<Relater> iRelater)
	{
	ZGuardMtxR guard(fMtxR);
	if (not sGetSet(fResultsAvailable, true))
		{
		foreachi (iter, fRelater_Clients)
			(*iter)->ResultsAvailable();
		}
	}

void RelaterMUX::pFinalizeRelater_Client(Relater_Client* iRelater)
	{
	ZGuardMtxR guard(fMtxR);

	if (not iRelater->FinishFinalize())
		return;

	vector<int64> removedQueries;
	removedQueries.reserve(iRelater->fMap_ClientToPRefcon.size());
	foreachi (iter, iRelater->fMap_ClientToPRefcon)
		removedQueries.push_back(iter->second);

	sEraseMust(fRelater_Clients, iRelater);
	delete iRelater;

	guard.Release();

	fRelater->ModifyRegistrations(
		nullptr, 0,
		sFirstOrNil(removedQueries), removedQueries.size());
	}

} // namespace Dataspace
} // namespace ZooLib
