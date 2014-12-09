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

#ifndef __ZooLib_DatonSet_WrappedDatonSetRemoter_h__
#define __ZooLib_DatonSet_WrappedDatonSetRemoter_h__ 1
#include "zconfig.h"

#include "zoolib/Connection.h"
#include "zoolib/Multi.h"

#include "zoolib/datonset/WrappedDatonSet.h"

namespace ZooLib {
namespace DatonSet {

// =================================================================================================
// MARK: - WrappedDatonSetRemoter

class WrappedDatonSetRemoter
:	public ZCounted
	{
public:
	// Client ctor
	WrappedDatonSetRemoter(
		const ZRef<ChannerComboFactoryRW_Bin>& iChannerComboFactory,
		const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other);

	// Server ctor
	WrappedDatonSetRemoter(
		const ChannerComboRW_Bin& iChannerComboRW,
		const ZRef<Callable_PullSuggested>& iCallable_PullSuggested_Other);

// From ZCounted
	virtual void Initialize();

// Our protocol
	ZRef<Callable_PullSuggested> GetCallable_PullSuggested();

private:
	void pPullSuggested(const ZRef<Callable_PullFrom>& iCallable_PullFrom);

	ZRef<Deltas> pPullFrom(ZRef<Event> iEvent);

	void pRead();
	static void spRead(ZRef<WrappedDatonSetRemoter> iWDSR);

	ZQ<ChannerComboRW_Bin> pQEnsureChannerCombo();

	const ZRef<ChannerComboFactoryRW_Bin> fChannerComboFactory;

	const ZRef<Callable_PullSuggested> fCallable_PullSuggested_Other;

	ZMtxR fMtxR;
	ZCnd fCnd;

	ZRef<Callable_PullSuggested> fCallable_PullSuggested_Self;

	ZRef<Callable_PullFrom> fCallable_PullFrom_Self;

	std::set<ZRef<Callable_PullFrom> > fCallables_PullFrom;

	ZQ<ChannerComboRW_Bin> fChannerComboQ;

	bool fConnectionBusy;

	ZRef<Deltas>* fPullFromPointer;
	};

} // namespace DatonSet
} // namespace ZooLib

#endif // __ZooLib_DatonSet_WrappedDatonSetRemoter_h__
