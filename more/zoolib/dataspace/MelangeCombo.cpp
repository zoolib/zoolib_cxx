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

#include "zoolib/Dataspace/MelangeCombo.h"
#include "zoolib/dataspace/Relater_Searcher.h"
#include "zoolib/dataspace/Searcher_DatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - MelangeRoot

MelangeRoot::MelangeRoot(const ZRef<ZDatonSet::DatonSet>& iDS)
:	fDS(iDS)
	{}

void MelangeRoot::Initialize()
	{
	ZCounted::Initialize();
	fWDS = new WrappedDatonSet(fDS, sCallable(sWeakRef(this), &MelangeRoot::pNeedsUpdate));
	fDS.Clear();
	}

ZRef<WrappedDatonSet> MelangeRoot::GetWrappedDatonSet()
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
// MARK: - MelangeCombo

MelangeCombo::MelangeCombo(const ZRef<WrappedDatonSet>& iWDS_Parent)
:	fWDS_Parent(iWDS_Parent)
	{}

ZQ<ZRef<ZCounted> > MelangeCombo::QCall(
	const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
	const ZRef<Expr_Rel>& iRel)
	{ return fRelsWatcher_Relater->QCall(iCallable_Changed, iRel); }

ZQ<ZRef<Event> > MelangeCombo::QCall(
	const ZQ<ZDatonSet::Daton>& iPrior, const ZQ<ZDatonSet::Daton>& iNew)
	{
	ZRef<ZDatonSet::DatonSet> theDS = fWDS->GetDatonSet_Active();

	if (iPrior)
		theDS->Erase(*iPrior);

	if (iNew)
		theDS->Insert(*iNew);

	return theDS->GetEvent();
	}

bool MelangeCombo::pTrigger()
	{
	if (fJob.first)
		{
		sNextStartIn(0, fJob);
		return true;
		}
	return false;
	}

void MelangeCombo::Start(ZRef<Starter> iStarter)
	{
	ZAssert(not fJob.first);

	fJob = StartScheduler::Job(
		iStarter,
		sCallable(sWeakRef(this), &MelangeCombo::pWork));

	ZRef<Callable_Void> theCallable_NeedsUpdate =
		sCallable(sWeakRef(this), &MelangeCombo::pNeedsUpdate);

	fWDS = sSpawned(fWDS_Parent, theCallable_NeedsUpdate);
	fWDS_Parent.Clear();

	ZRef<Searcher_DatonSet> theSearcher = new Searcher_DatonSet;
	fWDS->InsertCallable_PullSuggested(theSearcher->GetCallable_PullSuggested());

	ZRef<Relater> theRelater = new Relater_Searcher(theSearcher);

	fRelsWatcher_Relater = new RelsWatcher_Relater(theRelater, theCallable_NeedsUpdate);

	fWDS->SuggestPull(theSearcher->GetCallable_PullSuggested());

	sNextStartIn(0, fJob);
	}

void MelangeCombo::pNeedsUpdate()
	{ sNextStartIn(0, fJob); }

void MelangeCombo::pWork()
	{
	fWDS->Update();

	fRelsWatcher_Relater->Update();

	Starter_EventLoopBase::pInvokeClearQueue();
	}

} // namespace Dataspace
} // namespace ZooLib
