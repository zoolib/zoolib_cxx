/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/Relater_Searcher.h"

#include "zoolib/QueryEngine/ResultFromWalker.h"
#include "zoolib/QueryEngine/Transform_Search.h"
#include "zoolib/QueryEngine/Visitor_DoMakeWalker.h"
#include "zoolib/QueryEngine/Walker_Project.h"
#include "zoolib/QueryEngine/Walker_Result.h"
#include "zoolib/QueryEngine/Walker_Rename.h"
#include "zoolib/QueryEngine/Walker_Restrict.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"
#include "zoolib/RelationalAlgebra/Expr_Rel_Concrete.h"
#include "zoolib/RelationalAlgebra/Util_Strim_Rel.h"
#include "zoolib/RelationalAlgebra/Util_Strim_RelHead.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

namespace QE = QueryEngine;
namespace RA = RelationalAlgebra;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Relater_Searcher::ClientQuery

class Relater_Searcher::DLink_ClientQuery_InPQuery
:	public DListLink<ClientQuery, DLink_ClientQuery_InPQuery, kDebug>
	{};

class Relater_Searcher::DLink_ClientQuery_NeedsWork
:	public DListLink<ClientQuery, DLink_ClientQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::ClientQuery
:	public DLink_ClientQuery_InPQuery
,	public DLink_ClientQuery_NeedsWork
	{
public:
	ClientQuery(int64 iRefcon, PQuery* iPQuery)
	:	fRefcon(iRefcon)
	,	fPQuery(iPQuery)
		{}

	int64 const fRefcon;
	PQuery* const fPQuery;
	};

// =================================================================================================
// MARK: - Relater_Searcher::PQuery

class Relater_Searcher::DLink_PQuery_NeedsWork
:	public DListLink<PQuery, DLink_PQuery_NeedsWork, kDebug>
	{};

class Relater_Searcher::PQuery
:	public DLink_PQuery_NeedsWork
	{
public:
	PQuery(ZRef<RA::Expr_Rel> iRel)
	:	fRel(iRel)
		{}

	const ZRef<RA::Expr_Rel> fRel;
	DListHead<DLink_ClientQuery_InPQuery> fClientQuery_InPQuery;
	set<PRegSearch*> fPRegSearch_Used;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Relater_Searcher::PRegSearch

class Relater_Searcher::DLink_PRegSearch_NeedsWork
:	public DListLink<PRegSearch, DLink_PRegSearch_NeedsWork, kDebug>
	{};

class Relater_Searcher::PRegSearch
:	public DLink_PRegSearch_NeedsWork
	{
public:
	PRegSearch() {}

	int64 fRefconInSearcher;
	ConcreteHead fConcreteHead;
	set<PQuery*> fPQuery_Using;
	ZRef<QE::Result> fResult;
	};

// =================================================================================================
// MARK: - Relater_Searcher::Visitor_DoMakeWalker

class Relater_Searcher::Visitor_DoMakeWalker
:	public virtual QE::Visitor_DoMakeWalker
,	public virtual RA::Visitor_Expr_Rel_Concrete
,	public virtual QE::Visitor_Expr_Rel_Search
	{
	typedef QE::Visitor_DoMakeWalker inherited;
public:
	Visitor_DoMakeWalker(ZRef<Relater_Searcher> iSearcher, PQuery* iPQuery)
	:	fSearcher(iSearcher)
	,	fPQuery(iPQuery)
		{}

	virtual void Visit_Expr_Rel_Concrete(const ZRef<RA::Expr_Rel_Concrete>& iExpr)
		{ this->pSetResult(fSearcher->pMakeWalker_Concrete(fPQuery, iExpr->GetConcreteHead())); }

	virtual void Visit_Expr_Rel_Search(const ZRef<QE::Expr_Rel_Search>& iExpr)
		{
		ZUnimplemented();
		}

	ZRef<Relater_Searcher> const fSearcher;
	PQuery* const fPQuery;
	};

// =================================================================================================
// MARK: - Relater_Searcher

void Relater_Searcher::ForceUpdate()
	{ Relater::pTriggerResultsAvailable(); }

Relater_Searcher::Relater_Searcher(ZRef<Searcher> iSearcher)
:	fSearcher(iSearcher)
	{}

Relater_Searcher::~Relater_Searcher()
	{
	for (DListEraser<PRegSearch,DLink_PRegSearch_NeedsWork> eraser = fPRegSearch_NeedsWork;
		eraser; eraser.Advance())
		{}

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{}
	}

bool Relater_Searcher::Intersects(const RelHead& iRelHead)
	{ return true; }

void Relater_Searcher::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		ZRef<RA::Expr_Rel> theRel = iAdded->GetRel();

		const pair<Map_Rel_PQuery::iterator,bool> iterPQueryPair =
			fMap_Rel_PQuery.insert(make_pair(theRel, PQuery(theRel)));

		const Map_Rel_PQuery::iterator& iterPQuery = iterPQueryPair.first;
		PQuery* thePQuery = &iterPQuery->second;

		const int64 theRefcon = iAdded->GetRefcon();

		const pair<map<int64,ClientQuery>::iterator,bool> iterClientQueryPair =
			fMap_Refcon_ClientQuery.insert(
			make_pair(theRefcon, ClientQuery(theRefcon, thePQuery)));
		ZAssert(iterClientQueryPair.second);

		ClientQuery* theClientQuery = &iterClientQueryPair.first->second;
		sInsertBackMust(thePQuery->fClientQuery_InPQuery, theClientQuery);

		if (iterPQueryPair.second)
			{
			// It's a new PQuery, so we'll need to work on it.
			sInsertBackMust(fPQuery_NeedsWork, thePQuery);
			}
		else
			{
			// It's an existing PQuery, so the ClientQuery will need to be worked on.
			sInsertBackMust(fClientQuery_NeedsWork, theClientQuery);
			}
		}

	while (iRemovedCount--)
		{
		int64 theRefcon = *iRemoved++;

		map<int64, ClientQuery>::iterator iterClientQuery =
			fMap_Refcon_ClientQuery.find(theRefcon);

		ZAssertStop(kDebug, iterClientQuery != fMap_Refcon_ClientQuery.end());

		ClientQuery* theClientQuery = &iterClientQuery->second;

		PQuery* thePQuery = theClientQuery->fPQuery;
		sEraseMust(thePQuery->fClientQuery_InPQuery, theClientQuery);
		if (sIsEmpty(thePQuery->fClientQuery_InPQuery))
			{
			// Detach from any depended-upon PRegSearch
			for (set<PRegSearch*>::iterator iterPRegSearch = thePQuery->fPRegSearch_Used.begin();
				iterPRegSearch != thePQuery->fPRegSearch_Used.end(); ++iterPRegSearch)
				{
				PRegSearch* thePRegSearch = *iterPRegSearch;
				sEraseMust(kDebug, thePRegSearch->fPQuery_Using, thePQuery);
				if (thePRegSearch->fPQuery_Using.empty())
					sQInsertBack(fPRegSearch_NeedsWork, thePRegSearch);
				}
			thePQuery->fPRegSearch_Used.clear();

			sQErase(fPQuery_NeedsWork, thePQuery);
			sEraseMust(kDebug, fMap_Rel_PQuery, thePQuery->fRel);
			}

		sQErase(fClientQuery_NeedsWork, theClientQuery);
		fMap_Refcon_ClientQuery.erase(iterClientQuery);
		}

	if (sNotEmpty(fClientQuery_NeedsWork) || sNotEmpty(fPQuery_NeedsWork))
		{
		guard.Release();
		Relater::pTriggerResultsAvailable();
		}
	}

void Relater_Searcher::CollectResults(vector<QueryResult>& oChanged)
	{
	oChanged.clear();

	this->pCollectResultsCalled();

	// This is our "worker" -- the call to CollectResults is (indirectly) where we push
	// registrations down into the Searcher, by driving Walkers that are needed to satisfy
	// invalid PQueries (and thus ClientQueries). At the end of CollectResults
	// we reap anything that's no longer needed.


	ZGuardMtxR guard(fMtxR);

	for (DListEraser<PQuery,DLink_PQuery_NeedsWork> eraser = fPQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		PQuery* thePQuery = eraser.Current();

		guard.Release();

		ZRef<QE::Walker> theWalker = Visitor_DoMakeWalker(this, thePQuery).Do(thePQuery->fRel);

		thePQuery->fResult = QE::sResultFromWalker(theWalker);

		guard.Acquire();

		for (DListIterator<ClientQuery, DLink_ClientQuery_InPQuery>
			iter = thePQuery->fClientQuery_InPQuery; iter; iter.Advance())
			{ sQInsertBack(fClientQuery_NeedsWork, iter.Current()); }
		}

	for (DListEraser<ClientQuery,DLink_ClientQuery_NeedsWork> eraser = fClientQuery_NeedsWork;
		eraser; eraser.Advance())
		{
		ClientQuery* theClientQuery = eraser.Current();
		PQuery* thePQuery = theClientQuery->fPQuery;
		oChanged.push_back(QueryResult(theClientQuery->fRefcon, thePQuery->fResult, null));
		}

	// Remove any unused PRegSearches
	vector<int64> toRemove;
	for (DListEraser<PRegSearch,DLink_PRegSearch_NeedsWork> eraser = fPRegSearch_NeedsWork;
		eraser; eraser.Advance())
		{
		PRegSearch* thePRegSearch = eraser.Current();
		if (thePRegSearch->fPQuery_Using.empty())
			{
			toRemove.push_back(thePRegSearch->fRefconInSearcher);
			sEraseMust(kDebug, fMap_ConcreteHead_PRegSearch, thePRegSearch->fConcreteHead);
			sEraseMust(kDebug, fMap_Refcon_PRegSearch, thePRegSearch->fRefconInSearcher);
			}
		}
	guard.Release();

	if (sNotEmpty(toRemove))
		fSearcher->ModifyRegistrations(nullptr, 0, &toRemove[0], toRemove.size());
	}

void Relater_Searcher::pSearcherResultsAvailable(ZRef<Searcher>)
	{
	vector<SearchResult> theSearchResults;
	fSearcher->CollectResults(theSearchResults);

	ZAcqMtxR acq(fMtxR);
	for (vector<SearchResult>::const_iterator ii = theSearchResults.begin();
		ii != theSearchResults.end(); ++ii)
		{
		if (PRegSearch* thePRS = sPMut(fMap_Refcon_PRegSearch, ii->GetRefcon()))
			thePRS->fResult = ii->GetResult();
		}
	fCnd.Broadcast();

	// We'll pick up the data in *our* owners call to CollectResults
	// Actually, we're possibly blocked down in a call to CollectResults waiting for
	// a registration's values to fill in. So, we should probably pull the data
	// in here and allow anything that is blocked to wake up. Basically we want
	// to shove values into fResult fields and broadcast a cnd.
	Relater::pTriggerResultsAvailable();
	}

ZRef<QueryEngine::Walker> Relater_Searcher::pMakeWalker_Concrete(PQuery* iPQuery,
	const ConcreteHead& iConcreteHead)
	{
	ZGuardMtxR guard(fMtxR);

	PRegSearch* thePRegSearch = nullptr;
	if (PRegSearch** thePRegSearchP = sPMut(fMap_ConcreteHead_PRegSearch, iConcreteHead))
		{ thePRegSearch = *thePRegSearchP; }
	else
		{
		pair<Map_Refcon_PRegSearch::iterator,bool> iterPair =
			fMap_Refcon_PRegSearch.insert(make_pair(fNextRefcon++, PRegSearch()));
		ZAssert(iterPair.second);

		thePRegSearch = &iterPair.first->second;

		sInsertMust(fMap_ConcreteHead_PRegSearch, iConcreteHead, thePRegSearch);

		thePRegSearch->fConcreteHead = iConcreteHead;
		thePRegSearch->fRefconInSearcher = iterPair.first->first;

		const AddedSearch theAS(thePRegSearch->fRefconInSearcher, SearchSpec(iConcreteHead, null));

		guard.Release();

		fSearcher->ModifyRegistrations(&theAS, 1, nullptr, 0);

		while (not thePRegSearch->fResult)
			fCnd.Wait(fMtxR);
		}

	ZAssert(thePRegSearch->fResult);

	return new QE::Walker_Result(thePRegSearch->fResult);
	}

} // namespace ZDataspace
} // namespace ZooLib
