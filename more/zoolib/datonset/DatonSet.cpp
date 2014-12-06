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

#include "zoolib/Compare.h"
#include "zoolib/Util_STL_vector.h"

#include "zoolib/ZLog.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_Strim_IntervalTreeClock.h"

#include "zoolib/datonset/DatonSet.h"

#include <queue>

// =================================================================================================
// MARK: - sCompare_T

namespace ZooLib {

template <>
int sCompare_T<DatonSet::Daton>(const DatonSet::Daton& iL, const DatonSet::Daton& iR)
	{ return sCompare_T(iL.GetData(), iR.GetData()); }

ZMACRO_CompareRegistration_T(DatonSet::Daton)

} // namespace ZooLib

namespace ZooLib {
namespace DatonSet {

using namespace std;
using namespace Util_STL;

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

Daton::Daton(ZData_Any iData)
:	fData(iData)
	{}

bool Daton::operator==(const Daton& iOther) const
	{ return fData == iOther.fData; }

bool Daton::operator<(const Daton& iOther) const
	{ return fData < iOther.fData; }

ZData_Any Daton::GetData() const
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

ZRef<DatonSet> DatonSet::Fork()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	ZRef<Identity> newRight = sSplit(fIdentity);
	fEvent = fEvent->Advanced(fIdentity);
	ZAssert(fEvent);

	ZRef<DatonSet> newDS = new DatonSet(newRight, fEvent->Advanced(newRight), fDeltasChain);

	return newDS;
	}

bool DatonSet::Join(ZRef<DatonSet>& ioOther)
	{
	ZGuardMtx guard(fMtx);
	this->pCommit();
	ZRef<Event> theEvent = fEvent;
	guard.Release();

	ZRef<Deltas> theDeltas = ioOther->GetDeltas(theEvent);

	ZRef<Identity> otherIdentity = ioOther->fIdentity;

	// Now wipe out ioOther.
	ioOther->fIdentity.Clear();
	ioOther->fEvent.Clear();
	ioOther->fPendingStatements.clear();
	ioOther->fDeltasChain.Clear();
	ioOther.Clear();

	guard.Acquire();

	if (theDeltas && theDeltas->GetVector().size())
		{
		foreachi (iter, theDeltas->GetVector())
			{
			fEvent = fEvent->Joined(iter->first);
			ZAssert(fEvent);
			}
		fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);
		}

	fIdentity = fIdentity->Summed(otherIdentity); //?? Check this

	fEvent = fEvent->Advanced(fIdentity);
	ZAssert(fEvent);

	return sNotEmpty(theDeltas->GetVector());
	}

bool DatonSet::IncorporateDeltas(ZRef<Deltas> iDeltas)
	{
	ZGuardMtx guard(fMtx);
	this->pCommit();
	if (iDeltas && iDeltas->GetVector().size())
		{
		foreachi (iter, iDeltas->GetVector())
			{
			fEvent = fEvent->Joined(iter->first);
			ZAssert(fEvent);
			}
		fDeltasChain = new DeltasChain(fDeltasChain, iDeltas);
		}

	fEvent = fEvent->Advanced(fIdentity);

	ZAssert(fEvent);

	return sNotEmpty(iDeltas->GetVector());
	}

namespace { // anonymous
struct Accumulator_Join
	{
	void operator()(ZRef<Event>& io0, const ZRef<Event>& i1) const
		{ io0 = io0 ? io0->Joined(i1) : i1; }
	};
} // anonymous namespace

ZRef<Deltas> DatonSet::GetDeltas(ZRef<Event> iEvent)
	{
	ZGuardMtx guard(fMtx);
	this->pCommit();

	Vector_Event_Delta_t resultVector;
	ZRef<DeltasChain> current = fDeltasChain;
	guard.Release();

	for (/*no init*/; current; current = current->GetParent())
		{
		// Actually, maybe indexing can work -- we've got indeterminate ordering, which
		// may be unstable, but once a sequence firms up it doesn't go squishy again. That
		// may be enough to give us more efficient lookup than a linear scan.
		const Vector_Event_Delta_t& theVector = current->GetDeltas()->GetVector();
		for (Vector_Event_Delta_t::const_iterator
			ii = theVector.begin(), theEnd = theVector.end(); ii != theEnd; ++ii)
			{
			if (not ii->first->LessEqual(iEvent))
				{
				// ii->first is after iEvent, or is concurrent with it.
				resultVector.push_back(*ii);
				}
			}
		}
	return new Deltas(&resultVector);
	}

void DatonSet::pCommit()
	{
	if (fPendingStatements.empty())
		return;

	fEvent = fEvent->Advanced(fIdentity);

	ZAssert(fEvent);

	const ZRef<Delta> theDelta = new Delta(&fPendingStatements);

	Vector_Event_Delta_t theVector;
	theVector.push_back(Vector_Event_Delta_t::value_type(fEvent, theDelta));

	ZRef<Deltas> theDeltas = new Deltas(&theVector);

	fDeltasChain = new DeltasChain(fDeltasChain, theDeltas);

	fEvent = fEvent->Advanced(fIdentity);

	ZAssert(fEvent);
	}

} // namespace DatonSet
} // namespace ZooLib
