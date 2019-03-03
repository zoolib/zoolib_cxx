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

#include "zoolib/Dataspace/Searcher.h"

#include "zoolib/Compare_Ref.h"
#include "zoolib/Log.h"

#include "zoolib/ZMACRO_foreach.h"

#include "zoolib/RelationalAlgebra/GetRelHead.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - SearchSpec

SearchSpec::SearchSpec()
	{}

SearchSpec::SearchSpec(const SearchSpec& iOther)
:	fConcreteHead(iOther.fConcreteHead)
,	fRestriction(iOther.fRestriction)
	{}

SearchSpec::~SearchSpec()
	{}

SearchSpec SearchSpec::operator=(const SearchSpec& iOther)
	{
	fConcreteHead = iOther.fConcreteHead;
	fRestriction = iOther.fRestriction;
	return *this;
	}

SearchSpec::SearchSpec(const ConcreteHead& iConcreteHead,
	const ZRef<Expr_Bool>& iRestriction)
:	fConcreteHead(iConcreteHead)
,	fRestriction(iRestriction)
	{}

bool SearchSpec::operator==(const SearchSpec& iOther) const
	{
	return fConcreteHead == iOther.fConcreteHead
		&& 0 == sCompare_Ref_T(fRestriction, iOther.fRestriction);
	}

bool SearchSpec::operator<(const SearchSpec& iOther) const
	{
	if (fConcreteHead < iOther.fConcreteHead)
		return true;

	if (iOther.fConcreteHead < fConcreteHead)
		return false;

	return sCompare_Ref_T(fRestriction, iOther.fRestriction) < 0;
	}

const ConcreteHead& SearchSpec::GetConcreteHead() const
	{ return fConcreteHead; }

const ZRef<Expr_Bool>& SearchSpec::GetRestriction() const
	{ return fRestriction; }

// =================================================================================================
#pragma mark - AddedSearch

AddedSearch::AddedSearch()
	{}

AddedSearch::AddedSearch(const AddedSearch& iOther)
:	fRefcon(iOther.fRefcon)
,	fSearchSpec(iOther.fSearchSpec)
	{}

AddedSearch::~AddedSearch()
	{}

AddedSearch& AddedSearch::operator=(const AddedSearch& iOther)
	{
	fRefcon = iOther.fRefcon;
	fSearchSpec = iOther.fSearchSpec;
	return *this;
	}

AddedSearch::AddedSearch(int64 iRefcon, const SearchSpec& iSearchSpec)
:	fRefcon(iRefcon)
,	fSearchSpec(iSearchSpec)
	{}

int64 AddedSearch::GetRefcon() const
	{ return fRefcon; }

const SearchSpec& AddedSearch::GetSearchSpec() const
	{ return fSearchSpec; }

// =================================================================================================
#pragma mark - SearchResult

SearchResult::SearchResult()
:	fRefcon(0)
	{}

SearchResult::SearchResult(const SearchResult& iOther)
:	fRefcon(iOther.fRefcon)
,	fResult(iOther.fResult)
	{}

SearchResult::~SearchResult()
	{}

SearchResult& SearchResult::operator=(const SearchResult& iOther)
	{
	fRefcon = iOther.fRefcon;
	fResult = iOther.fResult;
	return *this;
	}

SearchResult::SearchResult(int64 iRefcon, const ZRef<QueryEngine::Result>& iResult)
:	fRefcon(iRefcon)
,	fResult(iResult)
	{}

int64 SearchResult::GetRefcon() const
	{ return fRefcon; }

ZRef<QueryEngine::Result> SearchResult::GetResult() const
	{ return fResult; }

// =================================================================================================
#pragma mark - Searcher

Searcher::Searcher()
	{}

Searcher::~Searcher()
	{}

void Searcher::SetCallable_SearcherResultsAvailable(
	ZRef<Callable_SearcherResultsAvailable> iCallable)
	{
	ZAcqMtx acq(fMtx);
	fCalled_SearcherResultsAvailable.Reset();
	fCallable_SearcherResultsAvailable = iCallable;
	}

void Searcher::pCollectResultsCalled()
	{
	ZAcqMtx acq(fMtx);
	fCalled_SearcherResultsAvailable.Reset();
	}

void Searcher::pTriggerSearcherResultsAvailable()
	{
	ZAcqMtx acq(fMtx);
	if (not fCalled_SearcherResultsAvailable())
		{
		if (ZRef<Callable_SearcherResultsAvailable> theCallable =
			fCallable_SearcherResultsAvailable)
			{
			ZRelMtx rel(fMtx);
			theCallable->Call(this);
			}
		}
	}

} // namespace Dataspace
} // namespace ZooLib
