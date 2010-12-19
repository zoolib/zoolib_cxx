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

#ifndef __ZDataspace_Dataspace__
#define __ZDataspace_Dataspace__ 1
#include "zconfig.h"

#include "zoolib/ZDList.h"
#include "zoolib/ZThread.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include <map>

namespace ZooLib {
namespace ZDataspace {

class Dataspace;
class Sieve;
class DLink_Sieve_JustRegistered;

// =================================================================================================
#pragma mark -
#pragma mark * Dataspace

class Dataspace
	{
public:
	enum { kDebug = 1 };

	typedef ZCallable<void(Dataspace*)> Callable_UpdateNeeded;

	Dataspace(Source* iSource);
	~Dataspace();

	void SetCallable_LocalUpdateNeeded(ZRef<Callable_UpdateNeeded> iCallable);
	void SetCallable_SourceUpdateNeeded(ZRef<Callable_UpdateNeeded> iCallable);

	void Register(ZRef<Sieve> iSieve, const ZRef<ZRA::Expr_Rel>& iRel);

	void LocalUpdate();
	void SourceUpdate();

// These methods *can* be overridden, but it's by no means essential.
	virtual void Updated(
		const std::set<ZRef<Sieve> >& iSievesLoaded,
		const std::set<ZRef<Sieve> >& iSievesChanged);

	virtual void Loaded(const std::set<ZRef<Sieve> >& iSieves);
	virtual void Changed(const std::set<ZRef<Sieve> >& iSieves);

	virtual void Loaded(const ZRef<Sieve> & iSieve);
	virtual void Changed(const ZRef<Sieve> & iSieve);

	const ZMtxR& GetMtxR_CallSourceUpdate()	
		{ return fMtxR_CallSourceUpdate; }
private:
	void pTriggerLocalUpdate();
	void pTriggerSourceUpdate();
	void pCallback_Source(Source* iSource);

	Source* fSource;
	ZRef<Source::Callable> fCallable_Source;
	
	ZMtxR fMtxR_CallLocalUpdate;
	ZMtxR fMtxR_CallSourceUpdate;
	ZMtxR fMtxR_Structure;

	ZRef<ZCallable<void(Dataspace*)> > fCallable_LocalUpdateNeeded;
	ZRef<ZCallable<void(Dataspace*)> > fCallable_SourceUpdateNeeded;

	bool fCalled_LocalUpdateNeeded;
	bool fCalled_SourceUpdateNeeded;

	class DLink_PSieve_LocalUpdate;
	class DLink_PSieve_SourceUpdate;
	class DLink_PSieve_Changed;	
	class PSieve;

	DListHead<DLink_PSieve_LocalUpdate> fPSieves_LocalUpdate;
	DListHead<DLink_PSieve_SourceUpdate> fPSieves_SourceUpdate;
	DListHead<DLink_PSieve_Changed> fPSieves_Changed;

	std::map<ZRef<ZRA::Expr_Rel>, PSieve> fRel_To_PSieve;
	std::map<int64, PSieve*> fRefcon_To_PSieveStar;
	int64 fNextRefcon;

	void pFinalize(Sieve* iSieve);
	ZRef<ZQE::Result> pGetSearchRows(Sieve* iSieve);
	bool pIsLoaded(Sieve* iSieve);

	DListHead<DLink_Sieve_JustRegistered> fSieves_JustRegistered;

	friend class Sieve;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Sieve

class DLink_Sieve_Using
:	public DListLink<Sieve, DLink_Sieve_Using>
	{};

class DLink_Sieve_JustRegistered
:	public DListLink<Sieve, DLink_Sieve_JustRegistered>
	{};

class Sieve
:	public ZCounted
,	public DLink_Sieve_Using
,	public DLink_Sieve_JustRegistered
	{
public:
	Sieve();
	virtual ~Sieve();

// From ZCounted
	virtual void Finalize();

// Our protocol
	virtual void Changed();
	virtual void Loaded();
	virtual void Changed(bool iIsLoad);

	ZRef<ZQE::Result> GetSearchRows();

	bool IsLoaded();

private:
	Dataspace::PSieve* fPSieve;
	
	friend class Dataspace;
	};

// =================================================================================================
#pragma mark -
#pragma mark * Sieve_Callable

class Sieve_Callable : public Sieve
	{
public:
	typedef ZCallable<void(ZRef<Sieve>, bool)> Callable;
	Sieve_Callable(ZRef<Callable> iCallable);

// From Sieve
	virtual void Changed(bool iIsLoad);

private:
	ZRef<Callable> fCallable;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Dataspace__
