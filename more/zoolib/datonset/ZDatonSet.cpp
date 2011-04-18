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

#include "zoolib/ZLog.h"
#include "zoolib/ZString.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"

#include "zoolib/datonset/ZDatonSet.h"

#include <queue>

namespace ZooLib {
namespace ZDatonSet {

using namespace std;

// =================================================================================================
#pragma mark -
#pragma mark * Daton

Daton::Daton()
	{}

Daton::Daton(const Daton& iOther)
:	fData(iOther.fData)
	{}

Daton::~Daton()
	{}

Daton& Daton::operator=(const Daton& iOther)
	{
	fData = iOther.fData;
	return *this;
	}

Daton::Daton(ZData iData)
:	fData(iData)
	{}

bool Daton::operator==(const Daton& iOther) const
	{ return fData == iOther.fData; }

bool Daton::operator!=(const Daton& iOther) const
	{ return !(fData == iOther.fData); }

bool Daton::operator<(const Daton& iOther) const
	{ return fData < iOther.fData; }

ZData Daton::GetData() const
	{ return fData; }

// =================================================================================================
#pragma mark -
#pragma mark * Nombre

Nombre::Nombre()
	{}

Nombre::Nombre(const Nombre& iOther)
:	fForks(iOther.fForks)
	{}

Nombre::~Nombre()
	{}

Nombre& Nombre::operator=(const Nombre& iOther)
	{
	fForks = iOther.fForks;
	return *this;
	}

Nombre::Nombre(uint64 iFork)
:	fForks(1, iFork)
	{}

Nombre::Nombre(const Nombre& iOther, uint64 iFork)
:	fForks(iOther.fForks)
	{
	fForks.push_back(iFork);
	}

bool Nombre::operator<(const Nombre& iRHS) const
	{ return fForks < iRHS.fForks; }

string Nombre::AsString() const
	{
	string result;
	for (vector<uint64>::const_iterator i = fForks.begin(); i != fForks.end(); ++i)
		result += "/" + ZStringf("%llu", *i);
	return result;
	}

// =================================================================================================
#pragma mark -
#pragma mark * NamedEvent

NamedEvent::NamedEvent()
	{}

NamedEvent::NamedEvent(const NamedEvent& iOther)
:	fNombre(iOther.fNombre)
,	fEvent(iOther.fEvent)
	{}

NamedEvent::~NamedEvent()
	{}

NamedEvent& NamedEvent::operator=(const NamedEvent& iOther)
	{
	fNombre = iOther.fNombre;
	fEvent = iOther.fEvent;
	return *this;
	}

NamedEvent::NamedEvent(const Nombre& iNombre, const ZRef<Event>& iEvent)
:	fNombre(iNombre)
,	fEvent(iEvent)
	{}

bool NamedEvent::operator<(const NamedEvent& iRHS) const
	{
	const bool aleb = fEvent->LessEqual(iRHS.fEvent);
	const bool blea = iRHS.fEvent->LessEqual(fEvent);

	if (aleb == blea)
		return fNombre < iRHS.fNombre;
	else
		return aleb;
	}

ZRef<Event> NamedEvent::GetEvent() const
	{ return fEvent; }

string NamedEvent::AsString() const
	{ return fNombre.AsString() + ":" /*+ ZStringf("%llu", fClock)*/; }

// =================================================================================================
#pragma mark -
#pragma mark * Delta

Delta::Delta(const map<Daton, bool>& iStatements)
:	fStatements(iStatements)
	{}

Delta::Delta(map<Daton, bool>* ioStatements)
	{ ioStatements->swap(fStatements); }

const map<Daton, bool>& Delta::GetStatements()
	{ return fStatements; }

// =================================================================================================
#pragma mark -
#pragma mark * Deltas

Deltas::Deltas(const Map_NamedEvent_Delta_t& iMap)
:	fMap(iMap)
	{}

Deltas::Deltas(Map_NamedEvent_Delta_t* ioMap)
	{ ioMap->swap(fMap); }

const Map_NamedEvent_Delta_t& Deltas::GetMap()
	{ return fMap; }

// =================================================================================================
#pragma mark -
#pragma mark * DeltasChain

DeltasChain::DeltasChain(const ZRef<DeltasChain>& iParent, const ZRef<Deltas>& iDeltas)
:	fParent(iParent)
,	fDeltas(iDeltas)
	{}

ZRef<DeltasChain> DeltasChain::GetParent()
	{ return fParent; }

ZRef<Deltas> DeltasChain::GetDeltas()
	{ return fDeltas; }

typedef pair<NamedEvent, size_t> TSIndex_t;

class CompareTSReverse
:	public std::binary_function<TSIndex_t, TSIndex_t, bool>
	{
public:
	// We want oldest ts first, so we reverse the sense of the compare.
	result_type operator()(const first_argument_type& i1, const second_argument_type& i2) const
		{ return i2.first < i1.first; }
	};

void sGetComposed(ZRef<DeltasChain> iDeltasChain, std::set<Daton>& oComposed)
	{
	// This is a bit tricky. We're effectively merge sorting from our history entries.

	vector<Map_NamedEvent_Delta_t::const_iterator> theIters, theEnds;
	for (ZRef<DeltasChain> current = iDeltasChain;
		current; current = current->GetParent())
		{
		Map_NamedEvent_Delta_t::const_iterator theBegin =
			current->GetDeltas()->GetMap().begin();

		Map_NamedEvent_Delta_t::const_iterator theEnd =
			current->GetDeltas()->GetMap().end();

		if (theBegin != theEnd)
			{
			theIters.push_back(theBegin);
			theEnds.push_back(theEnd);
			}
		}

	priority_queue<TSIndex_t, vector<TSIndex_t>, CompareTSReverse> queue;

	for (size_t x = 0; x < theIters.size(); ++x)
		queue.push(TSIndex_t(theIters[x]->first, x));

	while (!queue.empty())
		{
		const size_t theX = queue.top().second;
		queue.pop();

		const map<Daton, bool>& theStatements = theIters[theX]->second->GetStatements();
		for (map<Daton, bool>::const_iterator j = theStatements.begin();
			j != theStatements.end(); ++j)
			{
			if (j->second)
				oComposed.insert(j->first);
			else
				oComposed.erase(j->first);
			}

		if (++theIters[theX] != theEnds[theX])
			queue.push(TSIndex_t(theIters[theX]->first, theX));
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * DatonSet

DatonSet::DatonSet(
	const Nombre& iNombre, const ZRef<Clock>& iClock, const ZRef<DeltasChain>& iDeltasChain)
:	fNombre(iNombre)
,	fNextFork(1)
,	fClock(iClock)
,	fDeltasChain(iDeltasChain)
	{}

DatonSet::DatonSet(const Nombre& iNombre, const ZRef<Clock>& iClock)
:	fNombre(iNombre)
,	fNextFork(1)
,	fClock(iClock)
	{}

void DatonSet::Insert(const Daton& iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = true;
	}

void DatonSet::Erase(const Daton& iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = false;
	}

ZRef<Clock> DatonSet::GetClock()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fClock;
	}

ZRef<Event> DatonSet::GetEvent()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fClock->GetEvent();
	}

ZRef<DatonSet> DatonSet::Fork()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	ZRef<Clock> a, b;
	fClock->Forked(a, b);
	
//	if (random() & 1)
//		swap(a, b);
	fClock = a;
	
	return new DatonSet(Nombre(fNombre, fNextFork++), b, fDeltasChain);
	}

void DatonSet::Join(ZRef<DatonSet>& ioOther)
	{
	ZLOGF(s, eDebug + 1);
	if (s)
		s << "Before: " << fClock;

	ZRef<Deltas> theDeltas;
	ZRef<Event> otherEvent = ioOther->GetDeltas(theDeltas, fClock->GetEvent());
	this->IncorporateDeltas(otherEvent, theDeltas);
	if (s)
		s << ", incorporate other:" << otherEvent << ", clock:" << fClock;
	sJoin(fClock, ioOther->fClock);
	if (s)
		s << ", after join:" << fClock;
	ioOther->fClock.Clear();
	ioOther->fPendingStatements.clear();
	ioOther->fDeltasChain.Clear();
	ioOther.Clear();
	}

void DatonSet::GetDeltas(ZRef<Event>& oEvent, ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	const NamedEvent neLower(Nombre(), iEvent);

	Map_NamedEvent_Delta_t resultMap;
	for (ZRef<DeltasChain> current = fDeltasChain;
		current; current = current->GetParent())
		{
		const Map_NamedEvent_Delta_t& theMap = current->GetDeltas()->GetMap();
		for (Map_NamedEvent_Delta_t::const_iterator
			i = theMap.lower_bound(neLower), theEnd = theMap.end(); i != theEnd; ++i)
			{
			const ZRef<Event> candidate = i->first.GetEvent();
			if (! candidate->LessEqual(iEvent) || iEvent->LessEqual(candidate))
				resultMap.insert(*i);
			}
		}

	oDeltas = new Deltas(&resultMap);
	oEvent = fClock->GetEvent();
	}

ZRef<Event> DatonSet::GetDeltas(ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent)
	{
	ZRef<Event> result;
	this->GetDeltas(result, oDeltas, iEvent);
	return result;
	}

void DatonSet::IncorporateDeltas(const ZRef<Event>& iEvent, const ZRef<Deltas>& iDeltas)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	fDeltasChain = new DeltasChain(fDeltasChain, iDeltas);
	sReceive(fClock, iEvent);
	}

ZRef<DeltasChain> DatonSet::GetDeltasChain(ZRef<Event>* oEvent)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	if (oEvent)
		*oEvent = fClock->GetEvent();

	return fDeltasChain;
	}

const Nombre& DatonSet::GetNombre()
	{ return fNombre; }

void DatonSet::pCommit()
	{
	if (fPendingStatements.empty())
		return;

	ZLOGF(s, eDebug + 1);
	if (s)
		s << this << " before:" << fClock;

	const NamedEvent theNE(fNombre, fClock->GetEvent());
	const ZRef<Delta> theDelta = new Delta(&fPendingStatements);

	Map_NamedEvent_Delta_t theMap;
	theMap.insert(Map_NamedEvent_Delta_t::value_type(theNE, theDelta));

	ZRef<Deltas> theDeltas = new Deltas(&theMap);

	fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);
	// This is a hack -- comparing (2+0,0) against 2 has them be concurrent,
	// sEvent against (2+0,0) is 2, which might not be correct. FIXME
	sEvent(fClock);
	sEvent(fClock);

	if (s)
		s << ", after:"<< fClock;
	}

} // namespace ZDatonSet
} // namespace ZooLib
