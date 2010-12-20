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

#ifndef __ZDataset__
#define __ZDataset__ 1
#include "zconfig.h"

#include "zoolib/ZData_Any.h"
#include "zoolib/ZIntervalTreeClock.h"
#include "zoolib/ZThread.h"

#include "zoolib/dataset/ZDataset_Daton.h"

#include <map>
#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataset {

using ZIntervalTreeClock::Clock;
using ZIntervalTreeClock::Event;

// =================================================================================================
#pragma mark -
#pragma mark * Nombre

class Nombre
	{
public:
	Nombre();
	Nombre(const Nombre& iOther);
	~Nombre();
	Nombre& operator=(const Nombre& iOther);

	Nombre(uint64 iFork);
	Nombre(const Nombre& iOther, uint64 iFork);

	bool operator<(const Nombre& iRHS) const;

	std::string AsString() const;

private:
	std::vector<uint64> fForks;
	};

// =================================================================================================
#pragma mark -
#pragma mark * NamedEvent

class NamedEvent
	{
public:
	NamedEvent();
	NamedEvent(const NamedEvent& iOther);
	~NamedEvent();
	NamedEvent& operator=(const NamedEvent& iOther);

	NamedEvent(const Nombre& iNombre, const ZRef<Event>& iEvent);

	bool operator<(const NamedEvent& iRHS) const;

	std::string AsString() const;

	ZRef<Event> GetEvent() const;

private:
	Nombre fNombre;
	ZRef<Event> fEvent;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Delta

class Delta : public ZCountedWithoutFinalize
	{
public:
	Delta(const std::map<Daton, bool>& iStatements);
	Delta(std::map<Daton, bool>* ioStatements);

	const std::map<Daton, bool>& GetStatements();

private:
	std::map<Daton, bool> fStatements;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Map_NamedEvent_Delta_t

typedef std::map<NamedEvent, ZRef<Delta> > Map_NamedEvent_Delta_t;

// =================================================================================================
#pragma mark -
#pragma mark * Deltas

class Deltas : public ZCountedWithoutFinalize
	{
public:
	Deltas(const Map_NamedEvent_Delta_t& iMap);
	Deltas(Map_NamedEvent_Delta_t* ioMap);

	const Map_NamedEvent_Delta_t& GetMap();

private:
	Map_NamedEvent_Delta_t fMap;
	};

// =================================================================================================
#pragma mark -
#pragma mark * DeltasChain

class DeltasChain : public ZCountedWithoutFinalize
	{
public:
	DeltasChain(const ZRef<DeltasChain>& iParent, const ZRef<Deltas>& iDeltas);

	ZRef<DeltasChain> GetParent();
	ZRef<Deltas> GetDeltas();

private:
	// When we do consolidation of Deltas, fParent will need to be mutable.
	const ZRef<DeltasChain> fParent;
	const ZRef<Deltas> fDeltas;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Dataset

class Dataset : public ZCountedWithoutFinalize
	{
public:
	Dataset(const Nombre& iNombre, const ZRef<Clock>& iClock);

	void Insert(const Daton& iDaton);
	void Erase(const Daton& iDaton);

	ZRef<Clock> GetClock();
	ZRef<Event> GetEvent();

	ZRef<Dataset> Fork();

	void Join(ZRef<Dataset> iOther);

	void GetDeltas(ZRef<Event>& oEvent, ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent);
	ZRef<Event> GetDeltas(ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent);

	void IncorporateDeltas(const ZRef<Event>& iEvent, const ZRef<Deltas>& iDeltas);

	std::set<Daton> GetComposed();

	const Nombre& GetNombre();
	ZRef<DeltasChain> GetDeltasChain();

private:
	Dataset(
		const Nombre& iNombre, const ZRef<Clock>& iClock, const ZRef<DeltasChain>& iDeltasChain);

	void pCommit();

	ZMtx fMtx; // RWLock?
	const Nombre fNombre;
	uint64 fNextFork;
	ZRef<Clock> fClock;
	std::map<Daton, bool> fPendingStatements;
	ZRef<DeltasChain> fDeltasChain;
	};

} // namespace ZDataset
} // namespace ZooLib

#endif // __ZDataset__
