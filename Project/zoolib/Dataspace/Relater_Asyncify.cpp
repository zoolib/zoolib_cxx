// Copyright (c) 2011 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Dataspace/Relater_Asyncify.h"

#include "zoolib/Callable_PMF.h"
#include "zoolib/Log.h"
#include "zoolib/StartOnNewThread.h"
#include "zoolib/Util_STL_map.h"
#include "zoolib/Util_STL_set.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace Dataspace {

using std::map;
using std::set;
using std::vector;

using namespace Util_STL;

// =================================================================================================
#pragma mark - Relater_Asyncify

Relater_Asyncify::Relater_Asyncify(ZP<Relater> iRelater)
:	fRelater(iRelater)
,	fTriggered_Update(false)
,	fNeeds_RelaterCollectResults(false)
	{}

Relater_Asyncify::~Relater_Asyncify()
	{}

void Relater_Asyncify::Initialize()
	{
	Relater::Initialize();
	fRelater->SetCallable_RelaterResultsAvailable(
		sCallable(sWP(this), &Relater_Asyncify::pResultsAvailable));
	}

void Relater_Asyncify::Finalize()
	{
	fRelater->SetCallable_RelaterResultsAvailable(null);
	Relater::Finalize();
	}

bool Relater_Asyncify::Intersects(const RelHead& iRelHead)
	{ return fRelater->Intersects(iRelHead); }

void Relater_Asyncify::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtx acq(fMtx);

	while (iAddedCount--)
		{
		sInsertMust(fPendingAdds, iAdded->GetRefcon(), iAdded->GetRel());
		++iAdded;
		}

	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;
		if (not sQErase(fPendingAdds, theRefcon))
			sInsertMust(fPendingRemoves, theRefcon);

		if (sQErase(fPendingResults, theRefcon))
			{
			// Not sure if this is actually an issue.
			ZLOGTRACE(eDebug);
			}
		}

	this->pTrigger_Update();
	}

void Relater_Asyncify::CollectResults(vector<QueryResult>& oChanged, int64& oChangeCount)
	{
	ZAcqMtx acq(fMtx);
	Relater::pCalled_RelaterCollectResults();

	oChanged.reserve(fPendingResults.size());
	foreacha (entry, fPendingResults)
		{
		// May need to filter entries on our fPendingRemoves list.
		oChanged.push_back(entry.second);
		}
	fPendingResults.clear();

	oChangeCount = 0xDEADBEEF;
	}

void Relater_Asyncify::CrankIt()
	{
	this->pUpdate();
	}

void Relater_Asyncify::Shutdown()
	{
	ZAcqMtx acq(fMtx);
	while (fTriggered_Update)
		fCnd.Wait(fMtx);
	// Fake, to prevent pTrigger_Update from operating
	fTriggered_Update = true;
	}

void Relater_Asyncify::pTrigger_Update()
	{
	ZAcqMtx acq(fMtx);
	if (sGetSet(fTriggered_Update, true))
		return;

	sStartOnNewThread(sCallable(sZP(this), &Relater_Asyncify::pUpdate));
	}

void Relater_Asyncify::pUpdate()
	{
	ZThread::sSleep(2);

	ZAcqMtx acq(fMtx);

	for (;;)
		{
		bool didAnything = false;

		vector<AddedQuery> theAdds;
		theAdds.reserve(fPendingAdds.size());
		foreacha (entry, fPendingAdds)
			theAdds.push_back(AddedQuery(entry.first, entry.second));
		fPendingAdds.clear();

		vector<int64> theRemoves(fPendingRemoves.begin(), fPendingRemoves.end());
		fPendingRemoves.clear();

		if (theAdds.size() || theRemoves.size())
			{
			ZRelMtx rel(fMtx);
			didAnything = true;
			fRelater->ModifyRegistrations(sFirstOrNil(theAdds), theAdds.size(),
				sFirstOrNil(theRemoves), theRemoves.size());
			}

		if (fNeeds_RelaterCollectResults)
			{
			fNeeds_RelaterCollectResults = false;
			ZRelMtx rel(fMtx);

			int64 theChangeCount;
			vector<QueryResult> changes;
			fRelater->CollectResults(changes, theChangeCount);

			if (changes.size())
				{
				didAnything = true;

				{
				ZAcqMtx acq(fMtx);
				foreacha (entry, changes)
					fPendingResults[entry.GetRefcon()] = entry;
				}

				Relater::pTrigger_RelaterResultsAvailable();
				}
			}

		if (not didAnything)
			break;
		}
	fTriggered_Update = false;
	fCnd.Broadcast();
	}

void Relater_Asyncify::pResultsAvailable(ZP<Relater> iRelater)
	{
	ZAcqMtx acq(fMtx);
	fNeeds_RelaterCollectResults = true;
	this->pTrigger_Update();
	}

} // namespace Dataspace
} // namespace ZooLib
