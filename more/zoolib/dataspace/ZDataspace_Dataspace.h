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

#ifndef __ZDataspace_Dataspace_h__
#define __ZDataspace_Dataspace_h__ 1
#include "zconfig.h"

#include "zoolib/ZThread.h"
#include "zoolib/dataspace/ZDataspace_Source.h"

#include <map>

namespace ZooLib {
namespace ZDataspace {

class Sieve;

// =================================================================================================
// MARK: - Dataspace

class Dataspace
	{
public:
	enum { kDebug = 1 };

	Dataspace(ZRef<Source> iSource);
	~Dataspace();

	typedef ZCallable<void(Dataspace*)> Callable_UpdateNeeded;
	void SetCallable_UpdateNeeded(ZRef<Callable_UpdateNeeded> iCallable);

	void Register(ZRef<Sieve> iSieve, const ZRef<ZRA::Expr_Rel>& iRel);

	void Update();

// These methods *can* be overridden, but it's by no means essential.
	virtual void Updated(
		const std::set<ZRef<Sieve> >& iSievesLoaded,
		const std::set<ZRef<Sieve> >& iSievesChanged);

	virtual void Loaded(const std::set<ZRef<Sieve> >& iSieves);
	virtual void Changed(const std::set<ZRef<Sieve> >& iSieves);

	virtual void Loaded(const ZRef<Sieve>& iSieve);
	virtual void Changed(const ZRef<Sieve>& iSieve);

private:
	void pCallback_Source(ZRef<Source> iSource);
	void pFinalize(Sieve* iSieve);

	ZRef<Source> fSource;

	ZMtxR fMtxR;

	ZRef<Callable_UpdateNeeded> fCallable_UpdateNeeded;
	bool fCalled_UpdateNeeded;

	int64 fNextRefcon;

	typedef std::map<int64, Sieve*> Map_RefconToSieve;
	Map_RefconToSieve fMap_RefconToSieve;

	friend class Sieve;
	};

// =================================================================================================
// MARK: - Sieve

class Sieve
:	public ZCounted
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

	ZRef<ZRA::Expr_Rel> GetRel();
	ZRef<ZQE::Result> GetResult();
	ZRef<Event> GetEvent();

	bool IsLoaded();

private:
	Dataspace* fDataspace;
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	ZRef<ZQE::Result> fResult;
	ZRef<Event> fEvent;

	friend class Dataspace;
	};

// =================================================================================================
// MARK: - Sieve_Callable

class Sieve_Callable : public Sieve
	{
public:
	typedef ZCallable<void(ZRef<Sieve>, bool)> Callable;
	Sieve_Callable(ZRef<Callable> iCallable);

// From Sieve
	using Sieve::Changed;
	virtual void Changed(bool iIsLoad);

private:
	ZRef<Callable> fCallable;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Dataspace_h__
