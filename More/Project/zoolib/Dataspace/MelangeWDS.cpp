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

#include "zoolib/Callable_PMF.h"

#include "zoolib/Dataspace/MelangeWDS.h"
#include "zoolib/dataspace/Relater_Searcher.h"
#include "zoolib/dataspace/Searcher_DatonSet.h"

#include "zoolib/Log.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -
#pragma mark MelangeRoot

MelangeRoot::MelangeRoot(const ZRef<DatonSet::DatonSet>& iDS)
:	fDS(iDS)
	{}

void MelangeRoot::Initialize()
	{
	ZCounted::Initialize();
	fWDS = new DatonSet::WrappedDatonSet(fDS,
		sCallable(sWeakRef(this), &MelangeRoot::pNeedsUpdate));
	fDS.Clear();
	}

ZRef<DatonSet::WrappedDatonSet> MelangeRoot::GetWrappedDatonSet()
	{ return fWDS; }

void MelangeRoot::Start(ZRef<Starter> iStarter)
	{
	fJob = StartScheduler::Job(
		iStarter,
		sCallable(sWeakRef(this), &MelangeRoot::pWork));
	}

void MelangeRoot::pNeedsUpdate()
	{ sNextStartIn(0, fJob); }

void MelangeRoot::pWork()
	{
	ZThread::sSetName("MelangeRoot::pWork");

	fWDS->Update();
	}

// =================================================================================================
#pragma mark -
#pragma mark MelangeWDS

MelangeWDS::MelangeWDS(const ZRef<DatonSet::WrappedDatonSet>& iWDS_Parent,
	const std::vector<IndexSpec>& iIndexSpecs)
:	fWDS_Parent(iWDS_Parent)
,	fIndexSpecs(iIndexSpecs)
	{}

ZQ<ZRef<ZCounted> > MelangeWDS::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{ return fRelsWatcher_Relater->QCall(iCallable_Changed, iRel); }

ZQ<void> MelangeWDS::QCall(const DatonSet::Daton& iDaton, bool iTrue)
	{
	ZRef<DatonSet::DatonSet> theDS = fWDS->GetDatonSet_Active();
	if (iTrue)
		theDS->Insert(iDaton);
	else
		theDS->Erase(iDaton);
	return notnull;
	}

bool MelangeWDS::pTrigger()
	{
	if (fJob.first)
		{
		sNextStartIn(0, fJob);
		return true;
		}
	return false;
	}

void MelangeWDS::Start(ZRef<Starter> iStarter)
	{
	ZAssert(not fJob.first);

	fJob = StartScheduler::Job(
		iStarter,
		sCallable(sWeakRef(this), &MelangeWDS::pWork));

	{
	ZRef<Callable_Void> theCallable_NeedsUpdate_FromWDS =
		sCallable(sWeakRef(this), &MelangeWDS::pNeedsUpdate_FromWDS);

	fWDS = sSpawned(fWDS_Parent, theCallable_NeedsUpdate_FromWDS);
	}

	fWDS_Parent.Clear();

	ZRef<Searcher_DatonSet> theSearcher = new Searcher_DatonSet(fIndexSpecs);

	fWDS->InsertCallable_PullSuggested(theSearcher->GetCallable_PullSuggested());


	{
	ZRef<Relater> theRelater = new Relater_Searcher(theSearcher);

	ZRef<Callable_Void> theCallable_NeedsUpdate_FromRelWatcher =
		sCallable(sWeakRef(this), &MelangeWDS::pNeedsUpdate_FromRelWatcher);
	fRelsWatcher_Relater = new RelsWatcher_Relater(theRelater, theCallable_NeedsUpdate_FromRelWatcher);
	}

	fWDS->SuggestPull(theSearcher->GetCallable_PullSuggested());

	sNextStartIn(0, fJob);
	}


void MelangeWDS::pNeedsUpdate_FromWDS()
	{
	ZLOGTRACE(eDebug);
	sNextStartIn(0, fJob);
	}

void MelangeWDS::pNeedsUpdate_FromRelWatcher()
	{
	ZLOGTRACE(eDebug);
	sNextStartIn(0, fJob);
	}

void MelangeWDS::pWork()
	{
	fWDS->Update();

	fRelsWatcher_Relater->Update();

	Starter_EventLoopBase::pInvokeClearQueue();
	}

} // namespace Dataspace
} // namespace ZooLib
