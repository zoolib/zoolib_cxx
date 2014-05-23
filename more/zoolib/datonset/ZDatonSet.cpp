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
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/datonset/ZDatonSet.h"

#include <queue>

namespace ZooLib {
namespace ZDatonSet {

using namespace std;
using namespace ZUtil_STL;

// =================================================================================================
// MARK: - Daton

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

Daton::Daton(Data iData)
:	fData(iData)
	{}

bool Daton::operator==(const Daton& iOther) const
	{ return fData == iOther.fData; }

bool Daton::operator<(const Daton& iOther) const
	{ return fData < iOther.fData; }

Data Daton::GetData() const
	{ return fData; }

// =================================================================================================
// MARK: - Delta

Delta::Delta(const Statements_t& iStatements)
:	fStatements(iStatements)
	{}

Delta::Delta(Statements_t* ioStatements)
	{ ioStatements->swap(fStatements); }

const Delta::Statements_t& Delta::GetStatements() const
	{ return fStatements; }

// =================================================================================================
// MARK: - Deltas

Deltas::Deltas(const Vector_Event_Delta_t& iVector)
:	fVector(iVector)
	{}

Deltas::Deltas(Vector_Event_Delta_t* ioVector)
	{ ioVector->swap(fVector); }

const Vector_Event_Delta_t& Deltas::GetVector() const
	{ return fVector; }

// =================================================================================================
// MARK: - DeltasChain

DeltasChain::DeltasChain(const ZRef<DeltasChain>& iParent, const ZRef<Deltas>& iDeltas)
:	fParent(iParent)
,	fDeltas(iDeltas)
	{}

ZRef<DeltasChain> DeltasChain::GetParent() const
	{ return fParent; }

ZRef<Deltas> DeltasChain::GetDeltas() const
	{ return fDeltas; }

typedef pair<ZRef<Event>, size_t> TSIndex_t;

class CompareTSReverse
:	public std::binary_function<TSIndex_t, TSIndex_t, bool>
	{
public:
	// We want oldest ts first, so we reverse the sense of the compare.
	result_type operator()(const first_argument_type& i1, const second_argument_type& i2) const
		{ return sIsBefore(i2.first, i1.first); }
	};

void sGetComposed(ZRef<DeltasChain> iDeltasChain, std::set<Daton>& oComposed)
	{
	// This is a bit tricky. We're effectively merge sorting from our history entries.

	vector<Vector_Event_Delta_t::const_iterator> theIters, theEnds;
	for (ZRef<DeltasChain> current = iDeltasChain;
		current; current = current->GetParent())
		{
		Vector_Event_Delta_t::const_iterator theBegin =
			current->GetDeltas()->GetVector().begin();

		Vector_Event_Delta_t::const_iterator theEnd =
			current->GetDeltas()->GetVector().end();

		if (theBegin != theEnd)
			{
			theIters.push_back(theBegin);
			theEnds.push_back(theEnd);
			}
		}

	priority_queue<TSIndex_t, vector<TSIndex_t>, CompareTSReverse> queue;

	for (size_t xx = 0; xx < theIters.size(); ++xx)
		queue.push(TSIndex_t(theIters[xx]->first, xx));

	while (not queue.empty())
		{
		const size_t theX = queue.top().second;
		queue.pop();

		foreachi (jj, theIters[theX]->second->GetStatements())
			{
			if (jj->second)
				oComposed.insert(jj->first);
			else
				oComposed.erase(jj->first);
			}

		if (++theIters[theX] != theEnds[theX])
			queue.push(TSIndex_t(theIters[theX]->first, theX));
		}
	}

// =================================================================================================
// MARK: - DatonSet

DatonSet::DatonSet(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent, const ZRef<DeltasChain>& iDeltasChain)
:	fIdentity(iIdentity)
,	fEvent(iEvent)
,	fDeltasChain(iDeltasChain)
	{}

DatonSet::DatonSet(const ZRef<Identity>& iIdentity, const ZRef<Event>& iEvent)
:	fIdentity(iIdentity)
,	fEvent(iEvent)
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

ZRef<Event> DatonSet::GetEvent()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fEvent;
	}

ZRef<Event> DatonSet::TickleClock()
	{
	ZAcqMtx acq(fMtx);
	if (fPendingStatements.empty())
		fEvent = fEvent->Advanced(fIdentity);
	else
		this->pCommit();
	return fEvent;
	}

ZRef<DatonSet> DatonSet::Fork()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	ZRef<Identity> newLeft, newRight;
	fIdentity->Split(newLeft, newRight);
	fIdentity = newLeft;

	ZRef<DatonSet> newDS = new DatonSet(newRight, fEvent->Advanced(newRight), fDeltasChain);
	fEvent = fEvent->Advanced(fIdentity);
	return newDS;
	}


bool DatonSet::TentativeJoin(const ZRef<DatonSet>& iOther)
	{
	ZGuardMtx guard(fMtx);
	this->pCommit();
	guard.Release();

	ZRef<Event> otherEvent;
	ZRef<Deltas> theDeltas;
	iOther->GetDeltas(fEvent, theDeltas, otherEvent);
	return sNotEmpty(theDeltas->GetVector());
	}

bool DatonSet::Join(ZRef<DatonSet>& ioOther)
	{
	ZGuardMtx guard(fMtx);
	this->pCommit();
	guard.Release();

	ZRef<Event> otherEvent;
	ZRef<Deltas> theDeltas;
	ioOther->GetDeltas(fEvent, theDeltas, otherEvent);

	guard.Acquire();

	if (theDeltas && theDeltas->GetVector().size())
		fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);
	fEvent = fEvent->Joined(otherEvent);
	fEvent = fEvent->Advanced(fIdentity);

	fIdentity = fIdentity->Summed(ioOther->fIdentity);

	ioOther->fIdentity.Clear();
	ioOther->fEvent.Clear();
	ioOther->fPendingStatements.clear();
	ioOther->fDeltasChain.Clear();
	ioOther.Clear();

	return sNotEmpty(theDeltas->GetVector());
	}

void DatonSet::GetDeltas(ZRef<Event> iEvent, ZRef<Deltas>& oDeltas, ZRef<Event>& oEvent)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	oEvent = iEvent;

	Vector_Event_Delta_t resultVector;
	for (ZRef<DeltasChain> current = fDeltasChain;
		current; current = current->GetParent())
		{
		// Indexing can't work. But maybe we can record the join and meet of the
		// contents, and thus be able to reject entire blocks at times??
		const Vector_Event_Delta_t& theVector = current->GetDeltas()->GetVector();
		for (Vector_Event_Delta_t::const_iterator
			ii = theVector.begin(), theEnd = theVector.end(); ii != theEnd; ++ii)
			{
			if (not ii->first->LessEqual(iEvent))
				{
				// ii->first is after iEvent, or is concurrent with it.
				resultVector.push_back(*ii);
				oEvent = oEvent->Joined(ii->first);
				}
			}
		}
	oDeltas = new Deltas(&resultVector);
	}

ZRef<DeltasChain> DatonSet::GetDeltasChain(ZRef<Event>* oEvent)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	if (oEvent)
		*oEvent = fEvent;

	return fDeltasChain;
	}

void DatonSet::pCommit()
	{
	if (fPendingStatements.empty())
		return;

	fEvent = fEvent->Advanced(fIdentity);

	const ZRef<Delta> theDelta = new Delta(&fPendingStatements);

	Vector_Event_Delta_t theVector;
	theVector.push_back(Vector_Event_Delta_t::value_type(fEvent, theDelta));

	ZRef<Deltas> theDeltas = new Deltas(&theVector);

	fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);

	fEvent = fEvent->Advanced(fIdentity);
	}

} // namespace ZDatonSet
} // namespace ZooLib
