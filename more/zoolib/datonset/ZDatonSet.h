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

#ifndef __ZDatonSet_h__
#define __ZDatonSet_h__ 1
#include "zconfig.h"

#include "zoolib/ZData_Any.h"
#include "zoolib/ZIntervalTreeClock.h"
#include "zoolib/ZThread.h"
#include "zoolib/ZUtil_Relops.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace ZDatonSet {

typedef ZData_Any Data;

using ZIntervalTreeClock::Clock;
using ZIntervalTreeClock::Event;

// =================================================================================================
// MARK: - Daton

//! A trivial implementation till I get the signing stuff figured out

class Daton
	{
public:
	Daton();
	Daton(const Daton& iOther);
	~Daton();
	Daton& operator=(const Daton& iOther);

	Daton(Data iData);

	bool operator==(const Daton& iOther) const;
	bool operator<(const Daton& iOther) const;

	Data GetData() const;

private:
	Data fData;
	};

} // namespace ZDatonSet

template <> struct RelopsTraits_HasEQ<ZDatonSet::Daton> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<ZDatonSet::Daton> : public RelopsTraits_Has {};

namespace ZDatonSet {

// =================================================================================================
// MARK: - Delta

class Delta : public ZCountedWithoutFinalize
	{
public:
	typedef std::map<Daton, bool> Statements_t;

	Delta(const Statements_t& iStatements);

	Delta(Statements_t* ioStatements);

	const Statements_t& GetStatements() const;

private:
	Statements_t fStatements;
	};

// =================================================================================================
// MARK: - Vector_Event_Delta_t

typedef std::vector<std::pair<ZRef<Event>, ZRef<Delta> > > Vector_Event_Delta_t;

// =================================================================================================
// MARK: - Deltas

class Deltas : public ZCountedWithoutFinalize
	{
public:
	Deltas(const Vector_Event_Delta_t& iVector);
	Deltas(Vector_Event_Delta_t* ioVector);

	const Vector_Event_Delta_t& GetVector() const;

private:
	Vector_Event_Delta_t fVector;
	};

// =================================================================================================
// MARK: - DeltasChain

class DeltasChain : public ZCountedWithoutFinalize
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
// MARK: - DatonSet

class DatonSet : public ZCountedWithoutFinalize
	{
public:
	DatonSet(const ZRef<Clock>& iClock);

	void Insert(const Daton& iDaton);
	void Erase(const Daton& iDaton);

	ZRef<Event> GetEvent();

	ZRef<Event> TickleClock();

	ZRef<DatonSet> Fork();

	bool Join(ZRef<DatonSet>& ioOther);

	void GetDeltas(ZRef<Event> iEvent, ZRef<Event>& oEvent, ZRef<Deltas>& oDeltas);

	ZRef<DeltasChain> GetDeltasChain(ZRef<Event>* oEvent);

private:
	DatonSet(const ZRef<Clock>& iClock, const ZRef<DeltasChain>& iDeltasChain);

	void pIncorporateDeltas(const ZRef<Event>& iEvent, const ZRef<Deltas>& iDeltas);

	void pCommit();

	ZMtx fMtx; // RWLock?
	ZRef<Clock> fClock;
	Delta::Statements_t fPendingStatements;
	ZRef<DeltasChain> fDeltasChain;
	};

} // namespace ZDatonSet
} // namespace ZooLib

#endif // __ZDatonSet_h__
