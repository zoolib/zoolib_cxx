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
#include "zoolib/ZCallOnNewThread.h"
#include "zoolib/ZUtil_STL_vector.h"
#include "zoolib/dataspace/ZDataspace_SourceServer.h"

namespace ZooLib {
namespace ZDataspace {

using std::vector;

// =================================================================================================
// MARK: - SourceServer

SourceServer::SourceServer
	(ZRef<Source> iSource, ZRef<ZStreamerR> iStreamerR, ZRef<ZStreamerW> iStreamerW)
:	fSource(iSource)
,	fStreamerR(iStreamerR)
,	fStreamerW(iStreamerW)
,	fNeedsWrite(false)
	{}

SourceServer::~SourceServer()
	{
	fSource->SetCallable_ResultsAvailable(null);
	}

void SourceServer::Initialize()
	{
	ZCounted::Initialize();
	fSource->SetCallable_ResultsAvailable(sCallable(this, &SourceServer::pCallback_Source));
	(new ZWorker(sCallable(sRef(this), &SourceServer::pRead)))->Attach(ZCaller_Thread::sCaller());
	}

void SourceServer::pCallback_Source(ZRef<Source> iSource)
	{
	ZAcqMtx acq(fMtx);
	if (not sGetSet(fNeedsWrite, true))
		sCallOnNewThread(sCallable(sRef(this), &SourceServer::pWrite));
	}

bool SourceServer::pRead(ZRef<ZWorker> iWorker)
	{
	const ZStreamR& r = fStreamerR->GetStreamR();

// need some kind of a close indication.

	vector<AddedQuery> addedQueries;
	for (uint32 theCount = r.ReadCount(); theCount; --theCount)
		{
		int64 theRefcon = r.ReadInt64();
		ZRef<ZRA::Expr_Rel> theRel;
		addedQueries.push_back(AddedQuery(theRefcon, theRel));
		}

	vector<int64> removedQueries;
	for (uint32 theCount = r.ReadCount(); theCount; --theCount)
		{
		int64 theRefcon = r.ReadInt64();
		removedQueries.push_back(theRefcon);
		}

	if (not addedQueries.empty() || not removedQueries.empty())
		{
		fSource->ModifyRegistrations
			(ZUtil_STL::sFirstOrNil(addedQueries), addedQueries.size(),
			ZUtil_STL::sFirstOrNil(removedQueries), removedQueries.size());
		}

	iWorker->Wake();//##
	return true;
	}

void SourceServer::pWrite()
	{
	ZGuardMtx guard(fMtx);
	fNeedsWrite = false;
	guard.Release();

	vector<QueryResult> theChanged;
	fSource->CollectResults(theChanged);

	const ZStreamW& w = fStreamerW->GetStreamW();

	w.WriteCount(theChanged.size());
	for (vector<QueryResult>::iterator i = theChanged.begin(); i != theChanged.end(); ++i)
		{
		w.WriteInt64(i->GetRefcon());
		// Write contents
		}
	w.Flush();
	}

} // namespace ZDataspace
} // namespace ZooLib
