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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZCallOnNewThread.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/ZDataspace_Source_Asyncify.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::set;
using std::vector;

using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Source_Asyncify

Source_Asyncify::Source_Asyncify(ZRef<Source> iSource)
:	fSource(iSource)
,	fTriggered_Update(false)
,	fNeeds_SourceCollectResults(false)
	{}

Source_Asyncify::~Source_Asyncify()
	{}

void Source_Asyncify::Initialize()
	{
	Source::Initialize();
	fSource->SetCallable_ResultsAvailable
		(sCallable(sWeakRef(this), &Source_Asyncify::pResultsAvailable));
	}

void Source_Asyncify::Finalize()
	{
	fSource->SetCallable_ResultsAvailable(null);
	Source::Finalize();
	}

bool Source_Asyncify::Intersects(const RelHead& iRelHead)
	{ return fSource->Intersects(iRelHead); }

void Source_Asyncify::ModifyRegistrations
	(const AddedQuery* iAdded, size_t iAddedCount,
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

void Source_Asyncify::CollectResults(vector<QueryResult>& oChanged)
	{
	ZAcqMtxR acq(fMtxR);
	this->pCollectResultsCalled();

	oChanged.reserve(fPendingResults.size());
	foreachi (iter, fPendingResults)
		{
		// May need to filter entries on our fPendingRemoves list.
		oChanged.push_back(iter->second);
		}
	fPendingResults.clear();
	}

void Source_Asyncify::CrankIt()
	{
	this->pUpdate();
	}

void Source_Asyncify::Shutdown()
	{
	ZAcqMtxR acq(fMtxR);
	while (fTriggered_Update)
		fCnd.Wait(fMtxR);
	// Fake, to prevent pTrigger_Update from operating
	fTriggered_Update = true;
	}

void Source_Asyncify::pTrigger_Update()
	{
	ZAcqMtxR acq(fMtxR);
	if (sGetSet(fTriggered_Update, true))
		return;

	sCallOnNewThread(sCallable(sRef(this), &Source_Asyncify::pUpdate));
	}

void Source_Asyncify::pUpdate()
	{
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
			fSource->ModifyRegistrations(sFirstOrNil(theAdds), theAdds.size(),
				sFirstOrNil(theRemoves), theRemoves.size());
			guard.Acquire();
			}

		if (fNeeds_SourceCollectResults)
			{
			fNeeds_SourceCollectResults = false;
			guard.Release();

			vector<QueryResult> changes;
			fSource->CollectResults(changes);

			if (changes.size())
				{
				didAnything = true;
				guard.Acquire();

				foreachi (iter, changes)
					fPendingResults[iter->GetRefcon()] = *iter;

				guard.Release();
				Source::pTriggerResultsAvailable();
				}
			guard.Acquire();
			}

		if (not didAnything)
			break;
		}
	fTriggered_Update = false;
	fCnd.Broadcast();
	}

void Source_Asyncify::pResultsAvailable(ZRef<Source> iSource)
	{
	ZGuardMtxR guard(fMtxR);
	fNeeds_SourceCollectResults = true;
	this->pTrigger_Update();
	}

} // namespace ZDataspace
} // namespace ZooLib
