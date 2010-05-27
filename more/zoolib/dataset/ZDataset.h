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
#include "zoolib/ZThread.h"

#include "zoolib/dataset/ZDataset_Daton.h"

#include "ZIntervalTreeClock.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace ZooLib {
namespace ZDataset {

using namespace std;

typedef ZIntervalTreeClock::Clock Clock;

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

	string AsString() const;

private:
	vector<uint64> fForks;
	};

// =================================================================================================
#pragma mark -
#pragma mark * NamedClock

class NamedClock
	{
public:
	NamedClock();
	NamedClock(const NamedClock& iOther);
	~NamedClock();
	NamedClock& operator=(const NamedClock& iOther);
	
	NamedClock(const Nombre& iNombre, Clock iClock);

	bool operator<(const NamedClock& iRHS) const;

	string AsString() const;

	Clock GetClock() const;

private:
	Nombre fNombre;
	Clock fClock;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Delta

class Delta : public ZCountedWithoutFinalize
	{
public:
	Delta(const map<Daton, bool>& iStatements);
	Delta(map<Daton, bool>& ioStatements, bool iIKnowWhatImDoing);

	const map<Daton, bool>& GetStatements();

private:
	map<Daton, bool> fStatements;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Map_NamedClock_Delta_t

typedef map<NamedClock, ZRef<Delta> > Map_NamedClock_Delta_t;

// =================================================================================================
#pragma mark -
#pragma mark * ClockedDeltas

class ClockedDeltas : public ZCountedWithoutFinalize
	{
public:
	ClockedDeltas(const Map_NamedClock_Delta_t& iMap);
	ClockedDeltas(Map_NamedClock_Delta_t& ioMap, bool iKnowWhatImDoing);

	const Map_NamedClock_Delta_t& GetMap();

private:
	Map_NamedClock_Delta_t fMap;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ClockedDeltasChain

class ClockedDeltasChain : public ZCountedWithoutFinalize
	{
public:
	ClockedDeltasChain(ZRef<ClockedDeltasChain> iParent, ZRef<ClockedDeltas> iClockedDeltas);

	ZRef<ClockedDeltasChain> GetParent();
	ZRef<ClockedDeltas> GetClockedDeltas();

private:
	ZRef<ClockedDeltasChain> fParent;
	ZRef<ClockedDeltas> fClockedDeltas;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Dataset

class Dataset : public ZCounted
	{
public:
	Dataset(const Nombre& iNombre, Clock iClock);

	void Insert(Daton iDaton);
	void Erase(Daton iDaton);

	Clock GetClock();

	ZRef<Dataset> Fork();

	void Join(ZRef<Dataset> iOther);

	void GetClockedDeltas(const Clock& iClock, Clock& oClock, ZRef<ClockedDeltas>& oClockedDeltas);

	void IncorporateClockedDeltas(const Clock& iClock, ZRef<ClockedDeltas> iClockedDeltas);

	set<Daton> GetComposed();

	const Nombre& GetNombre();
	ZRef<ClockedDeltasChain> GetClockedDeltasChain();

private:
	Dataset(const Nombre& iNombre, Clock iClock, ZRef<ClockedDeltasChain> iClockedDeltasChain);

	void pCommit();

	ZMtx fMtx; // RWLock?
	const Nombre fNombre;
	uint64 fNextFork;
	Clock fClock;
	map<Daton, bool> fPendingStatements;
	ZRef<ClockedDeltasChain> fClockedDeltasChain;
	};

} // namespace ZDataset
} // namespace ZooLib

#endif // __ZDataset__
