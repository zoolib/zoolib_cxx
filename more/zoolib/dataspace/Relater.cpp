/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#include "zoolib/dataspace/Relater.h"

namespace ZooLib {
namespace Dataspace {

using std::set;

// =================================================================================================
// MARK: - AddedQuery

AddedQuery::AddedQuery()
	{}

AddedQuery::AddedQuery(const AddedQuery& iOther)
:	fRefcon(iOther.fRefcon)
,	fRel(iOther.fRel)
	{}

AddedQuery::~AddedQuery()
	{}

AddedQuery& AddedQuery::operator=(const AddedQuery& iOther)
	{
	fRefcon = iOther.fRefcon;
	fRel = iOther.fRel;
	return *this;
	}

AddedQuery::AddedQuery(int64 iRefcon, const ZRef<RelationalAlgebra::Expr_Rel>& iRel)
:	fRefcon(iRefcon)
,	fRel(iRel)
	{}

int64 AddedQuery::GetRefcon() const
	{ return fRefcon; }

ZRef<RelationalAlgebra::Expr_Rel> AddedQuery::GetRel() const
	{ return fRel; }

// =================================================================================================
// MARK: - QueryResult

QueryResult::QueryResult()
:	fRefcon(0)
	{}

QueryResult::QueryResult(const QueryResult& iOther)
:	fRefcon(iOther.fRefcon)
,	fResult(iOther.fResult)
,	fEvent(iOther.fEvent)
	{}

QueryResult::~QueryResult()
	{}

QueryResult& QueryResult::operator=(const QueryResult& iOther)
	{
	fRefcon = iOther.fRefcon;
	fResult = iOther.fResult;
	fEvent = iOther.fEvent;
	return *this;
	}

QueryResult::QueryResult(
	int64 iRefcon, const ZRef<QueryEngine::Result>& iResult, const ZRef<Event>& iEvent)
:	fRefcon(iRefcon)
,	fResult(iResult)
,	fEvent(iEvent)
	{}

int64 QueryResult::GetRefcon() const
	{ return fRefcon; }

ZRef<QueryEngine::Result> QueryResult::GetResult() const
	{ return fResult; }

ZRef<Event> QueryResult::GetEvent() const
	{ return fEvent; }

// =================================================================================================
// MARK: - Relater

Relater::Relater()
:	fCalled_ResultsAvailable(false)
	{}

Relater::~Relater()
	{}

void Relater::SetCallable_ResultsAvailable(ZRef<Callable_ResultsAvailable> iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCalled_ResultsAvailable = false;
	fCallable_ResultsAvailable = iCallable;
	}

void Relater::pCollectResultsCalled()
	{
	ZAcqMtx acq(fMtx);
	fCalled_ResultsAvailable = false;
	}

void Relater::pTriggerResultsAvailable()
	{
	ZGuardMtx guard(fMtx);
	if (not sGetSet(fCalled_ResultsAvailable, true))
		{
		if (ZRef<Callable_ResultsAvailable> theCallable = fCallable_ResultsAvailable)
			{
			guard.Release();
			theCallable->Call(this);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
