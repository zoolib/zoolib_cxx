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

#ifndef __ZooLib_Dataspace_MelangeWDS_h__
#define __ZooLib_Dataspace_MelangeWDS_h__ 1
#include "zconfig.h"

#include "zoolib/Starter_EventLoopBase.h"
#include "zoolib/StartScheduler.h"

#include "zooLib/Dataspace/Melange.h"
#include "zooLib/Dataspace/RelsWatcher_Relater.h"
#include "zoolib/Dataspace/Searcher_DatonSet.h"

#include "zoolib/datonset/WrappedDatonSet.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark -
#pragma mark MelangeRoot

class MelangeRoot
:	public ZCounted
	{
public:
	MelangeRoot(const ZRef<DatonSet::DatonSet>& iDS);

	virtual void Initialize();

	ZRef<DatonSet::WrappedDatonSet> GetWrappedDatonSet();

	void Start(ZRef<Starter> iStarter);

private:
	void pNeedsUpdate();

	void pWork();

	ZRef<DatonSet::DatonSet> fDS;
	StartScheduler::Job fJob;
	ZRef<DatonSet::WrappedDatonSet> fWDS;
	};

// =================================================================================================
#pragma mark -
#pragma mark MelangeWDS

class MelangeWDS
:	public Dataspace::Callable_Register
,	public Callable_DatonSetUpdate
,	public Starter_EventLoopBase
	{
public:
	MelangeWDS(const ZRef<DatonSet::WrappedDatonSet>& iWDS_Parent,
		const std::vector<Dataspace::IndexSpec>& iIndexSpecs);

// From Callable via Callable_Register
	virtual ZQ<ZRef<ZCounted> > QCall(
		const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel);

// From Callable via Callable_DatonSetUpdate
	virtual ZQ<void> QCall(const DatonSet::Daton& iDaton, bool iTrue);

// From Starter_EventLoopBase
	virtual bool pTrigger();

// Our protocol
	void Start(ZRef<Starter> iStarter);

private:
	void pNeedsUpdate_FromWDS();
	void pNeedsUpdate_FromRelWatcher();

	void pWork();

	ZRef<DatonSet::WrappedDatonSet> fWDS_Parent;
	ZRef<DatonSet::WrappedDatonSet> fWDS;

	const std::vector<Dataspace::IndexSpec> fIndexSpecs;

	StartScheduler::Job fJob;
	ZRef<RelsWatcher_Relater> fRelsWatcher_Relater;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_MelangeWDS_h__
