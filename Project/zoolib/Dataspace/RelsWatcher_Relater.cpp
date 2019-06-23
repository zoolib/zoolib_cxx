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

#include "zoolib/Dataspace/RelsWatcher_Relater.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/Log.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using namespace Util_STL;

using std::vector;

// =================================================================================================
#pragma mark - Registration

class RelsWatcher_Relater::Registration
:	public ZCounted
	{
public:
	Registration(
		const ZP<RelsWatcher_Relater>& iRWR,
		const ZP<RelsWatcher::Callable_Changed>& iCallable,
		const ZP<Expr_Rel>& iRel)
	:	fRefcon(0)
	,	fWeakRef_RWR(iRWR)
	,	fCallable(iCallable)
	,	fRel(iRel)
		{}

	virtual ~Registration()
		{}

	void Finalize()
		{
		if (ZP<RelsWatcher_Relater> theRWR = fWeakRef_RWR)
			theRWR->pFinalize(this);
		else
			ZCounted::Finalize();
		}

	int64 fRefcon;

	const WP<RelsWatcher_Relater> fWeakRef_RWR;
	const ZP<RelsWatcher::Callable_Changed> fCallable;
	const ZP<Expr_Rel> fRel;

	ZP<Dataspace::ResultDeltas> fResultDeltas;
	ZP<QueryEngine::Result> fResult;
	};

// =================================================================================================
#pragma mark - RelsWatcher_Relater

RelsWatcher_Relater::RelsWatcher_Relater(const ZP<Relater>& iRelater,
	const ZP<Callable_NeedsUpdate>& iCallable_NeedsUpdate)
:	fRelater(iRelater)
,	fCallable_NeedsUpdate(iCallable_NeedsUpdate)
,	fNextRefcon(1000)
	{}

void RelsWatcher_Relater::Initialize()
	{
	inherited::Initialize();

	fRelater->SetCallable_RelaterResultsAvailable(
		sCallable(sWeakRef(this), &RelsWatcher_Relater::pCallback_Relater));
	}

void RelsWatcher_Relater::Finalize()
	{
	// Push pending changes into our relater. We may actually need to do more, either
	// registrations keeping us alive, or forcibly disconnecting extant registrations.
	this->Update();

	inherited::Finalize();
	}

ZQ<ZP<ZCounted> > RelsWatcher_Relater::QCall(
	const ZP<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZP<Expr_Rel>& iRel)
	{
	ZP<Registration> theR = new Registration(this, iCallable_Changed, iRel);

	ZAcqMtx acq(fMtx);

	sInsertMust(fToAdd, theR.Get());

	if (not fCalled_NeedsUpdate())
		{
		ZRelMtx rel(fMtx);
		sCall(fCallable_NeedsUpdate);
		}

	return ZP<ZCounted>(theR);
	}

void RelsWatcher_Relater::Update()
	{
	{
	ZAcqMtx acq(fMtx);
	fCalled_NeedsUpdate.Reset();

	if (sNotEmpty(fToAdd) || sNotEmpty(fToRemove))
		{
		vector<AddedQuery> added;
		foreacha (rr, fToAdd)
			{
			const int64 theRefcon = fNextRefcon++;
			rr->fRefcon = theRefcon;
			sSet(fMap_RefconToRegistrationX, theRefcon, rr);
			added.push_back(AddedQuery(theRefcon, rr->fRel));
			}
		fToAdd.clear();

		vector<int64> removed(fToRemove.begin(), fToRemove.end());
		fToRemove.clear();

		ZRelMtx rel(fMtx);
		fRelater->ModifyRegistrations(
			sFirstOrNil(added), added.size(),
			sFirstOrNil(removed), removed.size());
		}
	}

	// Pick up any results
	int64 theChangeCount = -1;
	vector<QueryResult> theQueryResults;
	fRelater->CollectResults(theQueryResults, theChangeCount);

	if (sNotEmpty(theQueryResults))
		{
		// This vector must be outside the mutex acq, so any reg that goes out of scope
		// does not finalize and attempt to double-acquire the mutex. 
		vector<ZP<Registration> > changes;
		ZAcqMtx acq(fMtx);
		foreacha (entry, theQueryResults)
			{
			Map_RefconToRegistrationX::iterator iterRegistration =
				fMap_RefconToRegistrationX.find(entry.GetRefcon());

			if (fMap_RefconToRegistrationX.end() == iterRegistration)
				continue;

			ZP<Registration> theRegistration = iterRegistration->second;
			theRegistration->fResultDeltas = entry.GetResultDeltas();
			theRegistration->fResult = entry.GetResult();
			changes.push_back(theRegistration);
			}

		ZRelMtx rel(fMtx);
		foreacha (rr, changes)
			sCall(rr->fCallable, rr, theChangeCount, rr->fResult, rr->fResultDeltas);
		}
	}

void RelsWatcher_Relater::pCallback_Relater(ZP<Relater> iRelater)
	{
	ZAcqMtx acq(fMtx);
	if (not fCalled_NeedsUpdate())
		{
		ZRelMtx rel(fMtx);
		sCall(fCallable_NeedsUpdate);
		}
	}

void RelsWatcher_Relater::pFinalize(Registration* iRegistration)
	{
	ZAcqMtx acq(fMtx);

	if (not iRegistration->FinishFinalize())
		return;

	if (sQErase(fToAdd, iRegistration))
		{
		delete iRegistration;
		}
	else
		{
		int64 const theRefcon = iRegistration->fRefcon;

		sEraseMust(fMap_RefconToRegistrationX, theRefcon);

		sInsertMust(fToRemove, theRefcon);

		if (not fCalled_NeedsUpdate())
			{
			ZRelMtx rel(fMtx);
			sCall(fCallable_NeedsUpdate);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
