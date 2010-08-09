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

#include "zoolib/ZString.h"
#include "zoolib/dataset/ZDataset.h"

#include <queue>

namespace ZooLib {
namespace ZDataset {

using namespace std;

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
#pragma mark *

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

	if (aleb)
		{
		if (blea)
			return fNombre < iRHS.fNombre;
		else
			return true;
		}
	else if (blea)
		{
		return false;
		}
	else
		{
		return fNombre < iRHS.fNombre;
		}
	}

ZRef<Event> NamedEvent::GetEvent() const
	{ return fEvent; }

string NamedEvent::AsString() const
	{ return fNombre.AsString() + ":" /*+ ZStringf("%llu", fStamp)*/; }

// =================================================================================================
#pragma mark -
#pragma mark *

Delta::Delta(const map<Daton, bool>& iStatements)
:	fStatements(iStatements)
	{}

Delta::Delta(map<Daton, bool>* ioStatements)
	{ ioStatements->swap(fStatements); }

const map<Daton, bool>& Delta::GetStatements()
	{ return fStatements; }

// =================================================================================================
#pragma mark -
#pragma mark *

Deltas::Deltas(const Map_NamedEvent_Delta_t& iMap)
:	fMap(iMap)
	{}

Deltas::Deltas(Map_NamedEvent_Delta_t* ioMap)
	{ ioMap->swap(fMap); }

const Map_NamedEvent_Delta_t& Deltas::GetMap()
	{ return fMap; }

// =================================================================================================
#pragma mark -
#pragma mark *

DeltasChain::DeltasChain(const ZRef<DeltasChain>& iParent, const ZRef<Deltas>& iDeltas)
:	fParent(iParent)
,	fDeltas(iDeltas)
	{}

ZRef<DeltasChain> DeltasChain::GetParent()
	{ return fParent; }

ZRef<Deltas> DeltasChain::GetDeltas()
	{ return fDeltas; }

// =================================================================================================
#pragma mark -
#pragma mark *

Dataset::Dataset(
	const Nombre& iNombre, const ZRef<Stamp>& iStamp, const ZRef<DeltasChain>& iDeltasChain)
:	fNombre(iNombre)
,	fNextFork(1)
,	fStamp(iStamp)
,	fDeltasChain(iDeltasChain)
	{}

Dataset::Dataset(const Nombre& iNombre, const ZRef<Stamp>& iStamp)
:	fNombre(iNombre)
,	fNextFork(1)
,	fStamp(iStamp)
	{}

void Dataset::Insert(const Daton& iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = true;
	}

void Dataset::Erase(const Daton& iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = false;
	}

ZRef<Stamp> Dataset::GetStamp()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fStamp;
	}

ZRef<Event> Dataset::GetEvent()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fStamp->GetEvent();
	}

ZRef<Dataset> Dataset::Fork()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return new Dataset(Nombre(fNombre, fNextFork++), sFork(fStamp), fDeltasChain);
	}

void Dataset::Join(ZRef<Dataset> iOther)
	{
	ZRef<Deltas> theDeltas;
	ZRef<Event> otherEvent = iOther->GetDeltas(theDeltas, fStamp->GetEvent());
	this->IncorporateDeltas(otherEvent, theDeltas);
	}

void Dataset::GetDeltas(ZRef<Event>& oEvent, ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent)
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
	oEvent = fStamp->GetEvent();
	}

ZRef<Event> Dataset::GetDeltas(ZRef<Deltas>& oDeltas, const ZRef<Event>& iEvent)
	{
	ZRef<Event> result;
	this->GetDeltas(result, oDeltas, iEvent);
	return result;
	}

void Dataset::IncorporateDeltas(const ZRef<Event>& iEvent, const ZRef<Deltas>& iDeltas)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	fDeltasChain = new DeltasChain(fDeltasChain, iDeltas);
	sReceive(fStamp, iEvent);
	}

typedef pair<NamedEvent, size_t> TSIndex_t;

class CompareTSReverse
:	public std::binary_function<TSIndex_t, TSIndex_t, bool>
	{
public:
	// We want oldest ts first, so we reverse the sense of the compare.
	result_type operator()(const first_argument_type& i1, const second_argument_type& i2) const
		{ return i2.first < i1.first; }
	};

set<Daton> Dataset::GetComposed()
	{
	// This is a bit tricky. We're effectively merge sorting from our history entries.

	ZAcqMtx acq(fMtx);
	this->pCommit();

	vector<Map_NamedEvent_Delta_t::const_iterator> theIters, theEnds;
	for (ZRef<DeltasChain> current = fDeltasChain;
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

	set<Daton> result;
	while (!queue.empty())
		{
		const size_t theX = queue.top().second;
		queue.pop();

		ZRef<Delta> theDelta = theIters[theX]->second;

		if (++theIters[theX] != theEnds[theX])
			queue.push(TSIndex_t(theIters[theX]->first, theX));

		const map<Daton, bool>& theStatements = theDelta->GetStatements();
		for (map<Daton, bool>::const_iterator j = theStatements.begin();
			j != theStatements.end(); ++j)
			{
			if (j->second)
				result.insert(j->first);
			else
				result.erase(j->first);
			}
		}

	return result;
	}

const Nombre& Dataset::GetNombre()
	{ return fNombre; }

ZRef<DeltasChain> Dataset::GetDeltasChain()
	{ return fDeltasChain; }

void Dataset::pCommit()
	{
	if (!fPendingStatements.empty())
		{
		sEvent(fStamp);

		const NamedEvent theNE(fNombre, fStamp->GetEvent());
		const ZRef<Delta> theDelta = new Delta(&fPendingStatements);

		Map_NamedEvent_Delta_t theMap;
		theMap.insert(Map_NamedEvent_Delta_t::value_type(theNE, theDelta));

		ZRef<Deltas> theDeltas = new Deltas(&theMap);

		fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);
		}
	}

} // namespace ZDataset
} // namespace ZooLib
