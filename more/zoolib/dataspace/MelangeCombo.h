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

#ifndef __ZooLib_Dataspace_MelangeCombo_h__
#define __ZooLib_Dataspace_MelangeCombo_h__ 1
#include "zconfig.h"

#include "zoolib/Starter_EventLoopBase.h"
#include "zoolib/StartScheduler.h"

#include "zooLib/dataspace/MelangeCombo.h"
#include "zooLib/dataspace/RelsWatcher_Relater.h"
#include "zoolib/dataspace/WrappedDatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - MelangeRoot

class MelangeRoot
:	public ZCounted
	{
public:
	MelangeRoot(const ZRef<ZDatonSet::DatonSet>& iDS);

	virtual void Initialize();

	ZRef<WrappedDatonSet> GetWrappedDatonSet();

	void Start(ZRef<Starter> iStarter);

private:
	void pNeedsUpdate();

	void pWork();

	ZRef<ZDatonSet::DatonSet> fDS;
	StartScheduler::Job fJob;
	ZRef<WrappedDatonSet> fWDS;
	};

// =================================================================================================
// MARK: - MelangeCombo

class MelangeCombo
:	public RelsWatcher::Callable_Register
,	public Callable_DatonSetUpdate
,	public Starter_EventLoopBase
	{
public:
	MelangeCombo(const ZRef<WrappedDatonSet>& iWDS_Parent);

// From Callable via Callable_Register
	virtual ZQ<ZRef<ZCounted> > QCall(
		const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel);

// From Callable via Callable_DatonSetUpdate
	virtual ZQ<ZRef<Event> > QCall(
		const ZQ<ZDatonSet::Daton>& iPrior, const ZQ<ZDatonSet::Daton>& iNew);

// From Starter_EventLoopBase
	virtual bool pTrigger();

// Our protocol
	void Start(ZRef<Starter> iStarter);

private:
	void pNeedsUpdate();

	void pWork();

	ZRef<WrappedDatonSet> fWDS_Parent;
	ZRef<WrappedDatonSet> fWDS;

	StartScheduler::Job fJob;
	ZRef<RelsWatcher_Relater> fRelsWatcher_Relater;
	};

// =================================================================================================
// MARK: -

Melange_t sMelange(const ZRef<WrappedDatonSet>& iWDS_Parent);

ZRef<MelangeFactory> sMelangeFactory(const ZRef<WrappedDatonSet>& iWDS_Parent);

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_MelangeCombo_h__
