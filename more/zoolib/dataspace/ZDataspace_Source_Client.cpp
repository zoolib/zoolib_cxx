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
#include "zoolib/ZCaller_Thread.h"
#include "zoolib/ZWorker_Callable.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_set.h"
#include "zoolib/dataspace/ZDataspace_Source_Client.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * Source_Client

Source_Client::Source_Client(ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fNeedsWrite(false)
	{}

Source_Client::~Source_Client()
	{}

void Source_Client::Initialize()
	{
	ZRef<ZWorker> theWorker = sWorker(sCallable(sWeakRef(this), &Source_Client::pRead));
	sStartWorkerRunner(theWorker);
	}

bool Source_Client::Intersects(const RelHead& iRelHead)
	{
	//## needs work
	return true;
	}

void Source_Client::ModifyRegistrations
	(const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardRMtxR guard(fMtxR);

	while (iAddedCount--)
		{
		ZUtil_STL::sInsertMustNotContain(1,
			fAdds, iAdded->GetRefcon(), iAdded->GetRel());
		++iAdded;
		}

	while (iRemovedCount--)
		{
		const int64 theRefcon = *iRemoved++;
		if (not ZUtil_STL::sEraseIfContains(fAdds, theRefcon))
			ZUtil_STL::sInsertMustNotContain(1, fRemoves, theRefcon);
		}

	if (!fNeedsWrite)
		{
		fNeedsWrite = true;
		sCallOnNewThread(sCallable(sRef(this), &Source_Client::pWrite));
		}
	}

void Source_Client::CollectResults(std::vector<QueryResult>& oChanged)
	{
	ZGuardRMtxR guard(fMtxR);
	this->pCollectResultsCalled();
	oChanged.swap(fResults);
	fResults.clear();
	}

bool Source_Client::pRead(ZRef<ZWorker> iWorker)
	{
	const ZStreamR& r = fStreamerR->GetStreamR();

	vector<QueryResult> theSRs;
	for (uint32 theCount = r.ReadCount(); theCount; --theCount)
		{
		int64 theRefcon = r.ReadInt64();
		// Need to read a result and an Event
		theSRs.push_back(QueryResult(theRefcon, null, null));
		}

	ZGuardRMtxR guard(fMtxR);
	fResults.insert(fResults.end(), theSRs.begin(), theSRs.end());
	guard.Release();
	Source::pTriggerResultsAvailable();

	iWorker->Wake();//##
	return true;
	}

void Source_Client::pWrite()
	{
	ZGuardRMtxR guard(fMtxR);
	fNeedsWrite = false;
	std::map<int64, ZRef<ZRA::Expr_Rel> > theAdds;
	theAdds.swap(fAdds);

	std::set<int64> theRemoves;
	theRemoves.swap(fRemoves);
	guard.Release();

	const ZStreamW& w = fStreamerW->GetStreamW();
	// Send the stuff

	w.Flush();
	}

} // namespace ZDataspace
} // namespace ZooLib
