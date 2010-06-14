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
	{
	fForks.push_back(iFork);
	}

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

NamedClock::NamedClock()
	{}

NamedClock::NamedClock(const NamedClock& iOther)
:	fNombre(iOther.fNombre)
,	fClock(iOther.fClock)
	{}

NamedClock::~NamedClock()
	{}

NamedClock& NamedClock::operator=(const NamedClock& iOther)
	{
	fNombre = iOther.fNombre;
	fClock = iOther.fClock;
	return *this;
	}

NamedClock::NamedClock(const Nombre& iNombre, Clock iClock)
:	fNombre(iNombre)
,	fClock(iClock)
	{}

bool NamedClock::operator<(const NamedClock& iRHS) const
	{
	const bool aleb = fClock.LE(iRHS.fClock);
	const bool blea = iRHS.fClock.LE(fClock);

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

Clock NamedClock::GetClock() const
	{ return fClock; }

string NamedClock::AsString() const
	{ return fNombre.AsString() + ":" /*+ ZStringf("%llu", fClock)*/; }

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

ClockedDeltas::ClockedDeltas(const Map_NamedClock_Delta_t& iMap)
:	fMap(iMap)
	{}

ClockedDeltas::ClockedDeltas(Map_NamedClock_Delta_t* ioMap)
	{ ioMap->swap(fMap); }

const Map_NamedClock_Delta_t& ClockedDeltas::GetMap()
	{ return fMap; }

// =================================================================================================
#pragma mark -
#pragma mark * 

ClockedDeltasChain::ClockedDeltasChain(
	ZRef<ClockedDeltasChain> iParent, ZRef<ClockedDeltas> iClockedDeltas)
:	fParent(iParent)
,	fClockedDeltas(iClockedDeltas)
	{}

ZRef<ClockedDeltasChain> ClockedDeltasChain::GetParent()
	{ return fParent; }

ZRef<ClockedDeltas> ClockedDeltasChain::GetClockedDeltas()
	{ return fClockedDeltas; }

// =================================================================================================
#pragma mark -
#pragma mark * 

Dataset::Dataset(const Nombre& iNombre, Clock iClock, ZRef<ClockedDeltasChain> iClockedDeltasChain)
:	fNombre(iNombre)
,	fNextFork(1)
,	fClock(iClock)
,	fClockedDeltasChain(iClockedDeltasChain)
	{}

Dataset::Dataset(const Nombre& iNombre, Clock iClock)
:	fNombre(iNombre)
,	fNextFork(1)
,	fClock(iClock)
	{}

void Dataset::Insert(Daton iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = true;
	}

void Dataset::Erase(Daton iDaton)
	{
	ZAcqMtx acq(fMtx);

	fPendingStatements[iDaton] = false;
	}

Clock Dataset::GetClock()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return fClock.Peek();
	}

ZRef<Dataset> Dataset::Fork()
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	return new Dataset(Nombre(fNombre, fNextFork++), fClock.Fork(), fClockedDeltasChain);
	}

void Dataset::Join(ZRef<Dataset> iOther)
	{
	Clock otherClock;
	ZRef<ClockedDeltas> theClockedDeltas;
	iOther->GetClockedDeltas(this->GetClock(), otherClock, theClockedDeltas);
	this->IncorporateClockedDeltas(otherClock, theClockedDeltas);
	}

void Dataset::GetClockedDeltas(
	const Clock& iClock, Clock& oClock, ZRef<ClockedDeltas>& oClockedDeltas)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();

	const NamedClock tsLower(Nombre(), iClock);

	Map_NamedClock_Delta_t resultMap;
	for (ZRef<ClockedDeltasChain> current = fClockedDeltasChain;
		current; current = current->GetParent())
		{
		const Map_NamedClock_Delta_t& theMap = current->GetClockedDeltas()->GetMap();
		for (Map_NamedClock_Delta_t::const_iterator
			i = theMap.lower_bound(tsLower), theEnd = theMap.end(); i != theEnd; ++i)
			{
			const Clock candidate = (*i).first.GetClock();
			const bool cles = candidate.LE(iClock);
			const bool slec = iClock.LE(candidate);
			if (!cles || slec)
				resultMap.insert(*i);
			}
		}

	oClockedDeltas = new ClockedDeltas(&resultMap);
	oClock = fClock.Peek();
	}

void Dataset::IncorporateClockedDeltas(
	const Clock& iClock, ZRef<ClockedDeltas> iDelta)
	{
	ZAcqMtx acq(fMtx);
	this->pCommit();
	fClockedDeltasChain = new ClockedDeltasChain(fClockedDeltasChain, iDelta);
	fClock.Join(iClock);
	}

typedef pair<NamedClock, size_t> TSIndex_t;

class CompareTSReverse
:	public std::binary_function<TSIndex_t, TSIndex_t, bool>
	{
public:
	// We want oldest ts first, so we reverse the sense of the compare.
	result_type operator()(const first_argument_type& i1, const second_argument_type& i2) const
		{ return i1.first < i2.first; }
	};

set<Daton> Dataset::GetComposed()
	{
	// This is a bit tricky. We're effectively merge sorting from our history entries.

	ZAcqMtx acq(fMtx);
	this->pCommit();

	vector<Map_NamedClock_Delta_t::const_iterator> theIters, theEnds;
	for (ZRef<ClockedDeltasChain> current = fClockedDeltasChain;
		current; current = current->GetParent())
		{
		Map_NamedClock_Delta_t::const_iterator theBegin =
			current->GetClockedDeltas()->GetMap().begin();

		Map_NamedClock_Delta_t::const_iterator theEnd =
			current->GetClockedDeltas()->GetMap().end();

		if (theBegin != theEnd)
			{
			theIters.push_back(theBegin);
			theEnds.push_back(theEnd);
			}
		}

	priority_queue<TSIndex_t, vector<TSIndex_t>, CompareTSReverse> queue;

	for (size_t x = 0; x < theIters.size(); ++x)
		queue.push(TSIndex_t((*theIters[x]).first, x));

	set<Daton> result;
	while (!queue.empty())
		{
		const size_t theX = queue.top().second;
		queue.pop();

		ZRef<Delta> theDelta = (*theIters[theX]).second;

		if (++theIters[theX] != theEnds[theX])
			queue.push(TSIndex_t((*theIters[theX]).first, theX));

		const map<Daton, bool>& theStatements = theDelta->GetStatements();
		for (map<Daton, bool>::const_iterator j = theStatements.begin();
			j != theStatements.end(); ++j)
			{
			if ((*j).second)
				result.insert((*j).first);
			else
				result.erase((*j).first);
			}
		}

	return result;
	}

const Nombre& Dataset::GetNombre()
	{ return fNombre; }

ZRef<ClockedDeltasChain> Dataset::GetClockedDeltasChain()
	{ return fClockedDeltasChain; }

void Dataset::pCommit()
	{
	if (!fPendingStatements.empty())
		{
		fClock.Event();
		
		ZRef<Delta> theDelta = new Delta(&fPendingStatements);

		Map_NamedClock_Delta_t theMap;
		theMap.insert(
			Map_NamedClock_Delta_t::value_type(NamedClock(fNombre, fClock.Peek()), theDelta));

		ZRef<ClockedDeltas> theClockedDeltas = new ClockedDeltas(&theMap);

		fClockedDeltasChain = new ClockedDeltasChain(fClockedDeltasChain, theClockedDeltas);

		fClock.Event();
		}
	}

} // namespace ZDataset
} // namespace ZooLib
