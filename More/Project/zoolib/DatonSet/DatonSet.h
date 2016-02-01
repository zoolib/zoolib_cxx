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

#ifndef __ZooLib_DatonSet_DatonSet_h__
#define __ZooLib_DatonSet_DatonSet_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/Data_Any.h"
#include "zoolib/Util_Relops.h"
#include "zoolib/ZThread.h"

#include "zoolib/DatonSet/Daton.h"
#include "zoolib/DatonSet/IntervalTreeClock.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace DatonSet {

using IntervalTreeClock::Identity;
using IntervalTreeClock::Event;

// =================================================================================================
#pragma mark -
#pragma mark Delta

class Delta : public CountedWithoutFinalize
	{
public:
	typedef std::map<Daton,bool> Statements_t;

	Delta(const Statements_t& iStatements);

	Delta(Statements_t* ioStatements);

	const Statements_t& GetStatements() const;

private:
	Statements_t fStatements;
	};

// =================================================================================================
#pragma mark -
#pragma mark Vector_Event_Delta_t

typedef std::pair<ZRef<Event>,ZRef<Delta> > Event_Delta_t;
typedef std::vector<Event_Delta_t> Vector_Event_Delta_t;

// =================================================================================================
#pragma mark -
#pragma mark Deltas

class Deltas : public CountedWithoutFinalize
	{
public:
	Deltas(const Vector_Event_Delta_t& iVector);
	Deltas(Vector_Event_Delta_t* ioVector);

	const Vector_Event_Delta_t& GetVector() const;

private:
	Vector_Event_Delta_t fVector;
	};

// =================================================================================================
#pragma mark -
#pragma mark DeltasChain

class DeltasChain : public CountedWithoutFinalize
	{
public:
	DeltasChain(const ZRef<DeltasChain>& iParent, const ZRef<Deltas>& iDeltas);

	ZRef<DeltasChain> GetParent() const;
	ZRef<Deltas> GetDeltas() const;

private:
	// When we do consolidation of Deltas, fParent will need to be mutable.
	const ZRef<DeltasChain> fParent;
	const ZRef<Deltas> fDeltas;
	};

void sGetComposed(ZRef<DeltasChain> iDeltasChain, std::set<Daton>& oComposed);

// =================================================================================================
#pragma mark -
#pragma mark DatonSet

class DatonSet : public CountedWithoutFinalize
	{
public:
	DatonSet(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent);

	void Insert(const Daton& iDaton);
	void Erase(const Daton& iDaton);

	ZRef<Event> GetEvent();

	ZRef<DatonSet> Fork();

	bool Join(ZRef<DatonSet>& ioOther);

	bool IncorporateDeltas(ZRef<Deltas> iDeltas);

	ZRef<Deltas> GetDeltas(ZRef<Event> iEvent);

private:
	DatonSet(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent,
		const ZRef<DeltasChain>& iDeltasChain);

	void pCommit();

	ZMtx fMtx; // RWLock?
	ZRef<Identity> fIdentity;
	ZRef<Event> fEvent;
	Delta::Statements_t fPendingStatements;
	ZRef<DeltasChain> fDeltasChain;
	};

// =================================================================================================
#pragma mark -
#pragma mark Callables

typedef Callable<void(const Daton& iDaton, bool iTrue)> Callable_DatonSetUpdate;

typedef Callable<ZRef<Deltas>(ZRef<Event> iEvent)> Callable_PullFrom;

typedef Callable<void(const ZRef<Callable_PullFrom>& iCallable_PullFrom)> Callable_PullSuggested;

} // namespace DatonSet
} // namespace ZooLib

#endif // __ZooLib_DatonSet_DatonSet_h__
