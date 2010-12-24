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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZLog.h"
#include "zoolib/dataspace/ZDataspace_Dataspace.h"

namespace ZooLib {
namespace ZDataspace {

using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * PSieve

class Dataspace::PSieve
	{
public:
	PSieve()
		{}
		
	~PSieve()
		{}

	Dataspace* fDataspace;
	ZRef<ZRA::Expr_Rel> fRel;
	int64 fRefcon;

	DListHead<DLink_Sieve_Using> fSieves_Using;

	ZRef<ZQE::Result> fResult;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Dataspace

Dataspace::Dataspace(ZRef<Source> iSource)
:	fSource(iSource)
,	fCallable_Source(MakeCallable(this, &Dataspace::pCallback_Source))
,	fCalled_UpdateNeeded(false)
,	fNextRefcon(1)
	{
	fSource->SetCallable_ResultsAvailable(fCallable_Source);
	}

Dataspace::~Dataspace()
	{
	fSource->SetCallable_ResultsAvailable(null);
	}

void Dataspace::SetCallable_UpdateNeeded(ZRef<Callable_UpdateNeeded> iCallable)
	{
	ZAcqMtxR acq(fMtxR);
	fCallable_UpdateNeeded = iCallable;
	}

void Dataspace::Register(ZRef<Sieve> iSieve, const ZRef<ZRA::Expr_Rel>& iRel)
	{
	ZAssert(!iSieve->fPSieve);
	int64 theRefcon = 0;

	{
	ZAcqMtxR acq(fMtxR);

	PSieve* thePSieve;
	map<ZRef<ZRA::Expr_Rel>, PSieve>::iterator position = fRel_To_PSieve.lower_bound(iRel);
	if (position != fRel_To_PSieve.end() && position->first == iRel)
		{
		thePSieve = &position->second;
		}
	else
		{
		theRefcon = fNextRefcon++;

		thePSieve = &fRel_To_PSieve.
			insert(position, pair<ZRef<ZRA::Expr_Rel>, PSieve>(iRel, PSieve()))->second;

		ZUtil_STL::sInsertMustNotContain(kDebug, fRefcon_To_PSieveStar, theRefcon, thePSieve);

		thePSieve->fDataspace = this;
		thePSieve->fRel = iRel;
		thePSieve->fRefcon = theRefcon;
		}

	Sieve* theSieve = iSieve.Get();
	theSieve->fPSieve = thePSieve;
	thePSieve->fSieves_Using.Insert(theSieve);
	}

	if (theRefcon)
		{
		AddedSearch theAddedSearch(theRefcon, iRel);
		fSource->ModifyRegistrations(&theAddedSearch, 1, nullptr, 0);
		}
	}

void Dataspace::Update()
	{
	vector<SearchResult> theSearchResults;
	fSource->CollectResults(theSearchResults);

	set<ZRef<Sieve> > sievesChanged;
	set<ZRef<Sieve> > sievesLoaded;

	{
	ZAcqMtxR acq(fMtxR);
	fCalled_UpdateNeeded = false;

	if (theSearchResults.empty())
		return;
	
	for (vector<SearchResult>::iterator iterSearchResults = theSearchResults.begin();
		iterSearchResults != theSearchResults.end(); ++iterSearchResults)
		{
		map<int64, PSieve*>::iterator iterPSieve =
			fRefcon_To_PSieveStar.find(iterSearchResults->GetRefcon());

		if (fRefcon_To_PSieveStar.end() == iterPSieve)
			continue;

		PSieve* thePSieve = iterPSieve->second;
		thePSieve->fResult = iterSearchResults->GetResult();

		for (DListIterator<Sieve, DLink_Sieve_Using>
			iter = thePSieve->fSieves_Using;
			iter; iter.Advance())
			{
			Sieve* theSieve = iter.Current();
			if (theSieve->fJustRegistered)
				{
				theSieve->fJustRegistered = false;
				sievesLoaded.insert(theSieve);
				}
			else
				{
				sievesChanged.insert(theSieve);
				}
			}
		}
	}

	this->Updated(sievesLoaded, sievesChanged);
	}

void Dataspace::Updated(
	const set<ZRef<Sieve> >& iSievesLoaded,
	const set<ZRef<Sieve> >& iSievesChanged)
	{
	this->Loaded(iSievesLoaded);
	this->Changed(iSievesChanged);
	}

void Dataspace::Loaded(const set<ZRef<Sieve> >& iSieves)
	{
	for (set<ZRef<Sieve> >::const_iterator iter = iSieves.begin(), end = iSieves.end();
		iter != end; ++iter)
		{ this->Loaded(*iter); }
	}

void Dataspace::Changed(const set<ZRef<Sieve> >& iSieves)
	{
	for (set<ZRef<Sieve> >::const_iterator iter = iSieves.begin(), end = iSieves.end();
		iter != end; ++iter)
		{ this->Changed(*iter); }
	}

void Dataspace::Loaded(const ZRef<Sieve> & iSieve)
	{
	try { iSieve->Loaded(); }
	catch (...) {}
	}

void Dataspace::Changed(const ZRef<Sieve> & iSieve)
	{
	try { iSieve->Changed(); }
	catch (...) {}
	}

void Dataspace::pTriggerUpdate()
	{
	ZGuardRMtxR guard(fMtxR);
	if (!fCalled_UpdateNeeded)
		{
		fCalled_UpdateNeeded = true;
		if (ZRef<Callable_UpdateNeeded> theCallable = fCallable_UpdateNeeded)
			{
			guard.Release();
			theCallable->Call(this);
			}
		}
	}

void Dataspace::pCallback_Source(ZRef<Source> iSource)
	{ this->pTriggerUpdate(); }

void Dataspace::pFinalize(Sieve* iSieve)
	{
	ZGuardRMtxR guard(fMtxR);

	if (!iSieve->FinishFinalize())
		return;

	PSieve* const thePSieve = iSieve->fPSieve;
	
	thePSieve->fSieves_Using.Erase(iSieve);
	iSieve->fPSieve = nullptr;
	delete iSieve;
	
	if (thePSieve->fSieves_Using.Empty())
		{
		int64 const theRefcon = thePSieve->fRefcon;
		ZUtil_STL::sEraseMustContain(kDebug, fRel_To_PSieve, thePSieve->fRel);
		ZUtil_STL::sEraseMustContain(kDebug, fRefcon_To_PSieveStar, theRefcon);
		guard.Release();

		fSource->ModifyRegistrations(nullptr, 0, &theRefcon, 1);
		}
	}

ZRef<ZQE::Result> Dataspace::pGetResult(Sieve* iSieve)
	{ return iSieve->fPSieve->fResult; }

bool Dataspace::pIsLoaded(Sieve* iSieve)
	{ return iSieve->fPSieve->fResult; }

// =================================================================================================
#pragma mark -
#pragma mark * Sieve

Sieve::Sieve()
:	fPSieve(nullptr)
,	fJustRegistered(true)
	{}

Sieve::~Sieve()
	{
	ZAssert(!fPSieve);
	}

void Sieve::Finalize()
	{
	if (fPSieve)
		fPSieve->fDataspace->pFinalize(this);
	else
		ZCounted::Finalize();
	}

void Sieve::Changed()
	{ this->Changed(false); }

void Sieve::Loaded()
	{ this->Changed(true); }

void Sieve::Changed(bool iIsLoad)
	{}

ZRef<ZQE::Result> Sieve::GetResult()
	{
	if (fPSieve)
		return fPSieve->fDataspace->pGetResult(this);
	return null;
	}

bool Sieve::IsLoaded()
	{
	if (fPSieve)
		return fPSieve->fDataspace->pIsLoaded(this);
	return false;
	}

// =================================================================================================
#pragma mark -
#pragma mark * Sieve_Callable

Sieve_Callable::Sieve_Callable(ZRef<Callable> iCallable)
:	fCallable(iCallable)
	{}

void Sieve_Callable::Changed(bool iIsLoad)
	{
	if (ZRef<Callable> theCallable = fCallable)
		theCallable->Call(this, iIsLoad);
	}

} // namespace ZDataspace
} // namespace ZooLib
