/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/Callable_PMF.h"
#include "zoolib/StartOnNewThread.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/Relater_Asyncify.h"

namespace ZooLib {
namespace Dataspace {

using std::map;
using std::set;
using std::vector;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Relater_Asyncify

Relater_Asyncify::Relater_Asyncify(ZRef<Relater> iRelater)
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
		sCallable(sWeakRef(this), &Relater_Asyncify::pResultsAvailable));
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
	ZAcqMtxR acq(fMtxR);

	while (iAddedCount--)
		{
		if (ZLOGPF(s, eDebug + 1))
			s << "Add: " << iAdded->GetRefcon();
		sInsertMust(fPendingAdds, iAdded->GetRefcon(), iAdded->GetRel());
		++iAdded;
		}

	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;
		if (ZLOGPF(s, eDebug + 1))
			s << "Remove: " << theRefcon;
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

void Relater_Asyncify::CollectResults(vector<QueryResult>& oChanged)
	{
	ZAcqMtxR acq(fMtxR);
	Relater::pCalled_RelaterCollectResults();

	oChanged.reserve(fPendingResults.size());
	foreachi (iter, fPendingResults)
		{
		// May need to filter entries on our fPendingRemoves list.
		oChanged.push_back(iter->second);
		}
	fPendingResults.clear();
	}

void Relater_Asyncify::CrankIt()
	{
	this->pUpdate();
	}

void Relater_Asyncify::Shutdown()
	{
	ZAcqMtxR acq(fMtxR);
	while (fTriggered_Update)
		fCnd.Wait(fMtxR);
	// Fake, to prevent pTrigger_Update from operating
	fTriggered_Update = true;
	}

void Relater_Asyncify::pTrigger_Update()
	{
	ZAcqMtxR acq(fMtxR);
	if (sGetSet(fTriggered_Update, true))
		return;

	sStartOnNewThread(sCallable(sRef(this), &Relater_Asyncify::pUpdate));
	}

void Relater_Asyncify::pUpdate()
	{
	ZThread::sSleep(2);

	ZGuardMtxR guard(fMtxR);

	for (;;)
		{
		bool didAnything = false;

		vector<AddedQuery> theAdds;
		theAdds.reserve(fPendingAdds.size());
		foreachi (iter, fPendingAdds)
			theAdds.push_back(AddedQuery(iter->first, iter->second));
		fPendingAdds.clear();

		vector<int64> theRemoves(fPendingRemoves.begin(), fPendingRemoves.end());
		fPendingRemoves.clear();

		if (theAdds.size() || theRemoves.size())
			{
			guard.Release();
			didAnything = true;
			fRelater->ModifyRegistrations(sFirstOrNil(theAdds), theAdds.size(),
				sFirstOrNil(theRemoves), theRemoves.size());
			guard.Acquire();
			}

		if (fNeeds_RelaterCollectResults)
			{
			fNeeds_RelaterCollectResults = false;
			guard.Release();

			vector<QueryResult> changes;
			fRelater->CollectResults(changes);

			if (changes.size())
				{
				didAnything = true;
				guard.Acquire();

				foreachi (iter, changes)
					fPendingResults[iter->GetRefcon()] = *iter;

				guard.Release();
				Relater::pTrigger_RelaterResultsAvailable();
				}
			guard.Acquire();
			}

		if (not didAnything)
			break;
		}
	fTriggered_Update = false;
	fCnd.Broadcast();
	}

void Relater_Asyncify::pResultsAvailable(ZRef<Relater> iRelater)
	{
	ZGuardMtxR guard(fMtxR);
	fNeeds_RelaterCollectResults = true;
	this->pTrigger_Update();
	}

} // namespace Dataspace
} // namespace ZooLib
