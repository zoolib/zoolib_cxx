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

#include "zoolib/dataspace/ZDataspace_Source.h"

namespace ZooLib {
namespace ZDataspace {

using std::set;

// =================================================================================================
// MARK: - AddedQuery

AddedQuery::AddedQuery(int64 iRefcon, const ZRef<ZRA::Expr_Rel>& iRel)
:	fRefcon(iRefcon)
,	fRel(iRel)
	{}

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

int64 AddedQuery::GetRefcon() const
	{ return fRefcon; }

ZRef<ZRA::Expr_Rel> AddedQuery::GetRel() const
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

QueryResult::QueryResult
	(int64 iRefcon, const ZRef<ZQE::Result>& iResult, const ZRef<Event>& iEvent)
:	fRefcon(iRefcon)
,	fResult(iResult)
,	fEvent(iEvent)
	{}

int64 QueryResult::GetRefcon() const
	{ return fRefcon; }

ZRef<ZQE::Result> QueryResult::GetResult() const
	{ return fResult; }

ZRef<Event> QueryResult::GetEvent() const
	{ return fEvent; }

// =================================================================================================
// MARK: - Source

Source::Source()
:	fCalled_ResultsAvailable(false)
	{}

Source::~Source()
	{}

void Source::SetCallable_ResultsAvailable(ZRef<Callable_ResultsAvailable> iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCalled_ResultsAvailable = false;
	fCallable_ResultsAvailable = iCallable;
	}

void Source::pCollectResultsCalled()
	{
	ZAcqMtx acq(fMtx);
	fCalled_ResultsAvailable = false;
	}

void Source::pTriggerResultsAvailable()
	{
	ZGuardRMtx guard(fMtx);
	if (!fCalled_ResultsAvailable++)
		{
		if (ZRef<Callable_ResultsAvailable> theCallable = fCallable_ResultsAvailable)
			{
			guard.Release();
			theCallable->Call(this);
			}
		}
	}

// =================================================================================================
// MARK: - SourceFactory

SourceFactory::SourceFactory()
	{}

SourceFactory::~SourceFactory()
	{}

} // namespace ZDataspace
} // namespace ZooLib
