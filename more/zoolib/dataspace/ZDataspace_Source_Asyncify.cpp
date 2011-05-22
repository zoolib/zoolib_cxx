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
#include "zoolib/ZLog.h"
#include "zoolib/ZWorker_Callable.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/ZDataspace_Source_Asyncify.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Asyncify

Source_Asyncify::Source_Asyncify(ZRef<Source> iSource)
:	fSource(iSource)
,	fCallable_Source(MakeCallable(this, &Source_Asyncify::pResultsAvailable))
,	fTriggered_Update(false)
,	fNeeds_SourceCollectResults(false)
	{
	fSource->SetCallable_ResultsAvailable(fCallable_Source);
	}

Source_Asyncify::~Source_Asyncify()
	{
	fSource->SetCallable_ResultsAvailable(null);
	}

bool Source_Asyncify::Intersects(const RelHead& iRelHead)
	{ return fSource->Intersects(iRelHead); }

void Source_Asyncify::ModifyRegistrations(
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZAcqMtxR acq(fMtxR);

	while (iAddedCount--)
		{
		if (ZLOGPF(s, eDebug + 1))
			s << "Add: " << iAdded->GetRefcon();
		ZUtil_STL::sInsertMustNotContain(1, fPendingAdds, iAdded->GetRefcon(), iAdded->GetRel());
		++iAdded;
		}

	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;
		if (ZLOGPF(s, eDebug + 1))
			s << "Remove: " << theRefcon;
		if (!ZUtil_STL::sEraseIfContains(fPendingAdds, theRefcon))
			ZUtil_STL::sInsertMustNotContain(1, fPendingRemoves, theRefcon);

		if (ZUtil_STL::sEraseIfContains(fPendingResults, theRefcon))
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
	for (map<int64,QueryResult>::iterator iter = fPendingResults.begin();
		iter != fPendingResults.end(); ++iter)
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

void Source_Asyncify::pTrigger_Update()
	{
	ZAcqMtxR acq(fMtxR);
	if (fTriggered_Update)
		return;

	fTriggered_Update = true;
	sStartWorkerRunner(MakeWorker(MakeCallable(MakeRef(this), &Source_Asyncify::pUpdate)));
	}

void Source_Asyncify::pUpdate()
	{
	ZGuardRMtxR guard(fMtxR);

	for (;;)
		{
		bool didAnything = false;

		vector<AddedQuery> theAdds;
		theAdds.reserve(fPendingAdds.size());
		for (map<int64,ZRef<ZRA::Expr_Rel> >::iterator iter = fPendingAdds.begin();
			iter != fPendingAdds.end(); ++iter)
			{ theAdds.push_back(AddedQuery(iter->first, iter->second)); }
		fPendingAdds.clear();

		vector<int64> theRemoves(fPendingRemoves.begin(), fPendingRemoves.end());
		fPendingRemoves.clear();
		
		if (theAdds.size() || theRemoves.size())
			{
			guard.Release();
			didAnything = true;
			fSource->ModifyRegistrations(ZUtil_STL::sFirstOrNil(theAdds), theAdds.size(),
				ZUtil_STL::sFirstOrNil(theRemoves), theRemoves.size());
			guard.Acquire();
			}

		if (fNeeds_SourceCollectResults)
			{
			fNeeds_SourceCollectResults = false;
			guard.Release();

//##			if (ZMACRO_IOS_Simulator)
//##				ZThread::sSleep(1);
			vector<QueryResult> changes;
			fSource->CollectResults(changes);
			if (changes.size())
				{
				didAnything = true;
				guard.Acquire();

				for (vector<QueryResult>::iterator iter = changes.begin();
					iter != changes.end(); ++iter)
					{ fPendingResults[iter->GetRefcon()] = *iter; }

				guard.Release();
				this->pInvokeCallable_ResultsAvailable();
				}
			guard.Acquire();
			}

		if (!didAnything)
			break;
		}
	fTriggered_Update = false;
	}

void Source_Asyncify::pResultsAvailable(ZRef<Source> iSource)
	{
	ZGuardRMtxR guard(fMtxR);
	fNeeds_SourceCollectResults = true;
	this->pTrigger_Update();
	}

} // namespace ZDataspace
} // namespace ZooLib
