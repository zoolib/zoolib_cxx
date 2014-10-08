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

#ifndef __ZooLib_Dataspace_RelsWatcher_Relater_h__
#define __ZooLib_Dataspace_RelsWatcher_Relater_h__ 1
#include "zconfig.h"

#include "zooLib/ValueOnce.h"

#include "zooLib/dataspace/Relater.h"
#include "zooLib/dataspace/RelsWatcher.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
// MARK: - RelsWatcher_Relater

class RelsWatcher_Relater
:	public RelsWatcher::Callable_Register
	{
	typedef RelsWatcher::Callable_Register inherited;
public:
	typedef Callable_Void Callable_NeedsUpdate;

	RelsWatcher_Relater(const ZRef<Relater>& iRelater,
		const ZRef<Callable_NeedsUpdate>& iCallable_NeedsUpdate);

// From ZCounted via Callable_Register
	virtual void Initialize();
	virtual void Finalize();

// From Callable via Callable_Register
	ZQ<ZRef<ZCounted> > QCall(
		const ZRef<RelsWatcher::Callable_Changed>& iCallable_Changed,
		const ZRef<Expr_Rel>& iRel);

// Our protocol
	void Update();

private:
	void pCallback_Relater(ZRef<Relater> iRelater);

	class Registration;
	void pFinalize(Registration* iRegistration);

	ZMtxR fMtxR;

	ZRef<Relater> fRelater;

	FalseOnce fCalled_NeedsUpdate;
	ZRef<Callable_NeedsUpdate> fCallable_NeedsUpdate;

	int64 fNextRefcon;

	typedef std::map<int64, Registration*> Map_RefconToRegistrationX;
	Map_RefconToRegistrationX fMap_RefconToRegistrationX;

	std::set<Registration*> fToAdd;
	std::set<int64> fToRemove;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_RelsWatcher_Relater_h__
